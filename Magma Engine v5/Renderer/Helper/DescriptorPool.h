uint32_t DescriptorPool;
uint32_t ImGuiDescriptorPool;

void CreateDescriptorPool()
{
	uint32_t DescriptorTypes[] = { OPENVK_DESCRIPTOR_TYPE_IMAGE_SAMPLER, OPENVK_DESCRIPTOR_TYPE_UNIFORM_BUFFER };
									//1 Shadow, 1 Scene + Textures
								//FIX! I hate vulkan with textures!
	uint32_t DescriptorCounts[] = { 512, 2 };
	DescriptorPool = OpenVkCreateDescriptorPool(OPENVK_DESCRIPTOR_POOL_DEFAULT, 2, DescriptorTypes, DescriptorCounts);
}

void CreateImGuiDescriptorPool()
{
	uint32_t DescriptorTypes[] = { OPENVK_DESCRIPTOR_TYPE_IMAGE_SAMPLER };
	
	uint32_t DescriptorCounts[] = { 64 };
	ImGuiDescriptorPool = OpenVkCreateDescriptorPool(OPENVK_DESCRIPTOR_POOL_FREEABLE, 1, DescriptorTypes, DescriptorCounts);
}