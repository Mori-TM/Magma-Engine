typedef struct
{
	mat4 Model;
} ForwardVertexPushConstant;

typedef struct
{
	vec4 Color;
	float Metallic;
	float Roughness;
	float Occlusion;
	float NearPlane;
	float FarPlane;
} ForwardFragmentPushConstant;

uint32_t ForwardDepthAttachment;
//uint32_t ForwardColorAttachment;
uint32_t ForwardRenderPass;

uint32_t ForwardDescriptorSetLayoutCount;
uint32_t ForwardLayout;
uint32_t ForwardDepthLayout;

//uint32_t ForwardCullingMode = CULL_MODE_BACK;

uint32_t ForwardPipelineNoCulling;
uint32_t ForwardPipelineBackCulling;
uint32_t ForwardPipelineFrontCulling;

uint32_t ForwardDepthPipelineNoCulling;
uint32_t ForwardDepthPipelineBackCulling;
uint32_t ForwardDepthPipelineFrontCulling;

uint32_t ForwardFramebuffer;

//uint32_t ForwardVertexUniformBuffer;


uint32_t ForwardStaticDescriptorSet;

ForwardVertexPushConstant ForwardVertexPc;
ForwardFragmentPushConstant ForwardFragmentPc;