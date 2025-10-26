#include "alarm.h"

/***********************************************************************
                MODULE CONFIGURATION AND CREATION FROM JSON     
************************************************************************/

std::shared_ptr<Module> Alarm::create(const JsonObject& config, Remora* instance)
{
    const char* comment = config["Comment"];
    printf("%s\n", comment);

    int joint = config["Joint Number"];
    int bitNum = config["Data Bit"];
    const char* pinName = config["Pin"];
    bool invert = config["Invert"] | false;
    bool pullup = config["Pullup"] | true;

    printf("Creating Alarm for Joint %d at pin %s, bit %d\n", joint, pinName, bitNum);

    // Get pointer to inputs
    volatile uint32_t* ptrInputs = &instance->getTxData()->inputs;

    return std::make_shared<Alarm>(
        joint,
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

Alarm::Alarm(int _jointNumber, int _bitNumber, const char* _pin, bool _invert, bool _pullup, volatile uint32_t &_ptrInputs) :
    Module(),
    jointNumber(_jointNumber),
    bitNumber(_bitNumber),
    pin(_pin),
    invert(_invert),
    pullup(_pullup),
    ptrInputs(&_ptrInputs),
    mask(1 << _bitNumber),
    alarmPin(_pin, INPUT, _pullup ? PULLUP : PULLNONE),
    alarmState(false),
    lastAlarmState(false),
    debounceCount(0),
    alarmCount(0)
{
    printf("  Alarm created for joint %d at bit %d, pin %s\n", this->jointNumber, this->bitNumber, this->pin);
}

void Alarm::update()
{
    // Read the pin state
    bool rawState = this->alarmPin.get();
    
    // Apply inversion if configured
    // Typically alarm signals are active-low, so invert=true means alarm when pin is low
    bool currentAlarm = this->invert ? !rawState : rawState;

    // Debouncing - require 3 consistent readings to change state
    if (currentAlarm == this->lastAlarmState)
    {
        if (this->debounceCount < 3)
        {
            this->debounceCount++;
        }
        else
        {
            // State confirmed after debouncing
            if (currentAlarm && !this->alarmState)
            {
                // Rising edge - alarm triggered
                this->alarmCount++;
                printf("ALARM: Joint %d alarm triggered! (Count: %lu)\n", this->jointNumber, this->alarmCount);
            }
            this->alarmState = currentAlarm;
        }
    }
    else
    {
        this->debounceCount = 0;
        this->lastAlarmState = currentAlarm;
    }

    // Update the inputs word
    if (this->alarmState)
    {
        *(this->ptrInputs) |= this->mask;   // Set alarm bit high
    }
    else
    {
        *(this->ptrInputs) &= ~this->mask;  // Clear alarm bit
    }
}

void Alarm::slowUpdate()
{
    // Could log alarm status periodically
    if (this->alarmState)
    {
        printf("Joint %d in ALARM state\n", this->jointNumber);
    }
}
