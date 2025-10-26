#version 450
#extension GL_ARB_separate_shader_objects : enable

//Change in SceneFragment.frag too
#define SHADOW_MAP_CASCADE_COUNT 3
#define MAX_NUMBER_OF_LIGHTS 64

layout (location = 0) out vec4 OutColor;

layout(location = 0) in vec3 FragViewNormal;
layout(location = 1) in vec3 FragNormal;
layout(location = 2) in vec2 FragTexCoord;
layout(location = 3) in vec4 FragPosRelToCam;
layout(location = 4) in vec4 FragWorldPos;


layout(set = 0, binding = 1) uniform UniformBufferObject
{
	vec4 CascadeSplits;
	vec4 CascadeRange;
	vec4 CascadeScale;
	vec4 CascadeBias;
	mat4 CascadeProjectionView[SHADOW_MAP_CASCADE_COUNT];
	vec4 CameraPosition;
	mat4 View;
	mat4 InvViewProj;
	mat4 InvView;
	mat4 InvProj;
	vec4 ClearColor;
	float Gamma;
	float Exposure;
	float AmbientMultiplier;
	uint RenderSSAO;
} UBO;

layout(set = 0, binding = 2) readonly buffer StorageBufferObject 
{ 
	vec4 LightPos[MAX_NUMBER_OF_LIGHTS];
	vec4 LightColor[MAX_NUMBER_OF_LIGHTS];
	uint LightCastShadow[MAX_NUMBER_OF_LIGHTS];
	uint LightType[MAX_NUMBER_OF_LIGHTS];
	uint LightCount;
	uint Dummy;
} SBO;

layout(set = 0, binding = 3) uniform sampler2D ShadowMap[SHADOW_MAP_CASCADE_COUNT];

layout(set = 1, binding = 0) uniform sampler2D AlbedoMap;
layout(set = 1, binding = 1) uniform sampler2D NormalMap;
layout(set = 1, binding = 2) uniform sampler2D MetallicMap;
layout(set = 1, binding = 3) uniform sampler2D RoughnessMap;
layout(set = 1, binding = 4) uniform sampler2D OcclusionMap;


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

const mat4 BiasMat = mat4
( 
	0.5, 0.0, 0.0, 0.0,
	0.0, 0.5, 0.0, 0.0,
	0.0, 0.0, 1.0, 0.0,
	0.5, 0.5, 0.0, 1.0 
);

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


float TextureProjection(vec4 ShadowCoord, vec2 Offset, uint CascadeIndex, float Ambient)
{
	float Shadow = 0.0;
	
	float Bias = UBO.CascadeBias[CascadeIndex];

	if (ShadowCoord.z > -1.0 && ShadowCoord.z < 1.0) 
	{
		float Dist = texture(ShadowMap[CascadeIndex], ShadowCoord.st + Offset).r;
		if (ShadowCoord.w > 0.0 && Dist < ShadowCoord.z - Bias) //0.003
		{
			Shadow = Ambient;
		}
	}

	return Shadow;
}

