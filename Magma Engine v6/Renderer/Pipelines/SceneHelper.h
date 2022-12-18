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
	float CascadeSplits[4];
	mat4 CascadeProjectionView[SHADOW_MAP_CASCADE_COUNT];
	vec4 LightDirection;
	vec4 CameraPosition;
} SceneFragmentUniformBufferObject;

typedef struct
{
	char Name[MAX_CHAR_NAME_LENGTH];
	char Path[MAX_CHAR_PATH_LENGTH];
	bool ShowInAssetBrowser;
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

typedef struct
{
	//use the SceneMaterial struct
	uint32_t AlbedoIndex;
	uint32_t NormalIndex;
	uint32_t MetallicIndex;
	uint32_t RoughnessIndex;
	uint32_t OcclusionIndex;

	vec4 Color;
	float Metallic;
	float Roughness;
	float Occlusion;

	uint32_t VertexCount;
	SceneVertex* Vertices;
	uint32_t VertexBuffer;

	uint32_t IndexCount;
	uint32_t* Indices;
	uint32_t IndexBuffer;
} SceneMeshData;

typedef struct
{
	char Name[MAX_CHAR_NAME_LENGTH];
	char Path[MAX_CHAR_PATH_LENGTH];
	bool Freeable;
	uint32_t MeshCount;
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

bool SceneBackfaceCulling = true;
uint32_t SceneLayout;
uint32_t ScenePipelineBackCull;
uint32_t ScenePipelineNoneCull;
uint32_t SceneRenderPass;
uint32_t SceneFramebuffer;
uint32_t SceneMsaaAttachment;
uint32_t SceneDepthAttachment;
uint32_t SceneColorAttachment;
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

uint32_t LoadTexture(char* Path, uint32_t* TextureImage, uint32_t* TextureSampler);

void InitScene()
{
	/*
	* Just call AddTexture, AddMaterial and so on
	*/

	//Texture
	SceneTextures = CMA_Create(sizeof(SceneTextureImage));
	SceneTextureImage Image;
	Image.ShowInAssetBrowser = false;
	Image.TextureDescriptorSet = LoadTexture((char*)"Data/Textures/Default.png", &Image.TextureImage, &Image.TextureSampler);
	strcpy(Image.Name, "None");
	strcpy(Image.Path, "None");
	CMA_Push(&SceneTextures, &Image);
	//Material
	SceneMaterials = CMA_Create(sizeof(SceneMaterial));
	SceneMaterial Material;
	strcpy(Material.Name, "None");
	Material.Color = Vec4f(1.0);
	Material.Metallic = 0.0;
	Material.Roughness = 1.0;
	Material.Occlusion = 1.0;

	Material.AlbedoIndex = 0;
	Material.NormalIndex = 0;
	Material.MetallicIndex = 0;
	Material.RoughnessIndex = 0;
	Material.OcclusionIndex = 0;
	CMA_Push(&SceneMaterials, &Material);
	//Mesh
	SceneMeshes = CMA_Create(sizeof(SceneMesh));
	SceneMesh Mesh;
	Mesh.MeshData = (SceneMeshData*)malloc(1 * sizeof(SceneMeshData));
	Mesh.Freeable = false;
	strcpy(Mesh.Name, "None");
	strcpy(Mesh.Path, "None");
	Mesh.MeshCount = 0;
	
	Mesh.MeshData[0].Color = Vec4f(1.0);
	Mesh.MeshData[0].Metallic = 0.0;
	Mesh.MeshData[0].Roughness = 1.0;
	Mesh.MeshData[0].Occlusion = 1.0;
	
	Mesh.MeshData[0].AlbedoIndex = 0;
	Mesh.MeshData[0].NormalIndex = 0;
	Mesh.MeshData[0].MetallicIndex = 0;
	Mesh.MeshData[0].RoughnessIndex = 0;
	Mesh.MeshData[0].OcclusionIndex = 0;

	Mesh.MeshData[0].VertexCount = 0;
	Mesh.MeshData[0].Vertices = NULL;
	Mesh.MeshData[0].VertexBuffer = 0;
	Mesh.MeshData[0].IndexCount = 0;
	Mesh.MeshData[0].Indices = NULL;
	Mesh.MeshData[0].IndexBuffer = 0;
	CMA_Push(&SceneMeshes, &Mesh);

	SceneAnimations = CMA_Create(sizeof(SceneAnimation));
	SceneAnimation Animation;
	Animation.Start = 0;
	Animation.End = 1;
	Animation.Speed = 25.0;
	Animation.MeshData.NumFrames = 1;
	Animation.Vertices = NULL;
	Animation.VertexBuffer = 0;
	CMA_Push(&SceneAnimations, &Animation);
}

void DestroyScene()
{
	CMA_Destroy(&SceneTextures);
	CMA_Destroy(&SceneMaterials);
	CMA_Destroy(&SceneMeshes);
	CMA_Destroy(&SceneAnimations);
	free(SceneScripts);
}