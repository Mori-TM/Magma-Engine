void CreateGBufferRenderPass()
{
	uint32_t Format = OPENVK_FORMAT_RGBA32F;
	GBufferAttachments[0] = OpenVkCreateColorImageAttachment(SceneWidth, SceneHeight, 1, true, Format);
	GBufferAttachments[1] = OpenVkCreateColorImageAttachment(SceneWidth, SceneHeight, 1, true, Format);
	GBufferAttachments[2] = OpenVkCreateColorImageAttachment(SceneWidth, SceneHeight, 1, true, OPENVK_FORMAT_RGBA);
	GBufferAttachments[3] = OpenVkCreateColorImageAttachment(SceneWidth, SceneHeight, 1, true, Format);
	GBufferAttachments[4] = OpenVkCreateDepthImageAttachment(SceneWidth, SceneHeight, 1, true, OPENVK_FORMAT_DEFAULT);

	uint32_t Attachments[] = { OPENVK_ATTACHMENT_COLOR, OPENVK_ATTACHMENT_COLOR, OPENVK_ATTACHMENT_COLOR, OPENVK_ATTACHMENT_COLOR, OPENVK_ATTACHMENT_DEPTH };
	uint32_t AttachmentFormats[] = { Format, Format, OPENVK_FORMAT_RGBA, Format, OPENVK_FORMAT_DEFAULT };
	uint32_t AttachmentDescriptions[] = { OPENVK_ATTACHMENT_DESCRIPTION_LOAD_CLEAR, OPENVK_ATTACHMENT_DESCRIPTION_LOAD_CLEAR, OPENVK_ATTACHMENT_DESCRIPTION_LOAD_CLEAR, OPENVK_ATTACHMENT_DESCRIPTION_LOAD_CLEAR, OPENVK_ATTACHMENT_DESCRIPTION_LOAD_CLEAR };
	uint32_t MsaaSamples[] = { 1, 1, 1, 1, 1 };
	GBufferRenderPass = OpenVkCreateRenderPass(G_BUFFER_ATTACHMENT_COUNT, Attachments, AttachmentFormats, AttachmentDescriptions, MsaaSamples, OPENVK_RENDER_PASS_SAMPLED);
}

void CreateGBufferLayout()
{
	uint32_t DescriptorSetLayouts[] =
	{
		VertexUniformDescriptorSetLayout,
		TextureDescriptorSetLayout,
		TextureDescriptorSetLayout,
		TextureDescriptorSetLayout,
		TextureDescriptorSetLayout,
		TextureDescriptorSetLayout,
	};
	
	uint32_t PushTypes[] = { OPENVK_SHADER_TYPE_VERTEX, OPENVK_SHADER_TYPE_FRAGMENT };
	uint32_t PushOffsets[] = { 0, 64 };
	uint32_t PushSizes[] = { sizeof(GBufferVertexPushConstant), sizeof(GBufferFragmentPushConstant) };

	GBufferDescriptorSetLayoutCount = ARRAY_SIZE(DescriptorSetLayouts) < VkRenderer.PhysicalDeviceProperties.limits.maxBoundDescriptorSets ? ARRAY_SIZE(DescriptorSetLayouts) : VkRenderer.PhysicalDeviceProperties.limits.maxBoundDescriptorSets;
	
	OpenVkPipelineLayoutCreateInfo Layout;
	Layout.PushConstantCount = 2;
	Layout.PushConstantShaderTypes = PushTypes;
	Layout.PushConstantOffsets = PushOffsets;
	Layout.PushConstantSizes = PushSizes;
	Layout.DescriptorSetLayoutCount = GBufferDescriptorSetLayoutCount;
	Layout.DescriptorSetLayouts = DescriptorSetLayouts;
	GBufferLayout = OpenVkCreatePipelineLayout(&Layout);
}

