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
	uint32_t BufferCount;
	uint32_t* Buffers;
	uint32_t GeometryCount;
	uint32_t* Geometry;
	uint32_t InstanceCount;
	uint32_t* Instances;
	uint32_t BottomLevelASCount;
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
uint32_t RaytracingOutDescriptorSet;

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
		RTR.DescriptorPoolBufferCount += SceneMeshes.Size + 8;//+8 so we have somme more storage

		uint32_t DescriptorTypes[] =
		{
			OPENVK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE,
			OPENVK_DESCRIPTOR_TYPE_STORAGE_IMAGE,
			OPENVK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
			OPENVK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
			OPENVK_DESCRIPTOR_TYPE_IMAGE_SAMPLER
		};
		uint32_t DescriptorCounts[] = { 1, 1, 1, RTR.DescriptorPoolBufferCount, SceneTextures.Size + 1 }; //plus one storage image?
		RTR.DescriptorPool = OpenVkCreateDescriptorPool(OPENVK_DESCRIPTOR_POOL_UPDATABLE, 5, DescriptorTypes, DescriptorCounts);
	}
	// we also need to destroy the pool before recreating

	uint32_t DescriptorTypes[] =
	{
		OPENVK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE,
		OPENVK_DESCRIPTOR_TYPE_STORAGE_IMAGE,
		OPENVK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
		OPENVK_DESCRIPTOR_TYPE_VERTEX_BUFFER,
		OPENVK_DESCRIPTOR_TYPE_INDEX_BUFFER,
		OPENVK_DESCRIPTOR_TYPE_IMAGE_SAMPLER
	};

	uint32_t TextureCount = 1;

	RTR.ImageLayouts[0] = OPENVK_IMAGE_LAYOUT_GENERAL_OUTPUT;
	RTR.Images[0] = RTR.StorageImage;
	RTR.ImageTypes[0] = OPENVK_IMAGE_TYPE_STORAGE;
	RTR.ImageSampler[0] = ImageSampler; //Doesn't matter, I guess

	for (uint32_t i = 0; i < SceneTextures.Size; i++)
	{
		SceneTextureImage* Texture = (SceneTextureImage*)CMA_GetAt(&SceneTextures, i);
		if (Texture)
		{
			RTR.ImageLayouts[TextureCount] = OPENVK_IMAGE_LAYOUT_COLOR_OUTPUT;
			RTR.Images[TextureCount] = Texture->TextureImage;
			RTR.ImageTypes[TextureCount] = OPENVK_IMAGE_TYPE_TEXTURE;
			RTR.ImageSampler[TextureCount] = Texture->TextureSampler;

			OpenVkRuntimeError("PP: Texture: %d, Sampler: %d", Texture->TextureImage, Texture->TextureSampler);

			TextureCount++;
		}
	}
	TextureCount -= 1;

	uint32_t DescriptorCounts[] = { 1, 1, 1, VertexBufferCount, IndexBufferCount, TextureCount };

	uint32_t Bindings[] = { 0, 1, 2, 3, 4, 5 };

	uint32_t TopLevel[] = { RTR.TopLevelAS };

