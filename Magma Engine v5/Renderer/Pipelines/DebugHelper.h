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