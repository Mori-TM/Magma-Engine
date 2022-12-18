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

	if (SwapChainSupport.Capabilities.supportedUsageFlags & VK_IMAGE_USAGE_TRANSFER_SRC_BIT)
		SwapchainCreateInfo.imageUsage |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
	if (SwapChainSupport.Capabilities.supportedUsageFlags & VK_IMAGE_USAGE_TRANSFER_DST_BIT)
		SwapchainCreateInfo.imageUsage |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;

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

	VkCompositeAlphaFlagBitsKHR CompositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	VkCompositeAlphaFlagBitsKHR CompositeAlphaFlags[] = 
	{
		VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
		VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR,
		VK_COMPOSITE_ALPHA_POST_MULTIPLIED_BIT_KHR,
		VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR,
	};

	for (uint32_t i = 0; i < 4; i++)
	{
		if (SwapChainSupport.Capabilities.supportedCompositeAlpha & CompositeAlphaFlags[i])
		{
			CompositeAlpha = CompositeAlphaFlags[i];
			break;
		}			
	}

	SwapchainCreateInfo.preTransform = SwapChainSupport.Capabilities.currentTransform;
	SwapchainCreateInfo.compositeAlpha = CompositeAlpha;
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

uint32_t VkCreateRenderer(const char**(*GetExtensions)(uint32_t* ExtensionCount), VkSurfaceKHR(*GetSurface)(VkInstance* Instance), void (*GetWindowSize)(uint32_t* Width, uint32_t* Height))
{
	const char* ValidationLayers[] = { "VK_LAYER_KHRONOS_validation" };
//	const char* DeviceExtensions[] = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
	
	memset(&VkRenderer, 0, sizeof(VkRendererInfo));
	VkRenderer.ImageAttachments = CMA_Create(sizeof(VkImageInfo));
	VkRenderer.DescriptorSets = CMA_Create(sizeof(VkDescriptorSetInfo));
	VkRenderer.Images = CMA_Create(sizeof(VkImageInfo));
	VkRenderer.Sampler = CMA_Create(sizeof(VkSampler));
	VkRenderer.StaticBuffers = CMA_Create(sizeof(VkStaticBufferInfo));
	VkRenderer.DynamicBuffers = CMA_Create(sizeof(VkDynamicBufferInfo));
	VkRenderer.DescriptorPools = CMA_Create(sizeof(VkDescriptorPoolInfo));
	VkRenderer.BufferMemoryAlignment = 256;

	//Instance
	VkApplicationInfo AppInfo;
	AppInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	AppInfo.pNext = NULL;
	AppInfo.pApplicationName = "OpenVK Framework";
	AppInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	AppInfo.pEngineName = "OpenVK";
	AppInfo.engineVersion = VK_MAKE_VERSION(2, 0, 0);
	AppInfo.apiVersion = VK_MAKE_VERSION(1, 2, 0);

	uint32_t ExtensionCount;
	const char** Extensions = GetExtensions(&ExtensionCount);

	VkInstanceCreateInfo InstanceCreateInfo;
	InstanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	InstanceCreateInfo.pNext = NULL;
	InstanceCreateInfo.flags = 0;
	InstanceCreateInfo.pApplicationInfo = &AppInfo;
	InstanceCreateInfo.enabledLayerCount = (OpenVkRendererFlags & OPENVK_VALIDATION_LAYER ? 1 : 0);
	InstanceCreateInfo.ppEnabledLayerNames = ValidationLayers;
	InstanceCreateInfo.enabledExtensionCount = ExtensionCount;
	InstanceCreateInfo.ppEnabledExtensionNames = Extensions;

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

	VkPhysicalDeviceFeatures DeviceFeatures;
	memset(&DeviceFeatures, 0, sizeof(VkPhysicalDeviceFeatures));
	DeviceFeatures.samplerAnisotropy = VK_TRUE;
	DeviceFeatures.sampleRateShading = VK_TRUE;
	DeviceFeatures.fillModeNonSolid = VK_TRUE;
	DeviceFeatures.wideLines = VK_TRUE;
	DeviceFeatures.depthClamp = VK_TRUE;

	VkRenderer.DeviceExtensions[VkRenderer.DeviceExtensionCount] = VK_KHR_SWAPCHAIN_EXTENSION_NAME;
	VkRenderer.DeviceExtensionCount++;

	VkDeviceCreateInfo CreateInfo;
	CreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	CreateInfo.pNext = NULL;
	CreateInfo.flags = 0;
	CreateInfo.queueCreateInfoCount = 2;
	CreateInfo.pQueueCreateInfos = DeviceQueueCreateInfos;
	CreateInfo.pEnabledFeatures = &DeviceFeatures;
	CreateInfo.enabledLayerCount = (OpenVkRendererFlags & OPENVK_VALIDATION_LAYER ? 1 : 0);
	CreateInfo.ppEnabledLayerNames = ValidationLayers;
	CreateInfo.enabledExtensionCount = VkRenderer.DeviceExtensionCount;
	CreateInfo.ppEnabledExtensionNames = VkRenderer.DeviceExtensions;

	if (OpenVkRendererFlags & OPENVK_RAYTRACING)
		VkGetRaytracingFeatures(&CreateInfo);

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

	if (OpenVkRendererFlags & OPENVK_RAYTRACING)
		VkGetRaytracingInfos();

	//We push one dummy image attachment so when we get index 0 in create framebuffer/image copy we know the swap chain is ment
	VkImageInfo Dummy;
	memset(&Dummy, 1, sizeof(VkImageInfo));
	CMA_Push(&VkRenderer.Images, &Dummy);
	return CMA_Push(&VkRenderer.ImageAttachments, &Dummy);
}

uint32_t VkCreateColorImageAttachment(uint32_t Width, uint32_t Height, uint32_t MsaaSamples, OpenVkBool Sampled, uint32_t Format)
{
	VkFormat ColorFormat = VkGetOpenVkFormat(Format);

//	VkRenderer.Images = (VkImage*)OpenVkRealloc(VkRenderer.Images, (VkRenderer.ImageCount + 1) * sizeof(VkImage));
//	VkRenderer.ImageMemories = (VkDeviceMemory*)OpenVkRealloc(VkRenderer.ImageMemories, (VkRenderer.ImageCount + 1) * sizeof(VkDeviceMemory));
//	VkRenderer.ImageViews = (VkImageView*)OpenVkRealloc(VkRenderer.ImageViews, (VkRenderer.ImageCount + 1) * sizeof(VkImageView));

	VkSampleCountFlagBits Samples = (VkSampleCountFlagBits)(VkRenderer.MsaaSamples < MsaaSamples ? VkRenderer.MsaaSamples : MsaaSamples);
	VkImageInfo Image;
	Image.Format = ColorFormat;

	if (Sampled)
	{
		if (VkCreateImage(Width, Height, 1, Samples, ColorFormat, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &Image.Image, &Image.ImageMemory) == OPENVK_ERROR)
			return OpenVkRuntimeError("Failed to Create Sampled Attachment Image");
		Image.ImageView = VkCreateImageView(Image.Image, ColorFormat, VK_IMAGE_ASPECT_COLOR_BIT, 1);
	}
	else
	{
		if (VkCreateImage(Width, Height, 1, Samples, ColorFormat, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &Image.Image, &Image.ImageMemory) == OPENVK_ERROR)
			return OpenVkRuntimeError("Failed to Create Msaa Attachment Image");
		Image.ImageView = VkCreateImageView(Image.Image, ColorFormat, VK_IMAGE_ASPECT_COLOR_BIT, 1);
	}

	return CMA_Push(&VkRenderer.ImageAttachments, &Image);
//	VkRenderer.ImageCount++;

//	return VkRenderer.ImageCount - 1;
}

uint32_t VkCreateDepthImageAttachment(uint32_t Width, uint32_t Height, uint32_t MsaaSamples, OpenVkBool Sampled, uint32_t Format)
{
	VkFormat DepthFormat;
	if (Format == OPENVK_FORMAT_DEFAULT)
		DepthFormat = VkFindDepthFormat();
	else
		DepthFormat = VkGetOpenVkFormat(Format);

//	VkRenderer.Images = (VkImage*)OpenVkRealloc(VkRenderer.Images, (VkRenderer.ImageCount + 1) * sizeof(VkImage));
//	VkRenderer.ImageMemories = (VkDeviceMemory*)OpenVkRealloc(VkRenderer.ImageMemories, (VkRenderer.ImageCount + 1) * sizeof(VkDeviceMemory));
//	VkRenderer.ImageViews = (VkImageView*)OpenVkRealloc(VkRenderer.ImageViews, (VkRenderer.ImageCount + 1) * sizeof(VkImageView));
	VkImageInfo Image;
	Image.Format = DepthFormat;

	VkSampleCountFlagBits Samples = (VkSampleCountFlagBits)(VkRenderer.MsaaSamples < MsaaSamples ? VkRenderer.MsaaSamples : MsaaSamples);

	if (Sampled)
	{
		if (VkCreateImage(Width, Height, 1, Samples, DepthFormat, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &Image.Image, &Image.ImageMemory) == OPENVK_ERROR)
			return OpenVkRuntimeError("Failed to Create Depth Sampled Attachment Image");
		Image.ImageView = VkCreateImageView(Image.Image, DepthFormat, VK_IMAGE_ASPECT_DEPTH_BIT, 1);
	}
	else
	{
		if (VkCreateImage(Width, Height, 1, Samples, DepthFormat, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &Image.Image, &Image.ImageMemory) == OPENVK_ERROR)
			return OpenVkRuntimeError("Failed to Create Depth Attachment Image");
		Image.ImageView = VkCreateImageView(Image.Image, DepthFormat, VK_IMAGE_ASPECT_DEPTH_BIT, 1);
	}	

	return CMA_Push(&VkRenderer.ImageAttachments, &Image);

//	VkRenderer.ImageCount++;
//
//	return VkRenderer.ImageCount - 1;
}

