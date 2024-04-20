################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (11.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Hardwware/V8.c \
../Hardwware/si7021_SKIN.c 

OBJS += \
./Hardwware/V8.o \
./Hardwware/si7021_SKIN.o 

C_DEPS += \
./Hardwware/V8.d \
./Hardwware/si7021_SKIN.d 


# Each subdirectory must supply rules for building sources it contributes
Hardwware/%.o Hardwware/%.su Hardwware/%.cyclo: ../Hardwware/%.c Hardwware/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F411xE -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I"S:/STM32/CUBEIDE/si7021_stm32f411/Hardwware" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Hardwware

clean-Hardwware:
	-$(RM) ./Hardwware/V8.cyclo ./Hardwware/V8.d ./Hardwware/V8.o ./Hardwware/V8.su ./Hardwware/si7021_SKIN.cyclo ./Hardwware/si7021_SKIN.d ./Hardwware/si7021_SKIN.o ./Hardwware/si7021_SKIN.su

.PHONY: clean-Hardwware

