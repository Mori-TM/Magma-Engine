void EditorAssetBrowserShowAsset(CMA_MemoryZone* Zone, const float ThumbnailSize, const char* Icon, const char* WindowToFocus, uint32_t* Selection)
{
	for (uint32_t i = 1; i < Zone->Size; i++)
		{
			char* NamePtr = (char*)CMA_GetAt(Zone, i);
			if (NamePtr != NULL)
			{
				if (ImGuiIconButton(Icon, NamePtr, ICON_BUTTON_TYPE_LARGE, ImVec2(ThumbnailSize, ThumbnailSize)))
				{
					ImGui::SetWindowFocus(WindowToFocus);
					(*Selection) = i;
				}
				ImGui::Text(NamePtr);
				ImGui::NextColumn();
			}
		}
}

void EditorAssetBrowser()
{
	ImGui::Begin("Assets");
	{
		float Padding = 16.0;
		float ThumbnailSize = 100.0;
		float CellSize = ThumbnailSize + Padding;

		float PanelWidth = ImGui::GetContentRegionAvail().x;
		uint32_t ColumCount = (uint32_t)(PanelWidth / CellSize);
		if (ColumCount < 1)
			ColumCount = 1;
		ImGui::Columns(ColumCount, 0, false);

		for (uint32_t i = 1; i < SceneTextures.Size; i++)
		{
			SceneTextureImage* Image = (SceneTextureImage*)CMA_GetAt(&SceneTextures, i);
			// && Image->ShowInAssetBrowser
			if (Image != NULL)
			{
				ImGui::PushID(Image->Name);
				if (ImGuiImageButtonExt(&GetDescriptorSet(Image->TextureDescriptorSet)[0], ImVec2(ThumbnailSize, ThumbnailSize), ImVec2(Image->Width, Image->Height)))
				{
					ImGui::SetWindowFocus("Texture Inspector");
					SelectedTexture = i;
				}
				ImGui::Text(Image->Name);
				ImGui::NextColumn();
				ImGui::PopID();
			}
		}

		for (uint32_t i = 1; i < SceneMaterials.Size; i++)
		{
			SceneMaterial* Material = (SceneMaterial*)CMA_GetAt(&SceneMaterials, i);
			if (Material != NULL)
			{
				if (ImGuiIconButton("A", Material->Name, ICON_BUTTON_TYPE_LARGE, ImVec2(ThumbnailSize, ThumbnailSize)))
				{
					ImGui::SetWindowFocus("Material Inspector");
					SelectedMaterial = i;
				}
				ImGui::Text(Material->Name);
				ImGui::NextColumn();
			}
		}

		for (uint32_t i = 1; i < SceneMeshes.Size; i++)
		{
			SceneMesh* Mesh = (SceneMesh*)CMA_GetAt(&SceneMeshes, i);
			if (Mesh != NULL)
			{
				if (ImGuiIconButton("B", Mesh->Name, ICON_BUTTON_TYPE_LARGE, ImVec2(ThumbnailSize, ThumbnailSize)))
				{
					ImGui::SetWindowFocus("Mesh Inspector");
					SelectedMesh = i;
				}
				ImGui::Text(Mesh->Name);
				ImGui::NextColumn();
			}
		}

		for (uint32_t i = 1; i < SceneAnimations.Size; i++)
		{
			SceneAnimation* Animation = (SceneAnimation*)CMA_GetAt(&SceneAnimations, i);
			if (Animation != NULL)
			{
				if (ImGuiIconButton("z", Animation->Name, ICON_BUTTON_TYPE_LARGE, ImVec2(ThumbnailSize, ThumbnailSize)))
				{
					ImGui::SetWindowFocus("Mesh Inspector");
					SelectedMesh = i;
				}
				ImGui::Text(Animation->Name);
				ImGui::NextColumn();
			}
		}
/*
		for (uint32_t i = 0; i < SceneScriptCount; i++)
		{
			if (ImGuiIconButton("J", SceneScripts[i].Name, ICON_BUTTON_TYPE_LARGE, ImVec2(ThumbnailSize, ThumbnailSize)))
			{
				ImGui::SetWindowFocus("Script Inspector");
				SelectedScript = i;
			}

			ImGui::Text(SceneScripts[i].Name);
			ImGui::NextColumn();
		}
			*/

		EditorAssetBrowserShowAsset(&SceneScripts, ThumbnailSize, "J", "Script Inspector", &SelectedScript);

		ImGui::Columns(1);

		if (ImGui::BeginPopupContextWindow("Asset Pop Up"))
		{

			if (ImGui::Selectable("Add Script"))
			{
				AddScript();
				ImGui::SetWindowFocus("Script Inspector");
			}

			if (ImGui::Selectable("Add Material"))
			{
				AddMaterial(NULL);
				ImGui::SetWindowFocus("Material Inspector");
			}

			if (ImGui::Selectable("Add Plane"))
			{
				AddDefaultModel(DEFAULT_MODEL_PLANE);
				ImGui::SetWindowFocus("Mesh Inspector");
			}

			if (ImGui::Selectable("Add Cube"))
			{
				AddDefaultModel(DEFAULT_MODEL_CUBE);
				ImGui::SetWindowFocus("Mesh Inspector");
			}

			if (ImGui::Selectable("Add Sphere"))
			{
				AddDefaultModel(DEFAULT_MODEL_SPHERE);
				ImGui::SetWindowFocus("Mesh Inspector");
			}

			if (ImGui::Selectable("Add Bean"))
			{
				AddDefaultModel(DEFAULT_MODEL_BEAN);
				ImGui::SetWindowFocus("Mesh Inspector");
			}
			
			if (ImGui::Selectable("Load Model"))
			{
			//	get_open_filename("LoadModel", "Load Model", "All Models (*.obj;*.stl;*.gltf;*.glb){.obj,.stl,.gltf,.glb},.*", "", "", true);
			//	ifd::FileDialog::Instance().Open("LoadModel", "Load Model", "All Models (*.obj;*.stl;*.gltf;*.glb)     {.obj,.stl,.gltf,.glb},.*", true);
				FileDialogAddInstance("Load Model Dialog", "All Models (*.obj, *.stl, *.gltf, *.glb)\0*.obj;*.stl;*.gltf;*.glb\0", true, true);
			}

			if (ImGui::Selectable("Load Texture"))
			{
			//	get_open_filename("LoadTexture", "Load Texture", "Image files (*.png;*.jpg;*.tga;*.jpeg;*.hdr;*.psd;*.bmp){.png,.jpg,.tga,.jpeg,.hdr,.psd,.bmp},.*", "", "", true);
				FileDialogAddInstance("Load Texture Dialog", "All Textures (*.png, *.jpg, *.tga, *.jpeg, *.hdr, *.psd, *.bmp)\0*.png;*.jpg;*.tga;*.jpeg;*.hdr;*.psd;*.bmp\0", true, true);
			}

			if (ImGui::Selectable("Load Animation"))
			{
			//	get_open_filename("LoadAnimation", "Load Animation", "Quake Models (*.md2;*.bin){.md2,.bin},.*", "", "", true);
				FileDialogAddInstance("Load Animation Dialog", "All Animations (*.md2, *.bin)\0*.md2;*.bin\0", true, true);
			}

			ImGui::EndPopup();
		}			
	}
	ImGui::End();
}
