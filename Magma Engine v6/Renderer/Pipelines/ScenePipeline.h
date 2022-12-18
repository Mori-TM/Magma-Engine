void CreateSceneRenderPass()
{
	SceneMsaaAttachment = OpenVkCreateColorImageAttachment(SceneWidth, SceneHeight, MsaaSamples, false, OPENVK_FORMAT_DEFAULT);
	SceneDepthAttachment = OpenVkCreateDepthImageAttachment(SceneWidth, SceneHeight, MsaaSamples, false, OPENVK_FORMAT_DEFAULT);
	SceneColorAttachment = OpenVkCreateColorImageAttachment(SceneWidth, SceneHeight, 1, true, OPENVK_FORMAT_DEFAULT);
	uint32_t ColorFormats[] = { OPENVK_FORMAT_DEFAULT };
	SceneRenderPass = OpenVkCreateRenderPass(1, ColorFormats, true, OPENVK_FORMAT_DEFAULT, true, MsaaSamples, true);
}

#define RIGID_BODY_COUNT 3
WaveRigidBody RigidBodys[RIGID_BODY_COUNT];

void CreateSceneLayout()
{
	uint32_t DescriptorSetLayouts[] =
	{
		TextureDescriptorSetLayout,
		TextureDescriptorSetLayout,
		TextureDescriptorSetLayout,
		TextureDescriptorSetLayout,
		TextureDescriptorSetLayout,
		ShadowMapDescriptorSetLayout,
		FragmentUniformDescriptorSetLayout,
		VertexUniformDescriptorSetLayout
	};

	uint32_t PushTypes[] = { OPENVK_SHADER_TYPE_VERTEX, OPENVK_SHADER_TYPE_FRAGMENT };
	uint32_t PushOffsets[] = { 0, 64 };
	uint32_t PushSizes[] = { sizeof(SceneVertexPushConstant), sizeof(SceneFragmentPushConstant) };

	OpenVkPipelineLayoutCreateInfo Layout;
	Layout.PushConstantCount = 2;
	Layout.PushConstantShaderTypes = PushTypes;
	Layout.PushConstantOffsets = PushOffsets;
	Layout.PushConstantSizes = PushSizes;
	Layout.DescriptorSetLayoutCount = ARRAY_SIZE(DescriptorSetLayouts);
	Layout.DescriptorSetLayouts = DescriptorSetLayouts;
	SceneLayout = OpenVkCreatePipelineLayout(&Layout);
}

