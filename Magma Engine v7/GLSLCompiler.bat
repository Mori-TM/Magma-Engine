@echo off
glslc.exe Data/Shader/OffscreenVertex.vert -o Data/Shader/OffscreenVertex.spv

glslc.exe Data/Shader/SceneVertex.vert -o Data/Shader/SceneVertex.spv
glslc.exe Data/Shader/SceneFragment.frag -o Data/Shader/SceneFragment.spv

glslc.exe Data/Shader/ShadowVertex.vert -o Data/Shader/ShadowVertex.spv
glslc.exe Data/Shader/ShadowFragment.frag -o Data/Shader/ShadowFragment.spv

glslc.exe Data/Shader/DebugVertex.vert -o Data/Shader/DebugVertex.spv
glslc.exe Data/Shader/DebugFragment.frag -o Data/Shader/DebugFragment.spv

glslc.exe Data/Shader/GUIVertex.vert -o Data/Shader/GUIVertex.spv
glslc.exe Data/Shader/GUIFragment.frag -o Data/Shader/GUIFragment.spv

glslc.exe Data/Shader/GBufferVertex.vert -o Data/Shader/GBufferVertex.spv
glslc.exe Data/Shader/GBufferFragment.frag -o Data/Shader/GBufferFragment.spv

glslc.exe Data/Shader/SwapChainFragment.frag -o Data/Shader/SwapChainFragment.spv
glslc.exe Data/Shader/BlurFragment.frag -o Data/Shader/BlurFragment.spv
glslc.exe Data/Shader/SSRFragment.frag -o Data/Shader/SSRFragment.spv
glslc.exe Data/Shader/SSAOFragment.frag -o Data/Shader/SSAOFragment.spv
glslc.exe Data/Shader/SSAOBlurFragment.frag -o Data/Shader/SSAOBlurFragment.spv
glslc.exe Data/Shader/FXAAFragment.frag -o Data/Shader/FXAAFragment.spv

::Raytracin Shader
glslc.exe Data/Shader/Raytracing/Vertex.vert -o Data/Shader/Raytracing/Vertex.spv
glslc.exe Data/Shader/Raytracing/Fragment.frag -o Data/Shader/Raytracing/Fragment.spv

glslc.exe Data/Shader/Raytracing/closesthit.rchit --target-env=vulkan1.2 -o Data/Shader/Raytracing/closesthit.spv
glslc.exe Data/Shader/Raytracing/miss.rmiss --target-env=vulkan1.2 -o Data/Shader/Raytracing/miss.spv
glslc.exe Data/Shader/Raytracing/shadow.rmiss --target-env=vulkan1.2 -o Data/Shader/Raytracing/shadow.spv
glslc.exe Data/Shader/Raytracing/raygen.rgen --target-env=vulkan1.2 -o Data/Shader/Raytracing/raygen.spv
