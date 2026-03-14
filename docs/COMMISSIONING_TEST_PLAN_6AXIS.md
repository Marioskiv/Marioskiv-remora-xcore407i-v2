# Commissioning Test Plan: 6-Axis HardwareEncoder

This test plan validates firmware, feedback integrity, and safety chain behavior for 6-axis deployments.

## Preconditions

- Firmware flashed with `HardwareEncoder` JSON config
- LinuxCNC launched using `linuxcnc/xcore407i_6axis_hardware_encoder.hal`
- Machine can be safely jogged at low speed
- Emergency stop path is physically wired and verified

## Test Matrix

Perform all tests for joints 0..5.

## 1. Power-On Sanity

1. Verify board boots and LinuxCNC connects.
2. Confirm all feedback pins are readable.
3. Confirm no fault bit is asserted at idle.

Pass criteria:

- No communication errors
- No unexpected estop trip

## 2. Encoder Direction and Scale

1. Enable one joint.
2. Jog positive slowly.
3. Check `joint.N.motor-pos-fb` sign and monotonicity.
4. Jog negative slowly and confirm inverse direction.

Pass criteria:

- Feedback sign matches commanded direction
- No oscillating or stuck counts

## 3. Wrap Continuity (16-bit timers)

1. Select a joint mapped to 16-bit timer.
2. Jog continuously through enough counts to cross hardware wrap.
3. Observe host feedback trend.

Pass criteria:

- No discontinuity spikes around wrap
- Position remains continuous

## 4. High-Speed Count Integrity

1. Command progressively higher jog velocity.
2. Compare expected and observed displacement over fixed interval.
3. Repeat 3 runs per joint.

Pass criteria:

- Count error remains within acceptable machine tolerance
- No sporadic large count jumps

## 5. Joint Disable Behavior

1. Jog a joint to non-zero position.
2. Disable joint.
3. Re-enable joint.
4. Verify expected reset semantics and restart behavior.

Pass criteria:

- Behavior matches configured control strategy
- No stale/latched motion state leaks

## 6. Fault Trigger and Latch

1. Force a single axis driver fault input.
2. Confirm corresponding `remora.input.<fault bit>` goes active.
3. Confirm fault chain triggers estop.
4. Remove physical fault.
5. Validate latch policy:
   - If latch enabled, state remains active until clear condition.
   - Disable/enable joint and confirm clear if configured.

Pass criteria:

- Fault reaches estop path deterministically
- Latch/clear behavior matches JSON settings

## 7. Multi-Fault Aggregation

1. Trigger two different axis faults.
2. Observe OR-chain output.
3. Clear one fault, keep one active.
4. Clear all.

Pass criteria:

- OR chain remains asserted while any fault is active
- Clears only when all active faults are cleared per policy

## 8. Noise Immunity and Debounce

1. Operate spindle/VFD/noisy loads (if safe).
2. Monitor fault bits during motion and idle.
3. Repeat with debounce settings used in production JSON.

Pass criteria:

- No nuisance fault assertions
- Debounce does not mask genuine fault events

## Acceptance Record

Record for each joint:

- Timer used
- A/B pins
- Fault pin/bit
- Direction verified
- Wrap test result
- Fault test result
- Notes/actions

A release is accepted only when all sections pass with no unresolved critical findings.
