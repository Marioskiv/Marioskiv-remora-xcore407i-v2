#ifndef ALARM_H
#define ALARM_H

#include <cstdint>

#include "../../remora.h"
#include "../../modules/module.h"
#include "../../../remora-hal/pin/pin.h"

/**
 * @class Alarm
 * @brief Alarm input module for closed-loop servo motor errors.
 *
 * The Alarm class monitors alarm/error signals from closed-loop servo motors
 * (e.g., encoder error, overcurrent, position error, etc.) and reports them
 * to LinuxCNC through the inputs word.
 */
class Alarm : public Module
{
private:

	int jointNumber;               				/**< Joint number this alarm is associated with */
	int bitNumber;                 				/**< Bit position in inputs word */
	const char* pin;               				/**< Pin name for alarm input */
	bool invert;                   				/**< Invert input logic */
	bool pullup;                   				/**< Enable pullup resistor */

	volatile uint32_t* ptrInputs; 				/**< Pointer to inputs data */
	int mask;                      				/**< Bit mask for this input */

	Pin alarmPin;                  				/**< Pin object for alarm */
	bool alarmState;               				/**< Current alarm state */
	bool lastAlarmState;           				/**< Previous alarm state */
	uint8_t debounceCount;         				/**< Debounce counter */
	uint32_t alarmCount;           				/**< Count of alarm activations */

public:

	Alarm(int _jointNumber, int _bitNumber, const char* _pin, bool _invert, bool _pullup, volatile uint32_t &_ptrInputs);
	static std::shared_ptr<Module> create(const JsonObject& config, Remora* instance);

	void update(void) override;
	void slowUpdate(void) override;
	bool isAlarming() const { return alarmState; }
	uint32_t getAlarmCount() const { return alarmCount; }

};

#endif // ALARM_H
