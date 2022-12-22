#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 InPosition;
layout(location = 1) in vec2 InTexCoord;

layout(push_constant) uniform PushConstants
{
	layout(offset = 0) mat4 PVM;
} PushConst;

layout(location = 0) out vec2 FragTexCoord;

void main()
{
	gl_Position = PushConst.PVM * vec4(InPosition, 1.0);
	FragTexCoord = InTexCoord;
}