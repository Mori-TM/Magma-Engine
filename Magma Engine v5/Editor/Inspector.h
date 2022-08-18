void EditorEntityInspector()
{
	ImGui::Begin("Entity Inspector");
	{
		if (EntityCount > 0)
		{
			ImGui::InputText("Name", Entities[SelectedEntity].Name, 2048);

			if (ImGui::CollapsingHeader("TRANSFORM"))
			{
				ImGui::DragFloat3("Translation", (float*)&Entities[SelectedEntity].Translate, 0.1, -10000.0, 10000.0);
				ImGui::DragFloat3("Rotation", (float*)&Entities[SelectedEntity].Rotate, 0.1, -10000.0, 10000.0);
				ImGui::DragFloat3("Scale", (float*)&Entities[SelectedEntity].Scale, 0.1, -10000.0, 10000.0);
			}

			if (Entities[SelectedEntity].UsedComponents[COMPONENT_TYPE_MESH] == true)
			{
				if (ImGui::CollapsingHeader("MESH"))
				{
					SceneMesh* Mesh = (SceneMesh*)CMA_GetAt(&SceneMeshes, Entities[SelectedEntity].Mesh.MeshIndex);
					if (Mesh != NULL)
					{
						if (ImGui::BeginCombo("Mesh", Mesh->Name))
						{
							for (uint32_t i = 0; i < SceneMeshes.Size; i++)
							{
								Mesh = (SceneMesh*)CMA_GetAt(&SceneMeshes, i);
								if (Mesh != NULL)
								{
									if (ImGui::Button(Mesh->Name))
									{
										Entities[SelectedEntity].Mesh.MeshIndex = i;
										strcpy(Entities[SelectedEntity].Mesh.Name, Mesh->Name);
									}
								}						
							}
							ImGui::EndCombo();
						}
					}
					

					ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 0, 0, 255));
					if (ImGui::Button("Remove Mesh Component"))
					{
						Entities[SelectedEntity].UsedComponents[COMPONENT_TYPE_MESH] = false;
					}
					ImGui::PopStyleColor();
				}

			}

			if (Entities[SelectedEntity].UsedComponents[COMPONENT_TYPE_MATERIAL] == true)
			{
				if (ImGui::CollapsingHeader("MATERIAL"))
				{
					SceneMaterial* Material = (SceneMaterial*)CMA_GetAt(&SceneMaterials, Entities[SelectedEntity].Material.MaterialIndex);
					if (Material != NULL)
					{
						if (ImGui::BeginCombo("Material", Material->Name))
						{
							for (uint32_t i = 0; i < SceneMaterials.Size; i++)
							{
								Material = (SceneMaterial*)CMA_GetAt(&SceneMaterials, i);
								if (Material != NULL)
								{
									if (ImGui::Button(Material->Name))
									{
										Entities[SelectedEntity].Material.MaterialIndex = i;
										strcpy(Entities[SelectedEntity].Material.Name, Material->Name);
									}
								}
							}
							ImGui::EndCombo();
						}

						ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 0, 0, 255));
						if (ImGui::Button("Remove Material Component"))
						{
							Entities[SelectedEntity].UsedComponents[COMPONENT_TYPE_MATERIAL] = false;
						}
						ImGui::PopStyleColor();
					}					
				}
			}

			if (Entities[SelectedEntity].UsedComponents[COMPONENT_TYPE_CAMERA] == true)
			{
				if (ImGui::CollapsingHeader("CAMERA"))
				{
					ImGui::DragFloat("Field of View", &Entities[SelectedEntity].Camera.FOV, 0.1, 0.01, 179.0);
					ImGui::DragFloat("Near Plane", &Entities[SelectedEntity].Camera.NearPlane, 0.1, 0.01, 1000.0);
					ImGui::DragFloat("Far Plane", &Entities[SelectedEntity].Camera.FarPlane, 0.1, 0.01, 10000.0);

					ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 0, 0, 255));
					if (ImGui::Button("Remove Camera Component"))
					{
						Entities[SelectedEntity].UsedComponents[COMPONENT_TYPE_CAMERA] = false;
					}
					ImGui::PopStyleColor();
				}
			}
	
			if (ImGui::BeginPopupContextWindow("Entity Inspector Pop Up"))
			{
				ImGui::Text("Components");
				const char* Components[] = { "Mesh", "Material", "Camera" };
				for (uint32_t i = 0; i < COMPONENT_COUNT; i++)
				{
					if (ImGui::Button(Components[i]))
						Entities[SelectedEntity].UsedComponents[i] = true;
				}

				ImGui::NewLine();

				ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 255, 0, 255));
				if (ImGui::Button("Duplicate Entity"))
				{
					Entities = (EntityInfo*)realloc(Entities, (EntityCount + 1) * sizeof(EntityInfo));
					Entities[EntityCount] = Entities[SelectedEntity];
					sprintf(Entities[EntityCount].Name, "%s Copy", Entities[EntityCount].Name);
					SelectedEntity = EntityCount;
					EntityCount++;
				}
				ImGui::PopStyleColor();

				ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 0, 0, 255));
				if (ImGui::Button("Delete Entity"))
				{
					for (uint32_t i = SelectedEntity; i < EntityCount - 1; i++)
						Entities[i] = Entities[i + 1];

					Entities = (EntityInfo*)realloc(Entities, (EntityCount - 1) * sizeof(EntityInfo));

					EntityCount--;
					SelectedEntity = EntityCount - 1;
				}
				ImGui::PopStyleColor();

				ImGui::EndPopup();
			}
		}
	}
	ImGui::End();
}

