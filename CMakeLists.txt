cmake_minimum_required(VERSION 3.4)
cmake_policy(SET CMP0048 NEW)

project(circularbuf-sample LANGUAGES C)

file(GLOB_RECURSE
        SOURCE_FILES
        sample.c
        circularbuf.c
        )

add_executable(${PROJECT_NAME} ${SOURCE_FILES})

include_directories(${CMAKE_SOURCE_DIR})
