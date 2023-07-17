void CreateGBufferRenderPass()
{
	uint32_t Format = OPENVK_FORMAT_RGBA32F;
	GBufferAttachments[0] = OpenVkCreateColorImageAttachment(SceneWidth, SceneHeight, 1, true, Format);
	GBufferAttachments[1] = OpenVkCreateColorImageAttachment(SceneWidth, SceneHeight, 1, true, Format);
	GBufferAttachments[2] = OpenVkCreateColorImageAttachment(SceneWidth, SceneHeight, 1, true, OPENVK_FORMAT_RGBA);
	GBufferAttachments[3] = OpenVkCreateColorImageAttachment(SceneWidth, SceneHeight, 1, true, OPENVK_FORMAT_RGBA);
	GBufferAttachments[4] = OpenVkCreateColorImageAttachment(SceneWidth, SceneHeight, 1, true, Format);
	GBufferAttachments[5] = OpenVkCreateColorImageAttachment(SceneWidth, SceneHeight, 1, true, OPENVK_FORMAT_RG16F);
	GBufferAttachments[6] = OpenVkCreateDepthImageAttachment(SceneWidth, SceneHeight, 1, true, OPENVK_FORMAT_DEFAULT);

	uint32_t Attachments[] = { OPENVK_ATTACHMENT_COLOR, OPENVK_ATTACHMENT_COLOR, OPENVK_ATTACHMENT_COLOR, OPENVK_ATTACHMENT_COLOR, OPENVK_ATTACHMENT_COLOR, OPENVK_ATTACHMENT_COLOR, OPENVK_ATTACHMENT_DEPTH };
	uint32_t AttachmentFormats[] = { Format, Format, OPENVK_FORMAT_RGBA, OPENVK_FORMAT_RGBA, Format, OPENVK_FORMAT_RG16F, OPENVK_FORMAT_DEFAULT };
	uint32_t MsaaSamples[] = { 1, 1, 1, 1, 1, 1, 1 };
	GBufferRenderPass = OpenVkCreateRenderPass(G_BUFFER_ATTACHMENT_COUNT, Attachments, AttachmentFormats, MsaaSamples, OPENVK_RENDER_PASS_SAMPLED);
}

void CreateGBufferLayout()
{
	uint32_t DescriptorSetLayouts[] =
	{
		TextureDescriptorSetLayout,
		TextureDescriptorSetLayout,
		TextureDescriptorSetLayout,
		TextureDescriptorSetLayout,
		TextureDescriptorSetLayout,
		VertexUniformDescriptorSetLayout
	};
	
	uint32_t PushTypes[] = { OPENVK_SHADER_TYPE_VERTEX, OPENVK_SHADER_TYPE_FRAGMENT };
	uint32_t PushOffsets[] = { 0, 64 };
	uint32_t PushSizes[] = { sizeof(GBufferVertexPushConstant), sizeof(GBufferFragmentPushConstant) };

	OpenVkPipelineLayoutCreateInfo Layout;
	Layout.PushConstantCount = 2;
	Layout.PushConstantShaderTypes = PushTypes;
	Layout.PushConstantOffsets = PushOffsets;
	Layout.PushConstantSizes = PushSizes;
	Layout.DescriptorSetLayoutCount = ARRAY_SIZE(DescriptorSetLayouts);
	Layout.DescriptorSetLayouts = DescriptorSetLayouts;
	GBufferLayout = OpenVkCreatePipelineLayout(&Layout);
}

