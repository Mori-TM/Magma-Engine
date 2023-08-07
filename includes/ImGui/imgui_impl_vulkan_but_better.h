/*
* This Backend is written by Moritz Gooth a 15 year old student from germany
* You can do whatever you want with this, except claim that you wrote this
*/

//-----------------------------------------------------------------------------
// SHADERS
//-----------------------------------------------------------------------------

// glsl_shader.vert, compiled with:
// # glslangValidator -V -x -o glsl_shader.vert.u32 glsl_shader.vert
/*
#version 450 core
layout(location = 0) in vec2 aPos;
layout(location = 1) in vec2 aUV;
layout(location = 2) in vec4 aColor;
layout(push_constant) uniform uPushConstant { vec2 uScale; vec2 uTranslate; } pc;

out gl_PerVertex { vec4 gl_Position; };
layout(location = 0) out struct { vec4 Color; vec2 UV; } Out;

void main()
{
	Out.Color = aColor;
	Out.UV = aUV;
	gl_Position = vec4(aPos * pc.uScale + pc.uTranslate, 0, 1);
}
*/
static uint32_t ImGuiVertexShader[] =
{
	0x07230203,0x00010000,0x00080001,0x0000002e,0x00000000,0x00020011,0x00000001,0x0006000b,
	0x00000001,0x4c534c47,0x6474732e,0x3035342e,0x00000000,0x0003000e,0x00000000,0x00000001,
	0x000a000f,0x00000000,0x00000004,0x6e69616d,0x00000000,0x0000000b,0x0000000f,0x00000015,
	0x0000001b,0x0000001c,0x00030003,0x00000002,0x000001c2,0x00040005,0x00000004,0x6e69616d,
	0x00000000,0x00030005,0x00000009,0x00000000,0x00050006,0x00000009,0x00000000,0x6f6c6f43,
	0x00000072,0x00040006,0x00000009,0x00000001,0x00005655,0x00030005,0x0000000b,0x0074754f,
	0x00040005,0x0000000f,0x6c6f4361,0x0000726f,0x00030005,0x00000015,0x00565561,0x00060005,
	0x00000019,0x505f6c67,0x65567265,0x78657472,0x00000000,0x00060006,0x00000019,0x00000000,
	0x505f6c67,0x7469736f,0x006e6f69,0x00030005,0x0000001b,0x00000000,0x00040005,0x0000001c,
	0x736f5061,0x00000000,0x00060005,0x0000001e,0x73755075,0x6e6f4368,0x6e617473,0x00000074,
	0x00050006,0x0000001e,0x00000000,0x61635375,0x0000656c,0x00060006,0x0000001e,0x00000001,
	0x61725475,0x616c736e,0x00006574,0x00030005,0x00000020,0x00006370,0x00040047,0x0000000b,
	0x0000001e,0x00000000,0x00040047,0x0000000f,0x0000001e,0x00000002,0x00040047,0x00000015,
	0x0000001e,0x00000001,0x00050048,0x00000019,0x00000000,0x0000000b,0x00000000,0x00030047,
	0x00000019,0x00000002,0x00040047,0x0000001c,0x0000001e,0x00000000,0x00050048,0x0000001e,
	0x00000000,0x00000023,0x00000000,0x00050048,0x0000001e,0x00000001,0x00000023,0x00000008,
	0x00030047,0x0000001e,0x00000002,0x00020013,0x00000002,0x00030021,0x00000003,0x00000002,
	0x00030016,0x00000006,0x00000020,0x00040017,0x00000007,0x00000006,0x00000004,0x00040017,
	0x00000008,0x00000006,0x00000002,0x0004001e,0x00000009,0x00000007,0x00000008,0x00040020,
	0x0000000a,0x00000003,0x00000009,0x0004003b,0x0000000a,0x0000000b,0x00000003,0x00040015,
	0x0000000c,0x00000020,0x00000001,0x0004002b,0x0000000c,0x0000000d,0x00000000,0x00040020,
	0x0000000e,0x00000001,0x00000007,0x0004003b,0x0000000e,0x0000000f,0x00000001,0x00040020,
	0x00000011,0x00000003,0x00000007,0x0004002b,0x0000000c,0x00000013,0x00000001,0x00040020,
	0x00000014,0x00000001,0x00000008,0x0004003b,0x00000014,0x00000015,0x00000001,0x00040020,
	0x00000017,0x00000003,0x00000008,0x0003001e,0x00000019,0x00000007,0x00040020,0x0000001a,
	0x00000003,0x00000019,0x0004003b,0x0000001a,0x0000001b,0x00000003,0x0004003b,0x00000014,
	0x0000001c,0x00000001,0x0004001e,0x0000001e,0x00000008,0x00000008,0x00040020,0x0000001f,
	0x00000009,0x0000001e,0x0004003b,0x0000001f,0x00000020,0x00000009,0x00040020,0x00000021,
	0x00000009,0x00000008,0x0004002b,0x00000006,0x00000028,0x00000000,0x0004002b,0x00000006,
	0x00000029,0x3f800000,0x00050036,0x00000002,0x00000004,0x00000000,0x00000003,0x000200f8,
	0x00000005,0x0004003d,0x00000007,0x00000010,0x0000000f,0x00050041,0x00000011,0x00000012,
	0x0000000b,0x0000000d,0x0003003e,0x00000012,0x00000010,0x0004003d,0x00000008,0x00000016,
	0x00000015,0x00050041,0x00000017,0x00000018,0x0000000b,0x00000013,0x0003003e,0x00000018,
	0x00000016,0x0004003d,0x00000008,0x0000001d,0x0000001c,0x00050041,0x00000021,0x00000022,
	0x00000020,0x0000000d,0x0004003d,0x00000008,0x00000023,0x00000022,0x00050085,0x00000008,
	0x00000024,0x0000001d,0x00000023,0x00050041,0x00000021,0x00000025,0x00000020,0x00000013,
	0x0004003d,0x00000008,0x00000026,0x00000025,0x00050081,0x00000008,0x00000027,0x00000024,
	0x00000026,0x00050051,0x00000006,0x0000002a,0x00000027,0x00000000,0x00050051,0x00000006,
	0x0000002b,0x00000027,0x00000001,0x00070050,0x00000007,0x0000002c,0x0000002a,0x0000002b,
	0x00000028,0x00000029,0x00050041,0x00000011,0x0000002d,0x0000001b,0x0000000d,0x0003003e,
	0x0000002d,0x0000002c,0x000100fd,0x00010038
};

