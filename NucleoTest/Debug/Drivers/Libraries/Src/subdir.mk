################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Drivers/Libraries/Src/ssd1306.c \
../Drivers/Libraries/Src/ssd1306_fonts.c \
../Drivers/Libraries/Src/ssd1306_tests.c 

OBJS += \
./Drivers/Libraries/Src/ssd1306.o \
./Drivers/Libraries/Src/ssd1306_fonts.o \
./Drivers/Libraries/Src/ssd1306_tests.o 

C_DEPS += \
./Drivers/Libraries/Src/ssd1306.d \
./Drivers/Libraries/Src/ssd1306_fonts.d \
./Drivers/Libraries/Src/ssd1306_tests.d 


# Each subdirectory must supply rules for building sources it contributes
Drivers/Libraries/Src/%.o Drivers/Libraries/Src/%.su Drivers/Libraries/Src/%.cyclo: ../Drivers/Libraries/Src/%.c Drivers/Libraries/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m7 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F746xx -c -I../Core/Inc -I../Drivers/Libraries/Inc -I../Drivers/STM32F7xx_HAL_Driver/Inc -I../Drivers/STM32F7xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F7xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Drivers-2f-Libraries-2f-Src

clean-Drivers-2f-Libraries-2f-Src:
	-$(RM) ./Drivers/Libraries/Src/ssd1306.cyclo ./Drivers/Libraries/Src/ssd1306.d ./Drivers/Libraries/Src/ssd1306.o ./Drivers/Libraries/Src/ssd1306.su ./Drivers/Libraries/Src/ssd1306_fonts.cyclo ./Drivers/Libraries/Src/ssd1306_fonts.d ./Drivers/Libraries/Src/ssd1306_fonts.o ./Drivers/Libraries/Src/ssd1306_fonts.su ./Drivers/Libraries/Src/ssd1306_tests.cyclo ./Drivers/Libraries/Src/ssd1306_tests.d ./Drivers/Libraries/Src/ssd1306_tests.o ./Drivers/Libraries/Src/ssd1306_tests.su

.PHONY: clean-Drivers-2f-Libraries-2f-Src

