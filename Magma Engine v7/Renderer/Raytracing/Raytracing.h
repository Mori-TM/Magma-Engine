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

void RaytracingInit()
{
	if (!OpenVkHasRaytracingSupport())
		return;

	memset(&RTR, 0, sizeof(RaytracingRenderer));

	RTR.DescriptorPool = OPENVK_ERROR;
	RTR.DescriptorSets[0] = OPENVK_ERROR;
	RTR.DescriptorSets[1] = OPENVK_ERROR;
	RTR.TopLevelAS = OPENVK_ERROR;
	RTR.DescriptionBuffer = OPENVK_ERROR;

	//FIX - Also Destroy these!!!
//	RTR.TransformBuffers	= DynamicArrayCreate(sizeof(uint32_t), "Transform Buffers");
	RTR.Geometry			= CMA_Create(sizeof(RaytracingGeometry), "Geometry");
	RTR.BottomLevelAS		= CMA_Create(sizeof(uint32_t), "Bottom Level AS");
	RTR.DescriptionBuffers = DynamicArrayCreate(sizeof(RaytracingBufferDescription), "Description Buffers");
	RTR.Instances			= DynamicArrayCreate(sizeof(uint32_t), "Instances");
	RTR.Images				= DynamicArrayCreate(sizeof(uint32_t), "Images");
	RTR.ImageLayouts		= DynamicArrayCreate(sizeof(uint32_t), "Image Layouts");
	RTR.ImageTypes			= DynamicArrayCreate(sizeof(uint32_t), "Image Types");
	RTR.ImageSampler		= DynamicArrayCreate(sizeof(uint32_t), "Image Sampler");

	RTR.Meshes = DynamicArrayCreate(sizeof(RaytracingMesh), "Meshes");

	RTR.LastBuildHash = -1;
	RTR.CurrentBuildHash = 0;

	RTR.LastTransformHash = -1;
	RTR.CurrentTransformHash = 0;

	
	RTR.UniformBuffer = OpenVkCreateUniformBuffer(sizeof(RaytracingUniformBufferObject));

	RTR.StorageImageWidth = WindowWidth;
	RTR.StorageImageHeight = WindowHeight;
	RTR.StorageImage = OpenVkCreateStorageImage(WindowWidth, WindowHeight, OPENVK_FORMAT_DEFAULT);

	uint32_t Data = OPENVK_IMAGE_LAYOUT_GENERAL_OUTPUT;
	DynamicArrayPush(&RTR.ImageLayouts, &Data);

	Data = OPENVK_IMAGE_TYPE_STORAGE;
	DynamicArrayPush(&RTR.ImageTypes, &Data);

	DynamicArrayPush(&RTR.Images, &RTR.StorageImage);
	DynamicArrayPush(&RTR.ImageSampler, &RTR.StorageImage);

//
//	struct timespec TimerStartTime;
//	struct timespec TimerEndTime;
//
//	timespec_get(&TimerStartTime, TIME_UTC);
//	RtUpdateDescriptors(false);
//	timespec_get(&TimerEndTime, TIME_UTC);
//
//	double Time = (double)((TimerEndTime.tv_sec) * 1000.0 + (TimerEndTime.tv_nsec) / 1000000.0) - ((TimerStartTime.tv_sec) * 1000.0 + (TimerStartTime.tv_nsec) / 1000000.0);
//	printf("Update Time: %f ms\n", Time);
	

	{
		uint32_t DescriptorCounts[] = { 1, 1, 1, RAYTRACING_MAX_TEXTURE_COUNT };
		uint32_t Bindings[] = { 0, 1, 2, 3 };
		uint32_t DescriptorTypes[] =
		{
			OPENVK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE,
			OPENVK_DESCRIPTOR_TYPE_STORAGE_IMAGE,
			OPENVK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
			OPENVK_DESCRIPTOR_TYPE_IMAGE_SAMPLER
		};
		uint32_t DescriptorFlags[] =
		{
			OPENVK_DESCRIPTOR_FLAG_NONE,
			OPENVK_DESCRIPTOR_FLAG_NONE,
			OPENVK_DESCRIPTOR_FLAG_NONE,
			OPENVK_DESCRIPTOR_FLAG_VARIABLE_DESCRIPTOR_COUNT | OPENVK_DESCRIPTOR_FLAG_PARTIALLY_BOUND | OPENVK_DESCRIPTOR_FLAG_UPDATE_AFTER_BIND
		};
		uint32_t ShaderTypes[] =
		{
			OPENVK_SHADER_TYPE_RAYGEN | OPENVK_SHADER_TYPE_CLOSEST_HIT,
			OPENVK_SHADER_TYPE_RAYGEN,
			OPENVK_SHADER_TYPE_RAYGEN | OPENVK_SHADER_TYPE_CLOSEST_HIT | OPENVK_SHADER_TYPE_MISS,
			OPENVK_SHADER_TYPE_CLOSEST_HIT
		};

		OpenVkDescriptorSetLayoutCreateInfo DescriptorSetLayoutCreateInfo;
		DescriptorSetLayoutCreateInfo.Flags = OPENVK_DESCRIPTOR_SET_LAYOUT_FLAG_UPDATE_AFTER_BIND_POOL;
		DescriptorSetLayoutCreateInfo.BindingCount = ARRAY_SIZE(DescriptorTypes);
		DescriptorSetLayoutCreateInfo.Bindings = Bindings;
		DescriptorSetLayoutCreateInfo.DescriptorCounts = DescriptorCounts;
		DescriptorSetLayoutCreateInfo.DescriptorTypes = DescriptorTypes;
		DescriptorSetLayoutCreateInfo.DescriptorFlags = DescriptorFlags;
		DescriptorSetLayoutCreateInfo.ShaderTypes = ShaderTypes;
		RTR.DescriptorSetLayouts[0] = OpenVkCreateDescriptorSetLayout(&DescriptorSetLayoutCreateInfo);
	}

	{
		uint32_t DescriptorCounts[] = { 1 };
		uint32_t Bindings[] = { 0 };
		uint32_t DescriptorTypes[] =
		{
			OPENVK_DESCRIPTOR_TYPE_STORAGE_BUFFER
		};
		uint32_t DescriptorFlags[] =
		{
			OPENVK_DESCRIPTOR_FLAG_NONE
		};
		uint32_t ShaderTypes[] =
		{
			OPENVK_SHADER_TYPE_CLOSEST_HIT
		};

		OpenVkDescriptorSetLayoutCreateInfo DescriptorSetLayoutCreateInfo;
		DescriptorSetLayoutCreateInfo.Flags = OPENVK_DESCRIPTOR_SET_LAYOUT_FLAG_UPDATE_AFTER_BIND_POOL;
		DescriptorSetLayoutCreateInfo.BindingCount = ARRAY_SIZE(DescriptorTypes);
		DescriptorSetLayoutCreateInfo.Bindings = Bindings;
		DescriptorSetLayoutCreateInfo.DescriptorCounts = DescriptorCounts;
		DescriptorSetLayoutCreateInfo.DescriptorTypes = DescriptorTypes;
		DescriptorSetLayoutCreateInfo.DescriptorFlags = DescriptorFlags;
		DescriptorSetLayoutCreateInfo.ShaderTypes = ShaderTypes;
		RTR.DescriptorSetLayouts[1] = OpenVkCreateDescriptorSetLayout(&DescriptorSetLayoutCreateInfo);

	}

	{
		OpenVkPipelineLayoutCreateInfo LayoutCreateInfo;
		LayoutCreateInfo.PushConstantCount = 0;
		LayoutCreateInfo.PushConstantShaderTypes = NULL;
		LayoutCreateInfo.PushConstantOffsets = NULL;
		LayoutCreateInfo.PushConstantSizes = NULL;
		LayoutCreateInfo.DescriptorSetLayoutCount = 2;
		LayoutCreateInfo.DescriptorSetLayouts = RTR.DescriptorSetLayouts;
		RTR.PipelineLayout = OpenVkCreatePipelineLayout(&LayoutCreateInfo);
	}

	{
		uint32_t ShaderTypes[] = 
		{ 
			OPENVK_SHADER_TYPE_RAYGEN, 
			OPENVK_SHADER_TYPE_MISS, 
			OPENVK_SHADER_TYPE_MISS, 
			OPENVK_SHADER_TYPE_CLOSEST_HIT 
		};
		OpenVkFile Shader[] = 
		{ 
			OpenVkReadFile("Data/Shader/Raytracing/raygen.spv"), 
			OpenVkReadFile("Data/Shader/Raytracing/miss.spv"), 
			OpenVkReadFile("Data/Shader/Raytracing/shadow.spv"), 
			OpenVkReadFile("Data/Shader/Raytracing/closesthit.spv") 
		};
		RTR.RaytracingPipeline = VkCreateRaytracingPipeline(5, RTR.PipelineLayout, 4, ShaderTypes, Shader);

		uint32_t HandleCounts[] = { 1, 2, 1 };
		RTR.ShaderBindingTable = VkCreateShaderBindingTable(RTR.RaytracingPipeline, 3, HandleCounts);
	}


	uint32_t DescriptorTypes[] =
	{
		OPENVK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE,
		OPENVK_DESCRIPTOR_TYPE_STORAGE_IMAGE,
		OPENVK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
		OPENVK_DESCRIPTOR_TYPE_IMAGE_SAMPLER,
		OPENVK_DESCRIPTOR_TYPE_STORAGE_BUFFER
	};
	uint32_t DescriptorCounts[] = { 1, 1, 1, RAYTRACING_MAX_TEXTURE_COUNT, RAYTRACING_MAX_MODEL_COUNT }; //plus one storage image?
	RTR.DescriptorPool = OpenVkCreateDescriptorPool(OPENVK_DESCRIPTOR_POOL_UPDATABLE, ARRAY_SIZE(DescriptorTypes), DescriptorTypes, DescriptorCounts);

	{
		uint32_t DescriptorTypes[] =
		{
			OPENVK_DESCRIPTOR_TYPE_STORAGE_IMAGE,
			OPENVK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
		};

		uint32_t DescriptorCounts[] = { 1, 1 };
		uint32_t Bindings[] = { 1, 2 };
		RtCreateDescriptorSet(false, ARRAY_SIZE(DescriptorTypes), DescriptorTypes, DescriptorCounts, Bindings);
	}
	

//	RtCreateDescriptorSet(false);
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

	uint32_t DescriptorCounts[] = { 1, 1, RTR.Images.Size - 1 };
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
		GeometryInfo.VertexFormat = OPENVK_FORMAT_RGBA32F;
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

		uint32_t Instance = OpenVkCreateInstance(ModelOVK, OpenVkTrue, *BottomLevelAS);

		DynamicArrayPush(&RTR.Instances, &Instance);

		return RTR.Instances.Size - 1;
	}
}


