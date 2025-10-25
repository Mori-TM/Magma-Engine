int32_t LastMouseX = 0;
int32_t LastMouseY = 0;
int32_t LastWindowX = 0;
int32_t LastWindowY = 0;
bool WinBarMouseState = false;

bool WinResizeMouseState = false;

bool EditorBarButtonPressed = false;

//bool AABBCollision2D(int32_t x, int32_t y, int32_t MinX, int32_t Min)

typedef enum
{
	EDITOR_DRAGGING_STATE_FALSE = 0,
	EDITOR_DRAGGING_STATE_TRUE,
	EDITOR_DRAGGING_STATE_RELEASED,
	EDITOR_DRAGGING_STATE_COUNT,
} EditorDraggingState;

/*
EditorDraggingState EditorIsDragging(ImRect Rect, bool* IsDragging)
{
	if (ImGui::IsMouseHoveringRect(Rect.Min, Rect.Max) || *IsDragging)
	{
		if (ImGui::IsMouseDragging(ImGuiMouseButton_Left))
		{
			*IsDragging = true;
		//	ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeAll);
		//	SDL_SetWindowPosition(Window, MouseGX - MouseX, MouseGY - MouseY);
			return EDITOR_DRAGGING_STATE_TRUE;
		}
		else
		{
		//	SDL_GetMouseState(&MouseX, &MouseY);
			return EDITOR_DRAGGING_STATE_RELEASED;
		}
	}



	if (ImGui::IsMouseReleased(ImGuiMouseButton_Left))
	{
		*IsDragging = false;
		//	ImGui::ReleaseMouseCapture();
	}

	return EDITOR_DRAGGING_STATE_FALSE;
}
*/

void EditorWindowBar()
{
	ImVec2 Rect[2];
	ImGuiGetMenuBarRectCurrentWindow(Rect);


	float MouseGX;
	float MouseGY;
	uint32_t MouseState = SDL_GetGlobalMouseState(&MouseGX, &MouseGY);

	static float MouseX = 0;
	static float MouseY = 0;
	static bool IsDragging = false;
	static bool IsDoubleClicked = false;
	/*
	if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
		IsDoubleClicked = true;

	const Uint8* State = SDL_GetKeyboardState(NULL);
	
//	EditorDraggingState State = EditorIsDragging(Rect, &IsDragging);
	if (IsDoubleClicked == true &&
		ImGui::IsMouseDragging(ImGuiMouseButton_Left) &&
		State[SDL_SCANCODE_LCTRL])
	{
		ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeAll);
		SDL_SetWindowPosition(Window, MouseGX - MouseX, MouseGY - MouseY);

		if (FullScreen)
			PushEventSDL(SDL_KEYDOWN, SDLK_F11);
	}
	else
	{
		SDL_GetMouseState(&MouseX, &MouseY);
		
	}
	if (ImGui::IsMouseReleased(ImGuiMouseButton_Left))
		IsDoubleClicked = false;


	ImRect Rect;
	Rect = { (float)MainHWnd.Width - 25, (float)MainHWnd.Height - 25, (float)MainHWnd.Width + 25, (float)MainHWnd.Height + 25 };

	EditorDraggingState DraggingState = EditorIsDragging(Rect, &IsDragging);
	if (DraggingState == EDITOR_DRAGGING_STATE_TRUE)
	{
		ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeNESW);
	}

	if (DraggingState == EDITOR_DRAGGING_STATE_RELEASED)
	{
	//	SDL_GetMouseState(&MouseX, &MouseY);
	}
	*/
//	if (!ImGui::IsMouseDragging(ImGuiMouseButton_Left))
//		IsDoubleClicked = false;
	
	
	if (ImGui::IsMouseHoveringRect(Rect[0], Rect[1]) || IsDragging)
	{
		
		{
			if (ImGui::IsMouseDragging(ImGuiMouseButton_Left))
			{
				IsDragging = true;
				ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeAll);

				if (FullScreen)
					ForceFullScreenEvent = true;
				else
					SDL_SetWindowPosition(MainHWnd.Wnd, MouseGX - MouseX, MouseGY - MouseY);
			}
			else
			{
				SDL_GetMouseState(&MouseX, &MouseY);
			}
		}
		
	}
	
	

	if (ImGui::IsMouseReleased(ImGuiMouseButton_Left))
	{
		IsDragging = false;
	//	ImGui::ReleaseMouseCapture();
	}
	

	/*
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
	*/