// glsl_shader.frag, compiled with:
// # glslangValidator -V -x -o glsl_shader.frag.u32 glsl_shader.frag
/*
#version 450 core
layout(location = 0) out vec4 fColor;
layout(set=0, binding=0) uniform sampler2D sTexture;
layout(location = 0) in struct { vec4 Color; vec2 UV; } In;
void main()
{
	fColor = In.Color * texture(sTexture, In.UV.st);
}
*/
static uint32_t ImGuiFragmentShader[] =
{
	0x07230203,0x00010000,0x00080001,0x0000001e,0x00000000,0x00020011,0x00000001,0x0006000b,
	0x00000001,0x4c534c47,0x6474732e,0x3035342e,0x00000000,0x0003000e,0x00000000,0x00000001,
	0x0007000f,0x00000004,0x00000004,0x6e69616d,0x00000000,0x00000009,0x0000000d,0x00030010,
	0x00000004,0x00000007,0x00030003,0x00000002,0x000001c2,0x00040005,0x00000004,0x6e69616d,
	0x00000000,0x00040005,0x00000009,0x6c6f4366,0x0000726f,0x00030005,0x0000000b,0x00000000,
	0x00050006,0x0000000b,0x00000000,0x6f6c6f43,0x00000072,0x00040006,0x0000000b,0x00000001,
	0x00005655,0x00030005,0x0000000d,0x00006e49,0x00050005,0x00000016,0x78655473,0x65727574,
	0x00000000,0x00040047,0x00000009,0x0000001e,0x00000000,0x00040047,0x0000000d,0x0000001e,
	0x00000000,0x00040047,0x00000016,0x00000022,0x00000000,0x00040047,0x00000016,0x00000021,
	0x00000000,0x00020013,0x00000002,0x00030021,0x00000003,0x00000002,0x00030016,0x00000006,
	0x00000020,0x00040017,0x00000007,0x00000006,0x00000004,0x00040020,0x00000008,0x00000003,
	0x00000007,0x0004003b,0x00000008,0x00000009,0x00000003,0x00040017,0x0000000a,0x00000006,
	0x00000002,0x0004001e,0x0000000b,0x00000007,0x0000000a,0x00040020,0x0000000c,0x00000001,
	0x0000000b,0x0004003b,0x0000000c,0x0000000d,0x00000001,0x00040015,0x0000000e,0x00000020,
	0x00000001,0x0004002b,0x0000000e,0x0000000f,0x00000000,0x00040020,0x00000010,0x00000001,
	0x00000007,0x00090019,0x00000013,0x00000006,0x00000001,0x00000000,0x00000000,0x00000000,
	0x00000001,0x00000000,0x0003001b,0x00000014,0x00000013,0x00040020,0x00000015,0x00000000,
	0x00000014,0x0004003b,0x00000015,0x00000016,0x00000000,0x0004002b,0x0000000e,0x00000018,
	0x00000001,0x00040020,0x00000019,0x00000001,0x0000000a,0x00050036,0x00000002,0x00000004,
	0x00000000,0x00000003,0x000200f8,0x00000005,0x00050041,0x00000010,0x00000011,0x0000000d,
	0x0000000f,0x0004003d,0x00000007,0x00000012,0x00000011,0x0004003d,0x00000014,0x00000017,
	0x00000016,0x00050041,0x00000019,0x0000001a,0x0000000d,0x00000018,0x0004003d,0x0000000a,
	0x0000001b,0x0000001a,0x00050057,0x00000007,0x0000001c,0x00000017,0x0000001b,0x00050085,
	0x00000007,0x0000001d,0x00000012,0x0000001c,0x0003003e,0x00000009,0x0000001d,0x000100fd,
	0x00010038
};

typedef struct
{
	VkRenderPass RenderPass;
	VkDescriptorPool DescriptorPool;
	VkDevice Device;
	VkPhysicalDevice PhysicalDevice;
	uint32_t ImageCount;
	VkSampleCountFlagBits MsaaSamples;
} ImGui_ImplVulkan_InitInfo;

typedef struct
{
	VkDeviceMemory VertexBufferMemory;
	VkDeviceMemory IndexBufferMemory;
	VkDeviceSize VertexBufferSize;
	VkDeviceSize IndexBufferSize;
	VkBuffer VertexBuffer;
	VkBuffer IndexBuffer;
} ImGui_ImplVulkan_Buffers;

struct
{
	VkRenderPass RenderPass;
	VkDescriptorPool DescriptorPool;
	VkDevice Device;
	VkPhysicalDevice PhysicalDevice;
	uint32_t ImageCount;
	VkSampleCountFlagBits MsaaSamples;

	VkPipeline Pipeline;
	VkPipeline OpaquePipeline;
	VkShaderModule VertexShader;
	VkShaderModule FragmentShader;

	VkSampler Sampler;

	VkDescriptorSetLayout DescriptorSetLayout;
	VkDescriptorSet DescriptorSet;

	VkPipelineLayout PipelineLayout;

	VkImage FontImage;
	VkDeviceMemory FontImageMemory;
	VkImageView FontImageView;
	VkDeviceMemory FontUploadBufferMemory;
	VkBuffer FontUploadBuffer;

	bool SetNULL;
	uint32_t Index;
	ImGui_ImplVulkan_Buffers* Buffers;

	bool LastPipeline;//0 = Opaque, 1 = Transparent
	bool LastDescriptorSet;//0 = Font, 1 = Some other
} ImGui_ImplVulkan_Renderer_Info;

VkDeviceSize ImGui_BufferMemoryAlignment = 256;

uint32_t ImGui_ImplVulkan_MemoryType(VkMemoryPropertyFlags PropertyFlags, uint32_t TypeBits)
{
	VkPhysicalDeviceMemoryProperties MemoryProperties;
	vkGetPhysicalDeviceMemoryProperties(ImGui_ImplVulkan_Renderer_Info.PhysicalDevice, &MemoryProperties);

	for (uint32_t i = 0; i < MemoryProperties.memoryTypeCount; i++)
		if ((MemoryProperties.memoryTypes[i].propertyFlags & PropertyFlags) == PropertyFlags && TypeBits & (1 << i))
			return i;

	return 0xFFFFFFFF;
}

bool ImGui_ImplVulkanPrintError(const char* Msg)
{
	printf(Msg);
	return false;
}

