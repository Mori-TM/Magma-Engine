#define ENTITY_ALLOCATION_COUNT 32

typedef enum
{
	COMPONENT_TYPE_EMPTY = 0,
	COMPONENT_TYPE_MESH,
	COMPONENT_TYPE_MATERIAL,
	COMPONENT_TYPE_CAMERA,
	COMPONENT_TYPE_AUDIO,
	COMPONENT_TYPE_ANIMATION,
	COMPONENT_TYPE_LIGHT,
	COMPONENT_TYPE_COLLIDER,
	COMPONENT_TYPE_RIGIDBODY,
	COMPONENT_COUNT
} ComponentTypes;

const char* ComponentNames[] = 
{ 
	"Empty",
	"Mesh", 
	"Material", 
	"Camera", 
	"Audio", 
	"Animation", 
	"Light",
	"Collider",
	"Rigidbody"
};

typedef enum
{
	LIGHT_TYPE_POINT = 0,
	LIGHT_TYPE_DIRECTIONAL,
	LIGHT_TYPE_SPOT,
	LIGHT_COUNT
} LightTypes;

const char* LightNames[] =
{
	"Point",
	"Directional",
	"Spot"
};

typedef enum
{
	COLLIDER_TYPE_BOX = 0,
	COLLIDER_TYPE_SPHERE,
	COLLIDER_TYPE_CYLINDER,
	COLLIDER_TYPE_Bean,
	COLLIDER_TYPE_CONE,
	COLLIDER_TYPE_CONVEX,
	COLLIDER_TYPE_MESH,
	COLLIDER_COUNT
} ColliderTypes;

const char* ColliderNames[] =
{
	"Box",
	"Sphere",
	"Cylinder",
	"Bean",
	"Cone",
	"Convex",
	"Mesh",
};

typedef struct
{
	uint32_t MeshIndex;
} MeshComponent;

typedef struct
{
	uint32_t MaterialIndex;
} MaterialComponent;

typedef struct
{
	float FOV;
	float NearPlane;
	float FarPlane;
} CameraComponent;

typedef struct
{
	uint32_t AudioHandle;
} AudioComponent;

typedef struct
{
	uint32_t AnimationIndex;
} AnimationComponent;

typedef struct
{
	LightTypes Type;
	vec3 Color;
	float Strength;
	bool CastShadow;
} LightComponent;

typedef struct
{
	ColliderTypes Type;
	float Friction;
	float Bounciness;
} ColliderComponent;

typedef struct
{
	float Mass;
} RigidbodyComponent;

typedef struct
{
	char Name[MAX_CHAR_NAME_LENGTH];
	vec3 Translate;
	vec3 Rotate;
	vec3 Scale;
	bool Selected;

	//FIX ? - why bool array? - use enum ?
	bool UsedComponents[COMPONENT_COUNT];
	MeshComponent Mesh;
	MaterialComponent Material;
	CameraComponent Camera;
	AudioComponent Audio;
	AnimationComponent Animation;
	LightComponent Light;
	ColliderComponent Collider;
	RigidbodyComponent Rigidbody;
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
		printf("Error, your system doesn't have enough ram for %d entities, buy more than 1 mb of ram\n", ENTITY_ALLOCATION_COUNT);
		exit(0);
	}
}

void EntitiesDestroy()
{
	EntityCount = 0;
	EntityAllocatedCount = 0;

	SelectedEntity = 0;
	SelectedTexture = 0;
	SelectedMaterial = 0;
	SelectedMesh = 0;
	SelectedAudio = 0;
	SelectedAnimation = 0;
	SelectedScript = 0;

	free(Entities);
}

//Checks if there is enough room for one more entity
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

