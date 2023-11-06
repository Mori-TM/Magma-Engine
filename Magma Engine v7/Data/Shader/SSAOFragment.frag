#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) out float OutColor;

layout(location = 0) in vec2 FragTexCoord;

layout (binding = 0) uniform sampler2D SamplerPositionDepth;
layout (binding = 1) uniform sampler2D SamplerViewNormal;
layout (binding = 2) uniform sampler2D SSAONoise;

#define SSAO_KERNEL_SIZE 16
#define SSAO_RADIUS 0.6f

layout (binding = 3) uniform UniformBufferObject
{
	vec4 Samples[SSAO_KERNEL_SIZE];
	mat4 Projection;
} UBO;

#define MAX_SSAO_DIST 30.0

void main() 
{
	// Get G-Buffer values
	vec3 FragPos = texture(SamplerPositionDepth, FragTexCoord).rgb;
	if (FragPos.z > MAX_SSAO_DIST || FragPos.z < -MAX_SSAO_DIST)
	{
		OutColor = 1.0;
		return;
	}

//	vec3 normal = normalize(texture(SamplerViewNormal, FragTexCoord).rgb * 2.0 - 1.0);
	vec4 NormalTex = texture(SamplerViewNormal, FragTexCoord);
	vec3 normal = (NormalTex.rgb);
//	if (NormalTex.w < 0.9)
//	{
//		OutColor = 1.0;
//		return;
//	}

	// Get a random vector using a noise lookup
	ivec2 texDim = textureSize(SamplerPositionDepth, 0); 
	ivec2 noiseDim = textureSize(SSAONoise, 0);
	const vec2 noiseUV = vec2(float(texDim.x)/float(noiseDim.x), float(texDim.y)/(noiseDim.y)) * FragTexCoord;  
	vec3 randomVec = texture(SSAONoise, noiseUV).xyz * 2.0 - 1.0;
	
	// Create TBN matrix
	vec3 tangent = normalize(randomVec - normal * dot(randomVec, normal));
	vec3 bitangent = cross(tangent, normal);
	mat3 TBN = mat3(tangent, bitangent, normal);

	// Calculate occlusion value
	float occlusion = 0.0f;
	// remove banding
	const float bias = 0.025f;
	for(int i = 0; i < SSAO_KERNEL_SIZE; i++)
	{		
		vec3 samplePos = TBN * UBO.Samples[i].xyz; 
		samplePos = FragPos + samplePos * SSAO_RADIUS; 
		
		// project
		vec4 offset = vec4(samplePos, 1.0f);
		offset = UBO.Projection * offset; 
		offset.xyz /= offset.w; 
		offset.xyz = offset.xyz * 0.5f + 0.5f; 
		
		float sampleDepth = -texture(SamplerPositionDepth, offset.xy).w; 

		float rangeCheck = smoothstep(0.0f, 1.0f, SSAO_RADIUS / abs(FragPos.z - sampleDepth));
		occlusion += (sampleDepth >= samplePos.z + bias ? 1.0f : 0.0f) * rangeCheck;           
	}
	occlusion = 1.0 - (occlusion / float(SSAO_KERNEL_SIZE));
	
	OutColor = pow(occlusion, 2.0);
//	OutColor = occlusion;
//	OutColor = 1.0;
}

