void EngineInitEditor()
{
	InitLua();
	
}

void EngineDestroyEditor()
{
	free(Entities);
	DeInitLua();
}

int32_t LastMouseX = 0;
int32_t LastMouseY = 0;
int32_t LastWindowX = 0;
int32_t LastWindowY = 0;
bool WinBarMouseState = false;

bool WinResizeMouseState = false;

bool EditorBarButtonPressed = false;

void EditorWindowBar()
{
	ImGuiWindow* window = ImGui::GetCurrentWindow();
	ImRect Rect = window->MenuBarRect();

	int32_t MouseGX;
	int32_t MouseGY;
	uint32_t MouseState = SDL_GetGlobalMouseState(&MouseGX, &MouseGY);

	int32_t MouseX;
	int32_t MouseY;
	SDL_GetMouseState(&MouseX, &MouseY);

	if (MouseState == 1 && WinBarMouseState == 0 &&
		MouseX > Rect.Min.x && MouseX < Rect.Max.x &&
		MouseY > Rect.Min.y && MouseY < Rect.Max.y)
		WinBarMouseState = 1;

	if (MouseState == 0)
		WinBarMouseState = 0;

	if (WinBarMouseState == 1)
	{
		int32_t x = MouseGX - LastMouseX + LastWindowX;
		int32_t y = MouseGY - LastMouseY + LastWindowY;

		if (FullScreen)
		{
			if (LastMouseX != MouseGX && LastMouseY != MouseGY)
				PushEventSDL(SDL_KEYDOWN, SDLK_F11);
		}
		else
		{
			ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeAll);
			SDL_SetWindowPosition(Window, x, y);
		}
			
	}
	else
	{
		SDL_GetWindowPosition(Window, &LastWindowX, &LastWindowY);
		LastMouseX = MouseGX;
		LastMouseY = MouseGY;
	}

	if (MouseState == 1 && WinResizeMouseState == 0 &&
		MouseX > Rect.Min.x && MouseX < Rect.Max.x &&
		MouseY > Rect.Min.y && MouseY < Rect.Max.y)
		WinResizeMouseState = 1;

	if (MouseState == 0)
		WinResizeMouseState = 0;

	if (WinResizeMouseState == 1)
	{

	}
}

bool EditorOpenedSettingsWindow = false;
void EditorSettingsWindow()
{
	if (EditorOpenedSettingsWindow)
	{
		ImGui::Begin("Settings", &EditorOpenedSettingsWindow);
		{
			ImGui::BeginChild("##preferences", ImVec2(0, 0), false, ImGuiWindowFlags_None);

			const float total_width = ImGui::GetWindowWidth() - ImGui::GetStyle().ScrollbarSize - ImGui::GetStyle().WindowPadding.x * 2.0f;
			const float first_column_width = total_width * 0.25f;

			ImGui::Columns(2, "##preferences_columns", true);

			// First column
		//	ImGui::SetColumnWidth(0, first_column_width);

			if (ImGui::Button("Textures##ES-Tex", ImVec2(-1, 0))) {
				// Handle button 1 click
			}
			if (ImGui::Button("Model Importer##ES-Mim", ImVec2(-1, 0))) {
				// Handle button 2 click
			}
			if (ImGui::Button("Materials##ES-Mat", ImVec2(-1, 0))) {
				// Handle button 2 click
			}
			if (ImGui::Button("Themes##ES-The", ImVec2(-1, 0))) {
				// Handle button 3 click
			}
			if (ImGui::Button("Other##ES-Oth", ImVec2(-1, 0))) {
				// Handle button 3 click
			}

			// Second column
			ImGui::NextColumn();
			ImGui::Text("Second column");

			ImGui::Columns(1);

			ImGui::EndChild();

			/*
			ImGui::BeginChild("##preferences", ImVec2(0, 0), false, ImGuiWindowFlags_NoDecoration);

			ImGui::Columns(2, "##preferences_columns", false);
		

			
			const float button_width = ImGui::GetColumnWidth() - ImGui::GetStyle().ItemSpacing.x;
			if (ImGui::Button("Button 1", ImVec2(button_width, 0))) {
				// Handle button 1 click
			}
			if (ImGui::Button("Button 2", ImVec2(button_width, 0))) {
				// Handle button 2 click
			}
			if (ImGui::Button("Button 3", ImVec2(button_width, 0))) {
				// Handle button 3 click
			}

			ImGui::NextColumn();

			// More preferences...

			ImGui::Columns(1);

			ImGui::EndChild();
			*/

		}
		ImGui::End();
	}	
}