//if OldDescriptorSet == OPENVK_ERROR it means unused
uint32_t UpdatePBRTextureDescriptorSet(uint32_t OldDescriptorSet, uint32_t AlbedoIndex, uint32_t NormalIndex, uint32_t MetallicIndex, uint32_t RoughnessIndex, uint32_t OcclusionIndex)
{
	{
		uint32_t DescriptorCounts[] = { 1, 1, 1, 1, 1 };
		uint32_t DescriptorTypes[] = { OPENVK_DESCRIPTOR_TYPE_IMAGE_SAMPLER, OPENVK_DESCRIPTOR_TYPE_IMAGE_SAMPLER, OPENVK_DESCRIPTOR_TYPE_IMAGE_SAMPLER, OPENVK_DESCRIPTOR_TYPE_IMAGE_SAMPLER, OPENVK_DESCRIPTOR_TYPE_IMAGE_SAMPLER };
		uint32_t Bindings[] = { 0, 1, 2, 3, 4 };

		uint32_t ImageTypes[5]; FillUintArray(ImageTypes, OPENVK_IMAGE_TYPE_TEXTURE, ARRAY_SIZE(ImageTypes));
		uint32_t ImageLayouts[5]; FillUintArray(ImageLayouts, OPENVK_IMAGE_LAYOUT_COLOR_OUTPUT, ARRAY_SIZE(ImageLayouts));
		
		uint32_t Images[5]; memset(Images, 0, ARRAY_SIZE(Images));
		uint32_t ImageSampler[5]; memset(ImageSampler, 0, ARRAY_SIZE(ImageSampler));

		SceneTextureImage* DefaultImage = (SceneTextureImage*)CMA_GetAt(&SceneTextures, 0);
		if (DefaultImage == NULL)
		{
			printf("No Default SceneTexture found, packing not possible\n");
			return OPENVK_ERROR;
		}

		SceneTextureImage* Albedo;
		SceneTextureImage* Normal;
		SceneTextureImage* Metallic;
		SceneTextureImage* Roughness;
		SceneTextureImage* Occlusion;
		Albedo = (SceneTextureImage*)CMA_GetAt(&SceneTextures, AlbedoIndex);
		Normal = (SceneTextureImage*)CMA_GetAt(&SceneTextures, NormalIndex);
		Metallic = (SceneTextureImage*)CMA_GetAt(&SceneTextures, MetallicIndex);
		Roughness = (SceneTextureImage*)CMA_GetAt(&SceneTextures, RoughnessIndex);
		Occlusion = (SceneTextureImage*)CMA_GetAt(&SceneTextures, OcclusionIndex);

		if (Albedo != NULL) 	{ Images[0] = Albedo->TextureImage; ImageSampler[0] = Albedo->TextureSampler; }
		if (Normal != NULL) 	{ Images[1] = Normal->TextureImage; ImageSampler[1] = Normal->TextureSampler; }
		if (Metallic != NULL) 	{ Images[2] = Metallic->TextureImage; ImageSampler[2] = Metallic->TextureSampler; }
		if (Roughness != NULL) 	{ Images[3] = Roughness->TextureImage; ImageSampler[3] = Roughness->TextureSampler; }
		if (Occlusion != NULL) 	{ Images[4] = Occlusion->TextureImage; ImageSampler[4] = Occlusion->TextureSampler; }

	//	uint32_t DefaultImage = 

		OpenVkDescriptorSetCreateInfo DescriptorSetCreateInfo;
		DescriptorSetCreateInfo.DescriptorSetLayout = PBRDescriptorSetLayout;
		DescriptorSetCreateInfo.DescriptorPool = DescriptorPool;
		DescriptorSetCreateInfo.DescriptorWriteCount = ARRAY_SIZE(DescriptorCounts);
		DescriptorSetCreateInfo.DescriptorCounts = DescriptorCounts;
		DescriptorSetCreateInfo.DescriptorTypes = DescriptorTypes;
		DescriptorSetCreateInfo.Sampler = ImageSampler;
		DescriptorSetCreateInfo.ImageTypes = ImageTypes;
		DescriptorSetCreateInfo.Images = Images;
		DescriptorSetCreateInfo.ImageLayouts = ImageLayouts;
		DescriptorSetCreateInfo.Bindings = Bindings;
		if (OldDescriptorSet == OPENVK_ERROR)
			DescriptorSetCreateInfo.DescriptorSet = NULL;
		else
			DescriptorSetCreateInfo.DescriptorSet = &OldDescriptorSet;

		DescriptorSetCreateInfo.VariableDescriptorSetCount = 0;

		OldDescriptorSet = OpenVkCreateDescriptorSet(&DescriptorSetCreateInfo);
		if (OldDescriptorSet == 73)
		{
			printf("damn: %zu\n", VkRenderer.DescriptorSets.Size);
		}
	}

	return OldDescriptorSet;
}

uint32_t UpdatePBRTextureDescriptorSetFromMaterial(uint32_t OldDescriptorSet, uint32_t MaterialIndex)
{
	SceneMaterial* Material = (SceneMaterial*)CMA_GetAt(&SceneMaterials, MaterialIndex);
	if (!Material)
		return 0;

	return UpdatePBRTextureDescriptorSet(OldDescriptorSet, Material->AlbedoIndex, Material->NormalIndex, Material->Metallic, Material->RoughnessIndex, Material->OcclusionIndex);
}

char MaterialName[MAX_CHAR_NAME_LENGTH] = "Material";
vec4 MaterialColor = Vec4f(1.0f);
float MaterialMetallic = 0.0;
float MaterialRoughness = 1.0;
float MaterialOcclusion = 1.0;

void SetDefaultMaterial(SceneMaterial* Material, const char* Name)
{
	sstrcpy(Material->Name, Name);
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
}

void ResetEntityMaterial(EntityInfo* Entity)
{
	Entity->Material.MaterialIndex = 0;
}

//FIX - Default values should be in a config file
void ResetEntityCamera(EntityInfo* Entity)
{
	Entity->Camera.FOV = 75.0;
	Entity->Camera.NearPlane = 0.01;
	Entity->Camera.FarPlane = 1000.0;
}