void CreateGBufferPipeline()
{
	uint32_t ShaderAttributeFormats[] = { OPENVK_FORMAT_RGB32F, OPENVK_FORMAT_RG32F, OPENVK_FORMAT_RGB32F };
	uint32_t ShaderAttributeOffsets[] = { 0, 12, 20 };

	OpenVkFile VertexShader = OpenVkReadFile("Data/Shader/GBufferVertex.spv");
	OpenVkFile FragmentShader = OpenVkReadFile("Data/Shader/GBufferFragment.spv");
	VertexShader.Freeable = OpenVkFalse;
	FragmentShader.Freeable = OpenVkFalse;

	OpenVkBool AlphaBlendings[] = { OpenVkFalse, OpenVkFalse, OpenVkFalse, OpenVkFalse, OpenVkFalse, OpenVkFalse };
	OpenVkGraphicsPipelineCreateInfo GraphicsPipelineCreateInfo;
	GraphicsPipelineCreateInfo.VertexShader = VertexShader;
	GraphicsPipelineCreateInfo.FragmentShader = FragmentShader;
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
	GraphicsPipelineCreateInfo.PolygonMode = OPENVK_POLYGON_MODE_FILL;
	GraphicsPipelineCreateInfo.LineWidth = 3.0;
	GraphicsPipelineCreateInfo.CullMode = OPENVK_CULL_MODE_NONE;
	GraphicsPipelineCreateInfo.FrontFace = OPENVK_FRONT_FACE_COUNTER_CLOCK_WISE;
	GraphicsPipelineCreateInfo.MsaaSamples = 1;
	GraphicsPipelineCreateInfo.AlphaBlendings = AlphaBlendings;
	GraphicsPipelineCreateInfo.ColorBlendAttachments = G_BUFFER_ATTACHMENT_COUNT - 1;
	GraphicsPipelineCreateInfo.PipelineLayout = GBufferLayout;
	GraphicsPipelineCreateInfo.DepthStencil = true;
	GraphicsPipelineCreateInfo.RenderPass = GBufferRenderPass;
	GBufferPipelineNoneCull = OpenVkCreateGraphicsPipeline(&GraphicsPipelineCreateInfo);
	
	GraphicsPipelineCreateInfo.VertexShader = VertexShader;
	GraphicsPipelineCreateInfo.FragmentShader = FragmentShader;
	GraphicsPipelineCreateInfo.CullMode = OPENVK_CULL_MODE_BACK;
	GBufferPipelineBackCull = OpenVkCreateGraphicsPipeline(&GraphicsPipelineCreateInfo);

	GraphicsPipelineCreateInfo.VertexShader = VertexShader;
	GraphicsPipelineCreateInfo.FragmentShader = FragmentShader;
	GraphicsPipelineCreateInfo.CullMode = OPENVK_CULL_MODE_FRONT;
	GBufferPipelineFrontCull = OpenVkCreateGraphicsPipeline(&GraphicsPipelineCreateInfo);

	free(FragmentShader.Data);
	free(VertexShader.Data);
}

void CreateGBufferFramebuffer()
{
	OpenVkFramebufferCreateInfo FramebufferCreateInfo;
	FramebufferCreateInfo.AttachmentCount = ARRAY_SIZE(GBufferAttachments);
	FramebufferCreateInfo.Attachments = GBufferAttachments;
	FramebufferCreateInfo.RenderPass = GBufferRenderPass;
	FramebufferCreateInfo.Width = SceneWidth;
	FramebufferCreateInfo.Height = SceneHeight;
	GBufferFramebuffer = OpenVkCreateFramebuffer(&FramebufferCreateInfo);
}

void CreateGBufferUniformBuffer()
{
	GBufferVertexUniformBuffer = OpenVkCreateUniformBuffer(sizeof(GBufferVertexUniformBufferObject));
}

