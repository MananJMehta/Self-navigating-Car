################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../L5_Application/main.cpp 

OBJS += \
./L5_Application/main.o 

CPP_DEPS += \
./L5_Application/main.d 


# Each subdirectory must supply rules for building sources it contributes
L5_Application/%.o: ../L5_Application/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C++ Compiler'
	arm-none-eabi-g++ -mcpu=cortex-m3 -mthumb -Os -fmessage-length=0 -ffunction-sections -fdata-sections -Wall -Wshadow -Wlogical-op -Wfloat-equal -DBUILD_CFG_MPU=0 -I"C:\Users\admin\Documents\Autonomous-car\lpc1758_freertos_Distance_sensors" -I"C:\Users\admin\Documents\Autonomous-car\lpc1758_freertos_Distance_sensors\newlib" -I"C:\Users\admin\Documents\Autonomous-car\lpc1758_freertos_Distance_sensors\L0_LowLevel" -I"C:\Users\admin\Documents\Autonomous-car\lpc1758_freertos_Distance_sensors\L1_FreeRTOS" -I"C:\Users\admin\Documents\Autonomous-car\lpc1758_freertos_Distance_sensors\L1_FreeRTOS\include" -I"C:\Users\admin\Documents\Autonomous-car\lpc1758_freertos_Distance_sensors\L1_FreeRTOS\portable" -I"C:\Users\admin\Documents\Autonomous-car\lpc1758_freertos_Distance_sensors\L1_FreeRTOS\portable\no_mpu" -I"C:\Users\admin\Documents\Autonomous-car\lpc1758_freertos_Distance_sensors\L2_Drivers" -I"C:\Users\admin\Documents\Autonomous-car\lpc1758_freertos_Distance_sensors\L2_Drivers\base" -I"C:\Users\admin\Documents\Autonomous-car\lpc1758_freertos_Distance_sensors\L3_Utils" -I"C:\Users\admin\Documents\Autonomous-car\lpc1758_freertos_Distance_sensors\L3_Utils\tlm" -I"C:\Users\admin\Documents\Autonomous-car\lpc1758_freertos_Distance_sensors\L4_IO" -I"C:\Users\admin\Documents\Autonomous-car\lpc1758_freertos_Distance_sensors\L4_IO\fat" -I"C:\Users\admin\Documents\Autonomous-car\lpc1758_freertos_Distance_sensors\L4_IO\wireless" -I"C:\Users\admin\Documents\Autonomous-car\lpc1758_freertos_Distance_sensors\L5_Application" -std=gnu++11 -fabi-version=0 -fno-exceptions -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


