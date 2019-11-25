cmake_minimum_required(VERSION 2.8.3)

if (MSVC)
    #Use solution folders. 
    set_property(GLOBAL PROPERTY USE_FOLDERS ON)
endif()

project(libsfxplus_io)

find_package(LibSndFile REQUIRED)
include_directories(${LIBSNDFILE_INCLUDE_DIRS})

include_directories(${CMAKE_SOURCE_DIR}/deps/minimp3)

include_directories(${CMAKE_CURRENT_BINARY_DIR} include)

set (Main_SRC
    src/io.cpp
    src/mp3.cpp
)

set (Main_PRIVATE_HDR
    include/io.h
    include/mp3.h
)

set (Main_PUBLIC_HDR
    ${CMAKE_SOURCE_DIR}/include/sfxplus/sfxplusio.h
)

source_group("Library\\Sources" FILES ${Main_SRC})
source_group("Library\\Headers" FILES ${Main_PRIVATE_HDR})
source_group("Public Headers" FILES ${Main_PUBLIC_HDR})

set (Main_ALLSRC
    ${Main_SRC}
    ${Main_PRIVATE_HDR}
    ${Main_PUBLIC_HDR}
)

add_library(libsfxplus_io ${LIBRARY_TYPE} ${Main_ALLSRC})
target_link_libraries(libsfxplus_io ${LIBSNDFILE_LIBRARIES})

set_target_properties(libsfxplus_io PROPERTIES PREFIX "")
if (MSVC)
    set_target_properties(libsfxplus_io PROPERTIES FOLDER "Library")
endif()

install(TARGETS libsfxplus_io
        RUNTIME DESTINATION ${BIN_DESTINATION}
        LIBRARY DESTINATION ${LIB_DESTINATION}
        ARCHIVE DESTINATION ${LIB_DESTINATION}
)

