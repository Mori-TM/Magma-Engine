void SetDefaultMaterial(SceneMaterial* Material, const char* Name)
{
	strcpy(Material->Name, Name);
	Material->Color = Vec4f(1.0);
	Material->Metallic = 0.0;
	Material->Roughness = 1.0;
	Material->Occlusion = 1.0;
			
	Material->AlbedoIndex = 0;
	Material->NormalIndex = 0;
	Material->MetallicIndex = 0;
	Material->RoughnessIndex = 0;
	Material->OcclusionIndex = 0;
}

void AddMesh(const char* Name, SceneMesh* MeshInfo)
{
	CheckForSameNames(&SceneMeshes, Name, MeshInfo->Name);

	SelectedMesh = CMA_Push(&SceneMeshes, MeshInfo);
	ImGui::SetWindowFocus("Mesh Inspector");
}

void AddMaterial(const char* Name)
{
	SceneMaterial Material;
	SetDefaultMaterial(&Material, Name);

	CheckForSameNames(&SceneMaterials, Name, Material.Name);

	SelectedMaterial = CMA_Push(&SceneMaterials, &Material);
	ImGui::SetWindowFocus("Material Inspector");
}

uint32_t LoadTexture(char* Path, SceneTextureImage* Image)
{
	printf("%s\n", Path);
	OpenVkDeviceWaitIdle();

	unsigned char* Pixels = NULL;
	Image->Components = OPENVK_FORMAT_RGBA;
	
	if (OpenVkLoadTexture(Path, false, &Pixels, &Image->Width, &Image->Height, OPENVK_FORMAT_RGBA) == OpenVkTrue)
	{
		Image->TextureImage = OpenVkCreateTextureImage(Pixels, Image->Width, Image->Height, OPENVK_FORMAT_RGBA);
		OpenVkFree(Pixels);
		if (Image->TextureImage == OPENVK_ERROR)
			goto Error;

		uint32_t Filter = OPENVK_FILTER_LINEAR;
		if (Image->Width <= 64 && Image->Height <= 64)
			Filter = OPENVK_FILTER_NEAREST;

		Image->TextureSampler = OpenVkCreateImageSampler(Filter, OPENVK_ADDRESS_MODE_REPEAT);
	}
	else
	{
		Error:
		SceneTextureImage* DefaultImage = (SceneTextureImage*)CMA_GetAt(&SceneTextures, 0);
		if (DefaultImage != NULL)
		{
			memcpy(Image, DefaultImage, sizeof(SceneTextureImage));
			return DefaultImage->TextureDescriptorSet;
		}
		else
			printf("Du Kleiner Hurensohn bist gefickt\n");
	}		

	uint32_t DescriptorCounts[] = { 1 };
	uint32_t DescriptorTypes[] = { OPENVK_DESCRIPTOR_TYPE_IMAGE_SAMPLER };
	uint32_t ImageTypes[] = { OPENVK_IMAGE_TYPE_TEXTURE };
	uint32_t ImageLayouts[] = { OPENVK_IMAGE_LAYOUT_COLOR_OUTPUT };
	uint32_t Bindings[] = { 0 };

	OpenVkDescriptorSetCreateInfo DescriptorSetCreateInfo;
	DescriptorSetCreateInfo.DescriptorSetLayout = TextureDescriptorSetLayout;
	DescriptorSetCreateInfo.DescriptorPool = DescriptorPool;
	DescriptorSetCreateInfo.DescriptorWriteCount = 1;
	DescriptorSetCreateInfo.DescriptorCounts = DescriptorCounts;
	DescriptorSetCreateInfo.DescriptorTypes = DescriptorTypes;
	DescriptorSetCreateInfo.Sampler = &Image->TextureSampler;
	DescriptorSetCreateInfo.ImageTypes = ImageTypes;
	DescriptorSetCreateInfo.ImageLayouts = ImageLayouts;
	DescriptorSetCreateInfo.Bindings = Bindings;
	DescriptorSetCreateInfo.Images = &Image->TextureImage;
	DescriptorSetCreateInfo.DescriptorSet = NULL;

	return OpenVkCreateDescriptorSet(&DescriptorSetCreateInfo);
}


