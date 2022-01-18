cmake_minimum_required(VERSION 3.2)

project(glmmd)

SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
set(CMAKE_CXX_STANDARD 17)

add_subdirectory(deps)
add_subdirectory(src)