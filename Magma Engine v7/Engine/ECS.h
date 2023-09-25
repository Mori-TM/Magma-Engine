#define COMPONENT_COUNT 6
#define ENTITY_ALLOCATION_COUNT 32

typedef enum
{
	//Add Empty entity
	COMPONENT_TYPE_MESH = 0,
	COMPONENT_TYPE_MATERIAL = 1,
	COMPONENT_TYPE_CAMERA = 2,
	COMPONENT_TYPE_AUDIO = 3,
	COMPONENT_TYPE_ANIMATION = 4,
	COMPONENT_TYPE_LIGHT = 5,
} ComponentTypes;

typedef struct
{
	char Name[MAX_CHAR_NAME_LENGTH];
	uint32_t MeshIndex;
} MeshComponent;

typedef struct
{
	char Name[MAX_CHAR_NAME_LENGTH];
	uint32_t MaterialIndex;
} MaterialComponent;

typedef struct
{
	char Name[MAX_CHAR_NAME_LENGTH];
	float FOV;
	float NearPlane;
	float FarPlane;
} CameraComponent;

typedef struct
{
	char Name[MAX_CHAR_NAME_LENGTH];
	SoLoud::Wav Audio;
	SoLoud::handle Handle;
} AudioComponent;

typedef struct
{
	char Name[MAX_CHAR_NAME_LENGTH];
	uint32_t AnimationIndex;
} AnimationComponent;

typedef struct
{
	char Name[MAX_CHAR_NAME_LENGTH];
	vec3 Color;
	float Strength;
	uint32_t Type;
	bool CastShadow;
} LightComponent;

typedef struct
{
	char Name[MAX_CHAR_NAME_LENGTH];
	vec3 Translate;
	vec3 Rotate;
	vec3 Scale;
	bool Selected;

	bool UsedComponents[COMPONENT_COUNT];
	MeshComponent Mesh;
	MaterialComponent Material;
	CameraComponent Camera;
	AudioComponent Audio;
	AnimationComponent Animation;
	LightComponent Light;
} EntityInfo;

uint32_t SelectedEntity = 0;
uint32_t SelectedTexture = 0;
uint32_t SelectedMaterial = 0;
uint32_t SelectedMesh = 0;
uint32_t SelectedAudio = 0;
uint32_t SelectedAnimation = 0;
uint32_t SelectedScript = 0;

uint32_t EntityCount = 0;
uint32_t EntityAllocatedCount = 0;
EntityInfo* Entities = NULL;

void EntitiesInit()
{
	EntityAllocatedCount = ENTITY_ALLOCATION_COUNT;
	Entities = (EntityInfo*)malloc(ENTITY_ALLOCATION_COUNT * sizeof(EntityInfo));
	if (!Entities)
	{
		printf("Error, your system doesn't have enough ram for %d entities, buy more than 1 mb of ram", ENTITY_ALLOCATION_COUNT);
		exit(0);
	}
}

void EntitiesDestroy()
{
	free(Entities);
}

bool EntitiesCheckForResize()
{
	if (EntityCount + 1 >= EntityAllocatedCount)
	{		
		EntityInfo* EntitiesTmp = (EntityInfo*)realloc(Entities, (EntityAllocatedCount + ENTITY_ALLOCATION_COUNT) * sizeof(EntityInfo));
		if (!EntitiesTmp)
		{
			printf("Failed to allocate more entities\n");
			return false;
		}
	//	printf("Curr: %d\n", EntityAllocatedCount + ENTITY_ALLOCATION_COUNT);
		Entities = EntitiesTmp;
		EntityAllocatedCount += ENTITY_ALLOCATION_COUNT;
	}

	else if (EntityCount > ENTITY_ALLOCATION_COUNT && 
			 EntityCount < EntityAllocatedCount - ENTITY_ALLOCATION_COUNT)
	{
		EntityInfo* EntitiesTmp = (EntityInfo*)realloc(Entities, (EntityAllocatedCount - ENTITY_ALLOCATION_COUNT) * sizeof(EntityInfo));
		if (!EntitiesTmp)
		{
			printf("Failed to allocate less entities\n");
			return false;
		}

		Entities = EntitiesTmp;
		EntityAllocatedCount -= ENTITY_ALLOCATION_COUNT;
	}
	
	return true;
}

