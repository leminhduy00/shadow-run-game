include sources/platform/stm32h/Drivers/STM32H7xx_HAL_Driver/Makefile.mk
include sources/platform/stm32h/Drivers/CMSIS/Makefile.mk
include sources/platform/stm32h/Core/Makefile.mk

LDFILE = sources/platform/stm32h/stm32h743XX_FLASH.ld

CFLAGS += -I./sources/platform/stm32h

CPPFLAGS += -I./sources/platform/stm32h

VPATH += sources/platform/stm32h

SOURCES += $(wildcard sources/platform/stm32h/*.c)

# C++ source files
SOURCES_CPP += sources/platform/stm32h/mini_cpp.cpp

# ASM source files
SOURCES_ASM += sources/platform/stm32h/sys_ctrl.s
