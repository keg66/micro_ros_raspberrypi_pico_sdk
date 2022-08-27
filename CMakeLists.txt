cmake_minimum_required(VERSION 3.12)

include($ENV{PICO_SDK_PATH}/external/pico_sdk_import.cmake)

project(pico_micro_ros_example C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

pico_sdk_init()

include(ExternalProject)
set_directory_properties(PROPERTIES EP_PREFIX ${CMAKE_BINARY_DIR}/Rust)

ExternalProject_Add(
    rustlib
    DOWNLOAD_COMMAND ""
    CONFIGURE_COMMAND ""
    BUILD_COMMAND cargo build 
    #COMMAND cargo build --release
    SOURCE_DIR "${CMAKE_SOURCE_DIR}/rust_lib"
    BINARY_DIR "${CMAKE_SOURCE_DIR}/rust_lib/target"
    INSTALL_COMMAND ""
    LOG_MERGED_STDOUTERR ON
)

link_directories(libmicroros)
add_executable(pico_micro_ros_example
    src/pico_uart_transport.c
    pico_micro_ros_example.c
)

add_dependencies(pico_micro_ros_example rustlib)

target_link_libraries(pico_micro_ros_example
    pico_stdlib
    microros
    ${CMAKE_SOURCE_DIR}/rust_lib/target/thumbv6m-none-eabi/debug/librustlib.a
)

target_include_directories(pico_micro_ros_example PUBLIC
    include
    libmicroros/include
    rust_lib/include
)

SET(CMAKE_C_FLAGS  "${CMAKE_C_FLAGS} -ffunction-sections -fdata-sections")
SET(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -ffunction-sections -fdata-sections")

# Configure Pico
pico_enable_stdio_usb(pico_micro_ros_example 1)
pico_enable_stdio_uart(pico_micro_ros_example 0)
add_compile_definitions(PICO_UART_ENABLE_CRLF_SUPPORT=0)
add_compile_definitions(PICO_STDIO_ENABLE_CRLF_SUPPORT=0)
add_compile_definitions(PICO_STDIO_DEFAULT_CRLF=0)

# Generate UF2
pico_add_extra_outputs(pico_micro_ros_example)
