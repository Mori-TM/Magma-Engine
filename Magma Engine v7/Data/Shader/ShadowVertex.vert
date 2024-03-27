#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec4 InPositionTexX;
layout(location = 1) in vec4 InNormalTexY;

#define InPosition	InPositionTexX.xyz
#define InTexCoord	vec2(InPositionTexX.w, InNormalTexY.w)

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