void ResetEntityAnimation(EntityInfo* Entity)
{
	Entity->Animation.AnimationIndex = 0;
}


void ResetEntityLight(EntityInfo* Entity)
{
	Entity->Light.Color = Vec3f(1.0);
	Entity->Light.Strength = 1.0;
	Entity->Light.Type = LIGHT_TYPE_POINT;
	Entity->Light.CastShadow = false;
}

void ResetEntityCollider(EntityInfo* Entity)
{
	Entity->Collider.Type = COLLIDER_TYPE_BOX;
	Entity->Collider.Friction = 0.0;
	Entity->Collider.Bounciness = 0.0;
}

void ResetEntityRigidbody(EntityInfo* Entity)
{
	Entity->Rigidbody.Mass = 0.0;
}

void ResetEntity(EntityInfo* Entity)
{
	memset(Entity, 0, sizeof(EntityInfo));

	ResetEntityMesh(Entity);
	ResetEntityMaterial(Entity);
	ResetEntityCamera(Entity);
	ResetEntityAnimation(Entity);
	ResetEntityLight(Entity);
	ResetEntityCollider(Entity);
	ResetEntityRigidbody(Entity);

	Entity->Translate = Vec3f(0.0);
	Entity->Rotate = Vec3f(0.0);
	Entity->Scale = Vec3f(1.0);
	Entity->Selected = false;

	uint32_t Count = 0;
	for (uint32_t i = 0; i < EntityCount; i++)
		if (strstr(Entities[i].Name, "Entity") != 0)
			Count++;

	if (Count > 0)
		ssprintf(Entity->Name, "Entity (%d)", Count);
	else
		sstrcpy(Entity->Name, "Entity");
}

uint32_t AddDefaultEntity(uint32_t UsedComponent)
{
	if (!EntitiesCheckForResize())
		return SelectedEntity;

	EntityInfo* Entity = &Entities[EntityCount];
	SelectedEntity = EntityCount++;

	ResetEntity(Entity);

	if (UsedComponent < COMPONENT_COUNT)
		Entity->UsedComponents[UsedComponent] = true;

	return SelectedEntity;
}

uint32_t AddEntity(EntityInfo* Entity)
{
	if (!EntitiesCheckForResize())
		return SelectedEntity;

	memcpy(&Entities[EntityCount], Entity, sizeof(EntityInfo));
	SelectedEntity = EntityCount++;
	return SelectedEntity;
}

void AddMeshToEntity(uint32_t EntityIndex, uint32_t MeshIndex)
{
	if (MeshIndex == OPENVK_ERROR)
	{
		printf("Failed to add Mesh to Entity, MeshIndex is invalid\n");
		return;	
	}

	SceneMesh* Mesh = (SceneMesh*)CMA_GetAt(&SceneMeshes, MeshIndex);
	if (Mesh)
	{
		Entities[EntityIndex].Mesh.MeshIndex = MeshIndex;
		RaytracingAddMesh(SelectedMesh);
		return;
	}
	
	printf("No Mesh to add to Entity\n");
}

uint32_t AddMesh(SceneMesh* MeshInfo)
{
	char Name[MAX_CHAR_NAME_LENGTH];
	sstrcpy(Name, MeshInfo->Name);

	CheckForSameNames(&SceneMeshes, ARRAY_SIZE(MeshInfo->Name), Name, MeshInfo->Name);

	SelectedMesh = CMA_Push(&SceneMeshes, MeshInfo);

	return SelectedMesh;
}

//if NULL set to default
uint32_t AddMaterial(SceneMaterial* Material)
{
	SceneMaterial DefMaterial;
	char Name[MAX_CHAR_NAME_LENGTH];
	if (Material == NULL)
	{
		SetDefaultMaterial(&DefMaterial, MaterialName);
		Material = &DefMaterial;
		sstrcpy(Name, MaterialName);
	}
	else
		sstrcpy(Name, Material->Name);

	UpdatePBRTextureDescriptorSet(OPENVK_ERROR, Material->AlbedoIndex, Material->NormalIndex, Material->MetallicIndex, Material->RoughnessIndex, Material->OcclusionIndex);

	CheckForSameNames(&SceneMaterials, ARRAY_SIZE(Material->Name), Name, Material->Name);

	DynamicArrayCreate(&Material->MeshDataReferences, sizeof(SceneMeshReference), "Material Mesh References");

	SelectedMaterial = CMA_Push(&SceneMaterials, Material);

	return SelectedMaterial;//Why previously SelectedMesh?
}

