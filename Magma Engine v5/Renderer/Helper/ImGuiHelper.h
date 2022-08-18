ImFont* DefaultFont;
ImFont* IconFontLarge;
ImFont* IconFontSmall;

ImVec4 ColorFromBytes(uint8_t r, uint8_t g, uint8_t b)
{
	return ImVec4((float)r / 255.0, (float)g / 255.0, (float)b / 255.0, 1.0);
};

void SetStyleImGui()
{
	/*
	ImGuiStyle* Style = &ImGui::GetStyle();
	ImVec4* Colors = Style->Colors;

	const ImVec4 bgColor = ColorFromBytes(37, 37, 38);
	const ImVec4 lightBgColor = ColorFromBytes(82, 82, 85);
	const ImVec4 veryLightBgColor = ColorFromBytes(90, 90, 95);

	const ImVec4 panelColor = ColorFromBytes(51, 51, 55);
	const ImVec4 panelHoverColor = ColorFromBytes(29, 151, 236);
	const ImVec4 panelActiveColor = ColorFromBytes(0, 119, 200);

	const ImVec4 textColor = ColorFromBytes(255, 255, 255);
	const ImVec4 textDisabledColor = ColorFromBytes(151, 151, 151);
	const ImVec4 borderColor = ColorFromBytes(78, 78, 78);

	Colors[ImGuiCol_Text] = textColor;
	Colors[ImGuiCol_TextDisabled] = textDisabledColor;
	Colors[ImGuiCol_TextSelectedBg] = panelActiveColor;
	Colors[ImGuiCol_WindowBg] = bgColor;
	Colors[ImGuiCol_ChildBg] = bgColor;
	Colors[ImGuiCol_PopupBg] = bgColor;
	Colors[ImGuiCol_Border] = borderColor;
	Colors[ImGuiCol_BorderShadow] = borderColor;
	Colors[ImGuiCol_FrameBg] = panelColor;
	Colors[ImGuiCol_FrameBgHovered] = panelHoverColor;
	Colors[ImGuiCol_FrameBgActive] = panelActiveColor;
	Colors[ImGuiCol_TitleBg] = bgColor;
	Colors[ImGuiCol_TitleBgActive] = bgColor;
	Colors[ImGuiCol_TitleBgCollapsed] = bgColor;
	Colors[ImGuiCol_MenuBarBg] = panelColor;
	Colors[ImGuiCol_ScrollbarBg] = panelColor;
	Colors[ImGuiCol_ScrollbarGrab] = lightBgColor;
	Colors[ImGuiCol_ScrollbarGrabHovered] = veryLightBgColor;
	Colors[ImGuiCol_ScrollbarGrabActive] = veryLightBgColor;
	Colors[ImGuiCol_CheckMark] = panelActiveColor;
	Colors[ImGuiCol_SliderGrab] = panelHoverColor;
	Colors[ImGuiCol_SliderGrabActive] = panelActiveColor;
	Colors[ImGuiCol_Button] = panelColor;
	Colors[ImGuiCol_ButtonHovered] = panelHoverColor;
	Colors[ImGuiCol_ButtonActive] = panelHoverColor;
	Colors[ImGuiCol_Header] = panelColor;
	Colors[ImGuiCol_HeaderHovered] = panelHoverColor;
	Colors[ImGuiCol_HeaderActive] = panelActiveColor;
	Colors[ImGuiCol_Separator] = borderColor;
	Colors[ImGuiCol_SeparatorHovered] = borderColor;
	Colors[ImGuiCol_SeparatorActive] = borderColor;
	Colors[ImGuiCol_ResizeGrip] = bgColor;
	Colors[ImGuiCol_ResizeGripHovered] = panelColor;
	Colors[ImGuiCol_ResizeGripActive] = lightBgColor;
	Colors[ImGuiCol_PlotLines] = panelActiveColor;
	Colors[ImGuiCol_PlotLinesHovered] = panelHoverColor;
	Colors[ImGuiCol_PlotHistogram] = panelActiveColor;
	Colors[ImGuiCol_PlotHistogramHovered] = panelHoverColor;
//	Colors[ImGuiCol_ModalWindowDarkening] = bgColor;
	Colors[ImGuiCol_DragDropTarget] = bgColor;
	Colors[ImGuiCol_NavHighlight] = bgColor;
	Colors[ImGuiCol_DockingPreview] = panelActiveColor;
	Colors[ImGuiCol_Tab] = bgColor;
	Colors[ImGuiCol_TabActive] = panelActiveColor;
	Colors[ImGuiCol_TabUnfocused] = bgColor;
	Colors[ImGuiCol_TabUnfocusedActive] = panelActiveColor;
	Colors[ImGuiCol_TabHovered] = panelHoverColor;

	Style->WindowRounding = 0.0f;
	Style->ChildRounding = 0.0f;
	Style->FrameRounding = 0.0f;
	Style->GrabRounding = 0.0f;
	Style->PopupRounding = 0.0f;
	Style->ScrollbarRounding = 0.0f;
	Style->TabRounding = 0.0f;
	Style->WindowBorderSize = 0.0;
	*/
	
	ImGuiStyle* style = &ImGui::GetStyle();

	style->WindowBorderSize = 0.0;
	style->WindowPadding = ImVec2(15, 15);
	style->WindowRounding = 5.0f;
	style->FramePadding = ImVec2(5, 5);
	style->FrameRounding = 4.0f;
	style->ItemSpacing = ImVec2(12, 8);
	style->ItemInnerSpacing = ImVec2(8, 6);
	style->IndentSpacing = 25.0f;
	style->ScrollbarSize = 15.0f;
	style->ScrollbarRounding = 9.0f;
	style->GrabMinSize = 5.0f;
	style->GrabRounding = 3.0f;
	ImVec4* colors = ImGui::GetStyle().Colors;
	colors[ImGuiCol_Text] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
	colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
	colors[ImGuiCol_WindowBg] = ImVec4(0.06f, 0.06f, 0.06f, 0.94f);
	colors[ImGuiCol_ChildBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	colors[ImGuiCol_PopupBg] = ImVec4(0.08f, 0.08f, 0.08f, 0.94f);
	colors[ImGuiCol_Border] = ImVec4(0.43f, 0.43f, 0.50f, 0.50f);
	colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	colors[ImGuiCol_FrameBg] = ImVec4(0.44f, 0.44f, 0.44f, 0.60f);
	colors[ImGuiCol_FrameBgHovered] = ImVec4(0.57f, 0.57f, 0.57f, 0.70f);
	colors[ImGuiCol_FrameBgActive] = ImVec4(0.76f, 0.76f, 0.76f, 0.80f);
	colors[ImGuiCol_TitleBg] = ImVec4(0.04f, 0.04f, 0.04f, 1.00f);
	colors[ImGuiCol_TitleBgActive] = ImVec4(0.16f, 0.16f, 0.16f, 1.00f);
	colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.00f, 0.00f, 0.00f, 0.60f);
	colors[ImGuiCol_MenuBarBg] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
	colors[ImGuiCol_ScrollbarBg] = ImVec4(0.02f, 0.02f, 0.02f, 0.53f);
	colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.31f, 0.31f, 0.31f, 1.00f);
	colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.41f, 0.41f, 0.41f, 1.00f);
	colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.51f, 0.51f, 0.51f, 1.00f);
	colors[ImGuiCol_CheckMark] = ImVec4(0.13f, 0.75f, 0.55f, 0.80f);
	colors[ImGuiCol_SliderGrab] = ImVec4(0.13f, 0.75f, 0.75f, 0.80f);
	colors[ImGuiCol_SliderGrabActive] = ImVec4(0.13f, 0.75f, 1.00f, 0.80f);
	colors[ImGuiCol_Button] = ImVec4(0.13f, 0.75f, 0.55f, 0.40f);
	colors[ImGuiCol_ButtonHovered] = ImVec4(0.13f, 0.75f, 0.75f, 0.60f);
	colors[ImGuiCol_ButtonActive] = ImVec4(0.13f, 0.75f, 1.00f, 0.80f);
	colors[ImGuiCol_Header] = ImVec4(0.13f, 0.75f, 0.55f, 0.40f);
	colors[ImGuiCol_HeaderHovered] = ImVec4(0.13f, 0.75f, 0.75f, 0.60f);
	colors[ImGuiCol_HeaderActive] = ImVec4(0.13f, 0.75f, 1.00f, 0.80f);
	colors[ImGuiCol_Separator] = ImVec4(0.13f, 0.75f, 0.55f, 0.40f);
	colors[ImGuiCol_SeparatorHovered] = ImVec4(0.13f, 0.75f, 0.75f, 0.60f);
	colors[ImGuiCol_SeparatorActive] = ImVec4(0.13f, 0.75f, 1.00f, 0.80f);
	colors[ImGuiCol_ResizeGrip] = ImVec4(0.13f, 0.75f, 0.55f, 0.40f);
	colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.13f, 0.75f, 0.75f, 0.60f);
	colors[ImGuiCol_ResizeGripActive] = ImVec4(0.13f, 0.75f, 1.00f, 0.80f);
	colors[ImGuiCol_Tab] = ImVec4(0.13f, 0.75f, 0.55f, 0.80f);
	colors[ImGuiCol_TabHovered] = ImVec4(0.13f, 0.75f, 0.75f, 0.80f);
	colors[ImGuiCol_TabActive] = ImVec4(0.13f, 0.75f, 1.00f, 0.80f);
	colors[ImGuiCol_TabUnfocused] = ImVec4(0.18f, 0.18f, 0.18f, 1.00f);
	colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.36f, 0.36f, 0.36f, 0.54f);
	colors[ImGuiCol_DockingPreview] = ImVec4(0.13f, 0.75f, 0.55f, 0.80f);
	colors[ImGuiCol_DockingEmptyBg] = ImVec4(0.13f, 0.13f, 0.13f, 0.80f);
	colors[ImGuiCol_PlotLines] = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
	colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
	colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
	colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
	colors[ImGuiCol_TableHeaderBg] = ImVec4(0.19f, 0.19f, 0.20f, 1.00f);
	colors[ImGuiCol_TableBorderStrong] = ImVec4(0.31f, 0.31f, 0.35f, 1.00f);
	colors[ImGuiCol_TableBorderLight] = ImVec4(0.23f, 0.23f, 0.25f, 1.00f);
	colors[ImGuiCol_TableRowBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	colors[ImGuiCol_TableRowBgAlt] = ImVec4(1.00f, 1.00f, 1.00f, 0.07f);
	colors[ImGuiCol_TextSelectedBg] = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
	colors[ImGuiCol_DragDropTarget] = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
	colors[ImGuiCol_NavHighlight] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
	colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
	colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
	colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);
	
	/*
	ImGuiStyle* style = &ImGui::GetStyle();

	style->WindowBorderSize = 0.0;
	style->WindowPadding = ImVec2(15, 15);
	style->WindowRounding = 5.0f;
	style->FramePadding = ImVec2(5, 5);
	style->FrameRounding = 4.0f;
	style->ItemSpacing = ImVec2(12, 8);
	style->ItemInnerSpacing = ImVec2(8, 6);
	style->IndentSpacing = 25.0f;
	style->ScrollbarSize = 15.0f;
	style->ScrollbarRounding = 9.0f;
	style->GrabMinSize = 5.0f;
	style->GrabRounding = 3.0f;

	style->Colors[ImGuiCol_Text] = ImVec4(0.80f, 0.80f, 0.83f, 1.00f);
	style->Colors[ImGuiCol_TextDisabled] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
	style->Colors[ImGuiCol_WindowBg] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
	style->Colors[ImGuiCol_PopupBg] = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
	style->Colors[ImGuiCol_Border] = ImVec4(0.80f, 0.80f, 0.83f, 0.88f);
	style->Colors[ImGuiCol_BorderShadow] = ImVec4(0.92f, 0.91f, 0.88f, 0.00f);
	style->Colors[ImGuiCol_FrameBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
	style->Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
	style->Colors[ImGuiCol_FrameBgActive] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	style->Colors[ImGuiCol_TitleBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
	style->Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(1.00f, 0.98f, 0.95f, 0.75f);
	style->Colors[ImGuiCol_TitleBgActive] = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
	style->Colors[ImGuiCol_MenuBarBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
	style->Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
	style->Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
	style->Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	style->Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
	style->Colors[ImGuiCol_CheckMark] = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
	style->Colors[ImGuiCol_SliderGrab] = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
	style->Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
	style->Colors[ImGuiCol_Button] = ImVec4(0.21f, 0.20f, 0.26f, 1.00f);
	style->Colors[ImGuiCol_ButtonHovered] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
	style->Colors[ImGuiCol_ButtonActive] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	style->Colors[ImGuiCol_Header] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
	style->Colors[ImGuiCol_HeaderHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	style->Colors[ImGuiCol_HeaderActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);

	style->Colors[ImGuiCol_ResizeGrip] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	style->Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	style->Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);

	style->Colors[ImGuiCol_PlotLines] = ImVec4(0.40f, 0.39f, 0.38f, 0.63f);
	style->Colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.25f, 1.00f, 0.00f, 1.00f);
	style->Colors[ImGuiCol_PlotHistogram] = ImVec4(0.40f, 0.39f, 0.38f, 0.63f);
	style->Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.25f, 1.00f, 0.00f, 1.00f);
	style->Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.25f, 1.00f, 0.00f, 0.43f);
	*/
