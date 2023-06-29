typedef struct
{
	mat4 Model;
} GBufferVertexPushConstant;

typedef struct
{
	vec4 Color;
	float Metallic;
	float Roughness;
	float Occlusion;
	float NearPlane;
	float FarPlane;
} GBufferFragmentPushConstant;

typedef struct
{
	mat4 View;
	mat4 Projection;
} GBufferVertexUniformBufferObject;

uint32_t GBufferCullMode = CULL_MODE_BACK;
uint32_t GBufferLayout;
uint32_t GBufferPipelineNoneCull;
uint32_t GBufferPipelineBackCull;
uint32_t GBufferPipelineFrontCull;
uint32_t GBufferRenderPass;
#define G_BUFFER_ATTACHMENT_COUNT 7
uint32_t GBufferAttachments[G_BUFFER_ATTACHMENT_COUNT];
uint32_t GBufferFramebuffer;
//uint32_t GBufferDescriptorSet;
uint32_t GBufferVertexUniformDescriptorSet;
uint32_t GBufferVertexUniformBuffer;

GBufferVertexUniformBufferObject GBufferVertexUBO;
GBufferVertexPushConstant GBufferVertexPc;
GBufferFragmentPushConstant GBufferFragmentPc;