################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (9-2020-q2-update)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Src/AdcDriver.c \
../Src/BasicTimer.c \
../Src/CaptureFrecDriver.c \
../Src/ExtiDriver.c \
../Src/GPIOxDriver.c \
../Src/I2CDriver.c \
../Src/I2CLCD.c \
../Src/LCDDriver.c \
../Src/MainClockDriver.c \
../Src/PwmDriver.c \
../Src/RTCxDriver.c \
../Src/SpiDriver.c \
../Src/USARTxDriver.c \
../Src/prueba.c \
../Src/syscalls.c \
../Src/sysmem.c 

OBJS += \
./Src/AdcDriver.o \
./Src/BasicTimer.o \
./Src/CaptureFrecDriver.o \
./Src/ExtiDriver.o \
./Src/GPIOxDriver.o \
./Src/I2CDriver.o \
./Src/I2CLCD.o \
./Src/LCDDriver.o \
./Src/MainClockDriver.o \
./Src/PwmDriver.o \
./Src/RTCxDriver.o \
./Src/SpiDriver.o \
./Src/USARTxDriver.o \
./Src/prueba.o \
./Src/syscalls.o \
./Src/sysmem.o 

C_DEPS += \
./Src/AdcDriver.d \
./Src/BasicTimer.d \
./Src/CaptureFrecDriver.d \
./Src/ExtiDriver.d \
./Src/GPIOxDriver.d \
./Src/I2CDriver.d \
./Src/I2CLCD.d \
./Src/LCDDriver.d \
./Src/MainClockDriver.d \
./Src/PwmDriver.d \
./Src/RTCxDriver.d \
./Src/SpiDriver.d \
./Src/USARTxDriver.d \
./Src/prueba.d \
./Src/syscalls.d \
./Src/sysmem.d 


# Each subdirectory must supply rules for building sources it contributes
Src/%.o: ../Src/%.c Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -DNUCLEO_F411RE -DSTM32 -DSTM32F4 -DSTM32F411RETx -c -I../Inc -Os -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

