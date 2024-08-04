cmake_minimum_required(VERSION 3.13)

# initialize the SDK directly
include($ENV{PICO_SDK_PATH}/external/pico_sdk_import.cmake)

# set(PICO_BOARD pico_w)
set(PICO_BOARD pico)

project(soil_humidity_led C CXX ASM)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# initialize the Raspberry Pi Pico SDK
pico_sdk_init()

add_executable(${CMAKE_PROJECT_NAME}
    main.c
)

pico_enable_stdio_usb(${CMAKE_PROJECT_NAME} 1)
pico_enable_stdio_uart(${CMAKE_PROJECT_NAME} 0)

# target_compile_definitions(picow_tls_client_background PRIVATE
#         WIFI_SSID=\"${WIFI_SSID}\"
#         WIFI_PASSWORD=\"${WIFI_PASSWORD}\"
#         )

target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE 
    ${CMAKE_CURRENT_LIST_DIR} 
    ${CMAKE_CURRENT_LIST_DIR}/.. 
    
)

target_link_libraries(${CMAKE_PROJECT_NAME}
    pico_stdlib
    hardware_i2c
    hardware_adc
    ${CMAKE_SOURCE_DIR}/rust/target/thumbv6m-none-eabi/release/librust_interop.a
)

pico_add_extra_outputs(${CMAKE_PROJECT_NAME})
