#define RAYTRACING_MAX_TEXTURE_COUNT 512
#define RAYTRACING_MAX_PRIMITVE_COUNT 32
#define RAYTRACING_MAX_MODEL_COUNT 256

typedef struct
{
	mat4 viewInverse;
	mat4 projInverse;
	vec4 LightDir;
	uint32_t Time;
} RaytracingUniformBufferObject;

typedef struct
{
	uint32_t TransformBuffer;
	uint32_t Geometry;
} RaytracingGeometry;

typedef struct
{
	uint32_t MeshIndex;
	uint32_t Instance;
	mat4 Transform;
} RaytracingMesh;

typedef struct
{
	uint64_t      txtOffset;             // Texture index offset in the array of textures
	uint64_t vertexAddress;         // Address of the Vertex buffer
	uint64_t indexAddress;          // Address of the index buffer
	uint64_t materialAddress;       // Address of the material buffer
	uint64_t materialIndexAddress;  // Address of the triangle material index buffer
	uint64_t Allignment;


	uint64_t bs1;
	uint64_t bs2;
} RaytracingBufferDescription;

typedef struct
{
	bool WasInit;
	uint32_t DescriptorSetLayouts[2];
	uint32_t DescriptorPoolBufferCount;
	uint32_t DescriptorPool;
	uint32_t DescriptorSets[2];
	uint32_t UniformBuffer;
	uint32_t PipelineLayout;

	uint32_t DescriptionBuffer;

//	DynamicArray TransformBuffers;
//	DynamicArray Geometry;
	DynamicArray Instances;
//	DynamicArray BottomLevelAS;
	CMA_MemoryZone Geometry;
	CMA_MemoryZone BottomLevelAS;
//	CMA_MemoryZone BottomLevelAS;
	DynamicArray DescriptionBuffers;
	DynamicArray Images;
	DynamicArray ImageLayouts;
	DynamicArray ImageTypes;
	DynamicArray ImageSampler;
	
	DynamicArray Meshes;

	uint64_t LastBuildHash;
	uint64_t CurrentBuildHash;

	uint64_t LastTransformHash;
	uint64_t CurrentTransformHash;

	uint32_t TopLevelAS;
	
	uint32_t StorageImageWidth;
	uint32_t StorageImageHeight;

	uint32_t StorageImage;
	uint32_t RaytracingPipeline;
	uint32_t* ShaderBindingTable;

	bool Render;
} RaytracingRenderer;

RaytracingRenderer RTR;
uint32_t RaytracingOutDescriptorSet;

constexpr uint64_t HashUint64(uint64_t key) 
{
	const uint64_t FNV_offset_basis = 14695981039346656037ULL;
	const uint64_t FNV_prime = 1099511628211ULL;

	uint64_t hash = FNV_offset_basis;
	for (size_t i = 0; i < sizeof(key); i++)
	{
		hash ^= (key & 0xFF); // XOR the bottom byte into the hash
		hash *= FNV_prime;    // Multiply by the prime number
		key >>= 8;            // Shift the input down by one byte
	}
	return hash;
}


void RtCountBuffer(uint32_t* VertexBufferCount, uint32_t* IndexBufferCount)
{
	uint32_t VertexCount = 0;
	uint32_t IndexCount = 0;

	for (uint32_t i = 0; i < SceneMeshes.Size; i++)
	{
		SceneMesh* Mesh = (SceneMesh*)CMA_GetAt(&SceneMeshes, i);
		if (Mesh && Mesh->MeshCount > 0)
		{
			if (Mesh->IndexBuffer != OPENVK_ERROR)
				IndexCount++;

			VertexCount++;
		}
	}

	*VertexBufferCount = VertexCount;
	*IndexBufferCount = IndexCount;
}

void RtCountBufferSize(SceneMesh* Mesh, uint32_t* VertexBufferSize, uint32_t* IndexBufferSize)
{
	uint32_t VertexCount = 0;
	uint32_t IndexCount = 0;

	for (uint32_t i = 0; i < Mesh->MeshCount; i++)
	{
		VertexCount += Mesh->MeshData[i].VertexCount;
		IndexCount += Mesh->MeshData[i].IndexCount;
	}

	*VertexBufferSize = VertexCount;
	*IndexBufferSize = IndexCount;
}