void EditorTextureCombo(const char* Name, const char* ID, uint32_t* TextureIndex)
{
	SceneTextureImage* Image = (SceneTextureImage*)CMA_GetAt(&SceneTextures, *TextureIndex);

	ImGui::PushID(ID);
	if (ImGui::BeginCombo(Name, Image->Name))
	{
		for (uint32_t i = 0; i < SceneTextures.Size; i++)
		{
			Image = (SceneTextureImage*)CMA_GetAt(&SceneTextures, i);
			// && Image->ShowInAssetBrowser
			if (Image != NULL)
			{
				if (ImGui::ImageButton(&GetDescriptorSet(Image->TextureDescriptorSet)[0], ImVec2(30, 30)))
					*TextureIndex = i;

				ImGui::SameLine();
				ImGui::Text(Image->Name);
			}
		}
		ImGui::EndCombo();
	}
}

void EditorMaterialInspector()
{
	ImGui::Begin("Material Inspector");
	{
		if (SceneMaterials.Size > 1)
		{
			SceneMaterial* Material = (SceneMaterial*)CMA_GetAt(&SceneMaterials, SelectedMaterial);
			if (Material != NULL)
			{
				ImGui::InputText("Name", Material->Name, MAX_CHAR_NAME_LENGTH);
				ImGui::ColorPicker4("Color", (float*)&Material->Color, ImGuiColorEditFlags_AlphaBar);

				EditorTextureCombo("Albedo", "A-Mat", & Material->AlbedoIndex);
				EditorTextureCombo("Normal", "N-Mat", &Material->NormalIndex);
				EditorTextureCombo("Metallic", "M-Mat", &Material->MetallicIndex);
				ImGui::PushID("MS-Mat");
				ImGui::SliderFloat("Metallic Strength", &Material->Metallic, 0.0, 1.0);
				EditorTextureCombo("Roughness", "R-Mat", &Material->RoughnessIndex);
				ImGui::PushID("RS-Mat");
				ImGui::SliderFloat("Roughness Strength", &Material->Roughness, 0.0, 1.0);
				EditorTextureCombo("Occlusion", "O-Mat", &Material->OcclusionIndex);
				ImGui::PushID("OS-Mat");
				ImGui::SliderFloat("Occlusion Strength", &Material->Occlusion, 0.0, 1.0);

				ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 0, 0, 255));
				if (ImGui::Button("Delete Material"))
				{
					for (uint32_t i = 0; i < EntityCount; i++)
						if (Entities[i].UsedComponents[COMPONENT_TYPE_MATERIAL] && Entities[i].Material.MaterialIndex == SelectedMaterial)
							ResetEntityMaterial(&Entities[i]);

					CMA_Pop(&SceneMaterials, SelectedMaterial);

					for (uint32_t i = 1; i < SceneMaterials.Size; i++)
						if (CMA_GetAt(&SceneMaterials, i) != NULL)
							SelectedMaterial = i;
				}

				ImGui::PopStyleColor();
			}			
		}
	}
	ImGui::End();
}