void CreateGBufferPipeline()
{
	uint32_t ShaderAttributeFormats[] = { OPENVK_FORMAT_RGBA32F, OPENVK_FORMAT_RGBA32F };
	uint32_t ShaderAttributeOffsets[] = { 0, 16 };

	OpenVkFile VertexShader = OpenVkReadFile("Data/Shader/GBufferVertex.spv");
	OpenVkFile FragmentShader = OpenVkReadFile("Data/Shader/GBufferFragment.spv");
	VertexShader.Freeable = OpenVkFalse;
	FragmentShader.Freeable = OpenVkFalse;
	
	OpenVkBool AlphaBlendings[] = { OpenVkFalse, OpenVkFalse, OpenVkFalse, OpenVkFalse, OpenVkFalse, OpenVkFalse };
	OpenVkGraphicsPipelineCreateInfo GraphicsPipelineCreateInfo;
	GraphicsPipelineCreateInfo.VertexShader = VertexShader;
	GraphicsPipelineCreateInfo.FragmentShader = FragmentShader;
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
	GraphicsPipelineCreateInfo.CullMode = OPENVK_CULL_MODE_NONE;
	GraphicsPipelineCreateInfo.FrontFace = OPENVK_FRONT_FACE_COUNTER_CLOCK_WISE;
	GraphicsPipelineCreateInfo.MsaaSamples = 1;
	GraphicsPipelineCreateInfo.AlphaBlendings = AlphaBlendings;
	GraphicsPipelineCreateInfo.ColorBlendAttachments = G_BUFFER_ATTACHMENT_COUNT - 1;
	GraphicsPipelineCreateInfo.PipelineLayout = GBufferLayout;
	GraphicsPipelineCreateInfo.DepthStencil = true;
	GraphicsPipelineCreateInfo.DepthWrite = OpenVkTrue;
    GraphicsPipelineCreateInfo.DepthCompare = OPENVK_DEPTH_COMPARE_LESS_OR_EQUAL;
	GraphicsPipelineCreateInfo.RenderPass = GBufferRenderPass;
	GraphicsPipelineCreateInfo.Subpass = 0;
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
				SceneFragmentUBO.CameraPosition.x = Entities[i].Translate.x;
				SceneFragmentUBO.CameraPosition.y = Entities[i].Translate.y;
				SceneFragmentUBO.CameraPosition.z = Entities[i].Translate.z;
				SceneFragmentUBO.CameraPosition.w = 0.0;

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
		SceneFragmentUBO.CameraPosition.x = CameraPos.x;
		SceneFragmentUBO.CameraPosition.y = CameraPos.y;
		SceneFragmentUBO.CameraPosition.z = CameraPos.z;
		SceneFragmentUBO.CameraPosition.w = 0.0;

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

		if (!GameMode && !RenderGamePreview && RenderDebug)
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
		
		OpenVkBindDescriptorSet(GBufferLayout, 0, GBufferVertexUniformDescriptorSet, OPENVK_PIPELINE_TYPE_GRAPHICS);

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
					GBufferFragmentPc.Color = MaterialColor;
					GBufferFragmentPc.Metallic = MaterialMetallic;
					GBufferFragmentPc.Roughness = MaterialRoughness;
					GBufferFragmentPc.Occlusion = MaterialOcclusion;
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

					//	RaytracingAddEntityMesh(Entities[i].Mesh.MeshIndex, Mesh);

						for (uint32_t m = 0; m < Mesh->MeshCount; m++)
						{
							if (Mesh->MeshData[m].Render[RENDER_TYPE_DEFAULT])
							{
								if (!Entities[i].UsedComponents[COMPONENT_TYPE_MATERIAL])
								{
									Material = (SceneMaterial*)CMA_GetAt(&SceneMaterials, Mesh->MeshData[m].MaterialIndex);
									if (Material)
									{
										GBufferFragmentPc.Color =		Material->Color;
										GBufferFragmentPc.Metallic =	Material->Metallic;
										GBufferFragmentPc.Roughness =	Material->Roughness;
										GBufferFragmentPc.Occlusion =	Material->Occlusion;


										Albedo = (SceneTextureImage*)CMA_GetAt(&SceneTextures, Material->AlbedoIndex);
										Normal = (SceneTextureImage*)CMA_GetAt(&SceneTextures, Material->NormalIndex);
										Metallic = (SceneTextureImage*)CMA_GetAt(&SceneTextures, Material->MetallicIndex);
										Roughness = (SceneTextureImage*)CMA_GetAt(&SceneTextures, Material->RoughnessIndex);
										Occlusion = (SceneTextureImage*)CMA_GetAt(&SceneTextures, Material->OcclusionIndex);

										if (Albedo != NULL) AlbedoDescriptorSet = Albedo->TextureDescriptorSet;
										if (Normal != NULL) NormalDescriptorSet = Normal->TextureDescriptorSet;
										if (Metallic != NULL) MetallicDescriptorSet = Metallic->TextureDescriptorSet;
										if (Roughness != NULL) RoughnessDescriptorSet = Roughness->TextureDescriptorSet;
										if (Occlusion != NULL) OcclusionDescriptorSet = Occlusion->TextureDescriptorSet;
									}
									else
									{
										GBufferFragmentPc.Color = MaterialColor;
										GBufferFragmentPc.Metallic = MaterialMetallic;
										GBufferFragmentPc.Roughness = MaterialRoughness;
										GBufferFragmentPc.Occlusion = MaterialOcclusion;
									}
									GBufferFragmentPc.NearPlane = NearPlane;
									GBufferFragmentPc.FarPlane = FarPlane;
									OpenVkPushConstant(GBufferLayout, OPENVK_SHADER_TYPE_FRAGMENT, 64, sizeof(GBufferFragmentPushConstant), &GBufferFragmentPc);
								}



								if (LastAlbedoDescriptorSet != AlbedoDescriptorSet && GBufferDescriptorSetLayoutCount > 1)
									OpenVkBindDescriptorSet(GBufferLayout, 1, AlbedoDescriptorSet, OPENVK_PIPELINE_TYPE_GRAPHICS);
								if (LastNormalDescriptorSet != NormalDescriptorSet && GBufferDescriptorSetLayoutCount > 2)
									OpenVkBindDescriptorSet(GBufferLayout, 2, NormalDescriptorSet, OPENVK_PIPELINE_TYPE_GRAPHICS);
								if (LastMetallicDescriptorSet != MetallicDescriptorSet && GBufferDescriptorSetLayoutCount > 3)
									OpenVkBindDescriptorSet(GBufferLayout, 3, MetallicDescriptorSet, OPENVK_PIPELINE_TYPE_GRAPHICS);
								if (LastRoughnessDescriptorSet != RoughnessDescriptorSet && GBufferDescriptorSetLayoutCount > 4)
									OpenVkBindDescriptorSet(GBufferLayout, 4, RoughnessDescriptorSet, OPENVK_PIPELINE_TYPE_GRAPHICS);
								if (LastOcclusionDescriptorSet != OcclusionDescriptorSet && GBufferDescriptorSetLayoutCount > 5)
									OpenVkBindDescriptorSet(GBufferLayout, 5, OcclusionDescriptorSet, OPENVK_PIPELINE_TYPE_GRAPHICS);

								LastAlbedoDescriptorSet = AlbedoDescriptorSet;
								LastNormalDescriptorSet = NormalDescriptorSet;
								LastMetallicDescriptorSet = MetallicDescriptorSet;
								LastRoughnessDescriptorSet = RoughnessDescriptorSet;
								LastOcclusionDescriptorSet = OcclusionDescriptorSet;

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
							OpenVkPushConstant(GBufferLayout, OPENVK_SHADER_TYPE_VERTEX, 0, sizeof(GBufferVertexPushConstant), &GBufferVertexPc);
							OpenVkPushConstant(GBufferLayout, OPENVK_SHADER_TYPE_FRAGMENT, 64, sizeof(GBufferFragmentPushConstant), &GBufferFragmentPc);

							if (LastAlbedoDescriptorSet != AlbedoDescriptorSet && GBufferDescriptorSetLayoutCount > 1)
								OpenVkBindDescriptorSet(GBufferLayout, 1, AlbedoDescriptorSet, OPENVK_PIPELINE_TYPE_GRAPHICS);
							if (LastNormalDescriptorSet != NormalDescriptorSet && GBufferDescriptorSetLayoutCount > 2)
								OpenVkBindDescriptorSet(GBufferLayout, 2, NormalDescriptorSet, OPENVK_PIPELINE_TYPE_GRAPHICS);
							if (LastMetallicDescriptorSet != MetallicDescriptorSet && GBufferDescriptorSetLayoutCount > 3)
								OpenVkBindDescriptorSet(GBufferLayout, 3, MetallicDescriptorSet, OPENVK_PIPELINE_TYPE_GRAPHICS);
							if (LastRoughnessDescriptorSet != RoughnessDescriptorSet && GBufferDescriptorSetLayoutCount > 4)
								OpenVkBindDescriptorSet(GBufferLayout, 4, RoughnessDescriptorSet, OPENVK_PIPELINE_TYPE_GRAPHICS);
							if (LastOcclusionDescriptorSet != OcclusionDescriptorSet && GBufferDescriptorSetLayoutCount > 5)
								OpenVkBindDescriptorSet(GBufferLayout, 5, OcclusionDescriptorSet, OPENVK_PIPELINE_TYPE_GRAPHICS);

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
	}
	OpenVkEndRenderPass();
}