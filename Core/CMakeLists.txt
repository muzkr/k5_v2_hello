# ----------------------------
# FreeRTOS library
#

add_library(FreeRTOS INTERFACE)

target_include_directories(FreeRTOS SYSTEM INTERFACE
    ${CMAKE_SOURCE_DIR}/Middlewares/FreeRTOS/Source/include
    ${CMAKE_SOURCE_DIR}/Middlewares/FreeRTOS/Source/portable/GCC/ARM_CM0
)

target_sources(FreeRTOS INTERFACE
    ${CMAKE_SOURCE_DIR}/Middlewares/FreeRTOS/Source/croutine.c
    ${CMAKE_SOURCE_DIR}/Middlewares/FreeRTOS/Source/event_groups.c
    ${CMAKE_SOURCE_DIR}/Middlewares/FreeRTOS/Source/list.c
    ${CMAKE_SOURCE_DIR}/Middlewares/FreeRTOS/Source/queue.c
    ${CMAKE_SOURCE_DIR}/Middlewares/FreeRTOS/Source/stream_buffer.c
    ${CMAKE_SOURCE_DIR}/Middlewares/FreeRTOS/Source/tasks.c
    ${CMAKE_SOURCE_DIR}/Middlewares/FreeRTOS/Source/timers.c
    ${CMAKE_SOURCE_DIR}/Middlewares/FreeRTOS/Source/portable/GCC/ARM_CM0/port.c
)

# ----------------------------
# PY32_Drivers library
#

add_library(PY32_Drivers INTERFACE)

target_include_directories(PY32_Drivers SYSTEM INTERFACE
    ${CMAKE_SOURCE_DIR}/Drivers/PY32F0xx_HAL_Driver/Inc
    ${CMAKE_SOURCE_DIR}/Drivers/CMSIS/Device/PY32F0xx/Include
    ${CMAKE_SOURCE_DIR}/Drivers/CMSIS/Include
)

target_sources(PY32_Drivers INTERFACE
    Src/system_py32f0xx.c
    ${CMAKE_SOURCE_DIR}/Drivers/PY32F0xx_HAL_Driver/Src/py32f0xx_ll_utils.c
    ${CMAKE_SOURCE_DIR}/Drivers/PY32F0xx_HAL_Driver/Src/py32f0xx_ll_exti.c
    ${CMAKE_SOURCE_DIR}/Drivers/PY32F0xx_HAL_Driver/Src/py32f0xx_ll_gpio.c
    ${CMAKE_SOURCE_DIR}/Drivers/PY32F0xx_HAL_Driver/Src/py32f0xx_ll_adc.c
    ${CMAKE_SOURCE_DIR}/Drivers/PY32F0xx_HAL_Driver/Src/py32f0xx_ll_dma.c
    ${CMAKE_SOURCE_DIR}/Drivers/PY32F0xx_HAL_Driver/Src/py32f0xx_ll_rcc.c
    ${CMAKE_SOURCE_DIR}/Drivers/PY32F0xx_HAL_Driver/Src/py32f0xx_ll_crc.c
    ${CMAKE_SOURCE_DIR}/Drivers/PY32F0xx_HAL_Driver/Src/py32f0xx_ll_usart.c
)

# ---------------------------
# Core library  
#

add_library(Core INTERFACE)

target_include_directories(Core INTERFACE Inc)
target_compile_definitions(Core INTERFACE
	USE_FULL_LL_DRIVER 
	PY32F030x8 
    $<$<CONFIG:Debug>:DEBUG>
)

target_sources(Core INTERFACE
    Src/core_main.c
    Src/py32f0xx_it.c
    Src/app_freertos.c
    # Src/sysmem.c
    # Src/syscalls.c
    Src/startup_py32f030xx.s
)

target_link_libraries(Core INTERFACE FreeRTOS PY32_Drivers ${TOOLCHAIN_LINK_LIBRARIES})
