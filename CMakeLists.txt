cmake_minimum_required(VERSION 2.8)

project(libsfxplus)

# Add path for custom modules
set(CMAKE_MODULE_PATH
    ${CMAKE_MODULE_PATH}
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake"
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules"
)

find_package(OpenAL REQUIRED)
include_directories(${OPENAL_INCLUDE_DIR})

find_package(LibSndFile REQUIRED)
include_directories(${LIBSNDFILE_INCLUDE_DIRS})

include_directories(include)

set (Main_SRC
    # Sources
    src/sfxplus.cpp
    
    # Headers
    include/sfxplus.h
)

add_library(libsfxplus STATIC ${Main_SRC})
target_link_libraries(libsfxplus ${OPENAL_LIBRARY} ${LIBSNDFILE_LIBRARIES})
