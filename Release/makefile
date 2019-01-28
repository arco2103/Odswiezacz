################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
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

# Add inputs and outputs from these tool invocations to the build variables 
LSS += \
12_Silnik_DC.lss \

FLASH_IMAGE += \
12_Silnik_DC.hex \

EEPROM_IMAGE += \
12_Silnik_DC.eep \

SIZEDUMMY += \
sizedummy \


# All Target
all: 12_Silnik_DC.elf secondary-outputs

# Tool invocations
12_Silnik_DC.elf: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: AVR C Linker'
	avr-gcc -Wl,-Map,12_Silnik_DC.map -mmcu=atmega8 -o "12_Silnik_DC.elf" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

12_Silnik_DC.lss: 12_Silnik_DC.elf
	@echo 'Invoking: AVR Create Extended Listing'
	-avr-objdump -h -S 12_Silnik_DC.elf  >"12_Silnik_DC.lss"
	@echo 'Finished building: $@'
	@echo ' '

12_Silnik_DC.hex: 12_Silnik_DC.elf
	@echo 'Create Flash image (ihex format)'
	-avr-objcopy -R .eeprom -R .fuse -R .lock -R .signature -O ihex 12_Silnik_DC.elf  "12_Silnik_DC.hex"
	@echo 'Finished building: $@'
	@echo ' '

12_Silnik_DC.eep: 12_Silnik_DC.elf
	@echo 'Create eeprom image (ihex format)'
	-avr-objcopy -j .eeprom --no-change-warnings --change-section-lma .eeprom=0 -O ihex 12_Silnik_DC.elf  "12_Silnik_DC.eep"
	@echo 'Finished building: $@'
	@echo ' '

sizedummy: 12_Silnik_DC.elf
	@echo 'Invoking: Print Size'
	-avr-size --format=avr --mcu=atmega8 12_Silnik_DC.elf
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(FLASH_IMAGE)$(ELFS)$(OBJS)$(ASM_DEPS)$(EEPROM_IMAGE)$(S_DEPS)$(SIZEDUMMY)$(S_UPPER_DEPS)$(LSS)$(C_DEPS) 12_Silnik_DC.elf
	-@echo ' '

secondary-outputs: $(LSS) $(FLASH_IMAGE) $(EEPROM_IMAGE) $(SIZEDUMMY)

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets
