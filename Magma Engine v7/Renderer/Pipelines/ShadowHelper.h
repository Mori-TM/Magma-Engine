typedef struct
{
	mat4 PVM;
} ShadowVertexPushConstant;

typedef struct
{
	float SplitDepth;
	mat4 ProjectionView;
	mat4 ProjectionViewBias;
} Cascade;

#define SHADOW_MAP_CASCADE_COUNT 3
uint32_t ShadowMapWidth = 2048 * SHADOW_MAP_CASCADE_COUNT;
uint32_t ShadowMapHeight = 2048;
int32_t ShadowMapSizeTmp = 2048;

uint32_t ShadowCullMode = CULL_MODE_FRONT;
uint32_t ShadowLayout;
uint32_t ShadowPipelineNoneCull;
uint32_t ShadowPipelineBackCull;
uint32_t ShadowPipelineFrontCull;
uint32_t ShadowRenderPass;
uint32_t ShadowFramebuffer;
uint32_t ShadowDepthAttachment;
uint32_t ShadowMapDescriptorSet;

ShadowVertexPushConstant ShadowVertexPc;

vec4 ShadowDirection = { 0.2, 0.8, 0.0, 0.0 };
//float CascadeSplitLambda = 0.973;
//float CascadeNearClip = 0.01;
//float CascadeFarClip = 1000.0;
//float CascadeSplitLambda = 0.96;
//float CascadeNearClip = 0.355;
//float CascadeFarClip = 564.13;
float CascadeSplitLambda = 0.95;
float CascadeNearClip = 0.5;
float CascadeFarClip = 48.0;
Cascade Cascades[SHADOW_MAP_CASCADE_COUNT];
mat4 CullingCascades[SHADOW_MAP_CASCADE_COUNT];