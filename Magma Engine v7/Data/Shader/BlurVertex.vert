#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec2 InPosition;
layout(location = 1) in vec2 InTexCoord;

layout(location = 0) out vec2 FragTexCoord;

void main()
{
	gl_Position = vec4(InPosition, 0.0, 1.0);
	FragTexCoord = InTexCoord;
}