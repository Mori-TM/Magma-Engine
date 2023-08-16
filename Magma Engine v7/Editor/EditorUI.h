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

typedef enum
{
	EDITOR_SETTING_TEXTURE = 0,
	EDITOR_SETTING_MODEL_IMPORTER = 1,
	EDITOR_SETTING_MATERIAL = 2,
	EDITOR_SETTING_THEME = 3,
	EDITOR_SETTING_CAMERA = 4,
	EDITOR_SETTING_EDITOR = 5,
	EDITOR_SETTING_OTHER = 6,
} EditorSelectableSettings;

uint32_t EditorSelectedSetting = EDITOR_SETTING_TEXTURE;

void EditorSettingsWindow()
{
	if (EditorOpenedSettingsWindow)
	{
		ImGui::Begin("Settings", &EditorOpenedSettingsWindow);
		{
			ImGui::BeginChild("##preferences", ImVec2(0, 0), false, ImGuiWindowFlags_None);

			const float total_width = ImGui::GetWindowWidth() - ImGui::GetStyle().ScrollbarSize - ImGui::GetStyle().WindowPadding.x * 2.0f;
			const float first_column_width = total_width * 0.3f;

			ImGui::Columns(2, "##preferences_columns", true);

			ImGui::SetColumnWidth(0, first_column_width);

			// First column
			// 
			if (ImGui::Button("Textures##ES-Tex", ImVec2(-1, 0)))
				EditorSelectedSetting = EDITOR_SETTING_TEXTURE;
			if (ImGui::Button("Model Importer##ES-Mim", ImVec2(-1, 0)))
				EditorSelectedSetting = EDITOR_SETTING_MODEL_IMPORTER;
			if (ImGui::Button("Materials##ES-Mat", ImVec2(-1, 0)))
				EditorSelectedSetting = EDITOR_SETTING_MATERIAL;
			if (ImGui::Button("Themes##ES-The", ImVec2(-1, 0)))
				EditorSelectedSetting = EDITOR_SETTING_THEME;
			if (ImGui::Button("Camera##ES-Cam", ImVec2(-1, 0)))
				EditorSelectedSetting = EDITOR_SETTING_CAMERA;
			if (ImGui::Button("Editor##ES-Edi", ImVec2(-1, 0)))
				EditorSelectedSetting = EDITOR_SETTING_EDITOR;
			if (ImGui::Button("Other##ES-Oth", ImVec2(-1, 0)))
				EditorSelectedSetting = EDITOR_SETTING_OTHER;

			
		
			
			/*
			if (ImGui::Button("Textures##ES-Tex", ImVec2(-1, 0))) 
			{
				//Drop Down-Compression, no, bc1, bc4, bc7
				//Bool-Comprssion software mipmap generation(slow)

			}
			if (ImGui::Button("Model Importer##ES-Mim", ImVec2(-1, 0)))
			{
				//Bool-Load Albedo
				//Bool-Load Normal
				//Bool-Load Metallic
				//Bool-Load Roughness
				//Bool-Load Occlusion
				//Drop Down- Gen Normals, Gen Smooth Normals, flip uvs

			}
			if (ImGui::Button("Materials##ES-Mat", ImVec2(-1, 0))) 
			{
				//Input Text-Default Material Name
				//Color Picker-Default Color
				//float slider-Dfault metallic srength
				//float slider-Dfault roughness srength
				//float slider-Dfault occlusion srength
			}
			if (ImGui::Button("Themes##ES-The", ImVec2(-1, 0))) 
			{
				//Drop down- Default themes
				//Imgui theme editor
			}
			if (ImGui::Button("Camera##ES-Cam", ImVec2(-1, 0))) 
			{
				//float drag-Camera speed
				//float drag-Camera fov
			}
			if (ImGui::Button("Editor##ES-Edi", ImVec2(-1, 0))) 
			{
				//vec3-Font Color
				//vec3-Icon Color
			}
			if (ImGui::Button("Other##ES-Oth", ImVec2(-1, 0))) 
			{
				//bool-Frustum culling
			}
			*/

			// Second column
			ImGui::NextColumn();
			
			switch (EditorSelectedSetting)
			{
			case EDITOR_SETTING_TEXTURE:
				ImGui::Checkbox("Use BC1 Compression", &LoadTextureCompressed);
				ImGui::Checkbox("Use HQ Compression", &TextureCompressedHQ);
				ImGui::Checkbox("Generate MipMaps", &GenerateMipMaps);
				if (GenerateMipMaps)
				{
					ImGui::Checkbox("Use Custom Mip Count", &UseCustomMipLevels);
					if (UseCustomMipLevels)
					{
						ImGui::SliderInt("Custom Mip Count", (int32_t*)&CustomMipLevels, 2, 32);
					}
				}
				else
					UseCustomMipLevels = false;
				//	if (ImGui::BeginCombo("Compression Methode"))
				//	{
				//	//	for (uint32_t i = 0; i < SceneMeshes.Size; i++)
				//	//	{
				//	//		Mesh = (SceneMesh*)CMA_GetAt(&SceneMeshes, i);
				//	//		if (Mesh != NULL)
				//	//		{
				//	//			if (ImGui::Button(Mesh->Name))
				//	//			{
				//	//				Entities[SelectedEntity].Mesh.MeshIndex = i;
				//	//				strcpy(Entities[SelectedEntity].Mesh.Name, Mesh->Name);
				//	//			}
				//	//		}
				//	//	}
				//		ImGui::EndCombo();
				//	}
				break;
			case EDITOR_SETTING_MODEL_IMPORTER:
				ImGui::Checkbox("Import Albedo Texture", &ModelLoadAlbedo);
				ImGui::Checkbox("Import Normal Texture", &ModelLoadNormal);
				ImGui::Checkbox("Import Metallic Texture", &ModelLoadMetallic);
				ImGui::Checkbox("Import Roughness Texture", &ModelLoadRoughness);
				ImGui::Checkbox("Import Occlusion Texture", &ModelLoadOcclusion);
				ImGui::ColorEdit4("Default Color", (float*)&ModelColor);
				ImGui::SliderFloat("Default Metallic Strength", &ModelMetallic, 0.0, 1.0);
				ImGui::SliderFloat("Default Roughness Strength", &ModelRoughness, 0.0, 1.0);
				ImGui::SliderFloat("Default Occlusion Strength", &ModelOcclusion, 0.0, 1.0);
				ImGui::Separator();
				ImGui::Checkbox("Import Materials", &ModelLoadMaterials);
				ImGui::Checkbox("Generate Flat Normals", &ModelGenFlatNormals);
				ImGui::Checkbox("Generate Smooth Normals", &ModelGenSmoothNormals);
				ImGui::Checkbox("Flip UVs", &ModelFlipUVs);
				break;
			case EDITOR_SETTING_MATERIAL:
				ImGui::InputText("Default Name", MaterialName, MAX_CHAR_NAME_LENGTH);
				ImGui::ColorEdit4("Default Color", (float*)&MaterialColor);
				ImGui::SliderFloat("Default Metallic Strength", &MaterialMetallic, 0.0, 1.0);
				ImGui::SliderFloat("Default Roughness Strength", &MaterialRoughness, 0.0, 1.0);
				ImGui::SliderFloat("Default Occlusion Strength", &MaterialOcclusion, 0.0, 1.0);
				break;
			case EDITOR_SETTING_THEME:
				//Drop down- Default themes
				//Imgui theme editor
				
				if (ImGui::Combo("Theme##Selector", &UIStyleIndex, "Dark\0Light\0Classic\0Magma0\0Magma1\0Magma2\0Magma3\0"))
				{
					switch (UIStyleIndex)
					{
					case 0: ImGui::StyleColorsDark(); break;
					case 1: ImGui::StyleColorsLight(); break;
					case 2: ImGui::StyleColorsClassic(); break;
					default: SetStyleImGui(UIStyleIndex - 3); break;
					}
				}
				ImGui::ShowStyleEditor();
				break;
			case EDITOR_SETTING_CAMERA:
				ImGui::DragFloat("Camera Speed", &CameraInfo.Speed, 0.01, 0.01, 10.0);
				ImGui::DragFloat("Camera FOV", &CameraNormalFOV, 0.1, 5.0, 179.0);
				ImGui::DragFloat("Camera Zoom FOV", &CameraZoomFOV, 0.01, 5.0, 179.0);
				break;
			case EDITOR_SETTING_EDITOR:
				//vec3-Font Color
				//vec3-Icon Color
				
				ImGui::DragFloat("Font Size", &GImGui->IO.FontGlobalScale, 0.005, 0.1, 5.0, "%.2f", ImGuiSliderFlags_AlwaysClamp);
				break;
			case EDITOR_SETTING_OTHER:
				//bool-Frustum culling
				break;

			default:
				break;
			}

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
			//	printf("Hey there\n");
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
//	if (ifd::FileDialog::Instance().IsDone("LoadDefaultModel"))
//	{
//		if (ifd::FileDialog::Instance().HasResult())
//			for (uint32_t i = 0; i < ifd::FileDialog::Instance().GetResults().size(); i++)
//				LoadModel(0, (const char*)ifd::FileDialog::Instance().GetResults()[i].u8string().c_str());
//		ifd::FileDialog::Instance().Close();
//		ImGui::SetWindowFocus("Mesh Inspector");
//	}
//	if (ifd::FileDialog::Instance().IsDone("LoadSmoothModel"))
//	{
//		if (ifd::FileDialog::Instance().HasResult())
//			for (uint32_t i = 0; i < ifd::FileDialog::Instance().GetResults().size(); i++)
//				LoadModel(WAVE_GEN_SMOOTH_NORMALS, (const char*)ifd::FileDialog::Instance().GetResults()[i].u8string().c_str());
//		ifd::FileDialog::Instance().Close();
//		ImGui::SetWindowFocus("Mesh Inspector");
//	}
//	if (ifd::FileDialog::Instance().IsDone("LoadFlatModel"))
//	{
//		if (ifd::FileDialog::Instance().HasResult())
//			for (uint32_t i = 0; i < ifd::FileDialog::Instance().GetResults().size(); i++)
//				LoadModel(WAVE_FORCE_GEN_NORMALS, (const char*)ifd::FileDialog::Instance().GetResults()[i].u8string().c_str());
//		ifd::FileDialog::Instance().Close();
//		ImGui::SetWindowFocus("Mesh Inspector");
//	}
	if (ifd::FileDialog::Instance().IsDone("LoadModel"))
	{
		if (ifd::FileDialog::Instance().HasResult())
			for (uint32_t i = 0; i < ifd::FileDialog::Instance().GetResults().size(); i++)
				AddModel(0, (const char*)ifd::FileDialog::Instance().GetResults()[i].u8string().c_str());
		ifd::FileDialog::Instance().Close();
		ImGui::SetWindowFocus("Mesh Inspector");
	}
	if (ifd::FileDialog::Instance().IsDone("LoadTexture"))
	{
		if (ifd::FileDialog::Instance().HasResult())
			for (uint32_t i = 0; i < ifd::FileDialog::Instance().GetResults().size(); i++)
				AddTexture((char*)ifd::FileDialog::Instance().GetResults()[i].u8string().c_str(), true);
		ifd::FileDialog::Instance().Close();
		ImGui::SetWindowFocus("Texture Inspector");
	}
	if (ifd::FileDialog::Instance().IsDone("LoadAnimation"))
	{
		if (ifd::FileDialog::Instance().HasResult())
			for (uint32_t i = 0; i < ifd::FileDialog::Instance().GetResults().size(); i++)
				AddAnimation((char*)ifd::FileDialog::Instance().GetResults()[i].u8string().c_str(), 1024, 1024);
		ifd::FileDialog::Instance().Close();
		ImGui::SetWindowFocus("Animation Inspector");
	}
	if (ifd::FileDialog::Instance().IsDone("LoadScript"))
	{
		if (ifd::FileDialog::Instance().HasResult())
			AddScript((const char*)ifd::FileDialog::Instance().GetResult().u8string().c_str());
		ifd::FileDialog::Instance().Close();
		ImGui::SetWindowFocus("Script Inspector");
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

						if (Mesh->MeshData[j].IndexCount != 0)
							IndexCount += Mesh->MeshData[j].IndexCount;
						else
							IndexCount += Mesh->MeshData[j].VertexCount;
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
		ImGui::Text("SSR Render Time: %f ms", SSRRenderingTime);
		ImGui::Text("SwapChain Render Time: %f ms", SwapChainRenderingTime);
		ImGui::Text("Render Time: %f ms", ShadowRenderingTime + SceneRenderingTime + SSRRenderingTime + SwapChainRenderingTime);
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
		ImGui::Text("Pipeline Layout Count: %d", VkRenderer.PipelineLayoutCount);
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
	//	ImGui::DragFloat3("Light Direction", (float*)&SceneFragmentUBO.LightDirection, 0.01, -10000.0, 10000.0);
		ImGui::SliderFloat("Ambient Multiplier", &SceneFragmentUBO.AmbientMultiplier, 0.01, 1.0);
		ImGui::SliderFloat("Exposure", &SceneFragmentUBO.Exposure, 0.01, 10.0);
		ImGui::SliderFloat("Gamma", &SceneFragmentUBO.Gamma, 0.01, 10.0);
		{
			const char* ScaleOptions[] = { "0 %", "10 %", "20 %", "30 %", "40 %", "50 %", "60 %", "70 %", "80 %", "90 %", "100 %", "110 %", "120 %", "130 %", "140 %", "150 %", "160 %", "170 %", "180 %", "190 %", "200 %" };
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

	//	ImGui::Checkbox("Render Shadows", &RenderShadows);
		ImGui::Checkbox("Render SSAO", &RenderSSAO);
		if (ImGui::Checkbox("Render SSAO Blur", &RenderSSAOBlur))
		{
			ForceResizeEvent = true;
			PushEventSDL(0, 0);
		}			
		ImGui::Checkbox("Render SSR", &RenderSSR);
		ImGui::Checkbox("Render FXAA", &RenderFXAA);

		const char* CullingOptions[] = { "No Culling", "Back Face Culling", "Front Face Culling" };
		if (ImGui::BeginCombo("Scene Face Culling", CullingOptions[GBufferCullMode]))
		{
			for (uint32_t i = 0; i < SHADOW_MAP_CASCADE_COUNT; i++)
				if (ImGui::Button(CullingOptions[i]))
					GBufferCullMode = i;
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

	ImGui::Begin("Render Stages", NULL, ImGuiWindowFlags_NoScrollbar);
	{
		ImVec2 ImageSize = ImGui::GetWindowSize();
		float Aspect = (float)SceneWidth / (float)SceneHeight;
		ImageSize.y = ImageSize.x / Aspect;
		
		ImGui::Checkbox("Render Debug View", &RenderDebugDescriptorSet);

		for (uint32_t i = 0; i < ARRAY_SIZE(DebugDescriptorSets); i++)
		{
			ImGui::Text(DebugAttachmentNames[i]);
	//		ImGui::PushID(DebugDescriptorSets[i]);
			if (ImGui::ImageButton(&GetDescriptorSet(DebugDescriptorSets[i])[0], i == 0 ? ImVec2(ImageSize.x, ImageSize.x / 3) : ImageSize, ImVec2(0, 0), ImVec2(1, 1), 0))
				SceneRenderDescriptorSet = DebugDescriptorSets[i];

			ImGui::NewLine();
		//	ImGui::Image(&GetDescriptorSet(DebugDescriptorSets[i])[0], i == 0 ? ImVec2(ImageSize.x, ImageSize.x / 3) : ImageSize);
//			ImGui::PopID();
		}

		/*
		ImGui::Text("SSAO Pass");
		ImGui::PushID("SSAO Pass Texture");
		if (ImGui::ImageButton(&GetDescriptorSet(SSAOBlurDescriptorSet)[0], ImageSize, ImVec2(0, 0), ImVec2(1, 1), 0))
			SceneRenderDescriptorSet = SSAOBlurDescriptorSet;
		ImGui::PopID();

		ImGui::Text("Scene Pass");
		ImGui::PushID("Scene Pass Texture");
		if (ImGui::ImageButton(&GetDescriptorSet(SceneOutputDescriptorSet)[0], ImageSize, ImVec2(0, 0), ImVec2(1, 1), 0))
			SceneRenderDescriptorSet = SceneOutputDescriptorSet;
		ImGui::PopID();

		ImGui::Text("SSR Pass");
		ImGui::PushID("SSR Pass Texture");
		if (ImGui::ImageButton(&GetDescriptorSet(SSROutputDescriptorSet)[0], ImageSize, ImVec2(0, 0), ImVec2(1, 1), 0))
			SceneRenderDescriptorSet = SSROutputDescriptorSet;
		ImGui::PopID();

		ImGui::Text("FXAA Output");
		ImGui::PushID("FXAA Output Texture");
		if (ImGui::ImageButton(&GetDescriptorSet(FXAADescriptorSet)[0], ImageSize, ImVec2(0, 0), ImVec2(1, 1), 0))
			SceneRenderDescriptorSet = FXAADescriptorSet;
		ImGui::PopID();
		*/
	}
	ImGui::End();

	UpdateLuaScripts();
}