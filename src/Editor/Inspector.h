void EditorEntityInspector()
{
	ImGui::Begin("Entity Inspector");
	{
		if (EntityCount > 0)
		{
			ImGui::InputText("Name", Entities[SelectedEntity].Name, 2048);

			if (ImGui::CollapsingHeader("TRANSFORM"))
			{
				ImGuiVec3Control("Translation", Entities[SelectedEntity].Translate.Arr, 0.0, 120.0);
				ImGuiVec3Control("Rotation", Entities[SelectedEntity].Rotate.Arr, 0.0, 120.0);
				ImGuiVec3Control("Scale", Entities[SelectedEntity].Scale.Arr, 1.0, 120.0);
				
				mat4 TransformMatrix;
				LoadMat4IdentityP(&TransformMatrix);
				TransformMatrix = ScaleMat4P	(&TransformMatrix, &Entities[SelectedEntity].Scale);
				TransformMatrix = RotateXMat4P	(&TransformMatrix, ToRadians(Entities[SelectedEntity].Rotate.x));
				TransformMatrix = RotateYMat4P	(&TransformMatrix, ToRadians(Entities[SelectedEntity].Rotate.y));
				TransformMatrix = RotateZMat4P	(&TransformMatrix, ToRadians(Entities[SelectedEntity].Rotate.z));
				TransformMatrix = TranslateMat4P(&TransformMatrix, &Entities[SelectedEntity].Translate);

				ImGui::Text("Matrix =\n\t[ %.2f, %.2f, %.2f, %.2f ]\n\t[ %.2f, %.2f, %.2f, %.2f ]\n\t[ %.2f, %.2f, %.2f, %.2f ]\n\t[ %.2f, %.2f, %.2f, %.2f ]", 
					TransformMatrix.m[0][0], TransformMatrix.m[0][1], TransformMatrix.m[0][2], TransformMatrix.m[0][3], 
					TransformMatrix.m[1][0], TransformMatrix.m[1][1], TransformMatrix.m[1][2], TransformMatrix.m[1][3], 
					TransformMatrix.m[2][0], TransformMatrix.m[2][1], TransformMatrix.m[2][2], TransformMatrix.m[2][3], 
					TransformMatrix.m[3][0], TransformMatrix.m[3][1], TransformMatrix.m[3][2], TransformMatrix.m[3][3]);


			//	ImGui::DragFloat3("Translation", (float*)&Entities[SelectedEntity].Translate, 0.1, -10000.0, 10000.0);
			//	ImGui::DragFloat3("Rotation", (float*)&Entities[SelectedEntity].Rotate, 0.1, -10000.0, 10000.0);
			//	ImGui::DragFloat3("Scale", (float*)&Entities[SelectedEntity].Scale, 0.1, -10000.0, 10000.0);
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
										AddMeshToEntity(SelectedEntity, i);
									//	Entities[SelectedEntity].Mesh.MeshIndex = i;
									//	strcpy(Entities[SelectedEntity].Mesh.Name, Mesh->Name);
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
									//	strcpy(Entities[SelectedEntity].Material.Name, Material->Name);
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

			if(Entities[SelectedEntity].UsedComponents[COMPONENT_TYPE_ANIMATION] == true)
			{
				if (ImGui::CollapsingHeader("ANIMATION"))
				{
					SceneAnimation* Animation = (SceneAnimation*)CMA_GetAt(&SceneAnimations, Entities[SelectedEntity].Animation.AnimationIndex);
					if (Animation != NULL)
					{
						if (ImGui::BeginCombo("Animation", Animation->Name))
						{
							for (uint32_t i = 0; i < SceneAnimations.Size; i++)
							{
								Animation = (SceneAnimation*)CMA_GetAt(&SceneAnimations, i);
								if (Animation != NULL)
								{
									if (ImGui::Button(Animation->Name))
									{
										Entities[SelectedEntity].Animation.AnimationIndex = i;
									//	strcpy(Entities[SelectedEntity].Animation.Name, Animation->Name);
										Animation->End = Animation->MeshData.NumFrames - 1;//maybe -1?
									}
								}
							}
							ImGui::EndCombo();
						}

						ImGui::DragInt("Start", (int*)&Animation->Start, 1.0, 0, Animation->MeshData.NumFrames - 1);
						ImGui::DragInt("End", (int*)&Animation->End, 1.0, 1, Animation->MeshData.NumFrames);
						ImGui::DragFloat("Speed", &Animation->Speed, 0.1, 0.1, 100.0);
					}

					ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 0, 0, 255));
					if (ImGui::Button("Remove Animation Component"))
					{
						Entities[SelectedEntity].UsedComponents[COMPONENT_TYPE_ANIMATION] = false;
					}
					ImGui::PopStyleColor();
				}
			}

			if (Entities[SelectedEntity].UsedComponents[COMPONENT_TYPE_LIGHT] == true)
			{
				if (ImGui::CollapsingHeader("LIGHT"))
				{
					ImGui::ColorEdit3("Color", (float*)&Entities[SelectedEntity].Light.Color);
					ImGui::DragFloat("Strength", &Entities[SelectedEntity].Light.Strength, 0.1, 0.01, 10000.0);					

					if (ImGui::BeginCombo("Light Type", LightNames[Entities[SelectedEntity].Light.Type]))
					{
						for (uint32_t i = 0; i < LIGHT_COUNT; i++)
						{
							if (ImGui::Button(LightNames[i]))
							{
								Entities[SelectedEntity].Light.Type = (LightTypes)i;
							}
						}
						ImGui::EndCombo();
					}
					if (Entities[SelectedEntity].Light.Type == 1)
						ImGui::Checkbox("Cast Shadow", &Entities[SelectedEntity].Light.CastShadow);

					ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 0, 0, 255));
					if (ImGui::Button("Remove Light Component"))
					{
						Entities[SelectedEntity].UsedComponents[COMPONENT_TYPE_LIGHT] = false;
					}
					ImGui::PopStyleColor();
				}
			}

			if (Entities[SelectedEntity].UsedComponents[COMPONENT_TYPE_COLLIDER] == true)
			{
				if (ImGui::CollapsingHeader("COLLIDER"))
				{
					if (ImGui::BeginCombo("Collider Type", ColliderNames[Entities[SelectedEntity].Collider.Type]))
					{
						for (uint32_t i = 0; i < COLLIDER_COUNT; i++)
						{
							if (ImGui::Button(ColliderNames[i]))
							{
								Entities[SelectedEntity].Collider.Type = (ColliderTypes)i;
							}
						}
						ImGui::EndCombo();
					}
					ImGui::DragFloat("Friction", &Entities[SelectedEntity].Collider.Friction, 0.001, 0.0, 1.0);
					ImGui::DragFloat("Bounciness", &Entities[SelectedEntity].Collider.Bounciness, 0.001, 0.0, 1.0);

					ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 0, 0, 255));
					if (ImGui::Button("Remove Collider Component"))
					{
						Entities[SelectedEntity].UsedComponents[COMPONENT_TYPE_COLLIDER] = false;
					}
					ImGui::PopStyleColor();
				}
			}
		
			if (Entities[SelectedEntity].UsedComponents[COMPONENT_TYPE_RIGIDBODY] == true)
			{
				if (ImGui::CollapsingHeader("RIGIDBODY"))
				{
					ImGui::DragFloat("Mass", &Entities[SelectedEntity].Rigidbody.Mass, 0.001, 0.0, 100000.0);

					ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 0, 0, 255));
					if (ImGui::Button("Remove Rigidbody Component"))
					{
						Entities[SelectedEntity].UsedComponents[COMPONENT_TYPE_RIGIDBODY] = false;
					}
					ImGui::PopStyleColor();
				}
			}

			if (ImGui::BeginPopupContextWindow("Entity Inspector Pop Up"))
			{				
				ImGui::Text("Components");
				
				for (uint32_t i = 1; i < COMPONENT_COUNT; i++)
				{
					if (ImGui::Button(ComponentNames[i]))
						Entities[SelectedEntity].UsedComponents[i] = true;
				}
			
				ImGui::NewLine();

				ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 255, 0, 255));
				if (ImGui::Button("Duplicate Entity"))
				{
				//	Entities = (EntityInfo*)realloc(Entities, (EntityCount + 1) * sizeof(EntityInfo));
					if (EntitiesCheckForResize())
					{
						Entities[EntityCount] = Entities[SelectedEntity];
						if (strstr(Entities[EntityCount].Name, ")") == 0)
						{
							ssprintf(Entities[EntityCount].Name, "%s Copy(1)", Entities[EntityCount].Name);
						}
						else
						{
							size_t Length = strlen(Entities[EntityCount].Name);
							char* StrPtr = Entities[EntityCount].Name;
							StrPtr += Length;
							while (*(--StrPtr) != '(');
							uint32_t Count = atoi(StrPtr + 1) + 1;

							size_t MaxLenth = ARRAY_SIZE(Entities[EntityCount].Name) - Length;
							snprintf(StrPtr, MaxLenth, "(%d)", Count);
							StrPtr[MaxLenth - 1] = '\0';
						}

						SelectedEntity = EntityCount;
						EntityCount++;
					}					
				}
				ImGui::PopStyleColor();

				ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 0, 0, 255));
				if (ImGui::Button("Delete Entity"))
				{
					for (uint32_t i = SelectedEntity; i < EntityCount - 1; i++)
						Entities[i] = Entities[i + 1];
			
				//	Entities = (EntityInfo*)realloc(Entities, (EntityCount - 1) * sizeof(EntityInfo));
			
					EntityCount--;
					SelectedEntity = EntityCount - 1;

					EntitiesCheckForResize();
				}
				ImGui::PopStyleColor();
				
				ImGui::EndPopup();
			}
		}
	}
	ImGui::End();
}

