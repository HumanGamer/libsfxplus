cmake_minimum_required(VERSION 2.8.3)

if (MSVC)
    #Use solution folders. 
    set_property(GLOBAL PROPERTY USE_FOLDERS ON)
endif()

project(playing)

include_directories(${CMAKE_BINARY_DIR} include)
include_directories(${CMAKE_SOURCE_DIR} include)

set (Main_SRC
    # Sources
    main.cpp
    
    # Headers
    # ...
)

add_executable(playing ${Main_SRC})
add_dependencies(playing libsfxplus_core)
target_link_libraries(playing libsfxplus_core)
if (MSVC)
    set_target_properties(playing PROPERTIES FOLDER "Examples")
endif()
