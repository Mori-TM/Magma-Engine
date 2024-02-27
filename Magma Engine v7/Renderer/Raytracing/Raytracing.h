typedef struct
{
	mat4 viewInverse;
	mat4 projInverse;
	vec4 LightDir;
	uint32_t Time;
} RaytracingUniformBufferObject;

typedef struct
{
	bool WasInit;
	uint32_t DescriptorSetLayout;
	uint32_t DescriptorPoolBufferCount;
	uint32_t DescriptorPool;
	uint32_t DescriptorSet;
	uint32_t UniformBuffer;
	uint32_t PipelineLayout;
	uint32_t TransformBufferCount;
	uint32_t* TransformBuffers;
	uint32_t VertexCount;
	uint32_t IndexCount;
	uint32_t VertexBuffer;
	uint32_t IndexBuffer;
	uint32_t GeometryCount;
	uint32_t* Geometry;
	uint32_t InstanceCount;
	uint32_t* Instances;
	uint32_t* BottomLevelAS;
	uint32_t TopLevelAS;
	uint32_t StorageImage;
	uint32_t RaytracingPipeline;
	uint32_t* ShaderBindingTable;


	uint32_t ImageCount;
	uint32_t* ImageLayouts;
	uint32_t* Images;
	uint32_t* ImageTypes;
	uint32_t* ImageSampler;
} RaytracingRenderer;

RaytracingRenderer RTR;

void RtCountBuffer(uint32_t* VertexBufferCount, uint32_t* IndexBufferCount)
{
	uint32_t VertexCount = 0;
	uint32_t IndexCount = 0;

	for (uint32_t i = 0; i < SceneMeshes.Size; i++)
	{
		SceneMesh* Mesh = (SceneMesh*)CMA_GetAt(&SceneMeshes, i);
		if (Mesh)
		{
			if (Mesh->IndexBuffer != OPENVK_ERROR)
				IndexCount++;

			VertexCount++;
		}
	}

	*VertexBufferCount = VertexCount;
	*IndexBufferCount = IndexCount;
}

void RtUpdateDescriptors(bool Update)
{
	if (RTR.ImageCount <= SceneTextures.Size)
	{
		RTR.ImageCount		= SceneTextures.Size + 32;

		RTR.ImageLayouts	= (uint32_t*)realloc(RTR.ImageLayouts, RTR.ImageCount * sizeof(uint32_t));
		RTR.Images			= (uint32_t*)realloc(RTR.Images, RTR.ImageCount * sizeof(uint32_t));
		RTR.ImageTypes		= (uint32_t*)realloc(RTR.ImageTypes, RTR.ImageCount * sizeof(uint32_t));
		RTR.ImageSampler	= (uint32_t*)realloc(RTR.ImageSampler, RTR.ImageCount * sizeof(uint32_t));
	}

	uint32_t VertexBufferCount = 0;
	uint32_t IndexBufferCount = 0;
	RtCountBuffer(&VertexBufferCount, &IndexBufferCount);
	uint32_t TotalBufferCount = VertexBufferCount + IndexBufferCount;

	if (!Update || RTR.DescriptorPoolBufferCount <= SceneMeshes.Size)
	{
		RTR.DescriptorPoolBufferCount += SceneMeshes.Size + 8;

		uint32_t DescriptorTypes[] =
		{
			OPENVK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE,
			OPENVK_DESCRIPTOR_TYPE_STORAGE_IMAGE,
			OPENVK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
			OPENVK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
			OPENVK_DESCRIPTOR_TYPE_IMAGE_SAMPLER
		};
		uint32_t DescriptorCounts[] = { 1, 1, 1, RTR.DescriptorPoolBufferCount, SceneTextures.Size };
		RTR.DescriptorPool = OpenVkCreateDescriptorPool(OPENVK_DESCRIPTOR_POOL_UPDATABLE, 5, DescriptorTypes, DescriptorCounts);
	}

	uint32_t DescriptorTypes[] =
	{
		OPENVK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE,
		OPENVK_DESCRIPTOR_TYPE_STORAGE_IMAGE,
		OPENVK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
		OPENVK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
		OPENVK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
		OPENVK_DESCRIPTOR_TYPE_IMAGE_SAMPLER
	};

	uint32_t TextureCount = 1;

	RTR.ImageLayouts[0] = OPENVK_IMAGE_LAYOUT_GENERAL_OUTPUT;
	RTR.Images[0] = RTR.StorageImage;
	RTR.ImageTypes[0] = OPENVK_IMAGE_TYPE_STORAGE;
	RTR.ImageSampler[0] = -1; //Doesn't matter, I guess

	for (uint32_t i = 1; i < SceneTextures.Size + 1; i++)
	{
		SceneTextureImage* Texture = (SceneTextureImage*)CMA_GetAt(&SceneTextures, i);
		if (Texture)
		{
			RTR.ImageLayouts[TextureCount] = OPENVK_IMAGE_LAYOUT_COLOR_OUTPUT;
			RTR.Images[TextureCount] = Texture->TextureImage;
			RTR.ImageTypes[TextureCount] = OPENVK_IMAGE_TYPE_TEXTURE;
			RTR.ImageSampler[TextureCount] = Texture->TextureSampler;
			TextureCount++;
		}
	}
	TextureCount -= 1;

	uint32_t DescriptorCounts[] = { 1, 1, 1, 1, 1, TextureCount };

	uint32_t Bindings[] = { 0, 1, 2, 3, 4, 5 };

	uint32_t TopLevel[] = { RTR.TopLevelAS };

	uint32_t Buffers[] = { RTR.UniformBuffer, RTR.VertexBuffer, RTR.IndexBuffer };
	size_t Sizes[] = { sizeof(RaytracingUniformBufferObject), 0, 0 };

	printf("VertexBuffer: %d\n", RTR.VertexBuffer);
	printf("IndexBuffer: %d\n", RTR.IndexBuffer);
	printf("UniformBuffer: %d\n", RTR.UniformBuffer);

	OpenVkDescriptorSetCreateInfo DescriptorSetCreateInfo;
	DescriptorSetCreateInfo.DescriptorSetLayout = RTR.DescriptorSetLayout;
	DescriptorSetCreateInfo.DescriptorPool = RTR.DescriptorPool;
	DescriptorSetCreateInfo.DescriptorWriteCount = ARRAY_SIZE(DescriptorTypes);
	DescriptorSetCreateInfo.DescriptorCounts = DescriptorCounts;
	DescriptorSetCreateInfo.DescriptorTypes = DescriptorTypes;
	DescriptorSetCreateInfo.Buffers = Buffers;
	DescriptorSetCreateInfo.BufferSizes = Sizes;
	DescriptorSetCreateInfo.ImageLayouts = RTR.ImageLayouts;
	DescriptorSetCreateInfo.Images = RTR.Images;
	DescriptorSetCreateInfo.ImageTypes = RTR.ImageTypes;
	DescriptorSetCreateInfo.Sampler = RTR.ImageSampler;
	DescriptorSetCreateInfo.TopLevelAS = TopLevel;
	DescriptorSetCreateInfo.Bindings = Bindings;
	DescriptorSetCreateInfo.DescriptorSet = NULL;
	DescriptorSetCreateInfo.VariableDescriptorSetCount = TextureCount;
	if (Update) DescriptorSetCreateInfo.DescriptorSet = &RTR.DescriptorSet;
	RTR.DescriptorSet = OpenVkCreateDescriptorSet(&DescriptorSetCreateInfo);
}