void EditorTextureInspector()
{
	ImGui::Begin("Texture Inspector");
	{
		if (SceneTextures.Size > 1)
		{
			SceneTextureImage* Image = (SceneTextureImage*)CMA_GetAt(&SceneTextures, SelectedTexture);
			// && Image->ShowInAssetBrowser
			if (Image != NULL)
			{
				ImGui::Text(Image->Path);
				ImGui::InputText("Name", Image->Name, MAX_CHAR_NAME_LENGTH);
				ImVec2 Size = ImGui::GetWindowSize();
				Size.x -= 20;
				Size.y = Size.x;
				ImGui::Image(&GetDescriptorSet(Image->TextureDescriptorSet)[0], Size);

				ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 0, 0, 255));
				if (ImGui::Button("Delete Texture"))
				{
					for (uint32_t i = 0; i < SceneMaterials.Size; i++)
					{
						SceneMaterial* Material = (SceneMaterial*)CMA_GetAt(&SceneMaterials, i);
						if (Material != NULL)
						{
							if (Material->AlbedoIndex == SelectedTexture)
								Material->AlbedoIndex = 0;
							else if (Material->NormalIndex == SelectedTexture)
								Material->NormalIndex = 0;
							else if (Material->MetallicIndex == SelectedTexture)
								Material->MetallicIndex = 0;
							else if (Material->RoughnessIndex == SelectedTexture)
								Material->RoughnessIndex = 0;
							else if (Material->OcclusionIndex == SelectedTexture)
								Material->OcclusionIndex = 0;
						}
					}						

					TextureToDelete = Image->TextureImage;
					SamplerToDelete = Image->TextureSampler;

					CMA_Pop(&SceneTextures, SelectedTexture);
					
					for (uint32_t i = 1; i < SceneTextures.Size; i++)
						if (CMA_GetAt(&SceneTextures, i) != NULL)
							SelectedTexture = i;

					DeleteTexture = true;
				}
				ImGui::PopStyleColor();
			}
		}
	}
	ImGui::End();
}

