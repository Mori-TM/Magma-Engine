void CreateFXAARenderPass()
{
	FXAAColorAttachment = OpenVkCreateColorImageAttachment(SceneWidth, SceneHeight, 1, true, OPENVK_FORMAT_RGBA);

	uint32_t Attachments[] = { OPENVK_ATTACHMENT_COLOR };
	uint32_t AttachmentFormats[] = { OPENVK_FORMAT_RGBA };
	uint32_t MsaaSamples[] = { 1 };
	FXAARenderPass = OpenVkCreateRenderPass(1, Attachments, AttachmentFormats, MsaaSamples, OPENVK_RENDER_PASS_SAMPLED);
}

void CreateFXAALayout()
{
	OpenVkPipelineLayoutCreateInfo Layout;
	Layout.PushConstantCount = 0;
	Layout.PushConstantShaderTypes = NULL;
	Layout.PushConstantOffsets = NULL;
	Layout.PushConstantSizes = NULL;
	Layout.DescriptorSetLayoutCount = 1;
	Layout.DescriptorSetLayouts = &TextureDescriptorSetLayout;
	FXAALayout = OpenVkCreatePipelineLayout(&Layout);
}

void CreateFXAAPipeline()
{
	OpenVkGraphicsPipelineCreateInfo GraphicsPipelineCreateInfo;
	GraphicsPipelineCreateInfo.VertexShader = OpenVkReadFile("Data/Shader/OffscreenVertex.spv");
	GraphicsPipelineCreateInfo.FragmentShader = OpenVkReadFile("Data/Shader/FXAAFragment.spv");
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
	GraphicsPipelineCreateInfo.PipelineLayout = FXAALayout;
	GraphicsPipelineCreateInfo.DepthStencil = false;
	GraphicsPipelineCreateInfo.RenderPass = FXAARenderPass;

	FXAAPipeline = OpenVkCreateGraphicsPipeline(&GraphicsPipelineCreateInfo);
}

void CreateFXAAFramebuffer()
{
	OpenVkFramebufferCreateInfo FramebufferCreateInfo;
	FramebufferCreateInfo.AttachmentCount = 1;
	FramebufferCreateInfo.Attachments = &FXAAColorAttachment;
	FramebufferCreateInfo.RenderPass = FXAARenderPass;
	FramebufferCreateInfo.Width = SceneWidth;
	FramebufferCreateInfo.Height = SceneHeight;
	FXAAFramebuffer = OpenVkCreateFramebuffer(&FramebufferCreateInfo);
}

void CreateFXAADescriptorSet()
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
	DescriptorSetCreateInfo.Images = &FXAAColorAttachment;
	DescriptorSetCreateInfo.ImageLayouts = ImageLayouts;
	DescriptorSetCreateInfo.Bindings = Bindings;
	DescriptorSetCreateInfo.DescriptorSet = NULL;
	DescriptorSetCreateInfo.VariableDescriptorSetCount = 0;

	FXAADescriptorSet = OpenVkCreateDescriptorSet(&DescriptorSetCreateInfo);
}

void FXAADraw()
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
	BeginInfo.RenderPass = FXAARenderPass;
	BeginInfo.Framebuffer = FXAAFramebuffer;
	BeginInfo.x = 0;
	BeginInfo.y = 0;
	BeginInfo.Width = SceneWidth;
	BeginInfo.Height = SceneHeight;
	OpenVkBeginRenderPass(&BeginInfo);
	{
		OpenVkBindPipeline(FXAAPipeline, OPENVK_PIPELINE_TYPE_GRAPHICS);

		OpenVkBindDescriptorSet(FXAALayout, 0, SSROutputDescriptorSet, OPENVK_PIPELINE_TYPE_GRAPHICS);

		OpenVkDrawVertices(0, 3);
	}
	OpenVkEndRenderPass();
}