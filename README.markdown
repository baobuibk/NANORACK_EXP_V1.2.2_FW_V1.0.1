# CLI Command Reference for Firmware Control

This document provides a comprehensive guide to the Command Line Interface (CLI) commands available for controlling and configuring the firmware. Each command is listed with its category, format, description, and an example to help users understand and utilize the commands effectively.

## General Commands

### help
- **Description**: Displays a list of all available CLI commands for Firmware version 1.
- **Format**: `help`
- **Example**: `help`
  - Output: Lists all commands with their descriptions.

### cls
- **Description**: Clears the console output screen.
- **Format**: `cls`
- **Example**: `cls`
  - Output: Clears the terminal screen.

### reset
- **Description**: Performs a software reset of the Microcontroller Unit (MCU).
- **Format**: `reset`
- **Example**: `reset`
  - Output: Initiates a software reset of the MCU.

## NTC (Temperature Sensor) Commands

### ntc_get_temp
- **Description**: Reads the temperature value from the specified NTC sensor channel or all channels.
- **Format**: `ntc_get_temp [channel/a]`
  - `channel`: 0-7 (specific channel)
  - `a`: All channels
- **Example**: `ntc_get_temp 0`
  - Output: Returns the temperature value from NTC channel 0.
- **Example**: `ntc_get_temp a`
  - Output: Returns temperature values from all NTC channels.

## Power Supply Commands

### pwr_5v_set
- **Description**: Turns the 5V power supply ON or OFF.
- **Format**: `pwr_5v_set [0/1]`
  - `0`: OFF
  - `1`: ON
- **Example**: `pwr_5v_set 0`
  - Output: Turns OFF the 5V power supply.
- **Example**: `pwr_5v_set 1`
  - Output: Turns ON the 5V power supply.

### pwr_5v_get
- **Description**: Reads the current state of the 5V power supply.
- **Format**: `pwr_5v_get`
- **Example**: `pwr_5v_get`
  - Output: Returns `0` (OFF) or `1` (ON).

## TEC (Thermoelectric Cooler) Commands

### tec_init
- **Description**: Initializes the TEC driver for a specific channel or all channels.
- **Format**: `tec_init [channel/a]`
  - `channel`: 0-3 (specific TEC channel)
  - `a`: All TEC channels
- **Example**: `tec_init 0`
  - Output: Initializes TEC channel 0.
- **Example**: `tec_init a`
  - Output: Initializes all TEC channels.

### tec_profile_volt_set
- **Description**: Sets the profile voltage for TEC channels in auto mode (500-2500 mV).
- **Format**: `tec_profile_volt_set [voltage]`
- **Example**: `tec_profile_volt_set 1000`
  - Output: Sets the profile voltage to 1000 mV for TECs in auto mode.

### tec_profile_volt_get
- **Description**: Reads the current profile voltage for a specific TEC channel or all channels.
- **Format**: `tec_profile_volt_get [channel/a]`
  - `channel`: 0-3 (specific TEC channel)
  - `a`: All TEC channels
- **Example**: `tec_profile_volt_get 1`
  - Output: Returns the profile voltage for TEC channel 1.
- **Example**: `tec_profile_volt_get a`
  - Output: Returns the profile voltages for all TEC channels.

### tec_profile_set
- **Description**: Registers TEC channels for auto mode temperature control (0: OFF, 1: ON).
- **Format**: `tec_profile_set [tec0] [tec1] [tec2] [tec3]`
- **Example**: `tec_profile_set 1 0 1 0`
  - Output: Sets TEC0 and TEC2 to auto mode, TEC1 and TEC3 to OFF.

### tec_profile_get
- **Description**: Retrieves the auto mode status of all TEC channels.
- **Format**: `tec_profile_get`
- **Example**: `tec_profile_get`
  - Output: Returns the auto mode status (0/1) for all TEC channels.

### tec_man_volt_set
- **Description**: Sets the voltage and direction (cool/heat) for a TEC in manual mode.
- **Format**: `tec_man_volt_set [tec_idx] [cool/heat] [milivolt]`
  - `tec_idx`: 0-3 (TEC channel)
  - `cool/heat`: 0 (cool), 1 (heat)
  - `milivolt`: Voltage in millivolts
