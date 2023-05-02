void CreateSSRRenderPass()
{
	SSRColorAttachment = OpenVkCreateColorImageAttachment(SceneWidth, SceneHeight, 1, true, OPENVK_FORMAT_DEFAULT);
	
	uint32_t Attachments[] = { OPENVK_ATTACHMENT_COLOR };
	uint32_t AttachmentFormats[] = { OPENVK_FORMAT_DEFAULT };
	uint32_t MsaaSamples[] = { 1 };
	SSRRenderPass = OpenVkCreateRenderPass(1, Attachments, AttachmentFormats, MsaaSamples, NULL, true);
}

void CreateSSRLayout()
{
	uint32_t DescriptorSetLayouts[] =
	{
		SceneDescriptorSetLayout,
		FragmentUniformDescriptorSetLayout
	};

	OpenVkPipelineLayoutCreateInfo Layout;
	Layout.PushConstantCount = 0;
	Layout.PushConstantShaderTypes = NULL;
	Layout.PushConstantOffsets = 0;
	Layout.PushConstantSizes = 0;
	Layout.DescriptorSetLayoutCount = ARRAY_SIZE(DescriptorSetLayouts);
	Layout.DescriptorSetLayouts = DescriptorSetLayouts;
	SSRLayout = OpenVkCreatePipelineLayout(&Layout);
}

void CreateSSRPipeline()
{
	OpenVkGraphicsPipelineCreateInfo GraphicsPipelineCreateInfo;
	GraphicsPipelineCreateInfo.VertexShader = OpenVkReadFile("Data/Shader/OffscreenVertex.spv");
	GraphicsPipelineCreateInfo.FragmentShader = OpenVkReadFile("Data/Shader/SSRFragment.spv");
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
	GraphicsPipelineCreateInfo.LineWidth = 1.0;
	GraphicsPipelineCreateInfo.CullMode = OPENVK_CULL_MODE_FRONT;
	GraphicsPipelineCreateInfo.FrontFace = OPENVK_FRONT_FACE_COUNTER_CLOCK_WISE;
	GraphicsPipelineCreateInfo.MsaaSamples = 1;
	GraphicsPipelineCreateInfo.AlphaBlending = true;
	GraphicsPipelineCreateInfo.ColorBlendAttachments = 1;
	GraphicsPipelineCreateInfo.PipelineLayout = SSRLayout;
	GraphicsPipelineCreateInfo.DepthStencil = false;
	GraphicsPipelineCreateInfo.RenderPass = SSRRenderPass;

	SSRPipeline = OpenVkCreateGraphicsPipeline(&GraphicsPipelineCreateInfo);
}

void CreateSSRFramebuffer()
{
	uint32_t Attachments[] = { SSRColorAttachment };

	OpenVkFramebufferCreateInfo FramebufferCreateInfo;
	FramebufferCreateInfo.AttachmentCount = ARRAY_SIZE(Attachments);
	FramebufferCreateInfo.Attachments = Attachments;
	FramebufferCreateInfo.RenderPass = SSRRenderPass;
	FramebufferCreateInfo.Width = SceneWidth;
	FramebufferCreateInfo.Height = SceneHeight;
	SSRFramebuffer = OpenVkCreateFramebuffer(&FramebufferCreateInfo);
}

void CreateSSRUniformBuffer()
{
	SSRUniformBuffer = OpenVkCreateUniformBuffer(sizeof(SSRUniformBufferObject));
}

