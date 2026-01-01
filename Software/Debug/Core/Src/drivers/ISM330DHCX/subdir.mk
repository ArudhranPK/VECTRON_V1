################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/drivers/ISM330DHCX/ISM330DHCX.c 

OBJS += \
./Core/Src/drivers/ISM330DHCX/ISM330DHCX.o 

C_DEPS += \
./Core/Src/drivers/ISM330DHCX/ISM330DHCX.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/drivers/ISM330DHCX/%.o Core/Src/drivers/ISM330DHCX/%.su Core/Src/drivers/ISM330DHCX/%.cyclo: ../Core/Src/drivers/ISM330DHCX/%.c Core/Src/drivers/ISM330DHCX/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F407xx -c -I../FATFS/Target -I../FATFS/App -I../USB_DEVICE/App -I../USB_DEVICE/Target -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Middlewares/Third_Party/FatFs/src -I../Middlewares/ST/STM32_USB_Device_Library/Core/Inc -I../Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-Src-2f-drivers-2f-ISM330DHCX

clean-Core-2f-Src-2f-drivers-2f-ISM330DHCX:
	-$(RM) ./Core/Src/drivers/ISM330DHCX/ISM330DHCX.cyclo ./Core/Src/drivers/ISM330DHCX/ISM330DHCX.d ./Core/Src/drivers/ISM330DHCX/ISM330DHCX.o ./Core/Src/drivers/ISM330DHCX/ISM330DHCX.su

.PHONY: clean-Core-2f-Src-2f-drivers-2f-ISM330DHCX

