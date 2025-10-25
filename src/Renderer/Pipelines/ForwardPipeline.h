
void CreateForwardRenderPass()
{
	ForwardDepthAttachment = OpenVkCreateDepthImageAttachment(SceneWidth, SceneHeight, 1, true, OPENVK_FORMAT_DEFAULT);
   // ForwardColorAttachment = OpenVkCreateColorImageAttachment(SceneWidth, SceneHeight, 1, true, OPENVK_FORMAT_DEFAULT);

   	/*
    uint32_t Attachments[] = { OPENVK_ATTACHMENT_COLOR, OPENVK_ATTACHMENT_DEPTH };
	uint32_t AttachmentFormats[] = { OPENVK_FORMAT_DEFAULT, OPENVK_FORMAT_DEFAULT };
	uint32_t AttachmentDescriptions[] = { OPENVK_ATTACHMENT_DESCRIPTION_LOAD_CLEAR, OPENVK_ATTACHMENT_DESCRIPTION_LOAD_CLEAR };
	uint32_t MsaaSamples[] = { 1, 1 };
	ForwardRenderPass = OpenVkCreateRenderPass(ARRAY_SIZE(Attachments), Attachments, AttachmentFormats, AttachmentDescriptions, MsaaSamples, OPENVK_RENDER_PASS_SAMPLED);
	*/	

	VkAttachmentDescription Attachments[2];
	memset(Attachments, 0, ARRAY_SIZE(Attachments) * sizeof(VkAttachmentDescription));
	Attachments[0].format = VkGetOpenVkFormat(OPENVK_FORMAT_DEFAULT, NULL);
	Attachments[0].samples = VK_SAMPLE_COUNT_1_BIT;
	Attachments[0].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	Attachments[0].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	Attachments[0].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	Attachments[0].finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

	Attachments[1].format = VkFindDepthFormat();  // e.g., VK_FORMAT_D32_SFLOAT
	Attachments[1].samples = VK_SAMPLE_COUNT_1_BIT;
	Attachments[1].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	Attachments[1].storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	Attachments[1].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	Attachments[1].finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	// References
	VkAttachmentReference depthAttachmentRef = {1, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL};
	VkAttachmentReference colorAttachmentRef = {0, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL};

	VkSubpassDescription Subpasses[2];
	memset(Subpasses, 0, ARRAY_SIZE(Subpasses) * sizeof(VkSubpassDescription));
	
	Subpasses[0].pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	Subpasses[0].colorAttachmentCount = 0;  // no color output
	Subpasses[0].pDepthStencilAttachment = &depthAttachmentRef;


	Subpasses[1].pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	Subpasses[1].colorAttachmentCount = 1;
	Subpasses[1].pColorAttachments = &colorAttachmentRef;
	Subpasses[1].pDepthStencilAttachment = &depthAttachmentRef;

	


	VkSubpassDependency Dependencies[1];
	memset(Dependencies, 0, ARRAY_SIZE(Dependencies) * sizeof(VkSubpassDependency));

	Dependencies[0].srcSubpass = 0;
	Dependencies[0].dstSubpass = 1;
	Dependencies[0].srcStageMask = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
	Dependencies[0].dstStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
	Dependencies[0].srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
	Dependencies[0].dstAccessMask = VK_ACCESS_INPUT_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT;


	ForwardRenderPass = VkCreateRenderPassExt(ARRAY_SIZE(Attachments), Attachments, ARRAY_SIZE(Subpasses), Subpasses, ARRAY_SIZE(Dependencies), Dependencies);

}