- **Example**: `tec_man_volt_set 0 0 1000`
  - Output: Sets TEC0 to cool mode with 1000 mV.

### tec_man_output_set
- **Description**: Enables or disables TEC switching in manual mode.
- **Format**: `tec_man_output_set [tec_idx] [1/0]`
  - `tec_idx`: 0-3 (TEC channel)
  - `1`: Enable, `0`: Disable
- **Example**: `tec_man_output_set 2 1`
  - Output: Enables switching for TEC2.

### tec_ovr_set
- **Description**: Sets a TEC to override mode with a fixed voltage (default cool mode).
- **Format**: `tec_ovr_set [tec_idx] [milivolt]`
  - `tec_idx`: 0-3 (TEC channel)
  - `milivolt`: Voltage in millivolts
- **Example**: `tec_ovr_set 1 1200`
  - Output: Sets TEC1 to override mode with 1200 mV (cool).

### tec_ovr_start
- **Description**: Enables switching for a TEC in override mode.
- **Format**: `tec_ovr_start`
- **Example**: `tec_ovr_start`
  - Output: Enables switching for the TEC in override mode.

### tec_ovr_stop
- **Description**: Disables switching for a TEC in override mode.
- **Format**: `tec_ovr_stop`
- **Example**: `tec_ovr_stop`
  - Output: Disables switching for the TEC in override mode.

### tec_ovr_get
- **Description**: Retrieves the status of TECs in override mode.
- **Format**: `tec_ovr_get`
- **Example**: `tec_ovr_get`
  - Output: Returns which TECs are active in override mode.

## Heater Commands

### htr_profile_duty_set
- **Description**: Sets the duty cycle for a heater in profile mode (0-100%).
- **Format**: `htr_profile_duty_set [idx] [duty]`
  - `idx`: 0-3 (heater index)
  - `duty`: 0-100 (percentage)
- **Example**: `htr_profile_duty_set 0 50`
  - Output: Sets heater 0 to a 50% duty cycle.

### htr_profile_duty_get
- **Description**: Reads the current duty cycle setting for a specific heater or all heaters.
- **Format**: `htr_profile_duty_get [idx/a]`
  - `idx`: 0-3 (heater index)
  - `a`: All heaters
- **Example**: `htr_profile_duty_get 1`
  - Output: Returns the duty cycle for heater 1.
- **Example**: `htr_profile_duty_get a`
  - Output: Returns the duty cycles for all heaters.

### htr_profile_set
- **Description**: Registers heaters for auto mode (0: OFF, 1: ON).
- **Format**: `htr_profile_set [ht0] [ht1] [ht2] [ht3]`
- **Example**: `htr_profile_set 1 1 0 0`
  - Output: Sets heaters 0 and 1 to auto mode, heaters 2 and 3 to OFF.

### htr_profile_get
- **Description**: Retrieves the auto mode status of all heaters.
- **Format**: `htr_profile_get`
- **Example**: `htr_profile_get`
  - Output: Returns the auto mode status (0/1) for all heaters.

## Reference Temperature Commands

### ref_temp_set
- **Description**: Sets the reference temperature for control logic in auto mode (°C).
- **Format**: `ref_temp_set [temp]`
- **Example**: `ref_temp_set 252`
  - Output: Sets the reference temperature to 25.2°C.

### ref_temp_get
- **Description**: Reads the current reference temperature setting.
- **Format**: `ref_temp_get`
- **Example**: `ref_temp_get`
  - Output: Returns the current reference temperature.

### ref_ntc_set
- **Description**: Selects NTC channel for temperature feedback in auto mode (0: OFF, 1: ON).
- **Format**: `ref_ntc_set [ntc_idx]`
- **Example**: `ref_ntc_set 1`
  - Output: Selects NTC1 for temperature feedback.

### ref_ntc_get
- **Description**: Retrieves the currently selected NTC channels for temperature control.
- **Format**: `ref_ntc_get`
- **Example**: `ref_ntc_get`
  - Output: Returns the NTC channels used for feedback.

## Auto Control Commands

### auto_control_start
- **Description**: Starts the auto temperature control mode using the configured TEC, heater, and NTC profiles.
- **Format**: `auto_control_start`
- **Example**: `auto_control_start`
  - Output: Initiates auto temperature control.

