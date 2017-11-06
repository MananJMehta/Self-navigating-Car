################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../L4_IO/fat/disk/diskio.c \
../L4_IO/fat/disk/sd.c 

CPP_SRCS += \
../L4_IO/fat/disk/spi_flash.cpp 

OBJS += \
./L4_IO/fat/disk/diskio.o \
./L4_IO/fat/disk/sd.o \
./L4_IO/fat/disk/spi_flash.o 

C_DEPS += \
./L4_IO/fat/disk/diskio.d \
./L4_IO/fat/disk/sd.d 

CPP_DEPS += \
./L4_IO/fat/disk/spi_flash.d 


# Each subdirectory must supply rules for building sources it contributes
L4_IO/fat/disk/%.o: ../L4_IO/fat/disk/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m3 -mthumb -Os -fmessage-length=0 -ffunction-sections -fdata-sections -Wall -Wshadow -Wlogical-op -Wfloat-equal -DBUILD_CFG_MPU=0 -I"C:\Users\admin\Documents\Autonomous-car\lpc1758_freertos_Distance_sensors" -I"C:\Users\admin\Documents\Autonomous-car\lpc1758_freertos_Distance_sensors\newlib" -I"C:\Users\admin\Documents\Autonomous-car\lpc1758_freertos_Distance_sensors\L0_LowLevel" -I"C:\Users\admin\Documents\Autonomous-car\lpc1758_freertos_Distance_sensors\L1_FreeRTOS" -I"C:\Users\admin\Documents\Autonomous-car\lpc1758_freertos_Distance_sensors\L1_FreeRTOS\include" -I"C:\Users\admin\Documents\Autonomous-car\lpc1758_freertos_Distance_sensors\L1_FreeRTOS\portable\no_mpu" -I"C:\Users\admin\Documents\Autonomous-car\lpc1758_freertos_Distance_sensors\L2_Drivers" -I"C:\Users\admin\Documents\Autonomous-car\lpc1758_freertos_Distance_sensors\L2_Drivers\base" -I"C:\Users\admin\Documents\Autonomous-car\lpc1758_freertos_Distance_sensors\L3_Utils" -I"C:\Users\admin\Documents\Autonomous-car\lpc1758_freertos_Distance_sensors\L3_Utils\tlm" -I"C:\Users\admin\Documents\Autonomous-car\lpc1758_freertos_Distance_sensors\L4_IO" -I"C:\Users\admin\Documents\Autonomous-car\lpc1758_freertos_Distance_sensors\L4_IO\fat" -I"C:\Users\admin\Documents\Autonomous-car\lpc1758_freertos_Distance_sensors\L4_IO\wireless" -I"C:\Users\admin\Documents\Autonomous-car\lpc1758_freertos_Distance_sensors\L5_Application" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

L4_IO/fat/disk/%.o: ../L4_IO/fat/disk/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C++ Compiler'
	arm-none-eabi-g++ -mcpu=cortex-m3 -mthumb -Os -fmessage-length=0 -ffunction-sections -fdata-sections -Wall -Wshadow -Wlogical-op -Wfloat-equal -DBUILD_CFG_MPU=0 -I"C:\Users\admin\Documents\Autonomous-car\lpc1758_freertos_Distance_sensors" -I"C:\Users\admin\Documents\Autonomous-car\lpc1758_freertos_Distance_sensors\newlib" -I"C:\Users\admin\Documents\Autonomous-car\lpc1758_freertos_Distance_sensors\L0_LowLevel" -I"C:\Users\admin\Documents\Autonomous-car\lpc1758_freertos_Distance_sensors\L1_FreeRTOS" -I"C:\Users\admin\Documents\Autonomous-car\lpc1758_freertos_Distance_sensors\L1_FreeRTOS\include" -I"C:\Users\admin\Documents\Autonomous-car\lpc1758_freertos_Distance_sensors\L1_FreeRTOS\portable" -I"C:\Users\admin\Documents\Autonomous-car\lpc1758_freertos_Distance_sensors\L1_FreeRTOS\portable\no_mpu" -I"C:\Users\admin\Documents\Autonomous-car\lpc1758_freertos_Distance_sensors\L2_Drivers" -I"C:\Users\admin\Documents\Autonomous-car\lpc1758_freertos_Distance_sensors\L2_Drivers\base" -I"C:\Users\admin\Documents\Autonomous-car\lpc1758_freertos_Distance_sensors\L3_Utils" -I"C:\Users\admin\Documents\Autonomous-car\lpc1758_freertos_Distance_sensors\L3_Utils\tlm" -I"C:\Users\admin\Documents\Autonomous-car\lpc1758_freertos_Distance_sensors\L4_IO" -I"C:\Users\admin\Documents\Autonomous-car\lpc1758_freertos_Distance_sensors\L4_IO\fat" -I"C:\Users\admin\Documents\Autonomous-car\lpc1758_freertos_Distance_sensors\L4_IO\wireless" -I"C:\Users\admin\Documents\Autonomous-car\lpc1758_freertos_Distance_sensors\L5_Application" -std=gnu++11 -fabi-version=0 -fno-exceptions -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


