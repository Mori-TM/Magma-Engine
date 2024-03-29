void CreateDebugLayout()
{
	uint32_t PushTypes[] = { OPENVK_SHADER_TYPE_VERTEX };
	uint32_t PushOffsets[] = { 0 };
	uint32_t PushSizes[] = { sizeof(DebugVertexPushConstant) };

	OpenVkPipelineLayoutCreateInfo Layout;
	Layout.PushConstantCount = 1;
	Layout.PushConstantShaderTypes = PushTypes;
	Layout.PushConstantOffsets = PushOffsets;
	Layout.PushConstantSizes = PushSizes;
	Layout.DescriptorSetLayoutCount = 0;
	Layout.DescriptorSetLayouts = NULL;
	DebugLayout = OpenVkCreatePipelineLayout(&Layout);
}

void CreateDebugPipeline()
{
	uint32_t ShaderAttributeFormats[] = { OPENVK_FORMAT_RGB32F, OPENVK_FORMAT_RGB32F };
	uint32_t ShaderAttributeOffsets[] = { 0, 12 };

	OpenVkFile VertexShader = OpenVkReadFile("Data/Shader/DebugVertex.spv");
	OpenVkFile FragmentShader = OpenVkReadFile("Data/Shader/DebugFragment.spv");
	VertexShader.Freeable = OpenVkFalse;
	FragmentShader.Freeable = OpenVkFalse;

	OpenVkGraphicsPipelineCreateInfo GraphicsPipelineCreateInfo;
	GraphicsPipelineCreateInfo.VertexShader = VertexShader;
	GraphicsPipelineCreateInfo.FragmentShader = FragmentShader;
	GraphicsPipelineCreateInfo.BindingStride = sizeof(DebugVertex);
	GraphicsPipelineCreateInfo.ShaderAttributeFormatCount = 2;
	GraphicsPipelineCreateInfo.ShaderAttributeFormats = ShaderAttributeFormats;
	GraphicsPipelineCreateInfo.ShaderAttributeOffsets = ShaderAttributeOffsets;
	GraphicsPipelineCreateInfo.PrimitiveTopology = OPENVK_PRIMITIVE_TOPOLOGY_LINE;
	GraphicsPipelineCreateInfo.x = 0;
	GraphicsPipelineCreateInfo.y = 0;
	GraphicsPipelineCreateInfo.Width = SceneWidth;
	GraphicsPipelineCreateInfo.Height = SceneHeight;
	GraphicsPipelineCreateInfo.DepthClamp = false;
	GraphicsPipelineCreateInfo.PolygonMode = OPENVK_POLYGON_MODE_FILL;
	GraphicsPipelineCreateInfo.LineWidth = 1.5;
	GraphicsPipelineCreateInfo.CullMode = OPENVK_CULL_MODE_BACK;
	GraphicsPipelineCreateInfo.FrontFace = OPENVK_FRONT_FACE_COUNTER_CLOCK_WISE;
	GraphicsPipelineCreateInfo.MsaaSamples = 1;
	GraphicsPipelineCreateInfo.AlphaBlendings = NULL;
	GraphicsPipelineCreateInfo.ColorBlendAttachments = G_BUFFER_ATTACHMENT_COUNT - 1;
	GraphicsPipelineCreateInfo.PipelineLayout = DebugLayout;
	GraphicsPipelineCreateInfo.DepthStencil = true;
	GraphicsPipelineCreateInfo.RenderPass = GBufferRenderPass;

	DebugPipelineThinLine = OpenVkCreateGraphicsPipeline(&GraphicsPipelineCreateInfo);
	
	GraphicsPipelineCreateInfo.LineWidth = 3.0;
	GraphicsPipelineCreateInfo.VertexShader = VertexShader;
	GraphicsPipelineCreateInfo.FragmentShader = FragmentShader;
	DebugPipelineFatLine = OpenVkCreateGraphicsPipeline(&GraphicsPipelineCreateInfo);

	free(FragmentShader.Data);
	free(VertexShader.Data);
}

