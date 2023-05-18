#define SSAO_KERNEL_SIZE 32
#define SSAO_RADIUS 0.3
#define SSAO_NOISE_DIM 4

typedef struct
{
	vec4 Samples[SSAO_KERNEL_SIZE];
	mat4 Projection;
} SSAOFragmentUniformBufferObject;

uint32_t SSAOLayout;
uint32_t SSAOPipeline;
uint32_t SSAORenderPass;
uint32_t SSAOColorAttachment;
uint32_t SSAOFramebuffer;
uint32_t SSAODescriptorSet;
uint32_t SSAONoiseImage;
uint32_t SSAONoiseSampler;
uint32_t SSAOFragmentUniformBuffer;

SSAOFragmentUniformBufferObject SSAOFragmentUBO;