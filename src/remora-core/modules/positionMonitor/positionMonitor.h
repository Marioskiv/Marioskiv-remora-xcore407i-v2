#ifndef POSITION_MONITOR_H
#define POSITION_MONITOR_H

#include <cstdint>
#include <cmath>

#include "../../remora.h"
#include "../../remoraStatus.h"
#include "../../modules/module.h"

/**
 * @class PositionMonitor
 * @brief Real-time position error monitoring for closed-loop control.
 *
 * Compares commanded position (from stepgen) with actual position (from encoder)
 * to detect:
 * - Step loss (missed steps)
 * - Position deviation / following error
 * - Encoder mismatch
 * 
 * When error exceeds threshold, sets fault bit and can trigger safety halt.
 * Compatible with Ethernet communication to LinuxCNC.
 */
class PositionMonitor : public Module
{
private:

	int jointNumber;               				/**< Joint number this monitor applies to */
	
	// Pointers to data sources
	volatile int32_t* ptrStepgenCount; 			/**< Pointer to stepgen rawCount */
	volatile int32_t* ptrEncoderCount; 			/**< Pointer to encoder count */
	volatile uint32_t* ptrInputs;				/**< Pointer to inputs (fault bits) */
	volatile float* ptrProcessVariable;			/**< Pointer to report current error */
	Remora* remoraInstance;        				/**< Pointer to Remora instance for status */
	
	// Scaling factors
	float stepsPerUnit;            				/**< Steps per mm/inch/degree */
	float countsPerUnit;           				/**< Encoder counts per mm/inch/degree */
	
	// Error thresholds
	float errorThreshold;          				/**< Maximum allowed position error (units) */
	float velocityThreshold;       				/**< Maximum allowed velocity error (units/sec) */
	int debounceCycles;            				/**< Cycles before triggering fault */
	
	// State tracking
	int32_t lastStepCount;         				/**< Previous stepgen count */
	int32_t lastEncCount;          				/**< Previous encoder count */
	float currentError;            				/**< Current position error (units) */
	float currentVelError;         				/**< Current velocity error (units/s) */
	int errorCount;                				/**< Consecutive cycles above threshold */
	bool faultActive;              				/**< Fault state */
	int faultBit;                  				/**< Bit position in inputs word */
	
	// Statistics
	uint32_t faultTriggerCount;    				/**< Total fault events */
	float maxErrorSeen;            				/**< Peak error recorded */

public:

	PositionMonitor(int _jointNumber, 
	                float _stepsPerUnit, 
	                float _countsPerUnit,
	                float _errorThreshold,
	                int _debounceCycles,
	                int _faultBit,
	                volatile int32_t &_ptrStepgenCount,
	                volatile int32_t &_ptrEncoderCount,
	                volatile uint32_t &_ptrInputs,
	                volatile float &_ptrProcessVariable,
	                Remora* _remoraInstance);
	
	static std::shared_ptr<Module> create(const JsonObject& config, Remora* instance);

	void update(void) override;
	void slowUpdate(void) override;
	
	// Accessors
	bool isFaulted() const { return faultActive; }
	float getCurrentError() const { return currentError; }
	float getMaxError() const { return maxErrorSeen; }
	uint32_t getFaultCount() const { return faultTriggerCount; }
	
	// Reset fault (for external control)
	void clearFault();

};

#endif // POSITION_MONITOR_H
