#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec2 FragTexCoord;

layout(set = 0, binding = 0) uniform sampler2D Texture;

#define MAX_MIP 2

void main()
{
	gl_FragDepth = gl_FragCoord.z;

	float MipLevel = textureQueryLod(Texture, FragTexCoord).x;

	if (textureLod(Texture, FragTexCoord, MipLevel < MAX_MIP ? MAX_MIP : MipLevel).a < 0.9)
		discard;
}