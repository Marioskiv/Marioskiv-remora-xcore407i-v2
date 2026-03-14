#include "hardwareEncoder.h"

#include <cstdio>
#include <cstring>

std::shared_ptr<Module> HardwareEncoder::create(const JsonObject& config, Remora* instance)
{
    const char* comment = config["Comment"] | "Hardware encoder";
    printf("%s\n", comment);

    int joint = config["Joint Number"] | -1;
    const char* pinA = config["Channel A Pin"] | nullptr;
    const char* pinB = config["Channel B Pin"] | nullptr;
    const char* timer = config["Timer"] | nullptr;

    int faultBit = config["Fault Bit"] | -1;
    const char* faultPin = config["Fault Pin"] | nullptr;
    bool faultInvert = config["Fault Invert"] | true;
    bool faultPullup = config["Fault Pullup"] | true;
    bool faultLatch = config["Fault Latch"] | true;
    bool faultClearOnDisable = config["Fault Clear On Joint Disable"] | true;
    uint8_t faultDebounceSamples = config["Fault Debounce Samples"] | 3;

    if (faultDebounceSamples == 0) {
        faultDebounceSamples = 1;
    }

    bool encoderPullup = config["Encoder Pullup"] | false;
    bool allowTimerConflict = config["Allow Timer Conflict"] | false;

    if (joint < 0 || joint >= static_cast<int>(Config::joints)) {
        printf("HardwareEncoder: Invalid Joint Number %d\n", joint);
        return nullptr;
    }

    if (pinA == nullptr || pinB == nullptr) {
        printf("HardwareEncoder: Missing Channel A/B pin definition for joint %d\n", joint);
        return nullptr;
    }

    if (faultPin != nullptr && (faultBit < 0 || faultBit > 31)) {
        printf("HardwareEncoder: Invalid Fault Bit %d for joint %d\n", faultBit, joint);
        return nullptr;
    }

    volatile int32_t* ptrEncoderFeedback = &instance->getTxData()->jointFeedback[joint];
    volatile uint8_t* ptrJointEnable = &instance->getRxData()->jointEnable;
    volatile uint32_t* ptrInputs = &instance->getTxData()->inputs;

    printf("Creating HardwareEncoder for Joint %d, Timer %s, A=%s, B=%s\n",
           joint,
           timer != nullptr ? timer : "auto",
           pinA,
           pinB);

    if (faultPin != nullptr) {
        printf("  Fault input enabled on pin %s -> bit %d\n", faultPin, faultBit);
    }

    return std::make_shared<HardwareEncoder>(
        joint,
        timer,
        pinA,
        pinB,
        *ptrEncoderFeedback,
        *ptrJointEnable,
        *ptrInputs,
        faultBit,
        faultPin,
        faultInvert,
        faultPullup,
        faultLatch,
        faultClearOnDisable,
        faultDebounceSamples,
        encoderPullup,
        allowTimerConflict
    );
}

TIM_TypeDef* HardwareEncoder::timerFromName(const char* name)
{
    if (name == nullptr) return nullptr;

    if (strcmp(name, "TIM1") == 0) return TIM1;
    if (strcmp(name, "TIM2") == 0) return TIM2;
    if (strcmp(name, "TIM3") == 0) return TIM3;
    if (strcmp(name, "TIM4") == 0) return TIM4;
    if (strcmp(name, "TIM5") == 0) return TIM5;
    if (strcmp(name, "TIM8") == 0) return TIM8;
    if (strcmp(name, "TIM9") == 0) return TIM9;
    if (strcmp(name, "TIM12") == 0) return TIM12;

    return nullptr;
}

bool HardwareEncoder::is32BitTimer(TIM_TypeDef* timer)
{
    return (timer == TIM2 || timer == TIM5);
}