//	int32_t HitboxSize = 5;
//	if (MouseX - HitboxSize )
}

bool EditorOpenedSettingsWindow = false;

typedef enum
{
	EDITOR_SETTING_TEXTURE = 0,
	EDITOR_SETTING_MODEL_IMPORTER,
	EDITOR_SETTING_MATERIAL,
	EDITOR_SETTING_THEME,
	EDITOR_SETTING_CAMERA,
	EDITOR_SETTING_EDITOR,
	EDITOR_SETTING_OTHER,
	EDITOR_SETTING_COUNT
} EditorSelectableSettings;

EditorSelectableSettings EditorSelectedSetting = EDITOR_SETTING_TEXTURE;

void EditorSettingsWindow()
{
	if (!EditorOpenedSettingsWindow)
		return;
	
	ImGui::Begin("Settings", &EditorOpenedSettingsWindow);
	{
		ImGui::BeginChild("##preferences", ImVec2(0, 0), false, ImGuiWindowFlags_None);

		const float total_width = ImGui::GetWindowWidth() - ImGui::GetStyle().ScrollbarSize - ImGui::GetStyle().WindowPadding.x * 2.0f;
		const float first_column_width = total_width * 0.3f;

		ImGui::Columns(2, "##preferences_columns", true);

		ImGui::SetColumnWidth(0, first_column_width);

		// First column
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

		
		// Second column
		ImGui::NextColumn();
		
		ImGuiStyle& Style = ImGui::GetStyle();

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
			ImGui::Checkbox("Remove Redundant Materials", &ModelRedundantMaterials);
			ImGui::Checkbox("Generate Flat Normals", &ModelGenFlatNormals);
			ImGui::Checkbox("Generate Smooth Normals", &ModelGenSmoothNormals);
			ImGui::Checkbox("Flip UVs", &ModelFlipUVs);
			ImGui::Checkbox("Set Zero Alpha To One", &ModelSetZeroAlphaOne);
			//Add option to set 0.0 transparancie to 1.0

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

			ImGui::Checkbox("Draw Grid / Helper Lines", &RenderDebug);
			ImGui::DragFloat("Font Size", &Style.FontScaleMain, 0.005, 0.1, 5.0, "%.2f", ImGuiSliderFlags_AlwaysClamp);
//				ImGui::DragFloat("Font Multiplyer", &FontMultiplyer, 0.01, 0.1, 10.0);
			if (ImGui::Button("Apply"))
			{
				RestartEngine = true;
				FontMultiplyer *= Style.FontScaleMain;
			}					
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
				//	get_open_filename("SceneLoad", "Load Scene", "Magma Scene (*.lva;*.magma;*.mgs;*.mag){.lva,.magma,.mgs,.mag},.*", "", "", false);
				FileDialogAddInstance("Load Scene Dialog", "Magma Scene (*.lva, *.magma)\0*.lva;*.magma\0", true, false);
			}

			if (ImGui::MenuItem("Save Scene", "STRG+S"))
			{
				//	get_save_filename("SceneSave", "Save Scene", "Magma Scene (*.lva;*.magma;*.mgs){.lva,.magma,.mgs},.*", "", "");
				FileDialogAddInstance("Save Scene Dialog", "Magma Scene (*.lva, *.magma)\0*.lva;*.magma\0", false, false);
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
		//99, 201, 40
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4{ 0.388f, 0.788f, 0.156f, 1.0f });
		ImGui::PushFont(IconFontSmall);
		ImGui::SetCursorPosX((MainHWnd.Width * 0.5));
		
		char* ButtonType;
		if (GameMode)	ButtonType = (char*)"L";
		else			ButtonType = (char*)"G";
		
		ImGui::PushID("Play Button");
		if (ImGui::Button(ButtonType))
		{
			StartStopGameMode();
			EditorBarButtonPressed = true;
		}			
		ImGui::PopID();
		ImGui::PopFont();
		ImGui::PopStyleColor(1);
#ifdef MAGMA_ENGINE_CUSTOM_TITLE_BAR
		ImGui::PushFont(IconFontExt);
		{
			ImGuiStyle* Style = &ImGui::GetStyle();

			const ImVec2 LableSize = ImGui::CalcTextSize("a", NULL, true);
			ImVec2 ButtonSize = ImGui::CalcItemSize(ImVec2(0, 0), LableSize.x + Style->FramePadding.x * 2.0f, LableSize.y + Style->FramePadding.y * 2.0f);

			ImGui::SetCursorPosX((float)MainHWnd.Width - (Style->ItemSpacing.x * 3) - (ButtonSize.x * 3));
			
			ImGui::PushStyleColor(ImGuiCol_Button, Style->Colors[ImGuiCol_MenuBarBg]);
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.26f, 0.59f, 0.98f, 1.0f });
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.18f, 0.51f, 0.90f, 1.0f });

			if (ImGui::Button("r"))
			{
				EditorBarButtonPressed = true;
				ForceResizeEvent = true;
				SDL_MinimizeWindow(MainHWnd.Wnd);
			}
		//	ImGui::PopStyleColor(1);
			ImGui::SameLine();
		//	ImGui::PushStyleColor(ImGuiCol_Button, Style->Colors[ImGuiCol_MenuBarBg]);
			char Icon[4];
			if (FullScreen)
				strcpy(Icon, "W");
			else
				strcpy(Icon, "V");

			if (ImGui::Button(Icon))
			{
				EditorBarButtonPressed = true;
				PushEventSDL(SDL_EVENT_KEY_DOWN, SDLK_F11);
			}
			ImGui::PopStyleColor(3);
			ImGui::SameLine();
			ImGui::PushStyleColor(ImGuiCol_Button, Style->Colors[ImGuiCol_MenuBarBg]);
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 1.0, 0.0, 0.0, 1.0f });
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.6, 0.0, 0.0, 1.0f });
			if (ImGui::Button("'"))
			{
				EditorBarButtonPressed = true;
				PushEventSDL(SDL_EVENT_QUIT, 0);
			//	printf("Hey there\n");
			}
			ImGui::PopStyleColor(3);
		}
		ImGui::PopFont();

		if (!EditorBarButtonPressed)
			EditorWindowBar();

		EditorBarButtonPressed = false;