void CreateForwardLayout()
{
    uint32_t DescriptorSetLayouts[] =
	{
		ForwardStaticDescriptorSetLayout,
		PBRDescriptorSetLayout
	};
	
	uint32_t PushTypes[] = { OPENVK_SHADER_TYPE_VERTEX, OPENVK_SHADER_TYPE_FRAGMENT };
	uint32_t PushOffsets[] = { 0, 64 };
	uint32_t PushSizes[] = { sizeof(ForwardVertexPushConstant), sizeof(ForwardFragmentPushConstant) };

	ForwardDescriptorSetLayoutCount = ARRAY_SIZE(DescriptorSetLayouts) < VkRenderer.PhysicalDeviceProperties.limits.maxBoundDescriptorSets ? ARRAY_SIZE(DescriptorSetLayouts) : VkRenderer.PhysicalDeviceProperties.limits.maxBoundDescriptorSets;
	
	OpenVkPipelineLayoutCreateInfo Layout;
	Layout.PushConstantCount = 2;
	Layout.PushConstantShaderTypes = PushTypes;
	Layout.PushConstantOffsets = PushOffsets;
	Layout.PushConstantSizes = PushSizes;
	Layout.DescriptorSetLayoutCount = ARRAY_SIZE(DescriptorSetLayouts);
	Layout.DescriptorSetLayouts = DescriptorSetLayouts;
	ForwardLayout = OpenVkCreatePipelineLayout(&Layout);

	Layout.PushConstantCount = 1;
	Layout.DescriptorSetLayoutCount = 1;
	Layout.DescriptorSetLayouts = &VertexUniformDescriptorSetLayout;
	ForwardDepthLayout = OpenVkCreatePipelineLayout(&Layout);
}

void CreateForwardPipeline()
{
    uint32_t ShaderAttributeFormats[] = { OPENVK_FORMAT_RGBA32F, OPENVK_FORMAT_RGBA32F };
	uint32_t ShaderAttributeOffsets[] = { 0, 16 };

	OpenVkFile VertexShader = OpenVkReadFile("Data/Shader/ForwardVertex.spv");
	OpenVkFile FragmentShader = OpenVkReadFile("Data/Shader/ForwardFragment.spv");
	VertexShader.Freeable = OpenVkFalse;
	FragmentShader.Freeable = OpenVkFalse;

	OpenVkFile VertexDepthShader = OpenVkReadFile("Data/Shader/DepthPrePassVertex.spv");
	OpenVkFile FragmentDepthShader = OpenVkReadFile("Data/Shader/DepthPrePassFragment.spv");
	VertexDepthShader.Freeable = OpenVkFalse;
	FragmentDepthShader.Freeable = OpenVkFalse;
	
	OpenVkGraphicsPipelineCreateInfo GraphicsPipelineCreateInfo;
	
	GraphicsPipelineCreateInfo.BindingStride = sizeof(SceneVertex);
	GraphicsPipelineCreateInfo.ShaderAttributeFormatCount = 2;
	GraphicsPipelineCreateInfo.ShaderAttributeFormats = ShaderAttributeFormats;
	GraphicsPipelineCreateInfo.ShaderAttributeOffsets = ShaderAttributeOffsets;
	GraphicsPipelineCreateInfo.PrimitiveTopology = OPENVK_PRIMITIVE_TOPOLOGY_TRIANGLE;
	GraphicsPipelineCreateInfo.x = 0;
	GraphicsPipelineCreateInfo.y = 0;
	GraphicsPipelineCreateInfo.Width = SceneWidth;
	GraphicsPipelineCreateInfo.Height = SceneHeight;
	GraphicsPipelineCreateInfo.DepthClamp = false;
	GraphicsPipelineCreateInfo.PolygonMode = OPENVK_POLYGON_MODE_FILL;
	GraphicsPipelineCreateInfo.LineWidth = 3.0;
	GraphicsPipelineCreateInfo.FrontFace = OPENVK_FRONT_FACE_COUNTER_CLOCK_WISE;
	GraphicsPipelineCreateInfo.MsaaSamples = 1;
	GraphicsPipelineCreateInfo.AlphaBlendings = NULL;
	GraphicsPipelineCreateInfo.DepthStencil = true;
	GraphicsPipelineCreateInfo.RenderPass = ForwardRenderPass;
	
	{
		GraphicsPipelineCreateInfo.ColorBlendAttachments = 0;
		GraphicsPipelineCreateInfo.PipelineLayout = ForwardDepthLayout;
		GraphicsPipelineCreateInfo.VertexShader = VertexDepthShader;
		GraphicsPipelineCreateInfo.FragmentShader = FragmentDepthShader;
		GraphicsPipelineCreateInfo.DepthWrite = OpenVkTrue;
		GraphicsPipelineCreateInfo.DepthCompare = OPENVK_DEPTH_COMPARE_LESS;
		GraphicsPipelineCreateInfo.Subpass = 0;

		GraphicsPipelineCreateInfo.CullMode = OPENVK_CULL_MODE_NONE;
		ForwardDepthPipelineNoCulling = OpenVkCreateGraphicsPipeline(&GraphicsPipelineCreateInfo);

		GraphicsPipelineCreateInfo.CullMode = OPENVK_CULL_MODE_BACK;
		ForwardDepthPipelineBackCulling= OpenVkCreateGraphicsPipeline(&GraphicsPipelineCreateInfo);

		GraphicsPipelineCreateInfo.CullMode = OPENVK_CULL_MODE_FRONT;
		ForwardDepthPipelineFrontCulling = OpenVkCreateGraphicsPipeline(&GraphicsPipelineCreateInfo);		
	}

	{
		GraphicsPipelineCreateInfo.ColorBlendAttachments = 1;
		GraphicsPipelineCreateInfo.PipelineLayout = ForwardLayout;
		GraphicsPipelineCreateInfo.VertexShader = VertexShader;
		GraphicsPipelineCreateInfo.FragmentShader = FragmentShader;
		GraphicsPipelineCreateInfo.DepthWrite = OpenVkFalse;
		GraphicsPipelineCreateInfo.DepthCompare = OPENVK_DEPTH_COMPARE_EQUAL;
		GraphicsPipelineCreateInfo.Subpass = 1;
		
		GraphicsPipelineCreateInfo.CullMode = OPENVK_CULL_MODE_NONE;
		ForwardPipelineNoCulling = OpenVkCreateGraphicsPipeline(&GraphicsPipelineCreateInfo);

		GraphicsPipelineCreateInfo.CullMode = OPENVK_CULL_MODE_BACK;
		ForwardPipelineBackCulling= OpenVkCreateGraphicsPipeline(&GraphicsPipelineCreateInfo);

		GraphicsPipelineCreateInfo.CullMode = OPENVK_CULL_MODE_FRONT;
		ForwardPipelineFrontCulling = OpenVkCreateGraphicsPipeline(&GraphicsPipelineCreateInfo);
	}	

	free(FragmentShader.Data);
	free(VertexShader.Data);

	free(FragmentDepthShader.Data);
	free(VertexDepthShader.Data);
}