void CreateScenePipeline()
{
	uint32_t ShaderAttributeFormats[] = { OPENVK_FORMAT_RGB32F, OPENVK_FORMAT_RG32F, OPENVK_FORMAT_RGB32F };
	uint32_t ShaderAttributeOffsets[] = { 0, 12, 20 };

	OpenVkGraphicsPipelineCreateInfo GraphicsPipelineCreateInfo;
	GraphicsPipelineCreateInfo.VertexShader = OpenVkReadFile("Data/Shader/SceneVertex.spv");
	GraphicsPipelineCreateInfo.FragmentShader = OpenVkReadFile("Data/Shader/SceneFragment.spv");
	GraphicsPipelineCreateInfo.BindingStride = sizeof(SceneVertex);
	GraphicsPipelineCreateInfo.ShaderAttributeFormatCount = 3;
	GraphicsPipelineCreateInfo.ShaderAttributeFormats = ShaderAttributeFormats;
	GraphicsPipelineCreateInfo.ShaderAttributeOffsets = ShaderAttributeOffsets;
	GraphicsPipelineCreateInfo.PrimitiveTopology = OPENVK_PRIMITIVE_TOPOLOGY_TRIANGLE;
	GraphicsPipelineCreateInfo.x = 0;
	GraphicsPipelineCreateInfo.y = 0;
	GraphicsPipelineCreateInfo.Width = SceneWidth;
	GraphicsPipelineCreateInfo.Height = SceneHeight;
	GraphicsPipelineCreateInfo.DepthClamp = false;
	GraphicsPipelineCreateInfo.PolygonMode = OPENVK_POLYGON_MODE_LINE;
	GraphicsPipelineCreateInfo.LineWidth = 3.0;
	GraphicsPipelineCreateInfo.CullMode = OPENVK_CULL_MODE_BACK;
	GraphicsPipelineCreateInfo.FrontFace = OPENVK_FRONT_FACE_COUNTER_CLOCK_WISE;
	GraphicsPipelineCreateInfo.MsaaSamples = MsaaSamples;
	GraphicsPipelineCreateInfo.AlphaBlending = true;
	GraphicsPipelineCreateInfo.ColorBlendAttachments = 1;	
	GraphicsPipelineCreateInfo.PipelineLayout = SceneLayout;
	GraphicsPipelineCreateInfo.DepthStencil = true;
	GraphicsPipelineCreateInfo.RenderPass = SceneRenderPass;
	ScenePipelineBackCull = OpenVkCreateGraphicsPipeline(&GraphicsPipelineCreateInfo);

	GraphicsPipelineCreateInfo.VertexShader = OpenVkReadFile("Data/Shader/SceneVertex.spv");
	GraphicsPipelineCreateInfo.FragmentShader = OpenVkReadFile("Data/Shader/SceneFragment.spv");
	GraphicsPipelineCreateInfo.CullMode = OPENVK_CULL_MODE_NONE;
	ScenePipelineNoneCull = OpenVkCreateGraphicsPipeline(&GraphicsPipelineCreateInfo);

	for (uint32_t i = 0; i < RIGID_BODY_COUNT; i++)
	{
		RigidBodys[i].Mass = 0.1;
	//	RigidBodys[i].Force.x = (float)RandomInt(-1000, 1000) * 0.1;
	//	RigidBodys[i].Force.y = (float)RandomInt(0, 1000) * 0.1 + 100;
	//	RigidBodys[i].Force.z = (float)RandomInt(-1000, 1000) * 0.1;
		RigidBodys[i].Force.x = 0;
		if (i == 0)
			RigidBodys[i].Force.y = 200;
		RigidBodys[i].Force.z = 0;

	//	RigidBodys[i].Velocity.x = 0.0;
	//	RigidBodys[i].Velocity.y = -0.01;
	//	RigidBodys[i].Velocity.z = 0.0;
		RigidBodys[i].Position.x = 0.0;
		RigidBodys[i].Position.y = i * 8;
		RigidBodys[i].Position.z = 0.0;
		RigidBodys[i].Rotation.x = 0.0;
		RigidBodys[i].Rotation.y = 0.0;
		RigidBodys[i].Rotation.z = 0.0;
		RigidBodys[i].Scale.x = 1.0;
		RigidBodys[i].Scale.y = 1.0;
		RigidBodys[i].Scale.z = 1.0;
		RigidBodys[i].Type = WAVE_RIGID_BODY_TYPE_SPHERE;
		WaveAddRigidBody(&RigidBodys[i]);
	}
}

void CreateSceneFramebuffer()
{
	uint32_t Attachments[] = { SceneMsaaAttachment, SceneDepthAttachment, SceneColorAttachment };

	OpenVkFramebufferCreateInfo FramebufferCreateInfo;
	FramebufferCreateInfo.AttachmentCount = ARRAY_SIZE(Attachments);
	FramebufferCreateInfo.Attachments = Attachments;
	FramebufferCreateInfo.RenderPass = SceneRenderPass;
	FramebufferCreateInfo.Width = SceneWidth;
	FramebufferCreateInfo.Height = SceneHeight;
	SceneFramebuffer = OpenVkCreateFramebuffer(&FramebufferCreateInfo);
}

void CreateSceneUniformBuffer()
{
	SceneVertexUniformBuffer = OpenVkCreateUniformBuffer(sizeof(SceneVertexUniformBufferObject));
	SceneFragmentUniformBuffer = OpenVkCreateUniformBuffer(sizeof(SceneFragmentUniformBufferObject));
}

