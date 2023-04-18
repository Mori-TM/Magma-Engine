void CreateSwapChainRenderPass()
{
	uint32_t Attachments[] = { OPENVK_ATTACHMENT_COLOR };
	uint32_t AttachmentFormats[] = { OPENVK_FORMAT_DEFAULT };
	uint32_t MsaaSamples[] = { 1 };
	SwapChainRenderPass = OpenVkCreateRenderPass(1, Attachments, AttachmentFormats, MsaaSamples, NULL, OpenVkFalse);
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
	OpenVkGraphicsPipelineCreateInfo GraphicsPipelineCreateInfo;
	GraphicsPipelineCreateInfo.VertexShader = OpenVkReadFile("Data/Shader/OffscreenVertex.spv");
	GraphicsPipelineCreateInfo.FragmentShader = OpenVkReadFile("Data/Shader/SwapChainFragment.spv");
	GraphicsPipelineCreateInfo.BindingStride = 0;
	GraphicsPipelineCreateInfo.ShaderAttributeFormatCount = 0;
	GraphicsPipelineCreateInfo.ShaderAttributeFormats = NULL;
	GraphicsPipelineCreateInfo.ShaderAttributeOffsets = NULL;
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
		MeasureFPS();

		if (RenderGamePreview)
		{
			ImGuiScenePosX = 0;
			ImGuiScenePosY = 0;

			OpenVkSetScissor(0, 0, WindowWidth, WindowHeight);
			OpenVkSetViewport(0, 0, WindowWidth, WindowHeight);

			OpenVkBindPipeline(SwapChainPipeline, OPENVK_PIPELINE_TYPE_GRAPHICS);
			OpenVkBindDescriptorSet(SwapChainLayout, 0, SSRDescriptorSet, OPENVK_PIPELINE_TYPE_GRAPHICS);
			OpenVkDrawVertices(3);
		}
		else
		{
			EngineDrawEditor();
			//	ImGui::ShowDemoWindow();

			ImGui::Render();

			ImTextureID NonAlphaTextures[1] =
			{
				(ImTextureID*)&GetDescriptorSet(SSRDescriptorSet)[VkRenderer.CurrentFrame]
			};

			ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), VkRenderer.CommandBuffers[VkRenderer.ImageIndex], 1, NonAlphaTextures);

		}	
	 	
		OpenVkGUIBeginRender(WindowWidth, WindowHeight);
		{
			char String[2048];
			sprintf(String, "%f FPS\n%f MS", FPS, MS);
			OpenVkGUITextPos(String, ImGuiScenePosX + 20, ImGuiScenePosY + 20, 0, 0);
			
		//	OpenVkGUIBegin("OpenVk Window");
		//	{
		//		static OpenVkBool Selected = OpenVkTrue;
		//		if (!OpenVkGUICheckbox("Rounding", &Selected))
		//			OpenVkGUI.Rounding = 0.0;
		//
		//		static OpenVkBool Selected2 = OpenVkFalse;
		//		OpenVkGUICheckbox("Check Box 2", &Selected2);
		//
		//		OpenVkGUIButton("Button");
		//		OpenVkGUIText(String);				
		//		OpenVkGUIButton("Button2");
		//	//	static float Border = 13.2;
		//	//	OpenVkGUISlider("Border Size", 0.0, 80.0, &Border);				
		//	//	OpenVkGUI.Border = Border;
		//
		//		OpenVkGUISlider("Rounding", 0.0, 100.0, &OpenVkGUI.Rounding);
		//		OpenVkGUISlider("Window Color", 0.0, 1.0, &OpenVkGUI.WindowColor);
		//		OpenVkGUISlider("Color", 0.0, 1.0, &OpenVkGUI.Color);
		//		OpenVkGUISlider("Color Selected", 0.0, 1.0, &OpenVkGUI.ColorSelected);
		//		OpenVkGUISlider("Color Hovered", 0.0, 1.0, &OpenVkGUI.ColorHovered);
		//		float Offset = OpenVkGUI.Offset;
		//		OpenVkGUISlider("Offset Size", 0.0, 30.0, &Offset);
		//		OpenVkGUI.Offset = Offset;
		//
		//		OpenVkGUIText("Some Text\nMultiline Text");
		//	}
		//	OpenVkGUIEnd();
		//
		//	OpenVkGUIBegin("Another Window");
		//	{
		//		OpenVkGUIText(String);
		//		static OpenVkBool Selected2 = OpenVkFalse;
		//		OpenVkGUICheckbox("Check Box", &Selected2);
		//		static float Dummy = 2.0;
		//		OpenVkGUISlider("Offset Size", 0.0, 30.0, &Dummy);
		//		OpenVkGUIDragSlider("Drag Slider", 0.0, 30.0, &Dummy);
		//	}
		//	OpenVkGUIEnd();
		}
		OpenVkGUIEndRender();
		
	}
	OpenVkEndRenderPass();
}