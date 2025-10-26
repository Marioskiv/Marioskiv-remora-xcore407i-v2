#ifndef ENCODER_H
#define ENCODER_H

#include <cstdint>

#include "../../remora.h"
#include "../../modules/module.h"
#include "../../../remora-hal/pin/pin.h"

/**
 * @class Encoder
 * @brief Quadrature encoder module for position feedback in closed-loop control.
 *
 * The Encoder class provides quadrature encoder reading functionality for
 * closed-loop motor control. It supports standard A/B channels and optional
 * index pulse for absolute position reference.
 */
class Encoder : public Module
{
private:

	int jointNumber;               				/**< LinuxCNC joint number */
	const char* chA;               				/**< Channel A pin name */
	const char* chB;               				/**< Channel B pin name */
	const char* index;             				/**< Index pin name (optional) */
	bool hasIndex;                 				/**< Flag indicating if index pin is present */

	volatile int32_t* ptrEncoderCount; 			/**< Pointer to encoder count feedback data */
	volatile uint8_t* ptrJointEnable; 			/**< Pointer for joint enable data */

	Pin chAPin, chBPin, indexPin; 				/**< Pins for encoder channels */

	int32_t count;                 				/**< Current encoder count */
	int32_t indexCount;            				/**< Count value at index pulse */
	uint8_t state;                 				/**< Current quadrature state */
	int8_t indexPulse;             				/**< Number of cycles to output index pulse */
	int8_t pulseCount;             				/**< Counter for index pulse output */

	bool isEnabled;                				/**< Flag indicating if encoder is enabled */

public:

	Encoder(int32_t _threadFreq, int _jointNumber, const char* _chA, const char* _chB, volatile int32_t &_ptrEncoderCount, volatile uint8_t &_ptrJointEnable);
	Encoder(int32_t _threadFreq, int _jointNumber, const char* _chA, const char* _chB, const char* _index, volatile int32_t &_ptrEncoderCount, volatile uint8_t &_ptrJointEnable);
	static std::shared_ptr<Module> create(const JsonObject& config, Remora* instance);

	void update(void) override;
	void slowUpdate(void) override;
	void setEnabled(bool state);

};

#endif // ENCODER_H