uint32_t VkCreateRenderPass(uint32_t ColorAttachmentCount, uint32_t* ColorFormats, OpenVkBool DepthAttachment, uint32_t DepthFormat, OpenVkBool MsaaAttachment, uint32_t MsaaSamples, OpenVkBool Sampled)
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
	{
		AttachmentDescriptions[i] = ColorAttachmentDescription;
		AttachmentDescriptions[i].format = VkGetOpenVkFormat(ColorFormats[i]);
	}		
	for (uint32_t i = ColorAttachmentCount; i < ColorAttachmentCount + DepthAttachment; i++)
	{
		AttachmentDescriptions[i] = DepthAttachmentDescription;

		if (DepthFormat == OPENVK_FORMAT_DEFAULT)
			 AttachmentDescriptions[i].format = VkFindDepthFormat();
		else AttachmentDescriptions[i].format = VkGetOpenVkFormat(DepthFormat);
	}
		
	for (uint32_t i = ColorAttachmentCount + DepthAttachment; i < AttachmentCount; i++)
	{
		AttachmentDescriptions[i] = ColorAttachmentResolveDescription;
		AttachmentDescriptions[i].format = VkGetOpenVkFormat(ColorFormats[i - (ColorAttachmentCount + DepthAttachment)]);
	}
		

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

