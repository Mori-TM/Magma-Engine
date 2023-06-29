void CreateSceneRenderPass()
{
	SceneAttachment = OpenVkCreateColorImageAttachment(SceneWidth, SceneHeight, 1, true, OPENVK_FORMAT_DEFAULT);

	uint32_t Attachments[] = { OPENVK_ATTACHMENT_COLOR };
	uint32_t AttachmentFormats[] = { OPENVK_FORMAT_DEFAULT };
	uint32_t MsaaSamples[] = { 1 };
	SceneRenderPass = OpenVkCreateRenderPass(1, Attachments, AttachmentFormats, MsaaSamples, OPENVK_RENDER_PASS_SAMPLED | OPENVK_RENDER_PASS_BOTTOM_OF_PIPE | OPENVK_RENDER_PASS_MEMORY_READ | OPENVK_RENDER_PASS_COLOR_ACCESS_READ_AND_WRITE);
}

void CreateSceneLayout()
{
	OpenVkPipelineLayoutCreateInfo Layout;
	Layout.PushConstantCount = 0;
	Layout.PushConstantShaderTypes = NULL;
	Layout.PushConstantOffsets = NULL;
	Layout.PushConstantSizes = NULL;
	Layout.DescriptorSetLayoutCount = 1;
	Layout.DescriptorSetLayouts = &SceneDescriptorSetLayout;
	SceneLayout = OpenVkCreatePipelineLayout(&Layout);
}

void CreateScenePipeline()
{
	OpenVkGraphicsPipelineCreateInfo GraphicsPipelineCreateInfo;
	GraphicsPipelineCreateInfo.VertexShader = OpenVkReadFile("Data/Shader/OffscreenVertex.spv");
	GraphicsPipelineCreateInfo.FragmentShader = OpenVkReadFile("Data/Shader/SceneFragment.spv");
	GraphicsPipelineCreateInfo.BindingStride = 0;
	GraphicsPipelineCreateInfo.ShaderAttributeFormatCount = 0;
	GraphicsPipelineCreateInfo.ShaderAttributeFormats = NULL;
	GraphicsPipelineCreateInfo.ShaderAttributeOffsets = NULL;
	GraphicsPipelineCreateInfo.PrimitiveTopology = OPENVK_PRIMITIVE_TOPOLOGY_TRIANGLE;
	GraphicsPipelineCreateInfo.x = 0;
	GraphicsPipelineCreateInfo.y = 0;
	GraphicsPipelineCreateInfo.Width = SceneWidth;
	GraphicsPipelineCreateInfo.Height = SceneHeight;
	GraphicsPipelineCreateInfo.DepthClamp = false;
	GraphicsPipelineCreateInfo.PolygonMode = OPENVK_POLYGON_MODE_FILL;
	GraphicsPipelineCreateInfo.LineWidth = 3.0;
	GraphicsPipelineCreateInfo.CullMode = OPENVK_CULL_MODE_FRONT;
	GraphicsPipelineCreateInfo.FrontFace = OPENVK_FRONT_FACE_COUNTER_CLOCK_WISE;
	GraphicsPipelineCreateInfo.MsaaSamples = 1;
	GraphicsPipelineCreateInfo.AlphaBlendings = NULL;
	GraphicsPipelineCreateInfo.ColorBlendAttachments = 1;	
	GraphicsPipelineCreateInfo.PipelineLayout = SceneLayout;
	GraphicsPipelineCreateInfo.DepthStencil = false;
	GraphicsPipelineCreateInfo.RenderPass = SceneRenderPass;
	ScenePipeline = OpenVkCreateGraphicsPipeline(&GraphicsPipelineCreateInfo);
}

void CreateSceneFramebuffer()
{
	OpenVkFramebufferCreateInfo FramebufferCreateInfo;
	FramebufferCreateInfo.AttachmentCount = 1;
	FramebufferCreateInfo.Attachments = &SceneAttachment;
	FramebufferCreateInfo.RenderPass = SceneRenderPass;
	FramebufferCreateInfo.Width = SceneWidth;
	FramebufferCreateInfo.Height = SceneHeight;
	SceneFramebuffer = OpenVkCreateFramebuffer(&FramebufferCreateInfo);
}