//	uint32_t Buffers[] = { RTR.UniformBuffer, RTR.VertexBuffer, RTR.IndexBuffer };
											// + 1 Uniform Buffer
	RTR.Buffers = (uint32_t*)malloc((TotalBufferCount + 1) * sizeof(uint32_t));

	RTR.Buffers[0] = RTR.UniformBuffer;
	RTR.BufferCount = 1;

	for (uint32_t i = 0; i < SceneMeshes.Size; i++)
	{
		SceneMesh* Mesh = (SceneMesh*)CMA_GetAt(&SceneMeshes, i);
		if (Mesh && Mesh->MeshCount > 0)
		{
			RTR.Buffers[RTR.BufferCount++] = Mesh->VertexBuffer;

			if (Mesh->IndexBuffer != OPENVK_ERROR)
				RTR.Buffers[RTR.BufferCount++] = Mesh->IndexBuffer;

			
		}
	}

	{
		uint32_t DescriptorCounts[] = { 1, 1, 1, VertexBufferCount, IndexBufferCount, TextureCount };
		uint32_t Bindings[] = { 0, 1, 2, 3, 4, 5 };
		uint32_t DescriptorTypes[] =
		{
			OPENVK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE,
			OPENVK_DESCRIPTOR_TYPE_STORAGE_IMAGE,
			OPENVK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
			OPENVK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
			OPENVK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
			OPENVK_DESCRIPTOR_TYPE_IMAGE_SAMPLER
		};
		uint32_t DescriptorFlags[] =
		{
			OPENVK_DESCRIPTOR_FLAG_NONE,
			OPENVK_DESCRIPTOR_FLAG_NONE,
			OPENVK_DESCRIPTOR_FLAG_NONE,
			OPENVK_DESCRIPTOR_FLAG_NONE,
			OPENVK_DESCRIPTOR_FLAG_NONE,
			OPENVK_DESCRIPTOR_FLAG_VARIABLE_DESCRIPTOR_COUNT
		};
		uint32_t ShaderTypes[] =
		{
			OPENVK_SHADER_TYPE_RAYGEN | OPENVK_SHADER_TYPE_CLOSEST_HIT,
			OPENVK_SHADER_TYPE_RAYGEN,
			OPENVK_SHADER_TYPE_RAYGEN | OPENVK_SHADER_TYPE_CLOSEST_HIT | OPENVK_SHADER_TYPE_MISS,
			OPENVK_SHADER_TYPE_CLOSEST_HIT,
			OPENVK_SHADER_TYPE_CLOSEST_HIT,
			OPENVK_SHADER_TYPE_CLOSEST_HIT
		};
		RTR.DescriptorSetLayout = OpenVkCreateDescriptorSetLayout(ARRAY_SIZE(DescriptorTypes), Bindings, DescriptorCounts, DescriptorTypes, DescriptorFlags, ShaderTypes);
		//	exit(0);
	}

	size_t Sizes[] = { sizeof(RaytracingUniformBufferObject), 0, 0 };

	printf("BufferCount: %d\n", RTR.BufferCount);
	printf("UniformBuffer: %d\n", RTR.UniformBuffer);

	OpenVkDescriptorSetCreateInfo DescriptorSetCreateInfo;
	DescriptorSetCreateInfo.DescriptorSetLayout = RTR.DescriptorSetLayout;
	DescriptorSetCreateInfo.DescriptorPool = RTR.DescriptorPool;
	DescriptorSetCreateInfo.DescriptorWriteCount = ARRAY_SIZE(DescriptorTypes);
	DescriptorSetCreateInfo.DescriptorCounts = DescriptorCounts;
	DescriptorSetCreateInfo.DescriptorTypes = DescriptorTypes;
	DescriptorSetCreateInfo.Buffers = RTR.Buffers;
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
	OpenVkRuntimeInfo("LGTM!", "");
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

	RTR.TransformBufferCount = VertexBufferCount;
	RTR.TransformBuffers = (uint32_t*)malloc(RTR.TransformBufferCount * sizeof(uint32_t));

	RTR.BottomLevelASCount = VertexBufferCount;
	RTR.BottomLevelAS = (uint32_t*)malloc(RTR.BottomLevelASCount * sizeof(uint32_t));

	RTR.TransformBufferCount = 0;
	RTR.InstanceCount = 0;
	RTR.GeometryCount = 0;
	RTR.BottomLevelASCount = 0;

	for (uint32_t i = 0; i < EntityCount; i++)
	{
		if (Entities[i].UsedComponents[COMPONENT_TYPE_MESH])
		{
			OpenVkRuntimeWarning("Yeah buddy: %d", RTR.InstanceCount);

			SceneMesh* Mesh = (SceneMesh*)CMA_GetAt(&SceneMeshes, Entities[i].Mesh.MeshIndex);
			if (Mesh != NULL && Mesh->MeshCount > 0)
			{
				mat4 Model;
				LoadMat4IdentityP(&Model);
				Model = ScaleMat4P(&Model, &Entities[i].Scale);
				Model = RotateXMat4P(&Model, ToRadians(Entities[i].Rotate.x));
				Model = RotateYMat4P(&Model, ToRadians(Entities[i].Rotate.y));
				Model = RotateZMat4P(&Model, ToRadians(Entities[i].Rotate.z));
				Model = TranslateMat4P(&Model, &Entities[i].Translate);
				OpenVkTransformMatrix ModelOVK;
				memcpy(&ModelOVK, &Model, sizeof(OpenVkTransformMatrix));

				RTR.TransformBuffers[RTR.TransformBufferCount++] = VkCreateTranformBuffer(ModelOVK);

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
				GeometryInfo.IndexBuffer = Mesh->IndexBuffer == OPENVK_ERROR ? 0 : Mesh->IndexBuffer;
				GeometryInfo.TranformBuffer = RTR.TransformBuffers[RTR.TransformBufferCount - 1];

				RTR.Geometry[RTR.GeometryCount++] = OpenVkCreateRaytracingGeometry(&GeometryInfo);
				RTR.BottomLevelAS[RTR.BottomLevelASCount++] = OpenVkCreateBottomLevelAS(RTR.Geometry[RTR.GeometryCount - 1], OpenVkFalse, NULL);
				RTR.Instances[RTR.InstanceCount++] = OpenVkCreateInstance(ModelOVK, OpenVkFalse, RTR.BottomLevelAS[RTR.BottomLevelASCount - 1]);
				
			}
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

	OpenVkRuntimeWarning("Instance Count: %d", RTR.InstanceCount);

	RTR.TopLevelAS = OpenVkCreateTopLevelAS(RTR.InstanceCount, RTR.Instances, OpenVkTrue, NULL);

	RTR.UniformBuffer = OpenVkCreateUniformBuffer(sizeof(RaytracingUniformBufferObject));

	RTR.StorageImage = OpenVkCreateStorageImage(WindowWidth, WindowHeight, OPENVK_FORMAT_DEFAULT);

	RtUpdateDescriptors(false);

	{
		OpenVkPipelineLayoutCreateInfo LayoutCreateInfo;
		LayoutCreateInfo.PushConstantCount = 0;
		LayoutCreateInfo.PushConstantShaderTypes = NULL;
		LayoutCreateInfo.PushConstantOffsets = NULL;
		LayoutCreateInfo.PushConstantSizes = NULL;
		LayoutCreateInfo.DescriptorSetLayoutCount = 1;
		LayoutCreateInfo.DescriptorSetLayouts = &RTR.DescriptorSetLayout;
		RTR.PipelineLayout = OpenVkCreatePipelineLayout(&LayoutCreateInfo);
	}

	{
		uint32_t ShaderTypes[] = { OPENVK_SHADER_TYPE_RAYGEN, OPENVK_SHADER_TYPE_MISS, OPENVK_SHADER_TYPE_MISS, OPENVK_SHADER_TYPE_CLOSEST_HIT };
		OpenVkFile Shader[] = { OpenVkReadFile("Data/Shader/Raytracing/raygen.spv"), OpenVkReadFile("Data/Shader/Raytracing/miss.spv"), OpenVkReadFile("Data/Shader/Raytracing/shadow.spv"), OpenVkReadFile("Data/Shader/Raytracing/closesthit.spv") };
		RTR.RaytracingPipeline = VkCreateRaytracingPipeline(5, RTR.PipelineLayout, 4, ShaderTypes, Shader);

		uint32_t HandleCounts[] = { 1, 2, 1 };
		RTR.ShaderBindingTable = VkCreateShaderBindingTable(RTR.RaytracingPipeline, 3, HandleCounts);
	}
}

void RaytracingUpdate()
{
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
}

void RaytracingDraw()
{
	if (RenderRaytraced)
	{

		OpenVkBindPipeline(RTR.RaytracingPipeline, OPENVK_PIPELINE_TYPE_RAYTRACING);
		OpenVkBindDescriptorSet(RTR.PipelineLayout, 0, RTR.DescriptorSet, OPENVK_PIPELINE_TYPE_RAYTRACING);

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
		OpenVkCopyImage(SceneWidth, SceneHeight, OPENVK_IMAGE_TYPE_STORAGE, RTR.StorageImage, OPENVK_IMAGE_TYPE_ATTACHMENT, SceneAttachment);
		
	}

}

void RaytracingDestroy()
{

}