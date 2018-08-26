################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../vendor/remote/main.c \
../vendor/remote/rc_keyboard.c \
../vendor/remote/rc_mouse.c \
../vendor/remote/remote.c 

OBJS += \
./vendor/remote/main.o \
./vendor/remote/rc_keyboard.o \
./vendor/remote/rc_mouse.o \
./vendor/remote/remote.o 


# Each subdirectory must supply rules for building sources it contributes
vendor/remote/%.o: ../vendor/remote/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: TC32 Compiler'
	tc32-elf-gcc -ffunction-sections -fdata-sections -D__PROJECT_REMOTE__=1 -Wall -O2 -fpack-struct -fshort-enums -finline-small-functions -std=gnu99 -fshort-wchar -fms-extensions -c -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


