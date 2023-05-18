void CreateSSAORenderPass()
{
	SSAOColorAttachment = OpenVkCreateColorImageAttachment(SceneWidth, SceneHeight, 1, true, OPENVK_FORMAT_R);

	uint32_t Attachments[] = { OPENVK_ATTACHMENT_COLOR };
	uint32_t AttachmentFormats[] = { OPENVK_FORMAT_R };
	uint32_t MsaaSamples[] = { 1 };
	SSAORenderPass = OpenVkCreateRenderPass(1, Attachments, AttachmentFormats, MsaaSamples, OPENVK_RENDER_PASS_SAMPLED | OPENVK_RENDER_PASS_BOTTOM_OF_PIPE | OPENVK_RENDER_PASS_MEMORY_READ | OPENVK_RENDER_PASS_COLOR_ACCESS_READ_AND_WRITE);
}

void CreateSSAOLayout()
{
	OpenVkPipelineLayoutCreateInfo Layout;
	Layout.PushConstantCount = 0;
	Layout.PushConstantShaderTypes = NULL;
	Layout.PushConstantOffsets = NULL;
	Layout.PushConstantSizes = NULL;
	Layout.DescriptorSetLayoutCount = 1;
	Layout.DescriptorSetLayouts = &SSAODescriptorSetLayout;
	SSAOLayout = OpenVkCreatePipelineLayout(&Layout);
}

void CreateSSAOPipeline()
{
	OpenVkGraphicsPipelineCreateInfo GraphicsPipelineCreateInfo;
	GraphicsPipelineCreateInfo.VertexShader = OpenVkReadFile("Data/Shader/OffscreenVertex.spv");
	GraphicsPipelineCreateInfo.FragmentShader = OpenVkReadFile("Data/Shader/SSAOFragment.spv");
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
	GraphicsPipelineCreateInfo.AlphaBlendings = NULL;
	GraphicsPipelineCreateInfo.ColorBlendAttachments = 1;
	GraphicsPipelineCreateInfo.PipelineLayout = SSAOLayout;
	GraphicsPipelineCreateInfo.DepthStencil = false;
	GraphicsPipelineCreateInfo.RenderPass = SSAORenderPass;
	
	SSAOPipeline = OpenVkCreateGraphicsPipeline(&GraphicsPipelineCreateInfo);
}

void CreateSSAOFramebuffer()
{
	OpenVkFramebufferCreateInfo FramebufferCreateInfo;
	FramebufferCreateInfo.AttachmentCount = 1;
	FramebufferCreateInfo.Attachments = &SSAOColorAttachment;
	FramebufferCreateInfo.RenderPass = SSAORenderPass;
	FramebufferCreateInfo.Width = SceneWidth;
	FramebufferCreateInfo.Height = SceneHeight;
	SSAOFramebuffer = OpenVkCreateFramebuffer(&FramebufferCreateInfo);
}

float lerp(float a, float b, float t)
{
	return a + (b - a) * t;
}

void CreateSSAOUniformBuffer()
{
	SSAOFragmentUniformBuffer = OpenVkCreateUniformBuffer(sizeof(SSAOFragmentUniformBufferObject));
	srand(time(NULL));

	for (uint32_t i = 0; i < SSAO_KERNEL_SIZE; ++i)
	{
		vec3 sample;
		sample.x = ((float)rand() / (float)RAND_MAX) * 2.0f - 1.0f;
		sample.y = ((float)rand() / (float)RAND_MAX) * 2.0f - 1.0f;
		sample.z = ((float)rand() / (float)RAND_MAX);
		Normalize3P(&sample);

		sample.x *= ((float)rand() / (float)RAND_MAX);
		sample.y *= ((float)rand() / (float)RAND_MAX);
		sample.z *= ((float)rand() / (float)RAND_MAX);

		float scale = (float)i / (float)SSAO_KERNEL_SIZE;
		scale = lerp(0.1f, 1.0f, scale * scale);

		SSAOFragmentUBO.Samples[i].x = sample.x * scale;
		SSAOFragmentUBO.Samples[i].y = sample.y * scale;
		SSAOFragmentUBO.Samples[i].z = sample.z * scale;
		SSAOFragmentUBO.Samples[i].w = 0.0f;
	}
}

void CreateSSAONoiseImage()
{
	vec4* SSAONoise = (vec4*)malloc(SSAO_NOISE_DIM * SSAO_NOISE_DIM * sizeof(vec4));

	srand(time(NULL));

	for (uint32_t i = 0; i < SSAO_NOISE_DIM * SSAO_NOISE_DIM; ++i)
	{
		SSAONoise[i].x = ((float)rand() / (float)RAND_MAX) * 2.0f - 1.0f;
		SSAONoise[i].y = ((float)rand() / (float)RAND_MAX) * 2.0f - 1.0f;
		SSAONoise[i].z = 0.0f;
		SSAONoise[i].w = 0.0f;
	}

	OpenVkTextureCreateInfo TextureCreateInfo;
	TextureCreateInfo.Pixels = (unsigned char**)&SSAONoise;
	TextureCreateInfo.Width = SSAO_NOISE_DIM;
	TextureCreateInfo.Height = SSAO_NOISE_DIM;
	TextureCreateInfo.Format = OPENVK_FORMAT_RGBA32F;
	TextureCreateInfo.MipLevels = 1;
	TextureCreateInfo.GenerateMipmaps = OpenVkFalse;
	TextureCreateInfo.UseCustomMipmaps = OpenVkFalse;
	SSAONoiseImage = OpenVkCreateTexture(&TextureCreateInfo);
	SSAONoiseSampler = OpenVkCreateImageSampler(OPENVK_FILTER_NEAREST, OPENVK_ADDRESS_MODE_REPEAT);
	free(SSAONoise);
}

