uint32_t VertexUniformDescriptorSetLayout;
uint32_t FragmentUniformDescriptorSetLayout;
uint32_t TextureDescriptorSetLayout;
uint32_t ShadowMapDescriptorSetLayout;

void CreateDescriptorSetLayout()
{
	{
		uint32_t DescriptorTypes[] = { OPENVK_DESCRIPTOR_TYPE_UNIFORM_BUFFER };
		uint32_t ShaderTypes[] = { OPENVK_SHADER_TYPE_VERTEX };
		uint32_t DescriptorCounts[] = { 1 };

		VertexUniformDescriptorSetLayout = OpenVkCreateDescriptorSetLayout(0, 1, DescriptorCounts, DescriptorTypes, ShaderTypes);
	}

	{
		uint32_t DescriptorTypes[] = { OPENVK_DESCRIPTOR_TYPE_UNIFORM_BUFFER };
		uint32_t ShaderTypes[] = { OPENVK_SHADER_TYPE_FRAGMENT };
		uint32_t DescriptorCounts[] = { 1 };

		FragmentUniformDescriptorSetLayout = OpenVkCreateDescriptorSetLayout(0, 1, DescriptorCounts, DescriptorTypes, ShaderTypes);
	}
	
	{
		uint32_t DescriptorTypes[] = { OPENVK_DESCRIPTOR_TYPE_IMAGE_SAMPLER };
		uint32_t ShaderTypes[] = { OPENVK_SHADER_TYPE_FRAGMENT };
		uint32_t DescriptorCounts[] = { 1 };
		TextureDescriptorSetLayout = OpenVkCreateDescriptorSetLayout(0, 1, DescriptorCounts, DescriptorTypes, ShaderTypes);
	}

	{
		uint32_t DescriptorTypes[] = { OPENVK_DESCRIPTOR_TYPE_IMAGE_SAMPLER };
		uint32_t ShaderTypes[] = { OPENVK_SHADER_TYPE_FRAGMENT };
		uint32_t DescriptorCounts[] = { 1 };
		ShadowMapDescriptorSetLayout = OpenVkCreateDescriptorSetLayout(0, 1, DescriptorCounts, DescriptorTypes, ShaderTypes);
	}
}