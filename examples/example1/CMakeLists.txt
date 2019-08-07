cmake_minimum_required(VERSION 2.8)

project(example1)

include_directories(${CMAKE_BINARY_DIR} include)
include_directories(${CMAKE_SOURCE_DIR} include)

set (Main_SRC
    # Sources
    main.cpp
    
    # Headers
    # ...
)

add_executable(example1 ${Main_SRC})
target_link_libraries(example1 libsfxplus)