void CreateDebugDescriptorSets()
{
	uint32_t Attachments[] =
	{
		ShadowDepthAttachment,
		GBufferAttachments[0],
		GBufferAttachments[1],
		GBufferAttachments[2],
		GBufferAttachments[3],
		GBufferAttachments[4],
		GBufferAttachments[5],
		SSAOColorAttachment,
		SSAOBlurColorAttachment,
		SceneAttachment,
		SSRColorAttachment,
		FXAAColorAttachment,
	};

	for (uint32_t i = 0; i < ARRAY_SIZE(Attachments); i++)
	{
		uint32_t DescriptorCounts[] = { 1 };
		uint32_t DescriptorTypes[] = { OPENVK_DESCRIPTOR_TYPE_IMAGE_SAMPLER };
		uint32_t ImageTypes[] = { OPENVK_IMAGE_TYPE_ATTACHMENT };
		uint32_t ImageLayouts[] = {i == 0 ? OPENVK_IMAGE_LAYOUT_DEPTH_OUTPUT : OPENVK_IMAGE_LAYOUT_COLOR_OUTPUT};
		uint32_t Bindings[] = { 0 };

		OpenVkDescriptorSetCreateInfo DescriptorSetCreateInfo;
		DescriptorSetCreateInfo.DescriptorSetLayout = TextureDescriptorSetLayout;
		DescriptorSetCreateInfo.DescriptorPool = DescriptorPool;
		DescriptorSetCreateInfo.DescriptorWriteCount = 1;
		DescriptorSetCreateInfo.DescriptorCounts = DescriptorCounts;
		DescriptorSetCreateInfo.DescriptorTypes = DescriptorTypes;
		DescriptorSetCreateInfo.Sampler = &ImageSampler;
		DescriptorSetCreateInfo.ImageTypes = ImageTypes;
		DescriptorSetCreateInfo.Images = &Attachments[i];
		DescriptorSetCreateInfo.ImageLayouts = ImageLayouts;
		DescriptorSetCreateInfo.Bindings = Bindings;
		DescriptorSetCreateInfo.DescriptorSet = NULL;
		DescriptorSetCreateInfo.VariableDescriptorSetCount = 0;

		DebugDescriptorSets[i] = OpenVkCreateDescriptorSet(&DescriptorSetCreateInfo);
	}
}

void DebugDraw()
{
	Mutex.lock();
	mat4 PV = MultiplyMat4P(&GBufferVertexUBO.Projection, &GBufferVertexUBO.View);
	Mutex.unlock();
	DebugVertexPc.PVM = PV;
	
	OpenVkBindPipeline(DebugPipelineThinLine, OPENVK_PIPELINE_TYPE_GRAPHICS);
	OpenVkPushConstant(DebugLayout, OPENVK_SHADER_TYPE_VERTEX, 0, sizeof(DebugVertexPushConstant), &DebugVertexPc);

	OpenVkBindVertexBuffer(GridVertexBuffer);
	OpenVkDrawVertices(0, ARRAY_SIZE(GridVertices));

	OpenVkBindPipeline(DebugPipelineFatLine, OPENVK_PIPELINE_TYPE_GRAPHICS);
	OpenVkPushConstant(DebugLayout, OPENVK_SHADER_TYPE_VERTEX, 0, sizeof(DebugVertexPushConstant), &DebugVertexPc);

	OpenVkBindVertexBuffer(DirectionVertexBuffer);
	OpenVkDrawVertices(0, ARRAY_SIZE(DirectionVertices));	
	
	OpenVkBindPipeline(DebugPipelineThinLine, OPENVK_PIPELINE_TYPE_GRAPHICS);
	OpenVkBindIndexBuffer(CameraVertexBuffer, CameraIndexBuffer);
	for (uint32_t i = 0; i < EntityCount; i++)
	{
		if (Entities[i].UsedComponents[COMPONENT_TYPE_CAMERA])
		{
			mat4 Model = LoadMat4Identity();
			Model = ScaleMat4(Model, Vec3(0.2, 0.2, 0.2));
			Model = RotateXMat4P(&Model, ToRadians(Entities[i].Rotate.x - 90.0));
			Model = RotateYMat4P(&Model, ToRadians(Entities[i].Rotate.y));
			Model = RotateZMat4P(&Model, ToRadians(Entities[i].Rotate.z));
			Model = TranslateMat4P(&Model, &Entities[i].Translate);
			DebugVertexPc.PVM = MultiplyMat4P(&PV, &Model);

			OpenVkPushConstant(DebugLayout, OPENVK_SHADER_TYPE_VERTEX, 0, sizeof(DebugVertexPushConstant), &DebugVertexPc);
			OpenVkDrawIndices(0, ARRAY_SIZE(CameraIndices), 0);
		}
	}
}