char MaterialName[MAX_CHAR_NAME_LENGTH] = "Material";
vec4 MaterialColor = Vec4f(1.0f);
float MaterialMetallic = 0.0;
float MaterialRoughness = 1.0;
float MaterialOcclusion = 1.0;

void SetDefaultMaterial(SceneMaterial* Material, const char* Name)
{
	strcpy(Material->Name, Name);
	Material->Color = MaterialColor;
	Material->Metallic = MaterialMetallic;
	Material->Roughness = MaterialRoughness;
	Material->Occlusion = MaterialOcclusion;

	Material->AlbedoIndex = 0;
	Material->NormalIndex = 0;
	Material->MetallicIndex = 0;
	Material->RoughnessIndex = 0;
	Material->OcclusionIndex = 0;
}

void ResetEntityMesh(EntityInfo* Entity)
{
	Entity->Mesh.MeshIndex = 0;
	strcpy(Entity->Mesh.Name, "None");
}

void ResetEntityMaterial(EntityInfo* Entity)
{
	Entity->Material.MaterialIndex = 0;
	strcpy(Entity->Material.Name, "None");
}

void ResetEntityCamera(EntityInfo* Entity)
{
	strcpy(Entity->Camera.Name, "None");
	Entity->Camera.FOV = 75.0;
	Entity->Camera.NearPlane = 0.01;
	Entity->Camera.FarPlane = 1000.0;
}

void ResetEntityAnimation(EntityInfo* Entity)
{
	strcpy(Entity->Animation.Name, "None");
	Entity->Animation.AnimationIndex = 0;
}


void ResetEntityLight(EntityInfo* Entity)
{
	strcpy(Entity->Light.Name, "None");
	Entity->Light.Color = Vec3f(1.0);
	Entity->Light.Strength = 1.0;
	Entity->Light.Type = 0;
	Entity->Light.CastShadow = false;
}

uint32_t AddEntity(uint32_t UsedComponent)
{
	if (!EntitiesCheckForResize())
		return SelectedEntity;

	Mutex.lock();
	EntityInfo Entity;
	memset(Entity.UsedComponents, 0, ARRAY_SIZE(Entity.UsedComponents));
//	for (uint32_t i = 0; i < COMPONENT_COUNT; i++)
//		Entity.UsedComponents[i] = false;

	if (UsedComponent != UINT32_MAX)
		Entity.UsedComponents[UsedComponent] = true;

	ResetEntityMesh(&Entity);
	ResetEntityMaterial(&Entity);
	ResetEntityCamera(&Entity);
	ResetEntityAnimation(&Entity);
	ResetEntityLight(&Entity);

	Entity.Translate = Vec3f(0.0);
	Entity.Rotate = Vec3f(0.0);
	Entity.Scale = Vec3f(1.0);
	Entity.Selected = false;

	uint32_t Count = 0;
	for (uint32_t i = 0; i < EntityCount; i++)
		if (strstr(Entities[i].Name, "Entity") != 0)
			Count++;

	if (Count > 0)
		sprintf(Entity.Name, "Entity (%d)", Count);
	else
		strcpy(Entity.Name, "Entity");

//	Entities = (EntityInfo*)realloc(Entities, (EntityCount + 1) * sizeof(EntityInfo));
//	if (EntitiesCheckForResize())
//	{
		Entities[EntityCount] = Entity;
		SelectedEntity = EntityCount++;
//	}
//	else
//		SelectedEntity = EntityCount - 1;

	Mutex.unlock();

	return SelectedEntity;
}

void AddMeshToEntity(uint32_t EntityIndex, uint32_t MeshIndex)
{
	SceneMesh* Mesh = (SceneMesh*)CMA_GetAt(&SceneMeshes, MeshIndex);
	
	Entities[EntityIndex].Mesh.MeshIndex = MeshIndex;
	strcpy(Entities[EntityIndex].Mesh.Name, Mesh->Name);
}

uint32_t AddMesh(const char* Name, SceneMesh* MeshInfo)
{
	Mutex.lock();
	CheckForSameNames(&SceneMeshes, Name, MeshInfo->Name);

	SelectedMesh = CMA_Push(&SceneMeshes, MeshInfo);
	Mutex.unlock();

	return SelectedMesh;
}

uint32_t AddMaterial()
{
	SceneMaterial Material;
	SetDefaultMaterial(&Material, MaterialName);

	CheckForSameNames(&SceneMaterials, MaterialName, Material.Name);

	SelectedMaterial = CMA_Push(&SceneMaterials, &Material);

	return SelectedMesh;
}

