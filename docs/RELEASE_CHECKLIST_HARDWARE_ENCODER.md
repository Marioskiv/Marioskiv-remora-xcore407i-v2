# Release Checklist: HardwareEncoder Upgrade

Use this checklist before publishing a release that includes HardwareEncoder support.

## 1. Firmware Build Gate

- [ ] `platformio run` succeeds for target environment
- [ ] Firmware artifacts are generated (`firmware.elf`, `firmware.bin`)
- [ ] Build size is within project limits (RAM and Flash headroom acceptable)

## 2. JSON Configuration Gate

- [ ] At least one validated 6-axis HardwareEncoder example exists
- [ ] Every HardwareEncoder has:
  - [ ] `Joint Number`
  - [ ] `Timer`
  - [ ] `Channel A Pin`
  - [ ] `Channel B Pin`
- [ ] Fault reporting fields are set consistently (`Fault Pin`, `Fault Bit`, polarity)
- [ ] Fault hardening defaults are set (`Fault Latch`, `Fault Debounce Samples`)

## 3. Safety and Fault Gate

- [ ] Fault input triggers update in `txData.inputs` immediately
- [ ] LinuxCNC fault chain drives estop reliably
- [ ] Fault latch clear policy is validated (`Fault Clear On Joint Disable`)
- [ ] No false trips under idle electrical noise conditions

## 4. Motion and Encoder Gate

- [ ] Joint enable/disable resets behavior is validated
- [ ] Encoder direction verified on all axes
- [ ] Wrap-around continuity verified on 16-bit timers
- [ ] High-speed jog does not lose feedback counts

## 5. LinuxCNC Integration Gate

- [ ] HAL template used for HardwareEncoder deployment:
  - `linuxcnc/xcore407i_6axis_hardware_encoder.hal`
- [ ] INI gains and limits reviewed for machine-specific constraints
- [ ] Probe and estop path behavior verified in UI and logs

## 6. Documentation Gate

- [ ] Migration guide present and reviewed
- [ ] Example config is linked from docs index
- [ ] LinuxCNC README references new template and workflow
- [ ] Changelog entry added

## 7. Regression Gate

- [ ] Existing software `Encoder` configs still load
- [ ] Existing `Alarm` module behavior unchanged
- [ ] Boot/startup sequence unchanged except new module availability

## 8. Publish Gate

- [ ] Tag release version
- [ ] Include firmware hash/build metadata
- [ ] Attach known limitations and timer conflict notes
- [ ] Archive validated config and HAL files used during acceptance tests
