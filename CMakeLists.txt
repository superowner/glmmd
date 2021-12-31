cmake_minimum_required(VERSION 3.2)

project(glmmd)

SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

add_subdirectory(deps)
add_subdirectory(src)