uint32_t AddTexture(char* Path, bool ShowInAssetBrowser)
{
	SceneTextureImage Image;
	Image.ShowInAssetBrowser = ShowInAssetBrowser;
	Image.TextureDescriptorSet = LoadTexture(Path, &Image);
	strcpy(Image.Path, Path);

	char* Name = GetFileNameFromPath(Path);
	CheckForSameNames(&SceneTextures, Name, Image.Name);

	SelectedTexture = CMA_Push(&SceneTextures, &Image);
	ImGui::SetWindowFocus("Texture Inspector");

	return SelectedTexture;
}

uint32_t AddAnimation(char* Path, int32_t TexWidth, int32_t TexHeight)
{
	SceneAnimation Animation;
	Animation.Start = 0;
	Animation.End = 1;
	Animation.Speed = 25.0;
	Animation.MeshData.NumFrames = 1;
	Md2::Load(Path, TexWidth, TexHeight, &Animation.MeshData);
	Animation.VertexBuffer = OpenVkCreateDynamicVertexBuffer(Animation.MeshData.NumTriangles * 3 * sizeof(SceneVertex));
	Animation.Vertices = (SceneVertex*)malloc(Animation.MeshData.NumTriangles * 3 * sizeof(SceneVertex));
	strcpy(Animation.Path, Path);

	char* Name = GetFileNameFromPath(Path);
	CheckForSameNames(&SceneAnimations, Name, Animation.Name);

	SelectedAnimation = CMA_Push(&SceneAnimations, &Animation);
	ImGui::SetWindowFocus("Animation Inspector");

	return SelectedAnimation;
}

void AddScript()
{
	SceneScripts = (SceneScript*)realloc(SceneScripts, (SceneScriptCount + 1) * sizeof(SceneScript));
	SelectedScript = SceneScriptCount;

	char* ScriptName = (char*)"Script";
	char* Script = (char*)"function Start()\nend\n\nfunction Update()\n\tx, y, z, rx, ry, rz, sx, sy, sz = GetEntity(\"Entity\")\n\tprint(x, y, z, rx, ry, rz, sx, sy, sz)\nend";
	strcpy(SceneScripts[SceneScriptCount].Name, ScriptName);
	strcpy(SceneScripts[SceneScriptCount].Path, "None");
	strcpy(SceneScripts[SceneScriptCount].Script, Script);

	uint32_t Count = 0;
	for (uint32_t i = 0; i < SceneScriptCount; i++)
		if (strstr(SceneScripts[i].Name, ScriptName) != 0)
			Count++;

	if (Count > 0)
		sprintf(SceneScripts[SceneScriptCount].Name, "%s (%d)", ScriptName, Count);
	else
		strcpy(SceneScripts[SceneScriptCount].Name, ScriptName);

	ImGui::SetWindowFocus("Script Inspector");

	SceneScriptCount++;
}

