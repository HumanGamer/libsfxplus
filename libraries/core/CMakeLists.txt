cmake_minimum_required(VERSION 2.8.3)

if (MSVC)
    #Use solution folders. 
    set_property(GLOBAL PROPERTY USE_FOLDERS ON)
endif()

project(libsfxplus_core)

find_package(OpenAL REQUIRED)
include_directories(${OPENAL_INCLUDE_DIR})

include_directories(${CMAKE_CURRENT_BINARY_DIR} include)

set (Main_SRC
    src/init.cpp
    src/source.cpp
    src/stream.cpp
    src/listener.cpp
    src/audiofile.cpp
    src/io.cpp
    src/error.cpp
)

set (Main_PRIVATE_HDR
    include/core.h
    include/io.h
)

set (Main_PUBLIC_HDR
    ${CMAKE_SOURCE_DIR}/include/sfxplus/sfxplus.h
)

source_group("Library\\Sources" FILES ${Main_SRC})
source_group("Library\\Headers" FILES ${Main_PRIVATE_HDR})
source_group("Public Headers" FILES ${Main_PUBLIC_HDR})

set (Main_ALLSRC
    ${Main_SRC}
    ${Main_PRIVATE_HDR}
    ${Main_PUBLIC_HDR}
)

if (SHARED_LIBRARY)
    set(SFXPLUS_SHARED ON)
    set(LIBRARY_TYPE SHARED)
    add_definitions(-DSFXPLUS_SHARED_EXPORT)
else()
    set(LIBRARY_TYPE STATIC)
endif()

add_library(libsfxplus_core ${LIBRARY_TYPE} ${Main_ALLSRC})
add_dependencies(libsfxplus_core libsfxplus_io)
target_link_libraries(libsfxplus_core libsfxplus_io -lpthread ${OPENAL_LIBRARY})

set_target_properties(libsfxplus_core PROPERTIES PREFIX "")
if (MSVC)
    set_target_properties(libsfxplus_core PROPERTIES FOLDER "Library")
endif()

install(TARGETS libsfxplus_core
        RUNTIME DESTINATION ${BIN_DESTINATION}
        LIBRARY DESTINATION ${LIB_DESTINATION}
        ARCHIVE DESTINATION ${LIB_DESTINATION}
)