/*
typedef struct
{
	uint32_t BottomLevelAS;

	uint32_t VertexCount;
} RtBottomLevelAS;

typedef struct
{
	DynamicArray BottomLevelAS;


} RtFrameBuilder;
*/

void RaytracingRestBuild()
{
	if (!OpenVkHasRaytracingSupport())
		return;

	RTR.CurrentBuildHash = 0;
	DynamicArrayClear(&RTR.Meshes);
	DynamicArrayClear(&RTR.DescriptionBuffers);
}

void RaytracingAddEntityMesh(uint32_t MeshIndex, mat4* Transform, SceneMesh* Mesh)
{
	if (!OpenVkHasRaytracingSupport())
		return;

	RTR.CurrentBuildHash += Mesh->IndexBuffer == OPENVK_ERROR ? 0 : Mesh->IndexBuffer;
	RTR.CurrentBuildHash += Mesh->VertexBuffer;
	RTR.CurrentBuildHash += Mesh->MeshCount;
	RTR.CurrentBuildHash += Mesh->Destroyable;

	//Create a sum for every matrix so it can be hashed and check if any model was moved since the last frame
	{
		float Sum = 0.0;
		for (uint32_t i = 0; i < 16; i++)
			Sum += fabsf(Transform->m1D[i]);
		unsigned int ui;
		memcpy(&ui, &Sum, sizeof(float));
		RTR.CurrentTransformHash += ui & 0xfffff000;
	}
	

//	CurrentBuildHash += hash(Mesh->Name);

	RaytracingMesh RTMesh;
	RTMesh.MeshIndex = MeshIndex;
	memcpy(&RTMesh.Transform, Transform, sizeof(mat4));
//	RTMesh.Instance = OPENVK_ERROR;

	if (RTR.Instances.Size > 0)
	{
		uint32_t* Instance = (uint32_t*)DynamicArrayGetAt(&RTR.Instances, RTR.Meshes.Size);
		RTMesh.Instance = *Instance;
	}
	else
	{
		RTMesh.Instance = OPENVK_ERROR;
	}

	DynamicArrayPush(&RTR.Meshes, &RTMesh);

	RaytracingBufferDescription BufferDescription;
	BufferDescription.vertexAddress = VkGetBufferAddress(Mesh->VertexBuffer);
	BufferDescription.indexAddress = (Mesh->IndexBuffer == OPENVK_ERROR ? 0 : VkGetBufferAddress(Mesh->IndexBuffer));
	DynamicArrayPush(&RTR.DescriptionBuffers, &BufferDescription);

	

//	printf("Push it\n");
}

