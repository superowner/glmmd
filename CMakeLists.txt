cmake_minimum_required(VERSION 3.2)

project(glmmd)

SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
set(CMAKE_CXX_STANDARD 17)
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "" FORCE)
endif()
message(STATUS "Build type: " ${CMAKE_BUILD_TYPE})

add_subdirectory(deps)
add_subdirectory(src)