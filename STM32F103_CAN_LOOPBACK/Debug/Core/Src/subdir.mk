################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/it_app.c \
../Core/Src/main_app.c \
../Core/Src/msp_app.c \
../Core/Src/system_stm32f1xx.c 

OBJS += \
./Core/Src/it_app.o \
./Core/Src/main_app.o \
./Core/Src/msp_app.o \
./Core/Src/system_stm32f1xx.o 

C_DEPS += \
./Core/Src/it_app.d \
./Core/Src/main_app.d \
./Core/Src/msp_app.d \
./Core/Src/system_stm32f1xx.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/%.o Core/Src/%.su Core/Src/%.cyclo: ../Core/Src/%.c Core/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F103xB -c -I../Core/Inc -I../Drivers/STM32F1xx_HAL_Driver/Inc/Legacy -I../Drivers/STM32F1xx_HAL_Driver/Inc -I../Drivers/CMSIS/Device/ST/STM32F1xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-Core-2f-Src

clean-Core-2f-Src:
	-$(RM) ./Core/Src/it_app.cyclo ./Core/Src/it_app.d ./Core/Src/it_app.o ./Core/Src/it_app.su ./Core/Src/main_app.cyclo ./Core/Src/main_app.d ./Core/Src/main_app.o ./Core/Src/main_app.su ./Core/Src/msp_app.cyclo ./Core/Src/msp_app.d ./Core/Src/msp_app.o ./Core/Src/msp_app.su ./Core/Src/system_stm32f1xx.cyclo ./Core/Src/system_stm32f1xx.d ./Core/Src/system_stm32f1xx.o ./Core/Src/system_stm32f1xx.su

.PHONY: clean-Core-2f-Src

