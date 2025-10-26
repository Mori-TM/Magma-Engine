#version 450
#extension GL_ARB_separate_shader_objects : enable

//Change in ForwardFragment.frag too
#define SHADOW_MAP_CASCADE_COUNT 3
#define MAX_NUMBER_OF_LIGHTS 64

layout(location = 0) out vec4 OutColor;

layout(location = 0) in vec2 FragTexCoord;

layout(set = 0, binding = 0) uniform sampler2D SamplerPosition;
layout(set = 0, binding = 1) uniform sampler2D SamplerViewNormal;
layout(set = 0, binding = 2) uniform sampler2D SamplerAlbedo;
layout(set = 0, binding = 3) uniform sampler2D SamplerPBR;
//layout(set = 0, binding = 4) uniform sampler2D SamplerWorldPos;
layout(set = 0, binding = 4) uniform sampler2D SamplerSSAO;
layout(set = 0, binding = 5) uniform sampler2D ShadowMap[SHADOW_MAP_CASCADE_COUNT];

layout(set = 0, binding = 6) uniform UniformBufferObject
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

layout(set = 0, binding = 7) readonly buffer StorageBufferObject 
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
	vec2 TextureDimension = textureSize(ShadowMap[CascadeIndex], 0).xy;

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
vec3 PosRelToCam = vec3(0.0);
vec3 WorldPos = vec3(0.0);

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

vec3 ReconstructWorldPos(vec2 uv, float linearDepth, mat4 invViewProj) 
{
    float z = linearDepth;
    
    vec4 clipSpacePos = vec4(uv * 2.0 - 1.0, z, 1.0);
    vec4 viewSpacePos = UBO.InvProj * clipSpacePos;
    
    viewSpacePos /= viewSpacePos.w;

    vec4 worldSpacePosition = UBO.InvView * viewSpacePos;

    return worldSpacePosition.xyz;
}

// Reconstruct view space position from depth
vec3 ViewPositionFromDepth(vec2 texcoord, float depth)
{
  // Get x/w and y/w from the viewport position
  vec3 projectedPos = vec3(texcoord, depth);// * 2.0f - 1.0f;;

  // Transform by the inverse projection matrix
  vec4 positionVS = UBO.InvViewProj * vec4(projectedPos, 1.0f);

  // Divide by w to get the view-space position
  return positionVS.xyz / positionVS.w;
}

vec3 FresnelSchlickAprox(vec3 F0, float cosTheta)
{
    return mix(F0, vec3(1.0), pow(1.01 - cosTheta, 5.0));
}

// Schlick-Frensel approximation with added roughness lerp for ambient IBL
// See: https://seblagarde.wordpress.com/2011/08/17/hello-world/
vec3 FresnelSchlickWithRoughness(vec3 F0, float cosTheta, float roughness)
{
  return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(1.0 - cosTheta, 5.0);
}



// Full Cook-Torrence BRDF
vec3 CookTorrenceSpecularBRDF(vec3 F, vec3 N, vec3 V, vec3 H, vec3 L, float roughness)
{
  float D = DistributionGGX(N, H, roughness);
  float G = GeometrySmith(N, V, L, roughness);

  vec3 numerator    = D * G * F;
  float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0);
  vec3 specular     = numerator / max(denominator, 0.001);

  return specular;
}

