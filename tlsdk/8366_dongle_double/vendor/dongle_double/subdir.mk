################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../vendor/dongle_double/dongle.c \
../vendor/dongle_double/dongle_custom.c \
../vendor/dongle_double/dongle_emi.c \
../vendor/dongle_double/dongle_rf.c \
../vendor/dongle_double/dongle_suspend.c \
../vendor/dongle_double/dongle_usb.c \
../vendor/dongle_double/main.c 

OBJS += \
./vendor/dongle_double/dongle.o \
./vendor/dongle_double/dongle_custom.o \
./vendor/dongle_double/dongle_emi.o \
./vendor/dongle_double/dongle_rf.o \
./vendor/dongle_double/dongle_suspend.o \
./vendor/dongle_double/dongle_usb.o \
./vendor/dongle_double/main.o 


# Each subdirectory must supply rules for building sources it contributes
vendor/dongle_double/%.o: ../vendor/dongle_double/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: TC32 Compiler'
	tc32-elf-gcc -ffunction-sections -fdata-sections -D__PROJECT_DONGLE_DOUBLE__=1 -D__PROJECT_DONGLE_8366__=1 -Wall -O2 -fpack-struct -fshort-enums -finline-small-functions -std=gnu99 -fshort-wchar -fms-extensions -c -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


