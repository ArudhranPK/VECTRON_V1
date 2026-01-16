################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/drivers/SERVO/SERVO.c 

OBJS += \
./Core/Src/drivers/SERVO/SERVO.o 

C_DEPS += \
./Core/Src/drivers/SERVO/SERVO.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/drivers/SERVO/%.o Core/Src/drivers/SERVO/%.su Core/Src/drivers/SERVO/%.cyclo: ../Core/Src/drivers/SERVO/%.c Core/Src/drivers/SERVO/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F407xx -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../FATFS/Target -I../FATFS/App -I../USB_DEVICE/App -I../USB_DEVICE/Target -I../Middlewares/Third_Party/FatFs/src -I../Middlewares/ST/STM32_USB_Device_Library/Core/Inc -I../Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-Src-2f-drivers-2f-SERVO

clean-Core-2f-Src-2f-drivers-2f-SERVO:
	-$(RM) ./Core/Src/drivers/SERVO/SERVO.cyclo ./Core/Src/drivers/SERVO/SERVO.d ./Core/Src/drivers/SERVO/SERVO.o ./Core/Src/drivers/SERVO/SERVO.su

.PHONY: clean-Core-2f-Src-2f-drivers-2f-SERVO

