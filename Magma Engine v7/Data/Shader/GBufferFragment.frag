#version 450
#extension GL_ARB_separate_shader_objects : enable

layout (location = 0) out vec4 OutPosition;
layout (location = 1) out vec4 OutViewNormal;
layout (location = 2) out vec4 OutAlbedo;
layout (location = 3) out vec4 OutPBR;
layout (location = 4) out vec4 OutWorldPos;
layout (location = 5) out float OutNormal;

layout(location = 0) in vec3 FragViewNormal;
layout(location = 1) in vec3 FragNormal;
layout(location = 2) in vec2 FragTexCoord;
layout(location = 3) in vec4 FragPosRelToCam;
layout(location = 4) in vec4 FragWorldPos;

layout(set = 0, binding = 0) uniform sampler2D AlbedoMap;
layout(set = 1, binding = 0) uniform sampler2D NormalMap;
layout(set = 2, binding = 0) uniform sampler2D MetallicMap;
layout(set = 3, binding = 0) uniform sampler2D RoughnessMap;
layout(set = 4, binding = 0) uniform sampler2D OcclusionMap;

layout(push_constant) uniform PushConstants
{
	layout(offset = 64) vec4 Color;
	layout(offset = 80) float Metallic;
	layout(offset = 84) float Roughness;
	layout(offset = 88) float Occlusion;
	layout(offset = 92) float NearPlane;
	layout(offset = 96) float FarPlane;
} PushConst;

float LinearDepth(float Depth)
{
	float z = Depth * 2.0f - 1.0f; 
	return (2.0f * PushConst.NearPlane * PushConst.FarPlane) / (PushConst.FarPlane + PushConst.NearPlane - z * (PushConst.FarPlane - PushConst.NearPlane));	
}

vec3 GetNormalFromMap()
{
	vec3 Normal = texture(NormalMap, FragTexCoord).xyz;

	if (Normal.x == 1.0 &&	Normal.y == 1.0 &&	Normal.z == 1.0)
		return normalize(FragNormal);

	vec3 tangentNormal = Normal * 2.0 - 1.0;

	vec3 Q1 = dFdx(FragWorldPos.xyz);
	vec3 Q2 = dFdy(FragWorldPos.xyz);
	vec2 st1 = dFdx(FragTexCoord);
	vec2 st2 = dFdy(FragTexCoord);

	vec3 N = normalize(FragNormal);
	vec3 T = normalize(Q1*st2.t - Q2*st1.t);
	vec3 B = -normalize(cross(N, T));
	mat3 TBN = mat3(T, B, N);

	return normalize(TBN * tangentNormal);
	
}

void main() 
{
	vec3 Normal = GetNormalFromMap();//normalize(FragNormal);
	OutPosition = vec4(FragPosRelToCam.xyz, LinearDepth(gl_FragCoord.z));
	OutViewNormal = vec4(normalize(FragViewNormal), Normal.x);				//use GetNormalFromMap
	OutAlbedo = texture(AlbedoMap, FragTexCoord) * PushConst.Color;
	OutPBR.r = texture(MetallicMap, FragTexCoord).r * PushConst.Metallic;
	OutPBR.g = texture(RoughnessMap, FragTexCoord).r * PushConst.Roughness;
	OutPBR.b = texture(OcclusionMap, FragTexCoord).r * PushConst.Occlusion;
	OutPBR.a = Normal.y;
	OutWorldPos.x = FragWorldPos.x;
	OutWorldPos.y = FragWorldPos.y;
	OutWorldPos.z = FragWorldPos.z;
	OutWorldPos.w = FragWorldPos.w;
	OutNormal = Normal.z;
}