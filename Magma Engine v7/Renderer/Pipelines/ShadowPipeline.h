void CreateShadowRenderPass()
{
	ShadowDepthAttachment = OpenVkCreateDepthImageAttachment(ShadowMapWidth, ShadowMapHeight, 1, true, OPENVK_FORMAT_DEFAULT);

	uint32_t Attachments[] = { OPENVK_ATTACHMENT_DEPTH };
	uint32_t AttachmentFormats[] = { OPENVK_FORMAT_DEFAULT };
	uint32_t MsaaSamples[] = { 1 };
	ShadowRenderPass = OpenVkCreateRenderPass(1, Attachments, AttachmentFormats, MsaaSamples, NULL, true);
}

void CreateShadowLayout()
{
	uint32_t PushTypes[] = { OPENVK_SHADER_TYPE_VERTEX };
	uint32_t PushOffsets[] = { 0 };
	uint32_t PushSizes[] = { sizeof(ShadowVertexPushConstant) };

	OpenVkPipelineLayoutCreateInfo Layout;
	Layout.PushConstantCount = 1;
	Layout.PushConstantShaderTypes = PushTypes;
	Layout.PushConstantOffsets = PushOffsets;
	Layout.PushConstantSizes = PushSizes;
	Layout.DescriptorSetLayoutCount = 1;
	Layout.DescriptorSetLayouts = &TextureDescriptorSetLayout;
	ShadowLayout = OpenVkCreatePipelineLayout(&Layout);
}

void CreateShadowPipeline()
{
	uint32_t ShaderAttributeFormats[] = { OPENVK_FORMAT_RGB32F, OPENVK_FORMAT_RG32F };
	uint32_t ShaderAttributeOffsets[] = { 0, 12 };

	OpenVkFile VertexShader = OpenVkReadFile("Data/Shader/ShadowVertex.spv");
	OpenVkFile FragmentShader = OpenVkReadFile("Data/Shader/ShadowFragment.spv");
	VertexShader.Freeable = OpenVkFalse;
	FragmentShader.Freeable = OpenVkFalse;

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
	GraphicsPipelineCreateInfo.Width = ShadowMapWidth;
	GraphicsPipelineCreateInfo.Height = ShadowMapHeight;
	GraphicsPipelineCreateInfo.DepthClamp = true;
	GraphicsPipelineCreateInfo.PolygonMode = OPENVK_POLYGON_MODE_FILL;
	GraphicsPipelineCreateInfo.LineWidth = 3.0;
	GraphicsPipelineCreateInfo.CullMode = OPENVK_CULL_MODE_NONE;
	GraphicsPipelineCreateInfo.FrontFace = OPENVK_FRONT_FACE_COUNTER_CLOCK_WISE;
	GraphicsPipelineCreateInfo.MsaaSamples = 1;
	GraphicsPipelineCreateInfo.AlphaBlending = false;
	GraphicsPipelineCreateInfo.ColorBlendAttachments = 0;
	GraphicsPipelineCreateInfo.PipelineLayout = ShadowLayout;
	GraphicsPipelineCreateInfo.DepthStencil = true;
	GraphicsPipelineCreateInfo.RenderPass = ShadowRenderPass;
	ShadowPipelineNoneCull = OpenVkCreateGraphicsPipeline(&GraphicsPipelineCreateInfo);

	GraphicsPipelineCreateInfo.CullMode = OPENVK_CULL_MODE_BACK;
	GraphicsPipelineCreateInfo.VertexShader = VertexShader;
	GraphicsPipelineCreateInfo.FragmentShader = FragmentShader;
	ShadowPipelineBackCull = OpenVkCreateGraphicsPipeline(&GraphicsPipelineCreateInfo);

	GraphicsPipelineCreateInfo.CullMode = OPENVK_CULL_MODE_FRONT;
	GraphicsPipelineCreateInfo.VertexShader = VertexShader;
	GraphicsPipelineCreateInfo.FragmentShader = FragmentShader;
	ShadowPipelineFrontCull = OpenVkCreateGraphicsPipeline(&GraphicsPipelineCreateInfo);

	free(FragmentShader.Data);
	free(VertexShader.Data);
}

void CreateShadowFramebuffers()
{
	uint32_t Attachments[] = { ShadowDepthAttachment };

	OpenVkFramebufferCreateInfo FramebufferCreateInfo;
	FramebufferCreateInfo.AttachmentCount = ARRAY_SIZE(Attachments);
	FramebufferCreateInfo.Attachments = Attachments;
	FramebufferCreateInfo.RenderPass = ShadowRenderPass;
	FramebufferCreateInfo.Width = ShadowMapWidth;
	FramebufferCreateInfo.Height = ShadowMapHeight;
	ShadowFramebuffer = OpenVkCreateFramebuffer(&FramebufferCreateInfo);
}

