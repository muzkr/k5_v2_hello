
target_sources(${CMAKE_PROJECT_NAME} PRIVATE
    # main.c
    main_rtos.c
    serial_tx.c
    serial_msg.c
)

target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE
    .
)

# Symbols (macros)
target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE
)

target_link_libraries(${CMAKE_PROJECT_NAME}
    Core
    lwrb
)
