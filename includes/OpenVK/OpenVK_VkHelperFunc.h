#define MAX_FRAMES_IN_FLIGHT 2

typedef struct
{
	VkFramebuffer* Framebuffers;
} VkFramebufferInfo;

typedef struct
{
	uint32_t DescriptorPoolCount;
	VkDescriptorPool* DescriptorPools;
} VkDescriptorPoolInfo;

typedef struct
{
	VkDescriptorSet DescriptorSets[MAX_FRAMES_IN_FLIGHT];
} VkDescriptorSetInfo;

typedef struct
{
	VkBuffer Buffers[MAX_FRAMES_IN_FLIGHT];
	VkDeviceMemory BufferMemories[MAX_FRAMES_IN_FLIGHT];
} VkUniformBufferInfo;

typedef struct
{
	VkImage TextureImage;
	VkImageView TextureImageView;
	VkDeviceMemory TextureImageMemory;
} VkTextureImageInfo;

typedef struct
{
	VkBuffer Buffer;
	VkDeviceMemory BufferMemory;
} VkBufferInfo;

typedef struct
{
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

	uint32_t ImageCount;
	VkImage* Images;
	VkDeviceMemory* ImageMemories;
	VkImageView* ImageViews;

	uint32_t RenderPassCount;
	VkRenderPass* RenderPasses;

	uint32_t PipelineCount;
	VkPipelineLayout* PipelineLayouts;
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
	CMA_MemoryZone Buffers;

	uint32_t UniformBufferCount;
	VkUniformBufferInfo* UniformBuffers;

	uint32_t MipLevels;

//	uint32_t TextureImageCount;
//	VkImage* TextureImages;
//	VkImageView* TextureImageViews;
//	VkDeviceMemory* TextureImageMemories;
	CMA_MemoryZone TextureImages;

//	uint32_t SamplerCount;
//	VkSampler* Sampler;
	CMA_MemoryZone Sampler;

	VkSampleCountFlagBits MsaaSamples;

	VkDescriptorPoolInfo DynamicDescriptorPools;
	VkDescriptorPoolInfo StaticDescriptorPools;

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
	VkPhysicalDeviceProperties DeviceProperties;
	VkPhysicalDeviceFeatures DeviceFeatures;
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

OpenVkBool VkCreateShaderModule(const char* Path, VkShaderModule* ShaderModule)
{
	size_t Size;
	char* Code = OpenVkReadFileData(Path, &Size);

	VkShaderModuleCreateInfo CreateInfo;
	CreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	CreateInfo.pNext = NULL;
	CreateInfo.flags = 0;
	CreateInfo.codeSize = Size;
	CreateInfo.pCode = (uint32_t*)Code;

	if (vkCreateShaderModule(VkRenderer.Device, &CreateInfo, NULL, ShaderModule) != VK_SUCCESS)
		return OpenVkRuntimeError("Failed to Create Shader Module");

	OpenVkFree(Code);

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

	VkMemoryAllocateInfo AllocateInfo;
	AllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	AllocateInfo.pNext = NULL;
	AllocateInfo.allocationSize = MemoryRequirements.size;
	AllocateInfo.memoryTypeIndex = VkFindMemoryType(MemoryRequirements.memoryTypeBits, Properties);

	if (vkAllocateMemory(VkRenderer.Device, &AllocateInfo, NULL, BufferMemory) != VK_SUCCESS)
		return OpenVkRuntimeError("Failed to Allocate Buffer Memory");

	vkBindBufferMemory(VkRenderer.Device, *Buffer, *BufferMemory, 0);

	return 1;
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
	vkAllocateCommandBuffers(VkRenderer.Device, &AllocateInfo, &CommandBuffer);

	VkCommandBufferBeginInfo BeginInfo;
	BeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	BeginInfo.pNext = NULL;
	BeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
	BeginInfo.pInheritanceInfo = NULL;

	vkBeginCommandBuffer(CommandBuffer, &BeginInfo);

	return CommandBuffer;
}

void VkEndSingleTimeCommandBuffer(VkCommandBuffer CommandBuffer)
{
	vkEndCommandBuffer(CommandBuffer);

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

	vkQueueSubmit(VkRenderer.GraphicsQueue, 1, &SubmitInfo, VK_NULL_HANDLE);
	vkQueueWaitIdle(VkRenderer.GraphicsQueue);

	vkFreeCommandBuffers(VkRenderer.Device, VkRenderer.CommandPool, 1, &CommandBuffer);
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

VkWriteDescriptorSet VkDescriptorSetWrite(VkDescriptorSet DstSet, uint32_t DstBinding, VkDescriptorType DescriptorType, uint32_t DescriptorCount, VkDescriptorImageInfo* ImageInfo, VkDescriptorBufferInfo* BufferInfo)
{
	VkWriteDescriptorSet DescriptorWrite;
	DescriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	DescriptorWrite.pNext = NULL;
	DescriptorWrite.dstSet = DstSet;
	DescriptorWrite.dstBinding = DstBinding;
	DescriptorWrite.dstArrayElement = 0;
	DescriptorWrite.descriptorCount = DescriptorCount;
	DescriptorWrite.descriptorType = DescriptorType;
	DescriptorWrite.pImageInfo = ImageInfo;
	DescriptorWrite.pBufferInfo = BufferInfo;
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

//Textures
void VkTransitionImageLayout(VkImage Image, VkFormat Format, VkImageLayout OldLayout, VkImageLayout NewLayout, uint32_t MipLevels)
{
	VkCommandBuffer CommandBuffer = VkBeginSingleTimeCommands();

	VkImageMemoryBarrier Barrier;
	Barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	Barrier.pNext = NULL;
	Barrier.srcAccessMask = 0;
	Barrier.dstAccessMask = 0;
	Barrier.oldLayout = OldLayout;
	Barrier.newLayout = NewLayout;
	Barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	Barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	Barrier.image = Image;
	Barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	Barrier.subresourceRange.baseMipLevel = 0;
	Barrier.subresourceRange.levelCount = MipLevels;
	Barrier.subresourceRange.baseArrayLayer = 0;
	Barrier.subresourceRange.layerCount = 1;

	VkPipelineStageFlags SourceStage;
	VkPipelineStageFlags DestinationStage;

	if (OldLayout == VK_IMAGE_LAYOUT_UNDEFINED && NewLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
	{
		Barrier.srcAccessMask = 0;
		Barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

		SourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		DestinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
	}
	else if (OldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && NewLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
	{
		Barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		Barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

		SourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
		DestinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
	}
	else if (OldLayout == VK_IMAGE_LAYOUT_UNDEFINED && NewLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
	{
		Barrier.srcAccessMask = 0;
		Barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

		SourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		DestinationStage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
	}

	vkCmdPipelineBarrier(CommandBuffer, SourceStage, DestinationStage, 0, 0, NULL, 0, NULL, 1, &Barrier);

	VkEndSingleTimeCommandBuffer(CommandBuffer);
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
		Blit.dstOffsets[1].x = MipWidth > 1 ? MipWidth * 0.5 : 1;
		Blit.dstOffsets[1].y = MipHeight > 1 ? MipHeight * 0.5 : 1;
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
