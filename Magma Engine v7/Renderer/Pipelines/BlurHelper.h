typedef struct
{
	uint32_t Horizontal;
	float BlurScale;
	float BlurStrength;
} BlurFragmentPushConstant;

uint32_t BlurLayout;
uint32_t BlurPipeline;
uint32_t BlurRenderPass;
uint32_t BlurColorAttachments[2];
uint32_t BlurFramebuffers[2];
uint32_t BlurDescriptorSets[2];
//uint32_t BlurVertPipeline;
//uint32_t BlurHorzPipeline;
//uint32_t BlurVertDescriptorSet;
//uint32_t BlurHorzDescriptorSet;

BlurFragmentPushConstant BlurFragmentPc;