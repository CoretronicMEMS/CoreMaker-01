# Copyright (c) 2021 ARM Limited. All rights reserved.
# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.19.0)

set(MBED_PATH ${CMAKE_CURRENT_SOURCE_DIR}/mbed-os CACHE INTERNAL "")
set(MBED_CONFIG_PATH ${CMAKE_CURRENT_BINARY_DIR} CACHE INTERNAL "")
set(APP_TARGET AIOT_2101)

include(${MBED_PATH}/tools/cmake/app.cmake)

project(${APP_TARGET})

add_compile_options(-Wno-unused-variable)
add_subdirectory(TARGET_AIOT2101)
add_subdirectory(${MBED_PATH})

add_executable(${APP_TARGET})

include(${MBED_CONFIG_PATH}/mbed_config.cmake)

mbed_configure_app_target(${APP_TARGET})

target_include_directories(${APP_TARGET}
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/
        ${CMAKE_CURRENT_SOURCE_DIR}/nvt_rs485/
        ${CMAKE_CURRENT_SOURCE_DIR}/sensors/
        ${CMAKE_CURRENT_SOURCE_DIR}/sensors/BME680
        ${CMAKE_CURRENT_SOURCE_DIR}/sensors/KX122-1037
        ${CMAKE_CURRENT_SOURCE_DIR}/sd_card
)

target_sources(${APP_TARGET}
    PRIVATE
        main.cpp
        DebounceIn.cpp
        lightEffect.hpp
        sensors/ADS131E.cpp
        sensors/GMC306.cpp
        sensors/AcousticNode.cpp
        sensors/SensorHub.cpp
        sensors/BME680/mbed_bme680.cpp
        sensors/BME680/bme680.cpp
        sensors/KX122-1037/KX122.cpp
        sd_card/NuSDBlockDevice.cpp
)

target_link_libraries(${APP_TARGET}
    PRIVATE
        mbed-os
        mbed-usb
        mbed-netsocket
        mbed-storage
)

mbed_set_post_build(${APP_TARGET})

option(VERBOSE_BUILD "Have a verbose build process")
if(VERBOSE_BUILD)
    set(CMAKE_VERBOSE_MAKEFILE ON)
endif()
