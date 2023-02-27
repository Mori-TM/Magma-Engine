#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 FragColor;

layout(location = 0) out vec4 OutDepthPosition;
layout(location = 1) out vec4 OutNormalMap;
layout(location = 2) out vec4 OutPBRMap;
layout(location = 3) out vec4 OutLightPass;

void main()
{
	OutLightPass = vec4(FragColor, 1.0);
	OutDepthPosition = vec4(gl_FragCoord.z);
	OutNormalMap = vec4(0.0);
	OutPBRMap = vec4(0.0);
}