bool EditorTextureCombo(const char* Name, const char* ID, uint32_t* TextureIndex)
{
	bool Modification = false;

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
				//FIX - Legacy function ImageButton - imgui_widgets and imgui.h
				if (ImGui::ImageButton(Image->Name, &GetDescriptorSet(Image->TextureDescriptorSet)[0], ImVec2(30, 30)))
				{
					*TextureIndex = i;
					Modification = true;
				}

				ImGui::SameLine();
				ImGui::Text(Image->Name);
			}
		}
		ImGui::EndCombo();
	}
	ImGui::PopID();

	return Modification;
}

bool EditorMaterialCombo(const char* Name, size_t IDAdd, uint32_t* MaterialIndex)
{
	bool Modification = false;

	SceneMaterial* Material = (SceneMaterial*)CMA_GetAt(&SceneMaterials, *MaterialIndex);

	ImGui::PushID((void*)((char*)Material + IDAdd));
	if (ImGui::BeginCombo(Name, Material->Name))
	{
		for (uint32_t i = 0; i < SceneTextures.Size; i++)
		{
			Material = (SceneMaterial*)CMA_GetAt(&SceneMaterials, i);
			
			if (Material != NULL)
			{
				if (ImGui::Button(Material->Name))
				{
					*MaterialIndex = i;
					Modification = true;
				}
			}
		}
		ImGui::EndCombo();
	}
	ImGui::PopID();

	return Modification;
}