void RaytracingBuild()
{
	if (!OpenVkHasRaytracingSupport())
		return;

//	uint64_t CurrentBuildHash = -1;//HashUint64


	RTR.CurrentBuildHash = HashUint64(RTR.CurrentBuildHash);


	if (RTR.CurrentBuildHash == RTR.LastBuildHash)
		return;

	OpenVkDeviceWaitIdle();

	for (uint32_t i = 0; i < RTR.Instances.Size; i++)
	{
		uint32_t* Instance = (uint32_t*)DynamicArrayGetAt(&RTR.Instances, i);
		OpenVkDestroyInstance(*Instance);
	}

	DynamicArrayClear(&RTR.Instances);

	for (uint32_t i = 0; i < RTR.Meshes.Size; i++)
	{
		RaytracingMesh* Mesh = (RaytracingMesh*)DynamicArrayGetAt(&RTR.Meshes, i);
		Mesh->Instance = RaytracingAddMesh(Mesh->MeshIndex);
	}

	
	
	
	RTR.LastBuildHash = RTR.CurrentBuildHash;

	if (RTR.Meshes.Size == 0)
	{
		RTR.Render = false;
		return;
	}

	{
		if (RTR.DescriptionBuffer != OPENVK_ERROR) OpenVkDestroyDynamicBuffer(RTR.DescriptionBuffer);
		RTR.DescriptionBuffer = OpenVkCreateStorageBuffer(RTR.DescriptionBuffers.Size * sizeof(RaytracingBufferDescription));
		
		RtCreateDescriptorSetBufferDescriptions(RTR.DescriptionBuffer);

	}

//	RtUpdateTLAS = false;

	if (RTR.TopLevelAS == OPENVK_ERROR)
		RTR.TopLevelAS = OpenVkCreateTopLevelAS(RTR.Instances.Size, (uint32_t*)RTR.Instances.Data, RAYTRACING_MAX_PRIMITVE_COUNT, OpenVkTrue, NULL);
	else
	{
		VkDestroAccelerationStructure(RTR.TopLevelAS);
		CMA_Pop(&VkRaytracer.AccelerationStructures, RTR.TopLevelAS);
		RTR.TopLevelAS = OpenVkCreateTopLevelAS(RTR.Instances.Size, (uint32_t*)RTR.Instances.Data, RTR.Instances.Size, OpenVkTrue, NULL);
		//	OpenVkCreateTopLevelAS(RTR.Instances.Size, (uint32_t*)RTR.Instances.Data, RAYTRACING_MAX_PRIMITVE_COUNT, OpenVkTrue, &RTR.TopLevelAS);
	}

	printf("Hey there, created tlas\n");
	RTR.Render = true;

	uint32_t DescriptorTypes[] =
	{
		OPENVK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE,
		OPENVK_DESCRIPTOR_TYPE_STORAGE_IMAGE,
		OPENVK_DESCRIPTOR_TYPE_UNIFORM_BUFFER
	};

	uint32_t DescriptorCounts[] = { 1, 1, 1 };
	size_t BufferSizes[] = { sizeof(RaytracingUniformBufferObject) };
	uint32_t Bindings[] = { 0, 1, 2 };

	RtCreateDescriptorSet(true, ARRAY_SIZE(DescriptorTypes), DescriptorTypes, DescriptorCounts, Bindings);
}







