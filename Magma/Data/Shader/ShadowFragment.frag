#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec2 FragTexCoord;

layout(set = 0, binding = 0) uniform sampler2D Texture;

void main()
{
	gl_FragDepth = gl_FragCoord.z;

	if (texture(Texture, FragTexCoord).a < 0.9)
		discard;
}