void CreateSSRDescriptorSet()
{
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
		DescriptorSetCreateInfo.Images = &SSRColorAttachment;
		DescriptorSetCreateInfo.ImageLayouts = ImageLayouts;
		DescriptorSetCreateInfo.Bindings = Bindings;
		DescriptorSetCreateInfo.DescriptorSet = NULL;
		DescriptorSetCreateInfo.VariableDescriptorSetCount = 0;

		SSRDescriptorSet = OpenVkCreateDescriptorSet(&DescriptorSetCreateInfo);
	}
	/*
	{
		unsigned char* Pixels;
		int32_t Width;
		int32_t Height;
		OpenVkLoadTexture("Data/Textures/spiaggia_di_mondello.jpg", OpenVkTrue, &Pixels, &Width, &Height, OPENVK_FORMAT_RGBA);
		uint32_t Texture = OpenVkCreateTextureImage(Pixels, Width, Height, OPENVK_FORMAT_RGBA);
		uint32_t Sampler = OpenVkCreateImageSampler(OPENVK_FILTER_LINEAR, OPENVK_ADDRESS_MODE_REPEAT);

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
		DescriptorSetCreateInfo.Sampler = &Sampler;
		DescriptorSetCreateInfo.ImageTypes = ImageTypes;
		DescriptorSetCreateInfo.ImageLayouts = ImageLayouts;
		DescriptorSetCreateInfo.Bindings = Bindings;
		DescriptorSetCreateInfo.Images = &Texture;
		DescriptorSetCreateInfo.DescriptorSet = NULL;
		DescriptorSetCreateInfo.VariableDescriptorSetCount = 0;

		SkyboxDescriptorSet = OpenVkCreateDescriptorSet(&DescriptorSetCreateInfo);
	}
	*/
	{
		uint32_t DescriptorCounts[] = { 1 };
		uint32_t DescriptorTypes[] = { OPENVK_DESCRIPTOR_TYPE_UNIFORM_BUFFER };
		uint32_t UniformBuffers[] = { SSRUniformBuffer };
		size_t UniformSizes[] = { sizeof(SSRUniformBufferObject) };
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

		SSRUniformDescriptorSet = OpenVkCreateDescriptorSet(&DescriptorSetCreateInfo);
	}	
}

void SSRUpdateUniform()
{
	Mutex.lock();
	SSRFragmentUBO.Projection = SceneVertexUBO.Projection;
	SSRFragmentUBO.View = SceneVertexUBO.View;
	
	SSRFragmentUBO.InvProjection = InverseMat4(SceneVertexUBO.Projection);
	SSRFragmentUBO.InvView = InverseMat4(SceneVertexUBO.View);
	Mutex.unlock();

	SSRFragmentUBO.MsaaSamples = MsaaSamples;

	OpenVkUpdateBuffer(sizeof(SSRUniformBufferObject), &SSRFragmentUBO, SSRUniformBuffer);
}

void SSRDraw()
{
	OpenVkSetScissor(0, 0, SceneWidth, SceneHeight);
	OpenVkSetViewport(0, 0, SceneWidth, SceneHeight);

	OpenVkBeginRenderPassInfo BeginInfo;
	BeginInfo.ClearColor[0] = 0.0;
	BeginInfo.ClearColor[1] = 0.0;
	BeginInfo.ClearColor[2] = 0.0;
	BeginInfo.ClearColor[3] = 1.0;
	BeginInfo.ClearColors = 1;
	BeginInfo.ClearDepth = false;
	BeginInfo.RenderPass = SSRRenderPass;
	BeginInfo.Framebuffer = SSRFramebuffer;
	BeginInfo.x = 0;
	BeginInfo.y = 0;
	BeginInfo.Width = SceneWidth;
	BeginInfo.Height = SceneHeight;
	OpenVkBeginRenderPass(&BeginInfo);
	{
		OpenVkBindPipeline(SSRPipeline, OPENVK_PIPELINE_TYPE_GRAPHICS);

		OpenVkBindDescriptorSet(SSRLayout, 0, SceneDescriptorSet, OPENVK_PIPELINE_TYPE_GRAPHICS);
		OpenVkBindDescriptorSet(SSRLayout, 1, SSRUniformDescriptorSet, OPENVK_PIPELINE_TYPE_GRAPHICS);
		OpenVkDrawVertices(0, 3);
	}
	OpenVkEndRenderPass();
}