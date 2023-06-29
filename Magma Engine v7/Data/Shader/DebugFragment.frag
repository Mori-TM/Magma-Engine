#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 FragColor;

layout (location = 0) out vec4 OutPosition;
layout (location = 1) out vec4 OutViewNormal;
layout (location = 2) out vec4 OutAlbedo;
layout (location = 3) out vec4 OutPBR;
layout (location = 4) out vec4 OutWorldPos;
layout (location = 5) out float OutNormal;

void main()
{
	OutPosition = vec4(1000.0);
	OutViewNormal = vec4(0.0);
	OutAlbedo = vec4(FragColor, 1.0);
	OutPBR = vec4(0.0, 1.0, 1.0, 1.0);
	OutWorldPos = vec4(0.0);
	OutNormal = 0.0;
}