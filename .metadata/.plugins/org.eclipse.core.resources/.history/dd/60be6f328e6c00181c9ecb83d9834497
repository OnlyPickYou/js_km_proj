################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../vendor/dongle/dongle.c \
../vendor/dongle/dongle_custom.c \
../vendor/dongle/dongle_emi.c \
../vendor/dongle/dongle_rf.c \
../vendor/dongle/dongle_suspend.c \
../vendor/dongle/dongle_usb.c \
../vendor/dongle/main.c 

OBJS += \
./vendor/dongle/dongle.o \
./vendor/dongle/dongle_custom.o \
./vendor/dongle/dongle_emi.o \
./vendor/dongle/dongle_rf.o \
./vendor/dongle/dongle_suspend.o \
./vendor/dongle/dongle_usb.o \
./vendor/dongle/main.o 


# Each subdirectory must supply rules for building sources it contributes
vendor/dongle/%.o: ../vendor/dongle/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: TC32 Compiler'
	tc32-elf-gcc -ffunction-sections -fdata-sections -D__PROJECT_DONGLE__=1 -D__PROJECT_DONGLE_8366__=1 -Wall -O2 -fpack-struct -fshort-enums -finline-small-functions -std=gnu99 -fshort-wchar -fms-extensions -c -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