void CreateSceneDescriptorSets()
{	
	for (uint32_t i = 0; i < SceneTextures.Size; i++)
	{
		SceneTextureImage* Image = (SceneTextureImage*)CMA_GetAt(&SceneTextures, i);
		if (Image != NULL)
		{
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

			Image->TextureDescriptorSet = OpenVkCreateDescriptorSet(&DescriptorSetCreateInfo);
		}		
	}	
	
	{
		uint32_t DescriptorCounts[] = { 1 };
		uint32_t DescriptorTypes[] = { OPENVK_DESCRIPTOR_TYPE_UNIFORM_BUFFER };
		uint32_t UniformBuffers[] = { SceneVertexUniformBuffer };
		size_t UniformSizes[] = { sizeof(SceneVertexUniformBufferObject) };
		uint32_t Bindings[] = { 0 };

		OpenVkDescriptorSetCreateInfo DescriptorSetCreateInfo;
		DescriptorSetCreateInfo.DescriptorSetLayout = VertexUniformDescriptorSetLayout;
		DescriptorSetCreateInfo.DescriptorPool = DescriptorPool;
		DescriptorSetCreateInfo.DescriptorWriteCount = 1;
		DescriptorSetCreateInfo.DescriptorCounts = DescriptorCounts;
		DescriptorSetCreateInfo.DescriptorTypes = DescriptorTypes;
		DescriptorSetCreateInfo.Buffers = UniformBuffers;
		DescriptorSetCreateInfo.BufferSizes = UniformSizes;
		DescriptorSetCreateInfo.Bindings = Bindings;
		DescriptorSetCreateInfo.DescriptorSet = NULL;

		SceneVertexUniformDescriptorSet = OpenVkCreateDescriptorSet(&DescriptorSetCreateInfo);
	}

	{
		uint32_t DescriptorCounts[] = { 1 };
		uint32_t DescriptorTypes[] = { OPENVK_DESCRIPTOR_TYPE_UNIFORM_BUFFER };
		uint32_t UniformBuffers[] = { SceneFragmentUniformBuffer };
		size_t UniformSizes[] = { sizeof(SceneFragmentUniformBufferObject) };
		uint32_t Bindings[] = { 0 };

		OpenVkDescriptorSetCreateInfo DescriptorSetCreateInfo;
		DescriptorSetCreateInfo.DescriptorSetLayout = FragmentUniformDescriptorSetLayout;
		DescriptorSetCreateInfo.DescriptorPool = DescriptorPool;
		DescriptorSetCreateInfo.DescriptorWriteCount = 1;
		DescriptorSetCreateInfo.DescriptorCounts = DescriptorCounts;
		DescriptorSetCreateInfo.DescriptorTypes = DescriptorTypes;
		DescriptorSetCreateInfo.Buffers = UniformBuffers;
		DescriptorSetCreateInfo.BufferSizes = UniformSizes;
		DescriptorSetCreateInfo.Bindings = Bindings;
		DescriptorSetCreateInfo.DescriptorSet = NULL;

		SceneFragmentUniformDescriptorSet = OpenVkCreateDescriptorSet(&DescriptorSetCreateInfo);
	}
	
	{
		uint32_t DescriptorCounts[] = { 1 };
		uint32_t DescriptorTypes[] = { OPENVK_DESCRIPTOR_TYPE_IMAGE_SAMPLER };
		uint32_t ImageTypes[] = { OPENVK_IMAGE_TYPE_ATTACHMENT };
		uint32_t ImageLayouts[] = { OPENVK_IMAGE_LAYOUT_COLOR_OUTPUT };
		uint32_t Bindings[] = { 0 };

		OpenVkDescriptorSetCreateInfo DescriptorSetCreateInfo;
		DescriptorSetCreateInfo.DescriptorSetLayout = TextureDescriptorSetLayout;
		DescriptorSetCreateInfo.DescriptorPool = DescriptorPool;
		DescriptorSetCreateInfo.DescriptorWriteCount = 1;
		DescriptorSetCreateInfo.DescriptorCounts = DescriptorCounts;
		DescriptorSetCreateInfo.DescriptorTypes = DescriptorTypes;
		DescriptorSetCreateInfo.Sampler = &ImageSampler;
		DescriptorSetCreateInfo.ImageTypes = ImageTypes;
		DescriptorSetCreateInfo.Images = &SceneColorAttachment;
		DescriptorSetCreateInfo.ImageLayouts = ImageLayouts;
		DescriptorSetCreateInfo.Bindings = Bindings;
		DescriptorSetCreateInfo.DescriptorSet = NULL;

		SceneDescriptorSet = OpenVkCreateDescriptorSet(&DescriptorSetCreateInfo);
	}

	{
		uint32_t DescriptorCounts[] = { 1 };
		uint32_t DescriptorTypes[] = { OPENVK_DESCRIPTOR_TYPE_IMAGE_SAMPLER };
		uint32_t ImageTypes[] = { OPENVK_IMAGE_TYPE_ATTACHMENT };
		uint32_t ImageLayouts[] = { OPENVK_IMAGE_LAYOUT_DEPTH_OUTPUT };
		uint32_t Bindings[] = { 0 };

		OpenVkDescriptorSetCreateInfo DescriptorSetCreateInfo;
		DescriptorSetCreateInfo.DescriptorSetLayout = ShadowMapDescriptorSetLayout;
		DescriptorSetCreateInfo.DescriptorPool = DescriptorPool;
		DescriptorSetCreateInfo.DescriptorWriteCount = 1;
		DescriptorSetCreateInfo.DescriptorCounts = DescriptorCounts;
		DescriptorSetCreateInfo.DescriptorTypes = DescriptorTypes;
		DescriptorSetCreateInfo.Sampler = &ShadowSampler;
		DescriptorSetCreateInfo.ImageTypes = ImageTypes;
		DescriptorSetCreateInfo.Images = &ShadowDepthAttachment;
		DescriptorSetCreateInfo.ImageLayouts = ImageLayouts;
		DescriptorSetCreateInfo.Bindings = Bindings;
		DescriptorSetCreateInfo.DescriptorSet = NULL;

		SceneShadowMapDescriptorSet = OpenVkCreateDescriptorSet(&DescriptorSetCreateInfo);
	}
}

