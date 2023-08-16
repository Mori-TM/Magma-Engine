typedef struct
{
	vec3 Pos;
	vec3 Color;
} DebugVertex;

typedef struct
{
	mat4 PVM;
} DebugVertexPushConstant;

uint32_t DebugLayout;
uint32_t DebugPipelineThinLine;
uint32_t DebugPipelineFatLine;

DebugVertexPushConstant DebugVertexPc;

bool RenderDebugDescriptorSet = false;
uint32_t DebugDescriptorSets[12];

const char* DebugAttachmentNames[] =
{
	"Shadow Pass",
	"G-Buffer: Pos Rel To Cam",
	"G-Buffer: View Normal",
	"G-Buffer: Albedo",
	"G-Buffer: PBR",
	"G-Buffer: World Pos",
	"G-Buffer: Normal",
	"SSAO Pass",
	"SSAO Blur Pass",
	"Scene Pass",
	"SSR Pass",
	"FXAA Pass",
};