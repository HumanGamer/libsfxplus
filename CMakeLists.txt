cmake_minimum_required(VERSION 2.8.3)

if (MSVC)
    #Use solution folders. 
    set_property(GLOBAL PROPERTY USE_FOLDERS ON)
endif()

# Add path for custom modules
set(CMAKE_MODULE_PATH
    ${CMAKE_MODULE_PATH}
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake"
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules"
)

project(libsfxplus)

option(SHARED_LIBRARY "Build shared library" OFF)
option(BUILD_EXAMPLES "Build examples" ON)

include_directories(${CMAKE_CURRENT_BINARY_DIR} include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR} include)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/config/sfxplus_config.h.in ${CMAKE_CURRENT_BINARY_DIR}/sfxplus_config.h @ONLY)

set (CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

set(BIN_DESTINATION "bin")
set(LIB_DESTINATION "lib")

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/sfxplus_config.h DESTINATION include)
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/ DESTINATION include)

add_subdirectory(libraries)

if (BUILD_EXAMPLES)
add_subdirectory(examples)
endif()