void HardwareEncoder::enableTimerClock(TIM_TypeDef* timer)
{
    if (timer == TIM1) __HAL_RCC_TIM1_CLK_ENABLE();
    else if (timer == TIM2) __HAL_RCC_TIM2_CLK_ENABLE();
    else if (timer == TIM3) __HAL_RCC_TIM3_CLK_ENABLE();
    else if (timer == TIM4) __HAL_RCC_TIM4_CLK_ENABLE();
    else if (timer == TIM5) __HAL_RCC_TIM5_CLK_ENABLE();
    else if (timer == TIM8) __HAL_RCC_TIM8_CLK_ENABLE();
    else if (timer == TIM9) __HAL_RCC_TIM9_CLK_ENABLE();
    else if (timer == TIM12) __HAL_RCC_TIM12_CLK_ENABLE();
}

void HardwareEncoder::disableTimerClock(TIM_TypeDef* timer)
{
    if (timer == TIM1) __HAL_RCC_TIM1_CLK_DISABLE();
    else if (timer == TIM2) __HAL_RCC_TIM2_CLK_DISABLE();
    else if (timer == TIM3) __HAL_RCC_TIM3_CLK_DISABLE();
    else if (timer == TIM4) __HAL_RCC_TIM4_CLK_DISABLE();
    else if (timer == TIM5) __HAL_RCC_TIM5_CLK_DISABLE();
    else if (timer == TIM8) __HAL_RCC_TIM8_CLK_DISABLE();
    else if (timer == TIM9) __HAL_RCC_TIM9_CLK_DISABLE();
    else if (timer == TIM12) __HAL_RCC_TIM12_CLK_DISABLE();
}

bool HardwareEncoder::isThreadTimerConflict(TIM_TypeDef* timer)
{
    // Current Remora STM32F4 build uses TIM2/TIM3 for Servo/Base real-time threads.
    return (timer == TIM2 || timer == TIM3);
}

TIM_TypeDef* HardwareEncoder::resolveAndValidateTimer(const char* requestedTimerName,
                                                      const char* chA,
                                                      const char* chB,
                                                      bool allowTimerConflict,
                                                      PinName& pinAName,
                                                      PinName& pinBName,
                                                      uint32_t& pinAFunction,
                                                      uint32_t& pinBFunction)
{
    pinAName = portAndPinToPinName(chA);
    pinBName = portAndPinToPinName(chB);

    if (pinAName == NC || pinBName == NC) {
        printf("HardwareEncoder: Invalid pin names A=%s B=%s\n", chA, chB);
        return nullptr;
    }

    uint32_t timerA = pinmap_peripheral(pinAName, PinMap_PWM);
    uint32_t timerB = pinmap_peripheral(pinBName, PinMap_PWM);

    if (timerA == (uint32_t)NC || timerB == (uint32_t)NC) {
        printf("HardwareEncoder: Pins %s/%s are not valid timer inputs\n", chA, chB);
        return nullptr;
    }

    if (timerA != timerB) {
        printf("HardwareEncoder: A/B pins must belong to same timer. A=%s B=%s\n", chA, chB);
        return nullptr;
    }

    pinAFunction = pinmap_function(pinAName, PinMap_PWM);
    pinBFunction = pinmap_function(pinBName, PinMap_PWM);

    uint32_t chAIndex = STM_PIN_CHANNEL(pinAFunction);
    uint32_t chBIndex = STM_PIN_CHANNEL(pinBFunction);

    if (!((chAIndex == 1 && chBIndex == 2) || (chAIndex == 2 && chBIndex == 1))) {
        printf("HardwareEncoder: Pins must map to timer CH1 and CH2. A ch=%lu B ch=%lu\n", chAIndex, chBIndex);
        return nullptr;
    }

    TIM_TypeDef* resolved = reinterpret_cast<TIM_TypeDef*>(timerA);

    if (requestedTimerName != nullptr) {
        TIM_TypeDef* requested = timerFromName(requestedTimerName);
        if (requested == nullptr) {
            printf("HardwareEncoder: Unsupported timer '%s'. Supported: TIM1,TIM2,TIM3,TIM4,TIM5,TIM8,TIM9,TIM12\n", requestedTimerName);
            return nullptr;
        }

        if (requested != resolved) {
            printf("HardwareEncoder: Timer '%s' does not match selected pins %s/%s\n", requestedTimerName, chA, chB);
            return nullptr;
        }
    }

    if (isThreadTimerConflict(resolved) && !allowTimerConflict) {
        printf("HardwareEncoder: Timer conflict with Remora thread timer. Use TIM2/TIM3 only with 'Allow Timer Conflict': true\n");
        return nullptr;
    }

    return resolved;
}