float FilterPCF(vec4 ShadowCoord, int Range, float Scale, uint CascadeIndex, float Ambient)
{
	vec2 TextureDimension = textureSize(ShadowMap[CascadeIndex], 0).yy;

	float DX = Scale / TextureDimension.x;
	float DY = Scale / TextureDimension.y;

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

//get set in main
#define PosRelToCam vec4(FragPosRelToCam.xyz, LinearDepth(gl_FragCoord.z))
#define WorldPos FragWorldPos
//vec3 PosRelToCam = vec3(0.0);
//vec4 WorldPos = vec4(0.0);

float GetShadow(float Ambient)
{

	uint CascadeIndex = 0;
	for (uint i = 0; i < SHADOW_MAP_CASCADE_COUNT - 1; i++)
		if (PosRelToCam.z < UBO.CascadeSplits[i])
			CascadeIndex = i + 1;
			 
//	if (CascadeIndex > 1)
//		CascadeIndex = 1;

	vec4 ShadowCoord = (UBO.CascadeProjectionView[CascadeIndex]) * vec4(WorldPos.xyz, 1.0);	

//	ShadowCoord.x /= float(SHADOW_MAP_CASCADE_COUNT);
//	ShadowCoord.x += (1.0 / float(SHADOW_MAP_CASCADE_COUNT)) * float(CascadeIndex); 

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

//https://github.com/minus34/cesium1/blob/master/Cesium/Shaders/Builtin/Functions/luminance.glsl
float luminance(vec3 rgb)
{
    // Algorithm from Chapter 10 of Graphics Shaders.
    const vec3 W = vec3(0.299f, 0.587f, 0.114f);
    return dot(rgb, W);
}

vec3 saturate(vec3 color)
{
	return clamp(color,0.0,1.0);
}

#define LOG10_INV (1 / log(10))

float log10(float x)
{
	return LOG10_INV * log(x);
}

const mat3 ACESInputMat =
{
	{0.59719, 0.35458, 0.04823},
	{0.07600, 0.90834, 0.01566},
	{0.02840, 0.13383, 0.83777}
};

const mat3 ACESOutputMat =
{
	{ 1.60475, -0.53108, -0.07367},
	{-0.10208,  1.10813, -0.00605},
	{-0.00327, -0.07276,  1.07602}
};

vec3 RRTAndODTFit(vec3 v) {
	vec3 a = v * (v + 0.0245786f) - 0.000090537f;
	vec3 b = v * (0.983729f * v + 0.4329510f) + 0.238081f;
	return a / b;
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


// Unpacking from octahedron normals, input is the output from pack_normal_octahedron
vec3 unpack_normal_octahedron(vec2 packed_nrm) {

	// Version using newer GLSL capatibilities
	vec3 v = vec3(packed_nrm.xy, 1.0 - abs(packed_nrm.x) - abs(packed_nrm.y));

	// Branch-Less version
	v.xy = mix(v.xy, (1.0 - abs(v.yx)) * sign_not_zero(v.xy), step(v.z, 0));


    return normalize(v);

}

float orenNayarDiffuse(vec3 N, vec3 V, vec3 L, float roughness)
{
    float sigma = roughness * roughness * (PI / 2.0);
    float sigma2 = sigma * sigma;

    float NdotL = max(dot(N, L), 0.0);
    float NdotV = max(dot(N, V), 0.0);
    if (NdotL <= 0.0 || NdotV <= 0.0)
        return 0.0;

    float A = 1.0 - (0.5 * sigma2 / (sigma2 + 0.33));
    float B = 0.45 * sigma2 / (sigma2 + 0.09);

    float LdotV = max(dot(L, V), 0.0);
    float s = LdotV - NdotL * NdotV;
    float t = (s > 0.0) ? max(NdotL, NdotV) : 1.0;
    float alpha = acos(t == NdotL ? NdotL : NdotV);
    float beta  = acos(t == NdotL ? NdotV : NdotL);

    // Compute Oren-Nayar term
    float oren = (A + B * s / t) * NdotL;

    return oren;
}

void main()
{
	const float Eps = 0.01;

	vec4 Albedo = texture(AlbedoMap, FragTexCoord) * PushConst.Color;
/*
	if (Equality(UBO.ClearColor.x, Albedo.x, Eps) &&
		Equality(UBO.ClearColor.y, Albedo.y, Eps) && 
		Equality(UBO.ClearColor.z, Albedo.z, Eps))
	{
		OutColor = Albedo;
		return;
	}
*/
	float Metallic = texture(MetallicMap, FragTexCoord).r * PushConst.Metallic;
	float Roughness = texture(RoughnessMap, FragTexCoord).r * PushConst.Roughness;
	float Occlusion = texture(OcclusionMap, FragTexCoord).r * PushConst.Occlusion;

/*
	if (LinearDepth(gl_FragCoord.z) > 900.0)
	{
		OutColor = Albedo;
		return;
	}
*/	
//	vec3 Normal = normalize(vec3(ViewNormalTex.w, NormalTex.x, NormalTex.y));
	vec3 Normal = GetNormalFromMap();
	 
	float SSAO = 1.0;


	vec3 N = Normal;
					//Upload Camera Pos
	vec3 V = normalize(UBO.CameraPosition.xyz - WorldPos.xyz);

	vec3 F0 = vec3(0.04); 
	F0 = mix(F0, Albedo.xyz, Metallic);
	vec3 Lo = vec3(0.0);
	vec3 LoShadow = vec3(0.0);
	
	OutColor = vec4(1.0);

	bool HasShadow = false;

	for (uint i = 0; i < SBO.LightCount; i++)
	{
		

		vec3 Light;
		vec3 L;

		switch (SBO.LightType[i])
		{
		case 0:
			Light = SBO.LightPos[i].xyz - WorldPos.xyz;
			L = normalize(Light);
			break;

		case 1:
			Light = normalize(SBO.LightPos[i].xyz);
			L = Light;
			break;

		default:
			Light = normalize(SBO.LightPos[i].xyz);
			L = Light;
			break;
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
			// Oren–Nayar diffuse instead of Lambert
			float oren = orenNayarDiffuse(N, V, L, Roughness);
			vec3 diffuse = Albedo.xyz / PI * oren;

			LoShadow += (kD * diffuse + specular) * radiance;  
//			LoShadow += (kD * Albedo.xyz / PI + specular) * radiance * NdotL;
		}			
		else
		{
			// Oren–Nayar diffuse instead of Lambert
			float oren = orenNayarDiffuse(N, V, L, Roughness);
			vec3 diffuse = Albedo.xyz / PI * oren;

			Lo += (kD * diffuse + specular) * radiance;  
		//	Lo += (kD * Albedo.xyz / PI + specular) * radiance * NdotL;
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

	vec3 col = OutColor.rgb;

	//Tone mappers from: https://github.com/GarrettGunnell/Post-Processing/blob/main/Assets/Tone%20Mapping/Tonemapping.shader

//Ward
	float _Ldmax = UBO.Gamma;
 	float Lin = luminance(col);

	float m = (1.219f + pow(_Ldmax / 2.0f, 0.4f)) / (1.219f + pow(Lin, 0.4f));
	m = pow(m, 2.5f); 

	float Lout = m / _Ldmax * Lin;

	vec3 Cout = col / Lin * Lout;

	OutColor.rgb = saturate(Cout);

	if (UBO.RenderSSAO == 1)
		OutColor.rgb *= SSAO;

	OutColor.a = Albedo.a;
//	if (Albedo.a < 0.2)
//		discard;

	//OutColor.rgb = texture(ShadowMap, FragTexCoord).xyz;
}