/*
	ImGuiStyle &style = ImGui::GetStyle();

	style.Colors[ImGuiCol_Text]                  = make_text(0.78f);
	style.Colors[ImGuiCol_TextDisabled]          = make_text(0.28f);
	style.Colors[ImGuiCol_WindowBg]              = make_bg(1.00f, window_offset);
	style.Colors[ImGuiCol_ChildBg]               = make_bg(0.58f);
	style.Colors[ImGuiCol_PopupBg]               = make_bg(0.9f);
	style.Colors[ImGuiCol_Border]                = make_bg(0.6f, -0.05f);
	style.Colors[ImGuiCol_BorderShadow]          = make_bg(0.0f, 0.0f);
	style.Colors[ImGuiCol_FrameBg]               = make_bg(1.00f);
	style.Colors[ImGuiCol_FrameBgHovered]        = make_mid(0.78f);
	style.Colors[ImGuiCol_FrameBgActive]         = make_mid(1.00f);
	style.Colors[ImGuiCol_TitleBg]               = make_low(1.00f);
	style.Colors[ImGuiCol_TitleBgActive]         = make_high(1.00f);
	style.Colors[ImGuiCol_TitleBgCollapsed]      = make_bg(0.75f);
	style.Colors[ImGuiCol_MenuBarBg]             = make_bg(0.47f);
	style.Colors[ImGuiCol_ScrollbarBg]           = make_bg(1.00f);
	style.Colors[ImGuiCol_ScrollbarGrab]         = make_low(1.00f);
	style.Colors[ImGuiCol_ScrollbarGrabHovered]  = make_mid(0.78f);
	style.Colors[ImGuiCol_ScrollbarGrabActive]   = make_mid(1.00f);
	style.Colors[ImGuiCol_CheckMark]             = make_high(1.00f);
	style.Colors[ImGuiCol_SliderGrab]            = make_bg(1.0f, .1f);
	style.Colors[ImGuiCol_SliderGrabActive]      = make_high(1.0f);
	style.Colors[ImGuiCol_Button]                = make_bg(1.0f, .2f);
	style.Colors[ImGuiCol_ButtonHovered]         = make_mid(1.00f);
	style.Colors[ImGuiCol_ButtonActive]          = make_high(1.00f);
	style.Colors[ImGuiCol_Header]                = make_mid(0.76f);
	style.Colors[ImGuiCol_HeaderHovered]         = make_mid(0.86f);
	style.Colors[ImGuiCol_HeaderActive]          = make_high(1.00f);
	style.Colors[ImGuiCol_ResizeGrip]            = ImVec4(0.47f, 0.77f, 0.83f, 0.04f);
	style.Colors[ImGuiCol_ResizeGripHovered]     = make_mid(0.78f);
	style.Colors[ImGuiCol_ResizeGripActive]      = make_mid(1.00f);
	style.Colors[ImGuiCol_PlotLines]             = make_text(0.63f);
	style.Colors[ImGuiCol_PlotLinesHovered]      = make_mid(1.00f);
	style.Colors[ImGuiCol_PlotHistogram]         = make_text(0.63f);
	style.Colors[ImGuiCol_PlotHistogramHovered]  = make_mid(1.00f);
	style.Colors[ImGuiCol_TextSelectedBg]        = make_mid(0.43f);
	style.Colors[ImGuiCol_ModalWindowDimBg]      = make_bg(0.73f);
	style.Colors[ImGuiCol_Tab]                   = make_bg(0.40f);
	style.Colors[ImGuiCol_TabHovered]            = make_high(1.00f);
	style.Colors[ImGuiCol_TabActive]             = make_mid(1.00f);
	style.Colors[ImGuiCol_TabUnfocused]          = make_bg(0.40f);
	style.Colors[ImGuiCol_TabUnfocusedActive]    = make_bg(0.70f);
	style.Colors[ImGuiCol_DockingPreview]        = make_high(0.30f);
	*/
}