void EditorDrawMainMenuBar()
{
	ImGuiIO* IO = &ImGui::GetIO();
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			EditorBarButtonPressed = true;
			if (ImGui::MenuItem("Open Scene", "STRG+O"))
			{
			}

			if (ImGui::MenuItem("Save Scene", "STRG+S"))
			{
			}

			if (ImGui::MenuItem("New Scene", "STRG+N"))
			{
			}

			ImGui::EndMenu();
		}

	//	if (ImGui::BeginMenu("Settings"))
	//	{
	//		EditorBarButtonPressed = true;
	//		ImGui::DragFloat("Font Size", &IO->FontGlobalScale, 0.005, 0.1, 5.0, "%.2f", ImGuiSliderFlags_AlwaysClamp);
	//		
	//	//	ImGui::MenuItem("Texture Settings");
	//		ImGui::EndMenu();
	//	}
		if (ImGui::MenuItem("Settings"))
			EditorOpenedSettingsWindow = true;

		ImGui::PushFont(IconFontSmall);
		ImGui::SetCursorPosX((WindowWidth * 0.5));
		static char* ButtonType = (char*)"G";
		ImGui::PushID("Play Button");
		if (ImGui::Button(ButtonType))
		{
			EditorBarButtonPressed = true;
			GameMode = !GameMode;
			if (GameMode)
				ButtonType = (char*)"L";
			else
				ButtonType = (char*)"G";
			LuaOnStart = true;
		}		
		ImGui::PopID();
		ImGui::PopFont();

		ImGui::PushFont(IconFontExt);
		{
			ImGuiStyle* Style = &ImGui::GetStyle();

			const ImVec2 LableSize = ImGui::CalcTextSize("a", NULL, true);
			ImVec2 ButtonSize = ImGui::CalcItemSize(ImVec2(0, 0), LableSize.x + Style->FramePadding.x * 2.0f, LableSize.y + Style->FramePadding.y * 2.0f);

			ImGui::SetCursorPosX((float)WindowWidth - (Style->ItemSpacing.x * 3) - (ButtonSize.x * 3));
			ImGui::PushStyleColor(ImGuiCol_Button, Style->Colors[ImGuiCol_MenuBarBg]);
			if (ImGui::Button("r"))
			{
				EditorBarButtonPressed = true;
				ForceResizeEvent = true;
				SDL_MinimizeWindow(Window);
			}
			ImGui::PopStyleColor(1);
			ImGui::SameLine();
			ImGui::PushStyleColor(ImGuiCol_Button, Style->Colors[ImGuiCol_MenuBarBg]);
			char Icon[4];
			if (FullScreen)
				strcpy(Icon, "W");
			else
				strcpy(Icon, "V");

			if (ImGui::Button(Icon))
			{
				EditorBarButtonPressed = true;
				PushEventSDL(SDL_KEYDOWN, SDLK_F11);
			}
			ImGui::PopStyleColor(1);
			ImGui::SameLine();
			ImGui::PushStyleColor(ImGuiCol_Button, Style->Colors[ImGuiCol_MenuBarBg]);
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 1.0, 0.0, 0.0, 1.0f });
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.6, 0.0, 0.0, 1.0f });
			if (ImGui::Button("'"))
			{
				EditorBarButtonPressed = true;
				PushEventSDL(SDL_QUIT, 0);
				printf("Hey there\n");
			}
			ImGui::PopStyleColor(3);
		}
		ImGui::PopFont();

		if (!EditorBarButtonPressed)
			EditorWindowBar();

		EditorBarButtonPressed = false;

		ImGui::EndMainMenuBar();
	}
}

