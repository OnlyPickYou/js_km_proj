################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../vendor/keyboard_remington/kb.c \
../vendor/keyboard_remington/kb_batt.c \
../vendor/keyboard_remington/kb_custom.c \
../vendor/keyboard_remington/kb_emi.c \
../vendor/keyboard_remington/kb_info.c \
../vendor/keyboard_remington/kb_led.c \
../vendor/keyboard_remington/kb_ota.c \
../vendor/keyboard_remington/kb_pm.c \
../vendor/keyboard_remington/kb_rf.c \
../vendor/keyboard_remington/kb_test.c \
../vendor/keyboard_remington/main.c 

OBJS += \
./vendor/keyboard_remington/kb.o \
./vendor/keyboard_remington/kb_batt.o \
./vendor/keyboard_remington/kb_custom.o \
./vendor/keyboard_remington/kb_emi.o \
./vendor/keyboard_remington/kb_info.o \
./vendor/keyboard_remington/kb_led.o \
./vendor/keyboard_remington/kb_ota.o \
./vendor/keyboard_remington/kb_pm.o \
./vendor/keyboard_remington/kb_rf.o \
./vendor/keyboard_remington/kb_test.o \
./vendor/keyboard_remington/main.o 


# Each subdirectory must supply rules for building sources it contributes
vendor/keyboard_remington/%.o: ../vendor/keyboard_remington/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: TC32 Compiler'
	tc32-elf-gcc -ffunction-sections -fdata-sections -D__PROJECT_REMINGTON_KEYBOARD__=1 -Wall -O2 -fpack-struct -fshort-enums -finline-small-functions -std=gnu99 -fshort-wchar -fms-extensions -c -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


