#ifndef ENDSTOP_H
#define ENDSTOP_H

#include <cstdint>

#include "../../remora.h"
#include "../../modules/module.h"
#include "../../../remora-hal/pin/pin.h"

/**
 * @class Endstop
 * @brief Endstop/limit switch module for axis homing and limits.
 *
 * The Endstop class provides limit switch reading functionality for
 * axis homing and safety limits. Supports configurable inversion and
 * pullup/pulldown modes.
 */
class Endstop : public Module
{
private:

	int bitNumber;                 				/**< Bit position in inputs word */
	const char* pin;               				/**< Pin name for endstop */
	bool invert;                   				/**< Invert input logic */
	bool pullup;                   				/**< Enable pullup resistor */

	volatile uint32_t* ptrInputs; 				/**< Pointer to inputs data */
	int mask;                      				/**< Bit mask for this input */

	Pin endstopPin;                				/**< Pin object for endstop */
	bool state;                    				/**< Current switch state */
	bool lastState;                				/**< Previous switch state for debouncing */
	uint8_t debounceCount;         				/**< Debounce counter */

public:

	Endstop(int _bitNumber, const char* _pin, bool _invert, bool _pullup, volatile uint32_t &_ptrInputs);
	static std::shared_ptr<Module> create(const JsonObject& config, Remora* instance);

	void update(void) override;
	void slowUpdate(void) override;

};

#endif // ENDSTOP_H