void RaytracingResize()
{
	if (!OpenVkHasRaytracingSupport())
		return;

	if (RTR.StorageImageWidth >= WindowWidth &&
		RTR.StorageImageHeight >= WindowHeight)
		return;

	RTR.StorageImageWidth = WindowWidth;
	RTR.StorageImageHeight = WindowHeight;

	OpenVkRuntimeWarning("Raytrace resize!");

	OpenVkDestroyImage(RTR.StorageImage);
	RTR.StorageImage = OpenVkCreateStorageImage(WindowWidth, WindowHeight, OPENVK_FORMAT_DEFAULT);
	DynamicArrayReplace(&RTR.Images, &RTR.StorageImage, 0);

	uint32_t DescriptorTypes[] =
	{
		OPENVK_DESCRIPTOR_TYPE_STORAGE_IMAGE,
		OPENVK_DESCRIPTOR_TYPE_UNIFORM_BUFFER
	};

	uint32_t DescriptorCounts[] = { 1, 1 };
	uint32_t Bindings[] = { 1, 2 };

	RtCreateDescriptorSet(true, ARRAY_SIZE(DescriptorTypes), DescriptorTypes, DescriptorCounts, Bindings);
//	RaytracingUpdateAssets();
}

void RtUpdateInstances()
{
	for (uint32_t i = 0; i < RTR.Meshes.Size; i++)
	{
		RaytracingMesh* Mesh = (RaytracingMesh*)DynamicArrayGetAt(&RTR.Meshes, i);
		if (Mesh)
		{
			uint32_t* Instance = (uint32_t*)DynamicArrayGetAt(&RTR.Instances, Mesh->Instance);
			if (Instance)
			{
				//	OpenVkTransformMatrix Matrix =
				//	{
				//		1.0, 0.0, 0.0, 20.0 * sin(SDL_GetTicks() * ((float)RandomInt(1, 10) * 0.00001)),
				//		0.0, 1.0, 0.0, 0.0,
				//		0.0, 0.0, 1.0, 0.0
				//	};

				//	OpenVkTransformMatrix Transform =
				//	{
				//		Mesh->Transform.m[0][0], Mesh->Transform.m[1][0], Mesh->Transform.m[2][0], Mesh->Transform.m[3][0],
				//		Mesh->Transform.m[0][1], Mesh->Transform.m[1][1], Mesh->Transform.m[2][1], Mesh->Transform.m[3][1],
				//		Mesh->Transform.m[0][2], Mesh->Transform.m[1][2], Mesh->Transform.m[2][2], Mesh->Transform.m[3][2],
				//	//	Mesh->Transform.m[0][3], Mesh->Transform.m[1][3], Mesh->Transform.m[2][3], //Mesh->Transform.m[3][3]
				//	};

				OpenVkTransformMatrix Transform =
				{
					Mesh->Transform.m[0][0], Mesh->Transform.m[0][1], Mesh->Transform.m[0][2], Mesh->Transform.m[0][3],
					Mesh->Transform.m[1][0], Mesh->Transform.m[1][1], Mesh->Transform.m[1][2], Mesh->Transform.m[1][3],
					Mesh->Transform.m[2][0], Mesh->Transform.m[2][1], Mesh->Transform.m[2][2], Mesh->Transform.m[2][3],
					//	Mesh->Transform.m[3][0], Mesh->Transform.m[3][1], Mesh->Transform.m[3][2], Mesh->Transform.m[3][3]
				};

				VkUpdateInstanceTransform(Transform, *Instance);
			}
		}
	}
	OpenVkCreateTopLevelAS(RTR.Instances.Size, (uint32_t*)RTR.Instances.Data, RAYTRACING_MAX_PRIMITVE_COUNT, OpenVkTrue, &RTR.TopLevelAS);
}

