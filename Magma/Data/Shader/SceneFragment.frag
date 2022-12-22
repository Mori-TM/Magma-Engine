#version 450
#extension GL_ARB_separate_shader_objects : enable

#define SHADOW_MAP_CASCADE_COUNT 3

layout(location = 0) out vec4 OutColor;

layout(location = 0) in vec3 FragNormal;
layout(location = 1) in vec2 FragTexCoord;
layout(location = 2) in vec4 FragPosRelToCam;
layout(location = 3) in vec4 FragWorldPos;

#define SHADOW_MAP_SIZE 4096
layout(set = 0, binding = 0) uniform sampler2D AlbedoMap;
layout(set = 1, binding = 0) uniform sampler2D NormalMap;
layout(set = 2, binding = 0) uniform sampler2D MetallicMap;
layout(set = 3, binding = 0) uniform sampler2D RoughnessMap;
layout(set = 4, binding = 0) uniform sampler2D OcclusionMap;
layout(set = 5, binding = 0) uniform sampler2D ShadowMap;

layout(push_constant) uniform PushConstants
{
	layout(offset = 64) vec4 Color;
	layout(offset = 80) float Metallic;
	layout(offset = 84) float Roughness;
	layout(offset = 88) float Occlusion;
} PushConst;

layout(set = 6, binding = 0) uniform UniformBufferObject
{
	vec4 CascadeSplits;
	mat4 CascadeProjectionView[SHADOW_MAP_CASCADE_COUNT];
	vec4 LightDirection;
	vec4 CameraPosition;
} UBO;

//#define Ambient 0.6

const mat4 BiasMat = mat4
( 
	0.5, 0.0, 0.0, 0.0,
	0.0, 0.5, 0.0, 0.0,
	0.0, 0.0, 1.0, 0.0,
	0.5, 0.5, 0.0, 1.0 
);

float TextureProjection(vec4 ShadowCoord, vec2 Offset, uint CascadeIndex, float Ambient)
{
	float Shadow = 0.0;
	
	float Bias = 0.0;
	if (CascadeIndex == 0) 
		Bias = 0.002;
	else if (CascadeIndex == 1)
		Bias = 0.00085;
	else
		Bias = 0.0008;

	if (ShadowCoord.z > -1.0 && ShadowCoord.z < 1.0) 
	{
		float Dist = texture(ShadowMap, ShadowCoord.st + Offset).r;
		if (ShadowCoord.w > 0.0 && Dist < ShadowCoord.z - Bias) //0.003
		{
			Shadow = Ambient;
		}
	}

	return Shadow;
}

float FilterPCF(vec4 ShadowCoord, int Range, float Scale, uint CascadeIndex, float Ambient)
{
	ivec2 TextureDimension = ivec2(SHADOW_MAP_SIZE);

	float DX = Scale / float(TextureDimension.x);
	float DY = Scale / float(TextureDimension.y);

	float ShadowFactor = 0.0;
	int Count = 0;
//	int Range = 2;		

	for (int x = -Range; x <= Range; x++)
	{
		for (int y = -Range; y <= Range; y++)
		{
			ShadowFactor += TextureProjection(ShadowCoord, vec2(DX * x, DY * y), CascadeIndex, Ambient);
			Count++;
		}
	}

	return ShadowFactor / Count;
}

const float PI = 3.14159265359;

