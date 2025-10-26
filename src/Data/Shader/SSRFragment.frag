#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) out vec4 OutColor;

layout(location = 0) in vec2 FragTexCoord;

layout(set = 0, binding = 0) uniform sampler2D SamplerPosition;
layout(set = 0, binding = 1) uniform sampler2D SamplerViewNormal;
layout(set = 0, binding = 2) uniform sampler2D SamplerAlbedo;
layout(set = 0, binding = 3) uniform sampler2D SamplerPBR;

layout(set = 0, binding = 4) uniform UniformBufferObject
{
	mat4 Projection;
	mat4 InvProjection;
	mat4 View;
	mat4 InvView;

	float Step;
	float MinRayStep;
	int MaxSteps;
	int NumBinarySearchSteps;
	float ReflectionSpecularFalloffExponent;
} UBO;

#define step UBO.Step
#define minRayStep UBO.MinRayStep
#define maxSteps UBO.MaxSteps
#define numBinarySearchSteps UBO.NumBinarySearchSteps
#define reflectionSpecularFalloffExponent UBO.ReflectionSpecularFalloffExponent

/*
const float step = 0.1;
const float minRayStep = 0.1;
const float maxSteps = 90;//76
const int numBinarySearchSteps = 5;//7
const float reflectionSpecularFalloffExponent = 3.0;
*/

float Metallic;

#define Scale vec3(.8, .8, .8)
#define K 19.19

vec3 PositionFromDepth(float depth);

vec3 BinarySearch(inout vec3 dir, inout vec3 hitCoord, inout float dDepth);
 
vec4 RayMarch(vec3 dir, inout vec3 hitCoord, out float dDepth);

vec3 fresnelSchlick(float cosTheta, vec3 F0);

vec3 hash(vec3 a);

const float PI = 3.141592653589793238;
const float PI2 = 6.28318530718;

float atan2(in float y, in float x)
{
    bool s = (abs(x) > abs(y));
    return mix(PI/2.0 - atan(x,y), atan(y,x), s);
}

vec3 GetPosRelToCam(vec2 UV)
{
	vec3 WorldPos = texture(SamplerPosition, UV).rgb;
	return vec4(UBO.View * vec4(WorldPos.xyz, 1.0)).xyz;
}

void main()
{
//	OutColor = vec4(texture(DepthPosition, FragTexCoord).rgb, 1.0);
//	return;

	vec2 PBR = texture(SamplerPBR, FragTexCoord).rg;
	Metallic = PBR.r;
//	Metallic = 1.0;

	if(Metallic < 0.01)
	{
		OutColor = texture(SamplerAlbedo, FragTexCoord);
		return;
	}

	vec3 Color = texture(SamplerAlbedo, FragTexCoord).rgb;

	OutColor = vec4(Color, 1.0);
//    return;

	//    discard;
 
//	vec3 viewNormal = texture(SamplerViewNormal, FragTexCoord).xyz;
	vec4 NormalTex = texture(SamplerViewNormal, FragTexCoord);
	vec3 viewNormal = normalize(NormalTex.rgb);
//	vec3 viewPos = textureLod(DepthPosition, FragTexCoord, 2).xyz;
//	vec3 viewPos = PositionFromDepth(texture(DepthPosition, FragTexCoord).w);
    vec3 viewPos = GetPosRelToCam(FragTexCoord);//texture(SamplerPosition, FragTexCoord).xyz;

	vec3 viewPosNormalized = normalize(viewPos);
	vec3 albedo = texture(SamplerAlbedo, FragTexCoord).rgb;

	float spec = PBR.g;

	vec3 F0 = vec3(0.04); 
	F0      = mix(F0, albedo, Metallic);
	vec3 Fresnel = fresnelSchlick(max(dot(viewNormal, viewPosNormalized), 0.0), F0);

	// Reflection vector
	vec3 reflected = normalize(reflect(viewPosNormalized, viewNormal));


	vec3 hitPos = viewPos;
	float dDepth;
 
//	vec3 wp = vec3(vec4(viewPos, 1.0) * UBO.InvView);
	vec3 jitt = mix(vec3(-spec / 2), vec3(hash(albedo)), spec/ 2);
	vec4 coords = RayMarch((vec3(jitt) + reflected * max(minRayStep, -viewPos.z)), hitPos, dDepth);
 
 
	vec2 dCoords = smoothstep(0.2, 0.6, abs(vec2(0.5, 0.5) - coords.xy));
 
 
	float screenEdgefactor = clamp(1.0 - (dCoords.x + dCoords.y), 0.0, 1.0);

	float ReflectionMultiplier = pow(Metallic, reflectionSpecularFalloffExponent) * 
				screenEdgefactor * 
				-reflected.z;
 
	// Get color
	const float strength = 1.0;//0.6
	vec4 SSR = vec4(texture(SamplerAlbedo, coords.xy).rgb, clamp(ReflectionMultiplier * Fresnel * strength, 0.0, 0.9));  
	vec3 blending =  SSR.rgb * SSR.a + albedo * (1.0 - SSR.a);
	OutColor = vec4(blending, 1.0);
}