bool LoadModelWave(const char* Path, WaveModelData* ModelData, SceneMesh* MeshInfo)
{
	MeshInfo->Freeable = true;
	strcpy(MeshInfo->Path, Path);
	MeshInfo->MeshData = (SceneMeshData*)malloc(ModelData->MeshCount * sizeof(SceneMeshData));
	MeshInfo->MeshCount = ModelData->MeshCount;

	for (uint32_t i = 0; i < ModelData->MeshCount; i++)
	{
		WaveMeshData* WaveMesh = &ModelData->Meshes[i];
		SceneMeshData* SceneMesh = &MeshInfo->MeshData[i];

		SetDefaultMaterial(&SceneMesh->Material, "MESH");
		if (strcmp(ModelData->Materials[i].MaterialName, WaveEmptyMaterial.MaterialName) != 0)
			strncpy(SceneMesh->Material.Name, ModelData->Materials[i].MaterialName, sizeof(SceneMesh->Material.Name));

		SceneMesh->VertexCount = WaveMesh->VertexCount;
		SceneMesh->IndexCount = WaveMesh->IndexCount;

		SceneMesh->Vertices = (SceneVertex*)malloc(WaveMesh->VertexCount * sizeof(SceneVertex));
		SceneMesh->Indices = (uint32_t*)malloc(WaveMesh->IndexCount * sizeof(uint32_t));

		SceneMesh->Material.Metallic = ModelData->Materials[i].Metallic;
		SceneMesh->Material.Roughness = ModelData->Materials[i].Roughness;

		if (strcmp(ModelData->Materials[i].DiffuseTexture, "NoTexture") != 0) 
			SceneMesh->Material.AlbedoIndex = AddTexture(ModelData->Materials[i].DiffuseTexture, false);
		
		if (strcmp(ModelData->Materials[i].NormalTexture, "NoTexture") != 0)
			SceneMesh->Material.NormalIndex = AddTexture(ModelData->Materials[i].NormalTexture, false);
		else if (strcmp(ModelData->Materials[i].DisplacmentTexture, "NoTexture") != 0) 
			SceneMesh->Material.NormalIndex = AddTexture(ModelData->Materials[i].DisplacmentTexture, false);
		else if (strcmp(ModelData->Materials[i].BumpTexture, "NoTexture") != 0) 
			SceneMesh->Material.NormalIndex = AddTexture(ModelData->Materials[i].BumpTexture, false);
		
		if (strcmp(ModelData->Materials[i].RoughnessTexture, "NoTexture") != 0)
			SceneMesh->Material.RoughnessIndex = AddTexture(ModelData->Materials[i].RoughnessTexture, false);
		else if (strcmp(ModelData->Materials[i].SpecularTexture, "NoTexture") != 0) 
			SceneMesh->Material.RoughnessIndex = AddTexture(ModelData->Materials[i].SpecularTexture, false);

		if (strcmp(ModelData->Materials[i].MetallicTexture, "NoTexture") != 0)
			SceneMesh->Material.MetallicIndex = AddTexture(ModelData->Materials[i].MetallicTexture, false);

		SceneMesh->Material.Color.x = ModelData->Materials[i].DiffuseColor.x;
		SceneMesh->Material.Color.y = ModelData->Materials[i].DiffuseColor.y;
		SceneMesh->Material.Color.z = ModelData->Materials[i].DiffuseColor.z;
		SceneMesh->Material.Color.w = ModelData->Materials[i].Dissolve;

		for (uint32_t j = 0; j < SceneMesh->VertexCount; j++)
		{
			SceneMesh->Vertices[j].Pos.x = WaveMesh->Vertices[j].Vertices.x;
			SceneMesh->Vertices[j].Pos.y = WaveMesh->Vertices[j].Vertices.y;
			SceneMesh->Vertices[j].Pos.z = WaveMesh->Vertices[j].Vertices.z;
			SceneMesh->Vertices[j].Normal.x = WaveMesh->Vertices[j].Normals.x;
			SceneMesh->Vertices[j].Normal.y = WaveMesh->Vertices[j].Normals.y;
			SceneMesh->Vertices[j].Normal.z = WaveMesh->Vertices[j].Normals.z;
			SceneMesh->Vertices[j].TexCoord.x = WaveMesh->Vertices[j].TexCoords.x;
			SceneMesh->Vertices[j].TexCoord.y = WaveMesh->Vertices[j].TexCoords.y;
		}

		SceneMesh->VertexBuffer = OpenVkCreateVertexBuffer(SceneMesh->VertexCount * sizeof(SceneVertex), SceneMesh->Vertices);
		if (SceneMesh->VertexBuffer == OPENVK_ERROR)
			return false;

		SceneMesh->IndexCount = SceneMesh->IndexCount;
		SceneMesh->Indices = (uint32_t*)malloc(SceneMesh->IndexCount * sizeof(uint32_t));

		for (uint32_t i = 0; i < SceneMesh->IndexCount; i++)
			SceneMesh->Indices[i] = WaveMesh->Indices[i];

		SceneMesh->IndexBuffer = OpenVkCreateIndexBuffer(SceneMesh->IndexCount * sizeof(uint32_t), SceneMesh->Indices);
		if (SceneMesh->IndexBuffer == OPENVK_ERROR)
			return false;
	}
}