void SetMaterial(uint32_t MeshIndex, uint32_t MeshDataIndex, uint32_t MaterialIndex)
{
	SceneMaterial* Material = (SceneMaterial*)CMA_GetAt(&SceneMaterials, MaterialIndex);
	if (Material == NULL)
	{
		printf("Failed to find Material: %d, for setting: %d->%d\n", MaterialIndex, MeshIndex, MeshDataIndex);
		return;
	}

	//Remove old mesh reference from the material that was previously used by the mesh
	SceneMesh* Mesh = (SceneMesh*)CMA_GetAt(&SceneMeshes, MeshIndex);
	if (Mesh)
	{
		if (Mesh->MeshData[MeshDataIndex].MaterialIndex != 0)
		{
			SceneMaterial* MeshMaterial = (SceneMaterial*)CMA_GetAt(&SceneMaterials, Mesh->MeshData[MeshDataIndex].MaterialIndex);
			if (MeshMaterial != NULL)
			{
				for (size_t i = 0; i < MeshMaterial->MeshDataReferences.Size; i++)
				{
					SceneMeshReference* MeshReference = (SceneMeshReference*)DynamicArrayGetAt(&MeshMaterial->MeshDataReferences, i);
					if (MeshReference != NULL)
					{
						if (MeshReference->MeshIndex == MeshIndex &&
							MeshReference->MeshDataIndex == MeshDataIndex)							
						{
							DynamicArrayPop(&MeshMaterial->MeshDataReferences, i);
							break;
						}

					//	SceneMesh* MaterialMeshRefernce = (SceneMesh*)CMA_GetAt(&SceneMeshes, MeshReference->MeshIndex);
					//	if (MaterialMeshRefernce != NULL)
					//	{
					//
					//	}
					}
					

				}

			}
		}

		Mesh->MeshData[MeshDataIndex].MaterialIndex = MaterialIndex;
	}

	SceneMeshReference MeshReference;
	MeshReference.MeshIndex = MeshIndex;
	MeshReference.MeshDataIndex = MeshDataIndex;
	DynamicArrayPush(&Material->MeshDataReferences, &MeshReference);
}

//doesn't check if Material is valid
void DeleteMaterial(uint32_t Material)
{
	if (Material == 0)
		return;

	for (uint32_t i = 0; i < EntityCount; i++)
		if (Entities[i].UsedComponents[COMPONENT_TYPE_MATERIAL] && Entities[i].Material.MaterialIndex == Material)
			ResetEntityMaterial(&Entities[i]);
	
	//Reset the material for every mesh that used to use this material
	{
		SceneMaterial* MaterialPTR = (SceneMaterial*)CMA_GetAt(&SceneMaterials, Material);

		for (size_t i = 0; i < MaterialPTR->MeshDataReferences.Size; i++)
		{
			SceneMeshReference* MeshReference = (SceneMeshReference*)DynamicArrayGetAt(&MaterialPTR->MeshDataReferences, i);
			if (MeshReference != NULL)
			{
				SceneMesh* Mesh = (SceneMesh*)CMA_GetAt(&SceneMeshes, MeshReference->MeshIndex);
				if (Mesh != NULL)
				{
					Mesh->MeshData[MeshReference->MeshDataIndex].MaterialIndex = 0;
				}
			}
		}

		DynamicArrayDestroy(&MaterialPTR->MeshDataReferences);
	}	

	CMA_Pop(&SceneMaterials, Material);

	for (uint32_t i = 1; i < SceneMaterials.Size; i++)
		if (CMA_GetAt(&SceneMaterials, i) != NULL)
			SelectedMaterial = i;
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

	printf("Texture go Brr: %d, Sampler go: %d\n", Image->TextureImage, Image->TextureSampler);

	return OpenVkCreateDescriptorSet(&DescriptorSetCreateInfo);
}


uint32_t AddTexture(char* Path, bool ShowInAssetBrowser)
{
	SceneTextureImage Image;
	Image.ShowInAssetBrowser = ShowInAssetBrowser;
	Image.TextureDescriptorSet = LoadTexture(Path, &Image);
	sstrcpy(Image.Path, Path);
	ConvertFilePath(Image.Path);

	char* Name = GetFileNameFromPath(Path);
	CheckForSameNames(&SceneTextures, ARRAY_SIZE(Image.Name), Name, Image.Name);

	SelectedTexture = CMA_Push(&SceneTextures, &Image);

	printf("Texture Added, Name: %s, Index: %d\n", Path, SelectedTexture);

	RaytracingAddTexture(SelectedTexture);

	return SelectedTexture;
}

