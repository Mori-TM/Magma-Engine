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

//Update every frame mean in SceneUpdateUniformBuffer()
//FIX Change this to be only updated in SceneUpdateUniformBuffer()
typedef struct
{
	float CascadeSplits[4];	//Update every frame,	dont't change[4] to other number!
	float CascadeRange[4];	//Update via imgui or ResetSceneSettings()
	float CascadeScale[4];	//Update via imgui or ResetSceneSettings()
	float CascadeBias[4];	//Update via imgui or ResetSceneSettings()
	mat4 CascadeProjectionView[SHADOW_MAP_CASCADE_COUNT];	//Update every frame
	vec4 CameraPosition;	//Update every frame in G-Buffer pipeline, not the best place to update!
	mat4 View;				//Update every frame
	vec4 ClearColor;		//Update every frame
	float Gamma;			//Update via imgui or ResetSceneSettings()
	float Exposure;			//Update via imgui or ResetSceneSettings()
	float AmbientMultiplier;//Update via imgui or ResetSceneSettings()
	uint32_t RenderSSAO;	//Update every frame
} SceneFragmentUniformBufferObject;

#define MAX_NUMBER_OF_LIGHTS 64

typedef struct
{
	vec4 LightPos[MAX_NUMBER_OF_LIGHTS];
	vec4 LightColor[MAX_NUMBER_OF_LIGHTS];
	uint32_t LightCastShadow[MAX_NUMBER_OF_LIGHTS];
	uint32_t LightType[MAX_NUMBER_OF_LIGHTS];
	uint32_t LightCount;
	uint32_t Dummy;
} SceneFragmentStorageBufferObject;

typedef struct
{
	char Name[MAX_CHAR_NAME_LENGTH];
	char Path[MAX_CHAR_PATH_LENGTH];
	bool ShowInAssetBrowser;
	int32_t Width;
	int32_t Height;
	int32_t Format;
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
uint32_t ScenePipeline;
uint32_t SceneRenderPass;
uint32_t SceneFramebuffer;
uint32_t SceneAttachment;


uint32_t SceneMsaaAttachment;
uint32_t SceneDepthAttachment;

uint32_t SceneLightPassAttachment;
uint32_t SceneDepthPositionAttachment;
uint32_t SceneNormalMapAttachment;
uint32_t ScenePBRMapAttachment;

uint32_t SceneInputDescriptorSet;
uint32_t SceneOutputDescriptorSet;

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

uint32_t SceneFragmentStorageDescriptorSet;
uint32_t SceneFragmentStorageBuffer;

//SceneVertexPushConstant SceneVertexPc;
SceneFragmentPushConstant SceneFragmentPc;
//SceneVertexUniformBufferObject SceneVertexUBO;
SceneFragmentUniformBufferObject SceneFragmentUBO;
SceneFragmentStorageBufferObject SceneFragmentSBO;