void RaytracingUpdate()
{
	if (!OpenVkHasRaytracingSupport())
		return;

	RaytracingUniformBufferObject UBO;
	UBO.viewInverse = GBufferVertexUBO.View;
	UBO.projInverse = GBufferVertexUBO.Projection;
	UBO.viewInverse = InverseMat4(UBO.viewInverse);
	UBO.projInverse = InverseMat4(UBO.projInverse);
//	UBO.LightDir = Vec4(5.0 * sin(SDL_GetTicks() * 0.0001), 10.976, 6.0 * cos(SDL_GetTicks() * 0.0001), 0.0);
//	UBO.LightDir = Vec4(-3.6, 6.5, 2.75, 0.0);
	UBO.LightDir = ShadowDirection;
	UBO.LightDir.w = 0.0;
	Normalize4P(&UBO.LightDir);
	UBO.Time = SDL_GetTicks();
	OpenVkUpdateBuffer(sizeof(RaytracingUniformBufferObject), &UBO, RTR.UniformBuffer);

	RTR.CurrentTransformHash = HashUint64(RTR.CurrentTransformHash);

	
	if (RTR.Render)
	{
		if (RTR.LastTransformHash != RTR.CurrentTransformHash)
		{
			RtUpdateInstances();
			RTR.LastTransformHash = RTR.CurrentTransformHash;
		}

		OpenVkUpdateBuffer(RTR.DescriptionBuffers.Size * sizeof(RaytracingBufferDescription), RTR.DescriptionBuffers.Data, RTR.DescriptionBuffer);
	}
	

	
	RTR.CurrentTransformHash = 0;
}

