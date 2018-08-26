################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include subdir.mk
-include vendor/remote/subdir.mk
-include vendor/link_layer/subdir.mk
-include vendor/common/subdir.mk
-include proj_lib/subdir.mk
-include proj_lib/blt_ll/subdir.mk
-include proj/mcu_spec/subdir.mk
-include proj/mcu/subdir.mk
-include proj/drivers/subdir.mk
-include proj/common/subdir.mk
-include proj/app/subdir.mk
-include objects.mk

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
LST += \
remote.lst \

FLASH_IMAGE += \
remote.bin \

SIZEDUMMY += \
sizedummy \


# All Target
all: pre-build main-build

# Main-build Target
main-build: hamster.elf secondary-outputs

# Tool invocations
hamster.elf: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: TC32 C Linker'
	tc32-elf-ld --gc-sections -L"E:\code_on_git_server\hamster\hamster\tlsdk\proj_lib" -T ../boot.link -o"hamster.elf" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

remote.lst: hamster.elf
	@echo 'Invoking: TC32 Create Extended Listing'
	tc32-elf-objdump -x -D -l -S hamster.elf  >"remote.lst"
	@echo 'Finished building: $@'
	@echo ' '

remote.bin: hamster.elf
	@echo 'Create Flash image (binary format)'
	tc32-elf-objcopy -O binary hamster.elf  "remote.bin"
	@echo 'Finished building: $@'
	@echo ' '

sizedummy: hamster.elf
	@echo 'Invoking: Print Size'
	tc32-elf-size -t hamster.elf
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(OBJS)$(FLASH_IMAGE)$(ELFS)$(LST)$(SIZEDUMMY) hamster.elf
	-@echo ' '

pre-build:
	-E:\code_on_git_server\hamster\hamster\tlsdk/getver.sh
	-@echo ' '

secondary-outputs: $(LST) $(FLASH_IMAGE) $(SIZEDUMMY)

.PHONY: all clean dependents
.SECONDARY: main-build pre-build

-include ../makefile.targets
