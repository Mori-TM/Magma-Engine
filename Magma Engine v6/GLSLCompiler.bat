@echo off
glslc.exe Data/Shader/SceneVertex.vert -o Data/Shader/SceneVertex.spv
glslc.exe Data/Shader/SceneFragment.frag -o Data/Shader/SceneFragment.spv

glslc.exe Data/Shader/SwapChainVertex.vert -o Data/Shader/SwapChainVertex.spv
glslc.exe Data/Shader/SwapChainFragment.frag -o Data/Shader/SwapChainFragment.spv

glslc.exe Data/Shader/ShadowVertex.vert -o Data/Shader/ShadowVertex.spv
glslc.exe Data/Shader/ShadowFragment.frag -o Data/Shader/ShadowFragment.spv

glslc.exe Data/Shader/DebugVertex.vert -o Data/Shader/DebugVertex.spv
glslc.exe Data/Shader/DebugFragment.frag -o Data/Shader/DebugFragment.spv

glslc.exe Data/Shader/BlurVertex.vert -o Data/Shader/BlurVertex.spv
glslc.exe Data/Shader/BlurFragment.frag -o Data/Shader/BlurFragment.spv

glslc.exe Data/Shader/GUIVertex.vert -o Data/Shader/GUIVertex.spv
glslc.exe Data/Shader/GUIFragment.frag -o Data/Shader/GUIFragment.spv