void CreateSwapChainRenderPass()
{
	SwapChainRenderPass = OpenVkCreateRenderPass(1, false, false, 1, false);
}

void CreateSwapChainPipeline()
{
	uint32_t ShaderAttributeFormats[] = { 2, 2 };
	uint32_t ShaderAttributeOffsets[] = { 0, 8 };

	OpenVkGraphicsPipelineCreateInfo GraphicsPipelineCreateInfo;
	GraphicsPipelineCreateInfo.VertexPath = "Data/Shader/SwapChainVertex.spv";
	GraphicsPipelineCreateInfo.FragmentPath = "Data/Shader/SwapChainFragment.spv";
	GraphicsPipelineCreateInfo.BindingStride = sizeof(QuadVertex);
	GraphicsPipelineCreateInfo.ShaderAttributeFormatCount = 2;
	GraphicsPipelineCreateInfo.ShaderAttributeFormats = ShaderAttributeFormats;
	GraphicsPipelineCreateInfo.ShaderAttributeOffsets = ShaderAttributeOffsets;
	GraphicsPipelineCreateInfo.PrimitiveTopology = OPENVK_PRIMITIVE_TOPOLOGY_TRIANGLE;
	GraphicsPipelineCreateInfo.x = 0;
	GraphicsPipelineCreateInfo.y = 0;
	GraphicsPipelineCreateInfo.Width = WindowWidth;
	GraphicsPipelineCreateInfo.Height = WindowHeight;
	GraphicsPipelineCreateInfo.DepthClamp = false;
	GraphicsPipelineCreateInfo.PolygonMode = OPENVK_POLYGON_MODE_FILL;
	GraphicsPipelineCreateInfo.LineWidth = 3.0;
	GraphicsPipelineCreateInfo.CullMode = OPENVK_CULL_MODE_FRONT;
	GraphicsPipelineCreateInfo.FrontFace = OPENVK_FRONT_FACE_COUNTER_CLOCK_WISE;
	GraphicsPipelineCreateInfo.MsaaSamples = 1;
	GraphicsPipelineCreateInfo.AlphaBlending = false;
	GraphicsPipelineCreateInfo.ColorBlendAttachments = 1;
	GraphicsPipelineCreateInfo.PushConstantCount = 0;
	GraphicsPipelineCreateInfo.PushConstantShaderTypes = NULL;
	GraphicsPipelineCreateInfo.PushConstantOffsets = NULL;
	GraphicsPipelineCreateInfo.PushConstantSizes = NULL;
	GraphicsPipelineCreateInfo.DescriptorSetLayoutCount = 1;
	GraphicsPipelineCreateInfo.DescriptorSetLayouts = &TextureDescriptorSetLayout;
	GraphicsPipelineCreateInfo.DepthStencil = false;
	GraphicsPipelineCreateInfo.RenderPass = SwapChainRenderPass;

	SwapChainPipeline = OpenVkCreateGraphicsPipeline(&GraphicsPipelineCreateInfo);
}

void CreateSwapChainFramebuffer()
{
	uint32_t Attachments[] = { SwapChain };

	OpenVkFramebufferCreateInfo FramebufferCreateInfo;
	FramebufferCreateInfo.AttachmentCount = ARRAY_SIZE(Attachments);
	FramebufferCreateInfo.Attachments = Attachments;
	FramebufferCreateInfo.RenderPass = SwapChainRenderPass;
	FramebufferCreateInfo.Width = WindowWidth;
	FramebufferCreateInfo.Height = WindowHeight;
	SwapChainFramebuffer = OpenVkCreateFramebuffer(&FramebufferCreateInfo);
}

void SwapChainDraw()
{
	OpenVkBeginRenderPassInfo BeginInfo;
	BeginInfo.ClearColor[0] = 0.0;
	BeginInfo.ClearColor[1] = 0.0;
	BeginInfo.ClearColor[2] = 0.0;
	BeginInfo.ClearColor[3] = 1.0;
	BeginInfo.ClearColors = 1;
	BeginInfo.ClearDepth = false;
	BeginInfo.RenderPass = SwapChainRenderPass;
	BeginInfo.Framebuffer = SwapChainFramebuffer;
	BeginInfo.x = 0;
	BeginInfo.y = 0;
	BeginInfo.Width = WindowWidth;
	BeginInfo.Height = WindowHeight;
	OpenVkBeginRenderPass(&BeginInfo);
	{
		OpenVkSetScissor(0, 0, WindowWidth, WindowHeight);
		OpenVkSetViewport(0, 0, WindowWidth, WindowHeight);

	//	OpenVkBindGraphicsPipeline(SwapChainPipeline);
	//
	//	OpenVkBindDescriptorSet(SwapChainPipeline, 0, SceneDescriptorSet);
	//	OpenVkBindIndexBuffer(QuadVertexBuffer, QuadIndexBuffer);
	//	OpenVkDrawIndices(ARRAY_SIZE(QuadIndices));
		
	 	EngineDrawEditor();
		ImGui::ShowDemoWindow();

		ImGui::Render();

		ImTextureID NonAlphaTextures[1] =
		{
			(ImTextureID*)&GetDescriptorSet(SceneDescriptorSet)[VkRenderer.CurrentFrame]
		};

		ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), VkRenderer.CommandBuffers[VkRenderer.ImageIndex], 1, NonAlphaTextures);
	}
	OpenVkEndRenderPass();
}