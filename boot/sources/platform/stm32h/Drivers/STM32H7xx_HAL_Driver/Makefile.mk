CFLAGS += -I./sources/platform/stm32h/Drivers/STM32H7xx_HAL_Driver/Inc
CPPFLAGS += -I./sources/platform/stm32h/Drivers/STM32H7xx_HAL_Driver/Inc

VPATH += sources/platform/stm32h/Drivers/STM32H7xx_HAL_Driver/Src

# C source files
SOURCES += $(wildcard sources/platform/stm32h/Drivers/STM32H7xx_HAL_Driver/Src/*.c)
