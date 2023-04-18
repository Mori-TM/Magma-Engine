void		(*OpenVkDestroyRenderer				)();
void		(*OpenVkDeviceWaitIdle				)();
void		(*OpenVkRecreateSwapChain			)(uint32_t* Width, uint32_t* Height);
uint32_t	(*OpenVkCreateRenderPass			)(uint32_t AttachmentCount, uint32_t* Attachments, uint32_t* AttachmentFormats, uint32_t* MsaaSamples, uint32_t* ResolveAttachments, OpenVkBool Sampled);
uint32_t	(*OpenVkCreateGraphicsPipeline		)(OpenVkGraphicsPipelineCreateInfo* Info);
uint32_t	(*OpenVkCreatePipelineLayout		)(OpenVkPipelineLayoutCreateInfo* Info);
uint32_t	(*OpenVkCreateFramebuffer			)(OpenVkFramebufferCreateInfo* Info);
uint32_t	(*OpenVkCreateDescriptorSetLayout	)(uint32_t BindingCount, uint32_t* Bindings, uint32_t* DescriptorCounts, uint32_t* DescriptorTypes, uint32_t* DescriptorFlags, uint32_t* ShaderTypes);
uint32_t	(*OpenVkCreateDescriptorPool		)(uint32_t DescriptorPoolType, uint32_t PoolSizeCount, uint32_t* DescriptorTypes, uint32_t* DescriptorCounts);
OpenVkBool	(*OpenVkFreeDescriptorSet			)(uint32_t DescriptorPool, uint32_t DescriptorSet);
OpenVkBool	(*OpenVkDestroyDescriptorPool		)(uint32_t DescriptorPool);
uint32_t	(*OpenVkCreateDescriptorSet			)(OpenVkDescriptorSetCreateInfo* Info);
OpenVkBool	(*OpenVkDrawFrame					)(void (*RenderFunc)(void), void (*ResizeFunc)(void), void (*UpdateFunc)(void));
OpenVkBool	(*OpenVkBeginFrame					)();
OpenVkBool	(*OpenVkEndFrame					)();
void		(*OpenVkBeginRenderPass				)(OpenVkBeginRenderPassInfo* Info);
void		(*OpenVkEndRenderPass				)();
uint32_t	(*OpenVkCreateTextureImage			)(unsigned char* Pixels, int32_t Width, int32_t Height, uint32_t Format);
uint32_t	(*OpenVkCreateStorageImage			)(uint32_t Width, uint32_t Height, uint32_t Format);
void		(*OpenVkDestroyImage				)(uint32_t InImage);
OpenVkBool	(*OpenVkCopyImage					)(uint32_t Width, uint32_t Height, uint32_t Src, uint32_t Dst);
uint32_t	(*OpenVkCreateImageSampler			)(uint32_t Filter, uint32_t AddressMode);
void		(*OpenVkDestroySampler				)(uint32_t Sampler);
uint32_t	(*OpenVkCreateColorImageAttachment	)(uint32_t Width, uint32_t Height, uint32_t MsaaSamples, OpenVkBool Sampled, uint32_t Format);
uint32_t	(*OpenVkCreateDepthImageAttachment	)(uint32_t Width, uint32_t Height, uint32_t MsaaSamples, OpenVkBool Sampled, uint32_t Format);
uint32_t	(*OpenVkCreateUniformBuffer			)(size_t Size);
OpenVkBool	(*OpenVkUpdateBuffer				)(size_t Size, const void* BufferData, uint32_t Buffer);
OpenVkBool	(*OpenVkUpdateDynamicBuffer			)(size_t Size, const void* Data, uint32_t Buffer);
uint32_t	(*OpenVkCreateDynamicBuffer			)(size_t Size);
uint32_t	(*OpenVkCreateVertexBuffer			)(size_t Size, const void* Vertices);
uint32_t	(*OpenVkCreateIndexBuffer			)(size_t Size, const void* Indices);
uint32_t	(*OpenVkCreateDynamicVertexBuffer	)(size_t Size);
uint32_t	(*OpenVkCreateDynamicIndexBuffer	)(size_t Size);
void		(*OpenVkDestroyBuffer				)(uint32_t Buffer);
void		(*OpenVkDestroyDynamicBuffer		)(uint32_t Buffer);
void		(*OpenVkBindPipeline				)(uint32_t Pipeline, uint32_t PipelineType);
void		(*OpenVkSetViewport					)(float x, float y, float Width, float Height);
void		(*OpenVkSetScissor					)(int32_t x, int32_t y, uint32_t Width, uint32_t Height);
void		(*OpenVkBindVertexBuffer			)(uint32_t VertexBuffer);
void		(*OpenVkBindIndexBuffer				)(uint32_t VertexBuffer, uint32_t IndexBuffer);
void		(*OpenVkBindDynamicVertexBuffer		)(uint32_t VertexBuffer);
void		(*OpenVkBindDynamicIndexBuffer		)(uint32_t VertexBuffer, uint32_t IndexBuffer);
void		(*OpenVkDrawVertices				)(uint32_t VertexCount);
void		(*OpenVkDrawIndices					)(uint32_t IndexCount);
void		(*OpenVkBindDescriptorSet			)(uint32_t PipelineLayout, uint32_t Set, uint32_t DescriptorSet, uint32_t PipelineType);
void		(*OpenVkPushConstant				)(uint32_t PipelineLayout, uint32_t ShaderType, uint32_t Offset, size_t Size, const void* Data);
//Raytracing
uint32_t	(*OpenVkCreateTranformBuffer		)(OpenVkTransformMatrix Matrix);
uint32_t	(*OpenVkCreateRaytracingGeometry	)(OpenVkRaytracingGeometryCreateInfo* Info);
uint32_t	(*OpenVkCreateBottomLevelAS			)(uint32_t InGeometry, OpenVkBool AllowUpdate, uint32_t* OldBottomLevelAS);
uint32_t	(*OpenVkCreateInstance				)(OpenVkTransformMatrix Matrix, OpenVkBool TriangleFrontCCW, uint32_t BottomLevelAS);
OpenVkBool	(*OpenVkUpdateInstance				)(OpenVkTransformMatrix Matrix, OpenVkBool TriangleFrontCCW, uint32_t BottomLevelAS, uint32_t Instance);
OpenVkBool	(*OpenVkCreateTopLevelAS			)(uint32_t InstanceCount, uint32_t* Instances, OpenVkBool AllowUpdate, uint32_t* OldTopLevelAS);
uint32_t 	(*OpenVkCreateRaytracingPipeline	)(uint32_t MaxPipelineRayRecursionDepth, uint32_t PipelineLayout, uint32_t ShaderCount, uint32_t* ShaderTypes, OpenVkFile* Shader);
uint32_t*	(*OpenVkCreateShaderBindingTable	)(uint32_t Pipeline, uint32_t ShaderCount, uint32_t* HandleCount);
OpenVkBool	(*OpenVkTraceRays					)(OpenVkTraceRaysInfo* Info);