bool LoadTextureCompressed = false;
bool TextureCompressedHQ = false;
bool GenerateMipMaps = true;
bool UseCustomMipLevels = false;
uint32_t CustomMipLevels = 5;

typedef struct
{
	size_t BlockSize;
	unsigned char* Block;
} CompressionBlock;

uint32_t LoadTexture(char* Path, SceneTextureImage* Image)
{
	printf("%s\n", Path);
	OpenVkDeviceWaitIdle();

	unsigned char* Pixels = NULL;
	Image->Format = OPENVK_FORMAT_RGBA;

	if (OpenVkLoadTexture(Path, false, &Pixels, &Image->Width, &Image->Height, OPENVK_FORMAT_RGBA) == OpenVkTrue)
	{
		if (LoadTextureCompressed &&
			Image->Width % 4 == 0 &&
			Image->Height % 4 == 0)
		{
			Image->Format = OPENVK_FORMAT_BC1_RGB;

			if (GenerateMipMaps)
			{
				uint32_t MipLevels = floorf(log2f(MAX(Image->Width, Image->Height))) + 1;
				if (UseCustomMipLevels) MipLevels = CustomMipLevels;

				unsigned char** Blocks = (unsigned char**)malloc(MipLevels * sizeof(unsigned char*));
				if (!Blocks)
				{
					printf("Failed to allocate blocks: %s\n", Path);
					free(Pixels);
					goto Error;
				}
				size_t BlockSize = 0;
				if (!CompressImage(Pixels, Image->Width, Image->Height, &Blocks[0], &BlockSize, Image->Format, TextureCompressedHQ))
				{
					printf("Failed to compress block 0: %s\n", Path);
					free(Blocks);
					free(Pixels);
					goto Error;
				}

				int32_t MipWidth = Image->Width;
				int32_t MipHeight = Image->Height;
				//Only use rgba!!
				unsigned char* ResizeData = (unsigned char*)malloc(Image->Width * Image->Height * 4);
				if (!ResizeData)
				{
					printf("Failed to allocate resize data: %s\n", Path);
					free(Blocks);
					free(Pixels);
					goto Error;
				}

				for (uint32_t i = 1; i < MipLevels; i++)
				{
					stbir_resize_uint8(Pixels, Image->Width, Image->Height, 0, ResizeData, MipWidth > 1 ? MipWidth / 2 : 1, MipHeight > 1 ? MipHeight / 2 : 1, 0, 4);
					if (!CompressImage(ResizeData, MipWidth > 1 ? MipWidth / 2 : 1, MipHeight > 1 ? MipHeight / 2 : 1, &Blocks[i], &BlockSize, Image->Format, TextureCompressedHQ))
					{
						MipLevels = i;
						break;
					//	printf("Failed to compress block %d: %s\n", i, Path);
					//	for (uint32_t j = 0; j < i; j++)
					//		free(Blocks[j]);
					//
					//	free(Blocks);
					//	free(Pixels);
					//	goto Error;
					}

				//	printf("%d\n", BlockSize);

					if (MipWidth > 1) MipWidth /= 2;
					if (MipHeight > 1) MipHeight /= 2;

					if (BlockSize == 0)
					{
						MipLevels = i;
						break;
					}
				}

				free(ResizeData);

				OpenVkTextureCreateInfo TextureCreateInfo;
				TextureCreateInfo.Pixels = Blocks;
				TextureCreateInfo.Width = Image->Width;
				TextureCreateInfo.Height = Image->Height;
				TextureCreateInfo.Format = Image->Format;
				TextureCreateInfo.MipLevels = MipLevels;
				TextureCreateInfo.GenerateMipmaps = OpenVkTrue;
				TextureCreateInfo.UseCustomMipmaps = OpenVkTrue;
				Image->TextureImage = OpenVkCreateTexture(&TextureCreateInfo);

				Image->MipLevels = TextureCreateInfo.MipLevels;

				for (uint32_t i = 0; i < MipLevels; i++)
					free(Blocks[i]);

				free(Blocks);
			}
			else
			{
				size_t BlockSize = 0;
				unsigned char* Block;
				if (!CompressImage(Pixels, Image->Width, Image->Height, &Block, &BlockSize, Image->Format, TextureCompressedHQ))
				{
					printf("Failed to compress image: %s\n", Path);
					free(Pixels);
					goto Error;
				}

				OpenVkTextureCreateInfo TextureCreateInfo;
				TextureCreateInfo.Pixels = &Block;
				TextureCreateInfo.Width = Image->Width;
				TextureCreateInfo.Height = Image->Height;
				TextureCreateInfo.Format = Image->Format;
				TextureCreateInfo.MipLevels = 0;
				TextureCreateInfo.GenerateMipmaps = OpenVkFalse;
				TextureCreateInfo.UseCustomMipmaps = OpenVkFalse;
				Image->TextureImage = OpenVkCreateTexture(&TextureCreateInfo);
				Image->MipLevels = TextureCreateInfo.MipLevels;

				OpenVkFree(Block);
			}
			
			OpenVkFree(Pixels);
			printf("Its compressed\n");
		}
		else
		{
			OpenVkTextureCreateInfo TextureCreateInfo;
			TextureCreateInfo.Pixels = &Pixels;
			TextureCreateInfo.Width = Image->Width;
			TextureCreateInfo.Height = Image->Height;
			TextureCreateInfo.Format = Image->Format;
			TextureCreateInfo.MipLevels = UseCustomMipLevels ? CustomMipLevels : 0;
			TextureCreateInfo.GenerateMipmaps = GenerateMipMaps ? OpenVkTrue : OpenVkFalse;
			TextureCreateInfo.UseCustomMipmaps = OpenVkFalse;
			Image->TextureImage = OpenVkCreateTexture(&TextureCreateInfo);
			Image->MipLevels = TextureCreateInfo.MipLevels;

			OpenVkFree(Pixels);
		}


		if (Image->TextureImage == OPENVK_ERROR)
			goto Error;

		uint32_t Filter = OPENVK_FILTER_LINEAR;
		if (Image->Width <= 64 && Image->Height <= 64)
			Filter = OPENVK_FILTER_NEAREST;

		Image->TextureSampler = OpenVkCreateImageSampler(Filter, OPENVK_ADDRESS_MODE_REPEAT);
		if (Image->TextureSampler == OPENVK_ERROR)
		{
			Image->TextureSampler = ImageSampler;
			printf("Failed to create sampler for: %s, will use default one\n", Path);
		}			
	}
	else
	{
	Error:
		SceneTextureImage* DefaultImage = (SceneTextureImage*)CMA_GetAt(&SceneTextures, 0);
		if (DefaultImage != NULL)
		{
			memcpy(Image, DefaultImage, sizeof(SceneTextureImage));
			return DefaultImage->TextureDescriptorSet;
		}
		else
			printf("Du Kleiner Hurensohn bist gefickt\n");
	}

	uint32_t DescriptorCounts[] = { 1 };
	uint32_t DescriptorTypes[] = { OPENVK_DESCRIPTOR_TYPE_IMAGE_SAMPLER };
	uint32_t ImageTypes[] = { OPENVK_IMAGE_TYPE_TEXTURE };
	uint32_t ImageLayouts[] = { OPENVK_IMAGE_LAYOUT_COLOR_OUTPUT };
	uint32_t Bindings[] = { 0 };

	OpenVkDescriptorSetCreateInfo DescriptorSetCreateInfo;
	DescriptorSetCreateInfo.DescriptorSetLayout = TextureDescriptorSetLayout;
	DescriptorSetCreateInfo.DescriptorPool = DescriptorPool;
	DescriptorSetCreateInfo.DescriptorWriteCount = 1;
	DescriptorSetCreateInfo.DescriptorCounts = DescriptorCounts;
	DescriptorSetCreateInfo.DescriptorTypes = DescriptorTypes;
	DescriptorSetCreateInfo.Sampler = &Image->TextureSampler;
	DescriptorSetCreateInfo.ImageTypes = ImageTypes;
	DescriptorSetCreateInfo.ImageLayouts = ImageLayouts;
	DescriptorSetCreateInfo.Bindings = Bindings;
	DescriptorSetCreateInfo.Images = &Image->TextureImage;
	DescriptorSetCreateInfo.DescriptorSet = NULL;
	DescriptorSetCreateInfo.VariableDescriptorSetCount = 0;

	return OpenVkCreateDescriptorSet(&DescriptorSetCreateInfo);
}


