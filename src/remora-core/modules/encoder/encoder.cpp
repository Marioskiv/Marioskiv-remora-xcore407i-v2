#include "encoder.h"

/***********************************************************************
                MODULE CONFIGURATION AND CREATION FROM JSON     
************************************************************************/

std::shared_ptr<Module> Encoder::create(const JsonObject& config, Remora* instance)
{
    const char* comment = config["Comment"];
    printf("%s\n", comment);

    int joint = config["Joint Number"];
    const char* pinA = config["Channel A Pin"];
    const char* pinB = config["Channel B Pin"];
    const char* pinI = config["Index Pin"] | nullptr;

    // Get pointers to data structures
    volatile int32_t* ptrEncoderFeedback = &instance->getTxData()->jointFeedback[joint];
    volatile uint8_t* ptrJointEnable = &instance->getRxData()->jointEnable;

    if (pinI == nullptr)
    {
        printf("Creating Encoder for Joint %d at pins %s and %s (no index)\n", joint, pinA, pinB);
        return std::make_shared<Encoder>(
            Config::pruBaseFreq,
            joint,
            pinA,
            pinB,
            *ptrEncoderFeedback,
            *ptrJointEnable
        );
    }
    else
    {
        printf("Creating Encoder for Joint %d at pins %s, %s with index %s\n", joint, pinA, pinB, pinI);
        return std::make_shared<Encoder>(
            Config::pruBaseFreq,
            joint,
            pinA,
            pinB,
            pinI,
            *ptrEncoderFeedback,
            *ptrJointEnable
        );
    }
}

/***********************************************************************
*                METHOD DEFINITIONS                                    *
************************************************************************/

// Constructor without index
Encoder::Encoder(int32_t _threadFreq, int _jointNumber, const char* _chA, const char* _chB, 
                 volatile int32_t &_ptrEncoderCount, volatile uint8_t &_ptrJointEnable) :
    Module(),
    jointNumber(_jointNumber),
    chA(_chA),
    chB(_chB),
    index(nullptr),
    hasIndex(false),
    ptrEncoderCount(&_ptrEncoderCount),
    ptrJointEnable(&_ptrJointEnable),
    chAPin(_chA, INPUT, PULLUP),
    chBPin(_chB, INPUT, PULLUP),
    indexPin("NC", INPUT, PULLNONE),  // Dummy pin when no index
    count(0),
    indexCount(0),
    state(0),
    indexPulse(0),
    pulseCount(0),
    isEnabled(false)
{
    printf("  Encoder created for joint %d (no index)\n", this->jointNumber);
}

// Constructor with index
Encoder::Encoder(int32_t _threadFreq, int _jointNumber, const char* _chA, const char* _chB, const char* _index,
                 volatile int32_t &_ptrEncoderCount, volatile uint8_t &_ptrJointEnable) :
    Module(),
    jointNumber(_jointNumber),
    chA(_chA),
    chB(_chB),
    index(_index),
    hasIndex(true),
    ptrEncoderCount(&_ptrEncoderCount),
    ptrJointEnable(&_ptrJointEnable),
    chAPin(_chA, INPUT, PULLUP),
    chBPin(_chB, INPUT, PULLUP),
    indexPin(_index, INPUT, PULLUP),
    count(0),
    indexCount(0),
    state(0),
    indexPulse((_threadFreq / Config::pruServoFreq) * 3),  // Output index pulse for 3 servo cycles
    pulseCount(0),
    isEnabled(false)
{
    printf("  Encoder created for joint %d with index\n", this->jointNumber);
}

void Encoder::update()
{
    // Check if joint is enabled
    int mask = 1 << this->jointNumber;
    this->isEnabled = (*(this->ptrJointEnable) & mask) != 0;

    if (!this->isEnabled)
    {
        // Joint disabled, reset count
        this->count = 0;
        *(this->ptrEncoderCount) = 0;
        return;
    }

    // Read quadrature encoder state
    uint8_t s = this->state & 3;

    if (this->chAPin.get()) s |= 4;
    if (this->chBPin.get()) s |= 8;

    // Quadrature decoding state machine
    switch (s) {
        case 0: case 5: case 10: case 15:
            break;  // No change
        case 1: case 7: case 8: case 14:
            count++; 
            break;  // Forward
        case 2: case 4: case 11: case 13:
            count--; 
            break;  // Backward
        case 3: case 12:
            count += 2; 
            break;  // Fast forward
        default:
            count -= 2; 
            break;  // Fast backward
    }

    this->state = (s >> 2);

    // Handle index pulse if present
    if (this->hasIndex)
    {
        if (this->indexPin.get() && (this->pulseCount == 0))    // Rising edge on index
        {
            this->indexCount = this->count;                     // Capture count at index
            *(this->ptrEncoderCount) = this->indexCount;
            this->pulseCount = this->indexPulse;                // Start index pulse output
        }
        else if (this->pulseCount > 0)                          // Maintain index count during pulse
        {
            this->pulseCount--;
        }
        else
        {
            *(this->ptrEncoderCount) = this->count;             // Normal operation
        }
    }
    else
    {
        *(this->ptrEncoderCount) = this->count;                 // Update encoder count
    }
}

void Encoder::slowUpdate()
{
    // Nothing to do in slow update for now
    // Could add diagnostics or error checking here
}

void Encoder::setEnabled(bool state)
{
    this->isEnabled = state;
    if (!state)
    {
        this->count = 0;
        *(this->ptrEncoderCount) = 0;
    }
}

// Quadrature Encoder State Diagram
// Credit to: https://github.com/PaulStoffregen/Encoder
//
//                           _______         _______       
//               PinA ______|       |_______|       |______ PinA
// negative <---         _______         _______         __      --> positive
//               PinB __|       |_______|       |_______|   PinB
//
//	new	new	old	old
//	pinB	pinA	pinB	pinA	Result
//	----	----	----	----	------
//	0	0	0	0	no movement
//	0	0	0	1	+1
//	0	0	1	0	-1
//	0	0	1	1	+2  (assume pinA edges only)
//	0	1	0	0	-1
//	0	1	0	1	no movement
//	0	1	1	0	-2  (assume pinA edges only)
//	0	1	1	1	+1
//	1	0	0	0	+1
//	1	0	0	1	-2  (assume pinA edges only)
//	1	0	1	0	no movement
//	1	0	1	1	-1
//	1	1	0	0	+2  (assume pinA edges only)
//	1	1	0	1	-1
//	1	1	1	0	+1
//	1	1	1	1	no movement