bool ImGui_ImplVulkan_CreateFontsTexture(VkCommandBuffer CommandBuffer)
{
	ImGuiIO* IO = &ImGui::GetIO();

	unsigned char* Pixel;
	int Width;
	int Height;
	IO->Fonts->GetTexDataAsRGBA32(&Pixel, &Width, &Height);
	size_t UploadSize = Width * Height * 4 * sizeof(char);

	// Create the Image
	VkImageCreateInfo ImageCreateInfo;
	ImageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	ImageCreateInfo.pNext = NULL;
	ImageCreateInfo.flags = 0;
	ImageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	ImageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
	ImageCreateInfo.format = VK_FORMAT_R8G8B8A8_UNORM;
	ImageCreateInfo.extent.width = Width;
	ImageCreateInfo.extent.height = Height;
	ImageCreateInfo.extent.depth = 1;
	ImageCreateInfo.mipLevels = 1;
	ImageCreateInfo.arrayLayers = 1;
	ImageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
	ImageCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
	ImageCreateInfo.usage = VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
	ImageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	ImageCreateInfo.queueFamilyIndexCount = 0;
	ImageCreateInfo.pQueueFamilyIndices = NULL;
	ImageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

	if (vkCreateImage(ImGui_ImplVulkan_Renderer_Info.Device, &ImageCreateInfo, NULL, &ImGui_ImplVulkan_Renderer_Info.FontImage) != VK_SUCCESS)
		return ImGui_ImplVulkanPrintError("[ImGui Vulkan] Failed to Create Font Image");

	VkMemoryRequirements MemoryRequirements;
	vkGetImageMemoryRequirements(ImGui_ImplVulkan_Renderer_Info.Device, ImGui_ImplVulkan_Renderer_Info.FontImage, &MemoryRequirements);

	VkMemoryAllocateInfo MemoryAllocateInfo;
	MemoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	MemoryAllocateInfo.pNext = NULL;
	MemoryAllocateInfo.allocationSize = MemoryRequirements.size;
	MemoryAllocateInfo.memoryTypeIndex = ImGui_ImplVulkan_MemoryType(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, MemoryRequirements.memoryTypeBits);

	if (vkAllocateMemory(ImGui_ImplVulkan_Renderer_Info.Device, &MemoryAllocateInfo, NULL, &ImGui_ImplVulkan_Renderer_Info.FontImageMemory) != VK_SUCCESS)
		return ImGui_ImplVulkanPrintError("[ImGui Vulkan] Failed to Allocate Font Image Memory");

	if (vkBindImageMemory(ImGui_ImplVulkan_Renderer_Info.Device, ImGui_ImplVulkan_Renderer_Info.FontImage, ImGui_ImplVulkan_Renderer_Info.FontImageMemory, 0) != VK_SUCCESS)
		return ImGui_ImplVulkanPrintError("[ImGui Vulkan] Failed to Bind Font Image Memory");

	// Create the Image View
	VkImageViewCreateInfo ImageViewCreateInfo;
	ImageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	ImageViewCreateInfo.pNext = NULL;
	ImageViewCreateInfo.flags = 0;
	ImageViewCreateInfo.image = ImGui_ImplVulkan_Renderer_Info.FontImage;
	ImageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
	ImageViewCreateInfo.format = VK_FORMAT_R8G8B8A8_UNORM;
	ImageViewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
	ImageViewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
	ImageViewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
	ImageViewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
	ImageViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	ImageViewCreateInfo.subresourceRange.baseMipLevel = 0;
	ImageViewCreateInfo.subresourceRange.levelCount = 1;
	ImageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
	ImageViewCreateInfo.subresourceRange.layerCount = 1;

	if (vkCreateImageView(ImGui_ImplVulkan_Renderer_Info.Device, &ImageViewCreateInfo, NULL, &ImGui_ImplVulkan_Renderer_Info.FontImageView) != VK_SUCCESS)
		return ImGui_ImplVulkanPrintError("[ImGui Vulkan] Failed to Create Font Image View");

	// Update the Descriptor Set
	VkDescriptorImageInfo DescriptorImageInfo;
	DescriptorImageInfo.sampler = ImGui_ImplVulkan_Renderer_Info.Sampler;
	DescriptorImageInfo.imageView = ImGui_ImplVulkan_Renderer_Info.FontImageView;
	DescriptorImageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

	VkWriteDescriptorSet WriteDescriptorSet;
	WriteDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	WriteDescriptorSet.pNext = NULL;
	WriteDescriptorSet.dstSet = ImGui_ImplVulkan_Renderer_Info.DescriptorSet;
	WriteDescriptorSet.dstBinding = 0;
	WriteDescriptorSet.dstArrayElement = 0;
	WriteDescriptorSet.descriptorCount = 1;
	WriteDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	WriteDescriptorSet.pImageInfo = &DescriptorImageInfo;
	WriteDescriptorSet.pBufferInfo = NULL;
	WriteDescriptorSet.pTexelBufferView = NULL;

	vkUpdateDescriptorSets(ImGui_ImplVulkan_Renderer_Info.Device, 1, &WriteDescriptorSet, 0, NULL);

	// Create the Upload Buffer
	VkBufferCreateInfo BufferCreateInfo;
	BufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	BufferCreateInfo.pNext = NULL;
	BufferCreateInfo.flags = 0;
	BufferCreateInfo.size = UploadSize;
	BufferCreateInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
	BufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	BufferCreateInfo.queueFamilyIndexCount = 0;
	BufferCreateInfo.pQueueFamilyIndices = NULL;

	if (vkCreateBuffer(ImGui_ImplVulkan_Renderer_Info.Device, &BufferCreateInfo, NULL, &ImGui_ImplVulkan_Renderer_Info.FontUploadBuffer) != VK_SUCCESS)
		return ImGui_ImplVulkanPrintError("[ImGui Vulkan] Failed to Create Font Upload Buffer");

	vkGetBufferMemoryRequirements(ImGui_ImplVulkan_Renderer_Info.Device, ImGui_ImplVulkan_Renderer_Info.FontUploadBuffer, &MemoryRequirements);

	ImGui_BufferMemoryAlignment = (ImGui_BufferMemoryAlignment > MemoryRequirements.alignment) ? ImGui_BufferMemoryAlignment : MemoryRequirements.alignment;

	MemoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	MemoryAllocateInfo.pNext = NULL;
	MemoryAllocateInfo.allocationSize = MemoryRequirements.size;
	MemoryAllocateInfo.memoryTypeIndex = ImGui_ImplVulkan_MemoryType(VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, MemoryRequirements.memoryTypeBits);

	if (vkAllocateMemory(ImGui_ImplVulkan_Renderer_Info.Device, &MemoryAllocateInfo, NULL, &ImGui_ImplVulkan_Renderer_Info.FontUploadBufferMemory) != VK_SUCCESS)
		return ImGui_ImplVulkanPrintError("[ImGui Vulkan] Failed to Allocate Font Upload Buffer Memory");

	if (vkBindBufferMemory(ImGui_ImplVulkan_Renderer_Info.Device, ImGui_ImplVulkan_Renderer_Info.FontUploadBuffer, ImGui_ImplVulkan_Renderer_Info.FontUploadBufferMemory, 0) != VK_SUCCESS)
		return ImGui_ImplVulkanPrintError("[ImGui Vulkan] Failed to Bind Font Upload Buffer Memory");

	// Upload to Buffer
	char* Data = NULL;
	if (vkMapMemory(ImGui_ImplVulkan_Renderer_Info.Device, ImGui_ImplVulkan_Renderer_Info.FontUploadBufferMemory, 0, UploadSize, 0, (void**)(&Data)) != VK_SUCCESS)
		return ImGui_ImplVulkanPrintError("[ImGui Vulkan] Failed to Map Font Memory");

	memcpy(Data, Pixel, UploadSize);

	VkMappedMemoryRange MappedMemoryRange;
	MappedMemoryRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
	MappedMemoryRange.pNext = NULL;
	MappedMemoryRange.memory = ImGui_ImplVulkan_Renderer_Info.FontUploadBufferMemory;
	MappedMemoryRange.offset = 0;
	MappedMemoryRange.size = UploadSize;

	if (vkFlushMappedMemoryRanges(ImGui_ImplVulkan_Renderer_Info.Device, 1, &MappedMemoryRange) != VK_SUCCESS)
		return ImGui_ImplVulkanPrintError("[ImGui Vulkan] Failed to Flush Mapped Memory Ranges");

	vkUnmapMemory(ImGui_ImplVulkan_Renderer_Info.Device, ImGui_ImplVulkan_Renderer_Info.FontUploadBufferMemory);

	// Copy to Image
	VkImageMemoryBarrier ImageMemoryBarrier;
	ImageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	ImageMemoryBarrier.pNext = NULL;
	ImageMemoryBarrier.srcAccessMask = 0;
	ImageMemoryBarrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
	ImageMemoryBarrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	ImageMemoryBarrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
	ImageMemoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	ImageMemoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	ImageMemoryBarrier.image = ImGui_ImplVulkan_Renderer_Info.FontImage;
	ImageMemoryBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	ImageMemoryBarrier.subresourceRange.baseMipLevel = 0;
	ImageMemoryBarrier.subresourceRange.levelCount = 1;
	ImageMemoryBarrier.subresourceRange.baseArrayLayer = 0;
	ImageMemoryBarrier.subresourceRange.layerCount = 1;


	vkCmdPipelineBarrier(CommandBuffer, VK_PIPELINE_STAGE_HOST_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, NULL, 0, NULL, 1, &ImageMemoryBarrier);

	VkBufferImageCopy BufferImageCopy;
	BufferImageCopy.bufferOffset = 0;
	BufferImageCopy.bufferRowLength = 0;
	BufferImageCopy.bufferImageHeight = 0;
	BufferImageCopy.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	BufferImageCopy.imageSubresource.mipLevel = 0;
	BufferImageCopy.imageSubresource.baseArrayLayer = 0;
	BufferImageCopy.imageSubresource.layerCount = 1;
	BufferImageCopy.imageOffset = {};
	BufferImageCopy.imageExtent.width = Width;
	BufferImageCopy.imageExtent.height = Height;
	BufferImageCopy.imageExtent.depth = 1;

	vkCmdCopyBufferToImage(CommandBuffer, ImGui_ImplVulkan_Renderer_Info.FontUploadBuffer, ImGui_ImplVulkan_Renderer_Info.FontImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &BufferImageCopy);

	ImageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	ImageMemoryBarrier.pNext = NULL;
	ImageMemoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
	ImageMemoryBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
	ImageMemoryBarrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
	ImageMemoryBarrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	ImageMemoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	ImageMemoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	ImageMemoryBarrier.image = ImGui_ImplVulkan_Renderer_Info.FontImage;
	ImageMemoryBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	ImageMemoryBarrier.subresourceRange.baseMipLevel = 0;
	ImageMemoryBarrier.subresourceRange.levelCount = 1;
	ImageMemoryBarrier.subresourceRange.baseArrayLayer = 0;
	ImageMemoryBarrier.subresourceRange.layerCount = 1;
	vkCmdPipelineBarrier(CommandBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0, NULL, 0, NULL, 1, &ImageMemoryBarrier);

	IO->Fonts->SetTexID((ImTextureID)&ImGui_ImplVulkan_Renderer_Info.DescriptorSet);

	return 1;
}