VkCommandBuffer BeginSingleTimeCommands()
{
	VkCommandBufferAllocateInfo AllocateInfo;
	AllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	AllocateInfo.pNext = NULL;
	AllocateInfo.commandPool = VkRenderer.CommandPool;
	AllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	AllocateInfo.commandBufferCount = 1;

	VkCommandBuffer CommandBuffer;
	vkAllocateCommandBuffers(VkRenderer.Device, &AllocateInfo, &CommandBuffer);

	VkCommandBufferBeginInfo BeginInfo;
	BeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	BeginInfo.pNext = NULL;
	BeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
	BeginInfo.pInheritanceInfo = NULL;

	vkBeginCommandBuffer(CommandBuffer, &BeginInfo);

	return CommandBuffer;
}

void EndSingleTimeCommandBuffer(VkCommandBuffer CommandBuffer)
{
	vkEndCommandBuffer(CommandBuffer);

	VkSubmitInfo SubmitInfo;
	SubmitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	SubmitInfo.pNext = NULL;
	SubmitInfo.waitSemaphoreCount = 0;
	SubmitInfo.pWaitSemaphores = NULL;
	SubmitInfo.pWaitDstStageMask = NULL;
	SubmitInfo.commandBufferCount = 1;
	SubmitInfo.pCommandBuffers = &CommandBuffer;
	SubmitInfo.signalSemaphoreCount = 0;
	SubmitInfo.pSignalSemaphores = NULL;

	vkQueueSubmit(VkRenderer.GraphicsQueue, 1, &SubmitInfo, VK_NULL_HANDLE);
	vkQueueWaitIdle(VkRenderer.GraphicsQueue);

	vkFreeCommandBuffers(VkRenderer.Device, VkRenderer.CommandPool, 1, &CommandBuffer);
}

