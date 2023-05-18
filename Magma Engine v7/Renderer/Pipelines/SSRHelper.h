typedef struct
{
	mat4 Projection;
	mat4 InvProjection;
	mat4 View;
	mat4 InvView;
	uint32_t MsaaSamples;
} SSRUniformBufferObject;

uint32_t SSRLayout;
uint32_t SSRPipeline;
uint32_t SSRRenderPass;
uint32_t SSRColorAttachment;
uint32_t SSRFramebuffer;
uint32_t SSRUniformBuffer;
uint32_t SSRInputDescriptorSet;
uint32_t SSROutputDescriptorSet;

SSRUniformBufferObject SSRFragmentUBO;