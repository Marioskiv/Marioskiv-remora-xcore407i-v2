#include "endstop.h"

/***********************************************************************
                MODULE CONFIGURATION AND CREATION FROM JSON     
************************************************************************/

std::shared_ptr<Module> Endstop::create(const JsonObject& config, Remora* instance)
{
    const char* comment = config["Comment"];
    printf("%s\n", comment);

    int bitNum = config["Data Bit"];
    const char* pinName = config["Pin"];
    bool invert = config["Invert"] | false;
    bool pullup = config["Pullup"] | true;

    printf("Creating Endstop at pin %s, bit %d\n", pinName, bitNum);

    // Get pointer to inputs
    volatile uint32_t* ptrInputs = &instance->getTxData()->inputs;

    return std::make_shared<Endstop>(
        bitNum,
        pinName,
        invert,
        pullup,
        *ptrInputs
    );
}

/***********************************************************************
*                METHOD DEFINITIONS                                    *
************************************************************************/

Endstop::Endstop(int _bitNumber, const char* _pin, bool _invert, bool _pullup, volatile uint32_t &_ptrInputs) :
    Module(),
    bitNumber(_bitNumber),
    pin(_pin),
    invert(_invert),
    pullup(_pullup),
    ptrInputs(&_ptrInputs),
    mask(1 << _bitNumber),
    endstopPin(_pin, INPUT, _pullup ? PULLUP : PULLNONE),
    state(false),
    lastState(false),
    debounceCount(0)
{
    printf("  Endstop created at bit %d, pin %s\n", this->bitNumber, this->pin);
}

void Endstop::update()
{
    // Read the pin state
    bool rawState = this->endstopPin.get();
    
    // Apply inversion if configured
    bool currentState = this->invert ? !rawState : rawState;

    // Simple debouncing - require 3 consistent readings
    if (currentState == this->lastState)
    {
        if (this->debounceCount < 3)
        {
            this->debounceCount++;
        }
        else
        {
            this->state = currentState;
        }
    }
    else
    {
        this->debounceCount = 0;
        this->lastState = currentState;
    }

    // Update the inputs word
    if (this->state)
    {
        *(this->ptrInputs) |= this->mask;   // Set bit high
    }
    else
    {
        *(this->ptrInputs) &= ~this->mask;  // Set bit low
    }
}

void Endstop::slowUpdate()
{
    // Nothing to do in slow update
    // Could add diagnostics here if needed
}
