OpenVkBool VkCreateSwapChain(uint32_t* Width, uint32_t* Height)
{
	VkQueueFamilyIndices Indices = VkFindQueueFamilies(VkRenderer.PhysicalDevice);

	VkSwapChainSupportDetails SwapChainSupport;
	VkQuerySwapChainSupport(VkRenderer.PhysicalDevice, &SwapChainSupport);

	VkSurfaceFormatKHR SurfaceFormat = VkChooseSwapSurfaceFormat(SwapChainSupport.Formats, SwapChainSupport.FormatCount);
	VkPresentModeKHR PresentMode = VkChooseSwapPresentMode(SwapChainSupport.PresentModes, SwapChainSupport.PresentModeCount);
	VkExtent2D Extent = VkChooseSwapExtent(SwapChainSupport.Capabilities);

	*Width = Extent.width;
	*Height = Extent.height;

	uint32_t ImageCount = SwapChainSupport.Capabilities.minImageCount + 1;
	if (SwapChainSupport.Capabilities.maxImageCount > 0 && ImageCount > SwapChainSupport.Capabilities.maxImageCount)
		ImageCount = SwapChainSupport.Capabilities.maxImageCount;

	VkSwapchainCreateInfoKHR SwapchainCreateInfo;
	SwapchainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	SwapchainCreateInfo.pNext = NULL;
	SwapchainCreateInfo.flags = 0;
	SwapchainCreateInfo.surface = VkRenderer.Surface;
	SwapchainCreateInfo.minImageCount = ImageCount;
	SwapchainCreateInfo.imageFormat = SurfaceFormat.format;
	SwapchainCreateInfo.imageColorSpace = SurfaceFormat.colorSpace;
	SwapchainCreateInfo.imageExtent = Extent;
	SwapchainCreateInfo.imageArrayLayers = 1;
	SwapchainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

	uint32_t FamilyIndices[] = { Indices.GraphicsFamily, Indices.PresentFamily };

	//Don't know why to use VK_SHARING_MODE_CONCURRENT cause VK_SHARING_MODE_EXCLUSIVE is faster
/*
	if (Indices.GraphicsFamily != Indices.PresentFamily)
	{
		SwapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		SwapchainCreateInfo.queueFamilyIndexCount = 2;
		SwapchainCreateInfo.pQueueFamilyIndices = FamilyIndices;
	}
	else
*/
	{
		SwapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		SwapchainCreateInfo.queueFamilyIndexCount = 0;
		SwapchainCreateInfo.pQueueFamilyIndices = NULL;
	}

	SwapchainCreateInfo.preTransform = SwapChainSupport.Capabilities.currentTransform;
	SwapchainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	SwapchainCreateInfo.presentMode = PresentMode;
	SwapchainCreateInfo.clipped = VK_TRUE;
	SwapchainCreateInfo.oldSwapchain = VK_NULL_HANDLE;

	if (vkCreateSwapchainKHR(VkRenderer.Device, &SwapchainCreateInfo, NULL, &VkRenderer.SwapChain) != VK_SUCCESS)
		return OpenVkRuntimeError("Failed to Create SwapChain");

	//SwapChain Image
	vkGetSwapchainImagesKHR(VkRenderer.Device, VkRenderer.SwapChain, &ImageCount, NULL);
	VkRenderer.SwapChainImages = (VkImage*)OpenVkMalloc(ImageCount * sizeof(VkImage));
	vkGetSwapchainImagesKHR(VkRenderer.Device, VkRenderer.SwapChain, &ImageCount, VkRenderer.SwapChainImages);

	VkRenderer.SwapChainImageFormat = SurfaceFormat.format;
	VkRenderer.SwapChainExtent = Extent;

	//Swapchain Image Views
	VkRenderer.SwapChainImageViews = (VkImageView*)OpenVkMalloc(ImageCount * sizeof(VkImageView));

	for (uint32_t i = 0; i < ImageCount; i++)
		VkRenderer.SwapChainImageViews[i] = VkCreateImageView(VkRenderer.SwapChainImages[i], VkRenderer.SwapChainImageFormat, VK_IMAGE_ASPECT_COLOR_BIT, 1);

	OpenVkFree(SwapChainSupport.Formats);
	OpenVkFree(SwapChainSupport.PresentModes);

	VkRenderer.SwapChainImageCount = ImageCount;
	
	return 1;
}

OpenVkBool VkCreateCommandBuffers()
{
	//Command Buffers
	VkRenderer.CommandBuffers = (VkCommandBuffer*)OpenVkMalloc(VkRenderer.SwapChainImageCount * sizeof(VkCommandBuffer));

	VkCommandBufferAllocateInfo AllocateInfo;
	AllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	AllocateInfo.pNext = NULL;
	AllocateInfo.commandPool = VkRenderer.CommandPool;
	AllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	AllocateInfo.commandBufferCount = VkRenderer.SwapChainImageCount;

	if (vkAllocateCommandBuffers(VkRenderer.Device, &AllocateInfo, VkRenderer.CommandBuffers) != VK_SUCCESS)
		return OpenVkRuntimeError("Failed to Allocate Command Buffer");

	return 1;
}

uint32_t VkCreateRenderer(OpenVkBool EnableValidationLayers, const char**(*GetExtensions)(uint32_t* ExtensionCount), VkSurfaceKHR(*GetSurface)(VkInstance* Instance), void (*GetWindowSize)(uint32_t* Width, uint32_t* Height))
{
	const char* ValidationLayers[] = { "VK_LAYER_KHRONOS_validation" };
	const char* DeviceExtensions[] = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
	
	memset(&VkRenderer, 0, sizeof(VkRendererInfo));
	VkRenderer.DescriptorSets = CMA_Create(sizeof(VkDescriptorSetInfo));
	VkRenderer.TextureImages = CMA_Create(sizeof(VkTextureImageInfo));
	VkRenderer.Sampler = CMA_Create(sizeof(VkSampler));
	VkRenderer.Buffers = CMA_Create(sizeof(VkBufferInfo));

	//Instance
	VkApplicationInfo AppInfo;
	AppInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	AppInfo.pNext = NULL;
	AppInfo.pApplicationName = "OpenVK Framework";
	AppInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	AppInfo.pEngineName = "OpenVK";
	AppInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	AppInfo.apiVersion = VK_MAKE_VERSION(1, 0, 0);

	uint32_t ExtensionCount;
	const char** Extensions = GetExtensions(&ExtensionCount);

	VkInstanceCreateInfo InstanceCreateInfo;
	InstanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	InstanceCreateInfo.pNext = NULL;
	InstanceCreateInfo.flags = 0;
	InstanceCreateInfo.pApplicationInfo = &AppInfo;
	InstanceCreateInfo.enabledLayerCount = EnableValidationLayers;
	InstanceCreateInfo.ppEnabledLayerNames = ValidationLayers;
	InstanceCreateInfo.enabledExtensionCount = ExtensionCount;
	InstanceCreateInfo.ppEnabledExtensionNames = Extensions;

	OpenVkRuntimeInfo("Validation Layers: ", (EnableValidationLayers ? "Enabled" : "Disabled"));

	if (vkCreateInstance(&InstanceCreateInfo, NULL, &VkRenderer.Instance) != VK_SUCCESS)
		return OpenVkRuntimeError("Failed to Create Instance");

	for (uint32_t i = 0; i < ExtensionCount; i++)
		OpenVkRuntimeInfo("Used Extension: ", Extensions[i]);

	//may cause error FIX
	OpenVkFree(Extensions);

	ExtensionCount = 0;
	vkEnumerateInstanceExtensionProperties(NULL, &ExtensionCount, NULL);

	VkExtensionProperties* ExtensionProperties = (VkExtensionProperties*)OpenVkMalloc(ExtensionCount * sizeof(VkExtensionProperties));
	vkEnumerateInstanceExtensionProperties(NULL, &ExtensionCount, ExtensionProperties);

	for (uint32_t i = 0; i < ExtensionCount; i++)
		OpenVkRuntimeInfo("Available Extension: ", ExtensionProperties[i].extensionName);

	OpenVkFree(ExtensionProperties);

	//Surface
	VkRenderer.Surface = GetSurface(&VkRenderer.Instance);

	//Physical Device
	uint32_t PhysicalDeviceCount = 0;
	vkEnumeratePhysicalDevices(VkRenderer.Instance, &PhysicalDeviceCount, NULL);

	if (PhysicalDeviceCount == 0)
		return OpenVkRuntimeError("No Vulkan Supported GPU");

	VkPhysicalDevice* PhysicalDevices = (VkPhysicalDevice*)OpenVkMalloc(PhysicalDeviceCount * sizeof(VkPhysicalDevice));
	vkEnumeratePhysicalDevices(VkRenderer.Instance, &PhysicalDeviceCount, PhysicalDevices);

	for (uint32_t i = 0; i < PhysicalDeviceCount; i++)
		if (VkIsPhysicalDeviceSuitable(PhysicalDevices[i]))
			VkRenderer.PhysicalDevice = PhysicalDevices[i];

	OpenVkFree(PhysicalDevices);
	
	VkPhysicalDeviceProperties PhysicalDeviceProperties;
	vkGetPhysicalDeviceProperties(VkRenderer.PhysicalDevice, &PhysicalDeviceProperties);

	uint32_t ApiVersion = PhysicalDeviceProperties.apiVersion;
	OpenVkRuntimeInfo("Graphics Card: ", PhysicalDeviceProperties.deviceName);
	uint32_t VersionMajor = VK_VERSION_MAJOR(ApiVersion);
	uint32_t VersionMinor = VK_VERSION_MINOR(ApiVersion);
	char Version[32];
	sprintf(Version, "%d.%d", VersionMajor, VersionMinor);
	OpenVkRuntimeInfo("Api Version: ", Version);

	VkRenderer.MsaaSamples = VkGetMaxSampleCount();

	//Device
	float QueuePriority = 1.0;
	VkQueueFamilyIndices Indices = VkFindQueueFamilies(VkRenderer.PhysicalDevice);

	VkDeviceQueueCreateInfo DeviceQueueCreateInfos[2];
	DeviceQueueCreateInfos[0].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	DeviceQueueCreateInfos[0].pNext = NULL;
	DeviceQueueCreateInfos[0].flags = 0;
	DeviceQueueCreateInfos[0].queueFamilyIndex = Indices.GraphicsFamily;
	DeviceQueueCreateInfos[0].queueCount = 1;
	DeviceQueueCreateInfos[0].pQueuePriorities = &QueuePriority;

	DeviceQueueCreateInfos[1].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	DeviceQueueCreateInfos[1].pNext = NULL;
	DeviceQueueCreateInfos[1].flags = 0;
	DeviceQueueCreateInfos[1].queueFamilyIndex = Indices.PresentFamily;
	DeviceQueueCreateInfos[1].queueCount = 1;
	DeviceQueueCreateInfos[1].pQueuePriorities = &QueuePriority;

	VkPhysicalDeviceFeatures DeviceFeatures = { VK_FALSE };
	DeviceFeatures.samplerAnisotropy = VK_TRUE;
	DeviceFeatures.sampleRateShading = VK_TRUE;
	DeviceFeatures.fillModeNonSolid = VK_TRUE;
	DeviceFeatures.wideLines = VK_TRUE;
	DeviceFeatures.depthClamp = VK_TRUE;

	VkDeviceCreateInfo CreateInfo;
	CreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	CreateInfo.pNext = NULL;
	CreateInfo.flags = 0;
	CreateInfo.queueCreateInfoCount = 2;
	CreateInfo.pQueueCreateInfos = DeviceQueueCreateInfos;
	CreateInfo.pEnabledFeatures = &DeviceFeatures;
	CreateInfo.enabledLayerCount = EnableValidationLayers;
	CreateInfo.ppEnabledLayerNames = ValidationLayers;
	CreateInfo.enabledExtensionCount = 1;
	CreateInfo.ppEnabledExtensionNames = DeviceExtensions;

	if (vkCreateDevice(VkRenderer.PhysicalDevice, &CreateInfo, NULL, &VkRenderer.Device) != VK_SUCCESS)
		return OpenVkRuntimeError("Failed to Create Device");

	vkGetDeviceQueue(VkRenderer.Device, Indices.GraphicsFamily, 0, &VkRenderer.GraphicsQueue);
	vkGetDeviceQueue(VkRenderer.Device, Indices.PresentFamily, 0, &VkRenderer.PresentQueue);

	uint32_t Widht;
	uint32_t Height;
	GetWindowSizeFunc = GetWindowSize;
	VkCreateSwapChain(&Widht, &Height);

	//Command Pool
	VkCommandPoolCreateInfo PoolInfo;
	PoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	PoolInfo.pNext = NULL;
	PoolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	PoolInfo.queueFamilyIndex = Indices.GraphicsFamily;

	if (vkCreateCommandPool(VkRenderer.Device, &PoolInfo, NULL, &VkRenderer.CommandPool) != VK_SUCCESS)
		return OpenVkRuntimeError("Failed to Create Command Pool");

	VkCreateCommandBuffers();
	
	//Semaphoren
	VkRenderer.InFlightImages = (VkFence*)OpenVkMalloc(VkRenderer.SwapChainImageCount * sizeof(VkFence));

	for (uint32_t i = 0; i < VkRenderer.SwapChainImageCount; i++)
		VkRenderer.InFlightImages[i] = VK_NULL_HANDLE;

	VkSemaphoreCreateInfo SemaphoreInfo;
	SemaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
	SemaphoreInfo.pNext = NULL;
	SemaphoreInfo.flags = 0;

	VkFenceCreateInfo FenceInfo;
	FenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	FenceInfo.pNext = NULL;
	FenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

	for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
		if (vkCreateSemaphore(VkRenderer.Device, &SemaphoreInfo, NULL, &VkRenderer.ImageAvailableSemaphores[i]) != VK_SUCCESS ||
			vkCreateSemaphore(VkRenderer.Device, &SemaphoreInfo, NULL, &VkRenderer.RenderFinishedSemaphores[i]) != VK_SUCCESS ||
			vkCreateFence(VkRenderer.Device, &FenceInfo, NULL, &VkRenderer.InFlightFences[i]) != VK_SUCCESS)
			return OpenVkRuntimeError("Failed to Create Semaphores or Fence");

	VkRenderer.ImageCount = 1;
	return VkRenderer.ImageCount - 1;
}

