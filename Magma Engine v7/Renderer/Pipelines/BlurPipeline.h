void CreateBlurRenderPass()
{
	BlurColorAttachments[0] = OpenVkCreateColorImageAttachment(BLUR_PASS_SIZE, BLUR_PASS_SIZE, 1, true, OPENVK_FORMAT_DEFAULT);
	BlurColorAttachments[1] = OpenVkCreateColorImageAttachment(BLUR_PASS_SIZE, BLUR_PASS_SIZE, 1, true, OPENVK_FORMAT_DEFAULT);

	uint32_t Attachments[] = { OPENVK_ATTACHMENT_COLOR };
	uint32_t AttachmentFormats[] = { OPENVK_FORMAT_DEFAULT };
	uint32_t MsaaSamples[] = { 1 };
	BlurRenderPass = OpenVkCreateRenderPass(1, Attachments, AttachmentFormats, MsaaSamples, NULL, true);
}

void CreateBlurLayout()
{
	uint32_t PushTypes[] = { OPENVK_SHADER_TYPE_FRAGMENT };
	uint32_t PushOffsets[] = { 0 };
	uint32_t PushSizes[] = { sizeof(BlurFragmentPushConstant) };

	OpenVkPipelineLayoutCreateInfo Layout;
	Layout.PushConstantCount = 1;
	Layout.PushConstantShaderTypes = PushTypes;
	Layout.PushConstantOffsets = PushOffsets;
	Layout.PushConstantSizes = PushSizes;
	Layout.DescriptorSetLayoutCount = 1;
	Layout.DescriptorSetLayouts = &TextureDescriptorSetLayout;
	BlurLayout = OpenVkCreatePipelineLayout(&Layout);
}

void CreateBlurPipeline()
{
	OpenVkGraphicsPipelineCreateInfo GraphicsPipelineCreateInfo;
	GraphicsPipelineCreateInfo.VertexShader = OpenVkReadFile("Data/Shader/OffscreenVertex.spv");
	GraphicsPipelineCreateInfo.FragmentShader = OpenVkReadFile("Data/Shader/BlurFragment.spv");
	GraphicsPipelineCreateInfo.BindingStride = 0;
	GraphicsPipelineCreateInfo.ShaderAttributeFormatCount = 0;
	GraphicsPipelineCreateInfo.ShaderAttributeFormats = NULL;
	GraphicsPipelineCreateInfo.ShaderAttributeOffsets = NULL;
	GraphicsPipelineCreateInfo.PrimitiveTopology = OPENVK_PRIMITIVE_TOPOLOGY_TRIANGLE;
	GraphicsPipelineCreateInfo.x = 0;
	GraphicsPipelineCreateInfo.y = 0;
	GraphicsPipelineCreateInfo.Width = BLUR_PASS_SIZE;
	GraphicsPipelineCreateInfo.Height = BLUR_PASS_SIZE;
	GraphicsPipelineCreateInfo.DepthClamp = false;
	GraphicsPipelineCreateInfo.PolygonMode = OPENVK_POLYGON_MODE_FILL;
	GraphicsPipelineCreateInfo.LineWidth = 1.0;
	GraphicsPipelineCreateInfo.CullMode = OPENVK_CULL_MODE_FRONT;
	GraphicsPipelineCreateInfo.FrontFace = OPENVK_FRONT_FACE_COUNTER_CLOCK_WISE;
	GraphicsPipelineCreateInfo.MsaaSamples = 1;
	GraphicsPipelineCreateInfo.AlphaBlending = true;
	GraphicsPipelineCreateInfo.ColorBlendAttachments = 1;
	GraphicsPipelineCreateInfo.PipelineLayout = BlurLayout;
	GraphicsPipelineCreateInfo.DepthStencil = false;
	GraphicsPipelineCreateInfo.RenderPass = BlurRenderPass;

	BlurPipeline = OpenVkCreateGraphicsPipeline(&GraphicsPipelineCreateInfo);
}

void CreateBlurFramebuffers()
{
	for (uint32_t i = 0; i < 2; i++)
	{
		uint32_t Attachments[] = { BlurColorAttachments[i] };

		OpenVkFramebufferCreateInfo FramebufferCreateInfo;
		FramebufferCreateInfo.AttachmentCount = ARRAY_SIZE(Attachments);
		FramebufferCreateInfo.Attachments = Attachments;
		FramebufferCreateInfo.RenderPass = BlurRenderPass;
		FramebufferCreateInfo.Width = BLUR_PASS_SIZE;
		FramebufferCreateInfo.Height = BLUR_PASS_SIZE;
		BlurFramebuffers[i] = OpenVkCreateFramebuffer(&FramebufferCreateInfo);
	}	
}

void CreateBlurDescriptorSets()
{
	for (uint32_t i = 0; i < 2; i++)
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
		DescriptorSetCreateInfo.Images = &BlurColorAttachments[i];
		DescriptorSetCreateInfo.ImageLayouts = ImageLayouts;
		DescriptorSetCreateInfo.Bindings = Bindings;
		DescriptorSetCreateInfo.DescriptorSet = NULL;

		BlurDescriptorSets[i] = OpenVkCreateDescriptorSet(&DescriptorSetCreateInfo);
	}
}

uint32_t BlurDraw(uint32_t DescriptorSet, float BlurScale, float BlurStrength)
{
	OpenVkSetScissor(0, 0, BLUR_PASS_SIZE, BLUR_PASS_SIZE);
	OpenVkSetViewport(0, 0, BLUR_PASS_SIZE, BLUR_PASS_SIZE);

	for (uint32_t i = 0; i < 2; i++)
	{
		OpenVkBeginRenderPassInfo BeginInfo;
		BeginInfo.ClearColor[0] = 0.0;
		BeginInfo.ClearColor[1] = 0.0;
		BeginInfo.ClearColor[2] = 0.0;
		BeginInfo.ClearColor[3] = 1.0;
		BeginInfo.ClearColors = 1;
		BeginInfo.ClearDepth = false;
		BeginInfo.RenderPass = BlurRenderPass;
		BeginInfo.Framebuffer = BlurFramebuffers[i];
		BeginInfo.x = 0;
		BeginInfo.y = 0;
		BeginInfo.Width = BLUR_PASS_SIZE;
		BeginInfo.Height = BLUR_PASS_SIZE;
		OpenVkBeginRenderPass(&BeginInfo);
		{
			OpenVkBindPipeline(BlurPipeline, OPENVK_PIPELINE_TYPE_GRAPHICS);
			
			if (i == 1)
			{
				OpenVkBindDescriptorSet(BlurLayout, 0, BlurDescriptorSets[0], OPENVK_PIPELINE_TYPE_GRAPHICS);
			}
			else
				OpenVkBindDescriptorSet(BlurLayout, 0, DescriptorSet, OPENVK_PIPELINE_TYPE_GRAPHICS);
			
			BlurFragmentPc.Horizontal = i;
			BlurFragmentPc.BlurScale = BlurScale;
			BlurFragmentPc.BlurStrength = BlurStrength;
			OpenVkPushConstant(BlurLayout, OPENVK_SHADER_TYPE_FRAGMENT, 0, sizeof(BlurFragmentPushConstant), &BlurFragmentPc);

			OpenVkDrawVertices(3);
		}
		OpenVkEndRenderPass();
	}

	return BlurDescriptorSets[1];
}