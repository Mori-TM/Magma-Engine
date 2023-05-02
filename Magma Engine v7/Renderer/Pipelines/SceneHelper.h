typedef struct
{
	vec3 Pos;
	vec2 TexCoord;
	vec3 Normal;
} SceneVertex;

typedef struct
{
	mat4 Model;
} SceneVertexPushConstant;

typedef struct
{
	vec4 Color;
	float Metallic;
	float Roughness;
	float Occlusion;
} SceneFragmentPushConstant;

typedef struct
{
	mat4 View;
	mat4 Projection;
} SceneVertexUniformBufferObject;

typedef struct
{
	float CascadeSplits[4];//dont't change[4] to other number!
	float CascadeRange[4];
	float CascadeScale[4];
	float CascadeBias[4];
	mat4 CascadeProjectionView[SHADOW_MAP_CASCADE_COUNT];
	vec4 LightDirection;
	vec4 CameraPosition;
	mat4 View;
	float Gamma;
	float Exposure;
} SceneFragmentUniformBufferObject;

typedef struct
{
	char Name[MAX_CHAR_NAME_LENGTH];
	char Path[MAX_CHAR_PATH_LENGTH];
	bool ShowInAssetBrowser;
	int32_t Width;
	int32_t Height;
	int32_t Components;
	int32_t MipLevels;
	uint32_t TextureImage;
	uint32_t TextureSampler;
	uint32_t TextureDescriptorSet;
} SceneTextureImage;

typedef struct
{
	char Name[MAX_CHAR_NAME_LENGTH];
	uint32_t AlbedoIndex;
	uint32_t NormalIndex;
	uint32_t MetallicIndex;
	uint32_t RoughnessIndex;
	uint32_t OcclusionIndex;
	
	vec4 Color;
	float Metallic;
	float Roughness;
	float Occlusion;
} SceneMaterial;

typedef struct
{
	char Name[MAX_CHAR_NAME_LENGTH];
	char Path[MAX_CHAR_PATH_LENGTH];
	uint32_t Start;
	uint32_t End;
	float Speed;
	Md2::ModelData MeshData;

	SceneVertex* Vertices;
	uint32_t VertexBuffer;
} SceneAnimation;

typedef enum
{
	RENDER_TYPE_DEFAULT = 0,
	RENDER_TYPE_SHADOW_0 = 1,
	RENDER_TYPE_SHADOW_1 = 2,
	RENDER_TYPE_SHADOW_2 = 3,
	RENDER_TYPE_SHADOW_3 = 4,
} RenderTypes;

typedef struct
{
	SceneMaterial Material;

	uint32_t VertexOffset;
	uint32_t VertexCount;

	uint32_t IndexOffset;
	uint32_t IndexCount;

	AABBData AABB;
	bool Render[5];
} SceneMeshData;

typedef struct
{
	char Name[MAX_CHAR_NAME_LENGTH];
	char Path[MAX_CHAR_PATH_LENGTH];
	uint32_t MeshCount;
	bool Destroyable;

	uint32_t VertexBuffer;
	uint32_t IndexBuffer;
	SceneMeshData* MeshData;
} SceneMesh;

typedef struct
{
	char Name[MAX_CHAR_NAME_LENGTH];
	char Path[MAX_CHAR_PATH_LENGTH];
	char Script[MAX_CHAR_SCRIPT_LENGTH];
} SceneScript;

uint32_t ImGuiScenePosX = 0;
uint32_t ImGuiScenePosY = 0;

uint32_t ImGuiSceneWidth = 1600;
uint32_t ImGuiSceneHeight = 900;

uint32_t SceneWidth = 1600;
uint32_t SceneHeight = 900;

uint32_t SceneScaling = 100;

uint32_t SceneCullMode = CULL_MODE_BACK;
uint32_t SceneLayout;
uint32_t ScenePipelineNoneCull;
uint32_t ScenePipelineBackCull;
uint32_t ScenePipelineFrontCull;
uint32_t SceneRenderPass;
uint32_t SceneFramebuffer;
uint32_t SceneMsaaAttachment;
uint32_t SceneDepthAttachment;

uint32_t SceneLightPassAttachment;
uint32_t SceneDepthPositionAttachment;
uint32_t SceneNormalMapAttachment;
uint32_t ScenePBRMapAttachment;

uint32_t SceneDescriptorSet;

uint32_t SceneShadowMapDescriptorSet;

CMA_MemoryZone SceneTextures;
CMA_MemoryZone SceneMeshes;
CMA_MemoryZone SceneMaterials;
CMA_MemoryZone SceneAnimations;

uint32_t SceneScriptCount = 0;
SceneScript* SceneScripts = NULL;

uint32_t SceneVertexUniformDescriptorSet;
uint32_t SceneVertexUniformBuffer;

uint32_t SceneFragmentUniformDescriptorSet;
uint32_t SceneFragmentUniformBuffer;

SceneVertexPushConstant SceneVertexPc;
SceneFragmentPushConstant SceneFragmentPc;
SceneVertexUniformBufferObject SceneVertexUBO;
SceneFragmentUniformBufferObject SceneFragmentUBO;