void SceneUpdateUniformBuffer()
{
	LoadMat4IdentityP(&SceneVertexUBO.View);
	if (GameMode)
	{
		for (uint32_t i = 0; i < EntityCount; i++)
		{
			if (Entities[i].UsedComponents[COMPONENT_TYPE_CAMERA])
			{
				LoadMat4IdentityP(&SceneVertexUBO.View);
				SceneVertexUBO.View = RotateXMat4P(&SceneVertexUBO.View, ToRadians(Entities[i].Rotate.x));
				SceneVertexUBO.View = RotateYMat4P(&SceneVertexUBO.View, ToRadians(Entities[i].Rotate.y));
				SceneVertexUBO.View = RotateZMat4P(&SceneVertexUBO.View, ToRadians(Entities[i].Rotate.z));
				SceneVertexUBO.View = TranslateMat4P(&SceneVertexUBO.View, &Entities[i].Translate);
				SceneVertexUBO.View = InverseMat4(SceneVertexUBO.View);
				PerspectiveMat4P(((float)SceneWidth / SceneHeight), ToRadians(Entities[i].Camera.FOV), Entities[i].Camera.NearPlane, Entities[i].Camera.FarPlane, &SceneVertexUBO.Projection);
			}
		}
	}
	else
	{
		vec3 t = Add3P(&CameraPos, &CameraDir);
		SceneVertexUBO.View = LookAtMat4P(&CameraPos, &t, &CameraUp);
		PerspectiveMat4P(((float)SceneWidth / SceneHeight), ToRadians(CameraFOV), CameraNearZ, CameraFarZ, &SceneVertexUBO.Projection);
	}	

	for (uint32_t i = 0; i < SHADOW_MAP_CASCADE_COUNT; i++)
	{
		SceneFragmentUBO.CascadeSplits[i] = Cascades[i].SplitDepth;
		SceneFragmentUBO.CascadeProjectionView[i] = Cascades[i].ProjectionView;
	}

	OpenVkUpdateBuffer(sizeof(SceneVertexUniformBufferObject), &SceneVertexUBO, SceneVertexUniformBuffer);
	OpenVkUpdateBuffer(sizeof(SceneFragmentUniformBufferObject), &SceneFragmentUBO, SceneFragmentUniformBuffer);
}