void ImGui_ImplVulkan_DestroyFontUploadObjects()
{
	vkDestroyBuffer(ImGui_ImplVulkan_Renderer_Info.Device, ImGui_ImplVulkan_Renderer_Info.FontUploadBuffer, NULL);
	ImGui_ImplVulkan_Renderer_Info.FontUploadBuffer = VK_NULL_HANDLE;

	vkFreeMemory(ImGui_ImplVulkan_Renderer_Info.Device, ImGui_ImplVulkan_Renderer_Info.FontUploadBufferMemory, NULL);
	ImGui_ImplVulkan_Renderer_Info.FontUploadBufferMemory = VK_NULL_HANDLE;
}

void ImGui_ImplVulkan_NewFrame()
{

}

bool ImGui_CreateOrResizeBuffer(VkBuffer* Buffer, VkDeviceMemory* BufferMemory, VkDeviceSize* BufferSize, size_t NewBufferSize, VkBufferUsageFlagBits Usage)
{
	if (*Buffer != VK_NULL_HANDLE)
		vkDestroyBuffer(ImGui_ImplVulkan_Renderer_Info.Device, *Buffer, NULL);
	if (*BufferMemory != VK_NULL_HANDLE)
		vkFreeMemory(ImGui_ImplVulkan_Renderer_Info.Device, *BufferMemory, NULL);

	VkDeviceSize VertexBufferSizeAligned = ((NewBufferSize - 1) / ImGui_BufferMemoryAlignment + 1) * ImGui_BufferMemoryAlignment;

	VkBufferCreateInfo BufferCreateInfo;
	BufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	BufferCreateInfo.pNext = NULL;
	BufferCreateInfo.flags = 0;
	BufferCreateInfo.size = VertexBufferSizeAligned;
	BufferCreateInfo.usage = Usage;
	BufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	BufferCreateInfo.queueFamilyIndexCount = 0;
	BufferCreateInfo.pQueueFamilyIndices = NULL;

	if (vkCreateBuffer(ImGui_ImplVulkan_Renderer_Info.Device, &BufferCreateInfo, NULL, Buffer) != VK_SUCCESS)
		return ImGui_ImplVulkanPrintError("[ImGui Vulkan] Failed to Create Vertex Buffer");

	VkMemoryRequirements MemoryRequirements;
	vkGetBufferMemoryRequirements(ImGui_ImplVulkan_Renderer_Info.Device, *Buffer, &MemoryRequirements);

	ImGui_BufferMemoryAlignment = (ImGui_BufferMemoryAlignment > MemoryRequirements.alignment) ? ImGui_BufferMemoryAlignment : MemoryRequirements.alignment;

	VkMemoryAllocateInfo MemoryAllocateInfo;
	MemoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	MemoryAllocateInfo.pNext = NULL;
	MemoryAllocateInfo.allocationSize = MemoryRequirements.size;
	MemoryAllocateInfo.memoryTypeIndex = ImGui_ImplVulkan_MemoryType(VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, MemoryRequirements.memoryTypeBits);

	if (vkAllocateMemory(ImGui_ImplVulkan_Renderer_Info.Device, &MemoryAllocateInfo, NULL, BufferMemory) != VK_SUCCESS)
		return ImGui_ImplVulkanPrintError("[ImGui Vulkan] Failed to Allocate Vertex Buffer Memory");

	if (vkBindBufferMemory(ImGui_ImplVulkan_Renderer_Info.Device, *Buffer, *BufferMemory, 0) != VK_SUCCESS)
		return ImGui_ImplVulkanPrintError("[ImGui Vulkan] Failed to Bind Vertex Buffer Memory");

	*BufferSize = MemoryRequirements.size;

	return 1;
}

void ImGui_SetupRenderState(ImDrawData* DrawData, VkCommandBuffer CommandBuffer, int FramebufferWidth, int FramebufferHeight)
{
	vkCmdBindPipeline(CommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, ImGui_ImplVulkan_Renderer_Info.Pipeline);
	ImGui_ImplVulkan_Renderer_Info.LastPipeline = true;
	vkCmdBindDescriptorSets(CommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, ImGui_ImplVulkan_Renderer_Info.PipelineLayout, 0, 1, &ImGui_ImplVulkan_Renderer_Info.DescriptorSet, 0, NULL);
	ImGui_ImplVulkan_Renderer_Info.LastDescriptorSet = false;

	if (DrawData->TotalVtxCount > 0)
	{
		VkDeviceSize VertexOffset[1] = { 0 };
		vkCmdBindVertexBuffers(CommandBuffer, 0, 1, &ImGui_ImplVulkan_Renderer_Info.Buffers[ImGui_ImplVulkan_Renderer_Info.Index].VertexBuffer, VertexOffset);
		vkCmdBindIndexBuffer(CommandBuffer, ImGui_ImplVulkan_Renderer_Info.Buffers[ImGui_ImplVulkan_Renderer_Info.Index].IndexBuffer, 0, sizeof(ImDrawIdx) == 2 ? VK_INDEX_TYPE_UINT16 : VK_INDEX_TYPE_UINT32);
	}

	VkViewport Viewport;
	Viewport.x = 0;
	Viewport.y = 0;
	Viewport.width = (float)FramebufferWidth;
	Viewport.height = (float)FramebufferHeight;
	Viewport.minDepth = 0.0f;
	Viewport.maxDepth = 1.0f;
	vkCmdSetViewport(CommandBuffer, 0, 1, &Viewport);

	float Transform[4];
	Transform[0] = 2.0f / DrawData->DisplaySize.x;
	Transform[1] = 2.0f / DrawData->DisplaySize.y;
	Transform[2] = -1.0f - DrawData->DisplayPos.x * Transform[0];
	Transform[3] = -1.0f - DrawData->DisplayPos.y * Transform[1];

	vkCmdPushConstants(CommandBuffer, ImGui_ImplVulkan_Renderer_Info.PipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(float) * 4, Transform);
}

