uint32_t SceneDescriptorSetLayout;
uint32_t VertexUniformDescriptorSetLayout;
uint32_t FragmentUniformDescriptorSetLayout;
uint32_t TextureDescriptorSetLayout;

void CreateDescriptorSetLayout()
{
	{
		uint32_t DescriptorTypes[] = { OPENVK_DESCRIPTOR_TYPE_IMAGE_SAMPLER, OPENVK_DESCRIPTOR_TYPE_IMAGE_SAMPLER, OPENVK_DESCRIPTOR_TYPE_IMAGE_SAMPLER, OPENVK_DESCRIPTOR_TYPE_IMAGE_SAMPLER };
		uint32_t ShaderTypes[] = { OPENVK_SHADER_TYPE_FRAGMENT, OPENVK_SHADER_TYPE_FRAGMENT, OPENVK_SHADER_TYPE_FRAGMENT, OPENVK_SHADER_TYPE_FRAGMENT };
		uint32_t DescriptorFlags[] = { OPENVK_DESCRIPTOR_FLAG_NONE, OPENVK_DESCRIPTOR_FLAG_NONE, OPENVK_DESCRIPTOR_FLAG_NONE, OPENVK_DESCRIPTOR_FLAG_NONE };
		uint32_t DescriptorCounts[] = { 1, 1, 1, 1 };
		uint32_t Bindings[] = { 0, 1, 2, 3 };

		SceneDescriptorSetLayout = OpenVkCreateDescriptorSetLayout(4, Bindings, DescriptorCounts, DescriptorTypes, DescriptorFlags, ShaderTypes);
	}

	{
		uint32_t DescriptorTypes[] = { OPENVK_DESCRIPTOR_TYPE_UNIFORM_BUFFER };
		uint32_t ShaderTypes[] = { OPENVK_SHADER_TYPE_VERTEX };
		uint32_t DescriptorFlags[] = { OPENVK_DESCRIPTOR_FLAG_NONE };
		uint32_t DescriptorCounts[] = { 1 };
		uint32_t Bindings[] = { 0 };

		VertexUniformDescriptorSetLayout = OpenVkCreateDescriptorSetLayout(1, Bindings, DescriptorCounts, DescriptorTypes, DescriptorFlags, ShaderTypes);
	}

	{
		uint32_t DescriptorTypes[] = { OPENVK_DESCRIPTOR_TYPE_UNIFORM_BUFFER };
		uint32_t ShaderTypes[] = { OPENVK_SHADER_TYPE_FRAGMENT };
		uint32_t DescriptorFlags[] = { OPENVK_DESCRIPTOR_FLAG_NONE };
		uint32_t DescriptorCounts[] = { 1 };
		uint32_t Bindings[] = { 0 };

		FragmentUniformDescriptorSetLayout = OpenVkCreateDescriptorSetLayout(1, Bindings, DescriptorCounts, DescriptorTypes, DescriptorFlags, ShaderTypes);
	}
	
	{
		uint32_t DescriptorTypes[] = { OPENVK_DESCRIPTOR_TYPE_IMAGE_SAMPLER };
		uint32_t ShaderTypes[] = { OPENVK_SHADER_TYPE_FRAGMENT };
		uint32_t DescriptorFlags[] = { OPENVK_DESCRIPTOR_FLAG_NONE };
		uint32_t DescriptorCounts[] = { 1 };
		uint32_t Bindings[] = { 0 };

		TextureDescriptorSetLayout = OpenVkCreateDescriptorSetLayout(1, Bindings, DescriptorCounts, DescriptorTypes, DescriptorFlags, ShaderTypes);
	}
}