void CreateSSAODescriptorSets()
{
//	{
//		uint32_t DescriptorCounts[] = { 1 };
//		uint32_t DescriptorTypes[] = { OPENVK_DESCRIPTOR_TYPE_UNIFORM_BUFFER };
//		uint32_t UniformBuffers[] = { SSAOFragmentUniformBuffer };
//		size_t UniformSizes[] = { sizeof(SSAOFragmentUniformBufferObject) };
//		uint32_t Bindings[] = { 0 };
//
//		OpenVkDescriptorSetCreateInfo DescriptorSetCreateInfo;
//		DescriptorSetCreateInfo.DescriptorSetLayout = VertexUniformDescriptorSetLayout;
//		DescriptorSetCreateInfo.DescriptorPool = DescriptorPool;
//		DescriptorSetCreateInfo.DescriptorWriteCount = 1;
//		DescriptorSetCreateInfo.DescriptorCounts = DescriptorCounts;
//		DescriptorSetCreateInfo.DescriptorTypes = DescriptorTypes;
//		DescriptorSetCreateInfo.Buffers = UniformBuffers;
//		DescriptorSetCreateInfo.BufferSizes = UniformSizes;
//		DescriptorSetCreateInfo.Bindings = Bindings;
//		DescriptorSetCreateInfo.DescriptorSet = NULL;
//		DescriptorSetCreateInfo.VariableDescriptorSetCount = 0;
//
//		SSAOFragmentUniformDescriptorSet = OpenVkCreateDescriptorSet(&DescriptorSetCreateInfo);
//	}

	{
//		SSAONoiseImage = OpenVkCreateImage

		uint32_t Images[] = { GBufferAttachments[0], GBufferAttachments[1], SSAONoiseImage };
		uint32_t DescriptorCounts[] = { 1, 1, 1, 1 };
		uint32_t DescriptorTypes[] = { OPENVK_DESCRIPTOR_TYPE_IMAGE_SAMPLER, OPENVK_DESCRIPTOR_TYPE_IMAGE_SAMPLER, OPENVK_DESCRIPTOR_TYPE_IMAGE_SAMPLER, OPENVK_DESCRIPTOR_TYPE_UNIFORM_BUFFER };
		uint32_t ImageTypes[] = { OPENVK_IMAGE_TYPE_ATTACHMENT, OPENVK_IMAGE_TYPE_ATTACHMENT, OPENVK_IMAGE_TYPE_TEXTURE };
		uint32_t ImageLayouts[] = { OPENVK_IMAGE_LAYOUT_COLOR_OUTPUT, OPENVK_IMAGE_LAYOUT_COLOR_OUTPUT, OPENVK_IMAGE_LAYOUT_COLOR_OUTPUT };
		uint32_t Bindings[] = { 0, 1, 2, 3 };
		uint32_t Sampler[] = { GBufferSampler, GBufferSampler, SSAONoiseSampler };
		uint32_t UniformBuffers[] = { SSAOFragmentUniformBuffer };
		size_t UniformSizes[] = { sizeof(SSAOFragmentUniformBufferObject) };

		OpenVkDescriptorSetCreateInfo DescriptorSetCreateInfo;
		DescriptorSetCreateInfo.DescriptorSetLayout = SSAODescriptorSetLayout;
		DescriptorSetCreateInfo.DescriptorPool = DescriptorPool;
		DescriptorSetCreateInfo.DescriptorWriteCount = 4;
		DescriptorSetCreateInfo.DescriptorCounts = DescriptorCounts;
		DescriptorSetCreateInfo.DescriptorTypes = DescriptorTypes;
		DescriptorSetCreateInfo.Sampler = Sampler;
		DescriptorSetCreateInfo.ImageTypes = ImageTypes;
		DescriptorSetCreateInfo.Images = Images;
		DescriptorSetCreateInfo.ImageLayouts = ImageLayouts;
		DescriptorSetCreateInfo.Buffers = UniformBuffers;
		DescriptorSetCreateInfo.BufferSizes = UniformSizes;
		DescriptorSetCreateInfo.Bindings = Bindings;
		DescriptorSetCreateInfo.DescriptorSet = NULL;
		DescriptorSetCreateInfo.VariableDescriptorSetCount = 0;

		SSAODescriptorSet = OpenVkCreateDescriptorSet(&DescriptorSetCreateInfo);
	}	
}

void SSAOUpdateUniformBuffer()
{
	SSAOFragmentUBO.Projection = GBufferVertexUBO.Projection;
	OpenVkUpdateBuffer(sizeof(SSAOFragmentUniformBufferObject), &SSAOFragmentUBO, SSAOFragmentUniformBuffer);
}

void SSAODraw()
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
	BeginInfo.RenderPass = SSAORenderPass;
	BeginInfo.Framebuffer = SSAOFramebuffer;
	BeginInfo.x = 0;
	BeginInfo.y = 0;
	BeginInfo.Width = SceneWidth;
	BeginInfo.Height = SceneHeight;
	OpenVkBeginRenderPass(&BeginInfo);
	{
		OpenVkBindPipeline(SSAOPipeline, OPENVK_PIPELINE_TYPE_GRAPHICS);

		OpenVkBindDescriptorSet(SSAOLayout, 0, SSAODescriptorSet, OPENVK_PIPELINE_TYPE_GRAPHICS);
		OpenVkDrawVertices(0, 3);
	}
	OpenVkEndRenderPass();
}