uint32_t VkCreateColorImageAttachment(uint32_t Width, uint32_t Height, uint32_t MsaaSamples, OpenVkBool Sampled)
{
	VkFormat ColorFormat = VkRenderer.SwapChainImageFormat;

	VkRenderer.Images = (VkImage*)OpenVkRealloc(VkRenderer.Images, (VkRenderer.ImageCount + 1) * sizeof(VkImage));
	VkRenderer.ImageMemories = (VkDeviceMemory*)OpenVkRealloc(VkRenderer.ImageMemories, (VkRenderer.ImageCount + 1) * sizeof(VkDeviceMemory));
	VkRenderer.ImageViews = (VkImageView*)OpenVkRealloc(VkRenderer.ImageViews, (VkRenderer.ImageCount + 1) * sizeof(VkImageView));

	VkSampleCountFlagBits Samples = (VkSampleCountFlagBits)(VkRenderer.MsaaSamples < MsaaSamples ? VkRenderer.MsaaSamples : MsaaSamples);

	if (Sampled)
	{
		if (VkCreateImage(Width, Height, 1, Samples, ColorFormat, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &VkRenderer.Images[VkRenderer.ImageCount], &VkRenderer.ImageMemories[VkRenderer.ImageCount]) == OPENVK_ERROR)
			return OpenVkRuntimeError("Failed to Create Sampled Attachment Image");
		VkRenderer.ImageViews[VkRenderer.ImageCount] = VkCreateImageView(VkRenderer.Images[VkRenderer.ImageCount], ColorFormat, VK_IMAGE_ASPECT_COLOR_BIT, 1);
	}
	else
	{
		if (VkCreateImage(Width, Height, 1, Samples, ColorFormat, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &VkRenderer.Images[VkRenderer.ImageCount], &VkRenderer.ImageMemories[VkRenderer.ImageCount]) == OPENVK_ERROR)
			return OpenVkRuntimeError("Failed to Create Msaa Attachment Image");
		VkRenderer.ImageViews[VkRenderer.ImageCount] = VkCreateImageView(VkRenderer.Images[VkRenderer.ImageCount], ColorFormat, VK_IMAGE_ASPECT_COLOR_BIT, 1);
	}

	VkRenderer.ImageCount++;

	return VkRenderer.ImageCount - 1;
}

uint32_t VkCreateDepthImageAttachment(uint32_t Width, uint32_t Height, uint32_t MsaaSamples, OpenVkBool Sampled)
{
	VkFormat DepthFormat = VkFindDepthFormat();

	VkRenderer.Images = (VkImage*)OpenVkRealloc(VkRenderer.Images, (VkRenderer.ImageCount + 1) * sizeof(VkImage));
	VkRenderer.ImageMemories = (VkDeviceMemory*)OpenVkRealloc(VkRenderer.ImageMemories, (VkRenderer.ImageCount + 1) * sizeof(VkDeviceMemory));
	VkRenderer.ImageViews = (VkImageView*)OpenVkRealloc(VkRenderer.ImageViews, (VkRenderer.ImageCount + 1) * sizeof(VkImageView));

	VkSampleCountFlagBits Samples = (VkSampleCountFlagBits)(VkRenderer.MsaaSamples < MsaaSamples ? VkRenderer.MsaaSamples : MsaaSamples);

	if (Sampled)
	{
		if (VkCreateImage(Width, Height, 1, Samples, DepthFormat, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &VkRenderer.Images[VkRenderer.ImageCount], &VkRenderer.ImageMemories[VkRenderer.ImageCount]) == OPENVK_ERROR)
			return OpenVkRuntimeError("Failed to Create Depth Sampled Attachment Image");
		VkRenderer.ImageViews[VkRenderer.ImageCount] = VkCreateImageView(VkRenderer.Images[VkRenderer.ImageCount], DepthFormat, VK_IMAGE_ASPECT_DEPTH_BIT, 1);
	}
	else
	{
		if (VkCreateImage(Width, Height, 1, Samples, DepthFormat, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &VkRenderer.Images[VkRenderer.ImageCount], &VkRenderer.ImageMemories[VkRenderer.ImageCount]) == OPENVK_ERROR)
			return OpenVkRuntimeError("Failed to Create Depth Attachment Image");
		VkRenderer.ImageViews[VkRenderer.ImageCount] = VkCreateImageView(VkRenderer.Images[VkRenderer.ImageCount], DepthFormat, VK_IMAGE_ASPECT_DEPTH_BIT, 1);
	}	

	VkRenderer.ImageCount++;

	return VkRenderer.ImageCount - 1;
}

uint32_t VkCreateRenderPass(uint32_t ColorAttachmentCount, OpenVkBool DepthAttachment, OpenVkBool MsaaAttachment, uint32_t MsaaSamples, OpenVkBool Sampled)
{
	VkRenderer.RenderPasses = (VkRenderPass*)OpenVkRealloc(VkRenderer.RenderPasses, (VkRenderer.RenderPassCount + 1) * sizeof(VkRenderPass));
	
	VkAttachmentDescription ColorAttachmentDescription;
	VkAttachmentDescription DepthAttachmentDescription;
	VkAttachmentDescription ColorAttachmentResolveDescription;

	VkSampleCountFlagBits Samples = (VkSampleCountFlagBits)(VkRenderer.MsaaSamples < MsaaSamples ? VkRenderer.MsaaSamples : MsaaSamples);

	if (ColorAttachmentCount > 0)
	{
		ColorAttachmentDescription.flags = 0;
		ColorAttachmentDescription.format = VkRenderer.SwapChainImageFormat;
		ColorAttachmentDescription.samples = Samples;
		ColorAttachmentDescription.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		ColorAttachmentDescription.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		ColorAttachmentDescription.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		ColorAttachmentDescription.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		ColorAttachmentDescription.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		ColorAttachmentDescription.finalLayout = (MsaaAttachment ? VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL : (Sampled ? VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL : VK_IMAGE_LAYOUT_PRESENT_SRC_KHR));
	}

	if (DepthAttachment && ColorAttachmentCount > 0)
	{
		DepthAttachmentDescription.flags = 0;
		DepthAttachmentDescription.format = VkFindDepthFormat();
		DepthAttachmentDescription.samples = Samples;
		DepthAttachmentDescription.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		DepthAttachmentDescription.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		DepthAttachmentDescription.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		DepthAttachmentDescription.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		DepthAttachmentDescription.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		DepthAttachmentDescription.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
	}
	else if (DepthAttachment && ColorAttachmentCount == 0)
	{
		DepthAttachmentDescription.flags = 0;
		DepthAttachmentDescription.format = VkFindDepthFormat();
		DepthAttachmentDescription.samples = Samples;
		DepthAttachmentDescription.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		DepthAttachmentDescription.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		DepthAttachmentDescription.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		DepthAttachmentDescription.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		DepthAttachmentDescription.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		DepthAttachmentDescription.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;
	}

	if (MsaaAttachment)
	{
		ColorAttachmentResolveDescription.flags = 0;
		ColorAttachmentResolveDescription.format = VkRenderer.SwapChainImageFormat;
		ColorAttachmentResolveDescription.samples = VK_SAMPLE_COUNT_1_BIT;
		ColorAttachmentResolveDescription.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		ColorAttachmentResolveDescription.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		ColorAttachmentResolveDescription.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		ColorAttachmentResolveDescription.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		ColorAttachmentResolveDescription.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		ColorAttachmentResolveDescription.finalLayout = (Sampled ? VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL : VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);
	}


	VkAttachmentReference* ColorAttachmentReferences = NULL;
	VkAttachmentReference  DepthAttachmentReference;
	VkAttachmentReference* ColorAttachmentResolveReferences = NULL;

	if (ColorAttachmentCount > 0)
	{
		ColorAttachmentReferences = (VkAttachmentReference*)OpenVkMalloc(ColorAttachmentCount * sizeof(VkAttachmentReference));
		for (uint32_t i = 0; i < ColorAttachmentCount; i++)
		{
			ColorAttachmentReferences[i].attachment = i;
			ColorAttachmentReferences[i].layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		}
	}    

	if (DepthAttachment)
	{
		DepthAttachmentReference.attachment = ColorAttachmentCount;
		DepthAttachmentReference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
	}

	if (MsaaAttachment)
	{
		uint32_t ResolveRefrenceCount = 0;
		ColorAttachmentResolveReferences = (VkAttachmentReference*)OpenVkMalloc(ColorAttachmentCount * sizeof(VkAttachmentReference));;
		for (uint32_t i = ColorAttachmentCount + DepthAttachment; i < (ColorAttachmentCount * 2) + DepthAttachment; i++)
		{
			ColorAttachmentResolveReferences[ResolveRefrenceCount].attachment = i;
			ColorAttachmentResolveReferences[ResolveRefrenceCount].layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

			ResolveRefrenceCount++;
		}
	}

	VkSubpassDescription Subpass;
	Subpass.flags = 0;
	Subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	Subpass.inputAttachmentCount = 0;
	Subpass.pInputAttachments = NULL;
	Subpass.colorAttachmentCount = ColorAttachmentCount;
	Subpass.pColorAttachments = ColorAttachmentReferences;
	Subpass.pResolveAttachments = ColorAttachmentResolveReferences;
	Subpass.pDepthStencilAttachment = (DepthAttachment ? &DepthAttachmentReference : NULL);
	Subpass.preserveAttachmentCount = NULL;
	Subpass.pPreserveAttachments = NULL;

	VkSubpassDependency Dependencies[2];
	Dependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;
	Dependencies[0].dstSubpass = 0;
	Dependencies[0].srcStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
	Dependencies[0].srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
	Dependencies[0].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

	Dependencies[1].srcSubpass = 0;
	Dependencies[1].dstSubpass = VK_SUBPASS_EXTERNAL;
	Dependencies[1].dstStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
	Dependencies[1].dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
	Dependencies[1].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

	if (ColorAttachmentCount > 0)
	{
		Dependencies[0].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		Dependencies[0].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

		Dependencies[1].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		Dependencies[1].srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
	}
	else
	{
		Dependencies[0].dstStageMask = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
		Dependencies[0].dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

		Dependencies[1].srcStageMask = VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
		Dependencies[1].srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
	}

	uint32_t AttachmentCount = ((ColorAttachmentCount * (MsaaAttachment == 1 ? 2 : 1)) + DepthAttachment);
	VkAttachmentDescription* AttachmentDescriptions = (VkAttachmentDescription*)OpenVkMalloc(AttachmentCount * sizeof(VkAttachmentDescription));
	for (uint32_t i = 0; i < ColorAttachmentCount; i++)
		AttachmentDescriptions[i] = ColorAttachmentDescription;
	for (uint32_t i = ColorAttachmentCount; i < ColorAttachmentCount + DepthAttachment; i++)
		AttachmentDescriptions[i] = DepthAttachmentDescription;
	for (uint32_t i = ColorAttachmentCount + DepthAttachment; i < AttachmentCount; i++)
		AttachmentDescriptions[i] = ColorAttachmentResolveDescription;

	VkRenderPassCreateInfo RenderPassInfo;
	RenderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	RenderPassInfo.pNext = NULL;
	RenderPassInfo.flags = 0;
	RenderPassInfo.attachmentCount = AttachmentCount;
	RenderPassInfo.pAttachments = AttachmentDescriptions;
	RenderPassInfo.subpassCount = 1;
	RenderPassInfo.pSubpasses = &Subpass;
	RenderPassInfo.dependencyCount = 2;
	RenderPassInfo.pDependencies = Dependencies;

	if (vkCreateRenderPass(VkRenderer.Device, &RenderPassInfo, NULL, &VkRenderer.RenderPasses[VkRenderer.RenderPassCount]) != VK_SUCCESS)
		return OpenVkRuntimeError("Failed to Create Render Pass");

	if (ColorAttachmentCount > 0)
		OpenVkFree(ColorAttachmentReferences);
	if (MsaaAttachment)
		OpenVkFree(ColorAttachmentResolveReferences);
	
	OpenVkFree(AttachmentDescriptions);

	VkRenderer.RenderPassCount++;

	return VkRenderer.RenderPassCount - 1;
}