void CreateSceneUniformBuffer()
{
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
			DescriptorSetCreateInfo.VariableDescriptorSetCount = 0;

			Image->TextureDescriptorSet = OpenVkCreateDescriptorSet(&DescriptorSetCreateInfo);
		}		
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
		DescriptorSetCreateInfo.VariableDescriptorSetCount = 0;

		SceneFragmentUniformDescriptorSet = OpenVkCreateDescriptorSet(&DescriptorSetCreateInfo);
	}
	
	{
		uint32_t Attachments[] = { GBufferAttachments[0], GBufferAttachments[1], GBufferAttachments[2], GBufferAttachments[3], GBufferAttachments[4], GBufferAttachments[5], SSAOBlurColorAttachment, ShadowDepthAttachment };
		uint32_t DescriptorCounts[] = { 1, 1, 1, 1, 1, 1, 1, 1, 1 };
		uint32_t DescriptorTypes[] = 
		{ 
			OPENVK_DESCRIPTOR_TYPE_IMAGE_SAMPLER, 
			OPENVK_DESCRIPTOR_TYPE_IMAGE_SAMPLER, 
			OPENVK_DESCRIPTOR_TYPE_IMAGE_SAMPLER, 
			OPENVK_DESCRIPTOR_TYPE_IMAGE_SAMPLER, 
			OPENVK_DESCRIPTOR_TYPE_IMAGE_SAMPLER, 
			OPENVK_DESCRIPTOR_TYPE_IMAGE_SAMPLER, 
			OPENVK_DESCRIPTOR_TYPE_IMAGE_SAMPLER, 
			OPENVK_DESCRIPTOR_TYPE_IMAGE_SAMPLER,
			OPENVK_DESCRIPTOR_TYPE_UNIFORM_BUFFER 
		};
		uint32_t ImageTypes[] = { OPENVK_IMAGE_TYPE_ATTACHMENT, OPENVK_IMAGE_TYPE_ATTACHMENT, OPENVK_IMAGE_TYPE_ATTACHMENT, OPENVK_IMAGE_TYPE_ATTACHMENT, OPENVK_IMAGE_TYPE_ATTACHMENT, OPENVK_IMAGE_TYPE_ATTACHMENT, OPENVK_IMAGE_TYPE_ATTACHMENT, OPENVK_IMAGE_TYPE_ATTACHMENT };
		uint32_t ImageLayouts[] = { OPENVK_IMAGE_LAYOUT_COLOR_OUTPUT, OPENVK_IMAGE_LAYOUT_COLOR_OUTPUT, OPENVK_IMAGE_LAYOUT_COLOR_OUTPUT, OPENVK_IMAGE_LAYOUT_COLOR_OUTPUT, OPENVK_IMAGE_LAYOUT_COLOR_OUTPUT, OPENVK_IMAGE_LAYOUT_COLOR_OUTPUT, OPENVK_IMAGE_LAYOUT_COLOR_OUTPUT, OPENVK_IMAGE_LAYOUT_DEPTH_OUTPUT };
		uint32_t Bindings[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8 };
		uint32_t Sampler[] = { GBufferSampler, GBufferSampler, GBufferSampler, GBufferSampler, GBufferSampler, GBufferSampler, GBufferSampler, ShadowSampler };
		uint32_t UniformBuffers[] = { SceneFragmentUniformBuffer };
		size_t UniformSizes[] = { sizeof(SceneFragmentUniformBufferObject) };

		OpenVkDescriptorSetCreateInfo DescriptorSetCreateInfo;
		DescriptorSetCreateInfo.DescriptorSetLayout = SceneDescriptorSetLayout;
		DescriptorSetCreateInfo.DescriptorPool = DescriptorPool;
		DescriptorSetCreateInfo.DescriptorWriteCount = 9;
		DescriptorSetCreateInfo.DescriptorCounts = DescriptorCounts;
		DescriptorSetCreateInfo.DescriptorTypes = DescriptorTypes;
		DescriptorSetCreateInfo.Sampler = Sampler;
		DescriptorSetCreateInfo.ImageTypes = ImageTypes;
		DescriptorSetCreateInfo.Images = Attachments;
		DescriptorSetCreateInfo.ImageLayouts = ImageLayouts;
		DescriptorSetCreateInfo.Buffers = UniformBuffers;
		DescriptorSetCreateInfo.BufferSizes = UniformSizes;
		DescriptorSetCreateInfo.Bindings = Bindings;
		DescriptorSetCreateInfo.DescriptorSet = NULL;
		DescriptorSetCreateInfo.VariableDescriptorSetCount = 0;
		
		SceneInputDescriptorSet = OpenVkCreateDescriptorSet(&DescriptorSetCreateInfo);
	}
	
	{
		uint32_t Attachments[] = { SceneAttachment };
		uint32_t DescriptorCounts[] = { 1 };
		uint32_t DescriptorTypes[] = { OPENVK_DESCRIPTOR_TYPE_IMAGE_SAMPLER };
		uint32_t ImageTypes[] = { OPENVK_IMAGE_TYPE_ATTACHMENT };
		uint32_t ImageLayouts[] = { OPENVK_IMAGE_LAYOUT_COLOR_OUTPUT };
		uint32_t Bindings[] = { 0 };
		uint32_t Sampler[] = { ImageSampler };

		OpenVkDescriptorSetCreateInfo DescriptorSetCreateInfo;
		DescriptorSetCreateInfo.DescriptorSetLayout = TextureDescriptorSetLayout;
		DescriptorSetCreateInfo.DescriptorPool = DescriptorPool;
		DescriptorSetCreateInfo.DescriptorWriteCount = 1;
		DescriptorSetCreateInfo.DescriptorCounts = DescriptorCounts;
		DescriptorSetCreateInfo.DescriptorTypes = DescriptorTypes;
		DescriptorSetCreateInfo.Sampler = Sampler;
		DescriptorSetCreateInfo.ImageTypes = ImageTypes;
		DescriptorSetCreateInfo.Images = Attachments;
		DescriptorSetCreateInfo.ImageLayouts = ImageLayouts;
		DescriptorSetCreateInfo.Bindings = Bindings;
		DescriptorSetCreateInfo.DescriptorSet = NULL;
		DescriptorSetCreateInfo.VariableDescriptorSetCount = 0;

		SceneOutputDescriptorSet = OpenVkCreateDescriptorSet(&DescriptorSetCreateInfo);
	}

	
}

