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
	Mutex.lock();
	CheckForSameNames(&SceneMeshes, Name, MeshInfo->Name);

	SelectedMesh = CMA_Push(&SceneMeshes, MeshInfo);
	Mutex.unlock();
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
//	LoadTextureCompressed = true;

	printf("%s\n", Path);
	OpenVkDeviceWaitIdle();

	unsigned char* Pixels = NULL;
	Image->Components = OPENVK_FORMAT_RGBA;
	
//	LoadTextureCompressed = true;

	if (OpenVkLoadTexture(Path, false, &Pixels, &Image->Width, &Image->Height, OPENVK_FORMAT_RGBA) == OpenVkTrue)
	{
		if (Image->Width % 4 != 0 ||
			Image->Height % 4 != 0)
			LoadTextureCompressed = false;

		if (LoadTextureCompressed)
		{
			/*
			int32_t Size = Image->Width * Image->Height;

			void* Block = malloc(Size);

			bc7enc_compress_block_init();
			bc7enc_compress_block_params Para;
			Para.m_max_partitions_mode = BC7ENC_MAX_PARTITIONS1 / 2;
			Para.m_mode_partition_estimation_filterbank = 1;
			Para.m_perceptual = 0;
			Para.m_try_least_squares = 0;//
			Para.m_uber_level = BC7ENC_MAX_UBER_LEVEL / 2;
			Para.m_use_mode7_for_alpha = 1;
			Para.m_weights[0] = 1;
			Para.m_weights[1] = 1;
			Para.m_weights[2] = 1;
			Para.m_weights[3] = 1;
			bc7enc_compress_block_params_init(&Para);
			int i = bc7enc_compress_block(Block, Pixels, &Para);

			Image->TextureImage = OpenVkCreateTextureImage((unsigned char*)Block, Image->Width, Image->Height, OPENVK_FORMAT_BC7_RGBA);
			OpenVkFree(Pixels);
			OpenVkFree(Block);
			*/

		//	int32_t Size = (Image->Width / 4) * (Image->Height / 4) * 8;

		//	unsigned char* Block = (unsigned char*)malloc(Size);

		//	stb_compress_dxt_block(Block, Pixels, 1, STB_DXT_HIGHQUAL);

			Image->Components = OPENVK_FORMAT_BC1_RGB;

			size_t BlockSize = 0;
			unsigned char* Block;
			compress_image(Pixels, Image->Width, Image->Height, &Block, &BlockSize, Image->Components);

			
			Image->TextureImage = OpenVkCreateTextureImage((unsigned char*)Block, Image->Width, Image->Height, Image->Components);
			Image->MipLevels = VkRenderer.MipLevels;
			OpenVkFree(Pixels);
			OpenVkFree(Block);
			printf("Its compressed\n");
		}
		else
		{
			Image->TextureImage = OpenVkCreateTextureImage(Pixels, Image->Width, Image->Height, OPENVK_FORMAT_RGBA);
			Image->MipLevels = VkRenderer.MipLevels;
			OpenVkFree(Pixels);
		}

		
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
	DescriptorSetCreateInfo.VariableDescriptorSetCount = 0;

	LoadTextureCompressed = false;

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
	MeshInfo->Destroyable = true;
	strcpy(MeshInfo->Path, Path);
	MeshInfo->MeshData = (SceneMeshData*)malloc(ModelData->MeshCount * sizeof(SceneMeshData));
	MeshInfo->MeshCount = ModelData->MeshCount;

	uint32_t VertexCount = 0;
	uint32_t IndexCount = 0;

	for (uint32_t i = 0; i < ModelData->MeshCount; i++)
	{
		WaveMeshData* WaveMesh = &ModelData->Meshes[i];
		VertexCount += WaveMesh->VertexCount;
		IndexCount += WaveMesh->IndexCount;
	}

	SceneVertex* Vertices = (SceneVertex*)malloc(VertexCount * sizeof(SceneVertex));
	uint32_t* Indices = (uint32_t*)malloc(IndexCount * sizeof(uint32_t));

	VertexCount = 0;
	IndexCount = 0;

	for (uint32_t i = 0; i < ModelData->MeshCount; i++)
	{
		WaveMeshData* WaveMesh = &ModelData->Meshes[i];
		SceneMeshData* SceneMesh = &MeshInfo->MeshData[i];
		memset(&SceneMesh->Render, 1, ARRAY_SIZE(SceneMesh->Render) * sizeof(bool));

		SetDefaultMaterial(&SceneMesh->Material, "MESH");
		if (strcmp(ModelData->Materials[i].MaterialName, WaveEmptyMaterial.MaterialName) != 0)
			strncpy(SceneMesh->Material.Name, ModelData->Materials[i].MaterialName, sizeof(SceneMesh->Material.Name));

		SceneMesh->VertexCount = WaveMesh->VertexCount;
		SceneMesh->IndexCount = WaveMesh->IndexCount;

		SceneMesh->VertexOffset = VertexCount;
		SceneMesh->IndexOffset = IndexCount;
		
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
			Vertices[VertexCount].Pos.x = WaveMesh->Vertices[j].Vertices.x;
			Vertices[VertexCount].Pos.y = WaveMesh->Vertices[j].Vertices.y;
			Vertices[VertexCount].Pos.z = WaveMesh->Vertices[j].Vertices.z;
			Vertices[VertexCount].Normal.x = WaveMesh->Vertices[j].Normals.x;
			Vertices[VertexCount].Normal.y = WaveMesh->Vertices[j].Normals.y;
			Vertices[VertexCount].Normal.z = WaveMesh->Vertices[j].Normals.z;
			Vertices[VertexCount].TexCoord.x = WaveMesh->Vertices[j].TexCoords.x;
			Vertices[VertexCount].TexCoord.y = WaveMesh->Vertices[j].TexCoords.y;
			VertexCount++;
		}

		for (uint32_t j = 0; j < SceneMesh->IndexCount; j++)
			Indices[IndexCount++] = WaveMesh->Indices[j];
			
		GenerateAABB(&MeshInfo->MeshData[i].AABB, SceneMesh->VertexCount, Vertices + SceneMesh->VertexOffset);

	//	VertexCount += SceneMesh->VertexCount;
	//	IndexCount += SceneMesh->IndexCount;
	}

	MeshInfo->VertexBuffer = OpenVkCreateVertexBuffer(VertexCount * sizeof(SceneVertex), Vertices);
	if (MeshInfo->VertexBuffer == OPENVK_ERROR)
		return false;

	MeshInfo->IndexBuffer = OpenVkCreateIndexBuffer(IndexCount * sizeof(uint32_t), Indices);
	if (MeshInfo->IndexBuffer == OPENVK_ERROR)
		return false;

	free(Vertices);
	free(Indices);
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