bool MaterialEditor(SceneMaterial* Material, float Offset)
{
	bool TextureChange = false;

	ImGui::SetCursorPosX(Offset); ImGui::ColorPicker4("Color", (float*)&Material->Color, ImGuiColorEditFlags_AlphaBar);
	ImGui::SetCursorPosX(Offset); EditorTextureCombo("Albedo", "A-Mat", &Material->AlbedoIndex) == (TextureChange = true);
	ImGui::SetCursorPosX(Offset); EditorTextureCombo("Normal", "N-Mat", &Material->NormalIndex) == (TextureChange = true);
	ImGui::SetCursorPosX(Offset); EditorTextureCombo("Metallic", "M-Mat", &Material->MetallicIndex) == (TextureChange = true);
	ImGui::SetCursorPosX(Offset); ImGui::SliderFloat("Metallic Strength", &Material->Metallic, 0.0, 1.0);
	ImGui::SetCursorPosX(Offset); EditorTextureCombo("Roughness", "R-Mat", &Material->RoughnessIndex) == (TextureChange = true);
	ImGui::SetCursorPosX(Offset); ImGui::SliderFloat("Roughness Strength", &Material->Roughness, 0.0, 1.0);
	ImGui::SetCursorPosX(Offset); EditorTextureCombo("Occlusion", "O-Mat", &Material->OcclusionIndex) == (TextureChange = true);
	ImGui::SetCursorPosX(Offset); ImGui::SliderFloat("Occlusion Strength", &Material->Occlusion, 0.0, 1.0);

	return TextureChange;
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
				ImGui::PushID(Material);
				ImGui::InputText("Name", Material->Name, MAX_CHAR_NAME_LENGTH);
				MaterialEditor(Material, ImGui::GetCursorPosX());

				ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 0, 0, 255));
				if (ImGui::Button("Delete Material"))
				{
					DeleteMaterial(SelectedMaterial);
				}
				if (ImGui::Button("[FIX - Implement] Delete Material with Textures"))
				{
					//FIX - currently no way to remove multiple texture at the same time
				}

				ImGui::PopStyleColor();
				ImGui::PopID();
			}			
		}
	}
	ImGui::End();
}

