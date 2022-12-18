void CreateSwapChainRenderPass()
{
	uint32_t ColorFormats[] = { OPENVK_FORMAT_DEFAULT };
	SwapChainRenderPass = OpenVkCreateRenderPass(1, ColorFormats, false, 0, false, 1, false);
}

void CreateSwapChainLayout()
{
	OpenVkPipelineLayoutCreateInfo Layout;
	Layout.PushConstantCount = 0;
	Layout.PushConstantShaderTypes = NULL;
	Layout.PushConstantOffsets = NULL;
	Layout.PushConstantSizes = NULL;
	Layout.DescriptorSetLayoutCount = 1;
	Layout.DescriptorSetLayouts = &TextureDescriptorSetLayout;
	SwapChainLayout = OpenVkCreatePipelineLayout(&Layout);
}

void CreateSwapChainPipeline()
{
	uint32_t ShaderAttributeFormats[] = { 2, 2 };
	uint32_t ShaderAttributeOffsets[] = { 0, 8 };

	OpenVkGraphicsPipelineCreateInfo GraphicsPipelineCreateInfo;
	GraphicsPipelineCreateInfo.VertexShader = OpenVkReadFile("Data/Shader/SwapChainVertex.spv");
	GraphicsPipelineCreateInfo.FragmentShader = OpenVkReadFile("Data/Shader/SwapChainFragment.spv");
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
	GraphicsPipelineCreateInfo.PipelineLayout = SwapChainLayout;
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
	BeginInfo.ClearColor[0] = 0.4;
	BeginInfo.ClearColor[1] = 0.4;
	BeginInfo.ClearColor[2] = 0.4;
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

		OpenVkGUIBeginRender(WindowWidth, WindowHeight);
		{
			char String[2048];
			sprintf(String, "FPS: %f\nMS: %f\nSubscribe\nPatreon!", FPS, MS);
			OpenVkGUITextPos(String, ImGuiScenePosX + 20, ImGuiScenePosY, 0, 0);

			OpenVkGUIBegin("OpenVk Window");
			{
				static OpenVkBool Selected = OpenVkTrue;
				if (!OpenVkGUICheckbox("Rounding", &Selected))
					OpenVkGUI.Rounding = 0.0;

				static OpenVkBool Selected2 = OpenVkFalse;
				OpenVkGUICheckbox("Check Box 2", &Selected2);

				OpenVkGUIButton("Button");
				OpenVkGUIText(String);				
				OpenVkGUIButton("Button2");
			//	static float Border = 13.2;
			//	OpenVkGUISlider("Border Size", 0.0, 80.0, &Border);				
			//	OpenVkGUI.Border = Border;

				OpenVkGUISlider("Rounding", 0.0, 100.0, &OpenVkGUI.Rounding);
				OpenVkGUISlider("Window Color", 0.0, 1.0, &OpenVkGUI.WindowColor);
				OpenVkGUISlider("Color", 0.0, 1.0, &OpenVkGUI.Color);
				OpenVkGUISlider("Color Selected", 0.0, 1.0, &OpenVkGUI.ColorSelected);
				OpenVkGUISlider("Color Hovered", 0.0, 1.0, &OpenVkGUI.ColorHovered);
				float Offset = OpenVkGUI.Offset;
				OpenVkGUISlider("Offset Size", 0.0, 30.0, &Offset);
				OpenVkGUI.Offset = Offset;

				OpenVkGUIText("Some Text\nMultiline Text");
			}
			OpenVkGUIEnd();

		//	OpenVkGUIBegin("Another Window");
		//	{
		//
		//	}
		//	OpenVkGUIEnd();
		}
		OpenVkGUIEndRender();
	}
	OpenVkEndRenderPass();
}