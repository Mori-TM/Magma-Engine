uint32_t SceneDescriptorSetLayout;
uint32_t SSAODescriptorSetLayout;
uint32_t SSRDescriptorSetLayout;
uint32_t ForwardStaticDescriptorSetLayout;
uint32_t PBRDescriptorSetLayout;
//uint32_t GBufferDescriptorSetLayout;
uint32_t VertexUniformDescriptorSetLayout;
uint32_t FragmentUniformDescriptorSetLayout;
uint32_t TextureDescriptorSetLayout;
uint32_t StorageImageDescriptorSetLayout;

//Why is this in an extra file?
//FIX ? - DescriptorFlags can be NULL as far as I know
void CreateDescriptorSetLayout()
{
	{
		uint32_t DescriptorTypes[] = { OPENVK_DESCRIPTOR_TYPE_IMAGE_SAMPLER, OPENVK_DESCRIPTOR_TYPE_IMAGE_SAMPLER, OPENVK_DESCRIPTOR_TYPE_IMAGE_SAMPLER, OPENVK_DESCRIPTOR_TYPE_IMAGE_SAMPLER, OPENVK_DESCRIPTOR_TYPE_IMAGE_SAMPLER, OPENVK_DESCRIPTOR_TYPE_IMAGE_SAMPLER, OPENVK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, OPENVK_DESCRIPTOR_TYPE_STORAGE_BUFFER };
		uint32_t ShaderTypes[] = { OPENVK_SHADER_TYPE_FRAGMENT, OPENVK_SHADER_TYPE_FRAGMENT, OPENVK_SHADER_TYPE_FRAGMENT, OPENVK_SHADER_TYPE_FRAGMENT, OPENVK_SHADER_TYPE_FRAGMENT, OPENVK_SHADER_TYPE_FRAGMENT, OPENVK_SHADER_TYPE_FRAGMENT, OPENVK_SHADER_TYPE_FRAGMENT };
		uint32_t DescriptorFlags[] = { OPENVK_DESCRIPTOR_FLAG_NONE, OPENVK_DESCRIPTOR_FLAG_NONE, OPENVK_DESCRIPTOR_FLAG_NONE, OPENVK_DESCRIPTOR_FLAG_NONE,OPENVK_DESCRIPTOR_FLAG_NONE, OPENVK_DESCRIPTOR_FLAG_NONE, OPENVK_DESCRIPTOR_FLAG_NONE, OPENVK_DESCRIPTOR_FLAG_NONE };
		uint32_t DescriptorCounts[] = { 1, 1, 1, 1, 1, SHADOW_MAP_CASCADE_COUNT, 1, 1 };
		uint32_t Bindings[] = { 0, 1, 2, 3, 4, 5, 6, 7 };

		OpenVkDescriptorSetLayoutCreateInfo DescriptorSetLayoutCreateInfo;
		DescriptorSetLayoutCreateInfo.Flags = OPENVK_DESCRIPTOR_SET_LAYOUT_FLAG_NONE;
		DescriptorSetLayoutCreateInfo.BindingCount = ARRAY_SIZE(Bindings);
		DescriptorSetLayoutCreateInfo.Bindings = Bindings;
		DescriptorSetLayoutCreateInfo.DescriptorCounts = DescriptorCounts;
		DescriptorSetLayoutCreateInfo.DescriptorTypes = DescriptorTypes;
		DescriptorSetLayoutCreateInfo.DescriptorFlags = DescriptorFlags;
		DescriptorSetLayoutCreateInfo.ShaderTypes = ShaderTypes;
		SceneDescriptorSetLayout = OpenVkCreateDescriptorSetLayout(&DescriptorSetLayoutCreateInfo);
	}

	{
		uint32_t DescriptorTypes[] = { OPENVK_DESCRIPTOR_TYPE_IMAGE_SAMPLER, OPENVK_DESCRIPTOR_TYPE_IMAGE_SAMPLER, OPENVK_DESCRIPTOR_TYPE_IMAGE_SAMPLER, OPENVK_DESCRIPTOR_TYPE_UNIFORM_BUFFER };
		uint32_t ShaderTypes[] = { OPENVK_SHADER_TYPE_FRAGMENT, OPENVK_SHADER_TYPE_FRAGMENT, OPENVK_SHADER_TYPE_FRAGMENT, OPENVK_SHADER_TYPE_FRAGMENT };
		uint32_t DescriptorFlags[] = { OPENVK_DESCRIPTOR_FLAG_NONE, OPENVK_DESCRIPTOR_FLAG_NONE, OPENVK_DESCRIPTOR_FLAG_NONE, OPENVK_DESCRIPTOR_FLAG_NONE };
		uint32_t DescriptorCounts[] = { 1, 1, 1, 1 };
		uint32_t Bindings[] = { 0, 1, 2, 3 };

		OpenVkDescriptorSetLayoutCreateInfo DescriptorSetLayoutCreateInfo;
		DescriptorSetLayoutCreateInfo.Flags = OPENVK_DESCRIPTOR_SET_LAYOUT_FLAG_NONE;
		DescriptorSetLayoutCreateInfo.BindingCount = 4;
		DescriptorSetLayoutCreateInfo.Bindings = Bindings;
		DescriptorSetLayoutCreateInfo.DescriptorCounts = DescriptorCounts;
		DescriptorSetLayoutCreateInfo.DescriptorTypes = DescriptorTypes;
		DescriptorSetLayoutCreateInfo.DescriptorFlags = DescriptorFlags;
		DescriptorSetLayoutCreateInfo.ShaderTypes = ShaderTypes;
		SSAODescriptorSetLayout = OpenVkCreateDescriptorSetLayout(&DescriptorSetLayoutCreateInfo);
	}

	{
		uint32_t DescriptorTypes[] = { OPENVK_DESCRIPTOR_TYPE_IMAGE_SAMPLER, OPENVK_DESCRIPTOR_TYPE_IMAGE_SAMPLER, OPENVK_DESCRIPTOR_TYPE_IMAGE_SAMPLER, OPENVK_DESCRIPTOR_TYPE_IMAGE_SAMPLER, OPENVK_DESCRIPTOR_TYPE_UNIFORM_BUFFER };
		uint32_t ShaderTypes[] = { OPENVK_SHADER_TYPE_FRAGMENT, OPENVK_SHADER_TYPE_FRAGMENT, OPENVK_SHADER_TYPE_FRAGMENT, OPENVK_SHADER_TYPE_FRAGMENT, OPENVK_SHADER_TYPE_FRAGMENT };
		uint32_t DescriptorFlags[] = { OPENVK_DESCRIPTOR_FLAG_NONE, OPENVK_DESCRIPTOR_FLAG_NONE, OPENVK_DESCRIPTOR_FLAG_NONE, OPENVK_DESCRIPTOR_FLAG_NONE, OPENVK_DESCRIPTOR_FLAG_NONE };
		uint32_t DescriptorCounts[] = { 1, 1, 1, 1, 1 };
		uint32_t Bindings[] = { 0, 1, 2, 3, 4 };

		OpenVkDescriptorSetLayoutCreateInfo DescriptorSetLayoutCreateInfo;
		DescriptorSetLayoutCreateInfo.Flags = OPENVK_DESCRIPTOR_SET_LAYOUT_FLAG_NONE;
		DescriptorSetLayoutCreateInfo.BindingCount = 5;
		DescriptorSetLayoutCreateInfo.Bindings = Bindings;
		DescriptorSetLayoutCreateInfo.DescriptorCounts = DescriptorCounts;
		DescriptorSetLayoutCreateInfo.DescriptorTypes = DescriptorTypes;
		DescriptorSetLayoutCreateInfo.DescriptorFlags = DescriptorFlags;
		DescriptorSetLayoutCreateInfo.ShaderTypes = ShaderTypes;
		SSRDescriptorSetLayout = OpenVkCreateDescriptorSetLayout(&DescriptorSetLayoutCreateInfo);
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

		OpenVkDescriptorSetLayoutCreateInfo DescriptorSetLayoutCreateInfo;
		DescriptorSetLayoutCreateInfo.Flags = OPENVK_DESCRIPTOR_SET_LAYOUT_FLAG_NONE;
		DescriptorSetLayoutCreateInfo.BindingCount = 1;
		DescriptorSetLayoutCreateInfo.Bindings = Bindings;
		DescriptorSetLayoutCreateInfo.DescriptorCounts = DescriptorCounts;
		DescriptorSetLayoutCreateInfo.DescriptorTypes = DescriptorTypes;
		DescriptorSetLayoutCreateInfo.DescriptorFlags = DescriptorFlags;
		DescriptorSetLayoutCreateInfo.ShaderTypes = ShaderTypes;
		VertexUniformDescriptorSetLayout = OpenVkCreateDescriptorSetLayout(&DescriptorSetLayoutCreateInfo);
	}

	{
		uint32_t DescriptorTypes[] = { OPENVK_DESCRIPTOR_TYPE_UNIFORM_BUFFER };
		uint32_t ShaderTypes[] = { OPENVK_SHADER_TYPE_FRAGMENT };
		uint32_t DescriptorFlags[] = { OPENVK_DESCRIPTOR_FLAG_NONE };
		uint32_t DescriptorCounts[] = { 1 };
		uint32_t Bindings[] = { 0 };

		OpenVkDescriptorSetLayoutCreateInfo DescriptorSetLayoutCreateInfo;
		DescriptorSetLayoutCreateInfo.Flags = OPENVK_DESCRIPTOR_SET_LAYOUT_FLAG_NONE;
		DescriptorSetLayoutCreateInfo.BindingCount = 1;
		DescriptorSetLayoutCreateInfo.Bindings = Bindings;
		DescriptorSetLayoutCreateInfo.DescriptorCounts = DescriptorCounts;
		DescriptorSetLayoutCreateInfo.DescriptorTypes = DescriptorTypes;
		DescriptorSetLayoutCreateInfo.DescriptorFlags = DescriptorFlags;
		DescriptorSetLayoutCreateInfo.ShaderTypes = ShaderTypes;
		FragmentUniformDescriptorSetLayout = OpenVkCreateDescriptorSetLayout(&DescriptorSetLayoutCreateInfo);
	}
	
	{
		uint32_t DescriptorTypes[] = { OPENVK_DESCRIPTOR_TYPE_IMAGE_SAMPLER };
		uint32_t ShaderTypes[] = { OPENVK_SHADER_TYPE_FRAGMENT };
		uint32_t DescriptorFlags[] = { OPENVK_DESCRIPTOR_FLAG_NONE };
		uint32_t DescriptorCounts[] = { 1 };
		uint32_t Bindings[] = { 0 };

		OpenVkDescriptorSetLayoutCreateInfo DescriptorSetLayoutCreateInfo;
		DescriptorSetLayoutCreateInfo.Flags = OPENVK_DESCRIPTOR_SET_LAYOUT_FLAG_NONE;
		DescriptorSetLayoutCreateInfo.BindingCount = 1;
		DescriptorSetLayoutCreateInfo.Bindings = Bindings;
		DescriptorSetLayoutCreateInfo.DescriptorCounts = DescriptorCounts;
		DescriptorSetLayoutCreateInfo.DescriptorTypes = DescriptorTypes;
		DescriptorSetLayoutCreateInfo.DescriptorFlags = DescriptorFlags;
		DescriptorSetLayoutCreateInfo.ShaderTypes = ShaderTypes;
		TextureDescriptorSetLayout = OpenVkCreateDescriptorSetLayout(&DescriptorSetLayoutCreateInfo);
	}

	{
		uint32_t DescriptorTypes[] = { OPENVK_DESCRIPTOR_TYPE_STORAGE_IMAGE };
		uint32_t ShaderTypes[] = { OPENVK_SHADER_TYPE_FRAGMENT };
		uint32_t DescriptorFlags[] = { OPENVK_DESCRIPTOR_FLAG_NONE };
		uint32_t DescriptorCounts[] = { 1 };
		uint32_t Bindings[] = { 0 };

		OpenVkDescriptorSetLayoutCreateInfo DescriptorSetLayoutCreateInfo;
		DescriptorSetLayoutCreateInfo.Flags = OPENVK_DESCRIPTOR_SET_LAYOUT_FLAG_NONE;
		DescriptorSetLayoutCreateInfo.BindingCount = 1;
		DescriptorSetLayoutCreateInfo.Bindings = Bindings;
		DescriptorSetLayoutCreateInfo.DescriptorCounts = DescriptorCounts;
		DescriptorSetLayoutCreateInfo.DescriptorTypes = DescriptorTypes;
		DescriptorSetLayoutCreateInfo.DescriptorFlags = DescriptorFlags;
		DescriptorSetLayoutCreateInfo.ShaderTypes = ShaderTypes;
		StorageImageDescriptorSetLayout = OpenVkCreateDescriptorSetLayout(&DescriptorSetLayoutCreateInfo);
	}

	{
		uint32_t DescriptorTypes[] = 
		{ 
			OPENVK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
			OPENVK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
			OPENVK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
			OPENVK_DESCRIPTOR_TYPE_IMAGE_SAMPLER
		};
		uint32_t ShaderTypes[] = 
		{ 
			OPENVK_SHADER_TYPE_VERTEX,
			OPENVK_SHADER_TYPE_FRAGMENT,
			OPENVK_SHADER_TYPE_FRAGMENT,
			OPENVK_SHADER_TYPE_FRAGMENT,
		};
		uint32_t DescriptorFlags[4]; FillUintArray(DescriptorFlags, OPENVK_DESCRIPTOR_FLAG_NONE, ARRAY_SIZE(DescriptorFlags));
		uint32_t DescriptorCounts[] = { 1, 1, 1, SHADOW_MAP_CASCADE_COUNT };
		uint32_t Bindings[] = { 0, 1, 2, 3 };

		OpenVkDescriptorSetLayoutCreateInfo DescriptorSetLayoutCreateInfo;
		DescriptorSetLayoutCreateInfo.Flags = OPENVK_DESCRIPTOR_SET_LAYOUT_FLAG_NONE;
		DescriptorSetLayoutCreateInfo.BindingCount = ARRAY_SIZE(Bindings);
		DescriptorSetLayoutCreateInfo.Bindings = Bindings;
		DescriptorSetLayoutCreateInfo.DescriptorCounts = DescriptorCounts;
		DescriptorSetLayoutCreateInfo.DescriptorTypes = DescriptorTypes;
		DescriptorSetLayoutCreateInfo.DescriptorFlags = DescriptorFlags;
		DescriptorSetLayoutCreateInfo.ShaderTypes = ShaderTypes;
		ForwardStaticDescriptorSetLayout = OpenVkCreateDescriptorSetLayout(&DescriptorSetLayoutCreateInfo);
	}

	{
		uint32_t DescriptorTypes[] = 
		{ 
			OPENVK_DESCRIPTOR_TYPE_IMAGE_SAMPLER,
			OPENVK_DESCRIPTOR_TYPE_IMAGE_SAMPLER,
			OPENVK_DESCRIPTOR_TYPE_IMAGE_SAMPLER,
			OPENVK_DESCRIPTOR_TYPE_IMAGE_SAMPLER,
			OPENVK_DESCRIPTOR_TYPE_IMAGE_SAMPLER
		};
		uint32_t ShaderTypes[] = 
		{ 
			OPENVK_SHADER_TYPE_FRAGMENT,
			OPENVK_SHADER_TYPE_FRAGMENT,
			OPENVK_SHADER_TYPE_FRAGMENT,
			OPENVK_SHADER_TYPE_FRAGMENT,
			OPENVK_SHADER_TYPE_FRAGMENT
		};
		uint32_t DescriptorFlags[5]; FillUintArray(DescriptorFlags, OPENVK_DESCRIPTOR_FLAG_UPDATE_AFTER_BIND, ARRAY_SIZE(DescriptorFlags));
		uint32_t DescriptorCounts[] = { 1, 1, 1, 1, 1 };
		uint32_t Bindings[] = { 0, 1, 2, 3, 4 };

		OpenVkDescriptorSetLayoutCreateInfo DescriptorSetLayoutCreateInfo;
		DescriptorSetLayoutCreateInfo.Flags = OPENVK_DESCRIPTOR_SET_LAYOUT_FLAG_NONE;
		DescriptorSetLayoutCreateInfo.BindingCount = ARRAY_SIZE(Bindings);
		DescriptorSetLayoutCreateInfo.Bindings = Bindings;
		DescriptorSetLayoutCreateInfo.DescriptorCounts = DescriptorCounts;
		DescriptorSetLayoutCreateInfo.DescriptorTypes = DescriptorTypes;
		DescriptorSetLayoutCreateInfo.DescriptorFlags = DescriptorFlags;
		DescriptorSetLayoutCreateInfo.ShaderTypes = ShaderTypes;
		PBRDescriptorSetLayout = OpenVkCreateDescriptorSetLayout(&DescriptorSetLayoutCreateInfo);
	}
}