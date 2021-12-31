add_executable(glmmd)

target_sources(glmmd
PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/glad.c
    ${CMAKE_CURRENT_SOURCE_DIR}/application.cpp
)

target_include_directories(glmmd
PRIVATE 
    ${PROJECT_SOURCE_DIR}/deps/glad/include
    ${PROJECT_SOURCE_DIR}/deps/glfw/include
    ${PROJECT_SOURCE_DIR}/deps/glm
)

target_link_libraries(glmmd
PRIVATE 
    glfw
)