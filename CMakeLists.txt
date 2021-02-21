cmake_minimum_required(VERSION 3.15)

set(PROJECT_NAME "prog")

# set compiler
set(CMAKE_C_COMPILER "arm-none-eabi-gcc" CACHE PATH "" FORCE)

# set flags for compilation
set(CMAKE_C_FLAGS "-mcpu=cortex-m3 -march=armv7-m -mthumb -mfloat-abi=soft")

# dont use libc because my link script is not compatible (yet TODO)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -nostdlib")

# target specific flags
set(CMAKE_C_FLAGS_DEBUG "-Og -g -gdwarf-3 -gstrict-dwarf")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall")

# general project settings
project(${PROJECT_NAME} C)
set(CMAKE_C_STANDARD 99)

# enable export of compile commands
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

message("Generating Target: ${CMAKE_BUILD_TYPE}")



# --- get all the needed files together ---
# get all header files
include_directories("CMSIS_CORE" "CMSIS_DEV" "include" "lib/include")

# get all c files
file(GLOB APP_SOURCES "*.c")
file(GLOB LIB_SOURCES "src/*.c")
file(GLOB CORE_SOURCES "CMSIS_CORE/*.c")
file(GLOB STM_SOURCES "CMSIS_DEV/*.c")



# --- setup cmake for crosscompiling ---
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR arm)
set(CMAKE_CROSSCOMPILING 1)

# Try to get around the c compiler check
set(CMAKE_TRY_COMPILE_TARGET_TYPE "STATIC_LIBRARY")



# --- set compile options and compile lib ---
# select chip specific stuff
add_compile_definitions(STM32F10X_MD)
add_compile_definitions(HSE_VALUE=8000000)

add_library(display ${LIB_SOURCES} ${CORE_SOURCES} ${STM_SOURCES})



# --- compile uC application and strip some stuff ---
# set linker flags
set(CMAKE_EXE_LINKER_FLAGS "--entry handler_reset -Wl,-T\"${CMAKE_CURRENT_SOURCE_DIR}/startup.s\"")

# objcopy elf to bin
set(CMAKE_C_OBJCOPY arm-none-eabi-objcopy CACHE PATH "" FORCE)

add_custom_target(firmware.bin ALL DEPENDS firmware.elf)
add_custom_command(TARGET firmware.bin
    COMMAND ${CMAKE_C_OBJCOPY} ARGS -O binary firmware.elf firmware.bin)

# add executable from collected sources
add_executable(firmware.elf ${APP_SOURCES} ${CORE_SOURCES} ${STM_SOURCES} "${CMAKE_CURRENT_SOURCE_DIR}/startup.c")

add_dependencies(firmware.elf display)

target_link_libraries(firmware.elf
    PRIVATE
    display
)


# --- custom target for flashing .bin file ---
# add custom target to flash firmware to uC using st-flash
add_custom_target(flash DEPENDS firmware.bin)
add_custom_command(TARGET flash
    USES_TERMINAL
    COMMAND st-flash write firmware.bin 0x08000000)