uint32_t AddTexture(char* Path, bool ShowInAssetBrowser)
{
	SceneTextureImage Image;
	Image.ShowInAssetBrowser = ShowInAssetBrowser;
	Image.TextureDescriptorSet = LoadTexture(Path, &Image);
	strcpy(Image.Path, Path);

	char* Name = GetFileNameFromPath(Path);
	CheckForSameNames(&SceneTextures, Name, Image.Name);

	SelectedTexture = CMA_Push(&SceneTextures, &Image);

	return SelectedTexture;
}

//Fix memory
uint32_t AddAnimation(char* Path, int32_t TexWidth, int32_t TexHeight)
{
	SceneAnimation Animation;
	Animation.Start = 0;
	Animation.End = 1;
	Animation.Speed = 25.0;
	Animation.MeshData.NumFrames = 1;
	if (!Md2::Load(Path, TexWidth, TexHeight, &Animation.MeshData))
		return ERROR32;

	Animation.VertexBuffer = OpenVkCreateDynamicVertexBuffer(Animation.MeshData.NumTriangles * 3 * sizeof(SceneVertex));
	Animation.Vertices = (SceneVertex*)malloc(Animation.MeshData.NumTriangles * 3 * sizeof(SceneVertex));
	strcpy(Animation.Path, Path);

	char* Name = GetFileNameFromPath(Path);
	CheckForSameNames(&SceneAnimations, Name, Animation.Name);

	SelectedAnimation = CMA_Push(&SceneAnimations, &Animation);
	
	return SelectedAnimation;
}

