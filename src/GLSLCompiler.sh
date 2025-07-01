glslc Data/Shader/OffscreenVertex.vert -o Data/Shader/OffscreenVertex.spv

glslc Data/Shader/DepthPrePassVertex.vert -o Data/Shader/DepthPrePassVertex.spv
glslc Data/Shader/DepthPrePassFragment.frag -o Data/Shader/DepthPrePassFragment.spv

glslc Data/Shader/SceneVertex.vert -o Data/Shader/SceneVertex.spv
glslc Data/Shader/SceneFragment.frag -o Data/Shader/SceneFragment.spv

glslc Data/Shader/ShadowVertex.vert -o Data/Shader/ShadowVertex.spv
glslc Data/Shader/ShadowFragment.frag -o Data/Shader/ShadowFragment.spv

glslc Data/Shader/DebugVertex.vert -o Data/Shader/DebugVertex.spv
glslc Data/Shader/DebugFragment.frag -o Data/Shader/DebugFragment.spv

glslc Data/Shader/GUIVertex.vert -o Data/Shader/GUIVertex.spv
glslc Data/Shader/GUIFragment.frag -o Data/Shader/GUIFragment.spv

glslc Data/Shader/GBufferVertex.vert -o Data/Shader/GBufferVertex.spv
glslc Data/Shader/GBufferFragment.frag -o Data/Shader/GBufferFragment.spv

glslc Data/Shader/SwapChainFragment.frag -o Data/Shader/SwapChainFragment.spv
glslc Data/Shader/BlurFragment.frag -o Data/Shader/BlurFragment.spv
glslc Data/Shader/SSRFragment.frag -o Data/Shader/SSRFragment.spv
glslc Data/Shader/SSAOFragment.frag -o Data/Shader/SSAOFragment.spv
glslc Data/Shader/SSAOBlurFragment.frag -o Data/Shader/SSAOBlurFragment.spv
glslc Data/Shader/FXAAFragment.frag -o Data/Shader/FXAAFragment.spv

glslc Data/Shader/ForwardVertex.vert -o Data/Shader/ForwardVertex.spv
glslc Data/Shader/ForwardFragment.frag -o Data/Shader/ForwardFragment.spv

glslc Data/Shader/Raytracing/Vertex.vert -o Data/Shader/Raytracing/Vertex.spv
glslc Data/Shader/Raytracing/Fragment.frag -o Data/Shader/Raytracing/Fragment.spv

glslc Data/Shader/Raytracing/closesthit.rchit --target-env=vulkan1.2 -o Data/Shader/Raytracing/closesthit.spv
glslc Data/Shader/Raytracing/miss.rmiss --target-env=vulkan1.2 -o Data/Shader/Raytracing/miss.spv
glslc Data/Shader/Raytracing/shadow.rmiss --target-env=vulkan1.2 -o Data/Shader/Raytracing/shadow.spv
glslc Data/Shader/Raytracing/raygen.rgen --target-env=vulkan1.2 -o Data/Shader/Raytracing/raygen.spv
