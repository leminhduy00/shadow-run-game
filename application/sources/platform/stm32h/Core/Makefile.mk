CFLAGS += -I./sources/platform/stm32h/Core/Inc
CPPFLAGS += -I./sources/platform/stm32h/Core/Inc

VPATH += sources/platform/stm32h/Core/Src

# C source files
SOURCES += $(wildcard sources/platform/stm32h/Core/Src/*.c)
