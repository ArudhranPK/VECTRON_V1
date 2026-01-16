################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/drivers/ADC/ADC.c 

OBJS += \
./Core/Src/drivers/ADC/ADC.o 

C_DEPS += \
./Core/Src/drivers/ADC/ADC.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/drivers/ADC/%.o Core/Src/drivers/ADC/%.su Core/Src/drivers/ADC/%.cyclo: ../Core/Src/drivers/ADC/%.c Core/Src/drivers/ADC/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F407xx -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../FATFS/Target -I../FATFS/App -I../USB_DEVICE/App -I../USB_DEVICE/Target -I../Middlewares/Third_Party/FatFs/src -I../Middlewares/ST/STM32_USB_Device_Library/Core/Inc -I../Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-Src-2f-drivers-2f-ADC

clean-Core-2f-Src-2f-drivers-2f-ADC:
	-$(RM) ./Core/Src/drivers/ADC/ADC.cyclo ./Core/Src/drivers/ADC/ADC.d ./Core/Src/drivers/ADC/ADC.o ./Core/Src/drivers/ADC/ADC.su

.PHONY: clean-Core-2f-Src-2f-drivers-2f-ADC