//Fix memory
void AddScript()
{
	SceneScripts = (SceneScript*)realloc(SceneScripts, (SceneScriptCount + 1) * sizeof(SceneScript));
	SelectedScript = SceneScriptCount;

	char* ScriptName = (char*)"Script";
	char* Script = (char*)"function Start()\nend\n\nfunction Update()\n\tx, y, z, rx, ry, rz, sx, sy, sz = GetEntity(\"Entity\")\n\tprint(x, y, z, rx, ry, rz, sx, sy, sz)\nend";
	strcpy(SceneScripts[SceneScriptCount].Name, ScriptName);
	strcpy(SceneScripts[SceneScriptCount].Path, "None");
	strcpy(SceneScripts[SceneScriptCount].Script, Script);

	uint32_t Count = 0;
	for (uint32_t i = 0; i < SceneScriptCount; i++)
		if (strstr(SceneScripts[i].Name, ScriptName) != 0)
			Count++;

	if (Count > 0)
		sprintf(SceneScripts[SceneScriptCount].Name, "%s (%d)", ScriptName, Count);
	else
		strcpy(SceneScripts[SceneScriptCount].Name, ScriptName);

	SceneScriptCount++;
}

bool ModelLoadAlbedo = true;
bool ModelLoadNormal = true;
bool ModelLoadMetallic = true;
bool ModelLoadRoughness = true;
bool ModelLoadOcclusion = true;
vec4  ModelColor = { 1.0, 1.0, 1.0, 1.0 };
float ModelMetallic = 0.0;
float ModelRoughness = 1.0;
float ModelOcclusion = 1.0;
bool ModelLoadMaterials = true;
bool ModelGenFlatNormals = false;
bool ModelGenSmoothNormals = false;
bool ModelFlipUVs = true;
//uint32_t ModelSettings = WAVE_LOAD_MATERIAL | WAVE_GEN_NORMALS | WAVE_FLIP_UVS | WAVE_GEN_UVS | WAVE_GEN_INDICES | WAVE_MATERIAL_USE_MODEL_PATH | WAVE_REMOVE_REDUNDANT_MATERIALS | WAVE_PRINT_DEBUG_INOFS;