float CascadeSplits[SHADOW_MAP_CASCADE_COUNT];
void UpdateCascades()
{
	float ClipRange = CascadeFarClip - CascadeNearClip;

	float MinZ = CascadeNearClip;
	float MaxZ = CascadeNearClip + ClipRange;

	float Range = MaxZ - MinZ;
	float Ratio = MaxZ / MinZ;

	for (uint32_t i = 0; i < SHADOW_MAP_CASCADE_COUNT; i++)
	{
		float p = (i + 1) / (float)SHADOW_MAP_CASCADE_COUNT;
		float Log = MinZ * powf(Ratio, p);
		float Uniform = MinZ + Range * p;
		float d = CascadeSplitLambda * (Log - Uniform) + Uniform;
		CascadeSplits[i] = (d - CascadeNearClip) / ClipRange;
	}
	Mutex.lock();
	mat4 TempSceneProjection = SceneVertexUBO.Projection;

	SceneVertexUBO.Projection.m[2][2] = CascadeFarClip / (CascadeNearClip - CascadeFarClip);
	SceneVertexUBO.Projection.m[3][2] = (CascadeNearClip * CascadeFarClip) / (CascadeNearClip - CascadeFarClip);
	
	mat4 InvCam = InverseMat4(MultiplyMat4P(&SceneVertexUBO.Projection, &SceneVertexUBO.View));
	Mutex.unlock();

	float LastSplitDist = 0.0;
	for (uint32_t i = 0; i < SHADOW_MAP_CASCADE_COUNT; i++)
	{
		float SplitDist = CascadeSplits[i];

		vec3 FrustumCorners[8] =
		{
			{ -1.0, 1.0, -1.0 },
			{ 1.0, 1.0, -1.0 },
			{ 1.0, -1.0, -1.0 },
			{ -1.0, -1.0, -1.0 },
			{ -1.0, 1.0, 1.0 },
			{ 1.0, 1.0, 1.0 },
			{ 1.0, -1.0, 1.0 },
			{ -1.0, -1.0, 1.0 },
		};

		for (uint32_t j = 0; j < 8; j++)
		{
			vec4 m = Vec4(FrustumCorners[j].x, FrustumCorners[j].y, FrustumCorners[j].z, 1.0f);
			vec4 InvCorner = MultiplyVec4Mat4P(&m, &InvCam);

			FrustumCorners[j].x = InvCorner.x / InvCorner.w;
			FrustumCorners[j].y = InvCorner.y / InvCorner.w;
			FrustumCorners[j].z = InvCorner.z / InvCorner.w;
		}

		for (uint32_t j = 0; j < 4; j++)
		{
			vec3 Dist = Sub3(FrustumCorners[j + 4], FrustumCorners[j]);
			FrustumCorners[j + 4] = Add3(FrustumCorners[j], Mul3(Dist, Vec3f(SplitDist)));
			FrustumCorners[j] = Add3(FrustumCorners[j], Mul3(Dist, Vec3f(LastSplitDist)));
		}
		vec3 FrustumCenter = Vec3f(0.0);
		for (uint32_t j = 0; j < 8; j++)
			FrustumCenter = Add3(FrustumCenter, FrustumCorners[j]);
		FrustumCenter = Div3(FrustumCenter, Vec3f(8.0));

		float Radius = 0.0;
		for (uint32_t j = 0; j < 8; j++)
		{
			float distance = Length3(Sub3(FrustumCorners[j], FrustumCenter));
			Radius = MAX(Radius, distance);
		}
		Radius = ceilf(Radius * 16.0) / 16.0;

		vec3 MaxExtents = Vec3f(Radius);
		vec3 MinExtents = Vec3f(-Radius);

		mat4 Projection;
		mat4 View;
		OrthoMat4P(MinExtents.x, MaxExtents.x, MaxExtents.y, MinExtents.y, 0.0, MaxExtents.z - MinExtents.z, &Projection);

		vec3 LightDirection = { SceneFragmentUBO.LightDirection.x, SceneFragmentUBO.LightDirection.y, SceneFragmentUBO.LightDirection.z };
		vec3 pp = Sub3(FrustumCenter, Mul3(LightDirection, Vec3f(-MinExtents.z)));
		vec3 l = FrustumCenter;
		vec3 u = { 0.0f, 1.0f, 0.0f };
		View = LookAtMat4P(&pp, &l, &u);

		Cascades[i].SplitDepth = (CascadeNearClip + SplitDist * ClipRange) * -1.0;
		Cascades[i].ProjectionView = MultiplyMat4P(&Projection, &View);

		Mutex.lock();
		CullingCascades[i] = MultiplyMat4P(&Projection, &View);
		Mutex.unlock();

		LastSplitDist = CascadeSplits[i];
	}

	Mutex.lock();
	SceneVertexUBO.Projection = TempSceneProjection;
	Mutex.unlock();
}