void RtCreateDescriptorSet(bool Update, uint32_t DescriptorWriteCount, uint32_t* DescriptorTypes, uint32_t* DescriptorCounts, uint32_t* Bindings)
{
	OpenVkDescriptorSetCreateInfo DescriptorSetCreateInfo;
	DescriptorSetCreateInfo.DescriptorSetLayout = RTR.DescriptorSetLayouts[0];
	DescriptorSetCreateInfo.DescriptorPool = RTR.DescriptorPool;
	DescriptorSetCreateInfo.DescriptorWriteCount = DescriptorWriteCount;
	DescriptorSetCreateInfo.DescriptorCounts = DescriptorCounts;
	DescriptorSetCreateInfo.DescriptorTypes = DescriptorTypes;
	
	size_t BufferSizes[] = { sizeof(RaytracingUniformBufferObject) };
	DescriptorSetCreateInfo.Buffers = &RTR.UniformBuffer;
	DescriptorSetCreateInfo.BufferSizes = BufferSizes;
	
	DescriptorSetCreateInfo.ImageLayouts = (uint32_t*)RTR.ImageLayouts.Data;
	DescriptorSetCreateInfo.Images = (uint32_t*)RTR.Images.Data;
	DescriptorSetCreateInfo.ImageTypes = (uint32_t*)RTR.ImageTypes.Data;
	DescriptorSetCreateInfo.Sampler = (uint32_t*)RTR.ImageSampler.Data;

	DescriptorSetCreateInfo.TopLevelAS = &RTR.TopLevelAS;
	
	DescriptorSetCreateInfo.Bindings = Bindings;
	DescriptorSetCreateInfo.VariableDescriptorSetCount = RAYTRACING_MAX_TEXTURE_COUNT;
	DescriptorSetCreateInfo.DescriptorSet = NULL;
	if (Update) DescriptorSetCreateInfo.DescriptorSet = &RTR.DescriptorSets[0];
	RTR.DescriptorSets[0] = OpenVkCreateDescriptorSet(&DescriptorSetCreateInfo);
}

void RtCreateDescriptorSetBufferDescriptions(uint32_t DescriptionBuffer)
{

	uint32_t DescriptorTypes[] =
	{
		OPENVK_DESCRIPTOR_TYPE_STORAGE_BUFFER
	};

	uint32_t DescriptorCounts[] = { 1 };
	uint32_t Bindings[] = { 0 };

	OpenVkDescriptorSetCreateInfo DescriptorSetCreateInfo;
	DescriptorSetCreateInfo.DescriptorSetLayout = RTR.DescriptorSetLayouts[1];
	DescriptorSetCreateInfo.DescriptorPool = RTR.DescriptorPool;
	DescriptorSetCreateInfo.DescriptorWriteCount = 1;
	DescriptorSetCreateInfo.DescriptorCounts = DescriptorCounts;
	DescriptorSetCreateInfo.DescriptorTypes = DescriptorTypes;

	size_t BufferSizes[] = { RTR.DescriptionBuffers.Size * sizeof(RaytracingBufferDescription) };//
	DescriptorSetCreateInfo.Buffers = &DescriptionBuffer;
	DescriptorSetCreateInfo.BufferSizes = BufferSizes;

	DescriptorSetCreateInfo.Bindings = Bindings;
	DescriptorSetCreateInfo.VariableDescriptorSetCount = 0;//
	DescriptorSetCreateInfo.DescriptorSet = NULL;
	if (RTR.DescriptorSets[1] != OPENVK_ERROR) DescriptorSetCreateInfo.DescriptorSet = &RTR.DescriptorSets[1];
	RTR.DescriptorSets[1] = OpenVkCreateDescriptorSet(&DescriptorSetCreateInfo);
}

bool RaytracingAddTexture(uint32_t SceneTexture)
{
	if (!OpenVkHasRaytracingSupport())
		return false;

	SceneTextureImage* Texture = (SceneTextureImage*)CMA_GetAt(&SceneTextures, SceneTexture);
	if (Texture)
	{
		uint32_t Data = OPENVK_IMAGE_LAYOUT_COLOR_OUTPUT;
		DynamicArrayPush(&RTR.ImageLayouts, &Data);

		Data = OPENVK_IMAGE_TYPE_TEXTURE;
		DynamicArrayPush(&RTR.ImageTypes, &Data);
		
		DynamicArrayPush(&RTR.Images, &Texture->TextureImage);
		DynamicArrayPush(&RTR.ImageSampler, &Texture->TextureSampler);
	}
	else
	{
		printf("[Raytracing]: Failed to find texture to load: %d\n", SceneTexture);
		return false;
	}

//	RtCreateDescriptorSet(true);
	uint32_t DescriptorTypes[] =
	{
		OPENVK_DESCRIPTOR_TYPE_STORAGE_IMAGE,
		OPENVK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
		OPENVK_DESCRIPTOR_TYPE_IMAGE_SAMPLER
	};

//	if (SceneTextures.Size != RTR.Images.Size - 1)
//		exit(3666);

	uint32_t DescriptorCounts[] = { 1, 1, (uint32_t)RTR.Images.Size - 1 };
	uint32_t Bindings[] = { 1, 2, 3 };

	RtCreateDescriptorSet(true, ARRAY_SIZE(DescriptorTypes), DescriptorTypes, DescriptorCounts, Bindings);

	return true;
}

