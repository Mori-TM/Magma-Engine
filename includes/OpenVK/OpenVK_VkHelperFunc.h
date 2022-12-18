#define MAX_FRAMES_IN_FLIGHT 3

typedef struct
{
	VkFramebuffer* Framebuffers;
} VkFramebufferInfo;

typedef struct
{
	VkDescriptorPool DescriptorPool;
	CMA_MemoryZone DescriptorSets;
} VkDescriptorPoolInfo;

typedef struct
{
	VkDescriptorSet DescriptorSets[MAX_FRAMES_IN_FLIGHT];
} VkDescriptorSetInfo;

typedef struct
{
	VkBuffer Buffers[MAX_FRAMES_IN_FLIGHT];
	VkDeviceMemory BufferMemories[MAX_FRAMES_IN_FLIGHT];
	VkDeviceSize Size;
} VkDynamicBufferInfo;

typedef struct
{
	VkFormat Format;
	VkImage Image;
	VkImageView ImageView;
	VkDeviceMemory ImageMemory;
} VkImageInfo;

typedef struct
{
	VkBuffer Buffer;
	VkDeviceMemory BufferMemory;
} VkStaticBufferInfo;

typedef struct
{
	uint32_t DeviceExtensionCount;
	const char* DeviceExtensions[32];

	VkInstance Instance;
	VkPhysicalDevice PhysicalDevice;
	VkDevice Device;

	VkQueue GraphicsQueue;
	VkQueue PresentQueue;

	VkSurfaceKHR Surface;

	uint32_t SwapChainImageCount;
	VkSwapchainKHR SwapChain;
	VkFormat SwapChainImageFormat;
	VkExtent2D SwapChainExtent;
	VkImage* SwapChainImages;
	VkImageView* SwapChainImageViews;

	VkDeviceSize BufferMemoryAlignment;

//	uint32_t ImageCount;
//	VkImage* Images;
//	VkDeviceMemory* ImageMemories;
//	VkImageView* ImageViews;
	CMA_MemoryZone ImageAttachments;

	uint32_t RenderPassCount;
	VkRenderPass* RenderPasses;

	uint32_t PipelineLayoutCount;
	VkPipelineLayout* PipelineLayouts;

	uint32_t PipelineCount;
	VkPipeline* Pipelines;

	uint32_t FramebufferCount;
	VkFramebufferInfo* Framebuffers;

	VkCommandPool CommandPool;
	VkCommandBuffer* CommandBuffers;

	uint32_t CurrentFrame;
	uint32_t ImageIndex;
	VkSemaphore ImageAvailableSemaphores[MAX_FRAMES_IN_FLIGHT];
	VkSemaphore RenderFinishedSemaphores[MAX_FRAMES_IN_FLIGHT];
	VkFence InFlightFences[MAX_FRAMES_IN_FLIGHT];
	VkFence* InFlightImages;

	uint32_t DescriptorSetLayoutCount;
	VkDescriptorSetLayout* DescriptorSetLayouts;

//	uint32_t DescriptorSetCount;
//	VkDescriptorSetInfo* DescriptorSets;
	CMA_MemoryZone DescriptorSets;

//	uint32_t BufferCount;
//	VkBuffer* Buffers;
//	VkDeviceMemory* BufferMemories;
	CMA_MemoryZone StaticBuffers;

	CMA_MemoryZone DynamicBuffers;
//	uint32_t UniformBufferCount;
//	VkDynamicBufferInfo* UniformBuffers;

	uint32_t MipLevels;

//	uint32_t TextureImageCount;
//	VkImage* TextureImages;
//	VkImageView* TextureImageViews;
//	VkDeviceMemory* TextureImageMemories;
	CMA_MemoryZone Images;

//	uint32_t SamplerCount;
//	VkSampler* Sampler;
	CMA_MemoryZone Sampler;

	VkSampleCountFlagBits MsaaSamples;

	CMA_MemoryZone DescriptorPools;
//	VkDescriptorPoolInfo DynamicDescriptorPools;
//	VkDescriptorPoolInfo StaticDescriptorPools;

	VkPhysicalDeviceProperties PhysicalDeviceProperties;
	VkPhysicalDeviceFeatures PhysicalDeviceFeatures;
} VkRendererInfo;

typedef struct
{
	uint32_t GraphicsFamily;
	uint32_t PresentFamily;
} VkQueueFamilyIndices;

typedef struct
{
	VkSurfaceCapabilitiesKHR Capabilities;
	VkSurfaceFormatKHR* Formats;
	uint32_t FormatCount;
	VkPresentModeKHR* PresentModes;
	uint32_t PresentModeCount;
} VkSwapChainSupportDetails;

VkRendererInfo VkRenderer = { NULL };

