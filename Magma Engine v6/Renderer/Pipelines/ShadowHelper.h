typedef struct
{
	mat4 PVM;
} ShadowVertexPushConstant;

typedef struct
{
	float SplitDepth;
	mat4 ProjectionView;
} Cascade;

#define SHADOW_MAP_CASCADE_COUNT 3
#define SHADOW_MAP_WIDTH 4096 * 3
#define SHADOW_MAP_HEIGHT 4096

bool ShadowBackfaceCulling = true;
uint32_t ShadowLayout;
uint32_t ShadowPipelineNoneCull;
uint32_t ShadowPipelineBackCull;
uint32_t ShadowRenderPass;
uint32_t ShadowFramebuffer;
uint32_t ShadowDepthAttachment;

ShadowVertexPushConstant ShadowVertexPc;

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