bool ImGui_ImplVulkan_RenderDrawData(ImDrawData* DrawData, VkCommandBuffer CommandBuffer, int NonAlphaTextureCount, ImTextureID* NonAlphaTextures)
{
	int FramebufferWidth = (int)(DrawData->DisplaySize.x * DrawData->FramebufferScale.x);
	int FramebufferHeight = (int)(DrawData->DisplaySize.y * DrawData->FramebufferScale.y);

	if (FramebufferWidth <= 0 || FramebufferHeight <= 0)
		return 0;

	if (ImGui_ImplVulkan_Renderer_Info.SetNULL)
	{
		ImGui_ImplVulkan_Renderer_Info.SetNULL = false;
		ImGui_ImplVulkan_Renderer_Info.Index = 0;

		ImGui_ImplVulkan_Renderer_Info.Buffers = (ImGui_ImplVulkan_Buffers*)malloc(ImGui_ImplVulkan_Renderer_Info.ImageCount * sizeof(ImGui_ImplVulkan_Buffers));
		if (!ImGui_ImplVulkan_Renderer_Info.Buffers)
		{
			return ImGui_ImplVulkanPrintError("[ImGui Vulkan] Failed to Allocate Buffers");
		}
		memset(ImGui_ImplVulkan_Renderer_Info.Buffers, 0, ImGui_ImplVulkan_Renderer_Info.ImageCount * sizeof(ImGui_ImplVulkan_Buffers));
	}

	ImGui_ImplVulkan_Renderer_Info.Index = (ImGui_ImplVulkan_Renderer_Info.Index + 1) % ImGui_ImplVulkan_Renderer_Info.ImageCount;

	if (DrawData->TotalVtxCount > 0)
	{
		size_t VertexSize = DrawData->TotalVtxCount * sizeof(ImDrawVert);
		size_t IndexSize = DrawData->TotalIdxCount * sizeof(ImDrawIdx);

		if (ImGui_ImplVulkan_Renderer_Info.Buffers[ImGui_ImplVulkan_Renderer_Info.Index].VertexBuffer == VK_NULL_HANDLE || ImGui_ImplVulkan_Renderer_Info.Buffers[ImGui_ImplVulkan_Renderer_Info.Index].VertexBufferSize < VertexSize)
			ImGui_CreateOrResizeBuffer(&ImGui_ImplVulkan_Renderer_Info.Buffers[ImGui_ImplVulkan_Renderer_Info.Index].VertexBuffer, &ImGui_ImplVulkan_Renderer_Info.Buffers[ImGui_ImplVulkan_Renderer_Info.Index].VertexBufferMemory, &ImGui_ImplVulkan_Renderer_Info.Buffers[ImGui_ImplVulkan_Renderer_Info.Index].VertexBufferSize, VertexSize, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);
		if (ImGui_ImplVulkan_Renderer_Info.Buffers[ImGui_ImplVulkan_Renderer_Info.Index].IndexBuffer == VK_NULL_HANDLE || ImGui_ImplVulkan_Renderer_Info.Buffers[ImGui_ImplVulkan_Renderer_Info.Index].IndexBufferSize < IndexSize)
			ImGui_CreateOrResizeBuffer(&ImGui_ImplVulkan_Renderer_Info.Buffers[ImGui_ImplVulkan_Renderer_Info.Index].IndexBuffer, &ImGui_ImplVulkan_Renderer_Info.Buffers[ImGui_ImplVulkan_Renderer_Info.Index].IndexBufferMemory, &ImGui_ImplVulkan_Renderer_Info.Buffers[ImGui_ImplVulkan_Renderer_Info.Index].IndexBufferSize, IndexSize, VK_BUFFER_USAGE_INDEX_BUFFER_BIT);

		ImDrawVert* VertexDst = NULL;
		ImDrawIdx* IndexDst = NULL;

		if (vkMapMemory(ImGui_ImplVulkan_Renderer_Info.Device, ImGui_ImplVulkan_Renderer_Info.Buffers[ImGui_ImplVulkan_Renderer_Info.Index].VertexBufferMemory, 0, ImGui_ImplVulkan_Renderer_Info.Buffers[ImGui_ImplVulkan_Renderer_Info.Index].VertexBufferSize, 0, (void**)(&VertexDst)) != VK_SUCCESS)
			return ImGui_ImplVulkanPrintError("[ImGui Vulkan] Failed to Map Vertex Buffer Memory");

		if (vkMapMemory(ImGui_ImplVulkan_Renderer_Info.Device, ImGui_ImplVulkan_Renderer_Info.Buffers[ImGui_ImplVulkan_Renderer_Info.Index].IndexBufferMemory, 0, ImGui_ImplVulkan_Renderer_Info.Buffers[ImGui_ImplVulkan_Renderer_Info.Index].IndexBufferSize, 0, (void**)(&IndexDst)) != VK_SUCCESS)
			return ImGui_ImplVulkanPrintError("[ImGui Vulkan] Failed to Map Index Buffer Memory");

		for (int i = 0; i < DrawData->CmdListsCount; i++)
		{
			const ImDrawList* DrawList = DrawData->CmdLists[i];

			memcpy(VertexDst, DrawList->VtxBuffer.Data, DrawList->VtxBuffer.Size * sizeof(ImDrawVert));
			memcpy(IndexDst, DrawList->IdxBuffer.Data, DrawList->IdxBuffer.Size * sizeof(ImDrawIdx));

			VertexDst += DrawList->VtxBuffer.Size;
			IndexDst += DrawList->IdxBuffer.Size;
		}

		VkMappedMemoryRange MappedMemoryRange[2];
		MappedMemoryRange[0].sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
		MappedMemoryRange[0].pNext = NULL;
		MappedMemoryRange[0].memory = ImGui_ImplVulkan_Renderer_Info.Buffers[ImGui_ImplVulkan_Renderer_Info.Index].VertexBufferMemory;
		MappedMemoryRange[0].offset = 0;
		MappedMemoryRange[0].size = VK_WHOLE_SIZE;

		MappedMemoryRange[1].sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
		MappedMemoryRange[1].pNext = NULL;
		MappedMemoryRange[1].memory = ImGui_ImplVulkan_Renderer_Info.Buffers[ImGui_ImplVulkan_Renderer_Info.Index].IndexBufferMemory;
		MappedMemoryRange[1].offset = 0;
		MappedMemoryRange[1].size = VK_WHOLE_SIZE;

		if (vkFlushMappedMemoryRanges(ImGui_ImplVulkan_Renderer_Info.Device, 2, MappedMemoryRange) != VK_SUCCESS)
			return ImGui_ImplVulkanPrintError("[ImGui Vulkan] Failed to Flush Vertex Index Mapped Memory Ranges");

		vkUnmapMemory(ImGui_ImplVulkan_Renderer_Info.Device, ImGui_ImplVulkan_Renderer_Info.Buffers[ImGui_ImplVulkan_Renderer_Info.Index].VertexBufferMemory);
		vkUnmapMemory(ImGui_ImplVulkan_Renderer_Info.Device, ImGui_ImplVulkan_Renderer_Info.Buffers[ImGui_ImplVulkan_Renderer_Info.Index].IndexBufferMemory);
	}


	ImGui_SetupRenderState(DrawData, CommandBuffer, FramebufferWidth, FramebufferHeight);

	ImVec2 ClipOff = DrawData->DisplayPos;
	ImVec2 ClipScale = DrawData->FramebufferScale;

	int VertexOffset = 0;
	int IndexOffset = 0;

	for (int j = 0; j < DrawData->CmdListsCount; j++)
	{
		const ImDrawList* DrawList = DrawData->CmdLists[j];

		for (int i = 0; i < DrawList->CmdBuffer.Size; i++)
		{
			const ImDrawCmd* DrawCmd = &DrawList->CmdBuffer[i];

			if (DrawCmd->UserCallback != NULL)
			{
				// User callback, registered via ImDrawList::AddCallback()
				if (DrawCmd->UserCallback == ImDrawCallback_ResetRenderState)
					ImGui_SetupRenderState(DrawData, CommandBuffer, FramebufferWidth, FramebufferHeight);
				else
					DrawCmd->UserCallback(DrawList, DrawCmd);
			}
			else
			{
				// Project scissor/clipping rectangles into framebuffer space
				ImVec2 ClipMin((DrawCmd->ClipRect.x - ClipOff.x) * ClipScale.x, (DrawCmd->ClipRect.y - ClipOff.y) * ClipScale.y);
				ImVec2 ClipMax((DrawCmd->ClipRect.z - ClipOff.x) * ClipScale.x, (DrawCmd->ClipRect.w - ClipOff.y) * ClipScale.y);

				// Clamp to viewport as vkCmdSetScissor() won't accept values that are off bounds
				if (ClipMin.x < 0.0f) ClipMin.x = 0.0f;
				if (ClipMin.y < 0.0f) ClipMin.y = 0.0f;
				if (ClipMax.x > FramebufferWidth) ClipMax.x = (float)FramebufferWidth;
				if (ClipMax.y > FramebufferHeight) ClipMax.y = (float)FramebufferHeight;
				if (ClipMax.x <= ClipMin.x || ClipMax.y <= ClipMin.y) continue;

				// Apply scissor/clipping rectangle
				VkRect2D Scissor;
				Scissor.offset.x = (int32_t)(ClipMin.x);
				Scissor.offset.y = (int32_t)(ClipMin.y);
				Scissor.extent.width = (uint32_t)(ClipMax.x - ClipMin.x);
				Scissor.extent.height = (uint32_t)(ClipMax.y - ClipMin.y);
				vkCmdSetScissor(CommandBuffer, 0, 1, &Scissor);

				// Draw
				if (!ImGui_ImplVulkan_Renderer_Info.LastPipeline)
				{
					vkCmdBindPipeline(CommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, ImGui_ImplVulkan_Renderer_Info.Pipeline);
					ImGui_ImplVulkan_Renderer_Info.LastPipeline = true;
				}					

				for (int i = 0; i < NonAlphaTextureCount; i++)
				{
					if (NonAlphaTextures[i] == DrawCmd->TextureId)
					{
						vkCmdBindPipeline(CommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, ImGui_ImplVulkan_Renderer_Info.OpaquePipeline);
						ImGui_ImplVulkan_Renderer_Info.LastPipeline = false;
						break;
					}
				}
				
				ImTextureID Set0 = DrawCmd->TextureId;
				ImTextureID Set1 = &ImGui_ImplVulkan_Renderer_Info.DescriptorSet;

				if (!ImGui_ImplVulkan_Renderer_Info.LastDescriptorSet && Set0 == Set1)
				{}
				else
				{
					vkCmdBindDescriptorSets(CommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, ImGui_ImplVulkan_Renderer_Info.PipelineLayout, 0, 1, (VkDescriptorSet*)DrawCmd->TextureId, 0, NULL);
					if (DrawCmd->TextureId == ImGui_ImplVulkan_Renderer_Info.DescriptorSet)
						ImGui_ImplVulkan_Renderer_Info.LastDescriptorSet = false;
					else
						ImGui_ImplVulkan_Renderer_Info.LastDescriptorSet = true;
				}

				vkCmdDrawIndexed(CommandBuffer, DrawCmd->ElemCount, 1, DrawCmd->IdxOffset + IndexOffset, DrawCmd->VtxOffset + VertexOffset, 0);
			}
		}
		IndexOffset += DrawList->IdxBuffer.Size;
		VertexOffset += DrawList->VtxBuffer.Size;
	}

	VkRect2D Scissor =
	{
		{ 0, 0 },
		{
			(uint32_t)FramebufferWidth,
			(uint32_t)FramebufferHeight
		}
	};
	vkCmdSetScissor(CommandBuffer, 0, 1, &Scissor);

	return 1;
}