void RaytracingDraw()
{
	if (!OpenVkHasRaytracingSupport())
		return;

	if (RTR.Render && RTR.DescriptionBuffers.Size > 0)
	{
		OpenVkBindPipeline(RTR.RaytracingPipeline, OPENVK_PIPELINE_TYPE_RAYTRACING);
		OpenVkBindDescriptorSet(RTR.PipelineLayout, 0, RTR.DescriptorSets[0], OPENVK_PIPELINE_TYPE_RAYTRACING);
		OpenVkBindDescriptorSet(RTR.PipelineLayout, 1, RTR.DescriptorSets[1], OPENVK_PIPELINE_TYPE_RAYTRACING);

		OpenVkTraceRaysInfo TraceRaysInfo;
		TraceRaysInfo.Width = SceneWidth;
		TraceRaysInfo.Height = SceneHeight;
		TraceRaysInfo.RaygenShader = RTR.ShaderBindingTable[0];
		TraceRaysInfo.RaygenHandleCount = 1;
		TraceRaysInfo.MissShader = RTR.ShaderBindingTable[1];
		TraceRaysInfo.MissHandleCount = 2;
		TraceRaysInfo.HitShader = RTR.ShaderBindingTable[2];
		TraceRaysInfo.HitHandleCount = 1;

		OpenVkTraceRays(&TraceRaysInfo);



		//	if (RenderRaytraced)
	}

	OpenVkCopyImage(SceneWidth, SceneHeight, OPENVK_IMAGE_TYPE_STORAGE, RTR.StorageImage, OPENVK_IMAGE_TYPE_ATTACHMENT, SceneAttachment, OpenVkTrue);
}

void RaytracingDestroy()
{
	if (!OpenVkHasRaytracingSupport())
		return;

	CMA_Destroy(&RTR.Geometry);
	CMA_Destroy(&RTR.BottomLevelAS);
	DynamicArrayDestroy(&RTR.DescriptionBuffers);
	DynamicArrayDestroy(&RTR.Instances);
	DynamicArrayDestroy(&RTR.ImageLayouts);
	DynamicArrayDestroy(&RTR.Images);
	DynamicArrayDestroy(&RTR.ImageTypes);
	DynamicArrayDestroy(&RTR.ImageSampler);
	DynamicArrayDestroy(&RTR.Meshes);
}