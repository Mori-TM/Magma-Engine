void		(*OpenVkDestroyRenderer				)();
void		(*OpenVkRecreateSwapChain			)(uint32_t* Width, uint32_t* Height);
uint32_t	(*OpenVkCreateRenderPass			)(uint32_t ColorAttachmentCount, uint32_t* ColorFormats, OpenVkBool DepthAttachment, uint32_t DepthFormat, OpenVkBool MsaaAttachment, uint32_t MsaaSamples, OpenVkBool Sampled);
uint32_t	(*OpenVkCreateGraphicsPipeline		)(OpenVkGraphicsPipelineCreateInfo* Info);
uint32_t	(*OpenVkCreatePipelineLayout		)(OpenVkPipelineLayoutCreateInfo* Info);
uint32_t	(*OpenVkCreateFramebuffer			)(OpenVkFramebufferCreateInfo* Info);
uint32_t	(*OpenVkCreateDescriptorSetLayout	)(uint32_t BindingCount, uint32_t* Bindings, uint32_t* DescriptorCounts, uint32_t* DescriptorTypes, uint32_t* ShaderTypes);
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
/*
void		(*OpenVkDestroyRenderer)();
void		(*OpenVkRecreateSwapChain)(uint32_t* Width, uint32_t* Height);
uint32_t	(*OpenVkCreateRenderPass)(uint32_t ColorAttachmentCount, uint32_t* ColorFormats, OpenVkBool DepthAttachment, uint32_t DepthFormat, OpenVkBool MsaaAttachment, uint32_t MsaaSamples, OpenVkBool Sampled);
uint32_t	(*OpenVkCreateGraphicsPipeline)(OpenVkGraphicsPipelineCreateInfo* Info);
uint32_t	(*OpenVkCreatePipelineLayout)(OpenVkPipelineLayoutCreateInfo* Info);
uint32_t	(*OpenVkCreateFramebuffer)(OpenVkFramebufferCreateInfo* Info);
uint32_t	(*OpenVkCreateDescriptorSetLayout)(uint32_t BindingCount, uint32_t* Bindings, uint32_t* DescriptorCounts, uint32_t* DescriptorTypes, uint32_t* ShaderTypes);
uint32_t	(*OpenVkCreateDescriptorPool)(uint32_t DescriptorPoolType, uint32_t PoolSizeCount, uint32_t* DescriptorTypes, uint32_t* DescriptorCounts);
OpenVkBool	(*OpenVkFreeDescriptorSet)(uint32_t DescriptorPool, uint32_t DescriptorSet);
OpenVkBool	(*OpenVkDestroyDescriptorPool)(uint32_t DescriptorPool);
uint32_t	(*OpenVkCreateDescriptorSet)(OpenVkDescriptorSetCreateInfo* Info);
OpenVkBool	(*OpenVkDrawFrame)(void (*RenderFunc)(void), void (*ResizeFunc)(void), void (*UpdateFunc)(void));
OpenVkBool	(*OpenVkBeginFrame)();
OpenVkBool	(*OpenVkEndFrame)();
void		(*OpenVkBeginRenderPass)(OpenVkBeginRenderPassInfo* Info);
void		(*OpenVkEndRenderPass)();
uint32_t	(*OpenVkCreateTextureImage)(unsigned char* Pixels, int32_t Width, int32_t Height, uint32_t Format);
uint32_t	(*OpenVkCreateStorageImage)(uint32_t Width, uint32_t Height, uint32_t Format);
void		(*OpenVkDestroyImage)(uint32_t InImage);
OpenVkBool	(*OpenVkCopyImage)(uint32_t Width, uint32_t Height, uint32_t Src, uint32_t Dst);
uint32_t	(*OpenVkCreateImageSampler)(uint32_t Filter, uint32_t AddressMode);
void		(*OpenVkDestroySampler				)(uint32_t Sampler);
uint32_t	(*OpenVkCreateColorImageAttachment	)(uint32_t Width, uint32_t Height, uint32_t MsaaSamples, OpenVkBool Sampled, uint32_t Format);
uint32_t	(*OpenVkCreateDepthImageAttachment	)(uint32_t Width, uint32_t Height, uint32_t MsaaSamples, OpenVkBool Sampled, uint32_t Format);
uint32_t	(*OpenVkCreateVertexBuffer)(size_t Size, const void* Vertices);
uint32_t	(*OpenVkCreateIndexBuffer)(size_t Size, const void* Indices);
uint32_t	(*OpenVkCreateDynamicVertexBuffer)(size_t Size);
uint32_t	(*OpenVkCreateDynamicIndexBuffer)(size_t Size);
uint32_t	(*OpenVkCreateUniformBuffer)(size_t Size);
uint32_t	(*OpenVkCreateDynamicUniformBuffer)(size_t Size);
OpenVkBool	(*OpenVkUpdateBuffer)(size_t Size, const void* BufferData, uint32_t Buffer);
OpenVkBool	(*OpenVkUpdateDynamicBuffer)(size_t Size, const void* Data, uint32_t Buffer);
void		(*OpenVkDestroyBuffer)(uint32_t Buffer);
void		(*OpenVkBindPipeline)(uint32_t Pipeline, uint32_t PipelineType);
void		(*OpenVkSetViewport)(float x, float y, float Width, float Height);
void		(*OpenVkSetScissor)(int32_t x, int32_t y, uint32_t Width, uint32_t Height);
void		(*OpenVkBindVertexBuffer)(uint32_t VertexBuffer);
void		(*OpenVkBindIndexBuffer)(uint32_t VertexBuffer, uint32_t IndexBuffer);
void		(*OpenVkBindDynamicVertexBuffer)(uint32_t VertexBuffer);
void		(*OpenVkBindDynamicIndexBuffer)(uint32_t VertexBuffer, uint32_t IndexBuffer);
void		(*OpenVkDrawVertices)(uint32_t VertexCount);
void		(*OpenVkDrawIndices)(uint32_t IndexCount);
void		(*OpenVkBindDescriptorSet)(uint32_t PipelineLayout, uint32_t Set, uint32_t DescriptorSet, uint32_t PipelineType);
void		(*OpenVkPushConstant)(uint32_t PipelineLayout, uint32_t ShaderType, uint32_t Offset, size_t Size, const void* Data);
*/
extern inline uint32_t OpenVkCreateRenderer(uint32_t RendererFlags, const char** (*GetExtensions)(uint32_t* ExtensionCount), VkSurfaceKHR(*GetSurface)(VkInstance* Instance), void (*GetWindowSize)(uint32_t* Width, uint32_t* Height))
{
	OpenVkRendererFlags = RendererFlags;
	OpenVkRuntimeInfo("Validation Layers: ", (RendererFlags & OPENVK_VALIDATION_LAYER ? "Enabled" : "Disabled"));
	OpenVkRuntimeInfo("Raytracing: ", (RendererFlags & OPENVK_RAYTRACING ? "Enabled" : "Disabled"));

	if (RendererFlags & OPENVK_VULKAN)
	{
		OpenVkRuntimeInfo("Renderer: ", "Vulkan");
		OpenVkDestroyRenderer = VkDestroyRenderer;
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

/*
extern inline void OpenVkDestroyRenderer()
{
	VkDestroyRenderer();
}

extern inline void OpenVkRecreateSwapChain(uint32_t* Width, uint32_t* Height)
{
	VkRecreateSwapChain(Width, Height);
}

extern inline uint32_t OpenVkCreateRenderPass(uint32_t ColorAttachmentCount, OpenVkBool DepthAttachment, OpenVkBool MsaaAttachment, uint32_t MsaaSamples, OpenVkBool Sampled)
{
	return VkCreateRenderPass(ColorAttachmentCount, DepthAttachment, MsaaAttachment, MsaaSamples, Sampled);
}

extern inline uint32_t OpenVkCreateGraphicsPipeline(OpenVkGraphicsPipelineCreateInfo* Info)
{
	return VkCreateGraphicsPipeline
	(
		Info->VertexPath,
		Info->FragmentPath,
		Info->BindingStride,
		Info->ShaderAttributeFormatCount,
		Info->ShaderAttributeFormats,
		Info->ShaderAttributeOffsets,
		Info->PrimitiveTopology,
		Info->x,
		Info->y,
		Info->Width,
		Info->Height,
		Info->DepthClamp,
		Info->PolygonMode,
		Info->LineWidth,
		Info->CullMode,
		Info->FrontFace,
		Info->MsaaSamples,
		Info->AlphaBlending,
		Info->ColorBlendAttachments,
		Info->PushConstantCount,
		Info->PushConstantShaderTypes,
		Info->PushConstantOffsets,
		Info->PushConstantSizes,
		Info->DescriptorSetLayoutCount,
		Info->DescriptorSetLayouts,
		Info->DepthStencil,
		Info->RenderPass
	);
}

extern inline uint32_t OpenVkCreateFramebuffer(OpenVkFramebufferCreateInfo* Info)
{
	return VkCreateFrambuffer
	(
		Info->AttachmentCount,
		Info->Attachments,
		Info->RenderPass,
		Info->Width,
		Info->Height
	);
}

extern inline uint32_t OpenVkCreateDescriptorSetLayout(uint32_t Binding, uint32_t BindingCount, uint32_t* DescriptorCounts, uint32_t* DescriptorTypes, uint32_t* ShaderTypes)
{
	return VkCreateDescriptorSetLayout(Binding, BindingCount, DescriptorCounts, DescriptorTypes, ShaderTypes);
}

extern inline OpenVkDescriptorPool OpenVkCreateDescriptorPool(uint32_t PoolSizeCount, uint32_t* DescriptorTypes, uint32_t* DescriptorCounts, OpenVkBool Dynamic)
{
	uint32_t DescriptorPoolLocation = VkCreateDescriptorPool(PoolSizeCount, DescriptorTypes, DescriptorCounts, Dynamic);

	OpenVkDescriptorPool DescriptorPool;
	DescriptorPool.DescriptorPool = DescriptorPoolLocation;
	DescriptorPool.Dynamic = Dynamic;

	return DescriptorPool;
}

extern inline uint32_t OpenVkCreateDescriptorSet(OpenVkDescriptorSetCreateInfo* Info)
{
	return VkCreateDescriptorSet
	(
		Info->DescriptorSetLayout,
		Info->DescriptorPool.Dynamic,
		Info->DescriptorPool.DescriptorPool,
		Info->DescriptorWriteCount,
		Info->DescriptorCounts,
		Info->DescriptorTypes,
		Info->UniformBuffers,
		Info->UniformBufferSizes,
		Info->Sampler,
		Info->ImageTypes,
		Info->Images,
		Info->ImageLayouts,
		Info->Bindings
	);
}
*/
/*
extern inline void OpenVkUpdateDescriptorSet(OpenVkDescriptorSetCreateInfo* Info, uint32_t DescriptorSet)
{
	
	return VkUpdateDescriptorSet
	(
		Info->DescriptorSetLayout,
		Info->DescriptorPool.Dynamic,
		Info->DescriptorPool.DescriptorPool,
		Info->DescriptorCount,
		Info->DescriptorTypes,
		Info->UniformBuffers,
		Info->UniformBufferSizes,
		Info->Sampler,
		Info->ImageTypes,
		Info->Images,
		Info->ImageLayouts,
		Info->Bindings,
		DescriptorSet
	);
	
}
*/
/*
extern inline OpenVkBool OpenVkDrawFrame(void (*RenderFunc)(void), void (*ResizeFunc)(void), void (*UpdateFunc)(void))
{
	return VkDrawFrame(RenderFunc, ResizeFunc, UpdateFunc);
}

extern inline OpenVkBool OpenVkBegineFrame()
{
	return VkBeginFrame();
}

extern inline OpenVkBool OpenVkEndFrame()
{
	return VkEndFrame();
}

extern inline void OpenVkBeginRenderPass(OpenVkBeginRenderPassInfo* Info)
{
	VkBeginRenderPass
	(
		Info->ClearColor[0],
		Info->ClearColor[1],
		Info->ClearColor[2],
		Info->ClearColor[3],
		Info->ClearColors,
		Info->ClearDepth,
		Info->RenderPass,
		Info->Framebuffer,
		Info->x,
		Info->y,
		Info->Width,
		Info->Height
	);
}

extern inline void OpenVkEndRenderPass()
{
	VkEndRenderPass();
}

extern inline uint32_t OpenVkCreateTextureImage(const char* Path, OpenVkBool FlipVertical)
{
	return VkCreateTextureImage(Path, FlipVertical);
}

extern inline void OpenVkDestroyTextureImage(uint32_t TextureImage)
{
	VkDestroyTextureImage(TextureImage);
}

extern inline uint32_t OpenVkCreateImageSampler(uint32_t Filter, uint32_t AddressMode)
{
	return VkCreateImageSampler(Filter, AddressMode);
}

extern inline void OpenVkDestroySampler(uint32_t Sampler)
{
	VkDestroySampler(Sampler);
}

extern inline uint32_t OpenVkCreateColorImageAttachment(uint32_t Width, uint32_t Height, uint32_t MsaaSamples, OpenVkBool Sampled)
{
	return VkCreateColorImageAttachment(Width, Height, MsaaSamples, Sampled);
}

extern inline uint32_t OpenVkCreateDepthImageAttachment(uint32_t Width, uint32_t Height, uint32_t MsaaSamples, OpenVkBool Sampled)
{
	return VkCreateDepthImageAttachment(Width, Height, MsaaSamples, Sampled);
}

extern inline uint32_t OpenVkCreateUniformBuffer(size_t Size)
{
	return VkCreateUniformBuffer(Size);
}

extern inline void OpenVkUpdateUniformBuffer(size_t Size, const void* UBO, uint32_t UniformBuffer)
{
	VkUpdateUniformBuffer(Size, UBO, UniformBuffer);
}

extern inline uint32_t OpenVkCreateDynamicUniformBuffer(size_t Size)
{
	return VkCreateDynamicUniformBuffer(Size);
}

extern inline void OpenVkUpdateDynamicUniformBuffer(size_t Size, const void* UBO, uint32_t UniformBuffer)
{
	VkUpdateDynamicUniformBuffer(Size, UBO, UniformBuffer);
}

extern inline uint32_t OpenVkCreateVertexBuffer(size_t Size, const void* Vertices)
{
	return VkCreateVertexBuffer(Size, Vertices);
}

extern inline uint32_t OpenVkCreateIndexBuffer(size_t Size, const void* Indices)
{
	return VkCreateIndexBuffer(Size, Indices);
}

extern inline void OpenVkDestroyBuffer(uint32_t Buffer)
{
	VkDestroyBuffer(Buffer);
}

extern inline void OpenVkBindGraphicsPipeline(uint32_t Pipeline)
{
	VkBindGraphicsPipeline(Pipeline);
}

extern inline void OpenVkSetViewport(float x, float y, float Width, float Height)
{
	VkSetViewport(x, y, Width, Height);
}

extern inline void OpenVkSetScissor(int32_t x, int32_t y, uint32_t Width, uint32_t Height)
{
	VkSetScissor(x, y, Width, Height);
}

extern inline void OpenVkBindVertexBuffer(uint32_t VertexBuffer)
{
	VkBindVertexBuffer(VertexBuffer);
}

extern inline void OpenVkBindIndexBuffer(uint32_t VertexBuffer, uint32_t IndexBuffer)
{
	VkBindIndexBuffer(VertexBuffer, IndexBuffer);
}

extern inline void OpenVkDrawVertices(uint32_t VertexCount)
{
	VkDrawVertices(VertexCount);
}

extern inline void OpenVkDrawIndices(uint32_t IndexCount)
{
	VkDrawIndices(IndexCount);
}

extern inline void OpenVkBindDescriptorSet(uint32_t Pipeline, uint32_t Set, uint32_t DescriptorSet)
{
	VkBindDescriptorSet(Pipeline, Set, DescriptorSet);
}

extern inline void OpenVkPushConstant(uint32_t Pipeline, uint32_t ShaderType, uint32_t Offset, size_t Size, const void* Data)
{
	VkPushConstant(Pipeline, ShaderType, Offset, Size, Data);
}
*/