bool LoadModelWave(const char* Path, WaveModelData* ModelData, SceneMesh* MeshInfo)
{
	MeshInfo->Destroyable = true;
	strcpy(MeshInfo->Path, Path);
	MeshInfo->MeshData = (SceneMeshData*)malloc(ModelData->MeshCount * sizeof(SceneMeshData));
	if (!MeshInfo->MeshData)
	{
		printf("Failed to allocate meshes: %s\n", Path);
		return false;
	}
	MeshInfo->MeshCount = ModelData->MeshCount;

	uint32_t VertexCount = 0;
	uint32_t IndexCount = 0;

	for (uint32_t i = 0; i < ModelData->MeshCount; i++)
	{
		WaveMeshData* WaveMesh = &ModelData->Meshes[i];
		VertexCount += WaveMesh->VertexCount;
		IndexCount += WaveMesh->IndexCount;
	}

	SceneVertex* Vertices = (SceneVertex*)malloc(VertexCount * sizeof(SceneVertex));
	if (!Vertices)
	{
		printf("Failed to allocate Vertices: %s\n", Path);
		return false;
	}
	uint32_t* Indices = (uint32_t*)malloc(IndexCount * sizeof(uint32_t));
	if (!Indices)
	{
		printf("Failed to allocate Indices: %s\n", Path);
		return false;
	}

	VertexCount = 0;
	IndexCount = 0;

	for (uint32_t i = 0; i < ModelData->MeshCount; i++)
	{
		WaveMeshData* WaveMesh = &ModelData->Meshes[i];
		SceneMeshData* SceneMesh = &MeshInfo->MeshData[i];
		memset(&SceneMesh->Render, 1, ARRAY_SIZE(SceneMesh->Render) * sizeof(bool));

		SetDefaultMaterial(&SceneMesh->Material, "MESH");
		if (strcmp(ModelData->Materials[i].MaterialName, WaveEmptyMaterial.MaterialName) != 0)
			strncpy(SceneMesh->Material.Name, ModelData->Materials[i].MaterialName, sizeof(SceneMesh->Material.Name));

		SceneMesh->VertexCount = WaveMesh->VertexCount;
		SceneMesh->IndexCount = WaveMesh->IndexCount;

		SceneMesh->VertexOffset = VertexCount;
		SceneMesh->IndexOffset = IndexCount;

		SceneMesh->Material.Metallic = ModelData->Materials[i].Metallic;
		SceneMesh->Material.Roughness = ModelData->Materials[i].Roughness;

		if (ModelLoadAlbedo && strcmp(ModelData->Materials[i].DiffuseTexture, "NoTexture") != 0)
			SceneMesh->Material.AlbedoIndex = AddTexture(ModelData->Materials[i].DiffuseTexture, false);

		if (ModelLoadNormal)
		{
			if (strcmp(ModelData->Materials[i].NormalTexture, "NoTexture") != 0)
				SceneMesh->Material.NormalIndex = AddTexture(ModelData->Materials[i].NormalTexture, false);
			else if (strcmp(ModelData->Materials[i].DisplacmentTexture, "NoTexture") != 0)
				SceneMesh->Material.NormalIndex = AddTexture(ModelData->Materials[i].DisplacmentTexture, false);
			else if (strcmp(ModelData->Materials[i].BumpTexture, "NoTexture") != 0)
				SceneMesh->Material.NormalIndex = AddTexture(ModelData->Materials[i].BumpTexture, false);
		}
		if (ModelLoadRoughness)
		{
			if (strcmp(ModelData->Materials[i].RoughnessTexture, "NoTexture") != 0)
				SceneMesh->Material.RoughnessIndex = AddTexture(ModelData->Materials[i].RoughnessTexture, false);
			else if (strcmp(ModelData->Materials[i].SpecularTexture, "NoTexture") != 0)
				SceneMesh->Material.RoughnessIndex = AddTexture(ModelData->Materials[i].SpecularTexture, false);
		}
		
		if (ModelLoadMetallic && strcmp(ModelData->Materials[i].MetallicTexture, "NoTexture") != 0)
			SceneMesh->Material.MetallicIndex = AddTexture(ModelData->Materials[i].MetallicTexture, false);

		SceneMesh->Material.Color.x = ModelData->Materials[i].DiffuseColor.x;
		SceneMesh->Material.Color.y = ModelData->Materials[i].DiffuseColor.y;
		SceneMesh->Material.Color.z = ModelData->Materials[i].DiffuseColor.z;
		SceneMesh->Material.Color.w = ModelData->Materials[i].Dissolve;

		for (uint32_t j = 0; j < SceneMesh->VertexCount; j++)
		{
			Vertices[VertexCount].Pos.x = WaveMesh->Vertices[j].Vertices.x;
			Vertices[VertexCount].Pos.y = WaveMesh->Vertices[j].Vertices.y;
			Vertices[VertexCount].Pos.z = WaveMesh->Vertices[j].Vertices.z;
			Vertices[VertexCount].Normal.x = WaveMesh->Vertices[j].Normals.x;
			Vertices[VertexCount].Normal.y = WaveMesh->Vertices[j].Normals.y;
			Vertices[VertexCount].Normal.z = WaveMesh->Vertices[j].Normals.z;
			Vertices[VertexCount].TexCoord.x = WaveMesh->Vertices[j].TexCoords.x;
			Vertices[VertexCount].TexCoord.y = WaveMesh->Vertices[j].TexCoords.y;
			VertexCount++;
		}

		for (uint32_t j = 0; j < SceneMesh->IndexCount; j++)
			Indices[IndexCount++] = WaveMesh->Indices[j];

		GenerateAABB(&MeshInfo->MeshData[i].AABB, SceneMesh->VertexCount, Vertices + SceneMesh->VertexOffset);

		//	VertexCount += SceneMesh->VertexCount;
		//	IndexCount += SceneMesh->IndexCount;
	}

	MeshInfo->VertexBuffer = OpenVkCreateVertexBuffer(VertexCount * sizeof(SceneVertex), Vertices);
	if (MeshInfo->VertexBuffer == OPENVK_ERROR)
	{
		free(Vertices);
		free(Indices);
		return false;
	}		

	MeshInfo->IndexBuffer = OpenVkCreateIndexBuffer(IndexCount * sizeof(uint32_t), Indices);
	if (MeshInfo->IndexBuffer == OPENVK_ERROR)
	{
		free(Vertices);
		free(Indices);
		return false;
	}

	free(Vertices);
	free(Indices);
}