void CreateGBufferDescriptorSet()
{
	{
		uint32_t DescriptorCounts[] = { 1 };
		uint32_t DescriptorTypes[] = { OPENVK_DESCRIPTOR_TYPE_UNIFORM_BUFFER };
		uint32_t UniformBuffers[] = { GBufferVertexUniformBuffer };
		size_t UniformSizes[] = { sizeof(GBufferVertexUniformBufferObject) };
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
		DescriptorSetCreateInfo.VariableDescriptorSetCount = 0;

		GBufferVertexUniformDescriptorSet = OpenVkCreateDescriptorSet(&DescriptorSetCreateInfo);
	}

//	{
//		uint32_t DescriptorCounts[] = { 1, 1, 1, 1 };
//		uint32_t DescriptorTypes[] = { OPENVK_DESCRIPTOR_TYPE_IMAGE_SAMPLER, OPENVK_DESCRIPTOR_TYPE_IMAGE_SAMPLER, OPENVK_DESCRIPTOR_TYPE_IMAGE_SAMPLER, OPENVK_DESCRIPTOR_TYPE_IMAGE_SAMPLER };
//		uint32_t ImageTypes[] = { OPENVK_IMAGE_TYPE_ATTACHMENT, OPENVK_IMAGE_TYPE_ATTACHMENT, OPENVK_IMAGE_TYPE_ATTACHMENT, OPENVK_IMAGE_TYPE_ATTACHMENT };
//		uint32_t ImageLayouts[] = { OPENVK_IMAGE_LAYOUT_COLOR_OUTPUT, OPENVK_IMAGE_LAYOUT_COLOR_OUTPUT, OPENVK_IMAGE_LAYOUT_COLOR_OUTPUT, OPENVK_IMAGE_LAYOUT_COLOR_OUTPUT };
//		uint32_t Bindings[] = { 0, 1, 2, 3 };
//		uint32_t Sampler[] = { ImageSampler, ImageSampler, ImageSampler, ImageSampler };
//
//		OpenVkDescriptorSetCreateInfo DescriptorSetCreateInfo;
//		DescriptorSetCreateInfo.DescriptorSetLayout = GBufferDescriptorSetLayout;
//		DescriptorSetCreateInfo.DescriptorPool = DescriptorPool;
//		DescriptorSetCreateInfo.DescriptorWriteCount = ARRAY_SIZE(GBufferAttachments);
//		DescriptorSetCreateInfo.DescriptorCounts = DescriptorCounts;
//		DescriptorSetCreateInfo.DescriptorTypes = DescriptorTypes;
//		DescriptorSetCreateInfo.Sampler = Sampler;
//		DescriptorSetCreateInfo.ImageTypes = ImageTypes;
//		DescriptorSetCreateInfo.Images = GBufferAttachments;
//		DescriptorSetCreateInfo.ImageLayouts = ImageLayouts;
//		DescriptorSetCreateInfo.Bindings = Bindings;
//		DescriptorSetCreateInfo.DescriptorSet = NULL;
//		DescriptorSetCreateInfo.VariableDescriptorSetCount = 0;
//
//		GBufferDescriptorSet = OpenVkCreateDescriptorSet(&DescriptorSetCreateInfo);
//	}
}

float NearPlane;
float FarPlane;

void GBufferUpdateUniformBuffer()
{
	//	Mutex.lock();
	LoadMat4IdentityP(&GBufferVertexUBO.View);
	if (GameMode)
	{
		for (uint32_t i = 0; i < EntityCount; i++)
		{
			if (Entities[i].UsedComponents[COMPONENT_TYPE_CAMERA])
			{
				LoadMat4IdentityP(&GBufferVertexUBO.View);
				GBufferVertexUBO.View = RotateXMat4P(&GBufferVertexUBO.View, ToRadians(Entities[i].Rotate.x));
				GBufferVertexUBO.View = RotateYMat4P(&GBufferVertexUBO.View, ToRadians(Entities[i].Rotate.y));
				GBufferVertexUBO.View = RotateZMat4P(&GBufferVertexUBO.View, ToRadians(Entities[i].Rotate.z));
				GBufferVertexUBO.View = TranslateMat4P(&GBufferVertexUBO.View, &Entities[i].Translate);
				GBufferVertexUBO.View = InverseMat4(GBufferVertexUBO.View);
				PerspectiveMat4P(((float)SceneWidth / SceneHeight), ToRadians(Entities[i].Camera.FOV), Entities[i].Camera.NearPlane, Entities[i].Camera.FarPlane, &GBufferVertexUBO.Projection);
				NearPlane = Entities[i].Camera.NearPlane;
				FarPlane = Entities[i].Camera.FarPlane;
			}
		}
	}
	else
	{
		vec3 t = Add3P(&CameraPos, &CameraDir);
		GBufferVertexUBO.View = LookAtMat4P(&CameraPos, &t, &CameraUp);
		PerspectiveMat4P(((float)SceneWidth / SceneHeight), ToRadians(CameraFOV), CameraNearZ, CameraFarZ, &GBufferVertexUBO.Projection);
		NearPlane = CameraNearZ;
		FarPlane = CameraFarZ;
	}

	OpenVkUpdateBuffer(sizeof(GBufferVertexUniformBufferObject), &GBufferVertexUBO, GBufferVertexUniformBuffer);
}


