################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/it_app.c \
../Core/Src/main_app.c \
../Core/Src/msp_app.c \
../Core/Src/system_stm32f4xx.c 

OBJS += \
./Core/Src/it_app.o \
./Core/Src/main_app.o \
./Core/Src/msp_app.o \
./Core/Src/system_stm32f4xx.o 

C_DEPS += \
./Core/Src/it_app.d \
./Core/Src/main_app.d \
./Core/Src/msp_app.d \
./Core/Src/system_stm32f4xx.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/%.o Core/Src/%.su Core/Src/%.cyclo: ../Core/Src/%.c Core/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F401xC -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -Wextra -u _printf_float -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-Src

clean-Core-2f-Src:
	-$(RM) ./Core/Src/it_app.cyclo ./Core/Src/it_app.d ./Core/Src/it_app.o ./Core/Src/it_app.su ./Core/Src/main_app.cyclo ./Core/Src/main_app.d ./Core/Src/main_app.o ./Core/Src/main_app.su ./Core/Src/msp_app.cyclo ./Core/Src/msp_app.d ./Core/Src/msp_app.o ./Core/Src/msp_app.su ./Core/Src/system_stm32f4xx.cyclo ./Core/Src/system_stm32f4xx.d ./Core/Src/system_stm32f4xx.o ./Core/Src/system_stm32f4xx.su

.PHONY: clean-Core-2f-Src

