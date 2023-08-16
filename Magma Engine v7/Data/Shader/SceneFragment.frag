#version 450
#extension GL_ARB_separate_shader_objects : enable

#define SHADOW_MAP_CASCADE_COUNT 3
#define MAX_NUMBER_OF_LIGHTS 128

layout(location = 0) out vec4 OutColor;

layout(location = 0) in vec2 FragTexCoord;

layout(set = 0, binding = 0) uniform sampler2D SamplerPosition;
layout(set = 0, binding = 1) uniform sampler2D SamplerViewNormal;
layout(set = 0, binding = 2) uniform sampler2D SamplerAlbedo;
layout(set = 0, binding = 3) uniform sampler2D SamplerPBR;
layout(set = 0, binding = 4) uniform sampler2D SamplerWorldPos;
layout(set = 0, binding = 5) uniform sampler2D SamplerNormal;
layout(set = 0, binding = 6) uniform sampler2D SamplerSSAO;
layout(set = 0, binding = 7) uniform sampler2D ShadowMap;

layout(set = 0, binding = 8) uniform UniformBufferObject
{
	vec4 CascadeSplits;
	vec4 CascadeRange;
	vec4 CascadeScale;
	vec4 CascadeBias;
	mat4 CascadeProjectionView[SHADOW_MAP_CASCADE_COUNT];
	vec4 CameraPosition;
	mat4 View;
	vec4 ClearColor;
	float Gamma;
	float Exposure;
	float AmbientMultiplier;
	uint RenderSSAO;
} UBO;

layout(set = 0, binding = 9) buffer StorageBufferObject 
{ 
	vec4 LightPos[MAX_NUMBER_OF_LIGHTS];
	vec4 LightColor[MAX_NUMBER_OF_LIGHTS];
	uint LightCastShadow[MAX_NUMBER_OF_LIGHTS];
	uint LightType[MAX_NUMBER_OF_LIGHTS];
	uint LightCount;
	uint Dummy;
} SBO;

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
/*
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
*/

vec2 poissonDisk[32] = vec2[](
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
    vec2(0.14383161, -0.14100790),
    vec2(-0.84820694, -0.78380224),
    vec2(-0.50873361, 0.52738150),
    vec2(0.13990484, 0.86086441),
    vec2(-0.07118580, -0.95305477),
    vec2(-0.26651088, -0.46946140),
    vec2(0.10353921, 0.91848904),
    vec2(0.75837174, 0.20669898),
    vec2(0.47819716, -0.36261802),
    vec2(-0.97226835, -0.00363746),
    vec2(0.92033927, 0.32995477),
    vec2(0.27539877, -0.31290921),
    vec2(-0.66911676, 0.70071288),
    vec2(0.81428591, 0.71614587),
    vec2(0.07919059, -0.73169701),
    vec2(-0.81185319, -0.90736042),
    vec2(-0.14442749, -0.40515699)
//    vec2(0.74138804, -0.38163541)
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

	int numSamples = Range * 4;

	for (int i = 0; i < numSamples; i++)
	{
		sum += TextureProjection(ShadowCoord, poissonDisk[i] * vec2(DX, DY), CascadeIndex, Ambient);
	}
	return sum / float(numSamples);
}

const float PI = 3.14159265359;

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

vec3 PosRelToCam = vec3(0.0);
vec4 WorldPos = vec4(0.0);

