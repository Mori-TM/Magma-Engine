#version 450
#extension GL_ARB_separate_shader_objects : enable

layout (location = 0) out vec4 OutPosition;
layout (location = 1) out vec4 OutViewNormal;
layout (location = 2) out vec4 OutAlbedo;
layout (location = 3) out vec4 OutPBR;
//layout (location = 4) out vec4 OutWorldPos;

layout(location = 0) in vec3 FragViewNormal;
layout(location = 1) in vec3 FragNormal;
layout(location = 2) in vec2 FragTexCoord;
layout(location = 3) in vec4 FragPosRelToCam;
layout(location = 4) in vec4 FragWorldPos;

layout(set = 1, binding = 0) uniform sampler2D AlbedoMap;
layout(set = 2, binding = 0) uniform sampler2D NormalMap;
layout(set = 3, binding = 0) uniform sampler2D MetallicMap;

layout(set = 4, binding = 0) uniform sampler2D RoughnessMap;
layout(set = 5, binding = 0) uniform sampler2D OcclusionMap;


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
	
	/*
	 float near = PushConst.NearPlane;
    float far  = PushConst.FarPlane;
    return (near * far) / (far - Depth * (far - near));
	*/
}

vec3 GetNormalFromMap()
{
	vec3 Normal = texture(NormalMap, FragTexCoord).xyz;
	vec3 N = normalize(FragNormal);

	if (Normal.x >= 0.99999 &&	Normal.y >= 0.99999 &&	Normal.z >= 0.99999)
		return N;

	vec3 tangentNormal = Normal * 2.0 - 1.0;

	vec3 Q1 = dFdx(FragWorldPos.xyz);
	vec3 Q2 = dFdy(FragWorldPos.xyz);
	vec2 st1 = dFdx(FragTexCoord);
	vec2 st2 = dFdy(FragTexCoord);

	vec3 T = normalize(Q1*st2.t - Q2*st1.t);
	vec3 B = -normalize(cross(N, T));
	mat3 TBN = mat3(T, B, N);

	return normalize(TBN * tangentNormal);
	
}

/*

Normal packing as described in:
A Survey of Efficient Representations for Independent Unit Vectors
Source: http://jcgt.org/published/0003/02/01/paper.pdf

*/

// For each component of v, returns -1 if the component is < 0, else 1
vec2 sign_not_zero(vec2 v) {
	// Branch-Less version
	return fma(step(vec2(0.0), v), vec2(2.0), vec2(-1.0));
}

// Packs a 3-component normal to 2 channels using octahedron normals
vec2 pack_normal_octahedron(vec3 v) {
   
        // Faster version using newer GLSL capatibilities
	v.xy /= dot(abs(v), vec3(1));


	// Branch-Less version
	return mix(v.xy, (1.0 - abs(v.yx)) * sign_not_zero(v.xy), step(v.z, 0.0));


}

void main() 
{
	OutAlbedo = texture(AlbedoMap, FragTexCoord) * PushConst.Color;
	//	OutAlbedo.xyz = PushConst.Color.xyz * PushConst.Color.w +  PushConst.Color.xyz * (1.0 - PushConst.Color.w);
	//	OutAlbedo.w = 1.0;
	if (OutAlbedo.w < 0.9)
		discard;

	vec2 NormalPack = pack_normal_octahedron(GetNormalFromMap());//normalize(FragNormal);

	OutPosition = vec4(FragWorldPos.xyz, LinearDepth(gl_FragCoord.z));

	OutViewNormal = vec4(normalize(FragViewNormal), NormalPack.x);				//use GetNormalFromMap
//	float PrevAlpha = OutAlbedo.w;
	
//	OutPBR.r = 
	OutPBR.r = texture(MetallicMap, FragTexCoord).r * PushConst.Metallic;
	OutPBR.g = texture(RoughnessMap, FragTexCoord).r * PushConst.Roughness;
	OutPBR.b = texture(OcclusionMap, FragTexCoord).r * PushConst.Occlusion;
	OutPBR.a = NormalPack.y;

//	OutPBR.g = PushConst.Roughness;
//	OutPBR.b = PushConst.Occlusion;

	
//	OutWorldPos.x = FragWorldPos.x;
//	OutWorldPos.y = FragWorldPos.y;
//	OutWorldPos.z = FragWorldPos.z;
//	OutWorldPos.w = ;
}