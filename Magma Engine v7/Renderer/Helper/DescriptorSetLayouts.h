uint32_t SceneDescriptorSetLayout;
uint32_t SSAODescriptorSetLayout;
uint32_t SSRDescriptorSetLayout;
//uint32_t GBufferDescriptorSetLayout;
uint32_t VertexUniformDescriptorSetLayout;
uint32_t FragmentUniformDescriptorSetLayout;
uint32_t TextureDescriptorSetLayout;
uint32_t StorageImageDescriptorSetLayout;
//Why is this in an extra file?

void CreateDescriptorSetLayout()
{
	{
		uint32_t DescriptorTypes[] = { OPENVK_DESCRIPTOR_TYPE_IMAGE_SAMPLER, OPENVK_DESCRIPTOR_TYPE_IMAGE_SAMPLER, OPENVK_DESCRIPTOR_TYPE_IMAGE_SAMPLER, OPENVK_DESCRIPTOR_TYPE_IMAGE_SAMPLER, OPENVK_DESCRIPTOR_TYPE_IMAGE_SAMPLER, OPENVK_DESCRIPTOR_TYPE_IMAGE_SAMPLER, OPENVK_DESCRIPTOR_TYPE_IMAGE_SAMPLER, OPENVK_DESCRIPTOR_TYPE_IMAGE_SAMPLER, OPENVK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, OPENVK_DESCRIPTOR_TYPE_STORAGE_BUFFER };
		uint32_t ShaderTypes[] = { OPENVK_SHADER_TYPE_FRAGMENT, OPENVK_SHADER_TYPE_FRAGMENT, OPENVK_SHADER_TYPE_FRAGMENT, OPENVK_SHADER_TYPE_FRAGMENT, OPENVK_SHADER_TYPE_FRAGMENT, OPENVK_SHADER_TYPE_FRAGMENT, OPENVK_SHADER_TYPE_FRAGMENT, OPENVK_SHADER_TYPE_FRAGMENT, OPENVK_SHADER_TYPE_FRAGMENT, OPENVK_SHADER_TYPE_FRAGMENT };
		uint32_t DescriptorFlags[] = { OPENVK_DESCRIPTOR_FLAG_NONE, OPENVK_DESCRIPTOR_FLAG_NONE, OPENVK_DESCRIPTOR_FLAG_NONE, OPENVK_DESCRIPTOR_FLAG_NONE, OPENVK_DESCRIPTOR_FLAG_NONE,OPENVK_DESCRIPTOR_FLAG_NONE, OPENVK_DESCRIPTOR_FLAG_NONE, OPENVK_DESCRIPTOR_FLAG_NONE, OPENVK_DESCRIPTOR_FLAG_NONE, OPENVK_DESCRIPTOR_FLAG_NONE };
		uint32_t DescriptorCounts[] = { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 };
		uint32_t Bindings[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };

		SceneDescriptorSetLayout = OpenVkCreateDescriptorSetLayout(10, Bindings, DescriptorCounts, DescriptorTypes, DescriptorFlags, ShaderTypes);
	}

	{
		uint32_t DescriptorTypes[] = { OPENVK_DESCRIPTOR_TYPE_IMAGE_SAMPLER, OPENVK_DESCRIPTOR_TYPE_IMAGE_SAMPLER, OPENVK_DESCRIPTOR_TYPE_IMAGE_SAMPLER, OPENVK_DESCRIPTOR_TYPE_UNIFORM_BUFFER };
		uint32_t ShaderTypes[] = { OPENVK_SHADER_TYPE_FRAGMENT, OPENVK_SHADER_TYPE_FRAGMENT, OPENVK_SHADER_TYPE_FRAGMENT, OPENVK_SHADER_TYPE_FRAGMENT };
		uint32_t DescriptorFlags[] = { OPENVK_DESCRIPTOR_FLAG_NONE, OPENVK_DESCRIPTOR_FLAG_NONE, OPENVK_DESCRIPTOR_FLAG_NONE, OPENVK_DESCRIPTOR_FLAG_NONE };
		uint32_t DescriptorCounts[] = { 1, 1, 1, 1 };
		uint32_t Bindings[] = { 0, 1, 2, 3 };

		SSAODescriptorSetLayout = OpenVkCreateDescriptorSetLayout(4, Bindings, DescriptorCounts, DescriptorTypes, DescriptorFlags, ShaderTypes);
	}

	{
		uint32_t DescriptorTypes[] = { OPENVK_DESCRIPTOR_TYPE_IMAGE_SAMPLER, OPENVK_DESCRIPTOR_TYPE_IMAGE_SAMPLER, OPENVK_DESCRIPTOR_TYPE_IMAGE_SAMPLER, OPENVK_DESCRIPTOR_TYPE_IMAGE_SAMPLER, OPENVK_DESCRIPTOR_TYPE_UNIFORM_BUFFER };
		uint32_t ShaderTypes[] = { OPENVK_SHADER_TYPE_FRAGMENT, OPENVK_SHADER_TYPE_FRAGMENT, OPENVK_SHADER_TYPE_FRAGMENT, OPENVK_SHADER_TYPE_FRAGMENT, OPENVK_SHADER_TYPE_FRAGMENT };
		uint32_t DescriptorFlags[] = { OPENVK_DESCRIPTOR_FLAG_NONE, OPENVK_DESCRIPTOR_FLAG_NONE, OPENVK_DESCRIPTOR_FLAG_NONE, OPENVK_DESCRIPTOR_FLAG_NONE, OPENVK_DESCRIPTOR_FLAG_NONE };
		uint32_t DescriptorCounts[] = { 1, 1, 1, 1, 1 };
		uint32_t Bindings[] = { 0, 1, 2, 3, 4 };

		SSRDescriptorSetLayout = OpenVkCreateDescriptorSetLayout(5, Bindings, DescriptorCounts, DescriptorTypes, DescriptorFlags, ShaderTypes);
	}

//	{
//		uint32_t DescriptorTypes[] = { OPENVK_DESCRIPTOR_TYPE_IMAGE_SAMPLER, OPENVK_DESCRIPTOR_TYPE_IMAGE_SAMPLER, OPENVK_DESCRIPTOR_TYPE_IMAGE_SAMPLER, OPENVK_DESCRIPTOR_TYPE_IMAGE_SAMPLER };
//		uint32_t ShaderTypes[] = { OPENVK_SHADER_TYPE_FRAGMENT, OPENVK_SHADER_TYPE_FRAGMENT, OPENVK_SHADER_TYPE_FRAGMENT, OPENVK_SHADER_TYPE_FRAGMENT };
//		uint32_t DescriptorFlags[] = { OPENVK_DESCRIPTOR_FLAG_NONE, OPENVK_DESCRIPTOR_FLAG_NONE, OPENVK_DESCRIPTOR_FLAG_NONE, OPENVK_DESCRIPTOR_FLAG_NONE };
//		uint32_t DescriptorCounts[] = { 1, 1, 1, 1 };
//		uint32_t Bindings[] = { 0, 1, 2, 3 };
//
//		GBufferDescriptorSetLayout = OpenVkCreateDescriptorSetLayout(4, Bindings, DescriptorCounts, DescriptorTypes, DescriptorFlags, ShaderTypes);
//	}

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

	{
		uint32_t DescriptorTypes[] = { OPENVK_DESCRIPTOR_TYPE_STORAGE_IMAGE };
		uint32_t ShaderTypes[] = { OPENVK_SHADER_TYPE_FRAGMENT };
		uint32_t DescriptorFlags[] = { OPENVK_DESCRIPTOR_FLAG_NONE };
		uint32_t DescriptorCounts[] = { 1 };
		uint32_t Bindings[] = { 0 };

		StorageImageDescriptorSetLayout = OpenVkCreateDescriptorSetLayout(1, Bindings, DescriptorCounts, DescriptorTypes, DescriptorFlags, ShaderTypes);
	}
}