//Shader Types
//0 = Vertex Shader
//1 = Fragment Shader
//Primitive Topology
//0 = Point
//1 = Line
//2 = Triangle
//Polygon Mode
//0 = Fill
//1 = Line
//2 = Point
//Cull Mode
//0 = None
//1 = Front
//2 = Back
//3 = Front and Back
//Front Face
//0 = CCW
//1 = CW
uint32_t VkCreateGraphicsPipeline(const char* VertexPath, const char* FragmentPath,
								  size_t BindingStride, uint32_t ShaderAttributeFormatCount,
								  uint32_t* ShaderAttributeFormats, uint32_t* ShaderAttributeOffsets,
								  uint32_t PrimitiveTopology, uint32_t x, uint32_t y, uint32_t Width, uint32_t Height,
								  OpenVkBool DepthClamp, uint32_t PolygonMode, float LineWidth, uint32_t CullMode, uint32_t FrontFace,
								  uint32_t MsaaSamples, OpenVkBool AlphaBlending, uint32_t ColorBlendAttachments, 
								  uint32_t PushConstantCount, uint32_t* PushConstantShaderTypes, uint32_t* PushConstantOffsets, uint32_t* PushConstantSizes,
								  uint32_t DescriptorSetLayoutCount, uint32_t* DescriptorSetLayouts, OpenVkBool DepthStencil, uint32_t RenderPass)
{
	VkRenderer.PipelineLayouts = (VkPipelineLayout*)OpenVkRealloc(VkRenderer.PipelineLayouts, (VkRenderer.PipelineCount + 1) * sizeof(VkPipelineLayout));
	VkRenderer.Pipelines = (VkPipeline*)OpenVkRealloc(VkRenderer.Pipelines, (VkRenderer.PipelineCount + 1) * sizeof(VkPipeline));

	VkShaderModule VertexShaderModule;
	VkCreateShaderModule(VertexPath, &VertexShaderModule);
	VkShaderModule FragmentShaderModule;
	VkCreateShaderModule(FragmentPath, &FragmentShaderModule);

	VkPipelineShaderStageCreateInfo ShaderStageInfos[2];
	for (uint32_t i = 0; i < 2; i++)
	{
		ShaderStageInfos[i].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		ShaderStageInfos[i].pNext = NULL;
		ShaderStageInfos[i].flags = 0;
		ShaderStageInfos[i].stage = (i == 0 ? VK_SHADER_STAGE_VERTEX_BIT : VK_SHADER_STAGE_FRAGMENT_BIT);
		ShaderStageInfos[i].module = (i == 0 ? VertexShaderModule : FragmentShaderModule);
		ShaderStageInfos[i].pName = "main";
		ShaderStageInfos[i].pSpecializationInfo = NULL;
	}

	VkVertexInputBindingDescription BindingDescription;
	BindingDescription.binding = 0;
	BindingDescription.stride = BindingStride;
	BindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

	uint32_t AttributeDescriptionCount = ShaderAttributeFormatCount;
	VkVertexInputAttributeDescription* AttributeDescriptions = (VkVertexInputAttributeDescription*)OpenVkMalloc(AttributeDescriptionCount * sizeof(VkVertexInputAttributeDescription));
	for (uint32_t i = 0; i < AttributeDescriptionCount; i++)
	{
		AttributeDescriptions[i].location = i;
		AttributeDescriptions[i].binding = 0;
		if (ShaderAttributeFormats[i] == 1) AttributeDescriptions[i].format = VK_FORMAT_R32_SFLOAT;
		if (ShaderAttributeFormats[i] == 2) AttributeDescriptions[i].format = VK_FORMAT_R32G32_SFLOAT;
		if (ShaderAttributeFormats[i] == 3) AttributeDescriptions[i].format = VK_FORMAT_R32G32B32_SFLOAT;
		if (ShaderAttributeFormats[i] == 4) AttributeDescriptions[i].format = VK_FORMAT_R32G32B32A32_SFLOAT;
		AttributeDescriptions[i].offset = ShaderAttributeOffsets[i];
	}

	VkPipelineVertexInputStateCreateInfo VertexInputInfo;
	VertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	VertexInputInfo.pNext = NULL;
	VertexInputInfo.flags = 0;
	VertexInputInfo.vertexBindingDescriptionCount = 1;
	VertexInputInfo.pVertexBindingDescriptions = &BindingDescription;
	VertexInputInfo.vertexAttributeDescriptionCount = AttributeDescriptionCount;
	VertexInputInfo.pVertexAttributeDescriptions = AttributeDescriptions;

	VkPipelineInputAssemblyStateCreateInfo InputAssembly;
	InputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	InputAssembly.pNext = NULL;
	InputAssembly.flags = 0;
	if (PrimitiveTopology == 0) InputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_POINT_LIST;
	if (PrimitiveTopology == 1) InputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_LINE_LIST;
	if (PrimitiveTopology == 2) InputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	InputAssembly.primitiveRestartEnable = VK_FALSE;

	VkViewport Viewport;
	Viewport.x = x;
	Viewport.y = y;
	Viewport.width = Width;
	Viewport.height = Height;
	Viewport.minDepth = 0.0;
	Viewport.maxDepth = 1.0;

	VkOffset2D Offset;
	Offset.x = 0;
	Offset.y = 0;

	VkRect2D Scissor;
	Scissor.offset = Offset;
	Scissor.extent.width = Width;
	Scissor.extent.height = Height;

	VkPipelineViewportStateCreateInfo ViewportState;
	ViewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	ViewportState.pNext = NULL;
	ViewportState.flags = 0;
	ViewportState.viewportCount = 1;
	ViewportState.pViewports = &Viewport;
	ViewportState.scissorCount = 1;
	ViewportState.pScissors = &Scissor;

	VkPipelineRasterizationStateCreateInfo Rasterizer;
	Rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	Rasterizer.pNext = NULL;
	Rasterizer.flags = 0;
	Rasterizer.depthClampEnable = VK_TRUE;
	Rasterizer.rasterizerDiscardEnable = VK_FALSE;
	Rasterizer.polygonMode = (VkPolygonMode)PolygonMode;
	Rasterizer.lineWidth = LineWidth;
	Rasterizer.cullMode = CullMode;
	Rasterizer.frontFace = (VkFrontFace)FrontFace;
	Rasterizer.depthBiasEnable = VK_FALSE;
	Rasterizer.depthBiasConstantFactor = 1.25;
	Rasterizer.depthBiasClamp = 0.0;
	Rasterizer.depthBiasSlopeFactor = 1.75;

	VkPipelineMultisampleStateCreateInfo Multisampling;
	Multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	Multisampling.pNext = NULL;
	Multisampling.flags = 0;
	Multisampling.sampleShadingEnable = VK_FALSE;
	Multisampling.rasterizationSamples = (VkSampleCountFlagBits)(VkRenderer.MsaaSamples < MsaaSamples ? VkRenderer.MsaaSamples : MsaaSamples);
	Multisampling.minSampleShading = 1.0;
	Multisampling.pSampleMask = NULL;
	Multisampling.alphaToCoverageEnable = VK_FALSE;
	Multisampling.alphaToOneEnable = VK_FALSE;

	VkPipelineDepthStencilStateCreateInfo DepthStencilState;
	DepthStencilState.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
	DepthStencilState.pNext = NULL;
	DepthStencilState.flags = 0;
	DepthStencilState.depthTestEnable = VK_TRUE;
	DepthStencilState.depthWriteEnable = VK_TRUE;
	DepthStencilState.depthCompareOp = VK_COMPARE_OP_LESS;
	DepthStencilState.depthBoundsTestEnable = VK_FALSE;
	DepthStencilState.stencilTestEnable = VK_FALSE;
	DepthStencilState.front.failOp = (VkStencilOp)VK_NULL_HANDLE;
	DepthStencilState.front.passOp = (VkStencilOp)VK_NULL_HANDLE;
	DepthStencilState.front.depthFailOp = (VkStencilOp)VK_NULL_HANDLE;
	DepthStencilState.front.compareOp = (VkCompareOp)VK_NULL_HANDLE;
	DepthStencilState.front.compareMask = VK_NULL_HANDLE;
	DepthStencilState.front.writeMask = VK_NULL_HANDLE;
	DepthStencilState.front.reference = VK_NULL_HANDLE;
	DepthStencilState.back.failOp = (VkStencilOp)VK_NULL_HANDLE;
	DepthStencilState.back.passOp = (VkStencilOp)VK_NULL_HANDLE;
	DepthStencilState.back.depthFailOp = (VkStencilOp)VK_NULL_HANDLE;
	DepthStencilState.back.compareOp = (VkCompareOp)VK_NULL_HANDLE;
	DepthStencilState.back.compareMask = VK_NULL_HANDLE;
	DepthStencilState.back.writeMask = VK_NULL_HANDLE;
	DepthStencilState.back.reference = VK_NULL_HANDLE;
	DepthStencilState.minDepthBounds = 0.0;
	DepthStencilState.maxDepthBounds = 1.0;

	VkPipelineColorBlendAttachmentState* ColorBlendAttachmentStates = (VkPipelineColorBlendAttachmentState*)OpenVkMalloc(ColorBlendAttachments * sizeof(VkPipelineColorBlendAttachmentState));
	for (uint32_t i = 0; i < ColorBlendAttachments; i++)
	{
		ColorBlendAttachmentStates[i].blendEnable = AlphaBlending;
		ColorBlendAttachmentStates[i].srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
		ColorBlendAttachmentStates[i].dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
		ColorBlendAttachmentStates[i].colorBlendOp = VK_BLEND_OP_ADD;
		ColorBlendAttachmentStates[i].srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
		ColorBlendAttachmentStates[i].dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
		ColorBlendAttachmentStates[i].alphaBlendOp = VK_BLEND_OP_ADD;
		ColorBlendAttachmentStates[i].colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
	}	

	VkPipelineColorBlendStateCreateInfo ColorBlending;
	ColorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	ColorBlending.pNext = NULL;
	ColorBlending.flags = 0;
	ColorBlending.logicOpEnable = VK_FALSE;
	ColorBlending.logicOp = VK_LOGIC_OP_COPY;
	ColorBlending.attachmentCount = ColorBlendAttachments;
	ColorBlending.pAttachments = ColorBlendAttachmentStates;
	ColorBlending.blendConstants[0] = 0.0;
	ColorBlending.blendConstants[1] = 0.0;
	ColorBlending.blendConstants[2] = 0.0;
	ColorBlending.blendConstants[3] = 0.0;

	VkDescriptorSetLayout* SetLayouts = NULL;

	if (DescriptorSetLayoutCount > 0)
	{
		SetLayouts = (VkDescriptorSetLayout*)OpenVkMalloc(DescriptorSetLayoutCount * sizeof(VkDescriptorSetLayout));
		for (uint32_t i = 0; i < DescriptorSetLayoutCount; i++)
		{
			SetLayouts[i] = VkRenderer.DescriptorSetLayouts[DescriptorSetLayouts[i]];
		}
	}

	VkPushConstantRange* PushConstantRanges = NULL;

	if (PushConstantCount > 0)
	{
		PushConstantRanges = (VkPushConstantRange*)OpenVkMalloc(PushConstantCount * sizeof(VkPushConstantRange));
		for (uint32_t i = 0; i < PushConstantCount; i++)
		{
			if (PushConstantShaderTypes[i] == 0) PushConstantRanges[i].stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
			if (PushConstantShaderTypes[i] == 1) PushConstantRanges[i].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
			PushConstantRanges[i].offset = PushConstantOffsets[i];
			PushConstantRanges[i].size = PushConstantSizes[i];
		}
	}    

	VkPipelineLayoutCreateInfo PipeLineLayoutInfo;
	PipeLineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	PipeLineLayoutInfo.pNext = NULL;
	PipeLineLayoutInfo.flags = 0;
	PipeLineLayoutInfo.setLayoutCount = DescriptorSetLayoutCount;
	PipeLineLayoutInfo.pSetLayouts = SetLayouts;
	PipeLineLayoutInfo.pushConstantRangeCount = PushConstantCount;
	PipeLineLayoutInfo.pPushConstantRanges = PushConstantRanges;

	if (vkCreatePipelineLayout(VkRenderer.Device, &PipeLineLayoutInfo, NULL, &VkRenderer.PipelineLayouts[VkRenderer.PipelineCount]) != VK_SUCCESS)
		return OpenVkRuntimeError("Failed to Create Pipeline Layout");

	VkDynamicState DynamicStates[2] = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };

	VkPipelineDynamicStateCreateInfo DynamicStateInfo;
	DynamicStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	DynamicStateInfo.pNext = NULL;
	DynamicStateInfo.flags = 0;
	DynamicStateInfo.dynamicStateCount = 2;
	DynamicStateInfo.pDynamicStates = DynamicStates;

	VkGraphicsPipelineCreateInfo PipelineInfo;
	PipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	PipelineInfo.pNext = NULL;
	PipelineInfo.flags = 0;
	PipelineInfo.stageCount = 2;
	PipelineInfo.pStages = ShaderStageInfos;
	PipelineInfo.pVertexInputState = &VertexInputInfo;
	PipelineInfo.pInputAssemblyState = &InputAssembly;
	PipelineInfo.pTessellationState = NULL;
	PipelineInfo.pViewportState = &ViewportState;
	PipelineInfo.pRasterizationState = &Rasterizer;
	PipelineInfo.pMultisampleState = &Multisampling;
	PipelineInfo.pDepthStencilState = (DepthStencil ? &DepthStencilState : NULL);
	PipelineInfo.pColorBlendState = &ColorBlending;
	PipelineInfo.pDynamicState = &DynamicStateInfo;
	PipelineInfo.layout = VkRenderer.PipelineLayouts[VkRenderer.PipelineCount];
	PipelineInfo.renderPass = VkRenderer.RenderPasses[RenderPass];
	PipelineInfo.subpass = 0;
	PipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
	PipelineInfo.basePipelineIndex = -1;

	if (vkCreateGraphicsPipelines(VkRenderer.Device, VK_NULL_HANDLE, 1, &PipelineInfo, NULL, &VkRenderer.Pipelines[VkRenderer.PipelineCount]) != VK_SUCCESS)
		return OpenVkRuntimeError("Failed to Create Graphics Pipeline");

	vkDestroyShaderModule(VkRenderer.Device, VertexShaderModule, NULL);
	vkDestroyShaderModule(VkRenderer.Device, FragmentShaderModule, NULL);

	OpenVkFree(AttributeDescriptions);
	if (DescriptorSetLayoutCount > 0)
		OpenVkFree(SetLayouts);
	if (PushConstantCount > 0)
		OpenVkFree(PushConstantRanges);
	if (ColorBlendAttachments > 0)
		OpenVkFree(ColorBlendAttachmentStates);

	VkRenderer.PipelineCount++;

	return VkRenderer.PipelineCount - 1;
}

