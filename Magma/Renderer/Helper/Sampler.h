uint32_t ImageSampler;
uint32_t ShadowSampler;

void CreateImageSampler() {
    ImageSampler = OpenVkCreateImageSampler(OPENVK_FILTER_LINEAR, OPENVK_ADDRESS_MODE_REPEAT);
    ShadowSampler = OpenVkCreateImageSampler(OPENVK_FILTER_NEAREST, OPENVK_ADDRESS_MODE_CLAMP_TO_BORDER);
}