uint32_t AddModel(uint32_t Settings, const char* FileName)
{
	char LastPath[MAX_CHAR_PATH_LENGTH];
	char Path[MAX_CHAR_PATH_LENGTH];
	strcpy(Path, FileName);
	for (uint32_t i = strlen(FileName) - 1; i > 0; i--)
		if (Path[i] == '\\' || Path[i] == '/')
		{
			Path[i] = '\0';
			break;
		}

	SceneMesh MeshInfo;

	uint32_t LastTexture = SelectedTexture;

	memcpy(&WaveEmptyMaterial.DiffuseColor, &ModelColor, sizeof(WaveVec3));
	WaveEmptyMaterial.Dissolve = ModelColor.w;
	WaveEmptyMaterial.Metallic = ModelMetallic;
	WaveEmptyMaterial.Roughness = ModelRoughness;
	WaveEmptyMaterial.SpecularColor.x = ModelRoughness;
	WaveEmptyMaterial.SpecularColor.y = ModelRoughness;
	WaveEmptyMaterial.SpecularColor.z = ModelRoughness;

//	Settings |= WAVE_LOAD_MATERIAL | WAVE_GEN_NORMALS | WAVE_FLIP_UVS | WAVE_GEN_UVS | WAVE_GEN_INDICES | WAVE_MATERIAL_USE_MODEL_PATH | WAVE_REMOVE_REDUNDANT_MATERIALS | WAVE_PRINT_DEBUG_INOFS;

	Settings = WAVE_GEN_UVS | WAVE_GEN_NORMALS | WAVE_GEN_INDICES | WAVE_MATERIAL_USE_MODEL_PATH | WAVE_REMOVE_REDUNDANT_MATERIALS | WAVE_PRINT_DEBUG_INOFS;

	if (ModelLoadMaterials)		Settings |= WAVE_LOAD_MATERIAL;
	if (ModelGenFlatNormals)	Settings |= WAVE_FORCE_GEN_NORMALS;
	if (ModelGenSmoothNormals)	Settings |= WAVE_GEN_SMOOTH_NORMALS;
	if (ModelFlipUVs)			Settings |= WAVE_FLIP_UVS;


	WaveModelData Model = WaveLoadModel(FileName, Settings);
	if (Model.MeshCount == 0)
		return ERROR32;

	uint32_t MeshIndex = ERROR32;
	if (LoadModelWave(FileName, &Model, &MeshInfo))
		MeshIndex = AddMesh(GetFileNameFromPath((char*)FileName), &MeshInfo);

	WaveFreeModel(&Model);

	SelectedTexture = LastTexture;
	return MeshIndex;
}

