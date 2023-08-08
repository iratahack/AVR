# I target a recent cmake, it shouldn't be a problem on a dev machine
cmake_minimum_required(VERSION 3.11)
# Project name
project("FreeRTOS")

# Product filename
set(PRODUCT_NAME FreeRTOS)

## AVR Chip Configuration
# 8Mhz, this should match the crystal on your board,
# I use 8Mhz and 3.3V for the lowest power consumption
set(F_CPU 16000000UL)
# CPU, you can find the list here:
# https://gcc.gnu.org/onlinedocs/gcc/AVR-Options.html
set(MCU atmega328p)
# Default Baudrate for UART, read avr include/util/setbaud.h for usage
set(BAUD 2000000)
# The programmer to use, read avrdude manual for list
set(PROG_TYPE Arduino)
# FreeRTOS base directory
set(FREE_RTOS FreeRTOS/FreeRTOS)
# Serial port used for programming
set(SERIAL /dev/ttyUSB0)

# AVR Fuses, must be in concordance with your hardware and F_CPU
# http://eleccelerator.com/fusecalc/fusecalc.php?chip=atmega328p
#set(E_FUSE 0xfd)
#set(H_FUSE 0xda)
#set(L_FUSE 0xfd)
#set(LOCK_BIT 0xff)


# Use AVR GCC toolchain
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_CXX_COMPILER avr-g++)
set(CMAKE_C_COMPILER avr-gcc)
set(CMAKE_ASM_COMPILER avr-gcc)


# Pass defines to compiler
add_definitions(
    -DF_CPU=${F_CPU}
    -DBAUD=${BAUD}
)
# mmcu MUST be passed to both the compiler and linker, this handle the linker
set(CMAKE_EXE_LINKER_FLAGS -mmcu=${MCU})

add_compile_options(
    -mmcu=${MCU} # MCU
    -Og # optimize
    -Wall # enable warnings
    -I${PROJECT_SOURCE_DIR}/${FREE_RTOS}/Source/include
    -I${PROJECT_SOURCE_DIR}
    -I${PROJECT_SOURCE_DIR}/${FREE_RTOS}/Source/portable/ThirdParty/GCC/ATmega
)

set(SRC_FILES
    ${PROJECT_SOURCE_DIR}/${FREE_RTOS}/Source/croutine.c
    ${PROJECT_SOURCE_DIR}/${FREE_RTOS}/Source/event_groups.c
    ${PROJECT_SOURCE_DIR}/${FREE_RTOS}/Source/list.c
    ${PROJECT_SOURCE_DIR}/${FREE_RTOS}/Source/queue.c
    ${PROJECT_SOURCE_DIR}/${FREE_RTOS}/Source/stream_buffer.c
    ${PROJECT_SOURCE_DIR}/${FREE_RTOS}/Source/tasks.c
    ${PROJECT_SOURCE_DIR}/${FREE_RTOS}/Source/timers.c
    ${PROJECT_SOURCE_DIR}/${FREE_RTOS}/Source/portable/ThirdParty/GCC/ATmega/port.c
    ${PROJECT_SOURCE_DIR}/${FREE_RTOS}/Source/portable/MemMang/heap_4.c
    ${PROJECT_SOURCE_DIR}/main.c
    ${PROJECT_SOURCE_DIR}/uart.c
)

# Create one target
add_executable(${PRODUCT_NAME} ${SRC_FILES})

# Rename the output to .elf as we will create multiple files
set_target_properties(${PRODUCT_NAME} PROPERTIES OUTPUT_NAME ${PRODUCT_NAME}.elf)

# Strip binary for upload
add_custom_target(strip avr-strip ${PRODUCT_NAME}.elf DEPENDS ${PRODUCT_NAME})

# Transform binary into hex file, we ignore the eeprom segments in the step
add_custom_target(hex avr-objcopy -R .eeprom -O ihex ${PRODUCT_NAME}.elf ${PRODUCT_NAME}.hex DEPENDS strip)
# Transform binary into hex file, this is the eeprom part (empty if you don't
# use eeprom static variables)
add_custom_target(eeprom avr-objcopy -j .eeprom  --set-section-flags=.eeprom="alloc,load"  --change-section-lma .eeprom=0 -O ihex ${PRODUCT_NAME}.elf ${PRODUCT_NAME}.eep DEPENDS strip)

# Upload the firmware with avrdude
add_custom_target(upload sudo avrdude  -c ${PROG_TYPE} -p ${MCU} -P ${SERIAL} -B 115200 -U flash:w:${PRODUCT_NAME}.hex DEPENDS hex)

# Upload the eeprom with avrdude
add_custom_target(upload_eeprom avrdude -c ${PROG_TYPE} -p ${MCU}  -U eeprom:w:${PRODUCT_NAME}.eep DEPENDS eeprom)

# Burn fuses
add_custom_target(fuses avrdude -c ${PROG_TYPE} -p ${MCU}  -U lfuse:w:${L_FUSE}:m -U hfuse:w:${H_FUSE}:m -U efuse:w:${E_FUSE}:m -U lock:w:${LOCK_BIT}:m )

# Clean extra files
set_directory_properties(PROPERTIES ADDITIONAL_MAKE_CLEAN_FILES "${PRODUCT_NAME}.hex;${PRODUCT_NAME}.eeprom;${PRODUCT_NAME}.lst")