//TO-DO Needs to be fixed
void VkDestroyGraphicsPipeline(uint32_t GraphicsPipeline)
{
	vkDeviceWaitIdle(VkRenderer.Device);

	vkDestroyPipelineLayout(VkRenderer.Device, VkRenderer.PipelineLayouts[GraphicsPipeline], NULL);
	vkDestroyPipeline(VkRenderer.Device, VkRenderer.Pipelines[GraphicsPipeline], NULL);

	for (uint32_t i = GraphicsPipeline; i < VkRenderer.PipelineCount - 1; i++)
	{
		VkRenderer.Pipelines[i] = VkRenderer.Pipelines[i + 1];
		VkRenderer.PipelineLayouts[i] = VkRenderer.PipelineLayouts[i + 1];
	}

	VkRenderer.Pipelines = (VkPipeline*)OpenVkRealloc(VkRenderer.Pipelines, VkRenderer.PipelineCount * sizeof(VkPipeline));
	VkRenderer.PipelineLayouts = (VkPipelineLayout*)OpenVkRealloc(VkRenderer.PipelineLayouts, VkRenderer.PipelineCount * sizeof(VkPipelineLayout));
	VkRenderer.PipelineCount--;
}

uint32_t VkCreateFrambuffer(uint32_t AttachmentCount, uint32_t* Attachments, uint32_t RenderPass, uint32_t Width, uint32_t Height)
{
	VkRenderer.Framebuffers = (VkFramebufferInfo*)OpenVkRealloc(VkRenderer.Framebuffers, (VkRenderer.FramebufferCount + 1) * sizeof(VkFramebufferInfo));
	VkRenderer.Framebuffers[VkRenderer.FramebufferCount].Framebuffers = (VkFramebuffer*)OpenVkMalloc(VkRenderer.SwapChainImageCount * sizeof(VkFramebuffer));

	VkImageView* ImageViewAttachments = (VkImageView*)OpenVkMalloc(AttachmentCount * sizeof(VkImageView));
		
	for (uint32_t i = 0; i < VkRenderer.SwapChainImageCount; i++)
	{
		for (uint32_t j = 0; j < AttachmentCount; j++)
		{
			if (Attachments[j] == 0)
				ImageViewAttachments[j] = VkRenderer.SwapChainImageViews[i];
			else
				ImageViewAttachments[j] = VkRenderer.ImageViews[Attachments[j]];
		}
			

		VkFramebufferCreateInfo FramebufferInfo;
		FramebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		FramebufferInfo.pNext = NULL;
		FramebufferInfo.flags = 0;
		FramebufferInfo.renderPass = VkRenderer.RenderPasses[RenderPass];
		FramebufferInfo.attachmentCount = AttachmentCount;
		FramebufferInfo.pAttachments = ImageViewAttachments;
		FramebufferInfo.width = Width;
		FramebufferInfo.height = Height;
		FramebufferInfo.layers = 1;

		if (vkCreateFramebuffer(VkRenderer.Device, &FramebufferInfo, NULL, &VkRenderer.Framebuffers[VkRenderer.FramebufferCount].Framebuffers[i]) != VK_SUCCESS)
			return OpenVkRuntimeError("Failed to Create Framebuffer");
	}

	OpenVkFree(ImageViewAttachments);

	VkRenderer.FramebufferCount++;

	return VkRenderer.FramebufferCount - 1;
}

//DescriptorTypes
//0 = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER
//1 = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
//ShaderTypes
//0 = Vertex Shader
//1 = Fragment Shader
uint32_t VkCreateDescriptorSetLayout(uint32_t Binding, uint32_t BindingCount, uint32_t* DescriptorCounts, uint32_t* DescriptorTypes, uint32_t* ShaderTypes)
{
	VkRenderer.DescriptorSetLayouts = (VkDescriptorSetLayout*)OpenVkRealloc(VkRenderer.DescriptorSetLayouts, (VkRenderer.DescriptorSetLayoutCount + 1) * sizeof(VkDescriptorSetLayout));

	VkDescriptorSetLayoutBinding* LayoutBindings = (VkDescriptorSetLayoutBinding*)OpenVkMalloc(BindingCount * sizeof(VkDescriptorSetLayoutBinding));

	for (uint32_t i = 0; i < BindingCount; i++)
	{
		LayoutBindings[i].binding = Binding + i;
		if (DescriptorTypes[i] == 0) LayoutBindings[i].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		if (DescriptorTypes[i] == 1) LayoutBindings[i].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
		if (DescriptorTypes[i] == 2) LayoutBindings[i].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		LayoutBindings[i].descriptorCount = DescriptorCounts[i];
		if (ShaderTypes[i] == 0) LayoutBindings[i].stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
		if (ShaderTypes[i] == 1) LayoutBindings[i].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
		LayoutBindings[i].pImmutableSamplers = NULL;
	}

	VkDescriptorSetLayoutCreateInfo DescriptorSetLayoutCreateInfo;
	DescriptorSetLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	DescriptorSetLayoutCreateInfo.pNext = NULL;
	DescriptorSetLayoutCreateInfo.flags = 0;
	DescriptorSetLayoutCreateInfo.bindingCount = BindingCount;
	DescriptorSetLayoutCreateInfo.pBindings = LayoutBindings;

	if (vkCreateDescriptorSetLayout(VkRenderer.Device, &DescriptorSetLayoutCreateInfo, NULL, &VkRenderer.DescriptorSetLayouts[VkRenderer.DescriptorSetLayoutCount]) != VK_SUCCESS)
		return OpenVkRuntimeError("Failed to Create Descriptor Set Layout");
	OpenVkFree(LayoutBindings);

	VkRenderer.DescriptorSetLayoutCount++;
	return VkRenderer.DescriptorSetLayoutCount - 1;
}

