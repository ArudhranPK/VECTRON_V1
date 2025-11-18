################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/drivers/LIS2MDL/LIS2MDL.c 

OBJS += \
./Core/Src/drivers/LIS2MDL/LIS2MDL.o 

C_DEPS += \
./Core/Src/drivers/LIS2MDL/LIS2MDL.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/drivers/LIS2MDL/%.o Core/Src/drivers/LIS2MDL/%.su Core/Src/drivers/LIS2MDL/%.cyclo: ../Core/Src/drivers/LIS2MDL/%.c Core/Src/drivers/LIS2MDL/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F407xx -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../FATFS/Target -I../FATFS/App -I../Middlewares/Third_Party/FatFs/src -I../USB_DEVICE/App -I../USB_DEVICE/Target -I../Middlewares/ST/STM32_USB_Device_Library/Core/Inc -I../Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-Src-2f-drivers-2f-LIS2MDL

clean-Core-2f-Src-2f-drivers-2f-LIS2MDL:
	-$(RM) ./Core/Src/drivers/LIS2MDL/LIS2MDL.cyclo ./Core/Src/drivers/LIS2MDL/LIS2MDL.d ./Core/Src/drivers/LIS2MDL/LIS2MDL.o ./Core/Src/drivers/LIS2MDL/LIS2MDL.su

.PHONY: clean-Core-2f-Src-2f-drivers-2f-LIS2MDL