void CreateForwardFramebuffer()
{
	uint32_t Attachments[] = { SceneAttachment, ForwardDepthAttachment };//ForwardColorAttachment

	OpenVkFramebufferCreateInfo FramebufferCreateInfo;
	FramebufferCreateInfo.AttachmentCount = ARRAY_SIZE(Attachments);
	FramebufferCreateInfo.Attachments = Attachments;
	FramebufferCreateInfo.RenderPass = ForwardRenderPass;
	FramebufferCreateInfo.Width = SceneWidth;
	FramebufferCreateInfo.Height = SceneHeight;
	ForwardFramebuffer = OpenVkCreateFramebuffer(&FramebufferCreateInfo);
}

void CreateForwardUniformBuffer()
{
//	ForwardVertexUniformBuffer = OpenVkCreateUniformBuffer(sizeof(ForwardVertexUniformBufferObject));
}

void CreateForwardDescriptorSet()
{
	{
		uint32_t DescriptorCounts[] = { 1, 1, 1, SHADOW_MAP_CASCADE_COUNT };
		uint32_t DescriptorTypes[] = { OPENVK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, OPENVK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, OPENVK_DESCRIPTOR_TYPE_STORAGE_BUFFER, OPENVK_DESCRIPTOR_TYPE_IMAGE_SAMPLER };
		uint32_t UniformBuffers[] = { GBufferVertexUniformBuffer, SceneFragmentUniformBuffer, SceneFragmentStorageBuffer };
		size_t UniformSizes[] = { sizeof(GBufferVertexUniformBufferObject), sizeof(SceneFragmentUniformBufferObject), sizeof(SceneFragmentStorageBufferObject) };
		uint32_t Bindings[] = { 0, 1, 2, 3 };

		uint32_t ImageTypes[SHADOW_MAP_CASCADE_COUNT];
		uint32_t ImageLayouts[SHADOW_MAP_CASCADE_COUNT];
		uint32_t Images[SHADOW_MAP_CASCADE_COUNT];
		uint32_t Sampler[SHADOW_MAP_CASCADE_COUNT];
		
		for (uint32_t i = 0; i < SHADOW_MAP_CASCADE_COUNT; i++)
		{
			ImageTypes[i] = OPENVK_IMAGE_TYPE_ATTACHMENT;
			ImageLayouts[i] = OPENVK_IMAGE_LAYOUT_DEPTH_OUTPUT;
			Images[i] = ShadowDepthAttachments[i];
			Sampler[i] = ShadowSampler;
		}

		OpenVkDescriptorSetCreateInfo DescriptorSetCreateInfo;
		DescriptorSetCreateInfo.DescriptorSetLayout = ForwardStaticDescriptorSetLayout;
		DescriptorSetCreateInfo.DescriptorPool = DescriptorPool;
		DescriptorSetCreateInfo.DescriptorWriteCount = ARRAY_SIZE(DescriptorCounts);
		DescriptorSetCreateInfo.DescriptorCounts = DescriptorCounts;
		DescriptorSetCreateInfo.DescriptorTypes = DescriptorTypes;
		DescriptorSetCreateInfo.Buffers = UniformBuffers;
		DescriptorSetCreateInfo.BufferSizes = UniformSizes;
		DescriptorSetCreateInfo.Sampler = Sampler;
		DescriptorSetCreateInfo.ImageTypes = ImageTypes;
		DescriptorSetCreateInfo.Images = Images;
		DescriptorSetCreateInfo.ImageLayouts = ImageLayouts;
		DescriptorSetCreateInfo.Bindings = Bindings;
		DescriptorSetCreateInfo.DescriptorSet = NULL;
		DescriptorSetCreateInfo.VariableDescriptorSetCount = 0;

		ForwardStaticDescriptorSet = OpenVkCreateDescriptorSet(&DescriptorSetCreateInfo);
	}
    
	
}

