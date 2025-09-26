cmake_minimum_required(VERSION 3.22)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
endif()

message("Build type: " ${CMAKE_BUILD_TYPE})

set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)
enable_language(C ASM)

set(CMAKE_PROJECT_NAME k5_v2_hello)
project(${CMAKE_PROJECT_NAME})

add_subdirectory(Core)
add_subdirectory(Utilities)

# ------------------------------------
#  Executable target
#

add_executable(${CMAKE_PROJECT_NAME})
add_subdirectory(App)

# Remove wrong libob.a library dependency when using cpp files
list(REMOVE_ITEM CMAKE_C_IMPLICIT_LINK_LIBRARIES ob)

# Add the map file to the list of files to be removed with 'clean' target
set_target_properties(${CMAKE_PROJECT_NAME} PROPERTIES ADDITIONAL_CLEAN_FILES ${CMAKE_PROJECT_NAME}.map)

# -----------------------------------
#  Generate .hex and .bin files
#

set(TARGET_ELF "${CMAKE_BINARY_DIR}/${CMAKE_PROJECT_NAME}.elf")

# Generate .bin file
add_custom_command(
    TARGET ${CMAKE_PROJECT_NAME}
    POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -O binary ${TARGET_ELF} ${CMAKE_BINARY_DIR}/${CMAKE_PROJECT_NAME}.bin
    COMMENT "Generating .bin file"
)

# Generate .hex file
add_custom_command(
    TARGET ${CMAKE_PROJECT_NAME}
    POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -O ihex ${TARGET_ELF} ${CMAKE_BINARY_DIR}/${CMAKE_PROJECT_NAME}.hex
    COMMENT "Generating .hex file"
)