void GBufferDraw()
{
	OpenVkBeginRenderPassInfo BeginInfo;
	BeginInfo.ClearColor[0] = ClearColor.x;
	BeginInfo.ClearColor[1] = ClearColor.y;
	BeginInfo.ClearColor[2] = ClearColor.z;
	BeginInfo.ClearColor[3] = 1.0;
	BeginInfo.ClearColors = G_BUFFER_ATTACHMENT_COUNT - 1;
	BeginInfo.ClearDepth = true;
	BeginInfo.RenderPass = GBufferRenderPass;
	BeginInfo.Framebuffer = GBufferFramebuffer;
	BeginInfo.x = 0;
	BeginInfo.y = 0;
	BeginInfo.Width = SceneWidth;
	BeginInfo.Height = SceneHeight;

	OpenVkBeginRenderPass(&BeginInfo);
	{
		OpenVkSetScissor(0, 0, SceneWidth, SceneHeight);
		OpenVkSetViewport(0, 0, SceneWidth, SceneHeight);

		if (!GameMode && !RenderGamePreview)
			DebugDraw();

		uint32_t Pipeline = GBufferPipelineNoneCull;
		switch (GBufferCullMode)
		{
		case CULL_MODE_BACK:
			Pipeline = GBufferPipelineBackCull;
			break;
		case CULL_MODE_FRONT:
			Pipeline = GBufferPipelineFrontCull;
			break;
		default:
			break;
		}

		OpenVkBindPipeline(Pipeline, OPENVK_PIPELINE_TYPE_GRAPHICS);
		
		OpenVkBindDescriptorSet(GBufferLayout, 5, GBufferVertexUniformDescriptorSet, OPENVK_PIPELINE_TYPE_GRAPHICS);

		uint32_t AlbedoDescriptorSet = 0;
		uint32_t NormalDescriptorSet = 0;
		uint32_t MetallicDescriptorSet = 0;
		uint32_t RoughnessDescriptorSet = 0;
		uint32_t OcclusionDescriptorSet = 0;

		uint32_t LastAlbedoDescriptorSet = 0;
		uint32_t LastNormalDescriptorSet = 0;
		uint32_t LastMetallicDescriptorSet = 0;
		uint32_t LastRoughnessDescriptorSet = 0;
		uint32_t LastOcclusionDescriptorSet = 0;

		Mutex.lock();
		for (uint32_t i = 0; i < EntityCount; i++)
		{
			if (Entities[i].UsedComponents[COMPONENT_TYPE_MESH] ||
				Entities[i].UsedComponents[COMPONENT_TYPE_ANIMATION])
			{
				LoadMat4IdentityP(&GBufferVertexPc.Model);
				GBufferVertexPc.Model = ScaleMat4P(&GBufferVertexPc.Model, &Entities[i].Scale);
				GBufferVertexPc.Model = RotateXMat4P(&GBufferVertexPc.Model, ToRadians(Entities[i].Rotate.x));
				GBufferVertexPc.Model = RotateYMat4P(&GBufferVertexPc.Model, ToRadians(Entities[i].Rotate.y));
				GBufferVertexPc.Model = RotateZMat4P(&GBufferVertexPc.Model, ToRadians(Entities[i].Rotate.z));
				GBufferVertexPc.Model = TranslateMat4P(&GBufferVertexPc.Model, &Entities[i].Translate);

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

				if (Albedo != NULL) AlbedoDescriptorSet = Albedo->TextureDescriptorSet;
				if (Normal != NULL) NormalDescriptorSet = Normal->TextureDescriptorSet;
				if (Metallic != NULL) MetallicDescriptorSet = Metallic->TextureDescriptorSet;
				if (Roughness != NULL) RoughnessDescriptorSet = Roughness->TextureDescriptorSet;
				if (Occlusion != NULL) OcclusionDescriptorSet = Occlusion->TextureDescriptorSet;

				if (Entities[i].UsedComponents[COMPONENT_TYPE_MATERIAL])
				{
					GBufferFragmentPc.Color = Material->Color;
					GBufferFragmentPc.Metallic = Material->Metallic;
					GBufferFragmentPc.Roughness = Material->Roughness;
					GBufferFragmentPc.Occlusion = Material->Occlusion;
					GBufferFragmentPc.NearPlane = NearPlane;
					GBufferFragmentPc.FarPlane = FarPlane;
				}
				else
				{
					GBufferFragmentPc.Color = Vec4f(1.0);
					GBufferFragmentPc.Metallic = 0.0;
					GBufferFragmentPc.Roughness = 1.0;
					GBufferFragmentPc.Occlusion = 1.0;
					GBufferFragmentPc.NearPlane = NearPlane;
					GBufferFragmentPc.FarPlane = FarPlane;
				}

				if (Entities[i].UsedComponents[COMPONENT_TYPE_MESH])
				{
					SceneMesh* Mesh = (SceneMesh*)CMA_GetAt(&SceneMeshes, Entities[i].Mesh.MeshIndex);
					if (Mesh != NULL && Mesh->MeshCount > 0)
					{
						OpenVkPushConstant(GBufferLayout, OPENVK_SHADER_TYPE_VERTEX, 0, sizeof(GBufferVertexPushConstant), &GBufferVertexPc);
						OpenVkPushConstant(GBufferLayout, OPENVK_SHADER_TYPE_FRAGMENT, 64, sizeof(GBufferFragmentPushConstant), &GBufferFragmentPc);

						if (Mesh->IndexBuffer != OPENVK_ERROR)
							OpenVkBindIndexBuffer(Mesh->VertexBuffer, Mesh->IndexBuffer);
						else
							OpenVkBindVertexBuffer(Mesh->VertexBuffer);


						for (uint32_t m = 0; m < Mesh->MeshCount; m++)
						{
							if (Mesh->MeshData[m].Render[RENDER_TYPE_DEFAULT])
							{
								if (!Entities[i].UsedComponents[COMPONENT_TYPE_MATERIAL])
								{
									GBufferFragmentPc.Color = Mesh->MeshData[m].Material.Color;
									GBufferFragmentPc.Metallic = Mesh->MeshData[m].Material.Metallic;
									GBufferFragmentPc.Roughness = Mesh->MeshData[m].Material.Roughness;
									GBufferFragmentPc.Occlusion = Mesh->MeshData[m].Material.Occlusion;
									GBufferFragmentPc.NearPlane = NearPlane;
									GBufferFragmentPc.FarPlane = FarPlane;
									OpenVkPushConstant(GBufferLayout, OPENVK_SHADER_TYPE_FRAGMENT, 64, sizeof(GBufferFragmentPushConstant), &GBufferFragmentPc);

									Albedo = (SceneTextureImage*)CMA_GetAt(&SceneTextures, Mesh->MeshData[m].Material.AlbedoIndex);
									Normal = (SceneTextureImage*)CMA_GetAt(&SceneTextures, Mesh->MeshData[m].Material.NormalIndex);
									Metallic = (SceneTextureImage*)CMA_GetAt(&SceneTextures, Mesh->MeshData[m].Material.MetallicIndex);
									Roughness = (SceneTextureImage*)CMA_GetAt(&SceneTextures, Mesh->MeshData[m].Material.RoughnessIndex);
									Occlusion = (SceneTextureImage*)CMA_GetAt(&SceneTextures, Mesh->MeshData[m].Material.OcclusionIndex);

									if (Albedo != NULL) AlbedoDescriptorSet = Albedo->TextureDescriptorSet;
									if (Normal != NULL) NormalDescriptorSet = Normal->TextureDescriptorSet;
									if (Metallic != NULL) MetallicDescriptorSet = Metallic->TextureDescriptorSet;
									if (Roughness != NULL) RoughnessDescriptorSet = Roughness->TextureDescriptorSet;
									if (Occlusion != NULL) OcclusionDescriptorSet = Occlusion->TextureDescriptorSet;
								}

								if (LastAlbedoDescriptorSet != AlbedoDescriptorSet)
									OpenVkBindDescriptorSet(GBufferLayout, 0, AlbedoDescriptorSet, OPENVK_PIPELINE_TYPE_GRAPHICS);
								if (LastNormalDescriptorSet != NormalDescriptorSet)
									OpenVkBindDescriptorSet(GBufferLayout, 1, NormalDescriptorSet, OPENVK_PIPELINE_TYPE_GRAPHICS);
								if (LastMetallicDescriptorSet != MetallicDescriptorSet)
									OpenVkBindDescriptorSet(GBufferLayout, 2, MetallicDescriptorSet, OPENVK_PIPELINE_TYPE_GRAPHICS);
								if (LastRoughnessDescriptorSet != RoughnessDescriptorSet)
									OpenVkBindDescriptorSet(GBufferLayout, 3, RoughnessDescriptorSet, OPENVK_PIPELINE_TYPE_GRAPHICS);
								if (LastOcclusionDescriptorSet != OcclusionDescriptorSet)
									OpenVkBindDescriptorSet(GBufferLayout, 4, OcclusionDescriptorSet, OPENVK_PIPELINE_TYPE_GRAPHICS);

								LastAlbedoDescriptorSet = AlbedoDescriptorSet;
								LastNormalDescriptorSet = NormalDescriptorSet;
								LastMetallicDescriptorSet = MetallicDescriptorSet;
								LastRoughnessDescriptorSet = RoughnessDescriptorSet;
								LastOcclusionDescriptorSet = OcclusionDescriptorSet;

								if (Mesh->IndexBuffer != OPENVK_ERROR)
									OpenVkDrawIndices(Mesh->MeshData[m].IndexOffset, Mesh->MeshData[m].IndexCount, Mesh->MeshData[m].VertexOffset);
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
							OpenVkPushConstant(GBufferLayout, OPENVK_SHADER_TYPE_VERTEX, 0, sizeof(GBufferVertexPushConstant), &GBufferVertexPc);
							OpenVkPushConstant(GBufferLayout, OPENVK_SHADER_TYPE_FRAGMENT, 64, sizeof(GBufferFragmentPushConstant), &GBufferFragmentPc);

							if (LastAlbedoDescriptorSet != AlbedoDescriptorSet)
								OpenVkBindDescriptorSet(GBufferLayout, 0, AlbedoDescriptorSet, OPENVK_PIPELINE_TYPE_GRAPHICS);
							if (LastNormalDescriptorSet != NormalDescriptorSet)
								OpenVkBindDescriptorSet(GBufferLayout, 1, NormalDescriptorSet, OPENVK_PIPELINE_TYPE_GRAPHICS);
							if (LastMetallicDescriptorSet != MetallicDescriptorSet)
								OpenVkBindDescriptorSet(GBufferLayout, 2, MetallicDescriptorSet, OPENVK_PIPELINE_TYPE_GRAPHICS);
							if (LastRoughnessDescriptorSet != RoughnessDescriptorSet)
								OpenVkBindDescriptorSet(GBufferLayout, 3, RoughnessDescriptorSet, OPENVK_PIPELINE_TYPE_GRAPHICS);
							if (LastOcclusionDescriptorSet != OcclusionDescriptorSet)
								OpenVkBindDescriptorSet(GBufferLayout, 4, OcclusionDescriptorSet, OPENVK_PIPELINE_TYPE_GRAPHICS);

							LastAlbedoDescriptorSet = AlbedoDescriptorSet;
							LastNormalDescriptorSet = NormalDescriptorSet;
							LastMetallicDescriptorSet = MetallicDescriptorSet;
							LastRoughnessDescriptorSet = RoughnessDescriptorSet;
							LastOcclusionDescriptorSet = OcclusionDescriptorSet;

							OpenVkBindDynamicVertexBuffer(Animation->VertexBuffer);
							OpenVkDrawVertices(0, Animation->MeshData.NumTriangles * 3);
						}
					}
				}
			}
		}
		Mutex.unlock();
	}
	OpenVkEndRenderPass();
}