cmake_minimum_required(VERSION 3.0.0)
project(metaserializationlibrary VERSION 0.1.0)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})

include(CTest)
enable_testing()

add_library(metaserializationlibrary STATIC
    src/temp_source_file.cpp
)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
