#pragma once
#include "imgui.h"
#include <vulkan/vulkan.h>

typedef struct
{
	VkRenderPass RenderPass;
	VkDescriptorPool DescriptorPool;
	VkDevice Device;
	VkPhysicalDevice PhysicalDevice;
	uint32_t ImageCount;
	VkSampleCountFlagBits MsaaSamples;
	VkDescriptorSetLayout DescriptorSetLayout;//This Layout needs to be image sample, Fragment shader, no flags, one descriptor count and binding at 0
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

extern VkDeviceSize ImGui_BufferMemoryAlignment;

void ImGui_ImplVulkan_Init(ImGui_ImplVulkan_InitInfo* InitInfo);
bool ImGui_ImplVulkan_CreateFontsTexture(VkCommandBuffer CommandBuffer);
void ImGui_ImplVulkan_DestroyFontUploadObjects();
void ImGui_ImplVulkan_NewFrame();
bool ImGui_ImplVulkan_RenderDrawData(ImDrawData* DrawData, VkCommandBuffer CommandBuffer, int NonAlphaTextureCount, ImTextureID* NonAlphaTextures);
void ImGui_ImplVulkan_Shutdown();