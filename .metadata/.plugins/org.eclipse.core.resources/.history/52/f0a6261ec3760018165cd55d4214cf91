################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../proj/mcu_spec/adc_8267.c \
../proj/mcu_spec/gpio_8266.c \
../proj/mcu_spec/gpio_8267.c \
../proj/mcu_spec/gpio_8366.c \
../proj/mcu_spec/gpio_8368.c \
../proj/mcu_spec/gpio_8510.c \
../proj/mcu_spec/gpio_8511.c 

S_UPPER_SRCS += \
../proj/mcu_spec/cstartup_8266.S \
../proj/mcu_spec/cstartup_8266_ram.S \
../proj/mcu_spec/cstartup_8267.S \
../proj/mcu_spec/cstartup_8267_ram.S \
../proj/mcu_spec/cstartup_8366.S \
../proj/mcu_spec/cstartup_8366_ram.S \
../proj/mcu_spec/cstartup_8368.S \
../proj/mcu_spec/cstartup_8368_ram.S \
../proj/mcu_spec/cstartup_8510.S \
../proj/mcu_spec/cstartup_8510_ram.S \
../proj/mcu_spec/cstartup_8511.S \
../proj/mcu_spec/cstartup_8511_ram.S 

OBJS += \
./proj/mcu_spec/adc_8267.o \
./proj/mcu_spec/cstartup_8266.o \
./proj/mcu_spec/cstartup_8266_ram.o \
./proj/mcu_spec/cstartup_8267.o \
./proj/mcu_spec/cstartup_8267_ram.o \
./proj/mcu_spec/cstartup_8366.o \
./proj/mcu_spec/cstartup_8366_ram.o \
./proj/mcu_spec/cstartup_8368.o \
./proj/mcu_spec/cstartup_8368_ram.o \
./proj/mcu_spec/cstartup_8510.o \
./proj/mcu_spec/cstartup_8510_ram.o \
./proj/mcu_spec/cstartup_8511.o \
./proj/mcu_spec/cstartup_8511_ram.o \
./proj/mcu_spec/gpio_8266.o \
./proj/mcu_spec/gpio_8267.o \
./proj/mcu_spec/gpio_8366.o \
./proj/mcu_spec/gpio_8368.o \
./proj/mcu_spec/gpio_8510.o \
./proj/mcu_spec/gpio_8511.o 


# Each subdirectory must supply rules for building sources it contributes
proj/mcu_spec/%.o: ../proj/mcu_spec/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: TC32 Compiler'
	tc32-elf-gcc -ffunction-sections -fdata-sections -D__PROJECT_REMINGTON_KEYBOARD__=1 -Wall -O2 -fpack-struct -fshort-enums -finline-small-functions -std=gnu99 -fshort-wchar -fms-extensions -c -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

proj/mcu_spec/%.o: ../proj/mcu_spec/%.S
	@echo 'Building file: $<'
	@echo 'Invoking: TC32 CC/Assembler'
	tc32-elf-gcc -DMCU_CORE_8368 -D__LOAD_RAM_SIZE__=0xa -c -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


