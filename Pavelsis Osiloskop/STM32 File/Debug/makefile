################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include Drivers/STM32F0xx_HAL_Driver/Src/subdir.mk
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


BUILD_ARTIFACT_NAME := 009_Qt_ADC_RX_TX_MAGNITUDE_FREQ_UPDATE
BUILD_ARTIFACT_EXTENSION := elf
BUILD_ARTIFACT_PREFIX :=
BUILD_ARTIFACT := $(BUILD_ARTIFACT_PREFIX)$(BUILD_ARTIFACT_NAME)$(if $(BUILD_ARTIFACT_EXTENSION),.$(BUILD_ARTIFACT_EXTENSION),)

# Add inputs and outputs from these tool invocations to the build variables 
EXECUTABLES += \
009_Qt_ADC_RX_TX_MAGNITUDE_FREQ_UPDATE.elf \

MAP_FILES += \
009_Qt_ADC_RX_TX_MAGNITUDE_FREQ_UPDATE.map \

SIZE_OUTPUT += \
default.size.stdout \

OBJDUMP_LIST += \
009_Qt_ADC_RX_TX_MAGNITUDE_FREQ_UPDATE.list \


# All Target
all: main-build

# Main-build Target
main-build: 009_Qt_ADC_RX_TX_MAGNITUDE_FREQ_UPDATE.elf secondary-outputs

# Tool invocations
009_Qt_ADC_RX_TX_MAGNITUDE_FREQ_UPDATE.elf 009_Qt_ADC_RX_TX_MAGNITUDE_FREQ_UPDATE.map: $(OBJS) $(USER_OBJS) C:\Users\Yavuz\STM32CubeIDE\workspace_pavelsis\009_Qt_ADC_RX_TX_MAGNITUDE_FREQ_UPDATE\STM32F072RBTX_FLASH.ld makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-gcc -o "009_Qt_ADC_RX_TX_MAGNITUDE_FREQ_UPDATE.elf" @"objects.list" $(USER_OBJS) $(LIBS) -mcpu=cortex-m0 -T"C:\Users\Yavuz\STM32CubeIDE\workspace_pavelsis\009_Qt_ADC_RX_TX_MAGNITUDE_FREQ_UPDATE\STM32F072RBTX_FLASH.ld" --specs=nosys.specs -Wl,-Map="009_Qt_ADC_RX_TX_MAGNITUDE_FREQ_UPDATE.map" -Wl,--gc-sections -static --specs=nano.specs -mfloat-abi=soft -mthumb -Wl,--start-group -lc -lm -Wl,--end-group -u _printf_float -u _scanf_float
	@echo 'Finished building target: $@'
	@echo ' '

default.size.stdout: $(EXECUTABLES) makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-size  $(EXECUTABLES)
	@echo 'Finished building: $@'
	@echo ' '

009_Qt_ADC_RX_TX_MAGNITUDE_FREQ_UPDATE.list: $(EXECUTABLES) makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-objdump -h -S $(EXECUTABLES) > "009_Qt_ADC_RX_TX_MAGNITUDE_FREQ_UPDATE.list"
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) 009_Qt_ADC_RX_TX_MAGNITUDE_FREQ_UPDATE.elf 009_Qt_ADC_RX_TX_MAGNITUDE_FREQ_UPDATE.list 009_Qt_ADC_RX_TX_MAGNITUDE_FREQ_UPDATE.map default.size.stdout
	-@echo ' '

secondary-outputs: $(SIZE_OUTPUT) $(OBJDUMP_LIST)

fail-specified-linker-script-missing:
	@echo 'Error: Cannot find the specified linker script. Check the linker settings in the build configuration.'
	@exit 2

warn-no-linker-script-specified:
	@echo 'Warning: No linker script specified. Check the linker settings in the build configuration.'

.PHONY: all clean dependents main-build fail-specified-linker-script-missing warn-no-linker-script-specified

-include ../makefile.targets