uint32_t AddPlane()
{
	SceneMesh MeshInfo;

	MeshInfo.Destroyable = false;
	strcpy(MeshInfo.Path, "Plane");
	MeshInfo.MeshData = (SceneMeshData*)malloc(1 * sizeof(SceneMeshData));
	MeshInfo.MeshCount = 1;

	SetDefaultMaterial(&MeshInfo.MeshData[0].Material, "MESH");
	MeshInfo.MeshData[0].VertexCount = ARRAY_SIZE(PlaneVertices);
	MeshInfo.MeshData[0].VertexOffset = 0;
	MeshInfo.VertexBuffer = PlaneVertexBuffer;

	MeshInfo.MeshData[0].IndexCount = ARRAY_SIZE(PlaneIndices);
	MeshInfo.MeshData[0].IndexOffset = 0;
	MeshInfo.IndexBuffer = PlaneIndexBuffer;
	memset(&MeshInfo.MeshData[0].Render, 1, ARRAY_SIZE(MeshInfo.MeshData[0].Render) * sizeof(bool));
	MeshInfo.MeshData[0].AABB = PlaneAABB;

	return AddMesh("Plane", &MeshInfo);
}

uint32_t AddCube()
{
	SceneMesh MeshInfo;

	MeshInfo.Destroyable = false;
	strcpy(MeshInfo.Path, "Cube");
	MeshInfo.MeshData = (SceneMeshData*)malloc(1 * sizeof(SceneMeshData));
	MeshInfo.MeshCount = 1;

	SetDefaultMaterial(&MeshInfo.MeshData[0].Material, "MESH");
	MeshInfo.MeshData[0].VertexCount = ARRAY_SIZE(CubeVertices);
	MeshInfo.MeshData[0].VertexOffset = 0;
	MeshInfo.VertexBuffer = CubeVertexBuffer;

	MeshInfo.MeshData[0].IndexCount = 0;
	MeshInfo.MeshData[0].IndexOffset = 0;
	MeshInfo.IndexBuffer = OPENVK_ERROR;
	memset(&MeshInfo.MeshData[0].Render, 1, ARRAY_SIZE(MeshInfo.MeshData[0].Render) * sizeof(bool));
	MeshInfo.MeshData[0].AABB = CubeAABB;

	return AddMesh("Cube", &MeshInfo);
}

uint32_t AddSphere()
{
	SceneMesh MeshInfo;

	MeshInfo.Destroyable = false;
	strcpy(MeshInfo.Path, "Sphere");
	MeshInfo.MeshData = (SceneMeshData*)malloc(1 * sizeof(SceneMeshData));
	MeshInfo.MeshCount = 1;

	SetDefaultMaterial(&MeshInfo.MeshData[0].Material, "MESH");
	MeshInfo.MeshData[0].VertexCount = SphereVertexCount;
	MeshInfo.MeshData[0].VertexOffset = 0;
	MeshInfo.VertexBuffer = SphereVertexBuffer;

	MeshInfo.MeshData[0].IndexCount = SphereIndexCount;
	MeshInfo.MeshData[0].IndexOffset = 0;
	MeshInfo.IndexBuffer = SphereIndexBuffer;
	memset(&MeshInfo.MeshData[0].Render, 1, ARRAY_SIZE(MeshInfo.MeshData[0].Render) * sizeof(bool));
	MeshInfo.MeshData[0].AABB = SphereAABB;

	return AddMesh("Sphere", &MeshInfo);
}

uint32_t AddBean()
{
	SceneMesh MeshInfo;

	MeshInfo.Destroyable = false;
	strcpy(MeshInfo.Path, "Bean");
	MeshInfo.MeshData = (SceneMeshData*)malloc(1 * sizeof(SceneMeshData));
	MeshInfo.MeshCount = 1;

	SetDefaultMaterial(&MeshInfo.MeshData[0].Material, "MESH");
	MeshInfo.MeshData[0].VertexCount = BeanVertexCount;
	MeshInfo.MeshData[0].VertexOffset = 0;
	MeshInfo.VertexBuffer = BeanVertexBuffer;

	MeshInfo.MeshData[0].IndexCount = BeanIndexCount;
	MeshInfo.MeshData[0].IndexOffset = 0;
	MeshInfo.IndexBuffer = BeanIndexBuffer;
	
	memset(&MeshInfo.MeshData[0].Render, 1, ARRAY_SIZE(MeshInfo.MeshData[0].Render) * sizeof(bool));
	MeshInfo.MeshData[0].AABB = BeanAABB;

	return AddMesh("Bean", &MeshInfo);
}