#endif
		ImGui::EndMainMenuBar();
	}
}

void EditorFileDialog()
{
	char Path[MAX_CHAR_PATH_LENGTH];

	if (FileDialogGetResult("Load Model Dialog", MAX_CHAR_PATH_LENGTH, Path))
	{
		/*Example for a Path(it's windows style): "C:/textures/sky.png\0C:/textures/skin.png\0\0"*/
		char* p = Path;
		while (*p)
		{
			AddModel(0, (const char*)p);
			p += (strlen(p) + 1);
		}
		
		ImGui::SetWindowFocus("Mesh Inspector");
	}

	if (FileDialogGetResult("Load Texture Dialog", MAX_CHAR_PATH_LENGTH, Path))
	{
		char* p = Path;
		while (*p)
		{
			AddTexture((char*)p, true);
			p += (strlen(p) + 1);
		}
		
		ImGui::SetWindowFocus("Texture Inspector");
	}

	if (FileDialogGetResult("Load Animation Dialog", MAX_CHAR_PATH_LENGTH, Path))
	{
		char* p = Path;
		while (*p)
		{
			AddAnimation((char*)p, 1024, 1024);
			p += (strlen(p) + 1);
		}
		
		ImGui::SetWindowFocus("Animation Inspector");
	}
	
	if (FileDialogGetResult("Load Scene Dialog", MAX_CHAR_PATH_LENGTH, Path))
	{
		char* p = Path;
		while (*p)
		{
			SceneLoad((char*)p);
			p += (strlen(p) + 1);
		}
	}

	if (FileDialogGetResult("Save Scene Dialog", MAX_CHAR_PATH_LENGTH, Path))
	{
		char* p = Path;
		while (*p)
		{
			SceneSave((char*)p);
			p += (strlen(p) + 1);
		}
	}
}

