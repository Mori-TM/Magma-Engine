#version 450
#extension GL_ARB_separate_shader_objects : enable

#define SHADOW_MAP_CASCADE_COUNT 3

layout(location = 0) out vec4 OutDepthPosition;
layout(location = 1) out vec4 OutNormalMap;
layout(location = 2) out vec4 OutPBRMap;
layout(location = 3) out vec4 OutLightPass;

layout(location = 0) in vec3 FragNormal;
layout(location = 1) in vec2 FragTexCoord;
layout(location = 2) in vec4 FragPosRelToCam;
layout(location = 3) in vec4 FragWorldPos;

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
	vec4 CascadeRange;
	vec4 CascadeScale;
	vec4 CascadeBias;
	mat4 CascadeProjectionView[SHADOW_MAP_CASCADE_COUNT];
	vec4 LightDirection;
	vec4 CameraPosition;
	mat4 View;
	float Gamma;
	float Exposure;
} UBO;

//#define Ambient 0.6

const mat4 BiasMat = mat4
( 
	0.5, 0.0, 0.0, 0.0,
	0.0, 0.5, 0.0, 0.0,
	0.0, 0.0, 1.0, 0.0,
	0.5, 0.5, 0.0, 1.0 
);

/*
float TextureProjection(vec4 ShadowCoord, vec2 Offset, uint CascadeIndex, float Ambient)
{
	float Shadow = 0.0;
	
	float Bias = UBO.CascadeBias[CascadeIndex];

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
	ivec2 TextureDimension = textureSize(ShadowMap, 0).yy;


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
*/

vec2 poissonDisk[16] = vec2[](
	vec2(-0.94201624, -0.39906216),
	vec2(0.94558609, -0.76890725),
	vec2(-0.094184101, -0.92938870),
	vec2(0.34495938, 0.29387760),
	vec2(-0.91588581, 0.45771432),
	vec2(-0.81544232, -0.87912464),
	vec2(-0.38277543, 0.27676845),
	vec2(0.97484398, 0.75648379),
	vec2(0.44323325, -0.97511554),
	vec2(0.53742981, -0.47373420),
	vec2(-0.26496911, -0.41893023),
	vec2(-0.79197514, 0.19090188),
	vec2(-0.24188840, 0.99706507),
	vec2(-0.81409955, 0.91437590),
	vec2(0.19984126, 0.78641367),
	vec2(0.14383161, -0.14100790)
);

/*
vec4 TextureProjection(vec4 ShadowCoord, vec2 Offset, uint CascadeIndex, float Ambient)
{
	vec3 proj = worldPos - projDir * dot(worldPos, projDir);
	vec2 texCoord = vec2(0.5) + vec2(proj.x, proj.y) / proj.z * vec2(0.5);
	vec2 texelSize = vec2(1.0) / texSize;
	vec4 color = vec4(0.0);
	for (int x = -1; x <= 1; x++)
	{
		for (int y = -1; y <= 1; y++)
		{
			vec2 offset = vec2(x, y) * texelSize;
			color += texelFetch(tex, ivec2(texCoord * texSize + offset), 0);
		}
	}
	return color / 9.0;
}
*/

float TextureProjection(vec4 ShadowCoord, vec2 Offset, uint CascadeIndex, float Ambient)
{
	float Shadow = 0.0;
	
	float Bias = UBO.CascadeBias[CascadeIndex];

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
	ivec2 TextureDimension = textureSize(ShadowMap, 0).yy;


	float DX = Scale / float(TextureDimension.x);
	float DY = Scale / float(TextureDimension.y);

	float sum = 0.0;

	int numSamples = Range * Range * Range * Range;

	for (int i = 0; i < numSamples; i++)
	{
		sum += TextureProjection(ShadowCoord, poissonDisk[i] * vec2(DX, DY), CascadeIndex, Ambient);
	}
	return sum / float(numSamples);
}

const float PI = 3.14159265359;

vec4 TextureCustom(sampler2D Tex, vec2 Coord)
{
//	vec2 Size = vec2(textureSize(Tex, 1));
//
//	ivec2 texelCoord = ivec2(clamp(Coord * Size, vec2(0), vec2(Size.x - 1, Size.y - 1)));
//	vec4 texel = texelFetch(Tex, texelCoord, 1);
//	return texel;
	return texture(Tex, Coord);
//	return textureLod(Tex, Coord, 0);
}

