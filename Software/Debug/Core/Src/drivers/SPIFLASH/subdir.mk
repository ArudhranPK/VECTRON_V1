################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/drivers/SPIFLASH/SPIFLASH.c 

OBJS += \
./Core/Src/drivers/SPIFLASH/SPIFLASH.o 

C_DEPS += \
./Core/Src/drivers/SPIFLASH/SPIFLASH.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/drivers/SPIFLASH/%.o Core/Src/drivers/SPIFLASH/%.su Core/Src/drivers/SPIFLASH/%.cyclo: ../Core/Src/drivers/SPIFLASH/%.c Core/Src/drivers/SPIFLASH/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F407xx -c -I../FATFS/Target -I../FATFS/App -I../USB_DEVICE/App -I../USB_DEVICE/Target -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Middlewares/Third_Party/FatFs/src -I../Middlewares/ST/STM32_USB_Device_Library/Core/Inc -I../Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-Src-2f-drivers-2f-SPIFLASH

clean-Core-2f-Src-2f-drivers-2f-SPIFLASH:
	-$(RM) ./Core/Src/drivers/SPIFLASH/SPIFLASH.cyclo ./Core/Src/drivers/SPIFLASH/SPIFLASH.d ./Core/Src/drivers/SPIFLASH/SPIFLASH.o ./Core/Src/drivers/SPIFLASH/SPIFLASH.su

.PHONY: clean-Core-2f-Src-2f-drivers-2f-SPIFLASH