void LoadModel(uint32_t Settings, const char* FileName)
{
	char LastPath[MAX_CHAR_PATH_LENGTH];
	char Path[MAX_CHAR_PATH_LENGTH];
	strcpy(Path, FileName);
	for (uint32_t i = strlen(FileName) - 1; i > 0; i--)
		if (Path[i] == '\\' || Path[i] == '/')
		{
			Path[i] = '\0';
			break;
		}

	WaveGetPath(LastPath);
	WaveSetPath(Path);

	SceneMesh MeshInfo;

	uint32_t LastTexture = SelectedTexture;

	Settings |= WAVE_LOAD_MATERIAL | WAVE_GEN_NORMALS | WAVE_FLIP_UVS | WAVE_GEN_UVS | WAVE_GEN_INDICES | WAVE_REMOVE_REDUNDANT_MATERIALS | WAVE_PRINT_DEBUG_INOFS;
	WaveModelData Model = WaveLoadModel(FileName, Settings);

	if (LoadModelWave(FileName, &Model, &MeshInfo))
		AddMesh(GetFileNameFromPath((char*)FileName), &MeshInfo);

	WaveFreeModel(&Model);

	SelectedTexture = LastTexture;
	WaveSetPath(LastPath);
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

		//	ImGui::BeginChild("Asset Pop Up Child");
		{
			if (ImGui::BeginPopupContextWindow("Asset Pop Up"))
			{
				if (ImGui::Selectable("Add Script"))
				{
					AddScript();
				}

				if (ImGui::Selectable("Add Material"))
				{
					AddMaterial("Material");
				}

				if (ImGui::Selectable("Add Plane"))
				{
					SceneMesh MeshInfo;

					MeshInfo.Freeable = false;
					strcpy(MeshInfo.Path, "Plane");
					MeshInfo.MeshData = (SceneMeshData*)malloc(1 * sizeof(SceneMeshData));
					MeshInfo.MeshCount = 1;

					SetDefaultMaterial(&MeshInfo.MeshData[0].Material, "MESH");
					MeshInfo.MeshData[0].VertexCount = ARRAY_SIZE(PlaneVertices);
					MeshInfo.MeshData[0].Vertices = (SceneVertex*)&PlaneVertices;
					MeshInfo.MeshData[0].VertexBuffer = PlaneVertexBuffer;

					MeshInfo.MeshData[0].IndexCount = ARRAY_SIZE(PlaneIndices);
					MeshInfo.MeshData[0].Indices = (uint32_t*)&PlaneIndices;
					MeshInfo.MeshData[0].IndexBuffer = PlaneIndexBuffer;

					AddMesh("Plane", &MeshInfo);
				}

				if (ImGui::Selectable("Add Cube"))
				{
					SceneMesh MeshInfo;

					MeshInfo.Freeable = false;
					strcpy(MeshInfo.Path, "Cube");
					MeshInfo.MeshData = (SceneMeshData*)malloc(1 * sizeof(SceneMeshData));
					MeshInfo.MeshCount = 1;

					SetDefaultMaterial(&MeshInfo.MeshData[0].Material, "MESH");
					MeshInfo.MeshData[0].VertexCount = ARRAY_SIZE(CubeVertices);
					MeshInfo.MeshData[0].Vertices = (SceneVertex*)&CubeVertices;
					MeshInfo.MeshData[0].VertexBuffer = CubeVertexBuffer;

					MeshInfo.MeshData[0].IndexCount = 0;
					MeshInfo.MeshData[0].Indices = NULL;
					MeshInfo.MeshData[0].IndexBuffer = 0;

					AddMesh("Cube", &MeshInfo);
				}

				if (ImGui::Selectable("Add Sphere"))
				{
					SceneMesh MeshInfo;

					MeshInfo.Freeable = false;
					strcpy(MeshInfo.Path, "Sphere");
					MeshInfo.MeshData = (SceneMeshData*)malloc(1 * sizeof(SceneMeshData));
					MeshInfo.MeshCount = 1;

					SetDefaultMaterial(&MeshInfo.MeshData[0].Material, "MESH");
					MeshInfo.MeshData[0].VertexCount = SphereVertexCount;
					MeshInfo.MeshData[0].Vertices = SphereVertices;
					MeshInfo.MeshData[0].VertexBuffer = SphereVertexBuffer;

					MeshInfo.MeshData[0].IndexCount = SphereIndexCount;
					MeshInfo.MeshData[0].Indices = SphereIndices;
					MeshInfo.MeshData[0].IndexBuffer = SphereIndexBuffer;

					AddMesh("Sphere", &MeshInfo);
				}

				if (ImGui::Selectable("Add Bean"))
				{
					SceneMesh MeshInfo;

					MeshInfo.Freeable = false;
					strcpy(MeshInfo.Path, "Bean");
					MeshInfo.MeshData = (SceneMeshData*)malloc(1 * sizeof(SceneMeshData));
					MeshInfo.MeshCount = 1;

					SetDefaultMaterial(&MeshInfo.MeshData[0].Material, "MESH");
					MeshInfo.MeshData[0].VertexCount = BeanVertexCount;
					MeshInfo.MeshData[0].Vertices = BeanVertices;
					MeshInfo.MeshData[0].VertexBuffer = BeanVertexBuffer;

					MeshInfo.MeshData[0].IndexCount = BeanIndexCount;
					MeshInfo.MeshData[0].Indices = BeanIndices;
					MeshInfo.MeshData[0].IndexBuffer = BeanIndexBuffer;

					AddMesh("Bean", &MeshInfo);
				}

				if (ImGui::BeginMenu("Load Model"))
				{
					if (ImGui::MenuItem("Default")) ifd::FileDialog::Instance().Open("LoadDefaultModel", "Open Model", "All Models (*.obj;*.stl;*.gltf){.obj,.stl,.gltf},.*");
					if (ImGui::MenuItem("Gen Smooth Normals")) ifd::FileDialog::Instance().Open("LoadSmoothModel", "Open Model", "All Models (*.obj;*.stl;*.gltf){.obj,.stl,.gltf},.*");
					if (ImGui::MenuItem("Gen Flat Normals")) ifd::FileDialog::Instance().Open("LoadFlatModel", "Open Model", "All Models (*.obj;*.stl;*.gltf){.obj,.stl,.gltf},.*");
					ImGui::EndMenu();
				}

				if (ImGui::Selectable("Load Texture"))
					ifd::FileDialog::Instance().Open("LoadTexture", "Load Texture", "Image file (*.png;*.jpg;*.tga;*.jpeg;*.hdr;*.psd;*.bmp){.png,.jpg,.tga,.jpeg,.hdr,.psd,.bmp},.*");

				if (ImGui::Selectable("Load Animation"))
					ifd::FileDialog::Instance().Open("LoadAnimation", "Load Animation", "Quake Models (*.md2;*.bin){.md2,.bin},.*");
			}
			ImGui::EndPopup();
		}
		//	ImGui::EndChild();
	}
	ImGui::End();
}