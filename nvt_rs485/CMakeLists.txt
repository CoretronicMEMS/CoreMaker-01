
target_include_directories(${APP_TARGET}
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_sources(${APP_TARGET}
    PRIVATE
        MyUnbufferedSerial.cpp
        nvt_rs485.cpp
)