float GetShadow(float Ambient)
{
	uint CascadeIndex = 0;
	for (uint i = 0; i < SHADOW_MAP_CASCADE_COUNT - 1; i++)
		if (PosRelToCam.z < UBO.CascadeSplits[i])
			CascadeIndex = i + 1;
			 
	vec4 ShadowCoord = (UBO.CascadeProjectionView[CascadeIndex]) * WorldPos;	

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

bool Equality(float a, float b, float epsilon)
{
  return abs(a - b) < epsilon;
}

void main() 
{
/*
	vec4 AlbedoTexture = texture(SamplerAlbedo, FragTexCoord);

//	vec4 AlbedoTexture = texelFetch(AlbedoMap, ivec2(FragTexCoord * vec2(textureSize(AlbedoMap, 0))), 0);
	vec3 Albedo = pow(AlbedoTexture.rgb, vec3(2.2));
//	vec3 Albedo = AlbedoTexture.rgb * Color.rgb;
	vec3 Normal = normalize(texture(SamplerNormal, FragTexCoord).rgb * 2.0 - 1.0);
	float Metallic = texture(SamplerPBR, FragTexCoord).r;
	float Roughness = texture(SamplerPBR, FragTexCoord).g;
	float Occlusion = texture(SamplerPBR, FragTexCoord).b;

	vec3 N = Normal;
					//Upload Camera Pos
//	vec3 V = normalize(UBO.CameraPosition.xyz - FragWorldPos.xyz);
	vec3 V = texture(SamplerPosition, FragTexCoord).rgb;

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
	
//	float Shadow = GetShadow(Ambient);

	float Shadow = 0.0;
	OutColor.rgb = Ambient * Albedo + ((Ambient - Shadow) * Lo);
	
	float ssao = texture(SamplerSSAO, FragTexCoord).r;
	

	float Exposure = UBO.Exposure;
	float Gamma = UBO.Gamma;//1.3

	OutColor.rgb = Uncharted2Tonemap(OutColor.rgb * Exposure);

	const vec3 Uncharted2TonemapConst = (1.0f / Uncharted2Tonemap(vec3(11.2f)));
	OutColor.rgb = OutColor.rgb * Uncharted2TonemapConst;	
	OutColor.rgb = pow(OutColor.rgb, vec3(1.0f / Gamma));

	OutColor.rgb *= ssao.rrr;


	OutColor.a = AlbedoTexture.a;

	if (AlbedoTexture.a < 0.9)
		discard;
*/
	
	vec4 PositionTex = texture(SamplerPosition,	FragTexCoord);
	vec4 ViewNormalTex   = texture(SamplerViewNormal,	FragTexCoord);
	vec4 AlbedoTex   = texture(SamplerAlbedo,	FragTexCoord);
	vec4 PBRTex		 = texture(SamplerPBR,		FragTexCoord);
	vec4 WorldPosTex = texture(SamplerWorldPos,	FragTexCoord);
	vec2 NormalTex = texture(SamplerNormal,	FragTexCoord).xy;

	vec3 ViewNormal = normalize(ViewNormalTex.rgb);
	vec3 fragPos = PositionTex.rgb;
	PosRelToCam = fragPos;
	vec4 Albedo = AlbedoTex;
	WorldPos = WorldPosTex;

	const float Eps = 0.01;

	if (Equality(UBO.ClearColor.x, Albedo.x, Eps) &&
		Equality(UBO.ClearColor.y, Albedo.y, Eps) && 
		Equality(UBO.ClearColor.z, Albedo.z, Eps))
	{
		OutColor = Albedo;
		return;
	}

	float Metallic = PBRTex.x;
	float Roughness = PBRTex.y;
	float Occlusion = PBRTex.z;

	if (PositionTex.w > 900.0)
	{
		OutColor = Albedo;
		return;
	}
	
	vec3 Normal = normalize(vec3(ViewNormalTex.w, NormalTex.x, NormalTex.y));
	 
	float SSAO = texture(SamplerSSAO, FragTexCoord).r;
	/*
	vec3 lightPos = vec3(0.0);
//	vec3 L = normalize(-UBO.LightDirection.xyz);
	
	vec3 L = normalize(lightPos - fragPos);
	vec3 diffuse = max(dot(ViewNormal, L), 1.0) * Albedo.xyz ;
//	vec3 diffuse = max(dot(normal, L), 0.0) * albedo.xyz * vec3(1.0);
//	vec3 halfwayDir = normalize(L + viewDir);
//	float spec = pow(max(dot(Normal, halfwayDir), 0.0), 8.0);
//	vec3 specular = dirLight.Color * spec;

//	float NdotL = max(0.5, dot(normal, L));
//
//	vec3 baseColor = albedo.rgb * max(dot(normal, L), 0.0);
	OutColor.a = 1.0;
//	OutColor.rgb = vec3(ssao, 0, 0);
//	OutColor.rgba = vec4(ssao.rr, 0.5, 1.0); 
	
	OutColor.rgb = vec3(1.0);
	OutColor.rgb = ssao.rrr;
//	OutColor.rgb *= diffuse;
	vec3 Ambient = vec3(0.5);
//	OutColor.rgb = WorldPos.xyz;
//	OutColor.rgb = vec3(GetShadow(1.0));
	OutColor.rgb *= Albedo.xyz - vec3(GetShadow(Ambient.x));

	OutColor.rgb = Normal;
	*/









	vec3 N = Normal;
					//Upload Camera Pos
	vec3 V = normalize(UBO.CameraPosition.xyz - WorldPos.xyz);

	vec3 F0 = vec3(0.04); 
	F0 = mix(F0, Albedo.xyz, Metallic);
	vec3 Lo = vec3(0.0);
	vec3 LoShadow = vec3(0.0);
	/*
	{
		vec3 L = normalize(-UBO.LightDirection.xyz);
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

		Lo += (kD * Albedo.xyz / PI + specular) * radiance * NdotL;
	}

	float Ambient = 0.1 * Occlusion;
	
	float Shadow = 0.0;
	if (UBO.RenderShadows == 1)
		Shadow = GetShadow(Ambient);

	Lo = Ambient * Albedo.xyz + ((Ambient - Shadow) * Lo);
	*/
	/*
	vec3 L = normalize(-UBO.LightDirection.xyz);
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

	Lo += (kD * Albedo.xyz / PI + specular) * radiance * NdotL;

	float Ambient = 0.6 * Occlusion;
	
	float Shadow = 0.0;
	if (UBO.RenderShadows == 1)
		Shadow = GetShadow(Ambient);
		

	OutColor.rgb = Ambient * Albedo.xyz + ((Ambient - Shadow) * Lo);
	*/
	
	
	OutColor = vec4(1.0);

	bool HasShadow = false;

	for (uint i = 0; i < SBO.LightCount; i++)
	{
		

		vec3 Light;
		vec3 L;
		if (SBO.LightType[i] == 0)
		{
			Light = SBO.LightPos[i].xyz - WorldPos.xyz;
			L = normalize(Light);
		}			
		else
		{
			Light = normalize(SBO.LightPos[i].xyz);
			L = Light;
		}
		 
		vec3 H = normalize(V + L);

		vec3 LightColor = SBO.LightColor[i].xyz * vec3(SBO.LightColor[i].w);
	//	vec3 Diffuse = vec3(max(dot(N, L), 0.0)) * LightColor;
		float distance = length(Light);
		float attenuation = 1.0 / (distance * distance);
		vec3 radiance = LightColor * attenuation;

		float NDF = DistributionGGX(N, H, Roughness);   
		float G = GeometrySmith(N, V, L, Roughness);      
		vec3 F = FresnelSchlick(max(dot(H, V), 0.0), F0);

		vec3 nominator = NDF * G * F; 
		float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001;
		vec3 specular = nominator / denominator;

		vec3 kS = F;
		vec3 kD = vec3(1.0) - kS;
		kD *= 1.0 - Metallic;
		float NdotL = max(dot(N, L), 0.0); 

		

		if (SBO.LightCastShadow[i] == 1 && !HasShadow)
		{
			LoShadow += (kD * Albedo.xyz / PI + specular) * radiance * NdotL;
		}			
		else
		{
			Lo += (kD * Albedo.xyz / PI + specular) * radiance * NdotL;
		}

		if (SBO.LightCastShadow[i] == 1)
			HasShadow = true;
	}

	float Ambient = UBO.AmbientMultiplier * Occlusion;
	
	float Shadow = 0.0;

//	HasShadow = true;
	if (HasShadow)
	{
		Shadow = GetShadow(1.0);
		OutColor.rgb = Ambient * Albedo.xyz + ((1.0 - Shadow) * LoShadow);
		OutColor.rgb += Lo;

		/*
		Shadow = GetShadow(Ambient);
		OutColor.rgb = Ambient * Albedo.xyz + ((Ambient - Shadow) * LoShadow);
		OutColor.rgb += ((Ambient) * Lo);
		*/
	}
		
	else
		OutColor.rgb = Ambient * Albedo.xyz + ((Ambient) * Lo);

//	OutColor.rgb = Ambient * Albedo.xyz + ((Ambient - Shadow) * Lo);

//	vec3 ambient = vec3(0.03) * Albedo.xyz * Occlusion;
   // OutColor.rgb = Lo;
//	return; 
	
//	OutColor.rgb = Ambient * Albedo.xyz + ((Ambient - Shadow) * Lo);

//	float Exposure = 4.0;
//	float Gamma = 1.3;//1.3
//	OutColor.rgb = Uncharted2Tonemap(OutColor.rgb * Exposure);
//	OutColor.rgb = OutColor.rgb * (1.0f / Uncharted2Tonemap(vec3(11.2f)));	
//	// Gamma correction
//	OutColor.rgb = pow(OutColor.rgb, vec3(1.0f / Gamma));

	float Exposure = UBO.Exposure;
	float Gamma = UBO.Gamma;//1.3

	OutColor.rgb = Uncharted2Tonemap(OutColor.rgb * Exposure);

	const vec3 Uncharted2TonemapConst = (1.0f / Uncharted2Tonemap(vec3(11.2f)));
	OutColor.rgb = OutColor.rgb * Uncharted2TonemapConst;	
	OutColor.rgb = pow(OutColor.rgb, vec3(1.0f / Gamma));
	
	if (UBO.RenderSSAO == 1)
		OutColor.rgb *= SSAO;

//	vec3 mapped = OutColor.rgb / (OutColor.rgb + vec3(1.0));
//	vec3 mapped = vec3(1.0) - exp(-OutColor.rgb * 1.65);
//	OutColor.rgb = mapped;
  // gamma correction 
//    mapped = pow(mapped, vec3(1.0 / gamma));

	OutColor.a = Albedo.a;

//	if (Albedo.a < 0.9)
//		discard;

//	float Exposure = UBO.Exposure;
//	float Gamma = UBO.Gamma;//1.3
//	OutColor.rgb = Uncharted2Tonemap(OutColor.rgb * Exposure);
//	const vec3 Uncharted2TonemapConst = (1.0f / Uncharted2Tonemap(vec3(11.2f)));
//	OutColor.rgb = OutColor.rgb * Uncharted2TonemapConst;	
//	OutColor.rgb = pow(OutColor.rgb, vec3(1.0f / Gamma));
//OutColor.rgb = baseColor;
//	OutColor.rgb = albedo.rgb;
//	OutColor.rgb = texture(ShadowMap, FragTexCoord).rrr;
}