void UpdateAnimation(uint32_t AnimationIndex)
{
	SceneAnimation* Animation = (SceneAnimation*)CMA_GetAt(&SceneAnimations, AnimationIndex);
	if (Animation != NULL)
	{
		Md2::ModelData* Model = &Animation->MeshData;
		SceneVertex* Vertices = Animation->Vertices;
		//, SceneVertex* Vertices, uint32_t VertexBuffer;

		Md2::Proccess ProcModel;
		ProcModel.Model = Model;
		ProcModel.Start(Animation->Start, Animation->End);
		int j = 0;
		for (int i = 0; i < Model->NumTriangles; i++)
		{
			ProcModel.Update(i);
			Vertices[j].Pos.x = ProcModel.Vertex[0].P[0];
			Vertices[j].Pos.y = ProcModel.Vertex[0].P[1];
			Vertices[j].Pos.z = ProcModel.Vertex[0].P[2];
			Vertices[j].TexCoord.x = ProcModel.TexCoord[0].S;
			Vertices[j].TexCoord.y = ProcModel.TexCoord[0].T;
			Vertices[j].Normal.x = ProcModel.Normal[0].P[0];
			Vertices[j].Normal.y = ProcModel.Normal[0].P[1];
			Vertices[j].Normal.z = ProcModel.Normal[0].P[2];

			Vertices[j + 1].Pos.x = ProcModel.Vertex[2].P[0];
			Vertices[j + 1].Pos.y = ProcModel.Vertex[2].P[1];
			Vertices[j + 1].Pos.z = ProcModel.Vertex[2].P[2];
			Vertices[j + 1].TexCoord.x = ProcModel.TexCoord[2].S;
			Vertices[j + 1].TexCoord.y = ProcModel.TexCoord[2].T;
			Vertices[j + 1].Normal.x = ProcModel.Normal[2].P[0];
			Vertices[j + 1].Normal.y = ProcModel.Normal[2].P[1];
			Vertices[j + 1].Normal.z = ProcModel.Normal[2].P[2];

			Vertices[j + 2].Pos.x = ProcModel.Vertex[1].P[0];
			Vertices[j + 2].Pos.y = ProcModel.Vertex[1].P[1];
			Vertices[j + 2].Pos.z = ProcModel.Vertex[1].P[2];
			Vertices[j + 2].TexCoord.x = ProcModel.TexCoord[1].S;
			Vertices[j + 2].TexCoord.y = ProcModel.TexCoord[1].T;
			Vertices[j + 2].Normal.x = ProcModel.Normal[1].P[0];
			Vertices[j + 2].Normal.y = ProcModel.Normal[1].P[1];
			Vertices[j + 2].Normal.z = ProcModel.Normal[1].P[2];
			j += 3;
		}
		ProcModel.End(GetDeltaTime() * Animation->Speed);

		OpenVkUpdateDynamicBuffer(Model->NumTriangles * 3 * sizeof(SceneVertex), Vertices, Animation->VertexBuffer);
	}
}

