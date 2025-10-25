uint32_t ImageSampler;
uint32_t GBufferSampler;
uint32_t ShadowSampler;

void CreateImageSampler()
{
	VkRenderer.MipLevels = 1;
	ImageSampler = OpenVkCreateImageSampler(OPENVK_FILTER_NEAREST, OPENVK_ADDRESS_MODE_REPEAT);
	GBufferSampler = OpenVkCreateImageSampler(OPENVK_FILTER_NEAREST, OPENVK_ADDRESS_MODE_CLAMP_TO_EDGE);
	ShadowSampler = OpenVkCreateImageSampler(OPENVK_FILTER_NEAREST, OPENVK_ADDRESS_MODE_CLAMP_TO_BORDER);
} 