int EntityDistCompareFunc(const void* a, const void* b)
{
	EntityInfo* A = (EntityInfo*)a;
	EntityInfo* B = (EntityInfo*)b;

	float DistA = GetDistanceVec3P(&CameraPos, &A->Translate);
	float DistB = GetDistanceVec3P(&CameraPos, &B->Translate);

	return DistB - DistA;
}

void SceneDraw()
{
	OpenVkBeginRenderPassInfo BeginInfo;
	BeginInfo.ClearColor[0] = ClearColor.x;
	BeginInfo.ClearColor[1] = ClearColor.y;
	BeginInfo.ClearColor[2] = ClearColor.z;
	BeginInfo.ClearColor[3] = 1.0;
	BeginInfo.ClearColors = 1;
	BeginInfo.ClearDepth = true;
	BeginInfo.RenderPass = SceneRenderPass;
	BeginInfo.Framebuffer = SceneFramebuffer;
	BeginInfo.x = 0;
	BeginInfo.y = 0;
	BeginInfo.Width = SceneWidth;
	BeginInfo.Height = SceneHeight;

	OpenVkBeginRenderPass(&BeginInfo);
	{
		OpenVkSetScissor(0, 0, SceneWidth, SceneHeight);
		OpenVkSetViewport(0, 0, SceneWidth, SceneHeight);

		if (!GameMode)
			DebugDraw();
		
		uint32_t Pipeline = ScenePipelineNoneCull;
		if (SceneBackfaceCulling)
			Pipeline = ScenePipelineBackCull;

		OpenVkBindPipeline(Pipeline, OPENVK_PIPELINE_TYPE_GRAPHICS);
/*
		OpenVkBindDescriptorSet(Pipeline, 1, SceneShadowMapDescriptorSet);
		OpenVkBindDescriptorSet(Pipeline, 2, SceneVertexUniformDescriptorSet);
		OpenVkBindDescriptorSet(Pipeline, 3, SceneFragmentUniformDescriptorSet);
		
	//	if (GameMode)
			WaveStepRidgidBodys(DeltaTime);
		
		OpenVkBindDescriptorSet(Pipeline, 0, SceneTextures[0].TextureDescriptorSet);
		for (uint32_t i = 0; i < RIGID_BODY_COUNT; i++)
		{
			LoadMat4IdentityP(&SceneVertexPc.Model);
			SceneVertexPc.Model = RotateXMat4P(&SceneVertexPc.Model, ToRadians(RigidBodys[i].Rotation.x));
			SceneVertexPc.Model = RotateYMat4P(&SceneVertexPc.Model, ToRadians(RigidBodys[i].Rotation.y));
			SceneVertexPc.Model = RotateZMat4P(&SceneVertexPc.Model, ToRadians(RigidBodys[i].Rotation.z));
			SceneVertexPc.Model = ScaleMat4(SceneVertexPc.Model, Vec3(RigidBodys[i].Scale.x, RigidBodys[i].Scale.y, RigidBodys[i].Scale.z));
			SceneVertexPc.Model = TranslateMat4(SceneVertexPc.Model, Vec3(RigidBodys[i].Position.x, RigidBodys[i].Position.y, RigidBodys[i].Position.z));

			SceneFragmentPc.UseTexture = false;
			SceneFragmentPc.UseVertexColor = true;
			SceneFragmentPc.Color = Vec3f(1.0);


			

			OpenVkPushConstant(Pipeline, OPENVK_SHADER_TYPE_VERTEX, 0, sizeof(SceneVertexPushConstant), &SceneVertexPc);
			OpenVkPushConstant(Pipeline, OPENVK_SHADER_TYPE_FRAGMENT, 64, sizeof(SceneFragmentPushConstant), &SceneFragmentPc);

			OpenVkBindIndexBuffer(SphereVertexBuffer, SphereIndexBuffer);
			OpenVkDrawIndices(SphereIndexCount);
		}
*/		
		
		OpenVkBindDescriptorSet(SceneLayout, 5, SceneShadowMapDescriptorSet, OPENVK_PIPELINE_TYPE_GRAPHICS);
		OpenVkBindDescriptorSet(SceneLayout, 6, SceneFragmentUniformDescriptorSet, OPENVK_PIPELINE_TYPE_GRAPHICS);
		OpenVkBindDescriptorSet(SceneLayout, 7, SceneVertexUniformDescriptorSet, OPENVK_PIPELINE_TYPE_GRAPHICS);
		
	//	LoadMat4IdentityP(&SceneVertexPc.Model);
	//	SceneVertexPc.Model = RotateXMat4P(&SceneVertexPc.Model, ToRadians(-90));
	//	SceneFragmentPc.Metallic = 0.0;
	//	SceneFragmentPc.Occlusion = 1.0;
	//	SceneFragmentPc.Roughness = 1.0;
	//	
	//	SceneFragmentPc.Color = Vec4f(1.0);
	//
	//	OpenVkPushConstant(SceneLayout, OPENVK_SHADER_TYPE_VERTEX, 0, sizeof(SceneVertexPushConstant), &SceneVertexPc);
	//	OpenVkPushConstant(SceneLayout, OPENVK_SHADER_TYPE_FRAGMENT, 64, sizeof(SceneFragmentPushConstant), &SceneFragmentPc);
	//
	//	uint32_t AlbedoDescriptorSet = 0;
	//	uint32_t NormalDescriptorSet = 0;
	//	uint32_t MetallicDescriptorSet = 0;
	//	uint32_t RoughnessDescriptorSet = 0;
	//	uint32_t OcclusionDescriptorSet = 0;
	//
	//	OpenVkBindDescriptorSet(SceneLayout, 0, AlbedoDescriptorSet, OPENVK_PIPELINE_TYPE_GRAPHICS);
	//	OpenVkBindDescriptorSet(SceneLayout, 1, NormalDescriptorSet, OPENVK_PIPELINE_TYPE_GRAPHICS);
	//	OpenVkBindDescriptorSet(SceneLayout, 2, MetallicDescriptorSet, OPENVK_PIPELINE_TYPE_GRAPHICS);
	//	OpenVkBindDescriptorSet(SceneLayout, 3, RoughnessDescriptorSet, OPENVK_PIPELINE_TYPE_GRAPHICS);
	//	OpenVkBindDescriptorSet(SceneLayout, 4, OcclusionDescriptorSet, OPENVK_PIPELINE_TYPE_GRAPHICS);

	//	OpenVkBindDynamicVertexBuffer(ModelVertexBuffer);
	//	OpenVkDrawVertices(Model.NumTriangles * 3);

	//	qsort(Entities, EntityCount, sizeof(EntityInfo), EntityDistCompareFunc);

		for (uint32_t i = 0; i < EntityCount; i++)
		{
			if (Entities[i].UsedComponents[COMPONENT_TYPE_MESH] ||
				Entities[i].UsedComponents[COMPONENT_TYPE_ANIMATION])
			{
				LoadMat4IdentityP(&SceneVertexPc.Model);
				SceneVertexPc.Model = ScaleMat4P(&SceneVertexPc.Model, &Entities[i].Scale);
				SceneVertexPc.Model = RotateXMat4P(&SceneVertexPc.Model, ToRadians(Entities[i].Rotate.x));
				SceneVertexPc.Model = RotateYMat4P(&SceneVertexPc.Model, ToRadians(Entities[i].Rotate.y));
				SceneVertexPc.Model = RotateZMat4P(&SceneVertexPc.Model, ToRadians(Entities[i].Rotate.z));
				SceneVertexPc.Model = TranslateMat4P(&SceneVertexPc.Model, &Entities[i].Translate);
				
				SceneMaterial* Material = (SceneMaterial*)CMA_GetAt(&SceneMaterials, Entities[i].Material.MaterialIndex);
				if (Material == NULL)
					Material = (SceneMaterial*)CMA_GetAt(&SceneMaterials, 0);
				
				SceneTextureImage* Albedo;
				SceneTextureImage* Normal;
				SceneTextureImage* Metallic;
				SceneTextureImage* Roughness;
				SceneTextureImage* Occlusion;
				if (Entities[i].UsedComponents[COMPONENT_TYPE_MATERIAL])
				{
					Albedo = (SceneTextureImage*)CMA_GetAt(&SceneTextures, Material->AlbedoIndex);
					Normal = (SceneTextureImage*)CMA_GetAt(&SceneTextures, Material->NormalIndex);
					Metallic = (SceneTextureImage*)CMA_GetAt(&SceneTextures, Material->MetallicIndex);
					Roughness = (SceneTextureImage*)CMA_GetAt(&SceneTextures, Material->RoughnessIndex);
					Occlusion = (SceneTextureImage*)CMA_GetAt(&SceneTextures, Material->OcclusionIndex);
				}
					
				else
				{
					Albedo = (SceneTextureImage*)CMA_GetAt(&SceneTextures, 0);
					Normal = (SceneTextureImage*)CMA_GetAt(&SceneTextures, 0);
					Metallic = (SceneTextureImage*)CMA_GetAt(&SceneTextures, 0);
					Roughness = (SceneTextureImage*)CMA_GetAt(&SceneTextures, 0);
					Occlusion = (SceneTextureImage*)CMA_GetAt(&SceneTextures, 0);
				}

				
					
				uint32_t AlbedoDescriptorSet = 0;
				uint32_t NormalDescriptorSet = 0;
				uint32_t MetallicDescriptorSet = 0;
				uint32_t RoughnessDescriptorSet = 0;
				uint32_t OcclusionDescriptorSet = 0;
				if (Albedo != NULL) AlbedoDescriptorSet = Albedo->TextureDescriptorSet;
				if (Normal != NULL) NormalDescriptorSet = Normal->TextureDescriptorSet;
				if (Metallic != NULL) MetallicDescriptorSet = Metallic->TextureDescriptorSet;
				if (Roughness != NULL) RoughnessDescriptorSet = Roughness->TextureDescriptorSet;
				if (Occlusion != NULL) OcclusionDescriptorSet = Occlusion->TextureDescriptorSet;

				if (Entities[i].UsedComponents[COMPONENT_TYPE_MATERIAL])
				{
					SceneFragmentPc.Color = Material->Color;
					SceneFragmentPc.Metallic = Material->Metallic;
					SceneFragmentPc.Roughness = Material->Roughness;
					SceneFragmentPc.Occlusion = Material->Occlusion;
				}					
				else
				{
					SceneFragmentPc.Color = Vec4f(1.0);
					SceneFragmentPc.Metallic = 0.0;
					SceneFragmentPc.Roughness = 1.0;
					SceneFragmentPc.Occlusion = 1.0;
				}
					

				OpenVkPushConstant(SceneLayout, OPENVK_SHADER_TYPE_VERTEX, 0, sizeof(SceneVertexPushConstant), &SceneVertexPc);
				OpenVkPushConstant(SceneLayout, OPENVK_SHADER_TYPE_FRAGMENT, 64, sizeof(SceneFragmentPushConstant), &SceneFragmentPc);
				
				if (Entities[i].UsedComponents[COMPONENT_TYPE_MESH])
				{
					SceneMesh* Mesh = (SceneMesh*)CMA_GetAt(&SceneMeshes, Entities[i].Mesh.MeshIndex);
					if (Mesh != NULL)
					{
						for (uint32_t m = 0; m < Mesh->MeshCount; m++)
						{
							if (!Entities[i].UsedComponents[COMPONENT_TYPE_MATERIAL])
							{
								SceneFragmentPc.Color = Mesh->MeshData[m].Color;
								//set to mesh
								SceneFragmentPc.Metallic = Mesh->MeshData[m].Metallic;
								SceneFragmentPc.Roughness = Mesh->MeshData[m].Roughness;
								SceneFragmentPc.Occlusion = Mesh->MeshData[m].Occlusion;
								OpenVkPushConstant(SceneLayout, OPENVK_SHADER_TYPE_FRAGMENT, 64, sizeof(SceneFragmentPushConstant), &SceneFragmentPc);

								Albedo = (SceneTextureImage*)CMA_GetAt(&SceneTextures, Mesh->MeshData[m].AlbedoIndex);
								Normal = (SceneTextureImage*)CMA_GetAt(&SceneTextures, Mesh->MeshData[m].NormalIndex);
								Metallic = (SceneTextureImage*)CMA_GetAt(&SceneTextures, Mesh->MeshData[m].MetallicIndex);
								Roughness = (SceneTextureImage*)CMA_GetAt(&SceneTextures, Mesh->MeshData[m].RoughnessIndex);
								Occlusion = (SceneTextureImage*)CMA_GetAt(&SceneTextures, Mesh->MeshData[m].OcclusionIndex);

								if (Albedo != NULL) AlbedoDescriptorSet = Albedo->TextureDescriptorSet;
								if (Normal != NULL) NormalDescriptorSet = Normal->TextureDescriptorSet;
								if (Metallic != NULL) MetallicDescriptorSet = Metallic->TextureDescriptorSet;
								if (Roughness != NULL) RoughnessDescriptorSet = Roughness->TextureDescriptorSet;
								if (Occlusion != NULL) OcclusionDescriptorSet = Occlusion->TextureDescriptorSet;
							}

							OpenVkBindDescriptorSet(SceneLayout, 0, AlbedoDescriptorSet, OPENVK_PIPELINE_TYPE_GRAPHICS);
							OpenVkBindDescriptorSet(SceneLayout, 1, NormalDescriptorSet, OPENVK_PIPELINE_TYPE_GRAPHICS);
							OpenVkBindDescriptorSet(SceneLayout, 2, MetallicDescriptorSet, OPENVK_PIPELINE_TYPE_GRAPHICS);
							OpenVkBindDescriptorSet(SceneLayout, 3, RoughnessDescriptorSet, OPENVK_PIPELINE_TYPE_GRAPHICS);
							OpenVkBindDescriptorSet(SceneLayout, 4, OcclusionDescriptorSet, OPENVK_PIPELINE_TYPE_GRAPHICS);

							if (Mesh->MeshData[m].Indices == NULL)
							{
								OpenVkBindVertexBuffer(Mesh->MeshData[m].VertexBuffer);
								OpenVkDrawVertices(Mesh->MeshData[m].VertexCount);
							}
							else
							{
								OpenVkBindIndexBuffer(Mesh->MeshData[m].VertexBuffer, Mesh->MeshData[m].IndexBuffer);
								OpenVkDrawIndices(Mesh->MeshData[m].IndexCount);
							}
						}
					}
				}
				else
				{
					if (Entities[i].Animation.AnimationIndex != 0)
					{
						SceneAnimation* Animation = (SceneAnimation*)CMA_GetAt(&SceneAnimations, Entities[i].Animation.AnimationIndex);
						if (Animation != NULL)
						{
							OpenVkBindDescriptorSet(SceneLayout, 0, AlbedoDescriptorSet, OPENVK_PIPELINE_TYPE_GRAPHICS);
							OpenVkBindDescriptorSet(SceneLayout, 1, NormalDescriptorSet, OPENVK_PIPELINE_TYPE_GRAPHICS);
							OpenVkBindDescriptorSet(SceneLayout, 2, MetallicDescriptorSet, OPENVK_PIPELINE_TYPE_GRAPHICS);
							OpenVkBindDescriptorSet(SceneLayout, 3, RoughnessDescriptorSet, OPENVK_PIPELINE_TYPE_GRAPHICS);
							OpenVkBindDescriptorSet(SceneLayout, 4, OcclusionDescriptorSet, OPENVK_PIPELINE_TYPE_GRAPHICS);

						//	UpdateAnimation(Entities[i].Animation.AnimationIndex);
							OpenVkBindDynamicVertexBuffer(Animation->VertexBuffer);
							OpenVkDrawVertices(Animation->MeshData.NumTriangles * 3);
						}
					}					
				}
			}						
		}
	}
	OpenVkEndRenderPass();
}