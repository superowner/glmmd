add_subdirectory(3rdparty)
add_subdirectory(opengl)

add_executable(glmmd)

target_sources(glmmd
PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/glad.c
    ${CMAKE_CURRENT_SOURCE_DIR}/Application.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/pmx/PmxModel.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/pmx/PmxModelRenderer.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/engine/ObjectBase.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/engine/Camera.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/engine/Scene.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/engine/Event.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/engine/OffscreenRenderer.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/utils/CodeConverter.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/utils/GlobalConfig.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/utils/Logger.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/animator/VmdData.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/animator/PmxBoneAnimator.cpp

)

target_include_directories(glmmd
PRIVATE 
    ${PROJECT_SOURCE_DIR}/deps/glad/include
    ${PROJECT_SOURCE_DIR}/deps/glfw/include
    ${PROJECT_SOURCE_DIR}/deps/spdlog/include
    ${PROJECT_SOURCE_DIR}/include
    ${PROJECT_SOURCE_DIR}/include/3rdparty
)

find_package(OpenGL REQUIRED)

target_link_libraries(glmmd
PRIVATE 
    glfw
    imgui
    opengl_framework
    spdlog::spdlog
    OpenGL::GL
)