void EditorTextureInspector()
{
	ImGui::Begin("Texture Inspector");
	{
		if (SceneTextures.Size > 1 && SelectedTexture != 0)
		{
			SceneTextureImage* Image = (SceneTextureImage*)CMA_GetAt(&SceneTextures, SelectedTexture);
			// && Image->ShowInAssetBrowser
			if (Image != NULL)
			{
				ImGui::Text(Image->Path);
				ImGui::InputText("Name", Image->Name, MAX_CHAR_NAME_LENGTH);
				ImVec2 Size = ImGui::GetWindowSize();
				Size.x -= 45;
				Size.y = Size.x * ((float)Image->Height / (float)Image->Width);
				ImTextureID Tex = GetDescriptorSet(Image->TextureDescriptorSet);
				if (Tex)
					ImGui::Image(&((VkDescriptorSet*)Tex)[0], Size);								
				ImGui::Text("Width: %d", Image->Width);
				ImGui::Text("Height: %d", Image->Height);
				ImGui::Text("Aspect: %f", ((float)Image->Width / (float)Image->Height));
				ImGui::Text("Mip Levels: %d", Image->MipLevels);

				switch (Image->Format)
				{
				case OPENVK_FORMAT_RGB:
					ImGui::Text("Format: RGB");
					break;
				case OPENVK_FORMAT_RGBA:
					ImGui::Text("Format: RGBA");
					break;
				case OPENVK_FORMAT_BC1_RGB:
					ImGui::Text("Format: BC1/DXT1 RGB");
					break;
				case OPENVK_FORMAT_BC1_RGBA:
					ImGui::Text("Format: BC1/DXT1 RGBA");
					break;
				case OPENVK_FORMAT_BC4_RGBA:
					ImGui::Text("Format: BC4 RGBA");
					break;
				case OPENVK_FORMAT_BC7_RGBA:
					ImGui::Text("Format: BC7 RGBA");
					break;

				default:
					ImGui::Text("Format: unknown");
					break;
				}

				ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 0, 0, 255));
				if (ImGui::Button("Delete Texture"))
				{
					RequestTextureDeletion(SelectedTexture);
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
		if (SceneMeshes.Size > 1 && SelectedMesh != 0)
		{
			SceneMesh* Mesh = (SceneMesh*)CMA_GetAt(&SceneMeshes, SelectedMesh);
			if (Mesh != NULL)
			{
				ImGui::Text(Mesh->Path);
				ImGui::InputText("Name", Mesh->Name, MAX_CHAR_NAME_LENGTH);

				char MeshName[128];
				if (ImGui::CollapsingHeader("SUB MESHES"))
				{
					for (uint32_t i = 0; i < Mesh->MeshCount; i++)
					{						
						ImGui::Text("Vertex Count: %d", Mesh->MeshData[i].VertexCount);
						ImGui::Text("Index Count: %d", Mesh->MeshData[i].IndexCount);

						SceneMaterial* Material = (SceneMaterial*)CMA_GetAt(&SceneMaterials, Mesh->MeshData[i].MaterialIndex);
						if (Material)
						{
							ssprintf(MeshName, "%d\t%s", i, Mesh->MeshData[i].Name);

							ImGui::SetCursorPosX(33);
							if (ImGui::CollapsingHeader(MeshName))
							{
								ImGui::SetCursorPosX(66);
								EditorMaterialCombo("Material", i, &Mesh->MeshData[i].MaterialIndex);

								if (Mesh->MeshData[i].MaterialIndex != 0)
								{
									ImGui::SetCursorPosX(66);
									ImGui::PushID(Mesh->MeshData[i].Name);
									if (ImGui::CollapsingHeader("Material Options"))
									{
										ImGui::PushID(Material);
										if (MaterialEditor(Material, 99))
										{
										//	Mesh->MeshData[i].
											Material->DescriptorSet = UpdatePBRTextureDescriptorSetFromMaterial(Material->DescriptorSet, Mesh->MeshData[i].MaterialIndex);
										}
										ImGui::PopID();
									}
									ImGui::PopID();
								}								
							}
						}																	
					}
				}				
			}			

			ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 0, 0, 255));
			if (ImGui::Button("Delete Mesh"))
				RequestMeshDeletion(SelectedMesh, MODEL_DELETE_MESH);

			if (ImGui::Button("Delete Mesh with Textures"))
				RequestMeshDeletion(SelectedMesh, MODEL_DELETE_MESH | MODEL_DELETE_TEXURES);
			
			if (ImGui::Button("Delete Mesh with Textures and Materials"))
				RequestMeshDeletion(SelectedMesh, MODEL_DELETE_MESH | MODEL_DELETE_TEXURES | MODEL_DELETE_MATERIALS);

			ImGui::PopStyleColor();
		}
	}
	ImGui::End();
}