OpenVkBool VkIsPhysicalDeviceSuitable(VkPhysicalDevice PhysicalDevice)
{
	vkGetPhysicalDeviceProperties(PhysicalDevice, &VkRenderer.PhysicalDeviceProperties);
	vkGetPhysicalDeviceFeatures(PhysicalDevice, &VkRenderer.PhysicalDeviceFeatures);

	return VkRenderer.PhysicalDeviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU &&
		   VkRenderer.PhysicalDeviceFeatures.multiViewport &&
		   VkRenderer.PhysicalDeviceFeatures.wideLines &&
		   VkRenderer.PhysicalDeviceFeatures.samplerAnisotropy;
}

VkQueueFamilyIndices VkFindQueueFamilies(VkPhysicalDevice PhysicalDevice)
{
	uint32_t QueueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(PhysicalDevice, &QueueFamilyCount, NULL);

	VkQueueFamilyProperties* QueueFamilies = (VkQueueFamilyProperties*)OpenVkMalloc(QueueFamilyCount * sizeof(VkQueueFamilyProperties));
	vkGetPhysicalDeviceQueueFamilyProperties(PhysicalDevice, &QueueFamilyCount, QueueFamilies);

	VkQueueFamilyIndices Indices = { 0, 0 };

	for (uint32_t i = 0; i < QueueFamilyCount; i++)
	{
		if (QueueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
			Indices.GraphicsFamily = i;

		VkBool32 Supported = VK_FALSE;
		if (vkGetPhysicalDeviceSurfaceSupportKHR(PhysicalDevice, i, VkRenderer.Surface, &Supported) != VK_SUCCESS)
			OpenVkRuntimeError("Failed to Get Physical Device Surface Support\n");

		if (Supported)
			Indices.PresentFamily = i;

		if (Indices.GraphicsFamily != NULL && Indices.PresentFamily != NULL)
			break;
	}

	OpenVkFree(QueueFamilies);

	return Indices;
}

void VkQuerySwapChainSupport(VkPhysicalDevice PhysicalDevice, VkSwapChainSupportDetails* Details)
{
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(PhysicalDevice, VkRenderer.Surface, &Details->Capabilities);

	uint32_t FormatCount = 0;
	vkGetPhysicalDeviceSurfaceFormatsKHR(PhysicalDevice, VkRenderer.Surface, &FormatCount, NULL);
	Details->Formats = (VkSurfaceFormatKHR*)OpenVkMalloc(FormatCount * sizeof(VkSurfaceFormatKHR));
	vkGetPhysicalDeviceSurfaceFormatsKHR(PhysicalDevice, VkRenderer.Surface, &FormatCount, Details->Formats);
	Details->FormatCount = FormatCount;

	uint32_t PresentModeCount = 0;
	vkGetPhysicalDeviceSurfacePresentModesKHR(PhysicalDevice, VkRenderer.Surface, &PresentModeCount, NULL);
	Details->PresentModes = (VkPresentModeKHR*)OpenVkMalloc(PresentModeCount * sizeof(VkPresentModeKHR));
	vkGetPhysicalDeviceSurfacePresentModesKHR(PhysicalDevice, VkRenderer.Surface, &PresentModeCount, Details->PresentModes);
	Details->PresentModeCount = PresentModeCount;
}

VkSurfaceFormatKHR VkChooseSwapSurfaceFormat(VkSurfaceFormatKHR* AvailableFormats, uint32_t FormatCount)
{
	for (uint32_t i = 0; i < FormatCount; i++)
		if (AvailableFormats[i].format == VK_FORMAT_B8G8R8A8_UNORM && AvailableFormats[i].colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
			//	if (AvailableFormats[i].format == VK_FORMAT_B8G8R8A8_SRGB && AvailableFormats[i].colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
			return AvailableFormats[i];

	return AvailableFormats[0];
}

VkPresentModeKHR VkChooseSwapPresentMode(VkPresentModeKHR* AvailablePresentModes, uint32_t PresentCount)
{
	/*Present Modes(see officale programming guide for vulkan p.146)
	VK_PRESENT_MODE_IMMEDIATE_KHR: Presents Image as soon as possible,
								 it provides the highest Framerate
	VK_PRESENT_MODE_MAILBOX_KHR: Probably waits for v-sync
	VK_PRESENT_MODE_FIFO_KHR: Images saved in queue usually wait for
							  v-sync before shown on screen
	*/
	for (uint32_t i = 0; i < PresentCount; i++)
		if (AvailablePresentModes[i] == VK_PRESENT_MODE_MAILBOX_KHR)
			return AvailablePresentModes[i];

	return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D VkChooseSwapExtent(VkSurfaceCapabilitiesKHR Capabilities)
{
	if (Capabilities.currentExtent.width != UINT32_MAX)
		return Capabilities.currentExtent;
	else
	{
		uint32_t Width = 0;
		uint32_t Height = 0;

		GetWindowSizeFunc(&Width, &Height);

		VkExtent2D ActualExtent =
		{
			Width,
			Height
		};

		ActualExtent.width = MAX(Capabilities.minImageExtent.width, MIN(Capabilities.maxImageExtent.width, ActualExtent.width));
		ActualExtent.height = MAX(Capabilities.minImageExtent.height, MIN(Capabilities.maxImageExtent.height, ActualExtent.height));

		return ActualExtent;
	}
}

VkImageView VkCreateImageView(VkImage Image, VkFormat Format, VkImageAspectFlags AspectFlags, uint32_t MipLevels)
{
	VkImageViewCreateInfo ViewInfo;
	ViewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	ViewInfo.pNext = NULL;
	ViewInfo.flags = 0;
	ViewInfo.image = Image;
	ViewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
	ViewInfo.format = Format;
	ViewInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
	ViewInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
	ViewInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
	ViewInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
	ViewInfo.subresourceRange.aspectMask = AspectFlags;
	ViewInfo.subresourceRange.baseMipLevel = 0;
	ViewInfo.subresourceRange.levelCount = MipLevels;
	ViewInfo.subresourceRange.baseArrayLayer = 0;
	ViewInfo.subresourceRange.layerCount = 1;

	VkImageView ImageView;

	if (vkCreateImageView(VkRenderer.Device, &ViewInfo, NULL, &ImageView) != VK_SUCCESS)
		OpenVkRuntimeError("Failed to Create Image View");

	return ImageView;
}

OpenVkBool VkCreateShaderModule(OpenVkFile File, VkShaderModule* ShaderModule)
{
	VkShaderModuleCreateInfo CreateInfo;
	CreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	CreateInfo.pNext = NULL;
	CreateInfo.flags = 0;
	CreateInfo.codeSize = File.Size;
	CreateInfo.pCode = (const uint32_t*)File.Data;

	if (File.Size == 0 || File.Data == NULL || vkCreateShaderModule(VkRenderer.Device, &CreateInfo, NULL, ShaderModule) != VK_SUCCESS)
		return OpenVkRuntimeError("Failed to Create Shader Module");

	if (File.Freeable)
	{
		OpenVkFree(File.Data);
		File.Data = NULL;
		File.Size = 0;
	}

	return 1;
}

uint32_t VkFindMemoryType(uint32_t TypeFilter, VkMemoryPropertyFlags Properties)
{
	VkPhysicalDeviceMemoryProperties MemoryProperties;
	vkGetPhysicalDeviceMemoryProperties(VkRenderer.PhysicalDevice, &MemoryProperties);

	for (uint32_t i = 0; i < MemoryProperties.memoryTypeCount; i++)
		if (TypeFilter & (1 << i) && (MemoryProperties.memoryTypes[i].propertyFlags & Properties) == Properties)
			return i;

	return OpenVkRuntimeError("Failed To find a Suitable Memory Type");
}

VkCommandBuffer VkBeginSingleTimeCommands()
{
	VkCommandBufferAllocateInfo AllocateInfo;
	AllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	AllocateInfo.pNext = NULL;
	AllocateInfo.commandPool = VkRenderer.CommandPool;
	AllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	AllocateInfo.commandBufferCount = 1;

	VkCommandBuffer CommandBuffer;
	if (vkAllocateCommandBuffers(VkRenderer.Device, &AllocateInfo, &CommandBuffer) != VK_SUCCESS)
		OpenVkRuntimeError("Failed to allocate single time command buffer");

	VkCommandBufferBeginInfo BeginInfo;
	BeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	BeginInfo.pNext = NULL;
	BeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
	BeginInfo.pInheritanceInfo = NULL;

	if (vkBeginCommandBuffer(CommandBuffer, &BeginInfo) != VK_SUCCESS)
		OpenVkRuntimeError("Failed to begin single time command buffer");

	return CommandBuffer;
}

OpenVkBool VkEndSingleTimeCommandBuffer(VkCommandBuffer CommandBuffer)
{
	if (vkEndCommandBuffer(CommandBuffer) != VK_SUCCESS)
		return OpenVkRuntimeError("Failed to end single time command buffer");
	
	VkSubmitInfo SubmitInfo;
	SubmitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	SubmitInfo.pNext = NULL;
	SubmitInfo.waitSemaphoreCount = 0;
	SubmitInfo.pWaitSemaphores = NULL;
	SubmitInfo.pWaitDstStageMask = NULL;
	SubmitInfo.commandBufferCount = 1;
	SubmitInfo.pCommandBuffers = &CommandBuffer;
	SubmitInfo.signalSemaphoreCount = 0;
	SubmitInfo.pSignalSemaphores = NULL;

	VkFenceCreateInfo FenceCreateInfo;
	FenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	FenceCreateInfo.pNext = NULL;
	FenceCreateInfo.flags = 0;
	VkFence Fence;
	

	if (vkCreateFence(VkRenderer.Device, &FenceCreateInfo, NULL, &Fence) != VK_SUCCESS)
		return OpenVkRuntimeError("Failed to create single time command buffer fence");

	if (vkQueueSubmit(VkRenderer.GraphicsQueue, 1, &SubmitInfo, Fence) != VK_SUCCESS)
		return OpenVkRuntimeError("Failed to submit single time command buffer queue");

	if (vkWaitForFences(VkRenderer.Device, 1, &Fence, VK_TRUE, UINT64_MAX) != VK_SUCCESS)
		return OpenVkRuntimeError("Failed to wait for single time command buffer fence");

	vkDestroyFence(VkRenderer.Device, Fence, NULL);

	vkFreeCommandBuffers(VkRenderer.Device, VkRenderer.CommandPool, 1, &CommandBuffer);
	
	return OpenVkTrue;
}

void VkCopyBuffer(VkBuffer SrcBuffer, VkBuffer DstBuffer, VkDeviceSize Size)
{
	VkCommandBuffer CommandBuffer = VkBeginSingleTimeCommands();

	VkBufferCopy CopyRegion;
	CopyRegion.srcOffset = 0;
	CopyRegion.dstOffset = 0;
	CopyRegion.size = Size;

	vkCmdCopyBuffer(CommandBuffer, SrcBuffer, DstBuffer, 1, &CopyRegion);

	VkEndSingleTimeCommandBuffer(CommandBuffer);
}

OpenVkBool VkCreateBuffer(VkDeviceSize Size, VkBufferUsageFlags Usage, VkMemoryPropertyFlags Properties, VkBuffer* Buffer, VkDeviceMemory* BufferMemory)
{
	VkBufferCreateInfo BufferInfo;
	BufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	BufferInfo.pNext = NULL;
	BufferInfo.flags = 0;
	BufferInfo.size = Size;
	BufferInfo.usage = Usage;
	BufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	BufferInfo.queueFamilyIndexCount = 0;
	BufferInfo.pQueueFamilyIndices = NULL;

	if (vkCreateBuffer(VkRenderer.Device, &BufferInfo, NULL, Buffer) != VK_SUCCESS)
		return OpenVkRuntimeError("Failed to Create Buffer");

	VkMemoryRequirements MemoryRequirements;
	vkGetBufferMemoryRequirements(VkRenderer.Device, *Buffer, &MemoryRequirements);
	VkRenderer.BufferMemoryAlignment = (VkRenderer.BufferMemoryAlignment > MemoryRequirements.alignment) ? VkRenderer.BufferMemoryAlignment : MemoryRequirements.alignment;

	VkMemoryAllocateInfo AllocateInfo;
	AllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	AllocateInfo.pNext = NULL;
	AllocateInfo.allocationSize = MemoryRequirements.size;
	AllocateInfo.memoryTypeIndex = VkFindMemoryType(MemoryRequirements.memoryTypeBits, Properties);

	VkMemoryAllocateFlagsInfoKHR AllocateFlagsInfo;
	if (Usage & VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT) 
	{
		AllocateFlagsInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_FLAGS_INFO_KHR;
		AllocateFlagsInfo.pNext = NULL;
		AllocateFlagsInfo.flags = VK_MEMORY_ALLOCATE_DEVICE_ADDRESS_BIT_KHR;		
		AllocateFlagsInfo.deviceMask = 0;
		AllocateInfo.pNext = &AllocateFlagsInfo;
	}

	if (vkAllocateMemory(VkRenderer.Device, &AllocateInfo, NULL, BufferMemory) != VK_SUCCESS)
		return OpenVkRuntimeError("Failed to Allocate Buffer Memory");

	if (vkBindBufferMemory(VkRenderer.Device, *Buffer, *BufferMemory, 0) != VK_SUCCESS)
		return OpenVkRuntimeError("Failed to bind Buffer Memory");

	return 1;
}

OpenVkBool VkCreateVkBufferExt(VkBufferUsageFlags SrcUsage, VkMemoryPropertyFlags SrcProperties, VkBufferUsageFlags DstUsage, VkMemoryPropertyFlags DstProperties, size_t Size, const void* InData, VkBuffer* Buffer, VkDeviceMemory* BufferMemory)
{
	VkBuffer StagingBuffer;
	VkDeviceMemory StagingBufferMemory;
	if (VkCreateBuffer(Size, SrcUsage, SrcProperties, &StagingBuffer, &StagingBufferMemory) == OPENVK_ERROR)
		return OpenVkRuntimeError("Failed to Create Buffer: Func 0");

	void* Data;
	vkMapMemory(VkRenderer.Device, StagingBufferMemory, 0, Size, 0, &Data);
	memcpy(Data, InData, Size);

	if ((SrcProperties & VK_MEMORY_PROPERTY_HOST_COHERENT_BIT) == 0)
	{
		VkMappedMemoryRange MappedRange;
		MappedRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
		MappedRange.pNext = NULL;
		MappedRange.memory = StagingBufferMemory;
		MappedRange.offset = 0;
		MappedRange.size = VK_WHOLE_SIZE;
		if (vkFlushMappedMemoryRanges(VkRenderer.Device, 1, &MappedRange) != VK_SUCCESS)
			return OpenVkRuntimeError("Failed to flush mapped buffer memory ranges");
	}

	vkUnmapMemory(VkRenderer.Device, StagingBufferMemory);

	if (VkCreateBuffer(Size, DstUsage, DstProperties, Buffer, BufferMemory) == OPENVK_ERROR)
		return OpenVkRuntimeError("Failed to Create Buffer: Func 1");

	VkCopyBuffer(StagingBuffer, *Buffer, Size);

	vkDestroyBuffer(VkRenderer.Device, StagingBuffer, NULL);
	vkFreeMemory(VkRenderer.Device, StagingBufferMemory, NULL);

	return OpenVkTrue;
}

uint32_t VkCreateBufferExt(VkBufferUsageFlags SrcUsage, VkMemoryPropertyFlags SrcProperties, VkBufferUsageFlags DstUsage, VkMemoryPropertyFlags DstProperties, size_t Size, const void* InData)
{
	VkStaticBufferInfo BufferInfo;

	VkCreateVkBufferExt(SrcUsage, SrcProperties, DstUsage, DstProperties, Size, InData, &BufferInfo.Buffer, &BufferInfo.BufferMemory);

	return CMA_Push(&VkRenderer.StaticBuffers, &BufferInfo);
}

uint32_t VkCreateDynamicBuffer(size_t Size, VkBufferUsageFlags Usage)
{
	VkDynamicBufferInfo Buffer;

	Buffer.Size = OpenVkAlignedSize(Size, VkRenderer.BufferMemoryAlignment);

	for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
		if (VkCreateBuffer(Buffer.Size, Usage, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, &Buffer.Buffers[i], &Buffer.BufferMemories[i]) == OPENVK_ERROR)
			return OpenVkRuntimeError("Failed to create dynamic vertex buffer");

	VkMemoryRequirements MemoryRequirements;
	vkGetBufferMemoryRequirements(VkRenderer.Device, Buffer.Buffers[0], &MemoryRequirements);
	VkRenderer.BufferMemoryAlignment = (VkRenderer.BufferMemoryAlignment > MemoryRequirements.alignment) ? VkRenderer.BufferMemoryAlignment : MemoryRequirements.alignment;
	Buffer.Size = MemoryRequirements.size;

	return CMA_Push(&VkRenderer.DynamicBuffers, &Buffer);
}

void VkDestroyBuffer(uint32_t Buffer)
{
	vkDeviceWaitIdle(VkRenderer.Device);

	VkStaticBufferInfo* BufferInfo = (VkStaticBufferInfo*)CMA_GetAt(&VkRenderer.StaticBuffers, Buffer);
	if (BufferInfo != NULL)
	{
		vkDestroyBuffer(VkRenderer.Device, BufferInfo->Buffer, NULL);
		vkFreeMemory(VkRenderer.Device, BufferInfo->BufferMemory, NULL);
		CMA_Pop(&VkRenderer.StaticBuffers, Buffer);
		return;
	}

	OpenVkRuntimeError("Failed To Find Static Destroy Buffer");
}

void VkDestroyDynamicBuffer(uint32_t Buffer)
{
	vkDeviceWaitIdle(VkRenderer.Device);

	VkDynamicBufferInfo* BufferInfo = (VkDynamicBufferInfo*)CMA_GetAt(&VkRenderer.DynamicBuffers, Buffer);
	if (BufferInfo != NULL)
	{
		for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
		{
			vkDestroyBuffer(VkRenderer.Device, BufferInfo->Buffers[i], NULL);
			vkFreeMemory(VkRenderer.Device, BufferInfo->BufferMemories[i], NULL);
			CMA_Pop(&VkRenderer.DynamicBuffers, Buffer);
		}		
		return;
	}

	OpenVkRuntimeError("Failed To Find Dynamic Destroy Buffer");
}

OpenVkBool VkCreateImage(uint32_t Width, uint32_t Height, uint32_t MipLevels, VkSampleCountFlagBits NumSamples, VkFormat Format, VkImageTiling Tiling, VkImageUsageFlags Usage, VkMemoryPropertyFlags Properties, VkImage* Image, VkDeviceMemory* ImageMemory)
{
	VkImageCreateInfo ImageInfo;
	ImageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	ImageInfo.pNext = NULL;
	ImageInfo.flags = 0;
	ImageInfo.imageType = VK_IMAGE_TYPE_2D;
	ImageInfo.format = Format;
	ImageInfo.extent.width = Width;
	ImageInfo.extent.height = Height;
	ImageInfo.extent.depth = 1;
	ImageInfo.mipLevels = MipLevels;
	ImageInfo.arrayLayers = 1;
	ImageInfo.samples = NumSamples;
	ImageInfo.tiling = Tiling;
	ImageInfo.usage = Usage;
	ImageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	ImageInfo.queueFamilyIndexCount = 0;
	ImageInfo.pQueueFamilyIndices = NULL;
	ImageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

	if (vkCreateImage(VkRenderer.Device, &ImageInfo, NULL, Image) != VK_SUCCESS)
		return OpenVkRuntimeError("Failed to Create Image");

	VkMemoryRequirements MemoryRequirements;
	vkGetImageMemoryRequirements(VkRenderer.Device, *Image, &MemoryRequirements);

	VkMemoryAllocateInfo AllocateInfo;
	AllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	AllocateInfo.pNext = NULL;
	AllocateInfo.allocationSize = MemoryRequirements.size;
	AllocateInfo.memoryTypeIndex = VkFindMemoryType(MemoryRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

	if (vkAllocateMemory(VkRenderer.Device, &AllocateInfo, NULL, ImageMemory) != VK_SUCCESS)
		return OpenVkRuntimeError("Failed to Allocate Image Memory");

	vkBindImageMemory(VkRenderer.Device, *Image, *ImageMemory, 0);

	return 1;
}

VkFormat VkFindSupportedFormat(VkFormat* Candidates, uint32_t CandidatesCount, VkImageTiling Tiling, VkFormatFeatureFlags Features)
{
	for (uint32_t i = 0; i < CandidatesCount; i++)
	{
		VkFormatProperties Properties;
		vkGetPhysicalDeviceFormatProperties(VkRenderer.PhysicalDevice, Candidates[i], &Properties);

		if (Tiling == VK_IMAGE_TILING_LINEAR && (Properties.linearTilingFeatures & Features) == Features)
			return Candidates[i];
		else if (Tiling == VK_IMAGE_TILING_OPTIMAL && (Properties.optimalTilingFeatures & Features) == Features)
			return Candidates[i];
	}
}

VkFormat VkFindDepthFormat()
{
	VkFormat Formats[] = { VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT };
	return VkFindSupportedFormat(Formats, 3, VK_IMAGE_TILING_OPTIMAL, VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
}

//wtf?
VkDescriptorSetAllocateInfo VkAllocateDescriptorSets(VkDescriptorPool DescriptorPool, uint32_t Count, VkDescriptorSetLayout* SetLayouts)
{
	VkDescriptorSetAllocateInfo AllocateInfo;
	AllocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	AllocateInfo.pNext = NULL;
	AllocateInfo.descriptorPool = DescriptorPool;
	AllocateInfo.descriptorSetCount = Count;
	AllocateInfo.pSetLayouts = SetLayouts;

	return AllocateInfo;
}

VkWriteDescriptorSet VkDescriptorSetWrite(VkDescriptorSet DstSet, uint32_t DstBinding, VkDescriptorType DescriptorType, uint32_t DescriptorCount, VkDescriptorImageInfo* ImageInfos, VkDescriptorBufferInfo* BufferInfos, VkWriteDescriptorSetAccelerationStructureKHR* ASInfos)
{
	VkWriteDescriptorSet DescriptorWrite;
	DescriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	DescriptorWrite.pNext = ASInfos;
	DescriptorWrite.dstSet = DstSet;
	DescriptorWrite.dstBinding = DstBinding;
	DescriptorWrite.dstArrayElement = 0;
	DescriptorWrite.descriptorCount = DescriptorCount;
	DescriptorWrite.descriptorType = DescriptorType;
	DescriptorWrite.pImageInfo = ImageInfos;
	DescriptorWrite.pBufferInfo = BufferInfos;
	DescriptorWrite.pTexelBufferView = NULL;

	return DescriptorWrite;
}

VkSampleCountFlagBits VkGetMaxSampleCount()
{
	VkPhysicalDeviceProperties PhysicalDeviceProperties;
	vkGetPhysicalDeviceProperties(VkRenderer.PhysicalDevice, &PhysicalDeviceProperties);

	VkSampleCountFlags SampleCount = PhysicalDeviceProperties.limits.framebufferColorSampleCounts & PhysicalDeviceProperties.limits.framebufferDepthSampleCounts;

	if (SampleCount & VK_SAMPLE_COUNT_64_BIT) return VK_SAMPLE_COUNT_64_BIT;
	if (SampleCount & VK_SAMPLE_COUNT_32_BIT) return VK_SAMPLE_COUNT_32_BIT;
	if (SampleCount & VK_SAMPLE_COUNT_16_BIT) return VK_SAMPLE_COUNT_16_BIT;
	if (SampleCount & VK_SAMPLE_COUNT_8_BIT) return VK_SAMPLE_COUNT_8_BIT;
	if (SampleCount & VK_SAMPLE_COUNT_4_BIT) return VK_SAMPLE_COUNT_4_BIT;
	if (SampleCount & VK_SAMPLE_COUNT_2_BIT) return VK_SAMPLE_COUNT_2_BIT;

	return VK_SAMPLE_COUNT_1_BIT;
}

//get the vulkan color format from openvk
VkFormat VkGetOpenVkFormat(uint32_t Format)
{
	VkFormat ColorFormat = VkRenderer.SwapChainImageFormat;

	switch (Format)
	{
	case OPENVK_FORMAT_R:
		ColorFormat = VK_FORMAT_R8_UNORM;
		break;
	case OPENVK_FORMAT_RG:
		ColorFormat = VK_FORMAT_R8G8_UNORM;
		break;
	case OPENVK_FORMAT_RGB:
		ColorFormat = VK_FORMAT_R8G8B8_UNORM;
		break;
	case OPENVK_FORMAT_RGBA:
		ColorFormat = VK_FORMAT_R8G8B8A8_UNORM;
		break;

	case OPENVK_FORMAT_R16F:
		ColorFormat = VK_FORMAT_R16_SFLOAT;
		break;
	case OPENVK_FORMAT_RG16F:
		ColorFormat = VK_FORMAT_R16G16_SFLOAT;
		break;
	case OPENVK_FORMAT_RGB16F:
		ColorFormat = VK_FORMAT_R16G16B16_SFLOAT;
		break;
	case OPENVK_FORMAT_RGBA16F:
		ColorFormat = VK_FORMAT_R16G16B16A16_SFLOAT;
		break;

	case OPENVK_FORMAT_R32F:
		ColorFormat = VK_FORMAT_R32_SFLOAT;
		break;
	case OPENVK_FORMAT_RG32F:
		ColorFormat = VK_FORMAT_R32G32_SFLOAT;
		break;
	case OPENVK_FORMAT_RGB32F:
		ColorFormat = VK_FORMAT_R32G32B32_SFLOAT;
		break;
	case OPENVK_FORMAT_RGBA32F:
		ColorFormat = VK_FORMAT_R32G32B32A32_SFLOAT;
		break;

	case OPENVK_FORMAT_R_UINT:
		ColorFormat = VK_FORMAT_R8_UINT;
		break;
	case OPENVK_FORMAT_RG_UINT:
		ColorFormat = VK_FORMAT_R8G8_UINT;
		break;
	case OPENVK_FORMAT_RGB_UINT:
		ColorFormat = VK_FORMAT_R8G8B8_UINT;
		break;
	case OPENVK_FORMAT_RGBA_UINT:
		ColorFormat = VK_FORMAT_R8G8B8A8_UINT;
		break;

	default:
		break;
	}

	return ColorFormat;
}

void VkSetImageLayout(VkCommandBuffer CommandBuffer, VkImage Image, VkImageLayout OldImageLayout, VkImageLayout NewImageLayout, uint32_t MipLevels, VkImageSubresourceRange* SubresourceRange)
{
	VkImageMemoryBarrier ImageMemoryBarrier;
	ImageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	ImageMemoryBarrier.pNext = NULL;
	ImageMemoryBarrier.srcAccessMask = 0;
	ImageMemoryBarrier.dstAccessMask = 0;
	ImageMemoryBarrier.oldLayout = OldImageLayout;
	ImageMemoryBarrier.newLayout = NewImageLayout;
	ImageMemoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	ImageMemoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	ImageMemoryBarrier.image = Image;
	if (SubresourceRange)
	{
		ImageMemoryBarrier.subresourceRange = *SubresourceRange;
	}
	else
	{
		ImageMemoryBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		ImageMemoryBarrier.subresourceRange.baseMipLevel = 0;
		ImageMemoryBarrier.subresourceRange.levelCount = MipLevels;
		ImageMemoryBarrier.subresourceRange.baseArrayLayer = 0;
		ImageMemoryBarrier.subresourceRange.layerCount = 1;
	}
	
	switch (OldImageLayout)
	{
	case VK_IMAGE_LAYOUT_UNDEFINED:
		ImageMemoryBarrier.srcAccessMask = 0;
		break;

	case VK_IMAGE_LAYOUT_PREINITIALIZED:
		ImageMemoryBarrier.srcAccessMask = VK_ACCESS_HOST_WRITE_BIT;
		break;

	case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
		ImageMemoryBarrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		break;

	case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
		ImageMemoryBarrier.srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
		break;

	case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
		ImageMemoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
		break;

	case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
		ImageMemoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		break;

	case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
		ImageMemoryBarrier.srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
		break;
	default:
		break;
	}

	switch (NewImageLayout)
	{
	case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
		ImageMemoryBarrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		break;

	case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
		ImageMemoryBarrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
		break;

	case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
		ImageMemoryBarrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		break;

	case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
		ImageMemoryBarrier.dstAccessMask = ImageMemoryBarrier.dstAccessMask | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
		break;

	case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
		if (ImageMemoryBarrier.srcAccessMask == 0)
			ImageMemoryBarrier.srcAccessMask = VK_ACCESS_HOST_WRITE_BIT | VK_ACCESS_TRANSFER_WRITE_BIT;
		ImageMemoryBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
		break;
	default:
		break;
	}
	

	VkPipelineStageFlags SourceStage = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;
	VkPipelineStageFlags DestinationStage = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;

	vkCmdPipelineBarrier(
		CommandBuffer,
		SourceStage,
		DestinationStage,
		0,
		0, NULL,
		0, NULL,
		1, &ImageMemoryBarrier);
}

void VkCopyBufferToImage(VkBuffer Buffer, VkImage Image, uint32_t Width, uint32_t Height)
{
	VkCommandBuffer CommandBuffer = VkBeginSingleTimeCommands();

	VkBufferImageCopy Region;
	Region.bufferOffset = 0;
	Region.bufferRowLength = 0;
	Region.bufferImageHeight = 0;
	Region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	Region.imageSubresource.mipLevel = 0;
	Region.imageSubresource.baseArrayLayer = 0;
	Region.imageSubresource.layerCount = 1;
	Region.imageOffset.x = 0;
	Region.imageOffset.y = 0;
	Region.imageOffset.z = 0;
	Region.imageExtent.width = Width;
	Region.imageExtent.height = Height;
	Region.imageExtent.depth = 1;

	vkCmdCopyBufferToImage(CommandBuffer, Buffer, Image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &Region);

	VkEndSingleTimeCommandBuffer(CommandBuffer);
}

void VkGenerateMipmaps(VkImage Image, VkFormat ImageFormat, int32_t TextureWidth, int32_t TextureHeight, uint32_t MipLevels)
{
	VkFormatProperties FormatProperties;
	vkGetPhysicalDeviceFormatProperties(VkRenderer.PhysicalDevice, ImageFormat, &FormatProperties);
	if (!(FormatProperties.linearTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT))
		return;

	VkCommandBuffer CommandBuffer = VkBeginSingleTimeCommands();

	VkImageMemoryBarrier Barrier;
	Barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	Barrier.pNext = NULL;
	Barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	Barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	Barrier.image = Image;
	Barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	Barrier.subresourceRange.levelCount = 1;
	Barrier.subresourceRange.baseArrayLayer = 0;
	Barrier.subresourceRange.layerCount = 1;

	int32_t MipWidth = TextureWidth;
	int32_t MipHeight = TextureHeight;

	for (uint32_t i = 1; i < MipLevels; i++)
	{
		Barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		Barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
		Barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
		Barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
		Barrier.subresourceRange.baseMipLevel = i - 1;

		vkCmdPipelineBarrier(CommandBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, NULL, 0, NULL, 1, &Barrier);

		VkImageBlit Blit;

		Blit.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		Blit.srcSubresource.mipLevel = i - 1;
		Blit.srcSubresource.baseArrayLayer = 0;
		Blit.srcSubresource.layerCount = 1;
		Blit.srcOffsets[0].x = 0;
		Blit.srcOffsets[0].y = 0;
		Blit.srcOffsets[0].z = 0;
		Blit.srcOffsets[1].x = MipWidth;
		Blit.srcOffsets[1].y = MipHeight;
		Blit.srcOffsets[1].z = 1;
		Blit.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		Blit.dstSubresource.mipLevel = i;
		Blit.dstSubresource.baseArrayLayer = 0;
		Blit.dstSubresource.layerCount = 1;
		Blit.dstOffsets[0].x = 0;
		Blit.dstOffsets[0].y = 0;
		Blit.dstOffsets[0].z = 0;
		Blit.dstOffsets[1].x = MipWidth > 1 ? MipWidth / 2 : 1;
		Blit.dstOffsets[1].y = MipHeight > 1 ? MipHeight / 2 : 1;
		Blit.dstOffsets[1].z = 1;

		vkCmdBlitImage(CommandBuffer, Image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, Image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &Blit, VK_FILTER_LINEAR);

		Barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
		Barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
		Barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
		Barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

		vkCmdPipelineBarrier(CommandBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0, NULL, 0, NULL, 1, &Barrier);

		if (MipWidth > 1) MipWidth *= 0.5;
		if (MipHeight > 1) MipHeight *= 0.5;
	}

	Barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
	Barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
	Barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
	Barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	Barrier.subresourceRange.baseMipLevel = MipLevels - 1;

	vkCmdPipelineBarrier(CommandBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0, NULL, 0, NULL, 1, &Barrier);

	VkEndSingleTimeCommandBuffer(CommandBuffer);
}
