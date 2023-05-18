#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 FragColor;

layout (location = 0) out vec4 OutPosition;
layout (location = 1) out vec4 OutNormal;
layout (location = 2) out vec4 OutAlbedo;
layout (location = 3) out vec4 OutPBR;

void main()
{
	OutAlbedo = vec4(FragColor, 1.0);
	OutPosition = vec4(gl_FragCoord.z);
	OutNormal = vec4(0.0);
	OutPBR = vec4(0.0);
}