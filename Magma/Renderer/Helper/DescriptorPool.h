OpenVkDescriptorPool DescriptorPool;
OpenVkDescriptorPool ImGuiDescriptorPool;

void CreateDescriptorPool() {
    uint32_t DescriptorTypes[] = { OPENVK_DESCRIPTOR_TYPE_IMAGE_SAMPLER, OPENVK_DESCRIPTOR_TYPE_UNIFORM_BUFFER };
    //1 Shadow, 1 Scene + Textures
    //FIX! I hate vulkan with textures!
    uint32_t DescriptorCounts[] = { 512, 2 };

    DescriptorPool = OpenVkCreateDescriptorPool(2, DescriptorTypes, DescriptorCounts, false);
}

void CreateImGuiDescriptorPool() {
    uint32_t DescriptorTypes[] = { OPENVK_DESCRIPTOR_TYPE_IMAGE_SAMPLER };

    uint32_t DescriptorCounts[] = { 1 };

    ImGuiDescriptorPool = OpenVkCreateDescriptorPool(1, DescriptorTypes, DescriptorCounts, true);
}