vec3 orenNayarFast(vec3 n, vec3 l, vec3 v, vec3 albedo, float roughness) {
    float sigma2 = roughness * roughness;

    // Precompute Oren-Nayar coefficients
    float A = 1.0 - (sigma2 / (2.0 * (sigma2 + 0.33)));
    float B = 0.45 * sigma2 / (sigma2 + 0.09);

    float cosThetaI = max(dot(n, l), 0.0);
    float cosThetaR = max(dot(n, v), 0.0);

    // Use max/min trick to approximate alpha and beta
    float alpha = max(cosThetaI, cosThetaR);
    float beta  = min(cosThetaI, cosThetaR);

    // Approximate azimuthal term using dot of light/view tangent projections
    vec3 lProj = l - n * cosThetaI;
    vec3 vProj = v - n * cosThetaR;
    float cosPhiDiff = 0.0;
    float lLen = length(lProj);
    float vLen = length(vProj);
    if (lLen > 1e-4 && vLen > 1e-4) {
        cosPhiDiff = max(dot(lProj, vProj) / (lLen * vLen), 0.0);
    }

    // Approximate Oren-Nayar BRDF
    float diffuse = cosThetaI * (A + B * cosPhiDiff * (alpha * beta) / max(beta, 0.0001));

    return albedo * diffuse / 3.14159265;
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

//see https://github.com/tullrich/deferred-pbr
vec3 DirectRadiance(vec3 P, vec3 N, vec3 V, vec3 L, vec3 Light, vec3 albedo, float Roughness, float Metalness, vec3 F0, vec3 LightColor)
{
  // Direction to light in viewspace
//  vec3 L = normalize(MainLightPosition.xyz - P * MainLightPosition.w);

  // Half-Vector between light and eye in viewspace
  vec3 H = normalize(L + V);

  // cos(angle) between surface normal and light
  float NdL = max(0.001, dot(N, L));

  // cos(angle) between surface half vector and eye
  float HdV = max(0.001, dot(H, V));

  // Cook Torrence Terms
  vec3 F = FresnelSchlickAprox(F0, HdV);
  vec3 kD =  vec3(1.0) - F;

  // BRDF
  vec3 specBrdf = CookTorrenceSpecularBRDF(F, N, V, H, L, Roughness);
//  vec3 diffuseBrdf = kD * (albedo / PI) * (1.0 - Metalness); // Lambert diffuse
	vec3 diffuseBrdf = orenNayarFast(N, L, V, albedo, Roughness * (PI / 2));

  // Point/Directional light attenuation
//  float A = mix(1.0f, 1.0 / (1.0 + 0.1 * dot(MainLightPosition.xyz - P, MainLightPosition.xyz - P)), MainLightPosition.w);
	float distance = length(Light);
	float attenuation = 1.0 / (distance * distance);
  // L
  vec3 radiance = attenuation * LightColor ;
  return (specBrdf + diffuseBrdf) * radiance * NdL;
}




void main() 
{	
	vec4 PositionTex = texture(SamplerPosition,	FragTexCoord);
	vec4 ViewNormalTex   = texture(SamplerViewNormal,	FragTexCoord);
	vec4 AlbedoTex   = texture(SamplerAlbedo,	FragTexCoord);
	vec4 PBRTex		 = texture(SamplerPBR,		FragTexCoord);
//	vec4 WorldPosTex = texture(SamplerWorldPos,	FragTexCoord);
//	vec2 NormalTex = texture(SamplerNormal,	FragTexCoord).xy;

//	vec3 ViewNormal = normalize(ViewNormalTex.rgb);
//	vec3 fragPos = PositionTex.rgb;
	PosRelToCam = vec4(UBO.View * vec4(PositionTex.xyz, 1.0)).xyz;
	vec4 Albedo = AlbedoTex;
	WorldPos = PositionTex.xyz;//ReconstructWorldPos(FragTexCoord, PositionTex.w, UBO.InvViewProj);


//	WorldPos = -P;
	//WorldPos = 

//	OutColor.rgb = vec3(WorldPos );
//	OutColor.a = 1.0;
//	return;

	const float Eps = 0.01;

	if (Equality(UBO.ClearColor.x, Albedo.x, Eps) &&
		Equality(UBO.ClearColor.y, Albedo.y, Eps) && 
		Equality(UBO.ClearColor.z, Albedo.z, Eps))
	{
		OutColor = Albedo;
		return;
	}

	vec4 PBR = PBRTex;

	float Metallic = PBR.x;
	float Roughness = PBR.y;
	float Occlusion = PBR.z;

	if (PositionTex.w > 900.0)
	{
		OutColor = Albedo;
		return;
	}
	
//	vec3 Normal = normalize(vec3(ViewNormalTex.w, NormalTex.x, NormalTex.y));
	vec3 Normal = unpack_normal_octahedron(vec2(ViewNormalTex.w, PBRTex.w));
	 
	float SSAO = texture(SamplerSSAO, FragTexCoord).r;


	vec3 N = Normal;
					//Upload Camera Pos
	vec3 V = normalize(UBO.CameraPosition.xyz - WorldPos.xyz);
	 
	vec3 F0 = vec3(0.04); 
	F0 = mix(F0, Albedo.xyz, Metallic);
	vec3 Lo = vec3(0.0);
	vec3 LoShadow = vec3(0.0);

  // Direction to eye in viewspace
  /*
  	vec3 P = ViewPositionFromDepth(FragTexCoord, PositionTex.w);
	vec3 V = normalize(-P);

	vec3 L = normalize(SBO.LightPos[0].xyz);

	

	vec3 LightColor = SBO.LightColor[0].xyz * vec3(SBO.LightColor[0].w);
	OutColor.rgb = DirectRadiance(P, N, V, L, L, Albedo.rgb, Roughness, Metallic, F0, LightColor);
	OutColor.a = 1.0;
//	OutColor = vec4(orenNayarFast(N, L, V, Albedo.rgb, Roughness * (PI / 2)) * LightColor, 1.0);
//	OutColor = Albedo;
	return;

	*/
	
	OutColor = vec4(1.0);

	bool HasShadow = false;

	for (uint i = 0; i < SBO.LightCount; i++)
	{
		

		vec3 Light;
		vec3 L;

		switch (SBO.LightType[i])
		{
		case 0:
			Light = SBO.LightPos[i].xyz - WorldPos.xyz;//SBO.LightPos[i].w//WorldPos.xyz;
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

		/*
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
		*/
		 
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
		//	LoShadow += (kD * Albedo.xyz / PI + specular) * radiance * NdotL;
		//	LoShadow += DirectRadiance(PositionTex.xyz, N, V, L, L, Albedo.rgb, Roughness * (PI / 2), Metallic, F0, LightColor);
		}			
		else
		{
			// Oren–Nayar diffuse instead of Lambert
			float oren = orenNayarDiffuse(N, V, L, Roughness);
			vec3 diffuse = Albedo.xyz / PI * oren;

			Lo += (kD * diffuse + specular) * radiance;  
		//	Lo += (kD * Albedo.xyz / PI + specular) * radiance * NdotL;
		//	Lo += DirectRadiance(PositionTex.xyz, N, V, L, L, Albedo.rgb, Roughness, Metallic, F0, LightColor);
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



/*
	//Main Tone mapper
	float Exposure = UBO.Exposure;
	float Gamma = UBO.Gamma;//1.3

	OutColor.rgb = Uncharted2Tonemap(OutColor.rgb * Exposure);

	const vec3 Uncharted2TonemapConst = (1.0f / Uncharted2Tonemap(vec3(11.2f)));
	OutColor.rgb = OutColor.rgb * Uncharted2TonemapConst;	
	OutColor.rgb = pow(OutColor.rgb, vec3(1.0f / Gamma));
	*/

//	OutColor.rgb = clamp(OutColor.rgb,0.0,1.0);
//textureLod
	vec3 col = OutColor.rgb;

	//Tone mappers from: https://github.com/GarrettGunnell/Post-Processing/blob/main/Assets/Tone%20Mapping/Tonemapping.shader

//Ward
	float _Ldmax = UBO.Gamma;
 	float Lin = luminance(col);

	float m = (1.219f + pow(_Ldmax / 2.0f, 0.4f)) / (1.219f + pow(Lin, 0.4f));
	m = pow(m, 2.5f); 

	float Lout = m / _Ldmax * Lin;

	vec3 Cout = col / Lin * Lout;

/*
// Schlick
	float _P = UBO.Exposure;
	float _HiVal = UBO.Gamma;

	 

	float Lin = luminance(col);

	float Lout = (_P * Lin) / (_P * Lin - Lin + _HiVal);

	vec3 Cout = col / Lin * Lout;

	
*/

 // Narkowicz ACES
//	vec3 Cout = (col*(2.51f*col+0.03f))/(col*(2.43f*col+0.59f)+0.14f);

/*
// Hill ACES
	col = ACESInputMat * col;

                col = RRTAndODTFit(col);

                vec3 Cout = ACESOutputMat * col;
*/

	OutColor.rgb = saturate(Cout);

	if (UBO.RenderSSAO == 1)
		OutColor.rgb *= SSAO;

//	OutColor.rgb = vec3(SSAO);

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