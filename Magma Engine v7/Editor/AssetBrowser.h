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

		Mutex.lock();
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
		Mutex.unlock();

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
				AddMaterial();
				ImGui::SetWindowFocus("Material Inspector");
			}

			if (ImGui::Selectable("Add Plane"))
			{
				SceneMesh MeshInfo;

				MeshInfo.Destroyable = false;
				strcpy(MeshInfo.Path, "Plane");
				MeshInfo.MeshData = (SceneMeshData*)malloc(1 * sizeof(SceneMeshData));
				MeshInfo.MeshCount = 1;

				SetDefaultMaterial(&MeshInfo.MeshData[0].Material, "MESH");
				MeshInfo.MeshData[0].VertexCount = ARRAY_SIZE(PlaneVertices);
				MeshInfo.MeshData[0].VertexOffset = 0;
				MeshInfo.VertexBuffer = PlaneVertexBuffer;

				MeshInfo.MeshData[0].IndexCount = ARRAY_SIZE(PlaneIndices);
				MeshInfo.MeshData[0].IndexOffset = 0;
				MeshInfo.IndexBuffer = PlaneIndexBuffer;
				memset(&MeshInfo.MeshData[0].Render, 1, ARRAY_SIZE(MeshInfo.MeshData[0].Render) * sizeof(bool));
				MeshInfo.MeshData[0].AABB = PlaneAABB;

				AddMesh("Plane", &MeshInfo);
				ImGui::SetWindowFocus("Mesh Inspector");
			}

			if (ImGui::Selectable("Add Cube"))
			{
				SceneMesh MeshInfo;

				MeshInfo.Destroyable = false;
				strcpy(MeshInfo.Path, "Cube");
				MeshInfo.MeshData = (SceneMeshData*)malloc(1 * sizeof(SceneMeshData));
				MeshInfo.MeshCount = 1;

				SetDefaultMaterial(&MeshInfo.MeshData[0].Material, "MESH");
				MeshInfo.MeshData[0].VertexCount = ARRAY_SIZE(CubeVertices);
				MeshInfo.MeshData[0].VertexOffset = 0;
				MeshInfo.VertexBuffer = CubeVertexBuffer;

				MeshInfo.MeshData[0].IndexCount = 0;
				MeshInfo.MeshData[0].IndexOffset = 0;
				MeshInfo.IndexBuffer = OPENVK_ERROR;
				memset(&MeshInfo.MeshData[0].Render, 1, ARRAY_SIZE(MeshInfo.MeshData[0].Render) * sizeof(bool));
				MeshInfo.MeshData[0].AABB = CubeAABB;

				AddMesh("Cube", &MeshInfo);
				ImGui::SetWindowFocus("Mesh Inspector");
			}

			if (ImGui::Selectable("Add Sphere"))
			{
				SceneMesh MeshInfo;

				MeshInfo.Destroyable = false;
				strcpy(MeshInfo.Path, "Sphere");
				MeshInfo.MeshData = (SceneMeshData*)malloc(1 * sizeof(SceneMeshData));
				MeshInfo.MeshCount = 1;

				SetDefaultMaterial(&MeshInfo.MeshData[0].Material, "MESH");
				MeshInfo.MeshData[0].VertexCount = SphereVertexCount;
				MeshInfo.MeshData[0].VertexOffset = 0;
				MeshInfo.VertexBuffer = SphereVertexBuffer;

				MeshInfo.MeshData[0].IndexCount = SphereIndexCount;
				MeshInfo.MeshData[0].IndexOffset = 0;
				MeshInfo.IndexBuffer = SphereIndexBuffer;
				memset(&MeshInfo.MeshData[0].Render, 1, ARRAY_SIZE(MeshInfo.MeshData[0].Render) * sizeof(bool));
				MeshInfo.MeshData[0].AABB = SphereAABB;

				AddMesh("Sphere", &MeshInfo);
				ImGui::SetWindowFocus("Mesh Inspector");
			}

			if (ImGui::Selectable("Add Bean"))
			{
				SceneMesh MeshInfo;

				MeshInfo.Destroyable = false;
				strcpy(MeshInfo.Path, "Bean");
				MeshInfo.MeshData = (SceneMeshData*)malloc(1 * sizeof(SceneMeshData));
				MeshInfo.MeshCount = 1;

				SetDefaultMaterial(&MeshInfo.MeshData[0].Material, "MESH");
				MeshInfo.MeshData[0].VertexCount = BeanVertexCount;
				MeshInfo.MeshData[0].VertexOffset = 0;
				MeshInfo.VertexBuffer = BeanVertexBuffer;
				
				MeshInfo.MeshData[0].IndexCount = BeanIndexCount;
				MeshInfo.MeshData[0].IndexOffset = 0;
				MeshInfo.IndexBuffer = BeanIndexBuffer;

				memset(&MeshInfo.MeshData[0].Render, 1, ARRAY_SIZE(MeshInfo.MeshData[0].Render) * sizeof(bool));
				MeshInfo.MeshData[0].AABB = BeanAABB;
					
				AddMesh("Bean", &MeshInfo);
				ImGui::SetWindowFocus("Mesh Inspector");
			}

		//	if (ImGui::BeginMenu("Load Model"))
		//	{
		//		if (ImGui::MenuItem("Default")) ifd::FileDialog::Instance().Open("LoadDefaultModel", "Open Model", "All Models (*.obj;*.stl;*.gltf;*.glb){.obj,.stl,.gltf,.glb},.*", true);
		//		if (ImGui::MenuItem("Gen Smooth Normals")) ifd::FileDialog::Instance().Open("LoadSmoothModel", "Open Model", "All Models (*.obj;*.stl;*.gltf;*.glb){.obj,.stl,.gltf,.glb},.*", true);
		//		if (ImGui::MenuItem("Gen Flat Normals")) ifd::FileDialog::Instance().Open("LoadFlatModel", "Open Model", "All Models (*.obj;*.stl;*.gltf;*.glb){.obj,.stl,.gltf,.glb},.*", true);
		//		ImGui::EndMenu();
		//	}

			if (ImGui::Selectable("Load Model"))
				ifd::FileDialog::Instance().Open("LoadModel", "Load Model", "All Models (*.obj;*.stl;*.gltf;*.glb){.obj,.stl,.gltf,.glb},.*", true);

			if (ImGui::Selectable("Load Texture"))
				ifd::FileDialog::Instance().Open("LoadTexture", "Load Texture", "Image files (*.png;*.jpg;*.tga;*.jpeg;*.hdr;*.psd;*.bmp){.png,.jpg,.tga,.jpeg,.hdr,.psd,.bmp},.*", true);

			if (ImGui::Selectable("Load Animation"))
				ifd::FileDialog::Instance().Open("LoadAnimation", "Load Animation", "Quake Models (*.md2;*.bin){.md2,.bin},.*", true);

			ImGui::EndPopup();
		}			
	}
	ImGui::End();
}