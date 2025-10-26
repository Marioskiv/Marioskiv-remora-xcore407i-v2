#include "positionMonitor.h"
#include "../../internalState.h"

/***********************************************************************
                MODULE CONFIGURATION AND CREATION FROM JSON     
************************************************************************/

std::shared_ptr<Module> PositionMonitor::create(const JsonObject& config, Remora* instance)
{
    const char* comment = config["Comment"];
    printf("%s\n", comment);

    int joint = config["Joint Number"];
    float stepsPerUnit = config["Steps Per Unit"] | 1000.0f;
    float countsPerUnit = config["Counts Per Unit"] | 1000.0f;
    float errorThreshold = config["Error Threshold"] | 1.0f;  // Units (mm/deg)
    int debounceCycles = config["Debounce Cycles"] | 10;
    int pvSlot = config["PV Slot"] | joint;  // Use joint number as PV slot by default
    
    // Fault bit: use joint number offset (e.g., bits 8-15 for joints 0-7)
    int faultBit = 8 + joint;

    printf("Creating Position Monitor for Joint %d\n", joint);
    printf("  Steps/Unit: %.2f, Counts/Unit: %.2f\n", stepsPerUnit, countsPerUnit);
    printf("  Error Threshold: %.3f units, Debounce: %d cycles\n", errorThreshold, debounceCycles);

    // Get pointers to data structures
    // Stepgen writes to jointStepCount, Encoder writes to jointFeedback
    // Stepgen commanded count from internal state (not part of wire protocol)
    extern InternalState gInternalState;
    volatile int32_t* ptrStepgenCount = &gInternalState.jointStepCount[joint];
    volatile int32_t* ptrEncoderCount = &instance->getTxData()->jointFeedback[joint];
    volatile uint32_t* ptrInputs = &instance->getTxData()->inputs;
    volatile float* ptrPV = &instance->getTxData()->processVariable[pvSlot];

    return std::make_shared<PositionMonitor>(
        joint,
        stepsPerUnit,
        countsPerUnit,
        errorThreshold,
        debounceCycles,
        faultBit,
        *ptrStepgenCount,
        *ptrEncoderCount,
        *ptrInputs,
        *ptrPV,
        instance
    );
}

/***********************************************************************
*                METHOD DEFINITIONS                                    *
************************************************************************/

PositionMonitor::PositionMonitor(int _jointNumber, 
                                 float _stepsPerUnit, 
                                 float _countsPerUnit,
                                 float _errorThreshold,
                                 int _debounceCycles,
                                 int _faultBit,
                                 volatile int32_t &_ptrStepgenCount,
                                 volatile int32_t &_ptrEncoderCount,
                                 volatile uint32_t &_ptrInputs,
                                 volatile float &_ptrProcessVariable,
                                 Remora* _remoraInstance) :
    Module(Config::pruServoFreq, Config::pruServoFreq),  // Run at servo thread frequency
    jointNumber(_jointNumber),
    ptrStepgenCount(&_ptrStepgenCount),
    ptrEncoderCount(&_ptrEncoderCount),
    ptrInputs(&_ptrInputs),
    ptrProcessVariable(&_ptrProcessVariable),
    remoraInstance(_remoraInstance),
    stepsPerUnit(_stepsPerUnit),
    countsPerUnit(_countsPerUnit),
    errorThreshold(_errorThreshold),
    velocityThreshold(_errorThreshold * 10.0f),  // 10x position threshold
    debounceCycles(_debounceCycles),
    lastStepCount(0),
    lastEncCount(0),
    currentError(0.0f),
    currentVelError(0.0f),
    errorCount(0),
    faultActive(false),
    faultBit(_faultBit),
    faultTriggerCount(0),
    maxErrorSeen(0.0f)
{
    printf("  Position Monitor created for joint %d at fault bit %d\n", 
           this->jointNumber, this->faultBit);
}

