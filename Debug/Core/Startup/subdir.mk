################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
S_SRCS += \
../Core/Startup/startup_stm32f765vgtx.s 

OBJS += \
./Core/Startup/startup_stm32f765vgtx.o 

S_DEPS += \
./Core/Startup/startup_stm32f765vgtx.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Startup/%.o: ../Core/Startup/%.s Core/Startup/subdir.mk
	arm-none-eabi-gcc -mcpu=cortex-m7 -g3 -DDEBUG -c -I"D:/STworkspace/NANORACK_EXP_V1.2.2_FW_V1.0.1/Dev/Common" -I"D:/STworkspace/NANORACK_EXP_V1.2.2_FW_V1.0.1/Dev/Devices" -I"D:/STworkspace/NANORACK_EXP_V1.2.2_FW_V1.0.1/Dev/Sensor_I2C" -I"D:/STworkspace/NANORACK_EXP_V1.2.2_FW_V1.0.1/Dev/BSupport/UART" -I"D:/STworkspace/NANORACK_EXP_V1.2.2_FW_V1.0.1/Dev/BSupport/SysTick" -I"D:/STworkspace/NANORACK_EXP_V1.2.2_FW_V1.0.1/Dev/BSupport/I2C" -I"D:/STworkspace/NANORACK_EXP_V1.2.2_FW_V1.0.1/Dev/Devices/ADG1414" -I"D:/STworkspace/NANORACK_EXP_V1.2.2_FW_V1.0.1/Dev/Devices/IR_LED" -I"D:/STworkspace/NANORACK_EXP_V1.2.2_FW_V1.0.1/Dev/Devices/MB85RS2MT" -I"D:/STworkspace/NANORACK_EXP_V1.2.2_FW_V1.0.1/Dev/Sensor_I2C/LSM6DSOX" -I"D:/STworkspace/NANORACK_EXP_V1.2.2_FW_V1.0.1/Dev/Sensor_I2C/BME280" -I"D:/STworkspace/NANORACK_EXP_V1.2.2_FW_V1.0.1/Dev/Sensor_I2C/BMP390" -I"D:/STworkspace/NANORACK_EXP_V1.2.2_FW_V1.0.1/Dev/Sensor_I2C/H250" -I"D:/STworkspace/NANORACK_EXP_V1.2.2_FW_V1.0.1/Dev/Sensor_I2C/H3LIS331DL" -I"D:/STworkspace/NANORACK_EXP_V1.2.2_FW_V1.0.1/Dev/Sensor_I2C/K33" -I"D:/STworkspace/NANORACK_EXP_V1.2.2_FW_V1.0.1/Dev/MiddleWare/sst0_c/ports/arm-cm" -I"D:/STworkspace/NANORACK_EXP_V1.2.2_FW_V1.0.1/Dev/MiddleWare/container" -I"D:/STworkspace/NANORACK_EXP_V1.2.2_FW_V1.0.1/Dev/MiddleWare/Utils" -I"D:/STworkspace/NANORACK_EXP_V1.2.2_FW_V1.0.1/Dev/MiddleWare/sst0_c/src" -I"D:/STworkspace/NANORACK_EXP_V1.2.2_FW_V1.0.1/Dev/MiddleWare/FSM" -I"D:/STworkspace/NANORACK_EXP_V1.2.2_FW_V1.0.1/Dev/MiddleWare/STDIO" -I"D:/STworkspace/NANORACK_EXP_V1.2.2_FW_V1.0.1/Dev/App/Signals" -I"D:/STworkspace/NANORACK_EXP_V1.2.2_FW_V1.0.1/Dev/App/shell/CLI_Command" -I"D:/STworkspace/NANORACK_EXP_V1.2.2_FW_V1.0.1/Dev/App/app_main" -I"D:/STworkspace/NANORACK_EXP_V1.2.2_FW_V1.0.1/Dev/App/shell" -I"D:/STworkspace/NANORACK_EXP_V1.2.2_FW_V1.0.1/Dev/BSupport/BSP/Board" -I"D:/STworkspace/NANORACK_EXP_V1.2.2_FW_V1.0.1/Dev/MiddleWare/CLI_Terminal" -I"D:/STworkspace/NANORACK_EXP_V1.2.2_FW_V1.0.1/Dev/MiddleWare/CLI_Terminal/CLI_Src" -I"D:/STworkspace/NANORACK_EXP_V1.2.2_FW_V1.0.1/Dev/App/temperature_control" -I"D:/STworkspace/NANORACK_EXP_V1.2.2_FW_V1.0.1/Dev/BSupport/BSP/bsp_tec" -I"D:/STworkspace/NANORACK_EXP_V1.2.2_FW_V1.0.1/Dev/BSupport/BSP/bsp_ntc" -I"D:/STworkspace/NANORACK_EXP_V1.2.2_FW_V1.0.1/Dev/BSupport/BSP/bsp_heater" -I"D:/STworkspace/NANORACK_EXP_V1.2.2_FW_V1.0.1/Dev/Devices/ADS8327" -I"D:/STworkspace/NANORACK_EXP_V1.2.2_FW_V1.0.1/Dev/Devices/MCP4902" -I"D:/STworkspace/NANORACK_EXP_V1.2.2_FW_V1.0.1/Dev/BSupport/BSP/bsp_photodiode" -I"D:/STworkspace/NANORACK_EXP_V1.2.2_FW_V1.0.1/Dev/Devices/IS66WVS4M8BLL" -I"D:/STworkspace/NANORACK_EXP_V1.2.2_FW_V1.0.1/Dev/Devices/LT8722" -I"D:/STworkspace/NANORACK_EXP_V1.2.2_FW_V1.0.1/Dev/Devices/NTC" -I"D:/STworkspace/NANORACK_EXP_V1.2.2_FW_V1.0.1/Dev/BSupport/BSP/bsp_laser" -I"D:/STworkspace/NANORACK_EXP_V1.2.2_FW_V1.0.1/Dev/App/experiment" -I"D:/STworkspace/NANORACK_EXP_V1.2.2_FW_V1.0.1/Dev/BSupport/BSP/bsp_spi_ram" -I"D:/STworkspace/NANORACK_EXP_V1.2.2_FW_V1.0.1/Dev/MiddleWare/Utils/ports/stm32f7xx" -I"D:/STworkspace/NANORACK_EXP_V1.2.2_FW_V1.0.1/Drivers/CMSIS" -I"D:/STworkspace/NANORACK_EXP_V1.2.2_FW_V1.0.1/Drivers/CMSIS/Device/ST/STM32F7xx/Include" -I"D:/STworkspace/NANORACK_EXP_V1.2.2_FW_V1.0.1/Drivers/STM32F7xx_HAL_Driver" -I"D:/STworkspace/NANORACK_EXP_V1.2.2_FW_V1.0.1/Dev/App/adc_monitor" -I"D:/STworkspace/NANORACK_EXP_V1.2.2_FW_V1.0.1/Dev/App/min_shell" -I"D:/STworkspace/NANORACK_EXP_V1.2.2_FW_V1.0.1/Dev/MiddleWare/MIN_Protocol" -I"D:/STworkspace/NANORACK_EXP_V1.2.2_FW_V1.0.1/Dev/App/photodiode_cool" -I"D:/STworkspace/NANORACK_EXP_V1.2.2_FW_V1.0.1/Dev/MiddleWare/lwl" -I"D:/STworkspace/NANORACK_EXP_V1.2.2_FW_V1.0.1/Dev/App/system_log" -I"D:/STworkspace/NANORACK_EXP_V1.2.2_FW_V1.0.1/Dev/BSupport/BSP/bsp_spi_slave" -I"D:/STworkspace/NANORACK_EXP_V1.2.2_FW_V1.0.1/Dev/App/system_reset" -I"D:/STworkspace/NANORACK_EXP_V1.2.2_FW_V1.0.1/Dev/BSupport/DateTime" -I"D:/STworkspace/NANORACK_EXP_V1.2.2_FW_V1.0.1/Dev/BSupport/BSP/bsp_handshake" -I"D:/STworkspace/NANORACK_EXP_V1.2.2_FW_V1.0.1/Dev/App/spi_transmit" -I"D:/STworkspace/NANORACK_EXP_V1.2.2_FW_V1.0.1/Dev/App/wdg" -I"D:/STworkspace/NANORACK_EXP_V1.2.2_FW_V1.0.1/Dev/BSupport/BSP/bsp_watchdog" -I"D:/STworkspace/NANORACK_EXP_V1.2.2_FW_V1.0.1/Dev/BSupport/BSP/bsp_bkram" -I"D:/STworkspace/NANORACK_EXP_V1.2.2_FW_V1.0.1/Dev/BSupport/BSP/bsp_crc" -x assembler-with-cpp -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@" "$<"

clean: clean-Core-2f-Startup

clean-Core-2f-Startup:
	-$(RM) ./Core/Startup/startup_stm32f765vgtx.d ./Core/Startup/startup_stm32f765vgtx.o

.PHONY: clean-Core-2f-Startup

