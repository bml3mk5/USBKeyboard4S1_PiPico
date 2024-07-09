cmake_minimum_required(VERSION 3.12)

# Activate below lines if you use Tinyusb library on local directory
#add_definitions(-DUSE_LOCAL_TINYUSB)
#set(PICO_TINYUSB_PATH ${CMAKE_CURRENT_SOURCE_DIR}/tinyusb)
# Debug Level in Tinyusb
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(LOG 2)
endif()

# Pull in SDK (must be before project)
include(pico_sdk_import.cmake)

project(USBKeyboard4S1 C CXX ASM)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

if (PICO_SDK_VERSION_STRING VERSION_LESS "1.3.0")
    message(FATAL_ERROR "Raspberry Pi Pico SDK version 1.3.0 (or later) required. Your version is ${PICO_SDK_VERSION_STRING}")
endif()

# Initialize the SDK
pico_sdk_init()

add_compile_options(-Wall
        -Wno-format          # int != int32_t as far as the compiler is concerned because gcc has int32_t as long int
        -Wno-unused-function # we have some for the docs that aren't called
        )
if (CMAKE_C_COMPILER_ID STREQUAL "GNU")
    add_compile_options(-Wno-maybe-uninitialized)
endif()

set(CMAKE_C_FLAGS_DEBUG "-g -O0 -DDEBUG")
set(CMAKE_C_FLAGS_RELEASE "-O2 -DNDEBUG")

set(SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src)

add_executable(USBKeyboard4S1
    ${SOURCE_DIR}/main.c
    ${SOURCE_DIR}/hid_app.c
    ${SOURCE_DIR}/s1_led.c
)

# Add pico_stdlib library which aggregates commonly used features
target_link_libraries(USBKeyboard4S1 pico_stdlib tinyusb_host tinyusb_board)

# Add local include files
target_include_directories(USBKeyboard4S1 PUBLIC ${SOURCE_DIR})

pico_enable_stdio_usb(USBKeyboard4S1 0)
pico_enable_stdio_uart(USBKeyboard4S1 1)

# create map/bin/hex/uf2 file in addition to ELF.
pico_add_extra_outputs(USBKeyboard4S1)