void RaytracingAddGeometry(uint32_t SceneMeshIndex)
{
	if (!OpenVkHasRaytracingSupport())
		return;

	SceneMesh* Mesh = (SceneMesh*)CMA_GetAt(&SceneMeshes, SceneMeshIndex);
	if (Mesh == NULL || Mesh->MeshCount == 0)
	{
		printf("Damnn!\n");
		return;
	}

	{
		mat4 Model;
		LoadMat4IdentityP(&Model);
		printf("Hello: %zu\n", RTR.Instances.Size);
	//	if (RTR.Instances.Size == 0)
			Model = ScaleMat4(Model, Vec3(1.0, 1.0, 1.0));
	//	else
	//		Model = ScaleMat4(Model, Vec3((float)RandomInt(1, 1000) / 100.0, (float)RandomInt(1, 1000) / 100.0, (float)RandomInt(1, 1000) / 100.0));
		OpenVkTransformMatrix ModelOVK;
		memcpy(&ModelOVK, &Model, sizeof(OpenVkTransformMatrix));

		uint32_t TransformBuffer = VkCreateTranformBuffer(ModelOVK);
		

		uint32_t VertexSize = 0;
		uint32_t IndexSize = 0;
		RtCountBufferSize(Mesh, &VertexSize, &IndexSize);

		OpenVkRaytracingGeometryCreateInfo GeometryInfo;
		GeometryInfo.VertexFormat = OPENVK_FORMAT_RGB32F;
		GeometryInfo.VertexSize = sizeof(SceneVertex);
		GeometryInfo.VertexBufferDynamic = 0;
		GeometryInfo.VertexCount = VertexSize;
		GeometryInfo.VertexBuffer = Mesh->VertexBuffer;
		GeometryInfo.IndexBufferDynamic = 0;
		GeometryInfo.IndexCount = IndexSize;
		GeometryInfo.IndexBuffer = Mesh->IndexBuffer;
		GeometryInfo.TranformBuffer = TransformBuffer;

		uint32_t Geometry = OpenVkCreateRaytracingGeometry(&GeometryInfo);

		RaytracingGeometry RtGeometry;
		RtGeometry.Geometry = Geometry;
		RtGeometry.TransformBuffer = TransformBuffer;
		CMA_Push(&RTR.Geometry, &RtGeometry);

		uint32_t BottomLevelAS = OpenVkCreateBottomLevelAS(1, &Geometry, OpenVkFalse, NULL);		
		Mesh->BottomLevelAS = CMA_Push(&RTR.BottomLevelAS, &BottomLevelAS);
	}
}

uint32_t RaytracingAddMesh(uint32_t SceneMeshIndex)
{
	if (!OpenVkHasRaytracingSupport())
		return OPENVK_ERROR;

	SceneMesh* Mesh = (SceneMesh*)CMA_GetAt(&SceneMeshes, SceneMeshIndex);
	if (Mesh == NULL || Mesh->MeshCount == 0)
	{
		printf("Damnn!\n");
		return OPENVK_ERROR;
	}
		
	{
		mat4 Model;
		LoadMat4IdentityP(&Model);
		printf("Hello: %zu\n", RTR.Instances.Size);
		OpenVkTransformMatrix ModelOVK;
		memcpy(&ModelOVK, &Model, sizeof(OpenVkTransformMatrix));

		uint32_t* BottomLevelAS = (uint32_t*)CMA_GetAt(&RTR.BottomLevelAS, Mesh->BottomLevelAS);
		if (!BottomLevelAS)
		{
			printf("Failed to find BLAS\n");
			return OPENVK_ERROR;
		}

		uint32_t Instance = OpenVkCreateASInstance(ModelOVK, RTR.Instances.Size, OpenVkTrue, *BottomLevelAS);

		DynamicArrayPush(&RTR.Instances, &Instance);

		return RTR.Instances.Size - 1;
	}
}