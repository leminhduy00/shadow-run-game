include sources/libraries/arduino/cores/stm32/Makefile.mk

# header path
CPPFLAGS	+= -I./sources/libraries/arduino/cores

# sources path
VPATH += sources/libraries/arduino/cores

# cpp files
SOURCES_CPP += sources/libraries/arduino/cores/wiring_digital.cpp
SOURCES_CPP += sources/libraries/arduino/cores/wiring_shift.cpp
SOURCES_CPP += sources/libraries/arduino/cores/Print.cpp
SOURCES_CPP += sources/libraries/arduino/cores/Stream.cpp
SOURCES_CPP += sources/libraries/arduino/cores/WString.cpp
SOURCES_CPP += sources/libraries/arduino/cores/WMath.cpp
SOURCES_CPP += sources/libraries/arduino/cores/itoa.cpp
SOURCES_CPP += sources/libraries/arduino/cores/IPAddress.cpp
# SOURCES_CPP += sources/libraries/arduino/cores/HardwareSerial.cpp
# SOURCES_CPP += sources/libraries/arduino/cores/HardwareSerial2.cpp