//	WaveGetPath(LastPath);
//	WaveSetPath(Path);

	SceneMesh MeshInfo;

	uint32_t LastTexture = SelectedTexture;

	Settings |= WAVE_LOAD_MATERIAL | WAVE_GEN_NORMALS | WAVE_FLIP_UVS | WAVE_GEN_UVS | WAVE_GEN_INDICES | WAVE_MATERIAL_USE_MODEL_PATH | WAVE_REMOVE_REDUNDANT_MATERIALS | WAVE_PRINT_DEBUG_INOFS;
	WaveModelData Model = WaveLoadModel(FileName, Settings);
	if (Model.MeshCount == 0)
		return;

	if (LoadModelWave(FileName, &Model, &MeshInfo))
		AddMesh(GetFileNameFromPath((char*)FileName), &MeshInfo);

	WaveFreeModel(&Model);

	SelectedTexture = LastTexture;
//	WaveSetPath(LastPath);
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
			}

			if (ImGui::Selectable("Add Material"))
			{
				AddMaterial("Material");
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
			}

			if (ImGui::BeginMenu("Load Model"))
			{
				if (ImGui::MenuItem("Default")) ifd::FileDialog::Instance().Open("LoadDefaultModel", "Open Model", "All Models (*.obj;*.stl;*.gltf){.obj,.stl,.gltf},.*", true);
				if (ImGui::MenuItem("Gen Smooth Normals")) ifd::FileDialog::Instance().Open("LoadSmoothModel", "Open Model", "All Models (*.obj;*.stl;*.gltf){.obj,.stl,.gltf},.*", true);
				if (ImGui::MenuItem("Gen Flat Normals")) ifd::FileDialog::Instance().Open("LoadFlatModel", "Open Model", "All Models (*.obj;*.stl;*.gltf){.obj,.stl,.gltf},.*", true);
				ImGui::EndMenu();
			}

			if (ImGui::Selectable("Load Texture"))
				ifd::FileDialog::Instance().Open("LoadTexture", "Load Texture", "Image file (*.png;*.jpg;*.tga;*.jpeg;*.hdr;*.psd;*.bmp){.png,.jpg,.tga,.jpeg,.hdr,.psd,.bmp},.*", true);

			if (ImGui::Selectable("Load Animation"))
				ifd::FileDialog::Instance().Open("LoadAnimation", "Load Animation", "Quake Models (*.md2;*.bin){.md2,.bin},.*", true);

			ImGui::EndPopup();
		}			
	}
	ImGui::End();
}