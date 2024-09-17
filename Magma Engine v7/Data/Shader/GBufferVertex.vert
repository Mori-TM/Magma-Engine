#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec4 InPositionTexX;
layout(location = 1) in vec4 InNormalTexY;

#define InPosition	InPositionTexX.xyz
#define InTexCoord	vec2(InPositionTexX.w, InNormalTexY.w)
#define InNormal	InNormalTexY.xyz

layout(location = 0) out vec3 FragViewNormal;
layout(location = 1) out vec3 FragNormal;
layout(location = 2) out vec2 FragTexCoord;
layout(location = 3) out vec4 FragPosRelToCam;
layout(location = 4) out vec4 FragWorldPos;

layout(push_constant) uniform PushConstants
{
	layout(offset = 0) mat4 Model;
} PushConst;

layout(set = 5, binding = 0) uniform UniformBufferObject
{
	mat4 View;
	mat4 Projection;
} UBO;

void main() 
{
    vec4 WorldPos = PushConst.Model * vec4(InPosition, 1.0);
	vec4 PosRelToCam = UBO.View * WorldPos;

	gl_Position = UBO.Projection * PosRelToCam;

	FragViewNormal = transpose(inverse(mat3(UBO.View * PushConst.Model))) * InNormal;
	FragNormal = transpose(inverse(mat3(PushConst.Model))) * InNormal;
	FragTexCoord = InTexCoord;

	FragPosRelToCam = PosRelToCam;
	FragWorldPos = WorldPos;
}