void EditorDebugWindow()
{
	ImGui::Begin("Debug");
	{
		uint32_t VertexCount = 0;
		uint32_t IndexCount = 0;

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

		ImGui::Text("Record Camera Path");
		ImGui::SameLine();
		ImGui::PushFont(IconFontSmall);
		static char* ButtonType = (char*)"G";
		ImGui::PushID("Play Button");
		if (ImGui::Button(ButtonType))
		{
			CameraPath.Record = !CameraPath.Record;
			if (CameraPath.Record)
				ButtonType = (char*)"L";
			else
				ButtonType = (char*)"G";
		}
		ImGui::PopID();
		ImGui::PopFont();
		ImGui::Text("Recorded Frame: %d", CameraPath.FrameCountPath);

		if (ImGui::Button("Camera Path Reset"))
		{
			CameraPath.FrameCount = 0;
			CameraPath.FrameCountPath = 0;
		}

		if (ImGui::Button("Play Camera Path"))
			CameraPath.Play = !CameraPath.Play;

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
		ImGui::Text("Pipeline Count: %d", VkRenderer.Pipelines.Size);
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

#ifdef MAGMA_ENGINE_TRACK_MEMORY

		ImGui::NewLine();
		ImGui::Text("Current Memory Stats:");
		ImGui::Text("Blocks In Use: %zu", s_MemBlocks.Size);

		size_t TotalAlloactedSize = 0;
		for (size_t i = 0; i < s_MemBlocks.Size; i++)
		{
			s_MemBlockInfo* Block = (s_MemBlockInfo*)DynamicArrayGetAt(&s_MemBlocks, i);
			if (Block)
			{
				TotalAlloactedSize += Block->AllocatedSize;
			}
		}

		ImGui::Text("Total Memory Alloacted Size: %zu MB", TotalAlloactedSize / (1024 * 1024));

		for (size_t i = 0; i < s_MemBlocks.Size; i++)
		{
			s_MemBlockInfo* Block = (s_MemBlockInfo*)DynamicArrayGetAt(&s_MemBlocks, i);

			ImGui::NewLine();
			ImGui::Text("Block %zu: Size: %zu/%zu, Elements: %zu/%zu", i, Block->Size, Block->AllocatedSize, Block->AllocatedElements.Size, Block->AllocatedElements.SizeAllocated);

			for (size_t j = 0; j < Block->AllocatedElements.Size; j++)
			{
				s_AllocatedElementInfo* Element = (s_AllocatedElementInfo*)DynamicArrayGetAt(&Block->AllocatedElements, j);

				if (Element->InUse == 1)
				{
					ImGui::Text("\tElement %zu: Address: %p, Size: %zu/%zu\n", j, Element->Data, Element->Size, Element->PotentialSize);
				//	Element->
				}
			}

			//	printf("[Malloc Sucks]: Failed to free: %p, Free2: %td\n", Block->Data, Block->DataAddress);
		}
		ImGui::NewLine();
#endif
	}
	ImGui::End();
}