//DescriptorTypes
//Uniform Buffer = 0
//Dynamic Uniform Buffer = 1
//Image Sampler = 2
//Pool Types
//0 = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER
//1 = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE
uint32_t VkCreateDescriptorPool(uint32_t PoolSizeCount, uint32_t* DescriptorTypes, uint32_t* DescriptorCounts, OpenVkBool Dynamic)
{
	if (Dynamic)
		VkRenderer.DynamicDescriptorPools.DescriptorPools = (VkDescriptorPool*)OpenVkRealloc(VkRenderer.DynamicDescriptorPools.DescriptorPools, (VkRenderer.DynamicDescriptorPools.DescriptorPoolCount + 1) * sizeof(VkDescriptorPool));
	else
		VkRenderer.StaticDescriptorPools.DescriptorPools = (VkDescriptorPool*)OpenVkRealloc(VkRenderer.StaticDescriptorPools.DescriptorPools, (VkRenderer.StaticDescriptorPools.DescriptorPoolCount + 1) * sizeof(VkDescriptorPool));

	uint32_t MaxSets = 0;

	VkDescriptorPoolSize* PoolSizes = (VkDescriptorPoolSize*)OpenVkMalloc(PoolSizeCount * sizeof(VkDescriptorPoolSize));

	for (uint32_t i = 0; i < PoolSizeCount; i++)
	{
		if (DescriptorTypes[i] == 0) PoolSizes[i].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		if (DescriptorTypes[i] == 1) PoolSizes[i].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
		if (DescriptorTypes[i] == 2) PoolSizes[i].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		PoolSizes[i].descriptorCount = DescriptorCounts[i] * MAX_FRAMES_IN_FLIGHT;
		
		MaxSets += DescriptorCounts[i] * MAX_FRAMES_IN_FLIGHT;
	}

	VkDescriptorPoolCreateInfo PoolInfo;
	PoolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	PoolInfo.pNext = NULL;
	if (Dynamic)
	{
		PoolInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
		PoolInfo.maxSets = 512 * MaxSets * MAX_FRAMES_IN_FLIGHT;
	}
	else
	{
		PoolInfo.flags = 0;
		PoolInfo.maxSets = MaxSets * MAX_FRAMES_IN_FLIGHT;	//maximum number of descriptor sets that may be allocated
	}	
	PoolInfo.poolSizeCount = PoolSizeCount;
	PoolInfo.pPoolSizes = PoolSizes;

	if (Dynamic)
	{
		if (vkCreateDescriptorPool(VkRenderer.Device, &PoolInfo, NULL, &VkRenderer.DynamicDescriptorPools.DescriptorPools[VkRenderer.DynamicDescriptorPools.DescriptorPoolCount]) != VK_SUCCESS)
			return OpenVkRuntimeError("Failed to Create Dynamic Descriptor Pool");

		OpenVkFree(PoolSizes);

		VkRenderer.DynamicDescriptorPools.DescriptorPoolCount++;
		return VkRenderer.DynamicDescriptorPools.DescriptorPoolCount - 1;
	}
	else
	{
		if (vkCreateDescriptorPool(VkRenderer.Device, &PoolInfo, NULL, &VkRenderer.StaticDescriptorPools.DescriptorPools[VkRenderer.StaticDescriptorPools.DescriptorPoolCount]) != VK_SUCCESS)
			return OpenVkRuntimeError("Failed to Create Static Descriptor Pool");

		OpenVkFree(PoolSizes);

		VkRenderer.StaticDescriptorPools.DescriptorPoolCount++;
		return VkRenderer.StaticDescriptorPools.DescriptorPoolCount - 1;
	}
}

//DescriptorTypes
//Uniform Buffer = 0
//Dynamic Uniform Buffer = 1
//Image Sampler = 2
//ImageTypes
//Texture Image = 0
//Attachment Image = 1
//ImageLayouts
//Color Out = 0
//Depth Out = 1
uint32_t VkCreateDescriptorSet(uint32_t DescriptorSetLayout, OpenVkBool DynamicDescriptorPool, uint32_t DescriptorPool, 
							   uint32_t DescriptorWriteCount, uint32_t* DescriptorCounts, uint32_t* DescriptorTypes,
							   uint32_t* UniformBuffers, uint64_t* UniformBufferSizes, uint32_t* Sampler, uint32_t* ImageTypes, 
							   uint32_t* Images, uint32_t* ImageLayouts, uint32_t* Bindings)
{
//	VkRenderer.DescriptorSets = (VkDescriptorSetInfo*)OpenVkRealloc(VkRenderer.DescriptorSets, (VkRenderer.DescriptorSetCount + 1) * sizeof(VkDescriptorSetInfo));

	VkDescriptorSetInfo DescriptorSetInfo;

	VkDescriptorSetLayout DescriptorSetLayouts[MAX_FRAMES_IN_FLIGHT];
	for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
		DescriptorSetLayouts[i] = VkRenderer.DescriptorSetLayouts[DescriptorSetLayout];

	VkDescriptorSetAllocateInfo AllocateInfo;
	if (DynamicDescriptorPool)
		AllocateInfo = VkAllocateDescriptorSets(VkRenderer.DynamicDescriptorPools.DescriptorPools[DescriptorPool], MAX_FRAMES_IN_FLIGHT, DescriptorSetLayouts);
	else
		AllocateInfo = VkAllocateDescriptorSets(VkRenderer.StaticDescriptorPools.DescriptorPools[DescriptorPool], MAX_FRAMES_IN_FLIGHT, DescriptorSetLayouts);

	if (vkAllocateDescriptorSets(VkRenderer.Device, &AllocateInfo, DescriptorSetInfo.DescriptorSets) != VK_SUCCESS)
		return OpenVkRuntimeError("Failed to Allocate Descriptor Set");

	VkWriteDescriptorSet* DescriptorWrites = (VkWriteDescriptorSet*)OpenVkMalloc(DescriptorWriteCount * sizeof(VkWriteDescriptorSet));
	
	VkDescriptorBufferInfo* DescriptorBufferInfos = NULL;
	VkDescriptorImageInfo* DescriptorImageInfos = NULL;

	for (uint32_t j = 0; j < MAX_FRAMES_IN_FLIGHT; j++)
	{
		for (uint32_t i = 0; i < DescriptorWriteCount; i++)
		{
			if (DescriptorTypes[i] == 0)
			{
				//Uniform
				if (DescriptorBufferInfos != NULL) OpenVkFree(DescriptorBufferInfos);
				DescriptorBufferInfos = (VkDescriptorBufferInfo*)malloc(DescriptorCounts[i] * sizeof(VkDescriptorBufferInfo));
				
				for (uint32_t m = 0; m < DescriptorCounts[i]; m++)
				{
					DescriptorBufferInfos[m].buffer = VkRenderer.UniformBuffers[UniformBuffers[m]].Buffers[j];
					DescriptorBufferInfos[m].offset = 0;
					DescriptorBufferInfos[m].range = UniformBufferSizes[m];
				}
				
				DescriptorWrites[i] = VkDescriptorSetWrite(DescriptorSetInfo.DescriptorSets[j], Bindings[i], VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, DescriptorCounts[i], NULL, DescriptorBufferInfos);
			}
			if (DescriptorTypes[i] == 1)
			{
				//Dynamic Uniform
				if (DescriptorBufferInfos != NULL) OpenVkFree(DescriptorBufferInfos);
				DescriptorBufferInfos = (VkDescriptorBufferInfo*)malloc(DescriptorCounts[i] * sizeof(VkDescriptorBufferInfo));

				for (uint32_t m = 0; m < DescriptorCounts[i]; m++)
				{
					DescriptorBufferInfos[m].buffer = VkRenderer.UniformBuffers[UniformBuffers[m]].Buffers[j];
					DescriptorBufferInfos[m].offset = 0;
					DescriptorBufferInfos[m].range = UniformBufferSizes[m];
				}

				DescriptorWrites[i] = VkDescriptorSetWrite(DescriptorSetInfo.DescriptorSets[j], Bindings[i], VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, DescriptorCounts[i], NULL, DescriptorBufferInfos);
			}
			if (DescriptorTypes[i] == 2)
			{
				//image
				if (DescriptorImageInfos != NULL) OpenVkFree(DescriptorImageInfos);
				DescriptorImageInfos = (VkDescriptorImageInfo*)malloc(DescriptorCounts[i] * sizeof(VkDescriptorImageInfo));

				for (uint32_t m = 0; m < DescriptorCounts[i]; m++)
				{
					VkSampler* ImageSampler = (VkSampler*)CMA_GetAt(&VkRenderer.Sampler, Sampler[m]);
					if (ImageSampler != NULL)
						DescriptorImageInfos[m].sampler = *ImageSampler;
					else
						return OpenVkRuntimeError("Failed to find sampler");

					if (ImageTypes[m] == 0)
					{
						VkTextureImageInfo* TextureImage = (VkTextureImageInfo*)CMA_GetAt(&VkRenderer.TextureImages, Images[m]);
						if (TextureImage != NULL)
							DescriptorImageInfos[m].imageView = TextureImage->TextureImageView;
						else
						{
							OpenVkFree(DescriptorWrites);
							if (DescriptorBufferInfos != NULL) OpenVkFree(DescriptorBufferInfos);
							if (DescriptorImageInfos != NULL) OpenVkFree(DescriptorImageInfos);

							return OpenVkRuntimeError("Failed to Find Texture Image View this can be ok");
						}							
					}
					if (ImageTypes[m] == 1) DescriptorImageInfos[m].imageView = VkRenderer.ImageViews[Images[m]];//[j]?
					if (ImageLayouts[m] == 0) DescriptorImageInfos[m].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
					if (ImageLayouts[m] == 1) DescriptorImageInfos[m].imageLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;
				}

				DescriptorWrites[i] = VkDescriptorSetWrite(DescriptorSetInfo.DescriptorSets[j], Bindings[i], VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, DescriptorCounts[i], DescriptorImageInfos, NULL);
			}
		}

		vkUpdateDescriptorSets(VkRenderer.Device, DescriptorWriteCount, DescriptorWrites, 0, NULL);
	}

	OpenVkFree(DescriptorWrites);
	if (DescriptorBufferInfos != NULL) OpenVkFree(DescriptorBufferInfos);
	if (DescriptorImageInfos != NULL) OpenVkFree(DescriptorImageInfos);

	return CMA_Push(&VkRenderer.DescriptorSets, &DescriptorSetInfo);
}