### auto_control_stop
- **Description**: Stops the auto temperature control mode.
- **Format**: `auto_control_stop`
- **Example**: `auto_control_stop`
  - Output: Stops auto temperature control.

## Laser and Photodiode Commands

### laser_int_set_current
- **Description**: Sets the current for the internal laser as a percentage of the 3.3V reference.
- **Format**: `laser_int_set_current [percent]`
- **Example**: `laser_int_set_current 50`
  - Output: Sets the internal laser current to 50% of 3.3V.

### laser_ext_set_current
- **Description**: Sets the current for the external laser as a percentage of the 3.3V reference.
- **Format**: `laser_ext_set_current [percent]`
- **Example**: `laser_ext_set_current 75`
  - Output: Sets the external laser current to 75% of 3.3V.

### laser_get_current
- **Description**: Reads the current setting for the internal (0), external (1), or all (a) lasers.
- **Format**: `laser_get_current [0/1/a]`
- **Example**: `laser_get_current 0`
  - Output: Returns the current setting for the internal laser.
- **Example**: `laser_get_current a`
  - Output: Returns the current settings for both internal and external lasers. Example: 255 is 25.5mA

### laser_int_switch_on
- **Description**: Turns on the specified internal laser position.
- **Format**: `laser_int_switch_on [pos]`
- **Example**: `laser_int_switch_on 1`
  - Output: Turns on the internal laser at position 1.

### laser_ext_switch_on
- **Description**: Turns on the specified external laser position.
- **Format**: `laser_ext_switch_on [pos]`
- **Example**: `laser_ext_switch_on 2`
  - Output: Turns on the external laser at position 2.

### laser_int_switch_off
- **Description**: Turns off all internal lasers.
- **Format**: `laser_int_switch_off`
- **Example**: `laser_int_switch_off`
  - Output: Turns off all internal lasers.

### laser_ext_switch_off
- **Description**: Turns off all external lasers.
- **Format**: `laser_ext_switch_off`
- **Example**: `laser_ext_switch_off`
  - Output: Turns off all external lasers.

## Experiment Commands

### exp_set_profile
- **Description**: Configures the experiment profile with sampling rate, photodiode position, laser current percentage, and timing parameters.
- **Format**: `exp_set_profile [sampling_rate] [pos] [laser_percent] [pre_time] [experiment_time] [post_time]`
  - `sampling_rate`: Sampling rate in Hz
  - `pos`: Photodiode position
  - `laser_percent`: Laser current percentage
  - `pre_time`: Pre-measurement time (ms)
  - `experiment_time`: Sampling time (ms)
  - `post_time`: Post-measurement time (ms)
- **Example**: `exp_set_profile 10000 1 50 1000 2000 1000`
  - Output: Sets experiment with 10 kHz sampling, photodiode at position 1, 50% laser current, 1000 ms pre-time, 2000 ms sampling time, and 1000 ms post-time.

### exp_get_profile
- **Description**: Retrieves the current experiment profile settings.
- **Format**: `exp_get_profile`
- **Example**: `exp_get_profile`
  - Output: Returns the current experiment profile parameters.

### exp_start_measuring
- **Description**: Starts the experiment process: activates photodiode, waits for pre_time, turns on laser, waits for experiment_time, turns off laser, and stops after post_time.
- **Format**: `exp_start_measuring`
- **Example**: `exp_start_measuring`
  - Output: Initiates the experiment according to the set profile.

### exp_ram_read
- **Description**: Reads measurement results stored in RAM at the specified address and number of samples.
- **Format**: `exp_ram_read [address] [num_sample] [mode]`
- **Example**: 
  -`exp_ram_read 0 1000 0`
    - Output: Reads 1000 samples starting from address 0 in RAM by ascii character.
  -`exp_ram_read 0 1000 1`
    - Output: Reads 1000 samples starting from address 0 in RAM by frame binary: `3byte header[b1111 + b(size)]` + `data_big_endian` + `crc16_ccit_false`
  

## Notes
- Ensure the correct command format is used as specified above to avoid errors.
- For commands requiring channel selection (e.g., `ntc_get_temp`, `tec_init`), use either a specific channel number or `a` for all channels.
- Voltage and duty cycle values must be within the specified ranges to ensure proper operation.
- For further assistance or clarification, contact the support team or refer to the firmware documentation.