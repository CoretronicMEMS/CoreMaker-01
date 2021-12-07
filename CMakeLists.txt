# Copyright (c) 2021 ARM Limited. All rights reserved.
# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.19.0)

set(MBED_PATH ${CMAKE_CURRENT_SOURCE_DIR}/mbed-os CACHE INTERNAL "")
set(MBED_CONFIG_PATH ${CMAKE_CURRENT_BINARY_DIR} CACHE INTERNAL "")
set(APP_TARGET AIOT_2101)

include(${MBED_PATH}/tools/cmake/app.cmake)

project(${APP_TARGET})

add_compile_options(-Wno-unused-variable)
add_subdirectory(${MBED_PATH})

add_executable(${APP_TARGET})

include(${MBED_CONFIG_PATH}/mbed_config.cmake)



target_include_directories(${APP_TARGET}
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/
        ${CMAKE_CURRENT_SOURCE_DIR}/nvt_rs485/
        ${CMAKE_CURRENT_SOURCE_DIR}/sensors/
)

target_sources(${APP_TARGET}
    PRIVATE
        main.cpp
        sensors/ADS131E.cpp
        sensors/AcousticNode.cpp
        sensors/SensorHub.cpp
)

target_link_libraries(${APP_TARGET}
    PRIVATE
        mbed-os
        mbed-netsocket
)

mbed_set_post_build(${APP_TARGET})

option(VERBOSE_BUILD "Have a verbose build process")
if(VERBOSE_BUILD)
    set(CMAKE_VERBOSE_MAKEFILE ON)
endif()