bool ImGui_CreateSampler()
{
	VkSamplerCreateInfo SamplerInfo;
	SamplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	SamplerInfo.pNext = NULL;
	SamplerInfo.flags = 0;
	SamplerInfo.magFilter = VK_FILTER_NEAREST;
	SamplerInfo.minFilter = VK_FILTER_NEAREST;
	SamplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_NEAREST;
	SamplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	SamplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	SamplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	SamplerInfo.mipLodBias = 0.0;
	SamplerInfo.anisotropyEnable = VK_FALSE;
	SamplerInfo.maxAnisotropy = 1;
	SamplerInfo.compareEnable = VK_FALSE;
	SamplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
	SamplerInfo.minLod = 0.0;
	SamplerInfo.maxLod = 1.0;
	SamplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
	SamplerInfo.unnormalizedCoordinates = VK_FALSE;

	if (vkCreateSampler(ImGui_ImplVulkan_Renderer_Info.Device, &SamplerInfo, NULL, &ImGui_ImplVulkan_Renderer_Info.Sampler) != VK_SUCCESS)
		return ImGui_ImplVulkanPrintError("[ImGui Vulkan] Failed to Create Sampler");

	return 1;
}

bool ImGui_CreateDescriptorSets()
{
	VkDescriptorSetLayoutBinding LayoutBinding;

	LayoutBinding.binding = 0;
	LayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	LayoutBinding.descriptorCount = 1;
	LayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
	LayoutBinding.pImmutableSamplers = &ImGui_ImplVulkan_Renderer_Info.Sampler;

	VkDescriptorSetLayoutCreateInfo DescriptorSetLayoutCreateInfo;
	DescriptorSetLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	DescriptorSetLayoutCreateInfo.pNext = NULL;
	DescriptorSetLayoutCreateInfo.flags = 0;
	DescriptorSetLayoutCreateInfo.bindingCount = 1;
	DescriptorSetLayoutCreateInfo.pBindings = &LayoutBinding;

	if (vkCreateDescriptorSetLayout(ImGui_ImplVulkan_Renderer_Info.Device, &DescriptorSetLayoutCreateInfo, NULL, &ImGui_ImplVulkan_Renderer_Info.DescriptorSetLayout) != VK_SUCCESS)
		return ImGui_ImplVulkanPrintError("[ImGui Vulkan] Failed to Create Sampler");

	VkDescriptorSetAllocateInfo AllocateInfo;
	AllocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	AllocateInfo.pNext = NULL;
	AllocateInfo.descriptorPool = ImGui_ImplVulkan_Renderer_Info.DescriptorPool;
	AllocateInfo.descriptorSetCount = 1;
	AllocateInfo.pSetLayouts = &ImGui_ImplVulkan_Renderer_Info.DescriptorSetLayout;

	if (vkAllocateDescriptorSets(ImGui_ImplVulkan_Renderer_Info.Device, &AllocateInfo, &ImGui_ImplVulkan_Renderer_Info.DescriptorSet) != VK_SUCCESS)
		return ImGui_ImplVulkanPrintError("[ImGui Vulkan] Failed to Allocate Descriptor Set");

	return 1;
}

bool ImGui_CreatePipelineLayout()
{
	VkPushConstantRange PushConstant;
	PushConstant.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
	PushConstant.offset = 0;
	PushConstant.size = 16;

	VkPipelineLayoutCreateInfo LayoutCreateInfo;
	LayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	LayoutCreateInfo.pNext = NULL;
	LayoutCreateInfo.flags = 0;
	LayoutCreateInfo.setLayoutCount = 1;
	LayoutCreateInfo.pSetLayouts = &ImGui_ImplVulkan_Renderer_Info.DescriptorSetLayout;
	LayoutCreateInfo.pushConstantRangeCount = 1;
	LayoutCreateInfo.pPushConstantRanges = &PushConstant;

	if (vkCreatePipelineLayout(ImGui_ImplVulkan_Renderer_Info.Device, &LayoutCreateInfo, NULL, &ImGui_ImplVulkan_Renderer_Info.PipelineLayout) != VK_SUCCESS)
		return ImGui_ImplVulkanPrintError("[ImGui Vulkan] Failed to Create Pipeline Layout");

	return 1;
}

bool ImGui_CreateShaderModules()
{
	VkShaderModuleCreateInfo CreateInfo;
	CreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	CreateInfo.pNext = NULL;
	CreateInfo.flags = 0;
	CreateInfo.codeSize = sizeof(ImGuiVertexShader);
	CreateInfo.pCode = ImGuiVertexShader;

	if (vkCreateShaderModule(ImGui_ImplVulkan_Renderer_Info.Device, &CreateInfo, NULL, &ImGui_ImplVulkan_Renderer_Info.VertexShader) != VK_SUCCESS)
		return ImGui_ImplVulkanPrintError("[ImGui Vulkan] Failed to Create Vertex Shader Module");

	CreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	CreateInfo.pNext = NULL;
	CreateInfo.flags = 0;
	CreateInfo.codeSize = sizeof(ImGuiFragmentShader);
	CreateInfo.pCode = ImGuiFragmentShader;

	if (vkCreateShaderModule(ImGui_ImplVulkan_Renderer_Info.Device, &CreateInfo, NULL, &ImGui_ImplVulkan_Renderer_Info.FragmentShader) != VK_SUCCESS)
		return ImGui_ImplVulkanPrintError("[ImGui Vulkan] Failed to Create Fragment Shader Module");

	return 1;
}