void EditorMeshInspector()
{
	ImGui::Begin("Mesh Inspector");
	{
		if (SceneMeshes.Size > 1)
		{
			SceneMesh* Mesh = (SceneMesh*)CMA_GetAt(&SceneMeshes, SelectedMesh);
			if (Mesh != NULL)
			{
				ImGui::Text(Mesh->Path);
				ImGui::InputText("Name", Mesh->Name, MAX_CHAR_NAME_LENGTH);

				ImGui::Text("Sub Meshes");
				for (uint32_t i = 0; i < Mesh->MeshCount; i++)
				{
					ImGui::Text("\tMesh %d", i);

					ImGui::SetCursorPosX(66);
					EditorTextureCombo("Albedo", "A-SMat", &Mesh->MeshData[i].AlbedoIndex);
					ImGui::SetCursorPosX(66);
					EditorTextureCombo("Normal", "N-SMat", &Mesh->MeshData[i].NormalIndex);
					ImGui::SetCursorPosX(66);
					EditorTextureCombo("Metallic", "M-SMat", &Mesh->MeshData[i].MetallicIndex);
					ImGui::SetCursorPosX(66);
					ImGui::PushID("MS-SMat");
					ImGui::SliderFloat("Metallic Strength", &Mesh->MeshData[i].Metallic, 0.0, 1.0);
					ImGui::SetCursorPosX(66);
					EditorTextureCombo("Roughness", "R-SMat", &Mesh->MeshData[i].RoughnessIndex);
					ImGui::SetCursorPosX(66);
					ImGui::PushID("RS-SMat");
					ImGui::SliderFloat("Roughness Strength", &Mesh->MeshData[i].Roughness, 0.0, 1.0);
					ImGui::SetCursorPosX(66);
					EditorTextureCombo("Occlusion", "O-SMat", &Mesh->MeshData[i].OcclusionIndex);
					ImGui::SetCursorPosX(66);
					ImGui::PushID("OS-SMat");
					ImGui::SliderFloat("Occlusion Strength", &Mesh->MeshData[i].Occlusion, 0.0, 1.0);
					
				//	ImGui::SetCursorPosX(66);
				//	EditorTextureCombo("Albedo", &Mesh->MeshData[i].AlbedoIndex);
				//	ImGui::SetCursorPosX(66);
				//	EditorTextureCombo("Normal", &Mesh->MeshData[i].NormalIndex);
				//	ImGui::SetCursorPosX(66);
				//	EditorTextureCombo("Metallic", &Mesh->MeshData[i].MetallicIndex);
				//	ImGui::SetCursorPosX(66);
				//	ImGui::SliderFloat("Metallic Strength", &Mesh->MeshData[i].Metallic, 0.0, 1.0);
				//	ImGui::SetCursorPosX(66);
				//	EditorTextureCombo("Roughness", &Mesh->MeshData[i].RoughnessIndex);
				//	ImGui::SetCursorPosX(66);
				//	ImGui::SliderFloat("Roughness Strength", &Mesh->MeshData[i].Roughness, 0.0, 1.0);
				//	ImGui::SetCursorPosX(66);
				//	EditorTextureCombo("Occlusion", &Mesh->MeshData[i].OcclusionIndex);
				//	ImGui::SetCursorPosX(66);
				//	ImGui::SliderFloat("Occlusion Strength", &Mesh->MeshData[i].Occlusion, 0.0, 1.0);
				}
			}			

			ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 0, 0, 255));
			if (ImGui::Button("Delete Mesh"))
			{
				for (uint32_t i = 0; i < EntityCount; i++)
					if (Entities[i].UsedComponents[COMPONENT_TYPE_MESH] && Entities[i].Mesh.MeshIndex == SelectedMesh)
						ResetEntityMesh(&Entities[i]);

				MeshToDelete = SelectedMesh;
				DeleteMesh = true;
			}

			ImGui::PopStyleColor();
		}
	}
	ImGui::End();
}

void EditorScriptInspector()
{
	ImGui::Begin("Script Inspector");
	{
		if (SceneScriptCount > 0)
		{
			ImGui::InputText("Name", SceneScripts[SelectedScript].Name, MAX_CHAR_NAME_LENGTH);
			ImGui::InputText("Path", SceneScripts[SelectedScript].Path, MAX_CHAR_NAME_LENGTH);
			
			if (ImGui::Button("Save"))
			{
				if (strcmp(SceneScripts[SelectedScript].Path, "None") == 0)
				{
					char Path[MAX_CHAR_PATH_LENGTH];
					if (WaveSaveFileDialog(Path, "lua\0*.lua\0"))
					{
						strcpy(SceneScripts[SelectedScript].Path, Path);
						FILE* File = fopen(SceneScripts[SelectedScript].Path, "wb");
						fwrite(SceneScripts[SelectedScript].Script, 1, strlen(SceneScripts[SelectedScript].Script), File);
						fclose(File);
						WaveResetToLastPath();
					}
				}
				else
				{
					FILE* File = fopen(SceneScripts[SelectedScript].Path, "wb");
					fwrite(SceneScripts[SelectedScript].Script, 1, strlen(SceneScripts[SelectedScript].Script), File);
					fclose(File);
				}
									
			}
			ImGui::SameLine();
			if (ImGui::Button("Open"))
			{
				char Path[MAX_CHAR_PATH_LENGTH];
				if (WaveOpenFileDialog(Path, "lua\0*.lua\0"))
				{
					strcpy(SceneScripts[SelectedScript].Path, Path);
					size_t Size;
					strcpy(SceneScripts[SelectedScript].Script, OpenVkReadFileData(Path, &Size));
					WaveResetToLastPath();
				}
			}
			ImGui::SameLine();
			if (ImGui::Button("Save As"))
			{
				char Path[MAX_CHAR_PATH_LENGTH];
				if (WaveSaveFileDialog(Path, "lua\0*.lua\0"))
				{
					strcpy(SceneScripts[SelectedScript].Path, Path);
					FILE* File = fopen(SceneScripts[SelectedScript].Path, "wb");
					fwrite(SceneScripts[SelectedScript].Script, 1, strlen(SceneScripts[SelectedScript].Script), File);
					fclose(File);
					WaveResetToLastPath();
				}
			}

			ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(0, 255, 0, 255));
			ImVec2 Size = ImGui::GetWindowSize();
			ImGui::InputTextMultiline("Code", SceneScripts[SelectedScript].Script, MAX_CHAR_SCRIPT_LENGTH, ImVec2(Size.x, Size.y - 210), ImGuiInputTextFlags_AllowTabInput);
			ImGui::PopStyleColor();

			ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 0, 0, 255));
			if (ImGui::Button("Delete Script"))
			{
				for (uint32_t i = SelectedScript; i < SceneScriptCount - 1; i++)
					SceneScripts[i] = SceneScripts[i + 1];

				SceneScripts = (SceneScript*)realloc(SceneScripts, (SceneScriptCount - 1) * sizeof(SceneScript));

				SceneScriptCount--;
				SelectedScript = SceneScriptCount - 1;
			}


			ImGui::PopStyleColor();
		}
	}
	ImGui::End();
}

