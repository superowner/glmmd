add_library(opengl_framework STATIC)

target_sources(opengl_framework
PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/VertexBufferObject.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/IndexBufferObject.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/VertexArrayObject.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/FrameBufferObject.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/RenderBufferObject.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Shader.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Texture2D.cpp
)

target_include_directories(opengl_framework
PRIVATE 
    ${PROJECT_SOURCE_DIR}/deps/glad/include
    ${PROJECT_SOURCE_DIR}/deps/glfw/include
    ${PROJECT_SOURCE_DIR}/include
    ${PROJECT_SOURCE_DIR}/include/3rdparty
)

target_link_libraries(opengl_framework
PRIVATE 
    glfw
)