void HardwareEncoder::resetCounterState()
{
    __HAL_TIM_SET_COUNTER(&timerHandle, 0);
    lastCounterRaw = 0;
    accumulatedCount = 0;
    *ptrEncoderCount = 0;
}

void HardwareEncoder::clearFaultState()
{
    faultDebounceCount = 0;
    faultLatchedState = false;
    lastFaultSample = false;

    if (hasFaultPin && faultMask != 0) {
        *ptrInputs &= static_cast<uint32_t>(~faultMask);
    }
}

HardwareEncoder::HardwareEncoder(int _jointNumber,
                                 const char* _timerName,
                                 const char* _chA,
                                 const char* _chB,
                                 volatile int32_t& _ptrEncoderCount,
                                 volatile uint8_t& _ptrJointEnable,
                                 volatile uint32_t& _ptrInputs,
                                 int _faultBitNumber,
                                 const char* _faultPinName,
                                 bool _faultInvert,
                                 bool _faultPullup,
                                 bool _faultLatch,
                                 bool _faultClearOnDisable,
                                 uint8_t _faultDebounceSamples,
                                 bool _encoderPullup,
                                 bool _allowTimerConflict)
    : Module(),
      jointNumber(_jointNumber),
      timerName(_timerName),
      chA(_chA),
      chB(_chB),
      ptrEncoderCount(&_ptrEncoderCount),
      ptrJointEnable(&_ptrJointEnable),
      ptrInputs(&_ptrInputs),
      faultBitNumber(_faultBitNumber),
      faultMask((_faultBitNumber >= 0 && _faultBitNumber < 32) ? (1 << _faultBitNumber) : 0),
      hasFaultPin(_faultPinName != nullptr),
      faultInvert(_faultInvert),
            faultLatch(_faultLatch),
            faultClearOnDisable(_faultClearOnDisable),
            faultDebounceSamples(_faultDebounceSamples == 0 ? 1 : _faultDebounceSamples),
            faultDebounceCount(0),
            faultLatchedState(false),
            lastFaultSample(false),
      timerInstance(nullptr),
      timerHandle(),
      chAPin(nullptr),
      chBPin(nullptr),
      faultPin(nullptr),
      lastCounterRaw(0),
      accumulatedCount(0),
      isEnabled(false)
{
    PinName pinAName = NC;
    PinName pinBName = NC;
    uint32_t pinAFunction = 0;
    uint32_t pinBFunction = 0;

    timerInstance = resolveAndValidateTimer(_timerName,
                                            _chA,
                                            _chB,
                                            _allowTimerConflict,
                                            pinAName,
                                            pinBName,
                                            pinAFunction,
                                            pinBFunction);

    if (timerInstance == nullptr) {
        Error_Handler();
        return;
    }

    const uint32_t encoderPull = _encoderPullup ? GPIO_PULLUP : GPIO_NOPULL;

    chAPin.reset(createPinFromPinMap(_chA, pinAName, PinMap_PWM, GPIO_MODE_AF_PP, encoderPull, GPIO_SPEED_FREQ_LOW));
    chBPin.reset(createPinFromPinMap(_chB, pinBName, PinMap_PWM, GPIO_MODE_AF_PP, encoderPull, GPIO_SPEED_FREQ_LOW));

    if (!chAPin || !chBPin) {
        printf("HardwareEncoder: Failed to configure A/B pins\n");
        Error_Handler();
        return;
    }

    if (hasFaultPin) {
        faultPin = std::make_unique<Pin>(_faultPinName, INPUT, _faultPullup ? PULLUP : PULLNONE);
    }

    enableTimerClock(timerInstance);

    timerHandle.Instance = timerInstance;
    timerHandle.Init.Prescaler = 0;
    timerHandle.Init.CounterMode = TIM_COUNTERMODE_UP;
    timerHandle.Init.Period = is32BitTimer(timerInstance) ? 0xFFFFFFFF : 0xFFFF;
    timerHandle.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    timerHandle.Init.RepetitionCounter = 0;
    timerHandle.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;

    TIM_Encoder_InitTypeDef encoderCfg = {0};
    encoderCfg.EncoderMode = TIM_ENCODERMODE_TI12;
    encoderCfg.IC1Polarity = TIM_ICPOLARITY_RISING;
    encoderCfg.IC1Selection = TIM_ICSELECTION_DIRECTTI;
    encoderCfg.IC1Prescaler = TIM_ICPSC_DIV1;
    encoderCfg.IC1Filter = 0;
    encoderCfg.IC2Polarity = TIM_ICPOLARITY_RISING;
    encoderCfg.IC2Selection = TIM_ICSELECTION_DIRECTTI;
    encoderCfg.IC2Prescaler = TIM_ICPSC_DIV1;
    encoderCfg.IC2Filter = 0;

    if (HAL_TIM_Encoder_Init(&timerHandle, &encoderCfg) != HAL_OK) {
        printf("HardwareEncoder: HAL_TIM_Encoder_Init failed\n");
        Error_Handler();
        return;
    }

    TIM_MasterConfigTypeDef masterCfg = {0};
    masterCfg.MasterOutputTrigger = TIM_TRGO_RESET;
    masterCfg.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;

    if (HAL_TIMEx_MasterConfigSynchronization(&timerHandle, &masterCfg) != HAL_OK) {
        printf("HardwareEncoder: HAL_TIMEx_MasterConfigSynchronization failed\n");
        Error_Handler();
        return;
    }

    if (HAL_TIM_Encoder_Start(&timerHandle, TIM_CHANNEL_ALL) != HAL_OK) {
        printf("HardwareEncoder: HAL_TIM_Encoder_Start failed\n");
        Error_Handler();
        return;
    }

    resetCounterState();
    clearFaultState();

    printf("  HardwareEncoder created for joint %d using timer %s\n", jointNumber, _timerName ? _timerName : "auto");
}

