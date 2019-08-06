cmake_minimum_required(VERSION 2.8)

project(libsfxplus)

include_directories(include)

set (Main_SRC
    # Sources
    src/sfxplus.cpp
    
    # Headers
    include/sfxplus.h
)

add_library(libsfxplus STATIC ${Main_SRC})