void ShadowDraw()
{
	OpenVkBeginRenderPassInfo BeginInfo;
	BeginInfo.ClearColor[0] = 0.01;
	BeginInfo.ClearColor[1] = 0.01;
	BeginInfo.ClearColor[2] = 0.01;
	BeginInfo.ClearColor[3] = 1.0;
	BeginInfo.ClearColors = 0;
	BeginInfo.ClearDepth = true;
	BeginInfo.RenderPass = ShadowRenderPass;
	BeginInfo.Framebuffer = ShadowFramebuffer;
	BeginInfo.x = 0;
	BeginInfo.y = 0;
	BeginInfo.Width = ShadowMapWidth;
	BeginInfo.Height = ShadowMapHeight;
	OpenVkBeginRenderPass(&BeginInfo);
	{
		OpenVkSetScissor(0, 0, ShadowMapWidth, ShadowMapHeight);
		for (uint32_t j = 0; j < SHADOW_MAP_CASCADE_COUNT; j++)
		{
			uint32_t Offset = (ShadowMapHeight * j);
			
			OpenVkSetViewport(Offset, 0, ShadowMapHeight, ShadowMapHeight);

			uint32_t Pipeline = ShadowPipelineNoneCull;
			switch (ShadowCullMode)
			{
			case CULL_MODE_BACK:
				Pipeline = ShadowPipelineBackCull;
				break;
			case CULL_MODE_FRONT:
				Pipeline = ShadowPipelineFrontCull;
				break;
			default:
				break;
			}

			OpenVkBindPipeline(Pipeline, OPENVK_PIPELINE_TYPE_GRAPHICS);

			mat4 Model;

			uint32_t TextureDescriptorSet = 0;
			uint32_t LastTextureDescriptorSet = 0;

			Mutex.lock();
			for (uint32_t i = 0; i < EntityCount; i++)
			{
				LoadMat4IdentityP(&Model);
				Model = ScaleMat4P(&Model, &Entities[i].Scale);
				Model = RotateXMat4P(&Model, ToRadians(Entities[i].Rotate.x));
				Model = RotateYMat4P(&Model, ToRadians(Entities[i].Rotate.y));
				Model = RotateZMat4P(&Model, ToRadians(Entities[i].Rotate.z));
				Model = TranslateMat4P(&Model, &Entities[i].Translate);

				ShadowVertexPc.PVM = MultiplyMat4P(&Cascades[j].ProjectionView, &Model);

				
				SceneMaterial* Material = (SceneMaterial*)CMA_GetAt(&SceneMaterials, Entities[i].Material.MaterialIndex);
				if (Material == NULL)
					Material = (SceneMaterial*)CMA_GetAt(&SceneMaterials, 0);

				SceneTextureImage* Image;
				if (Entities[i].UsedComponents[COMPONENT_TYPE_MATERIAL])
					Image = (SceneTextureImage*)CMA_GetAt(&SceneTextures, Material->AlbedoIndex);
				else
					Image = (SceneTextureImage*)CMA_GetAt(&SceneTextures, 0);

				if (Image != NULL)
					TextureDescriptorSet = Image->TextureDescriptorSet;

				if (Entities[i].UsedComponents[COMPONENT_TYPE_MESH])
				{
					SceneMesh* Mesh = (SceneMesh*)CMA_GetAt(&SceneMeshes, Entities[i].Mesh.MeshIndex);
					if (Mesh != NULL && Mesh->MeshCount > 0)
					{
						OpenVkPushConstant(ShadowLayout, OPENVK_SHADER_TYPE_VERTEX, 0, sizeof(ShadowVertexPushConstant), &ShadowVertexPc);

						if (Mesh->IndexBuffer != OPENVK_ERROR)
							OpenVkBindIndexBuffer(Mesh->VertexBuffer, Mesh->IndexBuffer);
						else
							OpenVkBindVertexBuffer(Mesh->VertexBuffer);

						for (uint32_t m = 0; m < Mesh->MeshCount; m++)
						{
							if (Mesh->MeshData[m].Render[j + 1])
							{
								if (!Entities[i].UsedComponents[COMPONENT_TYPE_MATERIAL])
								{
									SceneTextureImage* Image = (SceneTextureImage*)CMA_GetAt(&SceneTextures, Mesh->MeshData[m].Material.AlbedoIndex);
									if (Image != NULL)
										TextureDescriptorSet = Image->TextureDescriptorSet;
								}

								if (LastTextureDescriptorSet != TextureDescriptorSet)
									OpenVkBindDescriptorSet(ShadowLayout, 0, TextureDescriptorSet, OPENVK_PIPELINE_TYPE_GRAPHICS);

								LastTextureDescriptorSet = TextureDescriptorSet;

								if (Mesh->IndexBuffer != OPENVK_ERROR)
									OpenVkDrawIndices(Mesh->MeshData[m].IndexOffset, Mesh->MeshData[m].IndexCount, Mesh->MeshData[m].VertexOffset);								
								else
									OpenVkDrawVertices(Mesh->MeshData[m].VertexOffset, Mesh->MeshData[m].VertexCount);
							}							
						}
					}
				}
				if (Entities[i].UsedComponents[COMPONENT_TYPE_ANIMATION])
				{
					if (Entities[i].Animation.AnimationIndex != 0)
					{
						SceneAnimation* Animation = (SceneAnimation*)CMA_GetAt(&SceneAnimations, Entities[i].Animation.AnimationIndex);
						if (Animation != NULL)
						{
							OpenVkPushConstant(ShadowLayout, OPENVK_SHADER_TYPE_VERTEX, 0, sizeof(ShadowVertexPushConstant), &ShadowVertexPc);

							UpdateAnimation(Entities[i].Animation.AnimationIndex);

							if (LastTextureDescriptorSet != TextureDescriptorSet)
								OpenVkBindDescriptorSet(ShadowLayout, 0, TextureDescriptorSet, OPENVK_PIPELINE_TYPE_GRAPHICS);
							LastTextureDescriptorSet = TextureDescriptorSet;

							OpenVkBindDynamicVertexBuffer(Animation->VertexBuffer);
							OpenVkDrawVertices(0, Animation->MeshData.NumTriangles * 3);
						}
					}
				}
			}
			Mutex.unlock();
		}
	}
	OpenVkEndRenderPass();
}