vec3 GetNormalFromMap()
{
	vec3 Normal = TextureCustom(NormalMap, FragTexCoord).xyz;

//	vec3 Normal = texelFetch(NormalMap, ivec2(FragTexCoord * vec2(textureSize(NormalMap, 0))), 0).xyz;
	

	if (Normal.x == 1.0 &&	Normal.y == 1.0 &&	Normal.z == 1.0)
		return normalize(FragNormal);
/*
	vec3 tangentNormal = Normal * 2.0 - 1.0;

	vec3 N = FragNormal;
	vec3 Q1 = dFdx(FragWorldPos.xyz);
	vec3 Q2 = dFdy(FragWorldPos.xyz);
	vec2 st1 = dFdx(FragTexCoord);
	vec2 st2 = dFdy(FragTexCoord);
	mat3 TBN = mat3(normalize(Q1*st2.t - Q2*st1.t), normalize(cross(N, Q1*st2.t - Q2*st1.t)), N);

	return normalize(TBN * tangentNormal);
	*/

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

	ShadowCoord.x /= SHADOW_MAP_CASCADE_COUNT;
	ShadowCoord.x += (1.0 / SHADOW_MAP_CASCADE_COUNT) * float(CascadeIndex); 

	float Shadow = 0.0;
	if (UBO.CascadeRange[CascadeIndex] > 0.1)
		Shadow = FilterPCF(ShadowCoord / ShadowCoord.w, int(UBO.CascadeRange[CascadeIndex]), UBO.CascadeScale[CascadeIndex], CascadeIndex, Ambient);
	else
		Shadow = TextureProjection(ShadowCoord / ShadowCoord.w, vec2(0.0), CascadeIndex, Ambient);

	return Shadow;
}

vec3 Uncharted2Tonemap(vec3 x)
{
	const float A = 0.15;
	const float B = 0.50;
	const float C = 0.10;
	const float D = 0.20;
	const float E = 0.02;
	const float F = 0.30;
	return ((x*(A*x+C*B)+D*E)/(x*(A*x+B)+D*F))-E/F;
}

vec3 AcesTonemap(vec3 x) 
{
  const float a = 2.51;
  const float b = 0.03;
  const float c = 2.43;
  const float d = 0.59;
  const float e = 0.14;
  return clamp((x * (a * x + b)) / (x * (c * x + d) + e), 0.0, 1.0);
}

void main() 
{
//	float Shadow1 = GetShadow(0.5);

//	OutLightPass = vec4(vec3(texture(ShadowMap, FragTexCoord).r), 1.0);
//	
//	return;
	vec4 Color = PushConst.Color;
	if (Color.w == 0.0)	Color.w = 1.0;
	vec4 AlbedoTexture = texture(AlbedoMap, FragTexCoord);

//	vec4 AlbedoTexture = texelFetch(AlbedoMap, ivec2(FragTexCoord * vec2(textureSize(AlbedoMap, 0))), 0);
	vec3 Albedo = pow(AlbedoTexture.rgb * Color.rgb, vec3(2.2));
//	vec3 Albedo = AlbedoTexture.rgb * Color.rgb;
	vec3 Normal = GetNormalFromMap();
	float Metallic = TextureCustom(MetallicMap, FragTexCoord).r * PushConst.Metallic;
	float Roughness = TextureCustom(RoughnessMap, FragTexCoord).r * PushConst.Roughness;
	float Occlusion = TextureCustom(OcclusionMap, FragTexCoord).r * PushConst.Occlusion;

	vec3 N = Normal;
					//Upload Camera Pos
	vec3 V = normalize(UBO.CameraPosition.xyz - FragWorldPos.xyz);

	vec3 F0 = vec3(0.04); 
	F0 = mix(F0, Albedo, Metallic);
	vec3 Lo = vec3(0.0);

	vec3 L = -UBO.LightDirection.xyz;
	vec3 H = normalize(V + L);

	vec3 LightColor = vec3(12.0);
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

	OutLightPass.rgb = Ambient * Albedo + ((Ambient - Shadow) * Lo);


	float Exposure = UBO.Exposure;
	float Gamma = UBO.Gamma;//1.3

	OutLightPass.rgb = Uncharted2Tonemap(OutLightPass.rgb * Exposure);

	const vec3 Uncharted2TonemapConst = (1.0f / Uncharted2Tonemap(vec3(11.2f)));
	OutLightPass.rgb = OutLightPass.rgb * Uncharted2TonemapConst;	
	OutLightPass.rgb = pow(OutLightPass.rgb, vec3(1.0f / Gamma));

	/*
	OutLightPass.rgb = pow(OutLightPass.rgb, vec3(1.0 / Gamma));	
	OutLightPass.rgb = AcesTonemap(OutLightPass.rgb * Exposure);
//	OutLightPass.rgb = OutLightPass.rgb * (1.0f / AcesTonemap(vec3(11.2f)));	
	// Gamma correction
	*/	

	OutLightPass.a = AlbedoTexture.a * Color.a;
	OutNormalMap = vec4((Normal).xyz * mat3((UBO.View)), Roughness);
//	OutNormalMap = vec4((Normal).xyz * mat3(inverse(UBO.View)), Roughness);

	OutDepthPosition = vec4(FragPosRelToCam.xyz, gl_FragCoord.z);

	OutPBRMap = vec4(Metallic, Roughness, 1.0, 1.0);

	if (AlbedoTexture.a < 0.9)
		discard;
}