void VkUpdateDescriptorSet(uint32_t DescriptorSetLayout, OpenVkBool DynamicDescriptorPool, uint32_t DescriptorPool, uint32_t DescriptorCount, uint32_t* DescriptorTypes,
						   uint32_t* UniformBuffers, uint64_t* UniformBufferSizes, uint32_t* Sampler, uint32_t* ImageTypes, 
						   uint32_t* Images, uint32_t* ImageLayouts, uint32_t* Bindings, uint32_t DescriptorSet)
{
	/*
	VkWriteDescriptorSet* DescriptorWrites = (VkWriteDescriptorSet*)OpenVkMalloc(DescriptorCount * sizeof(VkWriteDescriptorSet));

	VkDescriptorBufferInfo BufferInfo;
	VkDescriptorImageInfo ImageInfo;

	for (uint32_t j = 0; j < MAX_FRAMES_IN_FLIGHT; j++)
	{
		uint32_t UniformCount = 0;
		uint32_t ImageCount = 0;

	//	for (uint32_t i = 0; i < DescriptorCount; i++)
		{
			if (DescriptorTypes[i] == 0)
			{
				BufferInfo.buffer = VkRenderer.UniformBuffers[UniformBuffers[UniformCount]].Buffers[j];
				BufferInfo.offset = 0;
				BufferInfo.range = UniformBufferSizes[UniformCount];

				DescriptorWrites[i] = VkDescriptorSetWrite(VkRenderer.DescriptorSets[DescriptorSet].DescriptorSets[j], Bindings[i], VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, NULL, &BufferInfo);
				UniformCount++;
			}
			if (DescriptorTypes[i] == 1)
			{
				BufferInfo.buffer = VkRenderer.UniformBuffers[UniformBuffers[UniformCount]].Buffers[j];
				BufferInfo.offset = 0;
				BufferInfo.range = UniformBufferSizes[UniformCount];

				DescriptorWrites[i] = VkDescriptorSetWrite(VkRenderer.DescriptorSets[DescriptorSet].DescriptorSets[j], Bindings[i], VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, NULL, &BufferInfo);
				UniformCount++;
			}
			if (DescriptorTypes[i] == 2)
			{
				ImageInfo.sampler = VkRenderer.Sampler[Sampler[ImageCount]];
				if (ImageTypes[ImageCount] == 0) ImageInfo.imageView = VkRenderer.TextureImageViews[Images[ImageCount]];
				if (ImageTypes[ImageCount] == 1) ImageInfo.imageView = VkRenderer.Images[Images[ImageCount]].ImageViews[0];//[j]?
				if (ImageLayouts[ImageCount] == 0) ImageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
				if (ImageLayouts[ImageCount] == 1) ImageInfo.imageLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;

				DescriptorWrites[i] = VkDescriptorSetWrite(VkRenderer.DescriptorSets[DescriptorSet].DescriptorSets[j], Bindings[i], VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, &ImageInfo, NULL);
				ImageCount++;
			}
		}

		vkUpdateDescriptorSets(VkRenderer.Device, DescriptorCount, DescriptorWrites, 0, NULL);
	}	

	OpenVkFree(DescriptorWrites);
	*/
}

OpenVkBool VkDrawFrame(void (*RenderFunc)(void), void (*ResizeFunc)(void), void (*UpdateFunc)(void))
{
	vkWaitForFences(VkRenderer.Device, 1, &VkRenderer.InFlightFences[VkRenderer.CurrentFrame], VK_TRUE, UINT64_MAX);

	vkAcquireNextImageKHR(VkRenderer.Device, VkRenderer.SwapChain, UINT64_MAX, VkRenderer.ImageAvailableSemaphores[VkRenderer.CurrentFrame], VK_NULL_HANDLE, &VkRenderer.ImageIndex);

	if (VkRenderer.InFlightImages[VkRenderer.ImageIndex] != VK_NULL_HANDLE)
		vkWaitForFences(VkRenderer.Device, 1, &VkRenderer.InFlightImages[VkRenderer.ImageIndex], VK_TRUE, UINT64_MAX);

	VkRenderer.InFlightImages[VkRenderer.ImageIndex] = VkRenderer.InFlightFences[VkRenderer.CurrentFrame];

	VkSemaphore WaitSemaphores[] = { VkRenderer.ImageAvailableSemaphores[VkRenderer.CurrentFrame] };
	VkPipelineStageFlags WaitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
	VkSemaphore SignalSemaphores[] = { VkRenderer.RenderFinishedSemaphores[VkRenderer.CurrentFrame] };

	//Drawing Func
	if (UpdateFunc != NULL)
		UpdateFunc();
	if (RenderFunc != NULL)
		RenderFunc();

	VkSubmitInfo SubmitInfo;
	SubmitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	SubmitInfo.pNext = NULL;
	SubmitInfo.waitSemaphoreCount = 1;
	SubmitInfo.pWaitSemaphores = WaitSemaphores;
	SubmitInfo.pWaitDstStageMask = WaitStages;
	SubmitInfo.commandBufferCount = 1;
	SubmitInfo.pCommandBuffers = &VkRenderer.CommandBuffers[VkRenderer.ImageIndex];
	SubmitInfo.signalSemaphoreCount = 1;
	SubmitInfo.pSignalSemaphores = SignalSemaphores;

	vkResetFences(VkRenderer.Device, 1, &VkRenderer.InFlightFences[VkRenderer.CurrentFrame]);

	VkResult Success = vkQueueSubmit(VkRenderer.GraphicsQueue, 1, &SubmitInfo, VkRenderer.InFlightFences[VkRenderer.CurrentFrame]);

	if (Success == VK_ERROR_OUT_OF_DATE_KHR)
		if (ResizeFunc != NULL)
			ResizeFunc();
	else if (Success != VK_SUCCESS)
		return OpenVkRuntimeError("Failed to Submit Draw Command Buffer");

	VkSwapchainKHR SwapChains[] = { VkRenderer.SwapChain };

	VkPresentInfoKHR PresentInfo;
	PresentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	PresentInfo.pNext = NULL;
	PresentInfo.waitSemaphoreCount = 1;
	PresentInfo.pWaitSemaphores = SignalSemaphores;
	PresentInfo.swapchainCount = 1;
	PresentInfo.pSwapchains = SwapChains;
	PresentInfo.pImageIndices = &VkRenderer.ImageIndex;
	PresentInfo.pResults = NULL;

	Success = vkQueuePresentKHR(VkRenderer.PresentQueue, &PresentInfo);

	if (Success == VK_ERROR_OUT_OF_DATE_KHR || Success == VK_SUBOPTIMAL_KHR)
		if (ResizeFunc != NULL)
			ResizeFunc();
	else if (Success != VK_SUCCESS)
		return OpenVkRuntimeError("Failed to submit Draw Command Buffer");

	VkRenderer.CurrentFrame = (VkRenderer.CurrentFrame + 1) % MAX_FRAMES_IN_FLIGHT;

	return 1;
}

OpenVkBool VkBeginFrame()
{
	VkCommandBufferBeginInfo BeginInfo;
	BeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	BeginInfo.pNext = NULL;
	BeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
	BeginInfo.pInheritanceInfo = NULL;

	if (vkBeginCommandBuffer(VkRenderer.CommandBuffers[VkRenderer.ImageIndex], &BeginInfo) != VK_SUCCESS)
		return OpenVkRuntimeError("Failed to Begin Command Buffer");

	return 1;
}

OpenVkBool VkEndFrame()
{
	if (vkEndCommandBuffer(VkRenderer.CommandBuffers[VkRenderer.ImageIndex]) != VK_SUCCESS)
		return OpenVkRuntimeError("Failed to End Command Buffer");

	return 1;
}