void InitImGui()
{
	ImGui::CreateContext();
	ImGuiIO* IO = &ImGui::GetIO();
	IO->WantCaptureMouse;
	IO->WantCaptureKeyboard;
	IO->ConfigFlags |= ImGuiConfigFlags_DockingEnable;

	DefaultFont = IO->Fonts->AddFontFromFileTTF("Data/Fonts/arialbd.ttf", 18.0);
	IconFontSmall = IO->Fonts->AddFontFromFileTTF("Data/Fonts/MagmaIcons.ttf", 18.0);
	IconFontLarge = IO->Fonts->AddFontFromFileTTF("Data/Fonts/MagmaIcons.ttf", 85.0);
	
	ImGui::StyleColorsDark();
	ImGui_ImplSDL2_InitForVulkan(Window);
	
	ImGui_ImplVulkan_InitInfo InitInfo;
	InitInfo.DescriptorPool = VkRenderer.DynamicDescriptorPools.DescriptorPools[ImGuiDescriptorPool.DescriptorPool];
	InitInfo.Device = VkRenderer.Device;
	InitInfo.RenderPass = VkRenderer.RenderPasses[SwapChainRenderPass];
	InitInfo.PhysicalDevice = VkRenderer.PhysicalDevice;
	InitInfo.ImageCount = MAX_FRAMES_IN_FLIGHT;

	ImGui_ImplVulkan_Init(&InitInfo);

	VkCommandBuffer ImCommandBuffer = BeginSingleTimeCommands();
	ImGui_ImplVulkan_CreateFontsTexture(ImCommandBuffer);
	EndSingleTimeCommandBuffer(ImCommandBuffer);

	vkDeviceWaitIdle(VkRenderer.Device);
	ImGui_ImplVulkan_DestroyFontUploadObjects();
	
	/*
	ImGui_ImplVulkan_InitInfo InitInfo;
	InitInfo.Instance = VkRenderer.Instance;
	InitInfo.PhysicalDevice = VkRenderer.PhysicalDevice;
	InitInfo.Device = VkRenderer.Device;
	InitInfo.QueueFamily = 0;
	InitInfo.Queue = NULL;
	InitInfo.PipelineCache = NULL;
	InitInfo.DescriptorPool = ImGuiPool;
	InitInfo.Subpass;
	InitInfo.MinImageCount = 2;
	InitInfo.ImageCount = VkRenderer.SwapChainImageCount;
	InitInfo.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
	InitInfo.Allocator = NULL;
	InitInfo.CheckVkResultFn = NULL;

	ImGui_ImplVulkan_Init(&InitInfo, VkRenderer.RenderPasses[SwapChainRenderPass]);
	
	VkCommandBuffer ImCommandBuffer = VkBeginSingleTimeCommands();
	ImGui_ImplVulkan_CreateFontsTexture(ImCommandBuffer);
	VkEndSingleTimeCommandBuffer(ImCommandBuffer);

	vkDeviceWaitIdle(VkRenderer.Device);
	ImGui_ImplVulkan_DestroyFontUploadObjects();
	*/
	SetStyleImGui();	
}

void DestroyImGui()
{
	vkDeviceWaitIdle(VkRenderer.Device);
	ImGui_ImplVulkan_Shutdown();
	
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();
}

void ImGuiEvent()
{
	ImGui_ImplSDL2_ProcessEvent(&Event);
}