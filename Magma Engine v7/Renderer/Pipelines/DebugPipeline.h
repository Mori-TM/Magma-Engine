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

	OpenVkGraphicsPipelineCreateInfo GraphicsPipelineCreateInfo;
	GraphicsPipelineCreateInfo.VertexShader = OpenVkReadFile("Data/Shader/DebugVertex.spv");
	GraphicsPipelineCreateInfo.FragmentShader = OpenVkReadFile("Data/Shader/DebugFragment.spv");
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
	GraphicsPipelineCreateInfo.LineWidth = 1.0;
	GraphicsPipelineCreateInfo.CullMode = OPENVK_CULL_MODE_BACK;
	GraphicsPipelineCreateInfo.FrontFace = OPENVK_FRONT_FACE_COUNTER_CLOCK_WISE;
	GraphicsPipelineCreateInfo.MsaaSamples = MsaaSamples;
	GraphicsPipelineCreateInfo.AlphaBlending = true;
	GraphicsPipelineCreateInfo.ColorBlendAttachments = 4;
	GraphicsPipelineCreateInfo.PipelineLayout = DebugLayout;
	GraphicsPipelineCreateInfo.DepthStencil = true;
	GraphicsPipelineCreateInfo.RenderPass = SceneRenderPass;

	DebugPipelineThinLine = OpenVkCreateGraphicsPipeline(&GraphicsPipelineCreateInfo);
	
	GraphicsPipelineCreateInfo.LineWidth = 3.0;
	GraphicsPipelineCreateInfo.VertexShader = OpenVkReadFile("Data/Shader/DebugVertex.spv");
	GraphicsPipelineCreateInfo.FragmentShader = OpenVkReadFile("Data/Shader/DebugFragment.spv");
	DebugPipelineFatLine = OpenVkCreateGraphicsPipeline(&GraphicsPipelineCreateInfo);
}

void DebugDraw()
{
	Mutex.lock();
	mat4 PV = MultiplyMat4P(&SceneVertexUBO.Projection, &SceneVertexUBO.View);
	Mutex.unlock();
	DebugVertexPc.PVM = PV;
	
	OpenVkBindPipeline(DebugPipelineFatLine, OPENVK_PIPELINE_TYPE_GRAPHICS);
	OpenVkPushConstant(DebugLayout, OPENVK_SHADER_TYPE_VERTEX, 0, sizeof(DebugVertexPushConstant), &DebugVertexPc);

	OpenVkBindVertexBuffer(DirectionVertexBuffer);
	OpenVkDrawVertices(ARRAY_SIZE(DirectionVertices));
	
	OpenVkBindPipeline(DebugPipelineThinLine, OPENVK_PIPELINE_TYPE_GRAPHICS);
	OpenVkPushConstant(DebugLayout, OPENVK_SHADER_TYPE_VERTEX, 0, sizeof(DebugVertexPushConstant), &DebugVertexPc);
	
	OpenVkBindVertexBuffer(GridVertexBuffer);
	OpenVkDrawVertices(ARRAY_SIZE(GridVertices));
	
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
			OpenVkDrawIndices(ARRAY_SIZE(CameraIndices));
		}
	}
}