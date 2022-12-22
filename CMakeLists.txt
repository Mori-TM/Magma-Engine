cmake_minimum_required (VERSION 3.8)

project(Magma)

find_package(Lua REQUIRED)

set(SOURCE_FILES
    Magma/main.cpp
)

set(HEADER_FILES
    Magma/Editor/AssetBrowser.h
    Magma/Editor/Camera.h
    Magma/Editor/Editor.h
    Magma/Editor/EntityManager.h
    Magma/Editor/Helper.h
    Magma/Editor/Inspector.h
    Magma/Editor/SceneManager.h
    Magma/Editor/ScriptEngine.h
    Magma/Renderer/Helper/Buffers.h
    Magma/Renderer/Helper/DescriptorPool.h
    Magma/Renderer/Helper/DescriptorSetLayouts.h
    Magma/Renderer/Helper/Helper.h
    Magma/Renderer/Helper/ImGuiHelper.h
    Magma/Renderer/Helper/Meshes.h
    Magma/Renderer/Helper/Sampler.h
    Magma/Renderer/Pipelines/BlurHelper.h
    Magma/Renderer/Pipelines/BlurPipeline.h
    Magma/Renderer/Pipelines/DebugHelper.h
    Magma/Renderer/Pipelines/DebugPipeline.h
    Magma/Renderer/Pipelines/SceneHelper.h
    Magma/Renderer/Pipelines/ScenePipeline.h
    Magma/Renderer/Pipelines/ShadowHelper.h
    Magma/Renderer/Pipelines/ShadowPipeline.h
    Magma/Renderer/Pipelines/SwapChainHelper.h
    Magma/Renderer/Pipelines/SwapChainPipeline.h
    Magma/Renderer/Renderer.h
    Magma/Helper.h
)

include_directories(/usr/local/include ${LUA_INCLUDE_DIR} libs/ImGui/src)

add_executable(Magma ${SOURCE_FILES} ${HEADER_FILES})

add_subdirectory(libs/ImGui)

target_link_libraries(Magma ${LUA_LIBRARIES})