void RequestTextureDeletion(uint32_t Texture)
{
	SceneTextureImage* Image = (SceneTextureImage*)CMA_GetAt(&SceneTextures, Texture);
	if (Image == NULL)
	{
		printf("Failed to find texture that was requested to be deleted: %d\n", Texture);
		return;
	}

	for (uint32_t i = 0; i < SceneMaterials.Size; i++)
	{
		SceneMaterial* Material = (SceneMaterial*)CMA_GetAt(&SceneMaterials, i);
		if (Material != NULL)
		{
			if (Material->AlbedoIndex == Texture)
				Material->AlbedoIndex = 0;
			else if (Material->NormalIndex == Texture)
				Material->NormalIndex = 0;
			else if (Material->MetallicIndex == Texture)
				Material->MetallicIndex = 0;
			else if (Material->RoughnessIndex == Texture)
				Material->RoughnessIndex = 0;
			else if (Material->OcclusionIndex == Texture)
				Material->OcclusionIndex = 0;
		}
	}

	
	TextureToDelete = Image->TextureImage;
	SamplerToDelete = Image->TextureSampler;	

	CMA_Pop(&SceneTextures, Texture);

	for (uint32_t i = 1; i < SceneTextures.Size; i++)
		if (CMA_GetAt(&SceneTextures, i) != NULL)
			SelectedTexture = i;

	DeleteTexture = true;
}

//FIX memory
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
	sstrcpy(Animation.Path, Path);

	char* Name = GetFileNameFromPath(Path);
	CheckForSameNames(&SceneAnimations, ARRAY_SIZE(Animation.Name), Name, Animation.Name);

	SelectedAnimation = CMA_Push(&SceneAnimations, &Animation);
	
	return SelectedAnimation;
}