void SceneUpdateUniformBuffer()
{
////	Mutex.lock();
//	LoadMat4IdentityP(&SceneVertexUBO.View);
//	if (GameMode)
//	{
//		for (uint32_t i = 0; i < EntityCount; i++)
//		{
//			if (Entities[i].UsedComponents[COMPONENT_TYPE_CAMERA])
//			{
//				LoadMat4IdentityP(&SceneVertexUBO.View);
//				SceneVertexUBO.View = RotateXMat4P(&SceneVertexUBO.View, ToRadians(Entities[i].Rotate.x));
//				SceneVertexUBO.View = RotateYMat4P(&SceneVertexUBO.View, ToRadians(Entities[i].Rotate.y));
//				SceneVertexUBO.View = RotateZMat4P(&SceneVertexUBO.View, ToRadians(Entities[i].Rotate.z));
//				SceneVertexUBO.View = TranslateMat4P(&SceneVertexUBO.View, &Entities[i].Translate);
//				SceneVertexUBO.View = InverseMat4(SceneVertexUBO.View);
//				PerspectiveMat4P(((float)SceneWidth / SceneHeight), ToRadians(Entities[i].Camera.FOV), Entities[i].Camera.NearPlane, Entities[i].Camera.FarPlane, &SceneVertexUBO.Projection);
//			}
//		}
//	}
//	else
//	{
//		vec3 t = Add3P(&CameraPos, &CameraDir);
//		SceneVertexUBO.View = LookAtMat4P(&CameraPos, &t, &CameraUp);
//		PerspectiveMat4P(((float)SceneWidth / SceneHeight), ToRadians(CameraFOV), CameraNearZ, CameraFarZ, &SceneVertexUBO.Projection);
//	}	
//
//	for (uint32_t i = 0; i < SHADOW_MAP_CASCADE_COUNT; i++)
//	{
//		SceneFragmentUBO.CascadeSplits[i] = Cascades[i].SplitDepth;
//		SceneFragmentUBO.CascadeProjectionView[i] = Cascades[i].ProjectionView;
//	}
//	SceneFragmentUBO.View = SceneVertexUBO.View;
//	
////	mat4 ViewProj = MultiplyMat4P(&SceneVertexUBO.Projection, &SceneVertexUBO.View);
////	RunFrustumCulling(ViewProj, RENDER_TYPE_DEFAULT);
//
//
//	OpenVkUpdateBuffer(sizeof(SceneVertexUniformBufferObject), &SceneVertexUBO, SceneVertexUniformBuffer);

	for (uint32_t i = 0; i < SHADOW_MAP_CASCADE_COUNT; i++)
	{
		SceneFragmentUBO.CascadeSplits[i] = Cascades[i].SplitDepth;
		SceneFragmentUBO.CascadeProjectionView[i] = Cascades[i].ProjectionViewBias;
	}
	SceneFragmentUBO.View = GBufferVertexUBO.View;

	OpenVkUpdateBuffer(sizeof(SceneFragmentUniformBufferObject), &SceneFragmentUBO, SceneFragmentUniformBuffer);
//	Mutex.unlock();
}

void SceneDraw()
{
	OpenVkBeginRenderPassInfo BeginInfo;
	BeginInfo.ClearColor[0] = 0.0;//ClearColor.x;
	BeginInfo.ClearColor[1] = 0.0;//ClearColor.y;
	BeginInfo.ClearColor[2] = 0.0;//ClearColor.z;
	BeginInfo.ClearColor[3] = 1.0;
	BeginInfo.ClearColors = 1;
	BeginInfo.ClearDepth = false;
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

		OpenVkBindPipeline(ScenePipeline, OPENVK_PIPELINE_TYPE_GRAPHICS);

		OpenVkBindDescriptorSet(SceneLayout, 0, SceneInputDescriptorSet, OPENVK_PIPELINE_TYPE_GRAPHICS);
		OpenVkDrawVertices(0, 3);
	}
	OpenVkEndRenderPass();
}