void ForwardUpdateUniformBuffer()
{
	/*
	//	Mutex.lock();
	LoadMat4IdentityP(&ForwardVertexUBO.View);
	if (GameMode)
	{
		for (uint32_t i = 0; i < EntityCount; i++)
		{
			if (Entities[i].UsedComponents[COMPONENT_TYPE_CAMERA])
			{
				SceneFragmentUBO.CameraPosition.x = Entities[i].Translate.x;
				SceneFragmentUBO.CameraPosition.y = Entities[i].Translate.y;
				SceneFragmentUBO.CameraPosition.z = Entities[i].Translate.z;
				SceneFragmentUBO.CameraPosition.w = 0.0;

				LoadMat4IdentityP(&ForwardVertexUBO.View);
				ForwardVertexUBO.View = RotateXMat4P(&ForwardVertexUBO.View, ToRadians(Entities[i].Rotate.x));
				ForwardVertexUBO.View = RotateYMat4P(&ForwardVertexUBO.View, ToRadians(Entities[i].Rotate.y));
				ForwardVertexUBO.View = RotateZMat4P(&ForwardVertexUBO.View, ToRadians(Entities[i].Rotate.z));
				ForwardVertexUBO.View = TranslateMat4P(&ForwardVertexUBO.View, &Entities[i].Translate);
				ForwardVertexUBO.View = InverseMat4(ForwardVertexUBO.View);
				PerspectiveMat4P(((float)SceneWidth / SceneHeight), ToRadians(Entities[i].Camera.FOV), Entities[i].Camera.NearPlane, Entities[i].Camera.FarPlane, &ForwardVertexUBO.Projection);
				NearPlane = Entities[i].Camera.NearPlane;
				FarPlane = Entities[i].Camera.FarPlane;
			}
		}
	}
	else
	{
		SceneFragmentUBO.CameraPosition.x = CameraPos.x;
		SceneFragmentUBO.CameraPosition.y = CameraPos.y;
		SceneFragmentUBO.CameraPosition.z = CameraPos.z;
		SceneFragmentUBO.CameraPosition.w = 0.0;

		vec3 t = Add3P(&CameraPos, &CameraDir);
		ForwardVertexUBO.View = LookAtMat4P(&CameraPos, &t, &CameraUp);
		PerspectiveMat4P(((float)SceneWidth / SceneHeight), ToRadians(CameraFOV), CameraNearZ, CameraFarZ, &ForwardVertexUBO.Projection);
		NearPlane = CameraNearZ;
		FarPlane = CameraFarZ;
	}

	OpenVkUpdateBuffer(sizeof(ForwardVertexUniformBufferObject), &ForwardVertexUBO, ForwardVertexUniformBuffer);
	*/
}

