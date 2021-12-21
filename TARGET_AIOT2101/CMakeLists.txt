add_library(mbed-aiot2101 INTERFACE)

target_sources(mbed-aiot2101
    INTERFACE
)

target_include_directories(mbed-aiot2101
    INTERFACE
        .
)

target_link_libraries(mbed-aiot2101 INTERFACE mbed-m480)