void VkBeginRenderPass(float r, float g, float b, float a, uint32_t ClearColors, OpenVkBool ClearDepth, uint32_t RenderPass, uint32_t Framebuffer, uint32_t x, uint32_t y, uint32_t Width, uint32_t Height)
{
	VkClearValue ClearValues[8];
	for (uint32_t i = 0; i < ClearColors; i++)
	{
		VkClearValue ClearValue = { r, g, b, a };

		ClearValues[i] = ClearValue;
	}
	
	if (ClearDepth)
	{
		VkClearValue ClearValue = { 1.0, 0.0 };
		ClearValues[ClearColors] = ClearValue;
	}

	VkRenderPassBeginInfo RenderPassBeginInfo;
	RenderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	RenderPassBeginInfo.pNext = NULL;
	RenderPassBeginInfo.renderPass = VkRenderer.RenderPasses[RenderPass];
	RenderPassBeginInfo.framebuffer = VkRenderer.Framebuffers[Framebuffer].Framebuffers[VkRenderer.ImageIndex];
	RenderPassBeginInfo.renderArea.offset.x = x;
	RenderPassBeginInfo.renderArea.offset.y = y;
	RenderPassBeginInfo.renderArea.extent.width = Width;
	RenderPassBeginInfo.renderArea.extent.height = Height;
	RenderPassBeginInfo.clearValueCount = (ClearColors + ClearDepth);
	RenderPassBeginInfo.pClearValues = ClearValues;

	vkCmdBeginRenderPass(VkRenderer.CommandBuffers[VkRenderer.ImageIndex], &RenderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
}

void VkEndRenderPass()
{
	vkCmdEndRenderPass(VkRenderer.CommandBuffers[VkRenderer.ImageIndex]);
}

uint32_t VkCreateTextureImage(const char* Path, OpenVkBool FlipVertical)
{
//	VkRenderer.TextureImageMemories = (VkDeviceMemory*)OpenVkRealloc(VkRenderer.TextureImageMemories, (VkRenderer.TextureImageCount + 1) * sizeof(VkDeviceMemory));
//	VkRenderer.TextureImages = (VkImage*)OpenVkRealloc(VkRenderer.TextureImages, (VkRenderer.TextureImageCount + 1) * sizeof(VkImage));
//	VkRenderer.TextureImageViews = (VkImageView*)OpenVkRealloc(VkRenderer.TextureImageViews, (VkRenderer.TextureImageCount + 1) * sizeof(VkImageView));

	VkTextureImageInfo TextureImage;

	int32_t TextureWidth;
	int32_t TextureHeight;
	int32_t TextureChannels;

	stbi_set_flip_vertically_on_load(FlipVertical);
	unsigned char* Pixel = stbi_load(Path, &TextureWidth, &TextureHeight, &TextureChannels, STBI_rgb_alpha);
	VkRenderer.MipLevels = floorf(log2f(MAX(TextureWidth, TextureHeight))) + 1;

	VkDeviceSize ImageSize = TextureWidth * TextureHeight * 4;
	if (!Pixel)
		return OpenVkRuntimeError("Failed to Load Texture");

	VkBuffer StagingBuffer;
	VkDeviceMemory StagingBufferMemory;

	if (VkCreateBuffer(ImageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &StagingBuffer, &StagingBufferMemory) == OPENVK_ERROR)
		return OpenVkRuntimeError("Failed to create Texture Buffer");

	void* Data;
	vkMapMemory(VkRenderer.Device, StagingBufferMemory, 0, ImageSize, 0, &Data);
	memcpy(Data, Pixel, ImageSize);
	vkUnmapMemory(VkRenderer.Device, StagingBufferMemory);

	OpenVkFree(Pixel);

	if (VkCreateImage(TextureWidth, TextureHeight, VkRenderer.MipLevels, VK_SAMPLE_COUNT_1_BIT, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &TextureImage.TextureImage, &TextureImage.TextureImageMemory) == OPENVK_ERROR)
		return OpenVkRuntimeError("Failed to create Texture Image");

	VkTransitionImageLayout(TextureImage.TextureImage, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VkRenderer.MipLevels);

	VkCopyBufferToImage(StagingBuffer, TextureImage.TextureImage, TextureWidth, TextureHeight);
	//	if (TransitionImageLayout(TextureImages, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, MipLevels) != VK_SUCCESS)
	//		return 1;
	VkGenerateMipmaps(TextureImage.TextureImage, VK_FORMAT_R8G8B8A8_UNORM, TextureWidth, TextureHeight, VkRenderer.MipLevels);

	vkDestroyBuffer(VkRenderer.Device, StagingBuffer, NULL);
	vkFreeMemory(VkRenderer.Device, StagingBufferMemory, NULL);

	TextureImage.TextureImageView = VkCreateImageView(TextureImage.TextureImage, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_ASPECT_COLOR_BIT, VkRenderer.MipLevels);

	return CMA_Push(&VkRenderer.TextureImages, &TextureImage);
}

void VkDestroyTextureImage(uint32_t TextureImage)
{
	vkDeviceWaitIdle(VkRenderer.Device);

	VkTextureImageInfo* Image = (VkTextureImageInfo*)CMA_GetAt(&VkRenderer.TextureImages, TextureImage);
	if (Image != NULL)
	{
		vkDestroyImageView(VkRenderer.Device, Image->TextureImageView, NULL);

		vkDestroyImage(VkRenderer.Device, Image->TextureImage, NULL);
		vkFreeMemory(VkRenderer.Device, Image->TextureImageMemory, NULL);

		CMA_Pop(&VkRenderer.TextureImages, TextureImage);
	}	
}

//Filter
//VK_FILTER_NEAREST = 0,
//VK_FILTER_LINEAR = 1,
//Address Modes
//VK_SAMPLER_ADDRESS_MODE_REPEAT = 0,
//VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT = 1,
//VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE = 2,
//VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER = 3,
//VK_SAMPLER_ADDRESS_MODE_MIRROR_CLAMP_TO_EDGE = 4,
uint32_t VkCreateImageSampler(uint32_t Filter, uint32_t AddressMode)
{
//	VkRenderer.Sampler = (VkSampler*)OpenVkRealloc(VkRenderer.Sampler, (VkRenderer.SamplerCount + 1) * sizeof(VkSampler));

	VkSamplerCreateInfo SamplerInfo;
	SamplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	SamplerInfo.pNext = NULL;
	SamplerInfo.flags = 0;
	SamplerInfo.magFilter = (VkFilter)Filter;
	SamplerInfo.minFilter = (VkFilter)Filter;
	SamplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
	SamplerInfo.addressModeU = (VkSamplerAddressMode)AddressMode;
	SamplerInfo.addressModeV = (VkSamplerAddressMode)AddressMode;
	SamplerInfo.addressModeW = (VkSamplerAddressMode)AddressMode;
	SamplerInfo.mipLodBias = 0.0;
	SamplerInfo.anisotropyEnable = VK_TRUE;
	SamplerInfo.maxAnisotropy = 16;
	SamplerInfo.compareEnable = VK_FALSE;
	SamplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
	SamplerInfo.minLod = 0.0;
	SamplerInfo.maxLod = VkRenderer.MipLevels;
	SamplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
	SamplerInfo.unnormalizedCoordinates = VK_FALSE;

	VkSampler Sampler;
	if (vkCreateSampler(VkRenderer.Device, &SamplerInfo, NULL, &Sampler) != VK_SUCCESS)
		return OpenVkRuntimeError("Failed to Create Sampler");

//	VkRenderer.SamplerCount++;

//	return VkRenderer.SamplerCount - 1;
	return CMA_Push(&VkRenderer.Sampler, &Sampler);
}

void VkDestroySampler(uint32_t Sampler)
{
	vkDeviceWaitIdle(VkRenderer.Device);

	VkSampler* ImageSampler = (VkSampler*)CMA_GetAt(&VkRenderer.Sampler, Sampler);
	if (ImageSampler != NULL)
	{
		vkDestroySampler(VkRenderer.Device, *ImageSampler, NULL);
		CMA_Pop(&VkRenderer.Sampler, Sampler);
	}
}

uint32_t VkCreateVertexBuffer(size_t Size, const void* Vertices)
{
	VkBufferInfo BufferInfo;

	VkBuffer StagingBuffer;
	VkDeviceMemory StagingBufferMemory;
	if (VkCreateBuffer(Size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &StagingBuffer, &StagingBufferMemory) == OPENVK_ERROR)
		return OpenVkRuntimeError("Failed to Create Vertex Buffer: Func 0");

	void* Data;
	vkMapMemory(VkRenderer.Device, StagingBufferMemory, 0, Size, 0, &Data);
	memcpy(Data, Vertices, Size);
	vkUnmapMemory(VkRenderer.Device, StagingBufferMemory);

	if (VkCreateBuffer(Size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &BufferInfo.Buffer, &BufferInfo.BufferMemory) == OPENVK_ERROR)
		return OpenVkRuntimeError("Failed to Create Vertex Buffer: Func 1");

	VkCopyBuffer(StagingBuffer, BufferInfo.Buffer, Size);

	vkDestroyBuffer(VkRenderer.Device, StagingBuffer, NULL);
	vkFreeMemory(VkRenderer.Device, StagingBufferMemory, NULL);

	return CMA_Push(&VkRenderer.Buffers, &BufferInfo);
}

uint32_t VkCreateIndexBuffer(size_t Size, const void* Indices)
{
	VkBufferInfo BufferInfo;
	
	VkBuffer StagingBuffer;
	VkDeviceMemory StagingBufferMemory;
	if (VkCreateBuffer(Size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &StagingBuffer, &StagingBufferMemory) == OPENVK_ERROR)
		return OpenVkRuntimeError("Failed to Create Index Buffer 0");

	void* Data;
	vkMapMemory(VkRenderer.Device, StagingBufferMemory, 0, Size, 0, &Data);
	memcpy(Data, Indices, Size);
	vkUnmapMemory(VkRenderer.Device, StagingBufferMemory);

	if (VkCreateBuffer(Size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &BufferInfo.Buffer, &BufferInfo.BufferMemory) == OPENVK_ERROR)
		return OpenVkRuntimeError("Failed to Create Index Buffer 1");

	VkCopyBuffer(StagingBuffer, BufferInfo.Buffer, Size);

	vkDestroyBuffer(VkRenderer.Device, StagingBuffer, NULL);
	vkFreeMemory(VkRenderer.Device, StagingBufferMemory, NULL);

	return CMA_Push(&VkRenderer.Buffers, &BufferInfo);
}

void VkDestroyBuffer(uint32_t Buffer)
{
	vkDeviceWaitIdle(VkRenderer.Device);

	VkBufferInfo* BufferInfo = (VkBufferInfo*)CMA_GetAt(&VkRenderer.Buffers, Buffer);
	if (BufferInfo != NULL)
	{
		vkDestroyBuffer(VkRenderer.Device, BufferInfo->Buffer, NULL);
		vkFreeMemory(VkRenderer.Device, BufferInfo->BufferMemory, NULL);
		CMA_Pop(&VkRenderer.Buffers, Buffer);
	}
}


uint32_t VkCreateUniformBuffer(size_t Size)
{
	VkRenderer.UniformBuffers = (VkUniformBufferInfo*)OpenVkRealloc(VkRenderer.UniformBuffers, (VkRenderer.UniformBufferCount + 1) * sizeof(VkUniformBufferInfo));

	for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
		if (VkCreateBuffer(Size, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &VkRenderer.UniformBuffers[VkRenderer.UniformBufferCount].Buffers[i], &VkRenderer.UniformBuffers[VkRenderer.UniformBufferCount].BufferMemories[i]) == OPENVK_ERROR)
			return OpenVkRuntimeError("Failed To Create Uniform Buffer");

	VkRenderer.UniformBufferCount++;

	return VkRenderer.UniformBufferCount - 1;
}

uint32_t VkCreateDynamicUniformBuffer(size_t Size)
{
	//FIX make it working? and add MAX_FRAMES_IN_FLIGHT
/*
	VkRenderer.Buffers = (VkBuffer*)OpenVkRealloc(VkRenderer.Buffers, (VkRenderer.BufferCount + 1) * sizeof(VkBuffer));
	VkRenderer.BufferMemories = (VkDeviceMemory*)OpenVkRealloc(VkRenderer.BufferMemories, (VkRenderer.BufferCount + 1) * sizeof(VkDeviceMemory));

	if (VkCreateBuffer(Size, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, &VkRenderer.Buffers[VkRenderer.BufferCount], &VkRenderer.BufferMemories[VkRenderer.BufferCount]) == OPENVK_ERROR)
		return OpenVkRuntimeError("Failed To Create Dynamic Uniform Buffer");

	VkRenderer.BufferCount++;

	return VkRenderer.BufferCount - 1;
*/
	return 0;
}

void VkUpdateUniformBuffer(size_t Size, const void* UBO, uint32_t UniformBuffer)
{
	void* Data;
	vkMapMemory(VkRenderer.Device, VkRenderer.UniformBuffers[UniformBuffer].BufferMemories[VkRenderer.CurrentFrame], 0, Size, 0, &Data);
	memcpy(Data, UBO, Size);
	vkUnmapMemory(VkRenderer.Device, VkRenderer.UniformBuffers[UniformBuffer].BufferMemories[VkRenderer.CurrentFrame]);
}

void VkUpdateDynamicUniformBuffer(size_t Size, const void* UBO, uint32_t UniformBuffer)
{
	//FIX doesn't work I guess?
	/*
	void* Data;
	vkMapMemory(VkRenderer.Device, VkRenderer.BufferMemories[UniformBuffer], 0, Size, 0, &Data);
	memcpy(Data, UBO, Size);
	vkUnmapMemory(VkRenderer.Device, VkRenderer.BufferMemories[UniformBuffer]);

	VkMappedMemoryRange MemoryRange;
	MemoryRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
	MemoryRange.pNext = NULL;
	MemoryRange.memory = VkRenderer.BufferMemories[UniformBuffer];
	MemoryRange.offset = 0;
	MemoryRange.size = Size;
	vkFlushMappedMemoryRanges(VkRenderer.Device, 1, &MemoryRange);
	*/
}

void VkBindGraphicsPipeline(uint32_t Pipeline)
{
	vkCmdBindPipeline(VkRenderer.CommandBuffers[VkRenderer.ImageIndex], VK_PIPELINE_BIND_POINT_GRAPHICS, VkRenderer.Pipelines[Pipeline]);
}

void VkSetViewport(float x, float y, float Width, float Height)
{
	VkViewport Viewport;
	Viewport.x = x;
	Viewport.y = y;
	Viewport.width = Width;
	Viewport.height = Height;
	Viewport.minDepth = 0.0;
	Viewport.maxDepth = 1.0;

	vkCmdSetViewport(VkRenderer.CommandBuffers[VkRenderer.ImageIndex], 0, 1, &Viewport);
}

void VkSetScissor(int32_t x, int32_t y, uint32_t Width, uint32_t Height)
{
	VkOffset2D Offset;
	Offset.x = x;
	Offset.y = y;

	VkRect2D Scissor;
	Scissor.offset = Offset;
	Scissor.extent.width = Width;
	Scissor.extent.height = Height;

	vkCmdSetScissor(VkRenderer.CommandBuffers[VkRenderer.ImageIndex], 0, 1, &Scissor);
}

void VkBindVertexBuffer(uint32_t VertexBuffer)
{
	VkDeviceSize Offsets[1] = { 0 };
	VkBufferInfo* BufferInfo = (VkBufferInfo*)CMA_GetAt(&VkRenderer.Buffers, VertexBuffer);
	if (BufferInfo != NULL)
		vkCmdBindVertexBuffers(VkRenderer.CommandBuffers[VkRenderer.ImageIndex], 0, 1, &BufferInfo->Buffer, Offsets);
}

void VkBindIndexBuffer(uint32_t VertexBuffer, uint32_t IndexBuffer)
{
	VkDeviceSize Offsets[1] = { 0 };
	VkBufferInfo* VertexBufferInfo = (VkBufferInfo*)CMA_GetAt(&VkRenderer.Buffers, VertexBuffer);
	VkBufferInfo* IndexBufferInfo = (VkBufferInfo*)CMA_GetAt(&VkRenderer.Buffers, IndexBuffer);
	if (VertexBufferInfo != NULL)
		vkCmdBindVertexBuffers(VkRenderer.CommandBuffers[VkRenderer.ImageIndex], 0, 1, &VertexBufferInfo->Buffer, Offsets);
	if (IndexBufferInfo != NULL)
		vkCmdBindIndexBuffer(VkRenderer.CommandBuffers[VkRenderer.ImageIndex], IndexBufferInfo->Buffer, 0, VK_INDEX_TYPE_UINT32);
}

void VkDrawVertices(uint32_t VertexCount)
{
	vkCmdDraw(VkRenderer.CommandBuffers[VkRenderer.ImageIndex], VertexCount, 1, 0, 0);
}

void VkDrawIndices(uint32_t IndexCount)
{
	vkCmdDrawIndexed(VkRenderer.CommandBuffers[VkRenderer.ImageIndex], IndexCount, 1, 0, 0, 0);
}

void VkBindDescriptorSet(uint32_t Pipeline, uint32_t Set, uint32_t DescriptorSet)
{
	VkDescriptorSetInfo* DescriptorSetInfo = (VkDescriptorSetInfo*)CMA_GetAt(&VkRenderer.DescriptorSets, DescriptorSet);
	if (DescriptorSetInfo != NULL) vkCmdBindDescriptorSets(VkRenderer.CommandBuffers[VkRenderer.ImageIndex], VK_PIPELINE_BIND_POINT_GRAPHICS, VkRenderer.PipelineLayouts[Pipeline], Set, 1, &DescriptorSetInfo->DescriptorSets[VkRenderer.CurrentFrame], 0, NULL);
}

//Shader Types
//0 = Vertex Shader
//1 = Fragment Shader
void VkPushConstant(uint32_t Pipeline, uint32_t ShaderType, uint32_t Offset, size_t Size, const void* Data)
{
	vkCmdPushConstants(VkRenderer.CommandBuffers[VkRenderer.ImageIndex], VkRenderer.PipelineLayouts[Pipeline], (ShaderType == 0 ? VK_SHADER_STAGE_VERTEX_BIT : VK_SHADER_STAGE_FRAGMENT_BIT), Offset, Size, Data);
}

void VkCleanupSwapChain()
{
	vkDeviceWaitIdle(VkRenderer.Device);

	for (uint32_t i = 0; i < VkRenderer.SwapChainImageCount; i++)
	{
	//	vkDestroyImage(VkRenderer.Device, VkRenderer.Images[i].Images[j], NULL);
		vkDestroyImageView(VkRenderer.Device, VkRenderer.SwapChainImageViews[i], NULL);
	}

	for (uint32_t i = 1; i < VkRenderer.ImageCount; i++)
	{
		vkFreeMemory(VkRenderer.Device, VkRenderer.ImageMemories[i], NULL);
		vkDestroyImage(VkRenderer.Device, VkRenderer.Images[i], NULL);
		vkDestroyImageView(VkRenderer.Device, VkRenderer.ImageViews[i], NULL);
	}		
	
		//	for (uint32_t i = 0; i < VkRenderer.ImageCount; i++)
		//	{
		//		if (i != 0)
		//		OpenVkFree(VkRenderer.Images[i].ImageMemories);
		//		OpenVkFree(VkRenderer.Images[i].Images);
		//		OpenVkFree(VkRenderer.Images[i].ImageViews);
		//	}

//	if (VkRenderer.ImageCount > 0)
//	OpenVkFree(VkRenderer.Images);

	for (uint32_t i = 0; i < VkRenderer.FramebufferCount; i++)
		for (uint32_t j = 0; j < VkRenderer.SwapChainImageCount; j++)
			vkDestroyFramebuffer(VkRenderer.Device, VkRenderer.Framebuffers[i].Framebuffers[j], NULL);

	for (uint32_t i = 0; i < VkRenderer.FramebufferCount; i++)
		OpenVkFree(VkRenderer.Framebuffers[i].Framebuffers);

	for (uint32_t i = 0; i < VkRenderer.RenderPassCount; i++)
		vkDestroyRenderPass(VkRenderer.Device, VkRenderer.RenderPasses[i], NULL);

	for (uint32_t i = 0; i < VkRenderer.StaticDescriptorPools.DescriptorPoolCount; i++)
		vkDestroyDescriptorPool(VkRenderer.Device, VkRenderer.StaticDescriptorPools.DescriptorPools[i], NULL);

	vkFreeCommandBuffers(VkRenderer.Device, VkRenderer.CommandPool, VkRenderer.SwapChainImageCount, VkRenderer.CommandBuffers);
	OpenVkFree(VkRenderer.CommandBuffers);

	vkDestroySwapchainKHR(VkRenderer.Device, VkRenderer.SwapChain, NULL);
}

void VkRecreateSwapChain(uint32_t* Width, uint32_t* Height)
{
	VkCleanupSwapChain();

	VkRenderer.SwapChainImageCount = 0;
	VkRenderer.ImageCount = 1;
	VkRenderer.RenderPassCount = 0;
	VkRenderer.FramebufferCount = 0;
//	VkRenderer.DescriptorSetCount = 0;
	VkRenderer.StaticDescriptorPools.DescriptorPoolCount = 0;

	CMA_Destroy(&VkRenderer.DescriptorSets);
	VkRenderer.DescriptorSets = CMA_Create(sizeof(VkDescriptorSetInfo));

	VkCreateSwapChain(Width, Height);
	VkCreateCommandBuffers();
	VkRenderer.InFlightImages = (VkFence*)OpenVkRealloc(VkRenderer.InFlightImages, VkRenderer.SwapChainImageCount * sizeof(VkFence));
}

void VkDestroyRenderer()
{
	vkDeviceWaitIdle(VkRenderer.Device);

	for (uint32_t i = 0; i < VkRenderer.PipelineCount; i++)
	{
		vkDestroyPipelineLayout(VkRenderer.Device, VkRenderer.PipelineLayouts[i], NULL);
		vkDestroyPipeline(VkRenderer.Device, VkRenderer.Pipelines[i], NULL);
	}

	VkCleanupSwapChain();

	for (uint32_t i = 0; i < VkRenderer.DynamicDescriptorPools.DescriptorPoolCount; i++)
		vkDestroyDescriptorPool(VkRenderer.Device, VkRenderer.DynamicDescriptorPools.DescriptorPools[i], NULL);

	for (uint32_t i = 0; i < VkRenderer.UniformBufferCount; i++)
	{
		for (uint32_t j = 0; j < MAX_FRAMES_IN_FLIGHT; j++)
		{
			vkDestroyBuffer(VkRenderer.Device, VkRenderer.UniformBuffers[i].Buffers[j], NULL);
			vkFreeMemory(VkRenderer.Device, VkRenderer.UniformBuffers[i].BufferMemories[j], NULL);
		}
	}
	
	if (VkRenderer.UniformBufferCount > 0)
		OpenVkFree(VkRenderer.UniformBuffers);

	for (uint32_t i = 0; i < VkRenderer.Sampler.Size; i++)
	{
		VkSampler* Sampler = (VkSampler*)CMA_GetAt(&VkRenderer.Sampler, i);
		if (Sampler != NULL)
			vkDestroySampler(VkRenderer.Device, *Sampler, NULL);
	}
		

//	if (VkRenderer.SamplerCount > 0)
//		OpenVkFree(VkRenderer.Sampler);
	CMA_Destroy(&VkRenderer.Sampler);

	for (uint32_t i = 0; i < VkRenderer.TextureImages.Size; i++)
	{
		VkTextureImageInfo* TextureImage = (VkTextureImageInfo*)CMA_GetAt(&VkRenderer.TextureImages, i);
		if (TextureImage != NULL)
		{
			vkDestroyImageView(VkRenderer.Device, TextureImage->TextureImageView, NULL);
			vkDestroyImage(VkRenderer.Device, TextureImage->TextureImage, NULL);
			vkFreeMemory(VkRenderer.Device, TextureImage->TextureImageMemory, NULL);
		}		
	}

//	OpenVkFree(VkRenderer.TextureImageViews);
//	OpenVkFree(VkRenderer.TextureImages);
//	OpenVkFree(VkRenderer.TextureImageMemories);

	for (uint32_t i = 0; i < VkRenderer.DescriptorSetLayoutCount; i++)
		vkDestroyDescriptorSetLayout(VkRenderer.Device, VkRenderer.DescriptorSetLayouts[i], NULL);

//	OpenVkFree(VkRenderer.DescriptorSetLayouts);

	for (uint32_t i = 0; i < VkRenderer.Buffers.Size; i++)
	{
		VkBufferInfo* BufferInfo = (VkBufferInfo*)CMA_GetAt(&VkRenderer.Buffers, i);
		if (BufferInfo != NULL)
		{
			vkDestroyBuffer(VkRenderer.Device, BufferInfo->Buffer, NULL);
			vkFreeMemory(VkRenderer.Device, BufferInfo->BufferMemory, NULL);
		}
	}

//	OpenVkFree(VkRenderer.Buffers);
//	OpenVkFree(VkRenderer.BufferMemories);

	for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
	{
		vkDestroySemaphore(VkRenderer.Device, VkRenderer.RenderFinishedSemaphores[i], NULL);
		vkDestroySemaphore(VkRenderer.Device, VkRenderer.ImageAvailableSemaphores[i], NULL);
		vkDestroyFence(VkRenderer.Device, VkRenderer.InFlightFences[i], NULL);
	}
	
	OpenVkFree(VkRenderer.InFlightImages);

	vkDestroyCommandPool(VkRenderer.Device, VkRenderer.CommandPool, NULL);

	vkDestroyDevice(VkRenderer.Device, NULL);
	vkDestroySurfaceKHR(VkRenderer.Instance, VkRenderer.Surface, NULL);
	vkDestroyInstance(VkRenderer.Instance, NULL);

	CMA_Destroy(&VkRenderer.DescriptorSets);
	CMA_Destroy(&VkRenderer.Buffers);
	CMA_Destroy(&VkRenderer.TextureImages);

//	OpenVkFree(VkRenderer.Pipelines);
//	OpenVkFree(VkRenderer.PipelineLayouts);
	
//	OpenVkFree(VkRenderer.RenderPasses);
	//	OpenVkFree(VkRenderer.DescriptorSets);
//	OpenVkFree(VkRenderer.DescriptorSetLayouts);
//	
//	OpenVkFree(VkRenderer.DynamicDescriptorPools.DescriptorPools);
//	OpenVkFree(VkRenderer.StaticDescriptorPools.DescriptorPools);

	OpenVkRuntimeInfo("Successfuly destroyed Renderer", "");
}