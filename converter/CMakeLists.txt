cmake_minimum_required(VERSION 3.15)

set(PROJECT_NAME "converter")

# set compiler
set(CMAKE_C_COMPILER "gcc" CACHE PATH "" FORCE)

# set flags for compilation
set(CMAKE_C_FLAGS "")

# target specific flags
set(CMAKE_C_FLAGS_DEBUG "-Og")
set(CMAKE_C_FLAGS_RELEASE "-O3")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall")

# general project settings
project(${PROJECT_NAME} C)
set(CMAKE_C_STANDARD 99)

# enable export of compile commands
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

message("Generating Target: ${CMAKE_BUILD_TYPE}")



# --- get all the needed files together ---
# get all c files
file(GLOB APP_SOURCES "${CMAKE_SOURCE_DIR}/*.c")



# --- compile uC application and strip some stuff ---
# add executable from collected sources
add_executable(converter ${APP_SOURCES})
find_library(CMATH_LIB libmath.so)

target_link_libraries(converter
    PRIVATE
    m
)
