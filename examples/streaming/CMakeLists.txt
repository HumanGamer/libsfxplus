cmake_minimum_required(VERSION 2.8.3)

if (MSVC)
    #Use solution folders. 
    set_property(GLOBAL PROPERTY USE_FOLDERS ON)
endif()

project(streaming)

include_directories(${CMAKE_BINARY_DIR} include)
include_directories(${CMAKE_SOURCE_DIR} include)

set (Main_SRC
    # Sources
    main.cpp
    
    # Headers
    # ...
)

add_executable(streaming ${Main_SRC})
target_link_libraries(streaming libsfxplus_core)
if (MSVC)
    set_target_properties(streaming PROPERTIES FOLDER "Examples")
endif()