void RaytracingCreateBuffers()
{

}

void RaytracingUpdateBuffers()
{

}

void RaytracingBeginFrame()
{
	
}

void RaytracingInit()
{
	memset(&RTR, 0, sizeof(RaytracingRenderer));

	uint32_t VertexBufferCount = 0;
	uint32_t IndexBufferCount = 0;
	RtCountBuffer(&VertexBufferCount, &IndexBufferCount);

	RTR.GeometryCount = VertexBufferCount;
	RTR.Geometry = (uint32_t*)malloc(RTR.GeometryCount * sizeof(uint32_t));

	RTR.InstanceCount = VertexBufferCount;
	RTR.Instances = (uint32_t*)malloc(RTR.InstanceCount * sizeof(uint32_t));

	RTR.InstanceCount = 0;
	RTR.GeometryCount = 0;

	for (uint32_t i = 0; i < EntityCount; i++)
	{
		if (Entities[i].UsedComponents[COMPONENT_TYPE_MESH])
		{

		}
	}


	for (uint32_t i = 0; i < SceneMeshes.Size; i++)
	{
		SceneMesh* Mesh = (SceneMesh*)CMA_GetAt(&SceneMeshes, i);
		if (Mesh)
		{
			if (Mesh->IndexBuffer != OPENVK_ERROR)
			{

			}
		}
	}

	RTR.Geometry[0] = VkCreateRaytracingGeometry(4, sizeof(Vertex), VertexCount, VertexBuffer, IndexCount, RTR.IndexBuffer, RTR.TransformBuffer);
	RTR.BottomLevelAS[0] = VkCreateBottomLevelAccelerationStructure(Geometry[0], OpenVkFalse, NULL);
	RTR.Instances[0] = VkCreateInstance(Matrix, OpenVkFalse, BottomLevelAS[0]);
	RTR.TopLevelAS = OpenVkCreateTopLevelAS(RTR.GeometryCount, RTR.Instances, OpenVkTrue, NULL);

	RtUpdateDescriptors(false);
}

void RaytracingUpdate()
{

}

void RaytracingDraw()
{

}

void RaytracingDestroy()
{

}