vec3 PositionFromDepth(float depth) {
	float z = depth * 2.0 - 1.0;

	vec4 clipSpacePosition = vec4(FragTexCoord * 2.0 - 1.0, z, 1.0);
	vec4 viewSpacePosition = UBO.InvProjection * clipSpacePosition;

	// Perspective division
	viewSpacePosition /= viewSpacePosition.w;

	return viewSpacePosition.xyz;
}

vec3 BinarySearch(inout vec3 dir, inout vec3 hitCoord, inout float dDepth)
{
	float depth;

	vec4 projectedCoord;

	for(int i = 0; i < numBinarySearchSteps; i++)
	{

		projectedCoord = UBO.Projection * vec4(hitCoord, 1.0);
		projectedCoord.xy /= projectedCoord.w;
		projectedCoord.xy = projectedCoord.xy * 0.5 + 0.5;
 
		depth = GetPosRelToCam(projectedCoord.xy).z;//texture(SamplerPosition, projectedCoord.xy).z;

 
		dDepth = hitCoord.z - depth;

		dir *= 0.5;
		if(dDepth > 0.0)
			hitCoord += dir;
		else
			hitCoord -= dir;    
	}

		projectedCoord = UBO.Projection * vec4(hitCoord, 1.0);
		projectedCoord.xy /= projectedCoord.w;
		projectedCoord.xy = projectedCoord.xy * 0.5 + 0.5;
 
	return vec3(projectedCoord.xy, depth);
}
#define ACCURATE_SSR
vec4 RayMarch(vec3 dir, inout vec3 hitCoord, out float dDepth)
{

	dir *= step;
 
 
	float depth;
	int steps;
	vec4 projectedCoord;

 
	for(int i = 0; i < maxSteps; i++)
	{
		hitCoord += dir;
 
		projectedCoord = UBO.Projection * vec4(hitCoord, 1.0);
		projectedCoord.xy /= projectedCoord.w;
		projectedCoord.xy = projectedCoord.xy * 0.5 + 0.5;
 
		depth = GetPosRelToCam(projectedCoord.xy).z;//texture(SamplerPosition, projectedCoord.xy).z;
		if(depth > 1000.0)
			continue;
 
		dDepth = hitCoord.z - depth;

		if((dir.z - dDepth) < 1.2)
		{
			if(dDepth <= 0.0)
			{   
				vec4 Result;
				Result = vec4(BinarySearch(dir, hitCoord, dDepth), 1.0);

				return Result;
			}
		}		
		steps++;
	}
 
	
	return vec4(projectedCoord.xy, depth, 1.0);
}

vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
	return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}


vec3 hash(vec3 a)
{
	a = fract(a * Scale);
	a += dot(a, a.yxz + K);
	return fract((a.xxy + a.yxx)*a.zyx);
}