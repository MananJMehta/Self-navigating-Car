################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../L3_Utils/tlm/src/c_tlm_binary.c \
../L3_Utils/tlm/src/c_tlm_comp.c \
../L3_Utils/tlm/src/c_tlm_stream.c \
../L3_Utils/tlm/src/c_tlm_var.c 

OBJS += \
./L3_Utils/tlm/src/c_tlm_binary.o \
./L3_Utils/tlm/src/c_tlm_comp.o \
./L3_Utils/tlm/src/c_tlm_stream.o \
./L3_Utils/tlm/src/c_tlm_var.o 

C_DEPS += \
./L3_Utils/tlm/src/c_tlm_binary.d \
./L3_Utils/tlm/src/c_tlm_comp.d \
./L3_Utils/tlm/src/c_tlm_stream.d \
./L3_Utils/tlm/src/c_tlm_var.d 


# Each subdirectory must supply rules for building sources it contributes
L3_Utils/tlm/src/%.o: ../L3_Utils/tlm/src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m3 -mthumb -Os -fmessage-length=0 -ffunction-sections -fdata-sections -Wall -Wshadow -Wlogical-op -Wfloat-equal -DBUILD_CFG_MPU=0 -I"C:\Autonomous-car\lpc1758_freertos_Bluetooth" -I"C:\Autonomous-car\lpc1758_freertos_Bluetooth\newlib" -I"C:\Autonomous-car\lpc1758_freertos_Bluetooth\L0_LowLevel" -I"C:\Autonomous-car\lpc1758_freertos_Bluetooth\L1_FreeRTOS" -I"C:\Autonomous-car\lpc1758_freertos_Bluetooth\L1_FreeRTOS\include" -I"C:\Autonomous-car\lpc1758_freertos_Bluetooth\L1_FreeRTOS\portable\no_mpu" -I"C:\Autonomous-car\lpc1758_freertos_Bluetooth\L2_Drivers" -I"C:\Autonomous-car\lpc1758_freertos_Bluetooth\L2_Drivers\base" -I"C:\Autonomous-car\lpc1758_freertos_Bluetooth\L3_Utils" -I"C:\Autonomous-car\lpc1758_freertos_Bluetooth\L3_Utils\tlm" -I"C:\Autonomous-car\lpc1758_freertos_Bluetooth\L4_IO" -I"C:\Autonomous-car\lpc1758_freertos_Bluetooth\L4_IO\fat" -I"C:\Autonomous-car\lpc1758_freertos_Bluetooth\L4_IO\wireless" -I"C:\Autonomous-car\lpc1758_freertos_Bluetooth\L5_Application" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