void ForwardDrawDepth()
{
	uint32_t Pipeline = ForwardDepthPipelineNoCulling;
	switch (GBufferCullMode)
	{
	case CULL_MODE_BACK:
		Pipeline = ForwardDepthPipelineBackCulling;
		break;
	case CULL_MODE_FRONT:
		Pipeline = ForwardDepthPipelineFrontCulling;
		break;
	default:
		break;
	}

	OpenVkBindPipeline(Pipeline, OPENVK_PIPELINE_TYPE_GRAPHICS);
	
	OpenVkBindDescriptorSet(ForwardDepthLayout, 0, GBufferVertexUniformDescriptorSet, OPENVK_PIPELINE_TYPE_GRAPHICS);

	for (uint32_t i = 0; i < EntityCount; i++)
	{
		if (Entities[i].UsedComponents[COMPONENT_TYPE_MESH] ||
			Entities[i].UsedComponents[COMPONENT_TYPE_ANIMATION])
		{
			LoadMat4IdentityP(&ForwardVertexPc.Model);
			ForwardVertexPc.Model = ScaleMat4P(&ForwardVertexPc.Model, &Entities[i].Scale);
			ForwardVertexPc.Model = RotateXMat4P(&ForwardVertexPc.Model, ToRadians(Entities[i].Rotate.x));
			ForwardVertexPc.Model = RotateYMat4P(&ForwardVertexPc.Model, ToRadians(Entities[i].Rotate.y));
			ForwardVertexPc.Model = RotateZMat4P(&ForwardVertexPc.Model, ToRadians(Entities[i].Rotate.z));
			ForwardVertexPc.Model = TranslateMat4P(&ForwardVertexPc.Model, &Entities[i].Translate);

			
			if (Entities[i].UsedComponents[COMPONENT_TYPE_MESH])
			{
				SceneMesh* Mesh = (SceneMesh*)CMA_GetAt(&SceneMeshes, Entities[i].Mesh.MeshIndex);
				if (Mesh != NULL && Mesh->MeshCount > 0)
				{
					OpenVkPushConstant(ForwardDepthLayout, OPENVK_SHADER_TYPE_VERTEX, 0, sizeof(ForwardVertexPushConstant), &ForwardVertexPc);
					
					if (Mesh->IndexBuffer != OPENVK_ERROR)
						OpenVkBindIndexBuffer(Mesh->VertexBuffer, Mesh->IndexBuffer);
					else
						OpenVkBindVertexBuffer(Mesh->VertexBuffer);

				//	RaytracingAddEntityMesh(Entities[i].Mesh.MeshIndex, Mesh);

					for (uint32_t m = 0; m < Mesh->MeshCount; m++)
					{
						if (Mesh->MeshData[m].Render[RENDER_TYPE_DEFAULT])
						{
							
							if (Mesh->IndexBuffer != OPENVK_ERROR)
								OpenVkDrawIndices(Mesh->MeshData[m].IndexOffset, Mesh->MeshData[m].IndexCount, 0);//Mesh->MeshData[m].VertexOffset
							else
								OpenVkDrawVertices(Mesh->MeshData[m].VertexOffset, Mesh->MeshData[m].VertexCount);
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
						OpenVkPushConstant(ForwardLayout, OPENVK_SHADER_TYPE_VERTEX, 0, sizeof(ForwardVertexPushConstant), &ForwardVertexPc);

						printf("WARNING, Forward pipelines animation system is not updated!\n");
/*
						if (LastAlbedoDescriptorSet != AlbedoDescriptorSet && ForwardDescriptorSetLayoutCount > 1)
							OpenVkBindDescriptorSet(ForwardLayout, 1, AlbedoDescriptorSet, OPENVK_PIPELINE_TYPE_GRAPHICS);
						if (LastNormalDescriptorSet != NormalDescriptorSet && ForwardDescriptorSetLayoutCount > 2)
							OpenVkBindDescriptorSet(ForwardLayout, 2, NormalDescriptorSet, OPENVK_PIPELINE_TYPE_GRAPHICS);
						if (LastMetallicDescriptorSet != MetallicDescriptorSet && ForwardDescriptorSetLayoutCount > 3)
							OpenVkBindDescriptorSet(ForwardLayout, 3, MetallicDescriptorSet, OPENVK_PIPELINE_TYPE_GRAPHICS);
						if (LastRoughnessDescriptorSet != RoughnessDescriptorSet && ForwardDescriptorSetLayoutCount > 4)
							OpenVkBindDescriptorSet(ForwardLayout, 4, RoughnessDescriptorSet, OPENVK_PIPELINE_TYPE_GRAPHICS);
						if (LastOcclusionDescriptorSet != OcclusionDescriptorSet && ForwardDescriptorSetLayoutCount > 5)
							OpenVkBindDescriptorSet(ForwardLayout, 5, OcclusionDescriptorSet, OPENVK_PIPELINE_TYPE_GRAPHICS);

						LastAlbedoDescriptorSet = AlbedoDescriptorSet;
						LastNormalDescriptorSet = NormalDescriptorSet;
						LastMetallicDescriptorSet = MetallicDescriptorSet;
						LastRoughnessDescriptorSet = RoughnessDescriptorSet;
						LastOcclusionDescriptorSet = OcclusionDescriptorSet;
*/
					
						OpenVkBindDynamicVertexBuffer(Animation->VertexBuffer);
						OpenVkDrawVertices(0, Animation->MeshData.NumTriangles * 3);
					}
				}
			}
		}
	}
}

void ForwardDrawColor()
{
	uint32_t Pipeline = ForwardPipelineNoCulling;
	switch (GBufferCullMode)
	{
	case CULL_MODE_BACK:
		Pipeline = ForwardPipelineBackCulling;
		break;
	case CULL_MODE_FRONT:
		Pipeline = ForwardPipelineFrontCulling;
		break;
	default:
		break;
	}

	

	OpenVkBindPipeline(Pipeline, OPENVK_PIPELINE_TYPE_GRAPHICS);
	
	OpenVkBindDescriptorSet(ForwardLayout, 0, ForwardStaticDescriptorSet, OPENVK_PIPELINE_TYPE_GRAPHICS);

	for (uint32_t i = 0; i < EntityCount; i++)
	{
		if (Entities[i].UsedComponents[COMPONENT_TYPE_MESH] ||
			Entities[i].UsedComponents[COMPONENT_TYPE_ANIMATION])
		{
			LoadMat4IdentityP(&ForwardVertexPc.Model);
			ForwardVertexPc.Model = ScaleMat4P(&ForwardVertexPc.Model, &Entities[i].Scale);
			ForwardVertexPc.Model = RotateXMat4P(&ForwardVertexPc.Model, ToRadians(Entities[i].Rotate.x));
			ForwardVertexPc.Model = RotateYMat4P(&ForwardVertexPc.Model, ToRadians(Entities[i].Rotate.y));
			ForwardVertexPc.Model = RotateZMat4P(&ForwardVertexPc.Model, ToRadians(Entities[i].Rotate.z));
			ForwardVertexPc.Model = TranslateMat4P(&ForwardVertexPc.Model, &Entities[i].Translate);

			SceneMaterial* Material = (SceneMaterial*)CMA_GetAt(&SceneMaterials, Entities[i].Material.MaterialIndex);
			if (Material == NULL)
				Material = (SceneMaterial*)CMA_GetAt(&SceneMaterials, 0);

			if (Entities[i].UsedComponents[COMPONENT_TYPE_MATERIAL])
			{
				ForwardFragmentPc.Color = Material->Color;
				ForwardFragmentPc.Metallic = Material->Metallic;
				ForwardFragmentPc.Roughness = Material->Roughness;
				ForwardFragmentPc.Occlusion = Material->Occlusion;
				ForwardFragmentPc.NearPlane = NearPlane;
				ForwardFragmentPc.FarPlane = FarPlane;
			}
			else
			{
				ForwardFragmentPc.Color = MaterialColor;
				ForwardFragmentPc.Metallic = MaterialMetallic;
				ForwardFragmentPc.Roughness = MaterialRoughness;
				ForwardFragmentPc.Occlusion = MaterialOcclusion;
				ForwardFragmentPc.NearPlane = NearPlane;
				ForwardFragmentPc.FarPlane = FarPlane;
			}

			if (Entities[i].UsedComponents[COMPONENT_TYPE_MESH])
			{
				SceneMesh* Mesh = (SceneMesh*)CMA_GetAt(&SceneMeshes, Entities[i].Mesh.MeshIndex);
				if (Mesh != NULL && Mesh->MeshCount > 0)
				{
					OpenVkPushConstant(ForwardLayout, OPENVK_SHADER_TYPE_VERTEX, 0, sizeof(ForwardVertexPushConstant), &ForwardVertexPc);
					OpenVkPushConstant(ForwardLayout, OPENVK_SHADER_TYPE_FRAGMENT, 64, sizeof(ForwardFragmentPushConstant), &ForwardFragmentPc);

					if (Mesh->IndexBuffer != OPENVK_ERROR)
						OpenVkBindIndexBuffer(Mesh->VertexBuffer, Mesh->IndexBuffer);
					else
						OpenVkBindVertexBuffer(Mesh->VertexBuffer);

				//	RaytracingAddEntityMesh(Entities[i].Mesh.MeshIndex, Mesh);

					for (uint32_t m = 0; m < Mesh->MeshCount; m++)
					{
						if (Mesh->MeshData[m].Render[RENDER_TYPE_DEFAULT])
						{
							if (!Entities[i].UsedComponents[COMPONENT_TYPE_MATERIAL])
							{
								SceneMaterial* TmpMaterial = (SceneMaterial*)CMA_GetAt(&SceneMaterials, Mesh->MeshData[m].MaterialIndex);
								if (TmpMaterial)
								{
									Material = TmpMaterial;
									ForwardFragmentPc.Color =		Material->Color;
									ForwardFragmentPc.Metallic =	Material->Metallic;
									ForwardFragmentPc.Roughness =	Material->Roughness;
									ForwardFragmentPc.Occlusion =	Material->Occlusion;

								}
								else
								{
									ForwardFragmentPc.Color = MaterialColor;
									ForwardFragmentPc.Metallic = MaterialMetallic;
									ForwardFragmentPc.Roughness = MaterialRoughness;
									ForwardFragmentPc.Occlusion = MaterialOcclusion;
								}
								ForwardFragmentPc.NearPlane = NearPlane;
								ForwardFragmentPc.FarPlane = FarPlane;
								OpenVkPushConstant(ForwardLayout, OPENVK_SHADER_TYPE_FRAGMENT, 64, sizeof(ForwardFragmentPushConstant), &ForwardFragmentPc);
							}



							
							OpenVkBindDescriptorSet(ForwardLayout, 1, Material->DescriptorSet, OPENVK_PIPELINE_TYPE_GRAPHICS);
							
							if (Mesh->IndexBuffer != OPENVK_ERROR)
								OpenVkDrawIndices(Mesh->MeshData[m].IndexOffset, Mesh->MeshData[m].IndexCount, 0);//Mesh->MeshData[m].VertexOffset
							else
								OpenVkDrawVertices(Mesh->MeshData[m].VertexOffset, Mesh->MeshData[m].VertexCount);
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
						OpenVkPushConstant(ForwardLayout, OPENVK_SHADER_TYPE_VERTEX, 0, sizeof(ForwardVertexPushConstant), &ForwardVertexPc);
						OpenVkPushConstant(ForwardLayout, OPENVK_SHADER_TYPE_FRAGMENT, 64, sizeof(ForwardFragmentPushConstant), &ForwardFragmentPc);

						printf("WARNING, Forward pipelines animation system is not updated!\n");
/*
						if (LastAlbedoDescriptorSet != AlbedoDescriptorSet && ForwardDescriptorSetLayoutCount > 1)
							OpenVkBindDescriptorSet(ForwardLayout, 1, AlbedoDescriptorSet, OPENVK_PIPELINE_TYPE_GRAPHICS);
						if (LastNormalDescriptorSet != NormalDescriptorSet && ForwardDescriptorSetLayoutCount > 2)
							OpenVkBindDescriptorSet(ForwardLayout, 2, NormalDescriptorSet, OPENVK_PIPELINE_TYPE_GRAPHICS);
						if (LastMetallicDescriptorSet != MetallicDescriptorSet && ForwardDescriptorSetLayoutCount > 3)
							OpenVkBindDescriptorSet(ForwardLayout, 3, MetallicDescriptorSet, OPENVK_PIPELINE_TYPE_GRAPHICS);
						if (LastRoughnessDescriptorSet != RoughnessDescriptorSet && ForwardDescriptorSetLayoutCount > 4)
							OpenVkBindDescriptorSet(ForwardLayout, 4, RoughnessDescriptorSet, OPENVK_PIPELINE_TYPE_GRAPHICS);
						if (LastOcclusionDescriptorSet != OcclusionDescriptorSet && ForwardDescriptorSetLayoutCount > 5)
							OpenVkBindDescriptorSet(ForwardLayout, 5, OcclusionDescriptorSet, OPENVK_PIPELINE_TYPE_GRAPHICS);

						LastAlbedoDescriptorSet = AlbedoDescriptorSet;
						LastNormalDescriptorSet = NormalDescriptorSet;
						LastMetallicDescriptorSet = MetallicDescriptorSet;
						LastRoughnessDescriptorSet = RoughnessDescriptorSet;
						LastOcclusionDescriptorSet = OcclusionDescriptorSet;
*/
						OpenVkBindDescriptorSet(ForwardLayout, 1, Material->DescriptorSet, OPENVK_PIPELINE_TYPE_GRAPHICS);
						OpenVkBindDynamicVertexBuffer(Animation->VertexBuffer);
						OpenVkDrawVertices(0, Animation->MeshData.NumTriangles * 3);
					}
				}
			}
		}
	}
}

void ForwardDraw()
{
	OpenVkBeginRenderPassInfo BeginInfo;
	BeginInfo.ClearColor[0] = ClearColor.x;
	BeginInfo.ClearColor[1] = ClearColor.y;
	BeginInfo.ClearColor[2] = ClearColor.z;
	BeginInfo.ClearColor[3] = 1.0;
	BeginInfo.ClearColors = 1;
	BeginInfo.ClearDepth = true;
	BeginInfo.RenderPass = ForwardRenderPass;
	BeginInfo.Framebuffer = ForwardFramebuffer;
	BeginInfo.x = 0;
	BeginInfo.y = 0;
	BeginInfo.Width = SceneWidth;
	BeginInfo.Height = SceneHeight;

	OpenVkBeginRenderPass(&BeginInfo);
	{
		OpenVkSetScissor(0, 0, SceneWidth, SceneHeight);
		OpenVkSetViewport(0, 0, SceneWidth, SceneHeight);

	//	if (!GameMode && !RenderGamePreview && RenderDebug)
	//		DebugDraw();

		ForwardDrawDepth();
		OpenVkNextSubpass();
		ForwardDrawColor();

		
	}
	OpenVkEndRenderPass();
}