void CodeEditor(SceneScript* Script)
{
	ImDrawList* DrawList = ImGui::GetWindowDrawList();
	DrawList->AddRect(ImVec2(0.0, 0.0), ImVec2(100.0, 100.0), IM_COL32(255, 255, 255, 255));
	

	char NewLine[MAX_CHAR_LINE_LENGTH];
	for (uint32_t i = 0; i < Script->LineCount; i++)
	{
		ImGui::PushID((void*)Script->Lines[i].Line);
		ImGui::InputText("", Script->Lines[i].Line, MAX_CHAR_LINE_LENGTH);
		ImGui::PopID();
	}


//	ImGui::InputText("##NewScriptLine", NewLine, MAX_CHAR_LINE_LENGTH);

	ImGui::NewLine();
	ImGui::Text("%d Lines", Script->LineCount);
//	for (uint32_t i = 0; i < Lines; i++)
//	{
//		ImGui::Text("%d   ", i);
//		ImGui::SameLine();
//	}
		
}

void EditorScriptInspector()
{
	ImGui::Begin("Script Inspector");
	{
		if (SceneScripts.Size > 1 && SelectedScript != 0)
		{
			SceneScript* Script = (SceneScript*)CMA_GetAt(&SceneScripts, SelectedScript);

			if (Script)
			{
				ImGui::InputText("Name##ScrInspName", Script->Name, MAX_CHAR_NAME_LENGTH);
				ImGui::InputText("Path##ScrInspPath", Script->Path, MAX_CHAR_NAME_LENGTH);
				
				if (ImGui::Button("Save"))
				{
					if (strcmp(Script->Path, "None") == 0 || strlen(Script->Path) == 0)
					{
						#ifdef LINUX_PORT
						ifd::FileDialog::Instance().Save("SaveScript", "Save Script", "All Scripts (*.lua;*.txt){.lua,.txt},.*");				
						#endif
					}
					else
					{
						#ifdef LINUX_PORT
						SaveScript(NULL);
						#endif
					}
				}
				ImGui::SameLine();
				if (ImGui::Button("Open"))
				{
					#ifdef LINUX_PORT
					ifd::FileDialog::Instance().Open("LoadScript", "Open Script", "All Scripts (*.lua;*.txt){.lua,.txt},.*", false);
					#endif
				}
				ImGui::SameLine();
				if (ImGui::Button("Save As"))
				{
					#ifdef LINUX_PORT
					ifd::FileDialog::Instance().Save("SaveScript", "Save Script", "All Scripts (*.lua;*.txt){.lua,.txt},.*");
					#endif
				}

				ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(0, 255, 0, 255));
				ImVec2 Size = ImGui::GetWindowSize();
			//	ImGui::InputTextMultiline("Code", SceneScripts[SelectedScript].Script, MAX_CHAR_SCRIPT_LENGTH, ImVec2(Size.x, Size.y - 230), ImGuiInputTextFlags_AllowTabInput);
				CodeEditor(Script);
			//	ImGui::NewLine();
			//	ImGui::InputTextMultiline("Code", SceneScripts[SelectedScript].Script, MAX_CHAR_SCRIPT_LENGTH, ImVec2(Size.x, Size.y - 230), ImGuiInputTextFlags_AllowTabInput);
				ImGui::PopStyleColor();

				ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 0, 0, 255));
				if (ImGui::Button("Delete Script"))
				{
					free(Script->Lines);
					CMA_Pop(&SceneScripts, SelectedScript);
					SelectedScript = SelectedScript - 1;
				}


				ImGui::PopStyleColor();
			}

			
		}
	}
	ImGui::End();
}

void EditorVSInspector()
{
	ImGui::Begin("VS Inspector");
	{
		static CMA_MemoryZone points = CMA_Create(sizeof(ImVec2), "Magma Engine, Points");
	//	static ImVector<ImVec2> points;
		static ImVec2 scrolling(0.0f, 0.0f);
		static bool opt_enable_grid = true;
		static bool opt_enable_context_menu = true;
		static bool adding_line = false;

		ImGui::Text("Used Size: %lu", points.Size);
	//	ImGui::Text("Allocated Size: %d", points.BlockCount);//AllocateBlock

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
	EditorMeshInspector();
	EditorTextureInspector();
	EditorMaterialInspector();
	EditorScriptInspector();
//	EditorVSInspector();
}