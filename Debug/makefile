################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include Drivers/STM32F7xx_HAL_Driver/Src/subdir.mk
-include Dev/MiddleWare/sst0_c/src/subdir.mk
-include Dev/MiddleWare/lwl/subdir.mk
-include Dev/MiddleWare/container/subdir.mk
-include Dev/MiddleWare/Utils/subdir.mk
-include Dev/MiddleWare/STDIO/subdir.mk
-include Dev/MiddleWare/MIN_Protocol/subdir.mk
-include Dev/MiddleWare/FSM/subdir.mk
-include Dev/MiddleWare/CLI_Terminal/CLI_Src/subdir.mk
-include Dev/Devices/NTC/subdir.mk
-include Dev/Devices/MCP4902/subdir.mk
-include Dev/Devices/MB85RS2MT/subdir.mk
-include Dev/Devices/LT8722/subdir.mk
-include Dev/Devices/IS66WVS4M8BLL/subdir.mk
-include Dev/Devices/IR_LED/subdir.mk
-include Dev/Devices/ADS8327/subdir.mk
-include Dev/Devices/ADG1414/subdir.mk
-include Dev/BSupport/UART/RingBuffer/subdir.mk
-include Dev/BSupport/UART/subdir.mk
-include Dev/BSupport/SysTick/subdir.mk
-include Dev/BSupport/I2C/subdir.mk
-include Dev/BSupport/DateTime/subdir.mk
-include Dev/BSupport/BSP/bsp_watchdog/subdir.mk
-include Dev/BSupport/BSP/bsp_tec/subdir.mk
-include Dev/BSupport/BSP/bsp_spi_slave/subdir.mk
-include Dev/BSupport/BSP/bsp_spi_ram/subdir.mk
-include Dev/BSupport/BSP/bsp_photodiode/subdir.mk
-include Dev/BSupport/BSP/bsp_ntc/subdir.mk
-include Dev/BSupport/BSP/bsp_laser/subdir.mk
-include Dev/BSupport/BSP/bsp_heater/subdir.mk
-include Dev/BSupport/BSP/bsp_crc/subdir.mk
-include Dev/BSupport/BSP/bsp_bkram/subdir.mk
-include Dev/BSupport/BSP/Board/subdir.mk
-include Dev/App/wdg/subdir.mk
-include Dev/App/temperature_control/subdir.mk
-include Dev/App/system_log/subdir.mk
-include Dev/App/spi_transmit/subdir.mk
-include Dev/App/shell/CLI_Command/subdir.mk
-include Dev/App/shell/subdir.mk
-include Dev/App/photodiode_cool/subdir.mk
-include Dev/App/min_shell/subdir.mk
-include Dev/App/experiment/subdir.mk
-include Dev/App/app_main/subdir.mk
-include Core/Startup/subdir.mk
-include Core/Src/subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

OPTIONAL_TOOL_DEPS := \
$(wildcard ../makefile.defs) \
$(wildcard ../makefile.init) \
$(wildcard ../makefile.targets) \


BUILD_ARTIFACT_NAME := NANORACK_EXP_V1.2.2_FW_V1.0.1
BUILD_ARTIFACT_EXTENSION := elf
BUILD_ARTIFACT_PREFIX :=
BUILD_ARTIFACT := $(BUILD_ARTIFACT_PREFIX)$(BUILD_ARTIFACT_NAME)$(if $(BUILD_ARTIFACT_EXTENSION),.$(BUILD_ARTIFACT_EXTENSION),)

# Add inputs and outputs from these tool invocations to the build variables 
EXECUTABLES += \
NANORACK_EXP_V1.2.2_FW_V1.0.1.elf \

MAP_FILES += \
NANORACK_EXP_V1.2.2_FW_V1.0.1.map \

OBJCOPY_HEX += \
NANORACK_EXP_V1.2.2_FW_V1.0.1.hex \

OBJCOPY_BIN += \
NANORACK_EXP_V1.2.2_FW_V1.0.1.bin \


# All Target
all: main-build

# Main-build Target
main-build: NANORACK_EXP_V1.2.2_FW_V1.0.1.elf secondary-outputs

# Tool invocations
NANORACK_EXP_V1.2.2_FW_V1.0.1.elf NANORACK_EXP_V1.2.2_FW_V1.0.1.map: $(OBJS) $(USER_OBJS) D:\STworkspace\NANORACK_EXP_V1.2.2_FW_V1.0.1\STM32F765VGTX_FLASH.ld makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-gcc -o "NANORACK_EXP_V1.2.2_FW_V1.0.1.elf" @"objects.list" $(USER_OBJS) $(LIBS) -mcpu=cortex-m7 -T"D:\STworkspace\NANORACK_EXP_V1.2.2_FW_V1.0.1\STM32F765VGTX_FLASH.ld" --specs=nosys.specs -Wl,-Map="NANORACK_EXP_V1.2.2_FW_V1.0.1.map" -Wl,--gc-sections -static --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -u _printf_float -Wl,--start-group -lc -lm -Wl,--end-group
	@echo 'Finished building target: $@'
	@echo ' '

NANORACK_EXP_V1.2.2_FW_V1.0.1.hex: $(EXECUTABLES) makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-objcopy  -O ihex $(EXECUTABLES) "NANORACK_EXP_V1.2.2_FW_V1.0.1.hex"
	@echo 'Finished building: $@'
	@echo ' '

NANORACK_EXP_V1.2.2_FW_V1.0.1.bin: $(EXECUTABLES) makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-objcopy  -O binary $(EXECUTABLES) "NANORACK_EXP_V1.2.2_FW_V1.0.1.bin"
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) NANORACK_EXP_V1.2.2_FW_V1.0.1.bin NANORACK_EXP_V1.2.2_FW_V1.0.1.elf NANORACK_EXP_V1.2.2_FW_V1.0.1.hex NANORACK_EXP_V1.2.2_FW_V1.0.1.map
	-@echo ' '

secondary-outputs: $(OBJCOPY_HEX) $(OBJCOPY_BIN)

fail-specified-linker-script-missing:
	@echo 'Error: Cannot find the specified linker script. Check the linker settings in the build configuration.'
	@exit 2

warn-no-linker-script-specified:
	@echo 'Warning: No linker script specified. Check the linker settings in the build configuration.'

.PHONY: all clean dependents main-build fail-specified-linker-script-missing warn-no-linker-script-specified

-include ../makefile.targets