uint32_t OpenVkCreateRenderer(uint32_t RendererFlags, const char** (*GetExtensions)(uint32_t* ExtensionCount), VkSurfaceKHR(*GetSurface)(VkInstance* Instance), void (*GetWindowSize)(uint32_t* Width, uint32_t* Height))
{
	OpenVkRendererFlags = RendererFlags;
	OpenVkRuntimeInfo("Validation Layers: ", (RendererFlags & OPENVK_VALIDATION_LAYER ? "Enabled" : "Disabled"));
	OpenVkRuntimeInfo("Raytracing: ", (RendererFlags & OPENVK_RAYTRACING ? "Enabled" : "Disabled"));

	if (RendererFlags & OPENVK_VULKAN)
	{
		OpenVkRuntimeInfo("Renderer: ", "Vulkan");
		OpenVkDestroyRenderer = VkDestroyRenderer;
		OpenVkDeviceWaitIdle = VkDeviceWaitIdle;
		OpenVkRecreateSwapChain = VkRecreateSwapChain;
		OpenVkCreateRenderPass = VkCreateRenderPass;
		OpenVkCreateGraphicsPipeline = VkCreateGraphicsPipeline;
		OpenVkCreatePipelineLayout = VkCreatePipelineLayout;
		OpenVkCreateFramebuffer = VkCreateFramebuffer;
		OpenVkCreateDescriptorSetLayout = VkCreateDescriptorSetLayout;
		OpenVkCreateDescriptorPool = VkCreateDescriptorPool;
		OpenVkFreeDescriptorSet = VkFreeDescriptorSet;
		OpenVkDestroyDescriptorPool = VkDestroyDescriptorPool;
		OpenVkCreateDescriptorSet = VkCreateDescriptorSet;
		OpenVkDrawFrame = VkDrawFrame;
		OpenVkBeginFrame = VkBeginFrame;
		OpenVkEndFrame = VkEndFrame;
		OpenVkBeginRenderPass = VkBeginRenderPass;
		OpenVkEndRenderPass = VkEndRenderPass;
		OpenVkCreateTextureImage = VkCreateTextureImage;
		OpenVkCreateStorageImage = VkCreateStorageImage;
		OpenVkDestroyImage = VkDestroyImage;
		OpenVkCopyImage = VkCopyImage;
		OpenVkCreateImageSampler = VkCreateImageSampler;
		OpenVkDestroySampler = VkDestroySampler;
		OpenVkCreateColorImageAttachment = VkCreateColorImageAttachment;
		OpenVkCreateDepthImageAttachment = VkCreateDepthImageAttachment;
		OpenVkCreateVertexBuffer = VkCreateVertexBuffer;
		OpenVkCreateIndexBuffer = VkCreateIndexBuffer;
		OpenVkCreateDynamicVertexBuffer = VkCreateDynamicVertexBuffer;
		OpenVkCreateDynamicIndexBuffer = VkCreateDynamicIndexBuffer;
		OpenVkCreateUniformBuffer = VkCreateUniformBuffer;
		OpenVkCreateDynamicBuffer = VkCreateDynamicUniformBuffer;
		OpenVkUpdateBuffer = VkUpdateBuffer;
		OpenVkUpdateDynamicBuffer = VkUpdateDynamicBuffer;
		OpenVkDestroyBuffer = VkDestroyBuffer;
		OpenVkDestroyDynamicBuffer = VkDestroyDynamicBuffer;
		OpenVkBindPipeline = VkBindPipeline;
		OpenVkSetViewport = VkSetViewport;
		OpenVkSetScissor = VkSetScissor;
		OpenVkBindVertexBuffer = VkBindVertexBuffer;
		OpenVkBindIndexBuffer = VkBindIndexBuffer;
		OpenVkBindDynamicVertexBuffer = VkBindDynamicVertexBuffer;
		OpenVkBindDynamicIndexBuffer = VkBindDynamicIndexBuffer;
		OpenVkDrawVertices = VkDrawVertices;
		OpenVkDrawIndices = VkDrawIndices;
		OpenVkBindDescriptorSet = VkBindDescriptorSet;
		OpenVkPushConstant = VkPushConstant;
		if (RendererFlags & OPENVK_RAYTRACING)
		{
			OpenVkCreateTranformBuffer = VkCreateTranformBuffer;
			OpenVkCreateRaytracingGeometry = VkCreateRaytracingGeometry;
			OpenVkCreateBottomLevelAS = VkCreateBottomLevelAS;
			OpenVkCreateInstance = VkCreateInstance;
			OpenVkUpdateInstance = VkUpdateInstance;
			OpenVkCreateTopLevelAS = VkCreateTopLevelAS;
			OpenVkCreateRaytracingPipeline = VkCreateRaytracingPipeline;
			OpenVkCreateShaderBindingTable = VkCreateShaderBindingTable;
			OpenVkTraceRays = VkTraceRays;
		}

		return VkCreateRenderer(GetExtensions, GetSurface, GetWindowSize);
	}
	else if (RendererFlags & OPENVK_DIRECTX_12)
	{
		OpenVkRuntimeInfo("Renderer: ", "DirectX 12");
		return OpenVkRuntimeError("DirectX 12 currently not supported");
	}
	else
	{
		OpenVkRuntimeInfo("Renderer: ", "None");
		return OpenVkRuntimeError("No valid rendering backend selected");
	}
}