bool ImGui_CreateGraphicsPipeline()
{
	VkPipelineShaderStageCreateInfo ShaderStages[2];
	ShaderStages[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	ShaderStages[0].pNext = NULL;
	ShaderStages[0].flags = 0;
	ShaderStages[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
	ShaderStages[0].module = ImGui_ImplVulkan_Renderer_Info.VertexShader;
	ShaderStages[0].pName = "main";
	ShaderStages[0].pSpecializationInfo = NULL;

	ShaderStages[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	ShaderStages[1].pNext = NULL;
	ShaderStages[1].flags = 0;
	ShaderStages[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	ShaderStages[1].module = ImGui_ImplVulkan_Renderer_Info.FragmentShader;
	ShaderStages[1].pName = "main";
	ShaderStages[1].pSpecializationInfo = NULL;

	VkVertexInputBindingDescription InputBindingDescription;
	InputBindingDescription.binding = 0;
	InputBindingDescription.stride = sizeof(ImDrawVert);
	InputBindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

	VkVertexInputAttributeDescription InputAttributeDescription[3];
	InputAttributeDescription[0].location = 0;
	InputAttributeDescription[0].binding = InputBindingDescription.binding;
	InputAttributeDescription[0].format = VK_FORMAT_R32G32_SFLOAT;
	InputAttributeDescription[0].offset = IM_OFFSETOF(ImDrawVert, pos);

	InputAttributeDescription[1].location = 1;
	InputAttributeDescription[1].binding = InputBindingDescription.binding;
	InputAttributeDescription[1].format = VK_FORMAT_R32G32_SFLOAT;
	InputAttributeDescription[1].offset = IM_OFFSETOF(ImDrawVert, uv);

	InputAttributeDescription[2].location = 2;
	InputAttributeDescription[2].binding = InputBindingDescription.binding;
	InputAttributeDescription[2].format = VK_FORMAT_R8G8B8A8_UNORM;
	InputAttributeDescription[2].offset = IM_OFFSETOF(ImDrawVert, col);

	VkPipelineVertexInputStateCreateInfo VertexInputStateCreateInfo;
	VertexInputStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	VertexInputStateCreateInfo.pNext = NULL;
	VertexInputStateCreateInfo.flags = 0;
	VertexInputStateCreateInfo.vertexBindingDescriptionCount = 1;
	VertexInputStateCreateInfo.pVertexBindingDescriptions = &InputBindingDescription;
	VertexInputStateCreateInfo.vertexAttributeDescriptionCount = 3;
	VertexInputStateCreateInfo.pVertexAttributeDescriptions = InputAttributeDescription;

	VkPipelineInputAssemblyStateCreateInfo InputAssemblyStateCreateInfo;
	InputAssemblyStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	InputAssemblyStateCreateInfo.pNext = NULL;
	InputAssemblyStateCreateInfo.flags = 0;
	InputAssemblyStateCreateInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	InputAssemblyStateCreateInfo.primitiveRestartEnable = VK_FALSE;

	VkPipelineViewportStateCreateInfo ViewportStateCreateInfo;
	ViewportStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	ViewportStateCreateInfo.pNext = NULL;
	ViewportStateCreateInfo.flags = 0;
	ViewportStateCreateInfo.viewportCount = 1;
	ViewportStateCreateInfo.pViewports = NULL;
	ViewportStateCreateInfo.scissorCount = 1;
	ViewportStateCreateInfo.pScissors = NULL;

	VkPipelineRasterizationStateCreateInfo RasterizationStateCreateInfo;
	RasterizationStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	RasterizationStateCreateInfo.pNext = NULL;
	RasterizationStateCreateInfo.flags = 0;
	RasterizationStateCreateInfo.depthClampEnable = VK_FALSE;
	RasterizationStateCreateInfo.rasterizerDiscardEnable = VK_FALSE;
	RasterizationStateCreateInfo.polygonMode = VK_POLYGON_MODE_FILL;
	RasterizationStateCreateInfo.cullMode = VK_CULL_MODE_NONE;
	RasterizationStateCreateInfo.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
	RasterizationStateCreateInfo.depthBiasEnable = VK_FALSE;
	RasterizationStateCreateInfo.depthBiasConstantFactor = 0.0;
	RasterizationStateCreateInfo.depthBiasClamp = 0.0;
	RasterizationStateCreateInfo.depthBiasSlopeFactor = 0.0;
	RasterizationStateCreateInfo.lineWidth = 1.0;

	VkPipelineMultisampleStateCreateInfo MultisampleStateCreateInfo;
	MultisampleStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	MultisampleStateCreateInfo.pNext = NULL;
	MultisampleStateCreateInfo.flags = 0;
	MultisampleStateCreateInfo.rasterizationSamples = ImGui_ImplVulkan_Renderer_Info.MsaaSamples;
	MultisampleStateCreateInfo.sampleShadingEnable = VK_FALSE;
	MultisampleStateCreateInfo.minSampleShading = 1.0;
	MultisampleStateCreateInfo.pSampleMask = NULL;
	MultisampleStateCreateInfo.alphaToCoverageEnable = VK_FALSE;
	MultisampleStateCreateInfo.alphaToOneEnable = VK_FALSE;

	VkPipelineDepthStencilStateCreateInfo DepthStencilStateCreateInfo;
	DepthStencilStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
	DepthStencilStateCreateInfo.pNext = NULL;
	DepthStencilStateCreateInfo.flags = 0;
	DepthStencilStateCreateInfo.depthTestEnable = VK_FALSE;
	DepthStencilStateCreateInfo.depthWriteEnable = VK_FALSE;
	DepthStencilStateCreateInfo.depthCompareOp = VK_COMPARE_OP_NEVER;
	DepthStencilStateCreateInfo.depthBoundsTestEnable = VK_FALSE;
	DepthStencilStateCreateInfo.stencilTestEnable = VK_FALSE;
	DepthStencilStateCreateInfo.front = {};
	DepthStencilStateCreateInfo.back = {};
	DepthStencilStateCreateInfo.minDepthBounds = 0.0;
	DepthStencilStateCreateInfo.maxDepthBounds = 1.0;

	VkPipelineColorBlendAttachmentState ColorBlendAttachmentState;
	ColorBlendAttachmentState.blendEnable = VK_TRUE;
	ColorBlendAttachmentState.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
	ColorBlendAttachmentState.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
	ColorBlendAttachmentState.colorBlendOp = VK_BLEND_OP_ADD;
	ColorBlendAttachmentState.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
	ColorBlendAttachmentState.dstAlphaBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
	ColorBlendAttachmentState.alphaBlendOp = VK_BLEND_OP_ADD;
	ColorBlendAttachmentState.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;

	VkPipelineColorBlendStateCreateInfo ColorBlendStateCreateInfo;
	ColorBlendStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	ColorBlendStateCreateInfo.pNext = NULL;
	ColorBlendStateCreateInfo.flags = 0;
	ColorBlendStateCreateInfo.logicOpEnable = VK_FALSE;
	ColorBlendStateCreateInfo.logicOp = VK_LOGIC_OP_CLEAR;
	ColorBlendStateCreateInfo.attachmentCount = 1;
	ColorBlendStateCreateInfo.pAttachments = &ColorBlendAttachmentState;
	ColorBlendStateCreateInfo.blendConstants[0] = 0.0;
	ColorBlendStateCreateInfo.blendConstants[1] = 0.0;
	ColorBlendStateCreateInfo.blendConstants[2] = 0.0;
	ColorBlendStateCreateInfo.blendConstants[3] = 0.0;

	VkDynamicState DynamicStates[2] = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };

	VkPipelineDynamicStateCreateInfo DynamicStateCreateInfo;
	DynamicStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	DynamicStateCreateInfo.pNext = NULL;
	DynamicStateCreateInfo.flags = 0;
	DynamicStateCreateInfo.dynamicStateCount = 2;
	DynamicStateCreateInfo.pDynamicStates = DynamicStates;

	VkGraphicsPipelineCreateInfo PipelineCreateInfo;
	PipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	PipelineCreateInfo.pNext = NULL;
	PipelineCreateInfo.flags = 0;
	PipelineCreateInfo.stageCount = 2;
	PipelineCreateInfo.pStages = ShaderStages;
	PipelineCreateInfo.pVertexInputState = &VertexInputStateCreateInfo;
	PipelineCreateInfo.pInputAssemblyState = &InputAssemblyStateCreateInfo;
	PipelineCreateInfo.pTessellationState = NULL;
	PipelineCreateInfo.pViewportState = &ViewportStateCreateInfo;
	PipelineCreateInfo.pRasterizationState = &RasterizationStateCreateInfo;
	PipelineCreateInfo.pMultisampleState = &MultisampleStateCreateInfo;
	PipelineCreateInfo.pDepthStencilState = &DepthStencilStateCreateInfo;
	PipelineCreateInfo.pColorBlendState = &ColorBlendStateCreateInfo;
	PipelineCreateInfo.pDynamicState = &DynamicStateCreateInfo;
	PipelineCreateInfo.layout = ImGui_ImplVulkan_Renderer_Info.PipelineLayout;
	PipelineCreateInfo.renderPass = ImGui_ImplVulkan_Renderer_Info.RenderPass;
	PipelineCreateInfo.subpass = NULL;
	PipelineCreateInfo.basePipelineHandle = NULL;
	PipelineCreateInfo.basePipelineIndex = 0;

	if (vkCreateGraphicsPipelines(ImGui_ImplVulkan_Renderer_Info.Device, NULL, 1, &PipelineCreateInfo, NULL, &ImGui_ImplVulkan_Renderer_Info.Pipeline) != VK_SUCCESS)
		return ImGui_ImplVulkanPrintError("[ImGui Vulkan] Failed to Create Graphics Pipeline");

	ColorBlendAttachmentState.blendEnable = VK_FALSE;
	if (vkCreateGraphicsPipelines(ImGui_ImplVulkan_Renderer_Info.Device, NULL, 1, &PipelineCreateInfo, NULL, &ImGui_ImplVulkan_Renderer_Info.OpaquePipeline) != VK_SUCCESS)
		return ImGui_ImplVulkanPrintError("[ImGui Vulkan] Failed to Create Opaque Graphics Pipeline");

	return 1;
}

void ImGui_ImplVulkan_Init(ImGui_ImplVulkan_InitInfo* InitInfo)
{
	ImGui_ImplVulkan_Renderer_Info.SetNULL = true;
	ImGui_ImplVulkan_Renderer_Info.RenderPass = InitInfo->RenderPass;
	ImGui_ImplVulkan_Renderer_Info.DescriptorPool = InitInfo->DescriptorPool;
	ImGui_ImplVulkan_Renderer_Info.Device = InitInfo->Device;
	ImGui_ImplVulkan_Renderer_Info.PhysicalDevice = InitInfo->PhysicalDevice;
	ImGui_ImplVulkan_Renderer_Info.ImageCount = InitInfo->ImageCount;
	ImGui_ImplVulkan_Renderer_Info.MsaaSamples = InitInfo->MsaaSamples;
	ImGui_ImplVulkan_Renderer_Info.LastPipeline = false;
	ImGui_ImplVulkan_Renderer_Info.LastDescriptorSet = false;

	ImGuiIO* IO = &ImGui::GetIO();
	IM_ASSERT(IO->BackendRendererUserData == NULL && "Already initialized a renderer backend!");

	IO->BackendRendererUserData = &ImGui_ImplVulkan_Renderer_Info;
	IO->BackendRendererName = "Custom ImGui Vulkan Impl by Moritz Gooth";
	IO->BackendFlags |= ImGuiBackendFlags_RendererHasVtxOffset;

	ImGui_CreateSampler();
	ImGui_CreateDescriptorSets();
	ImGui_CreatePipelineLayout();

	ImGui_CreateShaderModules();
	ImGui_CreateGraphicsPipeline();
}

void ImGui_ImplVulkan_Shutdown()
{
	vkDestroyShaderModule(ImGui_ImplVulkan_Renderer_Info.Device, ImGui_ImplVulkan_Renderer_Info.VertexShader, NULL);
	vkDestroyShaderModule(ImGui_ImplVulkan_Renderer_Info.Device, ImGui_ImplVulkan_Renderer_Info.FragmentShader, NULL);

	vkDestroyImageView(ImGui_ImplVulkan_Renderer_Info.Device, ImGui_ImplVulkan_Renderer_Info.FontImageView, NULL);
	vkDestroyImage(ImGui_ImplVulkan_Renderer_Info.Device, ImGui_ImplVulkan_Renderer_Info.FontImage, NULL);
	vkFreeMemory(ImGui_ImplVulkan_Renderer_Info.Device, ImGui_ImplVulkan_Renderer_Info.FontImageMemory, NULL);
	vkDestroySampler(ImGui_ImplVulkan_Renderer_Info.Device, ImGui_ImplVulkan_Renderer_Info.Sampler, NULL);

	vkDestroyBuffer(ImGui_ImplVulkan_Renderer_Info.Device, ImGui_ImplVulkan_Renderer_Info.FontUploadBuffer, NULL);
	vkFreeMemory(ImGui_ImplVulkan_Renderer_Info.Device, ImGui_ImplVulkan_Renderer_Info.FontUploadBufferMemory, NULL);

	if (ImGui_ImplVulkan_Renderer_Info.Buffers != NULL)
	{
		for (uint32_t i = 0; i < ImGui_ImplVulkan_Renderer_Info.ImageCount; i++)
		{
			if (ImGui_ImplVulkan_Renderer_Info.Buffers[i].VertexBuffer != NULL)
			{
				vkDestroyBuffer(ImGui_ImplVulkan_Renderer_Info.Device, ImGui_ImplVulkan_Renderer_Info.Buffers[i].VertexBuffer, NULL);
				vkFreeMemory(ImGui_ImplVulkan_Renderer_Info.Device, ImGui_ImplVulkan_Renderer_Info.Buffers[i].VertexBufferMemory, NULL);
			}
			if (ImGui_ImplVulkan_Renderer_Info.Buffers[i].IndexBuffer != NULL)
			{
				vkDestroyBuffer(ImGui_ImplVulkan_Renderer_Info.Device, ImGui_ImplVulkan_Renderer_Info.Buffers[i].IndexBuffer, NULL);
				vkFreeMemory(ImGui_ImplVulkan_Renderer_Info.Device, ImGui_ImplVulkan_Renderer_Info.Buffers[i].IndexBufferMemory, NULL);
			}
		}
	}

	vkDestroyDescriptorSetLayout(ImGui_ImplVulkan_Renderer_Info.Device, ImGui_ImplVulkan_Renderer_Info.DescriptorSetLayout, NULL);
	vkDestroyPipelineLayout(ImGui_ImplVulkan_Renderer_Info.Device, ImGui_ImplVulkan_Renderer_Info.PipelineLayout, NULL);
	vkDestroyPipeline(ImGui_ImplVulkan_Renderer_Info.Device, ImGui_ImplVulkan_Renderer_Info.Pipeline, NULL);
	vkDestroyPipeline(ImGui_ImplVulkan_Renderer_Info.Device, ImGui_ImplVulkan_Renderer_Info.OpaquePipeline, NULL);
}