void PositionMonitor::update()
{
    // Get current counts
    int32_t stepCount = *(this->ptrStepgenCount);
    int32_t encCount = *(this->ptrEncoderCount);
    
    // Convert to real units
    float stepPos = static_cast<float>(stepCount) / this->stepsPerUnit;
    float encPos = static_cast<float>(encCount) / this->countsPerUnit;
    
    // Calculate position error (commanded - actual)
    this->currentError = stepPos - encPos;
    float absError = std::fabs(this->currentError);
    
    // Calculate velocity error (for diagnostic)
    int32_t stepDelta = stepCount - this->lastStepCount;
    int32_t encDelta = encCount - this->lastEncCount;
    float stepVel = static_cast<float>(stepDelta) / this->stepsPerUnit * Config::pruServoFreq;
    float encVel = static_cast<float>(encDelta) / this->countsPerUnit * Config::pruServoFreq;
    this->currentVelError = stepVel - encVel;
    
    // Update history
    this->lastStepCount = stepCount;
    this->lastEncCount = encCount;
    
    // Track peak error
    if (absError > this->maxErrorSeen)
    {
        this->maxErrorSeen = absError;
    }
    
    // Report current error via process variable
    *(this->ptrProcessVariable) = this->currentError;
    
    // Fault detection with debouncing
    if (absError > this->errorThreshold)
    {
        this->errorCount++;
        
        if (this->errorCount >= this->debounceCycles && !this->faultActive)
        {
            // Trigger fault
            this->faultActive = true;
            this->faultTriggerCount++;
            
            printf("POSITION FAULT: Joint %d error %.4f exceeds threshold %.4f\n",
                   this->jointNumber, absError, this->errorThreshold);
            printf("  Step pos: %.4f, Encoder pos: %.4f\n", stepPos, encPos);
            printf("  Fault count: %lu\n", this->faultTriggerCount);
            
            // Set fault bit in inputs
            *(this->ptrInputs) |= (1 << this->faultBit);
            
            // Set Remora status (FATAL if error is extreme)
            bool isFatal = (absError > (this->errorThreshold * 3.0f));
            uint8_t status = makeRemoraStatus(
                RemoraErrorSource::POSITION_MON,
                RemoraErrorCode::POSITION_ERROR_EXCEEDED,
                isFatal
            );
            this->remoraInstance->setStatus(status);
            
            if (isFatal)
            {
                printf("  FATAL ERROR - System halt recommended!\n");
            }
        }
    }
    else
    {
        // Error within threshold
        this->errorCount = 0;
        
        // Auto-clear fault if error drops (optional - comment out for latched faults)
        if (this->faultActive && absError < (this->errorThreshold * 0.75f))
        {
            printf("Position Monitor: Joint %d fault cleared (error now %.4f)\n", 
                   this->jointNumber, absError);
            this->clearFault();
        }
    }
}

void PositionMonitor::slowUpdate()
{
    // Periodic diagnostics (runs at servo frequency)
    if (this->faultActive)
    {
        printf("Joint %d FAULT ACTIVE - Error: %.4f, Max: %.4f, Count: %lu\n",
               this->jointNumber, this->currentError, this->maxErrorSeen, this->faultTriggerCount);
    }
    
    // Velocity error warning (not fault, just diagnostic)
    if (std::fabs(this->currentVelError) > this->velocityThreshold)
    {
        printf("Joint %d velocity error: %.2f units/s\n", 
               this->jointNumber, this->currentVelError);
    }
}

void PositionMonitor::clearFault()
{
    this->faultActive = false;
    this->errorCount = 0;
    *(this->ptrInputs) &= ~(1 << this->faultBit);
}

// Implementation notes:
// 
// This module runs in the SERVO thread (1 kHz typically) to monitor position error.
// It compares:
//   - Commanded position (from stepgen rawCount)
//   - Actual position (from encoder count)
// 
// Fault conditions:
//   - Position error > threshold for N consecutive cycles
//   - Fault bit set in txData.inputs
//   - LinuxCNC sees this via Ethernet and can trigger E-stop
// 
// Integration:
//   - Load AFTER both Stepgen and Encoder modules
//   - Requires separate storage for stepgen vs encoder counts
//   - Future: extend data.h to include separate arrays
