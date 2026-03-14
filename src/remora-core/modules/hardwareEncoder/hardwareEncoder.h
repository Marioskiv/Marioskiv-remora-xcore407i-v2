#ifndef HARDWARE_ENCODER_H
#define HARDWARE_ENCODER_H

#include <cstdint>
#include <memory>

#include "../../remora.h"
#include "../../modules/module.h"
#include "../../../remora-hal/hal_utils.h"

class HardwareEncoder : public Module
{
private:
    int jointNumber;
    const char* timerName;
    const char* chA;
    const char* chB;

    volatile int32_t* ptrEncoderCount;
    volatile uint8_t* ptrJointEnable;
    volatile uint32_t* ptrInputs;

    int faultBitNumber;
    int faultMask;
    bool hasFaultPin;
    bool faultInvert;
    bool faultLatch;
    bool faultClearOnDisable;
    uint8_t faultDebounceSamples;
    uint8_t faultDebounceCount;
    bool faultLatchedState;
    bool lastFaultSample;

    TIM_TypeDef* timerInstance;
    TIM_HandleTypeDef timerHandle;

    std::unique_ptr<Pin> chAPin;
    std::unique_ptr<Pin> chBPin;
    std::unique_ptr<Pin> faultPin;

    uint32_t lastCounterRaw;
    int32_t accumulatedCount;

    bool isEnabled;

    static TIM_TypeDef* timerFromName(const char* name);
    static bool is32BitTimer(TIM_TypeDef* timer);

    static void enableTimerClock(TIM_TypeDef* timer);
    static void disableTimerClock(TIM_TypeDef* timer);

    static bool isThreadTimerConflict(TIM_TypeDef* timer);

    static TIM_TypeDef* resolveAndValidateTimer(const char* timerName,
                                                const char* chA,
                                                const char* chB,
                                                bool allowTimerConflict,
                                                PinName& pinAName,
                                                PinName& pinBName,
                                                uint32_t& pinAFunction,
                                                uint32_t& pinBFunction);

    void resetCounterState();
    void clearFaultState();

public:
    HardwareEncoder(int jointNumber,
                    const char* timerName,
                    const char* chA,
                    const char* chB,
                    volatile int32_t& ptrEncoderCount,
                    volatile uint8_t& ptrJointEnable,
                    volatile uint32_t& ptrInputs,
                    int faultBitNumber,
                    const char* faultPinName,
                    bool faultInvert,
                    bool faultPullup,
                    bool faultLatch,
                    bool faultClearOnDisable,
                    uint8_t faultDebounceSamples,
                    bool encoderPullup,
                    bool allowTimerConflict);

    ~HardwareEncoder() override;

    static std::shared_ptr<Module> create(const JsonObject& config, Remora* instance);

    void update() override;
    void slowUpdate() override;
};

#endif // HARDWARE_ENCODER_H