void AddScript()
{
	SceneScript Script;
	sstrcpy(Script.Path, "None");
	Script.LineCount = 1;
	Script.AllocatedLineCount = 1;
	Script.Lines = (SceneScriptLine*)malloc(1 * sizeof(SceneScriptLine));
	strcpy(Script.Lines[0].Line, "//Sample");

//	char* ScriptName = (char*)"Script";
//	char* Script = (char*)"function Start()\nend\n\nfunction Update()\n\tx, y, z, rx, ry, rz, sx, sy, sz = GetEntity(\"Entity\")\n\tprint(x, y, z, rx, ry, rz, sx, sy, sz)\nend";
//	sstrcpy(SceneScripts[SceneScriptCount].Name, ScriptName);
//	sstrcpy(SceneScripts[SceneScriptCount].Path, "None");
//	sstrcpy(SceneScripts[SceneScriptCount].Script, Script);

	char* Name = (char*)"Script";
	CheckForSameNames(&SceneScripts, ARRAY_SIZE(Script.Name), Name, Script.Name);

	SelectedScript = CMA_Push(&SceneScripts, &Script);
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
bool ModelRedundantMaterials = true;
bool ModelGenFlatNormals = false;
bool ModelGenSmoothNormals = false;
bool ModelFlipUVs = true;
bool ModelSetZeroAlphaOne = true;
//uint32_t ModelSettings = WAVE_LOAD_MATERIAL | WAVE_GEN_NORMALS | WAVE_FLIP_UVS | WAVE_GEN_UVS | WAVE_GEN_INDICES | WAVE_MATERIAL_USE_MODEL_PATH | WAVE_REMOVE_REDUNDANT_MATERIALS | WAVE_PRINT_DEBUG_INOFS;

bool LoadModelWave(const char* Path, WaveModelData* ModelData, SceneMesh* MeshInfo)
{
	MeshInfo->Destroyable = true;
	sstrcpy(MeshInfo->Path, Path);
	ConvertFilePath(MeshInfo->Path);
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

		SceneMaterial Material;

		SetDefaultMaterial(&Material, "MESH");
		if (strcmp(ModelData->Materials[i].MaterialName, WaveEmptyMaterial.MaterialName) != 0)//FIX - don't use strncpy
			strncpy(Material.Name, ModelData->Materials[i].MaterialName, sizeof(Material.Name));

		SceneMesh->VertexCount = WaveMesh->VertexCount;
		SceneMesh->IndexCount = WaveMesh->IndexCount;

		SceneMesh->VertexOffset = VertexCount;
		SceneMesh->IndexOffset = IndexCount;

		Material.Metallic = ModelData->Materials[i].Metallic;
		Material.Roughness = ModelData->Materials[i].Roughness;

		if (ModelLoadAlbedo && strcmp(ModelData->Materials[i].DiffuseTexture, "NoTexture") != 0)
			Material.AlbedoIndex = AddTexture(ModelData->Materials[i].DiffuseTexture, false);

		if (ModelLoadNormal)
		{
			if (strcmp(ModelData->Materials[i].NormalTexture, "NoTexture") != 0)
				Material.NormalIndex = AddTexture(ModelData->Materials[i].NormalTexture, false);
			else if (strcmp(ModelData->Materials[i].DisplacmentTexture, "NoTexture") != 0)
				Material.NormalIndex = AddTexture(ModelData->Materials[i].DisplacmentTexture, false);
			else if (strcmp(ModelData->Materials[i].BumpTexture, "NoTexture") != 0)
				Material.NormalIndex = AddTexture(ModelData->Materials[i].BumpTexture, false);
		}
		if (ModelLoadRoughness)
		{
			if (strcmp(ModelData->Materials[i].RoughnessTexture, "NoTexture") != 0)
				Material.RoughnessIndex = AddTexture(ModelData->Materials[i].RoughnessTexture, false);
			else if (strcmp(ModelData->Materials[i].SpecularTexture, "NoTexture") != 0)
				Material.RoughnessIndex = AddTexture(ModelData->Materials[i].SpecularTexture, false);
		}
		
		if (ModelLoadMetallic && strcmp(ModelData->Materials[i].MetallicTexture, "NoTexture") != 0)
			Material.MetallicIndex = AddTexture(ModelData->Materials[i].MetallicTexture, false);

		Material.Color.x = ModelData->Materials[i].DiffuseColor.x;
		Material.Color.y = ModelData->Materials[i].DiffuseColor.y;
		Material.Color.z = ModelData->Materials[i].DiffuseColor.z;
		Material.Color.w = ModelSetZeroAlphaOne ? (ModelData->Materials[i].Dissolve < 0.01 ? 1.0 : ModelData->Materials[i].Dissolve) : ModelData->Materials[i].Dissolve;

		
		sstrcpy(SceneMesh->Name, ModelData->Materials[i].MaterialName);
		sstrcpy(Material.Name, ModelData->Materials[i].MaterialName);
	//	OpenVkRuntimeError("%s", Material.Name);
		
	//	SceneMesh->DescriptorSet = UpdatePBRTextureDescriptorSet(OPENVK_ERROR, Material.AlbedoIndex, Material.NormalIndex, Material.MetallicIndex, Material.RoughnessIndex, 0);
		SceneMesh->MaterialIndex = AddMaterial(&Material);
	//	SceneMesh->DescriptorSet = UpdatePBRTextureDescriptorSet(OPENVK_ERROR, 0, 0, 0, 0, 0);

		uint32_t LastVertexCount = VertexCount;

		for (uint32_t j = 0; j < SceneMesh->VertexCount; j++)
		{
			Vertices[VertexCount].PosTexX.x = WaveMesh->Vertices[j].Vertices.x;
			Vertices[VertexCount].PosTexX.y = WaveMesh->Vertices[j].Vertices.y;
			Vertices[VertexCount].PosTexX.z = WaveMesh->Vertices[j].Vertices.z;

			Vertices[VertexCount].PosTexX.w = WaveMesh->Vertices[j].TexCoords.x;
			Vertices[VertexCount].NormalTexY.w = WaveMesh->Vertices[j].TexCoords.y;

			Vertices[VertexCount].NormalTexY.x = WaveMesh->Vertices[j].Normals.x;
			Vertices[VertexCount].NormalTexY.y = WaveMesh->Vertices[j].Normals.y;
			Vertices[VertexCount].NormalTexY.z = WaveMesh->Vertices[j].Normals.z;

			Vertices[VertexCount].Data.VertexOffset = LastVertexCount;
			Vertices[VertexCount].Data.TextureIndex = (float)Material.AlbedoIndex;
			Vertices[VertexCount].Data.Unused0 = 0.0;
			Vertices[VertexCount].Data.Unused1 = 0.0;
			
			VertexCount++;
		}

		for (uint32_t j = 0; j < SceneMesh->IndexCount; j++)
			Indices[IndexCount++] = WaveMesh->Indices[j] + LastVertexCount;

		GenerateAABB(&MeshInfo->MeshData[i].AABB, SceneMesh->VertexCount, Vertices + SceneMesh->VertexOffset);

		//	VertexCount += SceneMesh->VertexCount;
		//	IndexCount += SceneMesh->IndexCount;
	}

	MeshInfo->VertexBuffer = OpenVkCreateVertexBuffer(VertexCount * sizeof(SceneVertex), Vertices);
	if (MeshInfo->VertexBuffer == OPENVK_ERROR)
	{
		printf("Failed to allocate Vertex buffer for: %s, count: %d\n", Path, VertexCount);
		free(Vertices);
		free(Indices);
		return false;
	}		

	MeshInfo->IndexBuffer = OpenVkCreateIndexBuffer(IndexCount * sizeof(uint32_t), Indices);
	if (MeshInfo->IndexBuffer == OPENVK_ERROR)
	{
		printf("Failed to allocate Index buffer for: %s, count: %d\n", Path, IndexCount);
		free(Vertices);
		free(Indices);
		OpenVkDestroyBuffer(MeshInfo->VertexBuffer);
		return false;
	}

//	free(Vertices);
//	free(Indices);
	MeshInfo->TotalVertexCount = VertexCount;
	MeshInfo->TotalIndexCount = IndexCount;

	MeshInfo->Vertices = Vertices;
	MeshInfo->Indices = Indices;

	return true;
}

