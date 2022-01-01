add_subdirectory(imgui)

add_executable(glmmd)

target_sources(glmmd
PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/glad.c
    ${CMAKE_CURRENT_SOURCE_DIR}/Application.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/pmx/PmxModel.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/utils/CodeConverter.cpp
    
    ${CMAKE_CURRENT_SOURCE_DIR}/opengl/VertexBufferObject.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/opengl/IndexBufferObject.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/opengl/VertexArrayObject.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/opengl/Shader.cpp

)

target_include_directories(glmmd
PRIVATE 
    ${PROJECT_SOURCE_DIR}/deps/glad/include
    ${PROJECT_SOURCE_DIR}/deps/glfw/include
    ${PROJECT_SOURCE_DIR}/deps/glm
    ${PROJECT_SOURCE_DIR}/include
)

target_link_libraries(glmmd
PRIVATE 
    glfw
    imgui
)