inline void ImGuiSetPosPaddingX(float Pos)
{
	ImGui::SetCursorPosX(Pos - GImGui->Style.FramePadding.x);
}

void EngineDrawEditor()
{
	ImGui_ImplVulkan_NewFrame();
	ImGui_ImplSDL2_NewFrame();
	ImGui::NewFrame();

	ImGuiIO* IO = &ImGui::GetIO();

	EditorDrawMainMenuBar();
	EditorSettingsWindow();
	EditorDrawScene();
	EditorEntities();
	EditorInspector();
	EditorAssetBrowser();

	//Make this code block shorter/ more readable
	if (ifd::FileDialog::Instance().IsDone("LoadDefaultModel")) 
	{
		if (ifd::FileDialog::Instance().HasResult())
			for (uint32_t i = 0; i < ifd::FileDialog::Instance().GetResults().size(); i++)
				LoadModel(0, (const char*)ifd::FileDialog::Instance().GetResults()[i].u8string().c_str());
		ifd::FileDialog::Instance().Close();
	}
	if (ifd::FileDialog::Instance().IsDone("LoadSmoothModel"))
	{
		if (ifd::FileDialog::Instance().HasResult())
			for (uint32_t i = 0; i < ifd::FileDialog::Instance().GetResults().size(); i++)
				LoadModel(WAVE_GEN_SMOOTH_NORMALS, (const char*)ifd::FileDialog::Instance().GetResults()[i].u8string().c_str());
		ifd::FileDialog::Instance().Close();
	}
	if (ifd::FileDialog::Instance().IsDone("LoadFlatModel"))
	{
		if (ifd::FileDialog::Instance().HasResult())
			for (uint32_t i = 0; i < ifd::FileDialog::Instance().GetResults().size(); i++)
				LoadModel(WAVE_FORCE_GEN_NORMALS, (const char*)ifd::FileDialog::Instance().GetResults()[i].u8string().c_str());
		ifd::FileDialog::Instance().Close();
	}
	if (ifd::FileDialog::Instance().IsDone("LoadTexture"))
	{
		if (ifd::FileDialog::Instance().HasResult())
			for (uint32_t i = 0; i < ifd::FileDialog::Instance().GetResults().size(); i++)
				AddTexture((char*)ifd::FileDialog::Instance().GetResults()[i].u8string().c_str(), true);
		ifd::FileDialog::Instance().Close();
	}
	if (ifd::FileDialog::Instance().IsDone("LoadAnimation"))
	{
		if (ifd::FileDialog::Instance().HasResult())
			for (uint32_t i = 0; i < ifd::FileDialog::Instance().GetResults().size(); i++)
				AddAnimation((char*)ifd::FileDialog::Instance().GetResults()[i].u8string().c_str(), 1024, 1024);
		ifd::FileDialog::Instance().Close();
	}
	if (ifd::FileDialog::Instance().IsDone("LoadScript"))
	{
		if (ifd::FileDialog::Instance().HasResult())
			AddScript((const char*)ifd::FileDialog::Instance().GetResult().u8string().c_str());
		ifd::FileDialog::Instance().Close();
	}
	if (ifd::FileDialog::Instance().IsDone("SaveScript"))
	{
		if (ifd::FileDialog::Instance().HasResult())
			SaveScript((const char*)ifd::FileDialog::Instance().GetResult().u8string().c_str());
		ifd::FileDialog::Instance().Close();
	}
	
	ImGui::Begin("Debug");
	{
		uint32_t VertexCount = 0;
		uint32_t IndexCount = 0;

		Mutex.lock();
		for (uint32_t i = 0; i < EntityCount; i++)
		{
			if (Entities[i].UsedComponents[COMPONENT_TYPE_MESH] == true)
			{
				SceneMesh* Mesh = (SceneMesh*)CMA_GetAt(&SceneMeshes, Entities[i].Mesh.MeshIndex);
				if (Mesh != NULL)
				{
					for (uint32_t j = 0; j < Mesh->MeshCount; j++)
					{
						VertexCount += Mesh->MeshData[j].VertexCount;
						IndexCount += Mesh->MeshData[j].IndexCount;
					}
				}
			}
		}
		Mutex.unlock();
			
		ImGui::Text("Scene Vertices: %d", VertexCount);
		ImGui::Text("Scene Indices: %d", IndexCount);
		ImGui::Text("Scene Triangles: %d", IndexCount / 3);
		ImGui::NewLine();

		ImGui::Text("FPS: %f MS: %f", FPS, MS);
		ImGui::Text("Shadow Render Time: %f ms", ShadowRenderingTime);
		ImGui::Text("Scene Render Time: %f ms", SceneRenderingTime);
		ImGui::Text("SwapChain Render Time: %f ms", SwapChainRenderingTime);
		ImGui::Text("Begin Frame Time: %f ms", BeginFrameTime);
		ImGui::Text("End Frame Time: %f ms", EndFrameTime);
		ImGui::Text("Frame Time: %f ms", FrameTime);
		ImGui::NewLine();

		ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(0, 255, 0, 255));
		ImGui::Text("Renderer Info:");
		ImGui::PopStyleColor();
		ImGui::Text("Swapchain Image Count: %d", VkRenderer.SwapChainImageCount);
		ImGui::Text("Image Count: %d", VkRenderer.ImageAttachments.Size);
		ImGui::Text("Render Pass Count: %d", VkRenderer.RenderPassCount);
		ImGui::Text("Pipeline Count: %d", VkRenderer.PipelineCount);
		ImGui::Text("Pipeline Count: %d", VkRenderer.PipelineLayoutCount);
		ImGui::Text("Framebuffer Count: %d", VkRenderer.FramebufferCount);
		ImGui::Text("Frames In Flight: %d", MAX_FRAMES_IN_FLIGHT);
		ImGui::Text("Current Frame: %d", VkRenderer.CurrentFrame);
		ImGui::Text("Descriptor Set Layout Count: %d", VkRenderer.DescriptorSetLayoutCount);
		ImGui::Text("Descriptor Set Count: %d", VkRenderer.DescriptorSets.Size);
		ImGui::Text("Dynamic Buffer Count: %d", VkRenderer.DynamicBuffers.Size);
		ImGui::Text("Static Buffer Count: %d", VkRenderer.StaticBuffers.Size);
		ImGui::Text("Image Count: %d", VkRenderer.Images.Size);
		ImGui::Text("Sampler Count: %d", VkRenderer.Sampler.Size);
		ImGui::Text("Max Msaa Sample Count: %d", VkRenderer.MsaaSamples);
		ImGui::Text("Used Msaa Sample Count: %d", MsaaSamples);
		ImGui::Text("Descriptor Pool Count: %d", VkRenderer.DescriptorPools.Size);
		ImGui::Text("Frame: %d", SDL_GetTicks());
		ImGui::Text("Used CPU Memory: %f MB", WaveGetUsedMemory() * 0.000001);
		ImGui::NewLine();
		ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(0, 255, 0, 255));
		ImGui::Text("GPU Info:");
		ImGui::PopStyleColor();

		ImGui::Text("GPU: %s", VkRenderer.PhysicalDeviceProperties.deviceName);
		ImGui::Text("API version: %d.%d.%d", VK_VERSION_MAJOR(VkRenderer.PhysicalDeviceProperties.apiVersion), VK_VERSION_MINOR(VkRenderer.PhysicalDeviceProperties.apiVersion), VK_VERSION_PATCH(VkRenderer.PhysicalDeviceProperties.apiVersion));
		ImGui::Text("Device ID: %d", VkRenderer.PhysicalDeviceProperties.deviceID);
		ImGui::Text("Driver Version: %d", VkRenderer.PhysicalDeviceProperties.driverVersion);
		ImGui::Text("Max Framebuffer Color Sample Count: %d", VkRenderer.PhysicalDeviceProperties.limits.framebufferColorSampleCounts);
		ImGui::Text("Max Framebuffer Depth Sample Count: %d", VkRenderer.PhysicalDeviceProperties.limits.framebufferDepthSampleCounts);
		ImGui::Text("Max Framebuffer Stencil Sample Count: %d", VkRenderer.PhysicalDeviceProperties.limits.framebufferStencilSampleCounts);
		ImGui::Text("Max Bound Descriptor Sets: %d", VkRenderer.PhysicalDeviceProperties.limits.maxBoundDescriptorSets);
		ImGui::Text("Max Color Attachments: %d", VkRenderer.PhysicalDeviceProperties.limits.maxColorAttachments);
		ImGui::Text("Max Descriptor Set Input Attachments: %d", VkRenderer.PhysicalDeviceProperties.limits.maxDescriptorSetInputAttachments);
		ImGui::Text("Max Descriptor Set Sampled Images: %d", VkRenderer.PhysicalDeviceProperties.limits.maxDescriptorSetSampledImages);
		ImGui::Text("Max Descriptor Set Samplers: %d", VkRenderer.PhysicalDeviceProperties.limits.maxDescriptorSetSamplers);
		ImGui::Text("Max Descriptor Set Uniform Buffers: %d", VkRenderer.PhysicalDeviceProperties.limits.maxDescriptorSetUniformBuffers);
		ImGui::Text("Max Descriptor Set Uniform Buffers Dynamic: %d", VkRenderer.PhysicalDeviceProperties.limits.maxDescriptorSetUniformBuffersDynamic);
		ImGui::Text("Max Fragment Input Components: %d", VkRenderer.PhysicalDeviceProperties.limits.maxFragmentInputComponents);
		ImGui::Text("Max Fragment Output Attachments: %d", VkRenderer.PhysicalDeviceProperties.limits.maxFragmentOutputAttachments);
		ImGui::Text("Max Framebuffer Width: %d", VkRenderer.PhysicalDeviceProperties.limits.maxFramebufferWidth);
		ImGui::Text("Max Framebuffer Height: %d", VkRenderer.PhysicalDeviceProperties.limits.maxFramebufferHeight);
		ImGui::Text("Max Memory Allocation Count: %d", VkRenderer.PhysicalDeviceProperties.limits.maxMemoryAllocationCount);
		ImGui::Text("Max Push Constants Size: %d", VkRenderer.PhysicalDeviceProperties.limits.maxPushConstantsSize);
		ImGui::Text("Max Viewport Dimensions: %d %d", VkRenderer.PhysicalDeviceProperties.limits.maxViewportDimensions[0], VkRenderer.PhysicalDeviceProperties.limits.maxViewportDimensions[1]);
		ImGui::Text("Max Viewports: %d", VkRenderer.PhysicalDeviceProperties.limits.maxViewports);
	}
	ImGui::End();

	ImGui::Begin("Scene Settings");
	{
		ImGui::ColorEdit3("Clear Color", (float*)&ClearColor);
		ImGui::DragFloat3("Light Direction", (float*)&SceneFragmentUBO.LightDirection, 0.01, -10000.0, 10000.0);
		ImGui::SliderFloat("Exposure", &SceneFragmentUBO.Exposure, 0.01, 10.0);
		ImGui::SliderFloat("Gamma", &SceneFragmentUBO.Gamma, 0.01, 10.0);
		{
			const char* ScaleOptions[] = { "0 %", "10 %", "20 %", "30 %", "40 %", "50 %", "60 %", "70 %", "80 %", "90 %", "100 %" };
			if (ImGui::BeginCombo("Scene Scaling", ScaleOptions[SceneScaling / 10]))
			{
				for (uint32_t i = 1; i < 11; i++)
					if (ImGui::Button(ScaleOptions[i]))
					{
						SceneScaling = i * 10;
						ForceResizeEvent = true;

						PushEventSDL(0, 0);
					}
						
				ImGui::EndCombo();
			}
		}
		
		if (ImGui::Button("Reset Settings"))
			ResetSceneSettings();
		/*
		uint8_t Index = 0;
		switch (MsaaSamples)
		{
		case 4:
			Index = 1;
			break;
		case 8:
			Index = 2;
			break;
		default:
			break;
		}

		const char* SampleOptions[] = { "Msaa x2", "Msaa x4", "Msaa x8" };
		if (ImGui::BeginCombo("Msaa Samples", SampleOptions[Index]))
		{
			for (uint32_t i = 0; i < 3; i++)
			{
				if (ImGui::Button(SampleOptions[i]))
				{
					switch (i)
					{
					case 0:
						MsaaSamples = 2;
						break;
					case 1:
						MsaaSamples = 4;
						break;
					case 2:
						MsaaSamples = 8;
						break;
					default:
						break;
					}
					ReloadShaders = true;
				}
			}
			ImGui::EndCombo();
		}
		*/
	//	ImGui::Checkbox("Scene Backface Culling", &SceneBackfaceCulling);

		ImGui::Checkbox("Render Shadows", &RenderShadows);

		const char* CullingOptions[] = { "No Culling", "Back Face Culling", "Front Face Culling" };
		if (ImGui::BeginCombo("Scene Face Culling", CullingOptions[SceneCullMode]))
		{
			for (uint32_t i = 0; i < SHADOW_MAP_CASCADE_COUNT; i++)
				if (ImGui::Button(CullingOptions[i]))
					SceneCullMode = i;
			ImGui::EndCombo();
		}

		if (ImGui::Button("Reload Shaders"))
			ReloadShaders = true;
		
		ImGui::Separator();
		if (ImGui::CollapsingHeader("Shadow Settings"))
		{			
			ImGuiSetPosPaddingX(33); ImGui::SliderFloat("Split lambda", &CascadeSplitLambda, 0.001, 1.0);
			ImGuiSetPosPaddingX(33); ImGui::SliderFloat("Near Clip", &CascadeNearClip, 0.0001, 2.0);
			ImGuiSetPosPaddingX(33); ImGui::SliderFloat("Far Clip", &CascadeFarClip, 10.0, 1000.0);
			ImGuiSetPosPaddingX(33); ImGui::DragInt("Shadow Resolution", &ShadowMapSizeTmp, 8.0, 128, 8192);
			ImGuiSetPosPaddingX(33);
			if (ImGui::BeginCombo("Shadow Face Culling", CullingOptions[ShadowCullMode]))
			{
				for (uint32_t i = 0; i < SHADOW_MAP_CASCADE_COUNT; i++)
					if (ImGui::Button(CullingOptions[i]))
						ShadowCullMode = i;
				ImGui::EndCombo();
			}
			ImGuiSetPosPaddingX(33);
			if (ImGui::Button("Apply"))
			{
				ShadowMapHeight = ShadowMapSizeTmp;
				ShadowMapWidth = ShadowMapHeight * SHADOW_MAP_CASCADE_COUNT;
				ReloadShaders = true;
			}

			for (uint32_t i = 0; i < SHADOW_MAP_CASCADE_COUNT; i++)
			{
				char Name[64];
				sprintf(Name, "Cascade %d", i);
				ImGui::PushID(Name);
				ImGui::SetCursorPosX(33);
				if (ImGui::CollapsingHeader(Name))
				{
					int32_t Range = SceneFragmentUBO.CascadeRange[i];
					ImGuiSetPosPaddingX(66); ImGui::SliderInt("Range", &Range, 0.0, 8.0);
					ImGuiSetPosPaddingX(66); ImGui::SliderFloat("Scale", &SceneFragmentUBO.CascadeScale[i], 0.1, 6.0);
					ImGuiSetPosPaddingX(66); ImGui::DragFloat("Bias", &SceneFragmentUBO.CascadeBias[i], 0.000001, 0.000001, 0.9, "%.6f");
					ImGuiSetPosPaddingX(66); ImGui::DragFloat("Split Depth", &Cascades[i].SplitDepth, 0.01, -1000.0, 1000.0);
					SceneFragmentUBO.CascadeRange[i] = Range;
				}
				ImGui::PopID();
			}
		}		
	}
	ImGui::End();

	UpdateLuaScripts();
}