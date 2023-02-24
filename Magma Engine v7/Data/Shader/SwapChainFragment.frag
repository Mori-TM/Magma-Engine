#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec2 FragTexCoord;

layout(set = 0, binding = 0) uniform sampler2D Sampler;

layout(location = 0) out vec4 OutColor;

void main()
{
	OutColor = texture(Sampler, FragTexCoord);
}