uint32_t VkCreatePipelineLayout(OpenVkPipelineLayoutCreateInfo* Info)
{
	VkRenderer.PipelineLayouts = (VkPipelineLayout*)OpenVkRealloc(VkRenderer.PipelineLayouts, (VkRenderer.PipelineLayoutCount + 1) * sizeof(VkPipelineLayout));

	VkDescriptorSetLayout* SetLayouts = NULL;

	if (Info->DescriptorSetLayoutCount > 0)
	{
		SetLayouts = (VkDescriptorSetLayout*)OpenVkMalloc(Info->DescriptorSetLayoutCount * sizeof(VkDescriptorSetLayout));
		for (uint32_t i = 0; i < Info->DescriptorSetLayoutCount; i++)
		{
			SetLayouts[i] = VkRenderer.DescriptorSetLayouts[Info->DescriptorSetLayouts[i]];
		}
	}

	VkPushConstantRange* PushConstantRanges = NULL;

	if (Info->PushConstantCount > 0)
	{
		PushConstantRanges = (VkPushConstantRange*)OpenVkMalloc(Info->PushConstantCount * sizeof(VkPushConstantRange));
		for (uint32_t i = 0; i < Info->PushConstantCount; i++)
		{
			if (Info->PushConstantShaderTypes[i] == 0) PushConstantRanges[i].stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
			if (Info->PushConstantShaderTypes[i] == 1) PushConstantRanges[i].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
			PushConstantRanges[i].offset = Info->PushConstantOffsets[i];
			PushConstantRanges[i].size = Info->PushConstantSizes[i];
		}
	}

	VkPipelineLayoutCreateInfo PipeLineLayoutInfo;
	PipeLineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	PipeLineLayoutInfo.pNext = NULL;
	PipeLineLayoutInfo.flags = 0;
	PipeLineLayoutInfo.setLayoutCount = Info->DescriptorSetLayoutCount;
	PipeLineLayoutInfo.pSetLayouts = SetLayouts;
	PipeLineLayoutInfo.pushConstantRangeCount = Info->PushConstantCount;
	PipeLineLayoutInfo.pPushConstantRanges = PushConstantRanges;

	if (vkCreatePipelineLayout(VkRenderer.Device, &PipeLineLayoutInfo, NULL, &VkRenderer.PipelineLayouts[VkRenderer.PipelineLayoutCount]) != VK_SUCCESS)
		return OpenVkRuntimeError("Failed to Create Pipeline Layout");

	if (Info->DescriptorSetLayoutCount > 0)
		OpenVkFree(SetLayouts);
	if (Info->PushConstantCount > 0)
		OpenVkFree(PushConstantRanges);

	return VkRenderer.PipelineLayoutCount++;
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
uint32_t VkCreateGraphicsPipeline(OpenVkGraphicsPipelineCreateInfo* Info)
{
	VkRenderer.Pipelines = (VkPipeline*)OpenVkRealloc(VkRenderer.Pipelines, (VkRenderer.PipelineCount + 1) * sizeof(VkPipeline));

	VkShaderModule VertexShaderModule;
	VkCreateShaderModule(Info->VertexShader, &VertexShaderModule);
	VkShaderModule FragmentShaderModule;
	VkCreateShaderModule(Info->FragmentShader, &FragmentShaderModule);

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
	BindingDescription.stride = Info->BindingStride;
	BindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

	uint32_t AttributeDescriptionCount = Info->ShaderAttributeFormatCount;
	VkVertexInputAttributeDescription* AttributeDescriptions = (VkVertexInputAttributeDescription*)OpenVkMalloc(AttributeDescriptionCount * sizeof(VkVertexInputAttributeDescription));
	for (uint32_t i = 0; i < AttributeDescriptionCount; i++)
	{
		AttributeDescriptions[i].location = i;
		AttributeDescriptions[i].binding = 0;
		AttributeDescriptions[i].format = VkGetOpenVkFormat(Info->ShaderAttributeFormats[i]);
	//	if (Info->ShaderAttributeFormats[i] == 1) AttributeDescriptions[i].format = VK_FORMAT_R32_SFLOAT;
	//	if (Info->ShaderAttributeFormats[i] == 2) AttributeDescriptions[i].format = VK_FORMAT_R32G32_SFLOAT;
	//	if (Info->ShaderAttributeFormats[i] == 3) AttributeDescriptions[i].format = VK_FORMAT_R32G32B32_SFLOAT;
	//	if (Info->ShaderAttributeFormats[i] == 4) AttributeDescriptions[i].format = VK_FORMAT_R32G32B32A32_SFLOAT;
		AttributeDescriptions[i].offset = Info->ShaderAttributeOffsets[i];
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
	if (Info->PrimitiveTopology == 0) InputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_POINT_LIST;
	if (Info->PrimitiveTopology == 1) InputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_LINE_LIST;
	if (Info->PrimitiveTopology == 2) InputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	InputAssembly.primitiveRestartEnable = VK_FALSE;

	VkViewport Viewport;
	Viewport.x = Info->x;
	Viewport.y = Info->y;
	Viewport.width = Info->Width;
	Viewport.height = Info->Height;
	Viewport.minDepth = 0.0;
	Viewport.maxDepth = 1.0;

	VkRect2D Scissor;
	Scissor.offset.x = 0;
	Scissor.offset.y = 0;
	Scissor.extent.width = Info->Width;
	Scissor.extent.height = Info->Height;

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
	Rasterizer.polygonMode = (VkPolygonMode)Info->PolygonMode;
	Rasterizer.lineWidth = Info->LineWidth;
	Rasterizer.cullMode = Info->CullMode;
	Rasterizer.frontFace = (VkFrontFace)Info->FrontFace;
	Rasterizer.depthBiasEnable = VK_FALSE;
	Rasterizer.depthBiasConstantFactor = 1.25;
	Rasterizer.depthBiasClamp = 0.0;
	Rasterizer.depthBiasSlopeFactor = 1.75;

	VkPipelineMultisampleStateCreateInfo Multisampling;
	Multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	Multisampling.pNext = NULL;
	Multisampling.flags = 0;
	Multisampling.sampleShadingEnable = VK_FALSE;
	Multisampling.rasterizationSamples = (VkSampleCountFlagBits)(VkRenderer.MsaaSamples < Info->MsaaSamples ? VkRenderer.MsaaSamples : Info->MsaaSamples);
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

	VkPipelineColorBlendAttachmentState* ColorBlendAttachmentStates = (VkPipelineColorBlendAttachmentState*)OpenVkMalloc(Info->ColorBlendAttachments * sizeof(VkPipelineColorBlendAttachmentState));
	for (uint32_t i = 0; i < Info->ColorBlendAttachments; i++)
	{
		ColorBlendAttachmentStates[i].blendEnable = Info->AlphaBlending;
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
	ColorBlending.attachmentCount = Info->ColorBlendAttachments;
	ColorBlending.pAttachments = ColorBlendAttachmentStates;
	ColorBlending.blendConstants[0] = 0.0;
	ColorBlending.blendConstants[1] = 0.0;
	ColorBlending.blendConstants[2] = 0.0;
	ColorBlending.blendConstants[3] = 0.0;

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
	PipelineInfo.pDepthStencilState = (Info->DepthStencil ? &DepthStencilState : NULL);
	PipelineInfo.pColorBlendState = &ColorBlending;
	PipelineInfo.pDynamicState = &DynamicStateInfo;
	PipelineInfo.layout = VkRenderer.PipelineLayouts[Info->PipelineLayout];
	PipelineInfo.renderPass = VkRenderer.RenderPasses[Info->RenderPass];
	PipelineInfo.subpass = 0;
	PipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
	PipelineInfo.basePipelineIndex = -1;

	if (vkCreateGraphicsPipelines(VkRenderer.Device, VK_NULL_HANDLE, 1, &PipelineInfo, NULL, &VkRenderer.Pipelines[VkRenderer.PipelineCount]) != VK_SUCCESS)
		return OpenVkRuntimeError("Failed to Create Graphics Pipeline");

	vkDestroyShaderModule(VkRenderer.Device, VertexShaderModule, NULL);
	vkDestroyShaderModule(VkRenderer.Device, FragmentShaderModule, NULL);

	OpenVkFree(AttributeDescriptions);
	if (Info->ColorBlendAttachments > 0)
		OpenVkFree(ColorBlendAttachmentStates);

	VkRenderer.PipelineCount++;

	return VkRenderer.PipelineCount - 1;
}
/*
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
*/
uint32_t VkCreateFramebuffer(OpenVkFramebufferCreateInfo* Info)
{
	VkRenderer.Framebuffers = (VkFramebufferInfo*)OpenVkRealloc(VkRenderer.Framebuffers, (VkRenderer.FramebufferCount + 1) * sizeof(VkFramebufferInfo));
	VkRenderer.Framebuffers[VkRenderer.FramebufferCount].Framebuffers = (VkFramebuffer*)OpenVkMalloc(VkRenderer.SwapChainImageCount * sizeof(VkFramebuffer));

	VkImageView* ImageViewAttachments = (VkImageView*)OpenVkMalloc(Info->AttachmentCount * sizeof(VkImageView));
		
	for (uint32_t i = 0; i < VkRenderer.SwapChainImageCount; i++)
	{
		for (uint32_t j = 0; j < Info->AttachmentCount; j++)
		{
			if (Info->Attachments[j] == 0)
				ImageViewAttachments[j] = VkRenderer.SwapChainImageViews[i];
			else
			{
				VkImageInfo* ImageAttachment = (VkImageInfo*)CMA_GetAt(&VkRenderer.ImageAttachments, Info->Attachments[j]);
				if (ImageAttachment != NULL)
					ImageViewAttachments[j] = ImageAttachment->ImageView;
			}				
		}
			

		VkFramebufferCreateInfo FramebufferInfo;
		FramebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		FramebufferInfo.pNext = NULL;
		FramebufferInfo.flags = 0;
		FramebufferInfo.renderPass = VkRenderer.RenderPasses[Info->RenderPass];
		FramebufferInfo.attachmentCount = Info->AttachmentCount;
		FramebufferInfo.pAttachments = ImageViewAttachments;
		FramebufferInfo.width = Info->Width;
		FramebufferInfo.height = Info->Height;
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
uint32_t VkCreateDescriptorSetLayout(uint32_t BindingCount, uint32_t* Bindings, uint32_t* DescriptorCounts, uint32_t* DescriptorTypes, uint32_t* ShaderTypes)
{
	VkRenderer.DescriptorSetLayouts = (VkDescriptorSetLayout*)OpenVkRealloc(VkRenderer.DescriptorSetLayouts, (VkRenderer.DescriptorSetLayoutCount + 1) * sizeof(VkDescriptorSetLayout));

	VkDescriptorSetLayoutBinding* LayoutBindings = (VkDescriptorSetLayoutBinding*)OpenVkMalloc(BindingCount * sizeof(VkDescriptorSetLayoutBinding));

	for (uint32_t i = 0; i < BindingCount; i++)
	{
		LayoutBindings[i].binding = Bindings[i];
		if (DescriptorTypes[i] == OPENVK_DESCRIPTOR_TYPE_UNIFORM_BUFFER) LayoutBindings[i].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		if (DescriptorTypes[i] == OPENVK_DESCRIPTOR_TYPE_DYNAMIC_UNIFORM_BUFFER) LayoutBindings[i].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
		if (DescriptorTypes[i] == OPENVK_DESCRIPTOR_TYPE_IMAGE_SAMPLER) LayoutBindings[i].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		if (DescriptorTypes[i] == OPENVK_DESCRIPTOR_TYPE_STORAGE_IMAGE) LayoutBindings[i].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
		if (DescriptorTypes[i] == OPENVK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE) LayoutBindings[i].descriptorType = VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR;
		LayoutBindings[i].descriptorCount = DescriptorCounts[i];
		if (ShaderTypes[i] == OPENVK_SHADER_TYPE_VERTEX) LayoutBindings[i].stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
		if (ShaderTypes[i] == OPENVK_SHADER_TYPE_FRAGMENT) LayoutBindings[i].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
		if (ShaderTypes[i] == OPENVK_SHADER_TYPE_RAYGEN) LayoutBindings[i].stageFlags = VK_SHADER_STAGE_RAYGEN_BIT_KHR;
		if (ShaderTypes[i] == OPENVK_SHADER_TYPE_MISS) LayoutBindings[i].stageFlags = VK_SHADER_STAGE_MISS_BIT_KHR;
		if (ShaderTypes[i] == OPENVK_SHADER_TYPE_CLOSEST_HIT) LayoutBindings[i].stageFlags = VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR;
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

	return VkRenderer.DescriptorSetLayoutCount++;
}

//DescriptorTypes
//Uniform Buffer = 0
//Dynamic Uniform Buffer = 1
//Image Sampler = 2
//Pool Types
//0 = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER
//1 = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE
uint32_t VkCreateDescriptorPool(uint32_t DescriptorPoolType, uint32_t PoolSizeCount, uint32_t* DescriptorTypes, uint32_t* DescriptorCounts)
{
	uint32_t MaxSets = 0;

	VkDescriptorPoolSize* PoolSizes = (VkDescriptorPoolSize*)OpenVkMalloc(PoolSizeCount * sizeof(VkDescriptorPoolSize));

	for (uint32_t i = 0; i < PoolSizeCount; i++)
	{
		if (DescriptorTypes[i] == OPENVK_DESCRIPTOR_TYPE_UNIFORM_BUFFER)			PoolSizes[i].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		if (DescriptorTypes[i] == OPENVK_DESCRIPTOR_TYPE_DYNAMIC_UNIFORM_BUFFER)	PoolSizes[i].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
		if (DescriptorTypes[i] == OPENVK_DESCRIPTOR_TYPE_IMAGE_SAMPLER)				PoolSizes[i].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		if (DescriptorTypes[i] == OPENVK_DESCRIPTOR_TYPE_STORAGE_IMAGE)				PoolSizes[i].type = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
		if (DescriptorTypes[i] == OPENVK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE)	PoolSizes[i].type = VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR;
		if (DescriptorTypes[i] == OPENVK_DESCRIPTOR_TYPE_STORAGE_BUFFER)			PoolSizes[i].type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
		PoolSizes[i].descriptorCount = DescriptorCounts[i] * MAX_FRAMES_IN_FLIGHT;
		
		MaxSets += DescriptorCounts[i] * MAX_FRAMES_IN_FLIGHT;
	}

	VkDescriptorPoolCreateInfo PoolInfo;
	PoolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	PoolInfo.pNext = NULL;
	PoolInfo.maxSets = MaxSets * MAX_FRAMES_IN_FLIGHT;	//maximum number of descriptor sets that may be allocated
	if (DescriptorPoolType == OPENVK_DESCRIPTOR_POOL_DEFAULT)	PoolInfo.flags = 0;
	if (DescriptorPoolType == OPENVK_DESCRIPTOR_POOL_FREEABLE)	PoolInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
	if (DescriptorPoolType == OPENVK_DESCRIPTOR_POOL_UPDATABLE)	PoolInfo.flags = VK_DESCRIPTOR_POOL_CREATE_UPDATE_AFTER_BIND_BIT;
	PoolInfo.poolSizeCount = PoolSizeCount;
	PoolInfo.pPoolSizes = PoolSizes;

	VkDescriptorPoolInfo DescriptorPool;
	DescriptorPool.DescriptorSets = CMA_Create(sizeof(uint32_t));

	if (vkCreateDescriptorPool(VkRenderer.Device, &PoolInfo, NULL, &DescriptorPool.DescriptorPool) != VK_SUCCESS)
	{
		OpenVkFree(PoolSizes);
		return OpenVkRuntimeError("Failed to create descriptor pool");
	}

	OpenVkFree(PoolSizes);
	return CMA_Push(&VkRenderer.DescriptorPools, &DescriptorPool);
}

//just works if descriptor pool is created "Freeable"
OpenVkBool VkFreeDescriptorSet(uint32_t DescriptorPool, uint32_t DescriptorSet)
{
	VkDescriptorPoolInfo* DescriptorPoolPTR = (VkDescriptorPoolInfo*)CMA_GetAt(&VkRenderer.DescriptorPools, DescriptorPool);
	if (DescriptorPoolPTR == NULL)
		return OpenVkRuntimeError("Failed to find descriptor pool");

	VkDescriptorSetInfo* DescriptorSetPTR = (VkDescriptorSetInfo*)CMA_GetAt(&VkRenderer.DescriptorSets, DescriptorSet);
	if (DescriptorSetPTR == NULL)
		return OpenVkRuntimeError("Failed find descriptor set");

	vkFreeDescriptorSets(VkRenderer.Device, DescriptorPoolPTR->DescriptorPool, MAX_FRAMES_IN_FLIGHT, DescriptorSetPTR->DescriptorSets);
	CMA_Pop(&VkRenderer.DescriptorSets, DescriptorSet);

	return OpenVkTrue;
}

OpenVkBool VkDestroyDescriptorPool(uint32_t DescriptorPool)
{
	VkDescriptorPoolInfo* DescriptorPoolPTR = (VkDescriptorPoolInfo*)CMA_GetAt(&VkRenderer.DescriptorPools, DescriptorPool);
	if (DescriptorPoolPTR == NULL)
		return OpenVkRuntimeError("Failed to destroy descriptor pool");

	for (uint32_t i = 0; i < DescriptorPoolPTR->DescriptorSets.Size; i++)
	{
		uint32_t* DescriptorSetPTR = (uint32_t*)CMA_GetAt(&DescriptorPoolPTR->DescriptorSets, i);
		CMA_Pop(&VkRenderer.DescriptorSets, *DescriptorSetPTR);
	}		

	vkDestroyDescriptorPool(VkRenderer.Device, DescriptorPoolPTR->DescriptorPool, NULL);

	CMA_Pop(&VkRenderer.DescriptorPools, DescriptorPool);

	return OpenVkTrue;
}

uint32_t VkUpdateDescriptorSet(OpenVkDescriptorSetCreateInfo* Info)
{
	VkDescriptorSetInfo* DescriptorSetInfo = (VkDescriptorSetInfo*)CMA_GetAt(&VkRenderer.DescriptorSets, *Info->DescriptorSet);
	if (DescriptorSetInfo == NULL)
		return OpenVkRuntimeError("Failed to find update descriptor set");

	VkWriteDescriptorSet* DescriptorWrites = (VkWriteDescriptorSet*)OpenVkMalloc(Info->DescriptorWriteCount * sizeof(VkWriteDescriptorSet));

	VkDescriptorBufferInfo* DescriptorBufferInfos = NULL;
	VkDescriptorImageInfo* DescriptorImageInfos = NULL;
	VkWriteDescriptorSetAccelerationStructureKHR* DescriptorASInfos = NULL;

	//i guess for static textures we don't need to do something like this(MAX_FRAMES_IN_FLIGHT)
	for (uint32_t j = 0; j < MAX_FRAMES_IN_FLIGHT; j++)
	{
		for (uint32_t i = 0; i < Info->DescriptorWriteCount; i++)
		{
			OpenVkBool Failed = OpenVkFalse;

			if (Info->DescriptorTypes[i] == OPENVK_DESCRIPTOR_TYPE_UNIFORM_BUFFER ||
				Info->DescriptorTypes[i] == OPENVK_DESCRIPTOR_TYPE_DYNAMIC_UNIFORM_BUFFER ||
				Info->DescriptorTypes[i] == OPENVK_DESCRIPTOR_TYPE_STORAGE_BUFFER)
			{
				//Uniform
				if (DescriptorBufferInfos != NULL) OpenVkFree(DescriptorBufferInfos);
				DescriptorBufferInfos = (VkDescriptorBufferInfo*)OpenVkMalloc(Info->DescriptorCounts[i] * sizeof(VkDescriptorBufferInfo));

				for (uint32_t m = 0; m < Info->DescriptorCounts[i]; m++)
				{
					VkDynamicBufferInfo* Buffer = (VkDynamicBufferInfo*)CMA_GetAt(&VkRenderer.DynamicBuffers, Info->Buffers[m]);

					if (Buffer == NULL)
						Failed = OpenVkTrue;

					DescriptorBufferInfos[m].buffer = Buffer->Buffers[j];
					DescriptorBufferInfos[m].offset = 0;
					if (Info->BufferSizes[m] == 0)
						 DescriptorBufferInfos[m].range = VK_WHOLE_SIZE;
					else DescriptorBufferInfos[m].range = Info->BufferSizes[m];
				}

				if (Info->DescriptorTypes[i] == OPENVK_DESCRIPTOR_TYPE_UNIFORM_BUFFER)
					DescriptorWrites[i] = VkDescriptorSetWrite(DescriptorSetInfo->DescriptorSets[j], Info->Bindings[i], VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, Info->DescriptorCounts[i], NULL, DescriptorBufferInfos, NULL);
				if (Info->DescriptorTypes[i] == OPENVK_DESCRIPTOR_TYPE_DYNAMIC_UNIFORM_BUFFER)
					DescriptorWrites[i] = VkDescriptorSetWrite(DescriptorSetInfo->DescriptorSets[j], Info->Bindings[i], VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, Info->DescriptorCounts[i], NULL, DescriptorBufferInfos, NULL);
			}
			if (Info->DescriptorTypes[i] == OPENVK_DESCRIPTOR_TYPE_STORAGE_BUFFER)
			{
				//Uniform
				if (DescriptorBufferInfos != NULL) OpenVkFree(DescriptorBufferInfos);
				DescriptorBufferInfos = (VkDescriptorBufferInfo*)OpenVkMalloc(Info->DescriptorCounts[i] * sizeof(VkDescriptorBufferInfo));

				for (uint32_t m = 0; m < Info->DescriptorCounts[i]; m++)
				{
					VkStaticBufferInfo* Buffer = (VkStaticBufferInfo*)CMA_GetAt(&VkRenderer.StaticBuffers, Info->Buffers[m]);
					if (Buffer == NULL)
						Failed = OpenVkTrue;

					DescriptorBufferInfos[m].buffer = Buffer->Buffer;
					DescriptorBufferInfos[m].offset = 0;
					if (Info->BufferSizes[m] == 0)
						DescriptorBufferInfos[m].range = VK_WHOLE_SIZE;
					else DescriptorBufferInfos[m].range = Info->BufferSizes[m];
				}

				if (Info->DescriptorTypes[i] == OPENVK_DESCRIPTOR_TYPE_STORAGE_BUFFER)
					DescriptorWrites[i] = VkDescriptorSetWrite(DescriptorSetInfo->DescriptorSets[j], Info->Bindings[i], VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, Info->DescriptorCounts[i], NULL, DescriptorBufferInfos, NULL);
			}
			if (Info->DescriptorTypes[i] == OPENVK_DESCRIPTOR_TYPE_IMAGE_SAMPLER ||
				Info->DescriptorTypes[i] == OPENVK_DESCRIPTOR_TYPE_STORAGE_IMAGE)
			{
				//image
				if (DescriptorImageInfos != NULL) OpenVkFree(DescriptorImageInfos);
				DescriptorImageInfos = (VkDescriptorImageInfo*)OpenVkMalloc(Info->DescriptorCounts[i] * sizeof(VkDescriptorImageInfo));

				for (uint32_t m = 0; m < Info->DescriptorCounts[i]; m++)
				{
					if (Info->ImageTypes[m] == OPENVK_IMAGE_TYPE_TEXTURE)
					{
						VkSampler* ImageSampler = (VkSampler*)CMA_GetAt(&VkRenderer.Sampler, Info->Sampler[m]);
						VkImageInfo* TextureImage = (VkImageInfo*)CMA_GetAt(&VkRenderer.Images, Info->Images[m]);

						if (ImageSampler != NULL) DescriptorImageInfos[m].sampler = *ImageSampler;
						else Failed = OpenVkTrue;
						if (TextureImage != NULL) DescriptorImageInfos[m].imageView = TextureImage->ImageView;
						else Failed = OpenVkTrue;
					}
					if (Info->ImageTypes[m] == OPENVK_IMAGE_TYPE_ATTACHMENT)
					{
						VkSampler* ImageSampler = (VkSampler*)CMA_GetAt(&VkRenderer.Sampler, Info->Sampler[m]);
						VkImageInfo* TextureImage = (VkImageInfo*)CMA_GetAt(&VkRenderer.ImageAttachments, Info->Images[m]);

						if (ImageSampler != NULL) DescriptorImageInfos[m].sampler = *ImageSampler;
						else Failed = OpenVkTrue;
						if (TextureImage != NULL) DescriptorImageInfos[m].imageView = TextureImage->ImageView;
						else Failed = OpenVkTrue;
					}
					if (Info->ImageTypes[m] == OPENVK_IMAGE_TYPE_STORAGE)
					{
						VkImageInfo* Image = (VkImageInfo*)CMA_GetAt(&VkRenderer.Images, Info->Images[m]);

						DescriptorImageInfos[m].sampler = 0;
						if (Image != NULL) DescriptorImageInfos[m].imageView = Image->ImageView;
						else Failed = OpenVkTrue;
					}

					if (Info->ImageLayouts[m] == OPENVK_IMAGE_LAYOUT_COLOR_OUTPUT) DescriptorImageInfos[m].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
					if (Info->ImageLayouts[m] == OPENVK_IMAGE_LAYOUT_DEPTH_OUTPUT) DescriptorImageInfos[m].imageLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;
					if (Info->ImageLayouts[m] == OPENVK_IMAGE_LAYOUT_GENERAL_OUTPUT) DescriptorImageInfos[m].imageLayout = VK_IMAGE_LAYOUT_GENERAL;
				}

				if (Info->DescriptorTypes[i] == OPENVK_DESCRIPTOR_TYPE_IMAGE_SAMPLER)
					DescriptorWrites[i] = VkDescriptorSetWrite(DescriptorSetInfo->DescriptorSets[j], Info->Bindings[i], VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, Info->DescriptorCounts[i], DescriptorImageInfos, NULL, NULL);
				if (Info->DescriptorTypes[i] == OPENVK_DESCRIPTOR_TYPE_STORAGE_IMAGE)
					DescriptorWrites[i] = VkDescriptorSetWrite(DescriptorSetInfo->DescriptorSets[j], Info->Bindings[i], VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, Info->DescriptorCounts[i], DescriptorImageInfos, NULL, NULL);
			}
			if (Info->DescriptorTypes[i] == OPENVK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE)
			{
				if (DescriptorASInfos != NULL) OpenVkFree(DescriptorASInfos);
				DescriptorASInfos = (VkWriteDescriptorSetAccelerationStructureKHR*)OpenVkMalloc(Info->DescriptorCounts[i] * sizeof(VkWriteDescriptorSetAccelerationStructureKHR));

				for (uint32_t m = 0; m < Info->DescriptorCounts[i]; m++)
				{
					//	DescriptorASInfos[m].buffer = VkRenderer.UniformBuffers[Info->UniformBuffers[m]].Buffers[j];
					//	DescriptorASInfos[m].offset = 0;
					//	DescriptorASInfos[m].range = Info->UniformBufferSizes[m];
					DescriptorASInfos[m].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET_ACCELERATION_STRUCTURE_KHR;
					DescriptorASInfos[m].pNext = NULL;
					DescriptorASInfos[m].accelerationStructureCount = 1;
					VkAccelerationStructure* AS = (VkAccelerationStructure*)CMA_GetAt(&VkRaytracer.TopLevelAS, Info->TopLevelAS[m]);
					if (AS != NULL)	DescriptorASInfos[m].pAccelerationStructures = &AS->Handle;
					else Failed = OpenVkTrue;
				}

				DescriptorWrites[i] = VkDescriptorSetWrite(DescriptorSetInfo->DescriptorSets[j], Info->Bindings[i], VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR, Info->DescriptorCounts[i], NULL, NULL, DescriptorASInfos);
			}

			if (Failed)
			{
				OpenVkFree(DescriptorWrites);
				if (DescriptorBufferInfos != NULL) OpenVkFree(DescriptorBufferInfos);
				if (DescriptorImageInfos != NULL) OpenVkFree(DescriptorImageInfos);

				return OpenVkRuntimeError("Failed to descriptor stuff");
			}
		}

		vkUpdateDescriptorSets(VkRenderer.Device, Info->DescriptorWriteCount, DescriptorWrites, 0, NULL);
	}

	OpenVkFree(DescriptorWrites);
	if (DescriptorBufferInfos != NULL) OpenVkFree(DescriptorBufferInfos);
	if (DescriptorImageInfos != NULL) OpenVkFree(DescriptorImageInfos);
	if (DescriptorASInfos != NULL) OpenVkFree(DescriptorASInfos);

	return *Info->DescriptorSet;
}

uint32_t VkCreateDescriptorSet(OpenVkDescriptorSetCreateInfo* Info)
{
	if (Info->DescriptorSet == NULL)
	{
		VkDescriptorSetInfo DescriptorSetInfo;

		VkDescriptorSetLayout DescriptorSetLayouts[MAX_FRAMES_IN_FLIGHT];
		for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
			DescriptorSetLayouts[i] = VkRenderer.DescriptorSetLayouts[Info->DescriptorSetLayout];

		VkDescriptorSetAllocateInfo AllocateInfo;

		VkDescriptorPoolInfo* DescriptorPoolPTR = (VkDescriptorPoolInfo*)CMA_GetAt(&VkRenderer.DescriptorPools, Info->DescriptorPool);
		if (DescriptorPoolPTR == NULL)
			return OpenVkRuntimeError("Failed to find descriptor pool");

		AllocateInfo = VkAllocateDescriptorSets(DescriptorPoolPTR->DescriptorPool, MAX_FRAMES_IN_FLIGHT, DescriptorSetLayouts);

		if (vkAllocateDescriptorSets(VkRenderer.Device, &AllocateInfo, DescriptorSetInfo.DescriptorSets) != VK_SUCCESS)
			return OpenVkRuntimeError("Failed to Allocate Descriptor Set");

		uint32_t DescriptorSetPTR = CMA_Push(&VkRenderer.DescriptorSets, &DescriptorSetInfo);
		CMA_Push(&DescriptorPoolPTR->DescriptorSets, &DescriptorSetPTR);
		Info->DescriptorSet = &DescriptorSetPTR;
	}	
	
	return VkUpdateDescriptorSet(Info);
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
//	exit(1);
	if (Success == VK_ERROR_OUT_OF_DATE_KHR)
	{
		if (ResizeFunc != NULL)
			ResizeFunc();
	}
	else if (Success != VK_SUCCESS)
		return OpenVkRuntimeError("Failed to submit draw command buffer queue");

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
		return OpenVkRuntimeError("Failed to present draw command buffer queue");

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

void VkBeginRenderPass(OpenVkBeginRenderPassInfo* Info)
{
	VkClearValue ClearValues[8];
	for (uint32_t i = 0; i < Info->ClearColors; i++)
	{
		VkClearValue ClearValue = { Info->ClearColor[0], Info->ClearColor[1], Info->ClearColor[2], Info->ClearColor[3] };

		ClearValues[i] = ClearValue;
	}
	
	if (Info->ClearDepth)
	{
		VkClearValue ClearValue = { 1.0, 0.0 };
		ClearValues[Info->ClearColors] = ClearValue;
	}

	VkRenderPassBeginInfo RenderPassBeginInfo;
	RenderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	RenderPassBeginInfo.pNext = NULL;
	RenderPassBeginInfo.renderPass = VkRenderer.RenderPasses[Info->RenderPass];
	RenderPassBeginInfo.framebuffer = VkRenderer.Framebuffers[Info->Framebuffer].Framebuffers[VkRenderer.ImageIndex];
	RenderPassBeginInfo.renderArea.offset.x = Info->x;
	RenderPassBeginInfo.renderArea.offset.y = Info->y;
	RenderPassBeginInfo.renderArea.extent.width = Info->Width;
	RenderPassBeginInfo.renderArea.extent.height = Info->Height;
	RenderPassBeginInfo.clearValueCount = (Info->ClearColors + Info->ClearDepth);
	RenderPassBeginInfo.pClearValues = ClearValues;

	vkCmdBeginRenderPass(VkRenderer.CommandBuffers[VkRenderer.ImageIndex], &RenderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
}

void VkEndRenderPass()
{
	vkCmdEndRenderPass(VkRenderer.CommandBuffers[VkRenderer.ImageIndex]);
}

uint32_t VkCreateTextureImage(unsigned char* Pixels, int32_t Width, int32_t Height, uint32_t Format)
{
//	VkRenderer.TextureImageMemories = (VkDeviceMemory*)OpenVkRealloc(VkRenderer.TextureImageMemories, (VkRenderer.TextureImageCount + 1) * sizeof(VkDeviceMemory));
//	VkRenderer.Images = (VkImage*)OpenVkRealloc(VkRenderer.Images, (VkRenderer.TextureImageCount + 1) * sizeof(VkImage));
//	VkRenderer.TextureImageViews = (VkImageView*)OpenVkRealloc(VkRenderer.TextureImageViews, (VkRenderer.TextureImageCount + 1) * sizeof(VkImageView));

	VkImageInfo TextureImage;
	TextureImage.Format = VkGetOpenVkFormat(Format);//VK_FORMAT_R8G8B8A8_UNORM;

//	int32_t TextureWidth;
//	int32_t TextureHeight;
//	int32_t TextureChannels;
//
//	stbi_set_flip_vertically_on_load(FlipVertical);
//	unsigned char* Pixel = stbi_load(Path, &TextureWidth, &TextureHeight, &TextureChannels, STBI_rgb_alpha);
	VkRenderer.MipLevels = floorf(log2f(MAX(Width, Height))) + 1;

	VkDeviceSize ImageSize = Width * Height * Format;
	if (!Pixels)
		return OpenVkRuntimeError("Invalid Texture");

	VkBuffer StagingBuffer;
	VkDeviceMemory StagingBufferMemory;

	if (VkCreateBuffer(ImageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &StagingBuffer, &StagingBufferMemory) == OPENVK_ERROR)
		return OpenVkRuntimeError("Failed to create Texture Buffer");

	void* Data;
	vkMapMemory(VkRenderer.Device, StagingBufferMemory, 0, ImageSize, 0, &Data);
	memcpy(Data, Pixels, ImageSize);
	vkUnmapMemory(VkRenderer.Device, StagingBufferMemory);

	OpenVkFree(Pixels);

	if (VkCreateImage(Width, Height, VkRenderer.MipLevels, VK_SAMPLE_COUNT_1_BIT, TextureImage.Format, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &TextureImage.Image, &TextureImage.ImageMemory) == OPENVK_ERROR)
		return OpenVkRuntimeError("Failed to create Texture Image");

//	VkTransitionImageLayout(TextureImage.Image, TextureImage.Format, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VkRenderer.MipLevels);
	VkCommandBuffer CommandBuffer = VkBeginSingleTimeCommands();
	VkSetImageLayout(CommandBuffer, TextureImage.Image, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VkRenderer.MipLevels, NULL);
	VkEndSingleTimeCommandBuffer(CommandBuffer);

	VkCopyBufferToImage(StagingBuffer, TextureImage.Image, Width, Height);
	//	if (TransitionImageLayout(TextureImages, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, MipLevels) != VK_SUCCESS)
	//		return 1;
	VkGenerateMipmaps(TextureImage.Image, TextureImage.Format, Width, Height, VkRenderer.MipLevels);

	vkDestroyBuffer(VkRenderer.Device, StagingBuffer, NULL);
	vkFreeMemory(VkRenderer.Device, StagingBufferMemory, NULL);

	TextureImage.ImageView = VkCreateImageView(TextureImage.Image, TextureImage.Format, VK_IMAGE_ASPECT_COLOR_BIT, VkRenderer.MipLevels);

	return CMA_Push(&VkRenderer.Images, &TextureImage);
}

uint32_t VkCreateStorageImage(uint32_t Width, uint32_t Height, uint32_t Format)
{
	VkImageInfo Image;
	Image.Format = VkGetOpenVkFormat(Format);
	VkCreateImage(Width, Height, 1, VK_SAMPLE_COUNT_1_BIT, Image.Format, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_STORAGE_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &Image.Image, &Image.ImageMemory);
	Image.ImageView = VkCreateImageView(Image.Image, Image.Format, VK_IMAGE_ASPECT_COLOR_BIT, 1);
	/*
	VkImageMemoryBarrier ImageMemoryBarrier;
	ImageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	ImageMemoryBarrier.pNext = NULL;
	ImageMemoryBarrier.srcAccessMask = 0;
	ImageMemoryBarrier.dstAccessMask = 0;
	ImageMemoryBarrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	ImageMemoryBarrier.newLayout = VK_IMAGE_LAYOUT_GENERAL;
	ImageMemoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	ImageMemoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	ImageMemoryBarrier.image = Image.Image;
	ImageMemoryBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	ImageMemoryBarrier.subresourceRange.baseMipLevel = 0;
	ImageMemoryBarrier.subresourceRange.levelCount = 1;
	ImageMemoryBarrier.subresourceRange.baseArrayLayer = 0;
	ImageMemoryBarrier.subresourceRange.layerCount = 1;

	VkCommandBuffer CommandBuffer = VkBeginSingleTimeCommands();
	vkCmdPipelineBarrier(
		CommandBuffer,
		VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
		VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
		0,
		0, NULL,
		0, NULL,
		1, &ImageMemoryBarrier);
	VkEndSingleTimeCommandBuffer(CommandBuffer);
	*/

	VkCommandBuffer CommandBuffer = VkBeginSingleTimeCommands();
	VkSetImageLayout(CommandBuffer, Image.Image, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_GENERAL, 1, NULL);
	VkEndSingleTimeCommandBuffer(CommandBuffer);

	return CMA_Push(&VkRenderer.Images, &Image);
}

void VkDestroyImage(uint32_t InImage)
{
	vkDeviceWaitIdle(VkRenderer.Device);

	VkImageInfo* Image = (VkImageInfo*)CMA_GetAt(&VkRenderer.Images, InImage);
	if (Image != NULL)
	{
		vkDestroyImageView(VkRenderer.Device, Image->ImageView, NULL);

		vkDestroyImage(VkRenderer.Device, Image->Image, NULL);
		vkFreeMemory(VkRenderer.Device, Image->ImageMemory, NULL);

		CMA_Pop(&VkRenderer.Images, InImage);
	}	
}

OpenVkBool VkCopyImage(uint32_t Width, uint32_t Height, uint32_t Src, uint32_t Dst)
{
	VkImageInfo* ImageInfo;
	VkImage SrcImage;
	VkImage DstImage;

	if (Src == 0)
		SrcImage = VkRenderer.SwapChainImages[VkRenderer.ImageIndex];
	else
	{
		ImageInfo = (VkImageInfo*)CMA_GetAt(&VkRenderer.Images, Src);
		if (ImageInfo == NULL)
			return OpenVkRuntimeError("Failed to find src image for copying");
		SrcImage = ImageInfo->Image;
	}
		
	if (Dst == 0)
		DstImage = VkRenderer.SwapChainImages[VkRenderer.ImageIndex];
	else
	{
		ImageInfo = (VkImageInfo*)CMA_GetAt(&VkRenderer.Images, Dst);
		if (ImageInfo == NULL)
			return OpenVkRuntimeError("Failed to find dst image for copying");
		DstImage = ImageInfo->Image;
	}

	VkSetImageLayout(VkRenderer.CommandBuffers[VkRenderer.ImageIndex], DstImage, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, NULL);
	VkSetImageLayout(VkRenderer.CommandBuffers[VkRenderer.ImageIndex], SrcImage, VK_IMAGE_LAYOUT_GENERAL, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, 1, NULL);
	
	VkImageSubresourceLayers SrcSubresource = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 0, 1 };
	VkOffset3D				 SrcOffset = { 0, 0, 0 };
	VkImageSubresourceLayers DstSubresource = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 0, 1 };
	VkOffset3D				 DstOffset = { 0, 0, 0 };
	VkExtent3D				 Extent = { Width, Height, 1 };
	
	VkImageCopy CopyRegion;
	CopyRegion.srcSubresource = SrcSubresource;
	CopyRegion.srcOffset = SrcOffset;
	CopyRegion.dstSubresource = DstSubresource;
	CopyRegion.dstOffset = DstOffset;
	CopyRegion.extent = Extent;
	vkCmdCopyImage(VkRenderer.CommandBuffers[VkRenderer.ImageIndex], SrcImage, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, DstImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &CopyRegion);

	VkSetImageLayout(VkRenderer.CommandBuffers[VkRenderer.ImageIndex], DstImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR, 1, NULL);
	VkSetImageLayout(VkRenderer.CommandBuffers[VkRenderer.ImageIndex], SrcImage, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, VK_IMAGE_LAYOUT_GENERAL, 1, NULL);

	return OpenVkTrue;
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
	SamplerInfo.mipmapMode = (VkSamplerMipmapMode)Filter;
//	SamplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
	SamplerInfo.addressModeU = (VkSamplerAddressMode)AddressMode;
	SamplerInfo.addressModeV = (VkSamplerAddressMode)AddressMode;
	SamplerInfo.addressModeW = (VkSamplerAddressMode)AddressMode;
	SamplerInfo.mipLodBias = 0.0;
	SamplerInfo.anisotropyEnable = VK_TRUE;
	SamplerInfo.maxAnisotropy = 16;
	SamplerInfo.compareEnable = VK_TRUE;
	SamplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
	SamplerInfo.minLod = 0.0;
	SamplerInfo.maxLod = VkRenderer.MipLevels;
	SamplerInfo.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;
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

//I need to come up with an idea to also create a normal vertex/ index buffer when using raytracing
uint32_t VkCreateVertexBuffer(size_t Size, const void* Vertices)
{
	if (OpenVkRendererFlags & OPENVK_RAYTRACING)
	{
		return VkCreateBufferExt(VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,			//removed VK_BUFFER_USAGE_STORAGE_BUFFER_BIT
								 VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
								 Size, Vertices);
	}
	

	return VkCreateBufferExt(VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
							 VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
							 Size, Vertices);
}

uint32_t VkCreateIndexBuffer(size_t Size, const void* Indices)
{
	if (OpenVkRendererFlags & OPENVK_RAYTRACING)
	{
		return VkCreateBufferExt(VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,			//removed VK_BUFFER_USAGE_STORAGE_BUFFER_BIT
								 VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
								 Size, Indices);
	}


	return VkCreateBufferExt(VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, 
							 VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
							 Size, Indices);
}

//FIX? Add raytracing support
uint32_t VkCreateDynamicVertexBuffer(size_t Size)
{
	return VkCreateDynamicBuffer(Size, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);
}

//FIX? Add raytracing support
uint32_t VkCreateDynamicIndexBuffer(size_t Size)
{
	return VkCreateDynamicBuffer(Size, VK_BUFFER_USAGE_INDEX_BUFFER_BIT);
}

uint32_t VkCreateUniformBuffer(size_t Size)
{
	VkDynamicBufferInfo UniformBuffer;

	for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
		if (VkCreateBuffer(Size, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &UniformBuffer.Buffers[i], &UniformBuffer.BufferMemories[i]) == OPENVK_ERROR)
			return OpenVkRuntimeError("Failed to create uniform buffer");

	return CMA_Push(&VkRenderer.DynamicBuffers, &UniformBuffer);
}

uint32_t VkCreateDynamicUniformBuffer(size_t Size)
{
	//FIX make it working?
/*
	VkRenderer.StaticBuffers = (VkBuffer*)OpenVkRealloc(VkRenderer.StaticBuffers, (VkRenderer.BufferCount + 1) * sizeof(VkBuffer));
	VkRenderer.BufferMemories = (VkDeviceMemory*)OpenVkRealloc(VkRenderer.BufferMemories, (VkRenderer.BufferCount + 1) * sizeof(VkDeviceMemory));

	if (VkCreateBuffer(Size, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, &VkRenderer.StaticBuffers[VkRenderer.BufferCount], &VkRenderer.BufferMemories[VkRenderer.BufferCount]) == OPENVK_ERROR)
		return OpenVkRuntimeError("Failed To Create Dynamic Uniform Buffer");

	VkRenderer.BufferCount++;

	return VkRenderer.BufferCount - 1;
*/
	return VkCreateDynamicBuffer(Size, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);
}

OpenVkBool VkUpdateBuffer(size_t Size, const void* BufferData, uint32_t Buffer)
{
	VkDynamicBufferInfo* BufferPTR = (VkDynamicBufferInfo*)CMA_GetAt(&VkRenderer.DynamicBuffers, Buffer);
	if (BufferPTR == NULL)
		return OpenVkRuntimeError("Failed to find update buffer");

	void* Data;
	vkMapMemory(VkRenderer.Device, BufferPTR->BufferMemories[VkRenderer.CurrentFrame], 0, Size, 0, &Data);
	memcpy(Data, BufferData, Size);
	vkUnmapMemory(VkRenderer.Device, BufferPTR->BufferMemories[VkRenderer.CurrentFrame]);

	return OpenVkTrue;
}

OpenVkBool VkUpdateDynamicBuffer(size_t Size, const void* Data, uint32_t Buffer)
{
	VkDynamicBufferInfo* BufferPTR = (VkDynamicBufferInfo*)CMA_GetAt(&VkRenderer.DynamicBuffers, Buffer);
	if (BufferPTR == NULL)
		return OpenVkRuntimeError("Failed to find dynamic update buffer");

	void* CopyData;
	vkMapMemory(VkRenderer.Device, BufferPTR->BufferMemories[VkRenderer.CurrentFrame], 0, BufferPTR->Size, 0, &CopyData);
	memcpy(CopyData, Data, Size);

	VkMappedMemoryRange MemoryRange;
	MemoryRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
	MemoryRange.pNext = NULL;
	MemoryRange.memory = BufferPTR->BufferMemories[VkRenderer.CurrentFrame];
	MemoryRange.offset = 0;
	MemoryRange.size = VK_WHOLE_SIZE;
	vkFlushMappedMemoryRanges(VkRenderer.Device, 1, &MemoryRange);

	vkUnmapMemory(VkRenderer.Device, BufferPTR->BufferMemories[VkRenderer.CurrentFrame]);

	return OpenVkTrue;

	//FIX doesn't work I guess?
	/*
	void* Data;
	vkMapMemory(VkRenderer.Device, VkRenderer.BufferMemories[UniformBuffer], 0, Size, 0, &Data);
	memcpy(Data, UBO, Size);
	

	VkMappedMemoryRange MemoryRange;
	MemoryRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
	MemoryRange.pNext = NULL;
	MemoryRange.memory = VkRenderer.BufferMemories[UniformBuffer];
	MemoryRange.offset = 0;
	MemoryRange.size = VK_WHOLE_SIZE;
	vkFlushMappedMemoryRanges(VkRenderer.Device, 1, &MemoryRange);

	vkUnmapMemory(VkRenderer.Device, VkRenderer.BufferMemories[UniformBuffer]);
	*/
}

void VkBindPipeline(uint32_t Pipeline, uint32_t PipelineType)
{
	if (PipelineType == OPENVK_PIPELINE_TYPE_GRAPHICS)
		PipelineType = VK_PIPELINE_BIND_POINT_GRAPHICS;
	else if (PipelineType == OPENVK_PIPELINE_TYPE_RAYTRACING)
		PipelineType = VK_PIPELINE_BIND_POINT_RAY_TRACING_KHR;

	vkCmdBindPipeline(VkRenderer.CommandBuffers[VkRenderer.ImageIndex], (VkPipelineBindPoint)PipelineType, VkRenderer.Pipelines[Pipeline]);
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
	VkRect2D Scissor;
	Scissor.offset.x = x;
	Scissor.offset.y = y;
	Scissor.extent.width = Width;
	Scissor.extent.height = Height;

	vkCmdSetScissor(VkRenderer.CommandBuffers[VkRenderer.ImageIndex], 0, 1, &Scissor);
}

void VkBindVertexBuffer(uint32_t VertexBuffer)
{
	VkDeviceSize Offsets[1] = { 0 };
	VkStaticBufferInfo* BufferInfo = (VkStaticBufferInfo*)CMA_GetAt(&VkRenderer.StaticBuffers, VertexBuffer);
	if (BufferInfo != NULL)
		vkCmdBindVertexBuffers(VkRenderer.CommandBuffers[VkRenderer.ImageIndex], 0, 1, &BufferInfo->Buffer, Offsets);
}

void VkBindIndexBuffer(uint32_t VertexBuffer, uint32_t IndexBuffer)
{
	VkDeviceSize Offsets[1] = { 0 };
	VkStaticBufferInfo* VertexBufferInfo = (VkStaticBufferInfo*)CMA_GetAt(&VkRenderer.StaticBuffers, VertexBuffer);
	VkStaticBufferInfo* IndexBufferInfo = (VkStaticBufferInfo*)CMA_GetAt(&VkRenderer.StaticBuffers, IndexBuffer);
	if (VertexBufferInfo != NULL)
		vkCmdBindVertexBuffers(VkRenderer.CommandBuffers[VkRenderer.ImageIndex], 0, 1, &VertexBufferInfo->Buffer, Offsets);
	if (IndexBufferInfo != NULL)
		vkCmdBindIndexBuffer(VkRenderer.CommandBuffers[VkRenderer.ImageIndex], IndexBufferInfo->Buffer, 0, VK_INDEX_TYPE_UINT32);
}

void VkBindDynamicVertexBuffer(uint32_t VertexBuffer)
{
	VkDeviceSize Offsets[1] = { 0 };
	VkDynamicBufferInfo* BufferInfo = (VkDynamicBufferInfo*)CMA_GetAt(&VkRenderer.DynamicBuffers, VertexBuffer);
	if (BufferInfo != NULL)
		vkCmdBindVertexBuffers(VkRenderer.CommandBuffers[VkRenderer.ImageIndex], 0, 1, &BufferInfo->Buffers[VkRenderer.CurrentFrame], Offsets);
}

void VkBindDynamicIndexBuffer(uint32_t VertexBuffer, uint32_t IndexBuffer)
{
	VkDeviceSize Offsets[1] = { 0 };
	VkDynamicBufferInfo* VertexBufferInfo = (VkDynamicBufferInfo*)CMA_GetAt(&VkRenderer.DynamicBuffers, VertexBuffer);
	VkDynamicBufferInfo* IndexBufferInfo = (VkDynamicBufferInfo*)CMA_GetAt(&VkRenderer.DynamicBuffers, IndexBuffer);
	if (VertexBufferInfo != NULL)
		vkCmdBindVertexBuffers(VkRenderer.CommandBuffers[VkRenderer.ImageIndex], 0, 1, &VertexBufferInfo->Buffers[VkRenderer.CurrentFrame], Offsets);
	if (IndexBufferInfo != NULL)
		vkCmdBindIndexBuffer(VkRenderer.CommandBuffers[VkRenderer.ImageIndex], IndexBufferInfo->Buffers[VkRenderer.CurrentFrame], 0, VK_INDEX_TYPE_UINT32);
}

void VkDrawVertices(uint32_t VertexCount)
{
	vkCmdDraw(VkRenderer.CommandBuffers[VkRenderer.ImageIndex], VertexCount, 1, 0, 0);
}

void VkDrawIndices(uint32_t IndexCount)
{
	vkCmdDrawIndexed(VkRenderer.CommandBuffers[VkRenderer.ImageIndex], IndexCount, 1, 0, 0, 0);
}

void VkBindDescriptorSet(uint32_t PipelineLayout, uint32_t Set, uint32_t DescriptorSet, uint32_t PipelineType)
{
	if (PipelineType == OPENVK_PIPELINE_TYPE_GRAPHICS)
		PipelineType = VK_PIPELINE_BIND_POINT_GRAPHICS;
	else if (PipelineType == OPENVK_PIPELINE_TYPE_RAYTRACING)
		PipelineType = VK_PIPELINE_BIND_POINT_RAY_TRACING_KHR;

	VkDescriptorSetInfo* DescriptorSetInfo = (VkDescriptorSetInfo*)CMA_GetAt(&VkRenderer.DescriptorSets, DescriptorSet);
	if (DescriptorSetInfo != NULL) vkCmdBindDescriptorSets(VkRenderer.CommandBuffers[VkRenderer.ImageIndex], (VkPipelineBindPoint)PipelineType, VkRenderer.PipelineLayouts[PipelineLayout], Set, 1, &DescriptorSetInfo->DescriptorSets[VkRenderer.CurrentFrame], 0, NULL);
}

//Shader Types
//0 = Vertex Shader
//1 = Fragment Shader
void VkPushConstant(uint32_t PipelineLayout, uint32_t ShaderType, uint32_t Offset, size_t Size, const void* Data)
{
	vkCmdPushConstants(VkRenderer.CommandBuffers[VkRenderer.ImageIndex], VkRenderer.PipelineLayouts[PipelineLayout], (ShaderType == 0 ? VK_SHADER_STAGE_VERTEX_BIT : VK_SHADER_STAGE_FRAGMENT_BIT), Offset, Size, Data);
}

void VkCleanupSwapChain()
{
	vkDeviceWaitIdle(VkRenderer.Device);

	for (uint32_t i = 0; i < VkRenderer.SwapChainImageCount; i++)
	{
	//	vkDestroyImage(VkRenderer.Device, VkRenderer.Images[i].Images[j], NULL);
		vkDestroyImageView(VkRenderer.Device, VkRenderer.SwapChainImageViews[i], NULL);
	}

//	for (uint32_t i = 1; i < VkRenderer.ImageCount; i++)
//	{
//		vkFreeMemory(VkRenderer.Device, VkRenderer.ImageMemories[i], NULL);
//		vkDestroyImage(VkRenderer.Device, VkRenderer.Images[i], NULL);
//		vkDestroyImageView(VkRenderer.Device, VkRenderer.ImageViews[i], NULL);
//	}		
	
	for (uint32_t i = 1; i < VkRenderer.ImageAttachments.Size; i++)
	{
		VkImageInfo* ImageAttachment = (VkImageInfo*)CMA_GetAt(&VkRenderer.ImageAttachments, i);
		if (ImageAttachment != NULL)
		{
			vkDestroyImageView(VkRenderer.Device, ImageAttachment->ImageView, NULL);
			vkDestroyImage(VkRenderer.Device, ImageAttachment->Image, NULL);
			vkFreeMemory(VkRenderer.Device, ImageAttachment->ImageMemory, NULL);
			CMA_Pop(&VkRenderer.ImageAttachments, i);
		}
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

	vkFreeCommandBuffers(VkRenderer.Device, VkRenderer.CommandPool, VkRenderer.SwapChainImageCount, VkRenderer.CommandBuffers);
	OpenVkFree(VkRenderer.CommandBuffers);

	vkDestroySwapchainKHR(VkRenderer.Device, VkRenderer.SwapChain, NULL);
}

void VkRecreateSwapChain(uint32_t* Width, uint32_t* Height)
{
	VkCleanupSwapChain();

	VkRenderer.SwapChainImageCount = 0;
//	VkRenderer.ImageCount = 1;
	VkRenderer.RenderPassCount = 0;
	VkRenderer.FramebufferCount = 0;
//	VkRenderer.DescriptorSetCount = 0;

	VkCreateSwapChain(Width, Height);
	VkCreateCommandBuffers();
	VkRenderer.InFlightImages = (VkFence*)OpenVkRealloc(VkRenderer.InFlightImages, VkRenderer.SwapChainImageCount * sizeof(VkFence));
}

void VkDestroyRenderer()
{
	vkDeviceWaitIdle(VkRenderer.Device);

	for (uint32_t i = 0; i < VkRenderer.PipelineCount; i++)
		vkDestroyPipeline(VkRenderer.Device, VkRenderer.Pipelines[i], NULL);

	for (uint32_t i = 0; i < VkRenderer.PipelineLayoutCount; i++)
		vkDestroyPipelineLayout(VkRenderer.Device, VkRenderer.PipelineLayouts[i], NULL);

	VkCleanupSwapChain();

	for (uint32_t i = 0; i < VkRenderer.DescriptorPools.Size; i++)
		VkDestroyDescriptorPool(i);

	CMA_Destroy(&VkRenderer.DescriptorPools);

	for (uint32_t i = 0; i < VkRenderer.DynamicBuffers.Size; i++)
	{
		VkDynamicBufferInfo* BufferPTR = (VkDynamicBufferInfo*)CMA_GetAt(&VkRenderer.DynamicBuffers, i);
		if (BufferPTR != NULL)
		{
			for (uint32_t j = 0; j < MAX_FRAMES_IN_FLIGHT; j++)
			{
				vkDestroyBuffer(VkRenderer.Device, BufferPTR->Buffers[j], NULL);
				vkFreeMemory(VkRenderer.Device, BufferPTR->BufferMemories[j], NULL);
			}
		}
	}
	
	CMA_Destroy(&VkRenderer.DynamicBuffers);

	for (uint32_t i = 0; i < VkRenderer.Sampler.Size; i++)
	{
		VkSampler* Sampler = (VkSampler*)CMA_GetAt(&VkRenderer.Sampler, i);
		if (Sampler != NULL)
			vkDestroySampler(VkRenderer.Device, *Sampler, NULL);
	}
		

//	if (VkRenderer.SamplerCount > 0)
//		OpenVkFree(VkRenderer.Sampler);
	CMA_Destroy(&VkRenderer.Sampler);

	//we start at 0 because the first one is a dummy(reserved for swapchain)
	for (uint32_t i = 1; i < VkRenderer.Images.Size; i++)
	{
		VkImageInfo* Image = (VkImageInfo*)CMA_GetAt(&VkRenderer.Images, i);
		if (Image != NULL)
		{
			vkDestroyImageView(VkRenderer.Device, Image->ImageView, NULL);
			vkDestroyImage(VkRenderer.Device, Image->Image, NULL);
			vkFreeMemory(VkRenderer.Device, Image->ImageMemory, NULL);
		}		
	}
	
	if (OpenVkRendererFlags & OPENVK_RAYTRACING)
		VkDestroyRaytracing();
	
//	OpenVkFree(VkRenderer.TextureImageViews);
//	OpenVkFree(VkRenderer.Images);
//	OpenVkFree(VkRenderer.TextureImageMemories);

	for (uint32_t i = 0; i < VkRenderer.DescriptorSetLayoutCount; i++)
		vkDestroyDescriptorSetLayout(VkRenderer.Device, VkRenderer.DescriptorSetLayouts[i], NULL);

//	OpenVkFree(VkRenderer.DescriptorSetLayouts);

	for (uint32_t i = 0; i < VkRenderer.StaticBuffers.Size; i++)
	{
		VkStaticBufferInfo* BufferInfo = (VkStaticBufferInfo*)CMA_GetAt(&VkRenderer.StaticBuffers, i);
		if (BufferInfo != NULL)
		{
			vkDestroyBuffer(VkRenderer.Device, BufferInfo->Buffer, NULL);
			vkFreeMemory(VkRenderer.Device, BufferInfo->BufferMemory, NULL);
		}
	}

//	OpenVkFree(VkRenderer.StaticBuffers);
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
	CMA_Destroy(&VkRenderer.StaticBuffers);
	CMA_Destroy(&VkRenderer.Images);

//	OpenVkFree(VkRenderer.Pipelines);
//	OpenVkFree(VkRenderer.PipelineLayouts);
	
//	OpenVkFree(VkRenderer.RenderPasses);
	//	OpenVkFree(VkRenderer.DescriptorSets);
//	OpenVkFree(VkRenderer.DescriptorSetLayouts);
//	
//	OpenVkFree(VkRenderer.DynamicDescriptorPools.DescriptorPools);
//	OpenVkFree(VkRenderer.StaticDescriptorPools.DescriptorPools);

	OpenVkRuntimeInfo("Successfully destroyed Renderer", "");
}