void EditorVSInspector()
{
	ImGui::Begin("VS Inspector");
	{
		static CMA_MemoryZone points = CMA_Create(sizeof(ImVec2));
	//	static ImVector<ImVec2> points;
		static ImVec2 scrolling(0.0f, 0.0f);
		static bool opt_enable_grid = true;
		static bool opt_enable_context_menu = true;
		static bool adding_line = false;

		ImGui::Checkbox("Enable grid", &opt_enable_grid);
		ImGui::Checkbox("Enable context menu", &opt_enable_context_menu);
		ImGui::Text("Mouse Left: drag to add lines,\nMouse Right: drag to scroll, click for context menu.");

		// Typically you would use a BeginChild()/EndChild() pair to benefit from a clipping region + own scrolling.
		// Here we demonstrate that this can be replaced by simple offsetting + custom drawing + PushClipRect/PopClipRect() calls.
		// To use a child window instead we could use, e.g:
		//      ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));      // Disable padding
		//      ImGui::PushStyleColor(ImGuiCol_ChildBg, IM_COL32(50, 50, 50, 255));  // Set a background color
		//      ImGui::BeginChild("canvas", ImVec2(0.0f, 0.0f), true, ImGuiWindowFlags_NoMove);
		//      ImGui::PopStyleColor();
		//      ImGui::PopStyleVar();
		//      [...]
		//      ImGui::EndChild();

		// Using InvisibleButton() as a convenience 1) it will advance the layout cursor and 2) allows us to use IsItemHovered()/IsItemActive()
		ImVec2 canvas_p0 = ImGui::GetCursorScreenPos();      // ImDrawList API uses screen coordinates!
		ImVec2 canvas_sz = ImGui::GetContentRegionAvail();   // Resize canvas to what's available
		if (canvas_sz.x < 50.0f) canvas_sz.x = 50.0f;
		if (canvas_sz.y < 50.0f) canvas_sz.y = 50.0f;
		ImVec2 canvas_p1 = ImVec2(canvas_p0.x + canvas_sz.x, canvas_p0.y + canvas_sz.y);

		// Draw border and background color
		ImGuiIO& io = ImGui::GetIO();
		ImDrawList* draw_list = ImGui::GetWindowDrawList();
		draw_list->AddRectFilled(canvas_p0, canvas_p1, IM_COL32(50, 50, 50, 255));
		draw_list->AddRect(canvas_p0, canvas_p1, IM_COL32(255, 255, 255, 255));

		// This will catch our interactions
		ImGui::InvisibleButton("canvas", canvas_sz, ImGuiButtonFlags_MouseButtonLeft | ImGuiButtonFlags_MouseButtonRight);
		const bool is_hovered = ImGui::IsItemHovered(); // Hovered
		const bool is_active = ImGui::IsItemActive();   // Held
		const ImVec2 origin(canvas_p0.x + scrolling.x, canvas_p0.y + scrolling.y); // Lock scrolled origin
		const ImVec2 mouse_pos_in_canvas(io.MousePos.x - origin.x, io.MousePos.y - origin.y);

		// Add first and second point
		if (is_hovered && !adding_line && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
		{
			CMA_Push(&points, (void*)&mouse_pos_in_canvas);
			CMA_Push(&points, (void*)&mouse_pos_in_canvas);
		//	points.push_back(mouse_pos_in_canvas);
		//	points.push_back(mouse_pos_in_canvas);
			adding_line = true;
		}

		if (adding_line)
		{
			CMA_Pop(&points, points.Size - 1);
			CMA_Push(&points, (void*)&mouse_pos_in_canvas);
		//	points.back() = mouse_pos_in_canvas;
			if (!ImGui::IsMouseDown(ImGuiMouseButton_Left))
				adding_line = false;
		}

		// Pan (we use a zero mouse threshold when there's no context menu)
		// You may decide to make that threshold dynamic based on whether the mouse is hovering something etc.
		const float mouse_threshold_for_pan = opt_enable_context_menu ? -1.0f : 0.0f;
		if (is_active && ImGui::IsMouseDragging(ImGuiMouseButton_Right, mouse_threshold_for_pan))
		{
			scrolling.x += io.MouseDelta.x;
			scrolling.y += io.MouseDelta.y;
		}

		// Context menu (under default mouse threshold)
		ImVec2 drag_delta = ImGui::GetMouseDragDelta(ImGuiMouseButton_Right);
		if (opt_enable_context_menu && ImGui::IsMouseReleased(ImGuiMouseButton_Right) && drag_delta.x == 0.0f && drag_delta.y == 0.0f)
			ImGui::OpenPopupOnItemClick("context");
		if (ImGui::BeginPopup("context"))
		{
		//	if (adding_line)
		//		points.resize(points.size() - 2);
			adding_line = false;
			if (ImGui::Selectable("Remove one")) 
			{
				CMA_Pop(&points, points.Size - 2);
				CMA_Pop(&points, points.Size - 1);
				
			//	points.resize(points.size() - 2); 
			}
			if (ImGui::Selectable("Remove all")) { CMA_Destroy(&points); }
			ImGui::EndPopup();
		}

		// Draw grid + all lines in the canvas
		draw_list->PushClipRect(canvas_p0, canvas_p1, true);
		if (opt_enable_grid)
		{
			const float GRID_STEP = 64.0f;
			for (float x = fmodf(scrolling.x, GRID_STEP); x < canvas_sz.x; x += GRID_STEP)
				draw_list->AddLine(ImVec2(canvas_p0.x + x, canvas_p0.y), ImVec2(canvas_p0.x + x, canvas_p1.y), IM_COL32(200, 200, 200, 40));
			for (float y = fmodf(scrolling.y, GRID_STEP); y < canvas_sz.y; y += GRID_STEP)
				draw_list->AddLine(ImVec2(canvas_p0.x, canvas_p0.y + y), ImVec2(canvas_p1.x, canvas_p0.y + y), IM_COL32(200, 200, 200, 40));
		}
		for (int n = 0; n < points.Size; n += 2)
		{
			ImVec2* point = (ImVec2*)CMA_GetAt(&points, n);
			ImVec2* point2 = (ImVec2*)CMA_GetAt(&points, n + 1);
			if (point != NULL && point2 != NULL)
				draw_list->AddLine(ImVec2(origin.x + point->x, origin.y + point->y), ImVec2(origin.x + point2->x, origin.y + point2->y), IM_COL32(255, 255, 0, 255), 2.0f);
		}
	//	printf("%zu/%zu\n", points.Size, points.AllocateSize);
		draw_list->PopClipRect();

		ImGui::EndTabItem();
	}
	ImGui::End();
}

void EditorInspector()
{
	EditorEntityInspector();
	EditorTextureInspector();
	EditorMeshInspector();
	EditorMaterialInspector();
	EditorScriptInspector();
	EditorVSInspector();
}