vec3 GetNormalFromMap()
{
	vec3 Normal = texture(NormalMap, FragTexCoord).xyz;

	if (Normal.x == 1.0 &&	Normal.y == 1.0 &&	Normal.z == 1.0)
		return FragNormal;

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

float DistributionGGX(vec3 N, vec3 H, float roughness)
{
	float a = roughness*roughness;
	float a2 = a*a;
	float NdotH = max(dot(N, H), 0.0);
	float NdotH2 = NdotH*NdotH;
	
	float num = a2;
	float denom = (NdotH2 * (a2 - 1.0) + 1.0);
	denom = PI * denom * denom;
	
	return num / denom;
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
	float r = (roughness + 1.0);
	float k = (r*r) / 8.0;

	float num = NdotV;
	float denom = NdotV * (1.0 - k) + k;
	
	return num / denom;
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
	float NdotV = max(dot(N, V), 0.0);
	float NdotL = max(dot(N, L), 0.0);
	float ggx2 = GeometrySchlickGGX(NdotV, roughness);
	float ggx1 = GeometrySchlickGGX(NdotL, roughness);
	
	return ggx1 * ggx2;
}

vec3 FresnelSchlick(float cosTheta, vec3 F0)
{
	return F0 + (1.0 - F0) * pow(max(1.0 - cosTheta, 0.0), 5.0);
}

float GetShadow(float Ambient)
{
	uint CascadeIndex = 0;
	for (uint i = 0; i < SHADOW_MAP_CASCADE_COUNT - 1; i++)
		if (FragPosRelToCam.z < UBO.CascadeSplits[i])
			CascadeIndex = i + 1;
			 
	vec4 ShadowCoord = (BiasMat * UBO.CascadeProjectionView[CascadeIndex]) * FragWorldPos;	

	ShadowCoord.x /= 3;
	ShadowCoord.x += (1.0 / 3) * float(CascadeIndex); 
//	ShadowCoord.x *= 2;
	
//	ShadowCoord.x += (float(SHADOW_MAP_WIDTH) * float(CascadeIndex));
//	ShadowCoord.s = float(CascadeIndex + 1);

	float Shadow = 0.0;
	if (CascadeIndex == 2)
		Shadow = TextureProjection(ShadowCoord / ShadowCoord.w, vec2(0.0), CascadeIndex, Ambient);
	else if (CascadeIndex == 1)
		Shadow = FilterPCF(ShadowCoord / ShadowCoord.w, 2, 0.6, CascadeIndex, Ambient);
	else
		Shadow = FilterPCF(ShadowCoord / ShadowCoord.w, 2, 1.0, CascadeIndex, Ambient);
	
//	Shadow = TextureProjection(ShadowCoord / ShadowCoord.w, vec2(0.0), CascadeIndex, Ambient);

//	float Shadow = FilterPCF(ShadowCoord / ShadowCoord.w, CascadeIndex, Ambient);
//	float Shadow = TextureProjection(ShadowCoord / ShadowCoord.w, vec2(0.0), CascadeIndex, Ambient);

	return Shadow;
}

vec3 Uncharted2Tonemap(vec3 x)
{
	float A = 0.15;
	float B = 0.50;
	float C = 0.10;
	float D = 0.20;
	float E = 0.02;
	float F = 0.30;
	return ((x*(A*x+C*B)+D*E)/(x*(A*x+B)+D*F))-E/F;
}

void main() 
{
	vec4 Color = PushConst.Color;
	if (Color.w == 0.0)	Color.w = 1.0;
	vec4 AlbedoTexture = texture(AlbedoMap, FragTexCoord);
	vec3 Albedo = pow(AlbedoTexture.rgb * Color.rgb, vec3(2.2));
//	vec3 Albedo = AlbedoTexture.rgb * Color.rgb;
	vec3 Normal = GetNormalFromMap();
	float Metallic = texture(MetallicMap, FragTexCoord).r * PushConst.Metallic;
	float Roughness = texture(RoughnessMap, FragTexCoord).r * PushConst.Roughness;
	float Occlusion = texture(OcclusionMap, FragTexCoord).r * PushConst.Occlusion;
//	float Metallic = 1.0;
//	float Roughness = 1.0;
//	float Occlusion = 1.0;

	/*
	vec3 N = normalize(FragNormal);
	vec3 L = normalize(-UBO.LightDirection);
	
	vec3 Diffuse = vec3(max(dot(N, L), 0.0)) * 1.5;
	
	vec4 Color = PushConst.Color;
	if (Color.w == 0.0)
		Color.w = 1.0;
	vec4 Tex = texture(AlbedoMap, FragTexCoord);
	OutColor = vec4((Ambient + (Ambient - Shadow) * Diffuse), 1.0) * Color * Tex;
	if (Tex.a < 0.9)
		discard;
	*/

	vec3 N = normalize(Normal);
					//Upload Camera Pos
	vec3 V = normalize(UBO.CameraPosition.xyz - FragWorldPos.xyz);

	vec3 F0 = vec3(0.04); 
	F0 = mix(F0, Albedo, Metallic);
	vec3 Lo = vec3(0.0);

	vec3 L = normalize(-UBO.LightDirection.xyz);
	vec3 H = normalize(V + L);

	vec3 LightColor = vec3(25.0);
	vec3 Diffuse = vec3(max(dot(N, L), 0.0)) * LightColor;
	float distance = length(-UBO.LightDirection.xyz);
	float attenuation = 1.0 / (distance * distance);
	vec3 radiance = LightColor * attenuation;

	float NDF = DistributionGGX(N, H, Roughness);   
	float G = GeometrySmith(N, V, L, Roughness);      
	vec3 F = FresnelSchlick(max(dot(H, V), 0.0), F0);

	vec3 nominator = NDF * G * F; 
	float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0);
	vec3 specular = nominator / max(denominator, 0.001);

	vec3 kS = F;
	vec3 kD = vec3(1.0) - kS;
	kD *= 1.0 - Metallic;
	float NdotL = max(dot(N, L), 0.0); 

	Lo += (kD * Albedo / PI + specular) * radiance * NdotL;

	float Ambient = 0.6 * Occlusion;
	
	float Shadow = GetShadow(Ambient);
//	OutColor.rgb = (Ambient + (Ambient - Shadow)) * Albedo + Lo;
//	OutColor.rgb = vec3((Ambient + (Ambient - Shadow))) * Albedo;

//	OutColor.rgb = (Ambient + (Ambient - Shadow)) * Albedo + (Lo * (1.0 - Shadow));
		//	OutColor.rgb = Albedo + (Lo * (Ambient + (Ambient - Shadow)));
//	OutColor.rgb = vec3(Ambient + (Ambient - Shadow));
			OutColor.rgb = Ambient * Albedo + ((Ambient - Shadow) * Lo);
		//	OutColor.rgb = (Ambient + (Ambient - Shadow) * Diffuse) * Albedo;
		//	OutColor.rgb = (Ambient - Shadow) * Albedo + Lo;
//	OutColor.rgb = vec3(Ambient);
//	OutColor.rgb = Ambient * Albedo + Lo;

//	float Shadow = GetShadow(Ambient);
//	OutColor.rgb = vec3((Ambient + (Ambient - Shadow) + Lo)) * Albedo;
//	OutColor.rgb = (Ambient + Lo) * Albedo;

	//OutColor = vec4((Ambient + (Ambient - Shadow) * Diffuse), 1.0) * Color * Tex;
	//combine shadows with pbr
//	vec3 color = ambient;
//	color /= Shadow;
//	OutColor.rgb = color + Lo;


//	OutColor.rgb = (ambient + (ambient - Shadow) + Lo);

	//	OutColor.rgb = OutColor.rgb / (OutColor.rgb + vec3(1.0));
									//2.2
//	OutColor.rgb = pow(OutColor.rgb, vec3(1.0/2.2)); 

	float Exposure = 4.0;
	float Gamma = 1.3;//1.3
	OutColor.rgb = Uncharted2Tonemap(OutColor.rgb * Exposure);
	OutColor.rgb = OutColor.rgb * (1.0f / Uncharted2Tonemap(vec3(11.2f)));	
	// Gamma correction
	OutColor.rgb = pow(OutColor.rgb, vec3(1.0f / Gamma));

//	vec3 mapped = OutColor.rgb / (OutColor.rgb + vec3(1.0));
//	vec3 mapped = vec3(1.0) - exp(-OutColor.rgb * 1.65);
//	OutColor.rgb = mapped;
  // gamma correction 
//    mapped = pow(mapped, vec3(1.0 / gamma));

	OutColor.a = AlbedoTexture.a * Color.a;

	if (AlbedoTexture.a < 0.9)
		discard;
//	OutColor = texture(NormalMap, FragTexCoord);
//	vec3 mapped = OutColor.rgb / (OutColor.rgb + vec3(1.0));
//	vec3 mapped = vec3(1.0) - exp(-OutColor.rgb * 1.65);
  // gamma correction 
//    mapped = pow(mapped, vec3(1.0 / gamma));
  
//    OutColor.rgb = mapped;

//	OutColor.a = texture(AlbedoTexture, FragTexCoord).a;
	/*
	switch(CascadeIndex) 
	{
		case 0 : 
			OutColor.rgb *= vec3(1.0f, 0.25f, 0.25f);
			break;
		case 1 : 
			OutColor.rgb *= vec3(0.25f, 1.0f, 0.25f);
			break;
		case 2 : 
			OutColor.rgb *= vec3(0.25f, 0.25f, 1.0f);
			break;
		case 3 : 
			OutColor.rgb *= vec3(1.0f, 1.0f, 0.25f);
			break;
	}
	*/

//	float depthValue = texture(ShadowMap[0], FragTexCoord).r;
//    OutColor = vec4(vec3(depthValue), 1.0);
}