uint32_t AddModel(uint32_t Settings, const char* FileName)
{
	char LastPath[MAX_CHAR_PATH_LENGTH];
	char Path[MAX_CHAR_PATH_LENGTH];
	sstrcpy(Path, FileName);
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

	Settings = WAVE_GEN_UVS | WAVE_GEN_NORMALS | WAVE_GEN_INDICES | WAVE_MATERIAL_USE_MODEL_PATH | WAVE_PRINT_DEBUG_INOFS;

	if (ModelLoadMaterials)		Settings |= WAVE_LOAD_MATERIAL;
	if (ModelRedundantMaterials)Settings |= WAVE_REMOVE_REDUNDANT_MATERIALS;
	if (ModelGenFlatNormals)	Settings |= WAVE_FORCE_GEN_NORMALS;
	if (ModelGenSmoothNormals)	Settings |= WAVE_GEN_SMOOTH_NORMALS;
	if (ModelFlipUVs)			Settings |= WAVE_FLIP_UVS;


	WaveModelData Model = WaveLoadModel(FileName, Settings);
	if (Model.MeshCount == 0)
	{
		printf("Failed to load Model correctly\n");
		return ERROR32;
	}
		

	uint32_t MeshIndex = ERROR32;
	if (LoadModelWave(FileName, &Model, &MeshInfo))
	{
		sstrcpy(MeshInfo.Name, GetFileNameFromPath((char*)FileName));
		MeshIndex = AddMesh(&MeshInfo);

		{
			SceneMesh* Mesh = (SceneMesh*)CMA_GetAt(&SceneMeshes, MeshIndex);

			for (uint32_t i = 0; i < Mesh->MeshCount; i++)
			{
				SceneMaterial* Material = (SceneMaterial*)CMA_GetAt(&SceneMaterials, Mesh->MeshData[i].MaterialIndex);

				SceneMeshReference MeshReference;
				MeshReference.MeshIndex = MeshIndex;
				MeshReference.MeshDataIndex = i;
				DynamicArrayPush(&Material->MeshDataReferences, &MeshReference);
			}
		}	
	}
	else
	{
		printf("Failed to parse Model\n");
	}

	WaveFreeModel(&Model);

	if (LastTexture != 0)
		SelectedTexture = LastTexture;

	RaytracingAddGeometry(MeshIndex);
	return MeshIndex;
}

typedef enum : uint32_t
{
	DEFAULT_MODEL_PLANE = 0,
	DEFAULT_MODEL_CUBE,
	DEFAULT_MODEL_SPHERE,
	DEFAULT_MODEL_BEAN,
	DEFAULT_MODEL_COUNT
} DefaultModels;

uint32_t SetMeshInfoForDefaultModel(DefaultModels Model, SceneMesh* MeshInfo)
{
	uint32_t MeshIndex = OPENVK_ERROR;

	char ModelName[MAX_CHAR_NAME_LENGTH_SHORT];

	switch (Model)
	{
	case DEFAULT_MODEL_PLANE:
		sstrcpy(ModelName, "Plane");

		sstrcpy(MeshInfo->Name, ModelName);
		sstrcpy(MeshInfo->Path, ModelName);
		sstrcpy(MeshInfo->MeshData[0].Name, ModelName);
		MeshInfo->MeshData[0].VertexCount = ARRAY_SIZE(PlaneVertices);
		MeshInfo->MeshData[0].IndexCount = ARRAY_SIZE(PlaneIndices);

		MeshInfo->TotalVertexCount = ARRAY_SIZE(PlaneVertices);
		MeshInfo->TotalIndexCount = ARRAY_SIZE(PlaneIndices);
		MeshInfo->Vertices = NULL;
		MeshInfo->Indices = NULL;

		MeshInfo->VertexBuffer = PlaneVertexBuffer;
		MeshInfo->IndexBuffer = PlaneIndexBuffer;
		MeshInfo->MeshData[0].AABB = PlaneAABB;
		MeshIndex = AddMesh(MeshInfo);
		break;

	case DEFAULT_MODEL_CUBE:
		sstrcpy(ModelName, "Cube");

		sstrcpy(MeshInfo->Name, ModelName);
		sstrcpy(MeshInfo->Path, ModelName);
		sstrcpy(MeshInfo->MeshData[0].Name, ModelName);
		MeshInfo->MeshData[0].VertexCount = ARRAY_SIZE(CubeVertices);
		MeshInfo->MeshData[0].IndexCount = 0;

		MeshInfo->TotalVertexCount = ARRAY_SIZE(CubeVertices);
		MeshInfo->TotalIndexCount = 0;
		MeshInfo->Vertices = NULL;
		MeshInfo->Indices = NULL;

		MeshInfo->VertexBuffer = CubeVertexBuffer;
		MeshInfo->IndexBuffer = OPENVK_ERROR;
		MeshInfo->MeshData[0].AABB = CubeAABB;
		MeshIndex = AddMesh(MeshInfo);
		break;

	case DEFAULT_MODEL_SPHERE:
		sstrcpy(ModelName, "Sphere");

		sstrcpy(MeshInfo->Name, ModelName);
		sstrcpy(MeshInfo->Path, ModelName);
		sstrcpy(MeshInfo->MeshData[0].Name, ModelName);
		MeshInfo->MeshData[0].VertexCount = SphereVertexCount;
		MeshInfo->MeshData[0].IndexCount = SphereIndexCount;

		MeshInfo->TotalVertexCount = SphereVertexCount;
		MeshInfo->TotalIndexCount = SphereIndexCount;
		MeshInfo->Vertices = NULL;
		MeshInfo->Indices = NULL;

		MeshInfo->VertexBuffer = SphereVertexBuffer;
		MeshInfo->IndexBuffer = SphereIndexBuffer;
		MeshInfo->MeshData[0].AABB = SphereAABB;
		MeshIndex = AddMesh(MeshInfo);
		break;

	case DEFAULT_MODEL_BEAN:
		sstrcpy(ModelName, "Bean");

		sstrcpy(MeshInfo->Name, ModelName);
		sstrcpy(MeshInfo->Path, ModelName);
		sstrcpy(MeshInfo->MeshData[0].Name, ModelName);
		MeshInfo->MeshData[0].VertexCount = BeanVertexCount;
		MeshInfo->MeshData[0].IndexCount = BeanIndexCount;

		MeshInfo->TotalVertexCount = BeanVertexCount;
		MeshInfo->TotalIndexCount = BeanIndexCount;
		MeshInfo->Vertices = NULL;
		MeshInfo->Indices = NULL;

		MeshInfo->VertexBuffer = BeanVertexBuffer;
		MeshInfo->IndexBuffer = BeanIndexBuffer;
		MeshInfo->MeshData[0].AABB = BeanAABB;
		MeshIndex = AddMesh(MeshInfo);
		break;

	default:
		printf("Invalid default model\n");
	}

	return MeshIndex;
}