void EditorSceneSettings()
{
	ImGui::Begin("Scene Settings");
	{
		ImGui::ColorEdit3("Clear Color", (float*)&ClearColor);
	//	ImGui::DragFloat3("Light Direction", (float*)&SceneFragmentUBO.LightDirection, 0.01, -10000.0, 10000.0);
		ImGui::SliderFloat("Ambient Multiplier", &SceneFragmentUBO.AmbientMultiplier, 0.01, 1.0);
		ImGui::SliderFloat("Exposure", &SceneFragmentUBO.Exposure, 0.01, 10.0);
		ImGui::SliderFloat("Gamma", &SceneFragmentUBO.Gamma, 0.01, 10.0);
		{
			const char* ScaleOptions[] = { "0 %", "10 %", "20 %", "30 %", "40 %", "50 %", "60 %", "70 %", "80 %", "90 %", "100 %", "110 %", "120 %", "130 %", "140 %", "150 %", "160 %", "170 %", "180 %", "190 %", "200 %" };
			if (ImGui::BeginCombo("Scene Scaling", ScaleOptions[SceneScaling / 10 < (size_t)ARRAY_SIZE(ScaleOptions) ? (SceneScaling / 10) : 0]))
			{
				for (uint32_t i = 1; i < 11; i++)//11
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
		
		//	if (OpenVkHasRaytracingSupport())
	//		ImGui::Checkbox("Render Raytraced", &RenderRaytraced);
		
		if (ImGui::BeginCombo("Render Mode", RenderModeNames[RenderMode]))
		{
			for (uint32_t i = 0; i < RENDER_MODE_COUNT - (1 - OpenVkHasRaytracingSupport()); i++)
				if (ImGui::Button(RenderModeNames[i]))
					RenderMode = i;
			ImGui::EndCombo();
		}	


	//	ImGui::Checkbox("Render Shadows", &RenderShadows);
		if (ImGui::Checkbox("Render Render Depth Pre Pass", &RenderDepthPrePass))
		{
			//FIX - create a function for this garbage
			ForceResizeEvent = true;
			PushEventSDL(0, 0);
		}
	

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
				ShadowMapWidth = ShadowMapHeight;
				ReloadShaders = true;
			}

			for (uint32_t i = 0; i < SHADOW_MAP_CASCADE_COUNT; i++)
			{
				char Name[64];
				ssprintf(Name, "Cascade %d", i);
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
}

void EditorRenderStages()
{
	ImGui::Begin("Render Stages", NULL, ImGuiWindowFlags_NoScrollbar);
	{
		ImVec2 ImageSize = ImGui::GetWindowSize();
		float Aspect = (float)SceneWidth / (float)SceneHeight;
		ImageSize.y = ImageSize.x / Aspect;
		
		ImGui::Checkbox("Render Debug View", &RenderDebugDescriptorSet);
		
		if (RenderMode == RENDER_MODE_FORWARD)
		{
			ImGui::Text("To-Do, EditorUI.h");
		}
		else if (RenderMode == RENDER_MODE_DEFERRED)
		{
			for (uint32_t i = 0; i < ARRAY_SIZE(DebugDescriptorSets); i++)
			{
				ImGui::Text(DebugAttachmentNames[i]);
				//		ImGui::PushID(DebugDescriptorSets[i]);
				if (ImGuiImageButtonID(DebugAttachmentNames[i], &GetDescriptorSet(DebugDescriptorSets[i])[0], ImageSize, ImVec2(0, 0), ImVec2(1, 1), 0))
					SceneRenderDescriptorSet = DebugDescriptorSets[i];

				ImGui::NewLine();
				//	ImGui::Image(&GetDescriptorSet(DebugDescriptorSets[i])[0], i == 0 ? ImVec2(ImageSize.x, ImageSize.x / 3) : ImageSize);
		//			ImGui::PopID();
			}
		}
		else if (RenderMode == RENDER_MODE_RAYTRACED)
		{
			ImGui::Text("Raytracing Pass");
			if (ImGuiImageButtonID("Raytraced Image ID", &GetDescriptorSet(SceneRenderDescriptorSet)[0], ImageSize, ImVec2(0, 0), ImVec2(1, 1), 0))
				SceneRenderDescriptorSet = SceneOutputDescriptorSet;
			ImGui::NewLine();

			ImGui::Text("FXAA Pass");
			if (ImGuiImageButtonID("FXAA Image ID", &GetDescriptorSet(DebugDescriptorSets[ARRAY_SIZE(DebugDescriptorSets) - 1])[0], ImageSize, ImVec2(0, 0), ImVec2(1, 1), 0))
				SceneRenderDescriptorSet = DebugDescriptorSets[ARRAY_SIZE(DebugDescriptorSets) - 1];
		}
	}
	ImGui::End();
}

void EditorSSRDebug()
{
	ImGui::Begin("SSR Debug");
	{
		/*
		SSRFragmentUBO.Step = 0.1;
		SSRFragmentUBO.MinRayStep = 0.1;
		SSRFragmentUBO.MaxSteps = 90;
		SSRFragmentUBO.NumBinarySearchSteps = 5;
		SSRFragmentUBO.ReflectionSpecularFalloffExponent = 3.0;
		*/
		ImGui::DragFloat("Step", &SSRFragmentUBO.Step, 0.01);
		ImGui::DragFloat("MinRayStep", &SSRFragmentUBO.MinRayStep, 0.01);
		ImGui::DragInt("MaxSteps", &SSRFragmentUBO.MaxSteps);
		ImGui::DragInt("NumBinarySearchSteps", &SSRFragmentUBO.NumBinarySearchSteps);
		ImGui::DragFloat("ReflectionSpecularFalloffExponent", &SSRFragmentUBO.ReflectionSpecularFalloffExponent, 0.01);
	}
	ImGui::End();
}


typedef struct
{
	float L;
	float a;
	float b;
} Lab;

typedef struct
{
	float L;
	float C;
	float h;
} LCh;

void linear_srgb_to_oklab(float c[3], Lab* res) 
{
    float l = 0.4122214708f * c[0] + 0.5363325363f * c[1] + 0.0514459929f * c[2];
	float m = 0.2119034982f * c[0] + 0.6806995451f * c[1] + 0.1073969566f * c[2];
	float s = 0.0883024619f * c[0] + 0.2817188376f * c[1] + 0.6299787005f * c[2];

    float l_ = cbrtf(l);
    float m_ = cbrtf(m);
    float s_ = cbrtf(s);

    
	res->L = 0.2104542553f*l_ + 0.7936177850f*m_ - 0.0040720468f*s_;
	res->a = 1.9779984951f*l_ - 2.4285922050f*m_ + 0.4505937099f*s_;
	res->b = 0.0259040371f*l_ + 0.7827717662f*m_ - 0.8086757660f*s_;

}

void oklab_to_linear_srgb(Lab c, float res[3]) 
{
    float l_ = c.L + 0.3963377774f * c.a + 0.2158037573f * c.b;
    float m_ = c.L - 0.1055613458f * c.a - 0.0638541728f * c.b;
    float s_ = c.L - 0.0894841775f * c.a - 1.2914855480f * c.b;

    float l = l_*l_*l_;
    float m = m_*m_*m_;
    float s = s_*s_*s_;

    
	res[0] = +4.0767416621f * l - 3.3077115913f * m + 0.2309699292f * s;
	res[1] = -1.2684380046f * l + 2.6097574011f * m - 0.3413193965f * s;
	res[2] = -0.0041960863f * l - 0.7034186147f * m + 1.7076147010f * s;

}

void OkLabToLCh(Lab* lab, LCh* lch)
{
	lch->L = lab->L;
	lch->C = sqrtf((lab->a * lab->a) + (lab->b * lab->b));
	lch->h = atan2f(lab->b, lab->a);
}

void LChToOkLab(LCh* lch, Lab* lab)
{
	lab->L = lch->L;
	lab->a = lch->C * cosf(0.01745329251 * lch->h);
	lab->b = lch->C * sinf(0.01745329251 * lch->h);
}

typedef enum
{
	GRADIENT_TYPE_HUE = 0,
	GRADIENT_TYPE_CHROMA,
	GRADIENT_TYPE_LIGHTNESS
} GradientTypes;

void DrawGradient(LCh* lch, GradientTypes GradientType, bool GammaCorrected)
{
	ImVec2 p = ImGui::GetCursorScreenPos();

	uint32_t Segments = 0;
	float Max = 0.0;

	LCh Gradient = *lch;
	float RGBGradient[3];

	switch (GradientType)
	{
	case GRADIENT_TYPE_HUE:
		Gradient.h = 0;
		Segments = 12;
		Max = 360.0;
		break;
	case GRADIENT_TYPE_CHROMA:
		Gradient.C = 0;
		Segments = 3;
		Max = 0.47;
		break;
	case GRADIENT_TYPE_LIGHTNESS:
		Gradient.L = 0;
		Segments = 3;
		Max = 1.0;
		break;
	default:
		break;
	}
	
	ImGuiStyle* Style = &ImGui::GetStyle();

	

	Lab OkLab;
	LChToOkLab(&Gradient, &OkLab);
	oklab_to_linear_srgb(OkLab, RGBGradient);
	
	float Width = (ImGui::GetWindowWidth() - ImGui::GetCursorStartPos().x * 2) / (Segments - 1);
	const float Height = 60;

	ImDrawList* DL = ImGui::GetWindowDrawList();

	for (int i = 0; i < (Segments - 1); i++)
	{
		
		
		ImU32 Color1 = ImGuiColorConvertFloat4ToU32GammaCorrected(ImVec4(RGBGradient[0], RGBGradient[1], RGBGradient[2], 1.0), GammaCorrected);
		
	//	Gradient.h += 30;

		switch (GradientType)
		{
		case GRADIENT_TYPE_HUE:
			Gradient.h += (Max / Segments);
			break;
		case GRADIENT_TYPE_CHROMA:
			Gradient.C += (Max / Segments);
			break;
		case GRADIENT_TYPE_LIGHTNESS:
			Gradient.L += (Max / Segments);
			break;
		default:
			break;
		}

		LChToOkLab(&Gradient, &OkLab);
		oklab_to_linear_srgb(OkLab, RGBGradient);

		ImU32 Color2 = ImGuiColorConvertFloat4ToU32GammaCorrected(ImVec4(RGBGradient[0], RGBGradient[1], RGBGradient[2], 1.0), GammaCorrected);

		DL->AddRectFilledMultiColor(p + ImVec2(Width * i, 0), p + ImVec2(Width * i + Width, Height), Color1, Color2, Color2, Color1);
	}

	const float ThumbWidth = 10;
	const float ThumbHeight = 00;

	float OffsetX = 0.0;
	switch (GradientType)
	{
	case GRADIENT_TYPE_HUE:
		OffsetX = lch->h / Max;
		break;
	case GRADIENT_TYPE_CHROMA:
		OffsetX = lch->C / Max;
		break;
	case GRADIENT_TYPE_LIGHTNESS:
		OffsetX = lch->L / Max;
		break;
	default:
		break;
	}
	//

	ImVec2 ThumbOffset = ImVec2(OffsetX * (ImGui::GetWindowWidth() - ImGui::GetCursorStartPos().x * 2), 0.0);
	DL->AddRect(p + ImVec2(-ThumbWidth, -ThumbHeight) + ThumbOffset, p + ImVec2(ThumbWidth, Height + ThumbHeight) + ThumbOffset, ImGuiColorConvertFloat4ToU32GammaCorrected(ImVec4(0.8, 0.8, 0.8, 1.0), GammaCorrected), Style->GrabRounding);

	

	ImGui::Dummy(ImVec2(0.0, Height));
}

void OkColorPicker(const char* Label, LCh* lch, float RGB[3])
{
	ImGui::Text(Label);
	ImGui::Separator();

	
//	OkLabToLCh()

//	int Hue = lch->h;// * 360;
//	int Sat = lch->C / 0.4f * 100;
//	int Lig = lch->L * 100;
//
//	ImGui::SliderInt("Hue", &Hue, 0, 360);
//	ImGui::SliderInt("Saturation (Chroma)", &Sat, 0, 100);
//	ImGui::SliderInt("Lightness", &Lig, 0, 100);
//
//	lch->h = (float)Hue;// / 360.0f;
//	lch->C = (float)Sat / 100.0f * 0.4f;
//	lch->L = (float)Lig / 100.0f;

	ImGui::SliderFloat("Hue", &lch->h, 0.0, 360.0, "%.3f");
	ImGui::SliderFloat("Chroma (Saturation)", &lch->C, 0.0, 0.47, "%.4f");
	ImGui::SliderFloat("Lightness", &lch->L, 0.0, 1.0, "%.4f");

//	ImGui::SliderFloat("L - perceived lightness", &OkLab->L, 0.0, 1.0);
//	ImGui::SliderFloat("a - how green/red the color is", &OkLab->a, -0.4, 0.4);
//	ImGui::SliderFloat("b - how blue/yellow the color is", &OkLab->b, -0.4, 0.4);

//	int Hue = OkLab.;
//	ImGui::SliderInt("Hue", &Hue, 0, 360);

	Lab OkLab;
	LChToOkLab(lch, &OkLab);
	oklab_to_linear_srgb(OkLab, RGB);

//	ImGui::SliderFloat("Alpha", &RGB[3], 0.0, 1.0);

	ImGui::Text("Color");
	ImGui::SameLine();
	ImVec2 p = ImGui::GetCursorScreenPos();
	
	//ImGui::GetWindowPos();

	static bool GammaCorrected = false;

	ImDrawList* DL = ImGui::GetWindowDrawList();
	DL->AddRectFilled(p, p + ImVec2(120, 120), ImGuiColorConvertFloat4ToU32GammaCorrected(ImVec4(RGB[0], RGB[1], RGB[2], 1.0), GammaCorrected));
	ImGui::Dummy(ImVec2(120, 120));


	
		
//	ImGui::NewLine();
//	ImGui::NewLine();
	
//	ImGui::SetCursorPosY(240);

	char buf[64];
	int ColorInt[] = { (int)(RGB[0] * 255), (int)(RGB[1] * 255), (int)(RGB[2] * 255), 255 };
	ImGuiGet32bitColorHexCode(ColorInt, ARRAY_SIZE(buf), buf);

	ImGui::InputText("Color Hex", buf, IM_ARRAYSIZE(buf));
	ImGui::DragFloat3("Color RGB Float", RGB);
	ImGui::DragInt3("Color RGB Int", ColorInt);

//	p = ImGui::GetCursorScreenPos();

//	p.y += 100;
	DrawGradient(lch, GRADIENT_TYPE_HUE, GammaCorrected);

	
	DrawGradient(lch, GRADIENT_TYPE_CHROMA, GammaCorrected);

	
	DrawGradient(lch, GRADIENT_TYPE_LIGHTNESS, GammaCorrected);

	ImGui::Checkbox("Gamma Correction", &GammaCorrected);
	
/*
	{
		LCh Gradient = *lch;
		float RGBGradient[3];
		Gradient.h = 0;

		LChToOkLab(&Gradient, &OkLab);
		oklab_to_linear_srgb(OkLab, RGB);
		
		float Width = (ImGui::GetWindowWidth() - ImGui::GetCursorStartPos().x * 2) / 11;
		const float Height = 60;

		for (int i = 0; i < 11; i++)
		{
			
			
			ImU32 Color1 = ImGui::ColorConvertFloat4ToU32(ImVec4(RGBGradient[0], RGBGradient[1], RGBGradient[2], 1.0));
			
			Gradient.h += 30;
			LChToOkLab(&Gradient, &OkLab);
			oklab_to_linear_srgb(OkLab, RGBGradient);

			ImU32 Color2 = ImGui::ColorConvertFloat4ToU32(ImVec4(RGBGradient[0], RGBGradient[1], RGBGradient[2], 1.0));

			DL->AddRectFilledMultiColor(p + ImVec2(Width * i, 0), p + ImVec2(Width * i + Width, Height), Color1, Color2, Color2, Color1);
		}

		const float ThumbWidth = 10;
		const float ThumbHeight = 10;
		ImVec2 ThumbOffset = ImVec2(lch->h / (360) * (ImGui::GetWindowWidth() - ImGui::GetCursorStartPos().x * 2), 0.0);
		DL->AddRect(p + ImVec2(-ThumbWidth, -ThumbHeight) + ThumbOffset, p + ImVec2(ThumbWidth, Height + ThumbHeight) + ThumbOffset, ImGui::ColorConvertFloat4ToU32(ImVec4(0.8, 0.8, 0.8, 1.0)));
	}
		*/
}

void EngineDrawEditor()
{
	ImGui_ImplVulkan_NewFrame();
	ImGui_ImplSDL3_NewFrame();
	ImGui::NewFrame();

	ImGuiIO* IO = &ImGui::GetIO();

	EditorDrawMainMenuBar();
	EditorSettingsWindow();
	EditorDrawScene();
	EditorEntities();
	EditorInspector();
	EditorAssetBrowser();
/*
	ImGui::Begin("Sample Picker");
	static LCh Color = { 0.7374, 0.1151, 217.39 };
	float RGB[] = { 0.0, 0.0, 0.0 };
	OkColorPicker("Ok Color Picker", &Color, RGB);
	ImGui::End();
*/
	EditorFileDialog();
	EditorDebugWindow();
	EditorSceneSettings();	
	EditorRenderStages();
	EditorSSRDebug();

	RenderWorkflowDrawBuilder();

	#ifdef LINUX_PORT
	UpdateLuaScripts();
	#endif
}