#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>

#include <vulkan/vulkan.h>

#define STB_IMAGE_IMPLEMENTATION
#include "CMA.h"
#include "stb_image.h"
#include "OpenVK_Defines.h"
#include "OpenVK_Helper.h"

//FIX Vulkan
//descriptor set/pools get destroy every resize except the dynamic one thats pretty fucked up
#include "OpenVK_VkHelperFunc.h"
#include "OpenVK_VkRenderer.h"

extern inline uint32_t OpenVkCreateRenderer(OpenVkBool ValidationLayers, const char** (*GetExtensions)(uint32_t* ExtensionCount), VkSurfaceKHR(*GetSurface)(VkInstance* Instance), void (*GetWindowSize)(uint32_t* Width, uint32_t* Height))
{
	return VkCreateRenderer(ValidationLayers, GetExtensions, GetSurface, GetWindowSize);
}

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

extern inline void OpenVkUpdateDescriptorSet(OpenVkDescriptorSetCreateInfo* Info, uint32_t DescriptorSet)
{
	/*
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
	*/
}

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