HardwareEncoder::~HardwareEncoder()
{
    if (timerHandle.Instance != nullptr) {
        HAL_TIM_Encoder_Stop(&timerHandle, TIM_CHANNEL_ALL);
        HAL_TIM_Encoder_DeInit(&timerHandle);
        disableTimerClock(timerHandle.Instance);
    }
}

void HardwareEncoder::update()
{
    int mask = 1 << jointNumber;
    isEnabled = ((*ptrJointEnable) & mask) != 0;

    if (!isEnabled) {
        resetCounterState();
        if (faultClearOnDisable) {
            clearFaultState();
        }
    } else {
        uint32_t raw = __HAL_TIM_GET_COUNTER(&timerHandle);

        if (is32BitTimer(timerInstance)) {
            int32_t delta = static_cast<int32_t>(raw - lastCounterRaw);
            accumulatedCount += delta;
        } else {
            uint16_t raw16 = static_cast<uint16_t>(raw);
            uint16_t last16 = static_cast<uint16_t>(lastCounterRaw);
            int16_t delta = static_cast<int16_t>(raw16 - last16);
            accumulatedCount += delta;
        }

        lastCounterRaw = raw;
        *ptrEncoderCount = accumulatedCount;
    }

    if (hasFaultPin && faultMask != 0) {
        bool rawFault = faultPin->get();
        bool faultSample = faultInvert ? !rawFault : rawFault;

        if (faultSample == lastFaultSample) {
            if (faultDebounceCount < faultDebounceSamples) {
                faultDebounceCount++;
            }
        } else {
            faultDebounceCount = 1;
            lastFaultSample = faultSample;
        }

        bool faultActive = false;
        if (faultDebounceCount >= faultDebounceSamples) {
            faultActive = lastFaultSample;
        }

        if (faultLatch) {
            if (faultActive) {
                faultLatchedState = true;
            }
            faultActive = faultLatchedState;
        }

        if (faultActive) {
            *ptrInputs |= static_cast<uint32_t>(faultMask);
        } else {
            *ptrInputs &= static_cast<uint32_t>(~faultMask);
        }
    }
}

void HardwareEncoder::slowUpdate()
{
}
