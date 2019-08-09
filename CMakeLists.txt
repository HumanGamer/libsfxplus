cmake_minimum_required(VERSION 2.8.3)

if (MSVC)
    #Use solution folders. 
    set_property(GLOBAL PROPERTY USE_FOLDERS ON)
endif()

project(libsfxplus)

# Add path for custom modules
set(CMAKE_MODULE_PATH
    ${CMAKE_MODULE_PATH}
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake"
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules"
)

option(SHARED_LIBRARY "Build shared library" OFF)
option(BUILD_EXAMPLES "Build examples" ON)

find_package(OpenAL REQUIRED)
include_directories(${OPENAL_INCLUDE_DIR})

find_package(LibSndFile REQUIRED)
include_directories(${LIBSNDFILE_INCLUDE_DIRS})

include_directories(${CMAKE_CURRENT_BINARY_DIR} include)

if (SHARED_LIBRARY)
    set(SFXPLUS_SHARED ON)
    set(LIBRARY_TYPE SHARED)
    add_definitions(-DSFXPLUS_SHARED_EXPORT)
else()
    set(LIBRARY_TYPE STATIC)
endif()

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/config/sfxplus_config.h.in ${CMAKE_CURRENT_BINARY_DIR}/sfxplus_config.h @ONLY)

set (Main_SRC
    src/init.cpp
    src/source.cpp
    src/stream.cpp
    src/loader.cpp
    src/error.cpp
)

set (Main_PRIVATE_HDR
    src/core.h
)

set (Main_PUBLIC_HDR
    include/sfxplus.h
)

source_group("Library\\Sources" FILES ${Main_SRC})
source_group("Library\\Headers" FILES ${Main_PRIVATE_HDR})
source_group("Public Headers" FILES ${Main_PUBLIC_HDR})

set (Main_ALLSRC
    ${Main_SRC}
    ${Main_PRIVATE_HDR}
    ${Main_PUBLIC_HDR}
)

add_library(libsfxplus ${LIBRARY_TYPE} ${Main_ALLSRC})
target_link_libraries(libsfxplus -lpthread ${OPENAL_LIBRARY} ${LIBSNDFILE_LIBRARIES})

set_target_properties(libsfxplus PROPERTIES PREFIX "")
if (MSVC)
    set_target_properties(libsfxplus PROPERTIES FOLDER "Library")
endif()

set (CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

set(BIN_DESTINATION "bin")
set(LIB_DESTINATION "lib")

install(TARGETS libsfxplus
        RUNTIME DESTINATION ${BIN_DESTINATION}
        LIBRARY DESTINATION ${LIB_DESTINATION}
        ARCHIVE DESTINATION ${LIB_DESTINATION}
)

install(DIRECTORY ${CMAKE_SOURCE_DIR}/include/ DESTINATION include)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/sfxplus_config.h DESTINATION include)

if (BUILD_EXAMPLES)
add_subdirectory(examples)
endif()