uint32_t AddDefaultModel(DefaultModels Model)
{
	SceneMesh MeshInfo;
	memset(&MeshInfo, 0, sizeof(SceneMesh));

	MeshInfo.Destroyable = false;
	MeshInfo.MeshData = (SceneMeshData*)malloc(1 * sizeof(SceneMeshData));
	MeshInfo.MeshCount = 1;
	MeshInfo.MeshData[0].VertexOffset = 0;
	MeshInfo.MeshData[0].IndexOffset = 0;
//	MeshInfo.MeshData[0].DescriptorSet = UpdatePBRTextureDescriptorSet(OPENVK_ERROR, 0, 0, 0, 0, 0);
	memset(&MeshInfo.MeshData[0].Render, 1, ARRAY_SIZE(MeshInfo.MeshData[0].Render) * sizeof(bool));

	uint32_t MeshIndex = SetMeshInfoForDefaultModel(Model, &MeshInfo);

	SetMaterial(MeshIndex, 0, 0);

	RaytracingAddGeometry(MeshIndex);
	return MeshIndex;
}

void RequestMeshDeletion(uint32_t Mesh, uint32_t Options)
{
	for (uint32_t i = 0; i < EntityCount; i++)
		if (Entities[i].UsedComponents[COMPONENT_TYPE_MESH] && Entities[i].Mesh.MeshIndex == Mesh)
			ResetEntityMesh(&Entities[i]);

	ModelToDelete = Mesh;
	ModelToDeleteOptions = Options;
}

/*

uint32_t AddPlane()
{
	

	SceneMesh MeshInfo;

	MeshInfo.Destroyable = false;
	sstrcpy(MeshInfo.Path, "Plane");
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

	uint32_t MeshIndex = AddMesh("Plane", &MeshInfo);
	RaytracingAddGeometry(MeshIndex);
	return MeshIndex;
}

uint32_t AddCube()
{
	SceneMesh MeshInfo;

	MeshInfo.Destroyable = false;
	sstrcpy(MeshInfo.Path, "Cube");
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

	uint32_t MeshIndex = AddMesh("Cube", &MeshInfo);
	RaytracingAddGeometry(MeshIndex);
	return MeshIndex;
}

uint32_t AddSphere()
{
	SceneMesh MeshInfo;

	MeshInfo.Destroyable = false;
	sstrcpy(MeshInfo.Path, "Sphere");
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

	uint32_t MeshIndex = AddMesh("Sphere", &MeshInfo);
	RaytracingAddGeometry(MeshIndex);
	return MeshIndex;
}

uint32_t AddBean()
{
	SceneMesh MeshInfo;

	MeshInfo.Destroyable = false;
	sstrcpy(MeshInfo.Path, "Bean");
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

	uint32_t MeshIndex = AddMesh("Bean", &MeshInfo);
	RaytracingAddGeometry(MeshIndex);
	return MeshIndex;
}
*/