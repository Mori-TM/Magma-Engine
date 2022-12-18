void AddMesh(const char* Name, SceneMesh* MeshInfo)
{
	CheckForSameNames(&SceneMeshes, Name, MeshInfo->Name);

	SelectedMesh = CMA_Push(&SceneMeshes, MeshInfo);
	ImGui::SetWindowFocus("Mesh Inspector");
}

void AddMaterial(const char* Name)
{
	SceneMaterial Material;
//	strcpy(Material.AlbedoName, "None");
//	strcpy(Material.NormalName, "None");
//	strcpy(Material.MetallicName, "None");
//	strcpy(Material.RoughnessName, "None");
//	strcpy(Material.OcclusionName, "None");

	Material.Color = Vec4f(1.0);
	Material.Metallic = 0.0;
	Material.Roughness = 1.0;
	Material.Occlusion = 1.0;

	Material.AlbedoIndex = 0;
	Material.NormalIndex = 0;
	Material.MetallicIndex = 0;
	Material.RoughnessIndex = 0;
	Material.OcclusionIndex = 0;

	CheckForSameNames(&SceneMaterials, Name, Material.Name);

	SelectedMaterial = CMA_Push(&SceneMaterials, &Material);
	ImGui::SetWindowFocus("Material Inspector");
}

uint32_t LoadTexture(char* Path, uint32_t* TextureImage, uint32_t* TextureSampler)
{
	printf("%s\n", Path);
	vkDeviceWaitIdle(VkRenderer.Device);

	unsigned char* Pixels = NULL;
	int32_t Width;
	int32_t Height;
	
	
	if (OpenVkLoadTexture(Path, false, &Pixels, &Width, &Height, OPENVK_FORMAT_RGBA) == OpenVkTrue)
	{
		*TextureImage = OpenVkCreateTextureImage(Pixels, Width, Height, OPENVK_FORMAT_RGBA);
		if (*TextureImage == OPENVK_ERROR)
			goto Error;
		*TextureSampler = OpenVkCreateImageSampler(OPENVK_FILTER_LINEAR, OPENVK_ADDRESS_MODE_REPEAT);
	}
	else
	{
		Error:
		SceneTextureImage* Image = (SceneTextureImage*)CMA_GetAt(&SceneTextures, 0);
		if (Image != NULL)
		{
			*TextureImage = Image->TextureImage;
			*TextureSampler = Image->TextureSampler;
			return Image->TextureDescriptorSet;
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
	DescriptorSetCreateInfo.Sampler = TextureSampler;
	DescriptorSetCreateInfo.ImageTypes = ImageTypes;
	DescriptorSetCreateInfo.ImageLayouts = ImageLayouts;
	DescriptorSetCreateInfo.Bindings = Bindings;
	DescriptorSetCreateInfo.Images = TextureImage;
	DescriptorSetCreateInfo.DescriptorSet = NULL;

	return OpenVkCreateDescriptorSet(&DescriptorSetCreateInfo);
}


uint32_t AddTexture(char* Path, bool ShowInAssetBrowser)
{
	SceneTextureImage Image;
	Image.ShowInAssetBrowser = ShowInAssetBrowser;
	Image.TextureDescriptorSet = LoadTexture(Path, &Image.TextureImage, &Image.TextureSampler);
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
/*
bool LoadModel(const char* Path, WaveModelData* ModelData, SceneMesh* MeshInfo)
{
	MeshInfo->Freeable = true;
	strcpy(MeshInfo->Path, Path);
	MeshInfo->MeshData = (SceneMeshData*)malloc((ModelData->MaterialCount + 1) * sizeof(SceneMeshData));
	MeshInfo->MeshCount = ModelData->MaterialCount;

	if (ModelData->MaterialCount > 0)
	{
		for (uint32_t i = 0; i < ModelData->MaterialCount; i++)
		{
			MeshInfo->MeshData[i].Color = Vec4f(1.0);
			MeshInfo->MeshData[i].Metallic = 0.0;
			MeshInfo->MeshData[i].Roughness = 1.0;
			MeshInfo->MeshData[i].Occlusion = 1.0;

			MeshInfo->MeshData[i].AlbedoIndex = 0;
			MeshInfo->MeshData[i].NormalIndex = 0;
			MeshInfo->MeshData[i].MetallicIndex = 0;
			MeshInfo->MeshData[i].RoughnessIndex = 0;
			MeshInfo->MeshData[i].OcclusionIndex = 0;

			MeshInfo->MeshData[i].IndexCount = 0;
			MeshInfo->MeshData[i].Indices = NULL;
			MeshInfo->MeshData[i].IndexBuffer = 0;
			MeshInfo->MeshData[i].VertexCount = 0;
		}

		uint32_t Count = 0;
		for (uint32_t i = 0; i < ModelData->MaterialRefrenceCount; i++)
		{
			if (i + 1 != ModelData->MaterialRefrenceCount)
				MeshInfo->MeshData[ModelData->MaterialRefrences[i].MaterialIndex].VertexCount += ModelData->MaterialRefrences[i + 1].VertexIndex - ModelData->MaterialRefrences[i].VertexIndex;
			else
				MeshInfo->MeshData[ModelData->MaterialRefrences[i].MaterialIndex].VertexCount += ModelData->VertexCount - ModelData->MaterialRefrences[i].VertexIndex;
		}

		uint32_t AlbedoCount = 0;
		uint32_t NormalCount = 0;
		uint32_t MetallicCount = 0;
		uint32_t RoughnessCount = 0;
		uint32_t OcclusionCount = 0;
		uint32_t* Albedos = (uint32_t*)calloc(ModelData->MaterialCount, sizeof(uint32_t));
		uint32_t* Normals = (uint32_t*)calloc(ModelData->MaterialCount, sizeof(uint32_t));
		uint32_t* Metallics = (uint32_t*)calloc(ModelData->MaterialCount, sizeof(uint32_t));
		uint32_t* Roughnesses = (uint32_t*)calloc(ModelData->MaterialCount, sizeof(uint32_t));
		uint32_t* Occlusions = (uint32_t*)calloc(ModelData->MaterialCount, sizeof(uint32_t));

		for (uint32_t i = 0; i < ModelData->MaterialCount; i++)
		{
			MeshInfo->MeshData[i].Vertices = (SceneVertex*)malloc(MeshInfo->MeshData[i].VertexCount * sizeof(SceneVertex));
			MeshInfo->MeshData[i].VertexCount = 0;

			if (strcmp(ModelData->Materials[i].DiffuseTexture, "NoTexture") != 0) Albedos[i] = AddTexture(ModelData->Materials[i].DiffuseTexture, false);
			if (strcmp(ModelData->Materials[i].NormalTexture, "NoTexture") != 0) Normals[i] = AddTexture(ModelData->Materials[i].NormalTexture, false);
			else if (strcmp(ModelData->Materials[i].BumpTexture, "NoTexture") != 0) Normals[i] = AddTexture(ModelData->Materials[i].BumpTexture, false);
			if (strcmp(ModelData->Materials[i].SpecularTexture, "NoTexture") != 0) Roughnesses[i] = AddTexture(ModelData->Materials[i].SpecularTexture, false);
		}

		uint32_t VertexIndex = 0;
		for (uint32_t i = 0; i < ModelData->MaterialRefrenceCount; i++)
		{
			uint32_t End = 0;
			if (i + 1 != ModelData->MaterialRefrenceCount)
				End = ModelData->MaterialRefrences[i + 1].VertexIndex;
			else
				End = ModelData->VertexCount;

			uint32_t j = ModelData->MaterialRefrences[i].VertexIndex;

			for (; j < End; j++)
			{
				VertexIndex = MeshInfo->MeshData[ModelData->MaterialRefrences[i].MaterialIndex].VertexCount;
				MeshInfo->MeshData[ModelData->MaterialRefrences[i].MaterialIndex].Vertices[VertexIndex].Pos.x = ModelData->Vertices[j].Vertices.x;
				MeshInfo->MeshData[ModelData->MaterialRefrences[i].MaterialIndex].Vertices[VertexIndex].Pos.y = ModelData->Vertices[j].Vertices.y;
				MeshInfo->MeshData[ModelData->MaterialRefrences[i].MaterialIndex].Vertices[VertexIndex].Pos.z = ModelData->Vertices[j].Vertices.z;

				MeshInfo->MeshData[ModelData->MaterialRefrences[i].MaterialIndex].Vertices[VertexIndex].Normal.x = ModelData->Vertices[j].Normals.x;
				MeshInfo->MeshData[ModelData->MaterialRefrences[i].MaterialIndex].Vertices[VertexIndex].Normal.y = ModelData->Vertices[j].Normals.y;
				MeshInfo->MeshData[ModelData->MaterialRefrences[i].MaterialIndex].Vertices[VertexIndex].Normal.z = ModelData->Vertices[j].Normals.z;

				MeshInfo->MeshData[ModelData->MaterialRefrences[i].MaterialIndex].Vertices[VertexIndex].TexCoord.x = ModelData->Vertices[j].TexCoords.x;
				MeshInfo->MeshData[ModelData->MaterialRefrences[i].MaterialIndex].Vertices[VertexIndex].TexCoord.y = ModelData->Vertices[j].TexCoords.y;

				MeshInfo->MeshData[ModelData->MaterialRefrences[i].MaterialIndex].Color.x = ModelData->Materials[ModelData->Vertices[j].MaterialIndex].DiffuseColor.x;
				MeshInfo->MeshData[ModelData->MaterialRefrences[i].MaterialIndex].Color.y = ModelData->Materials[ModelData->Vertices[j].MaterialIndex].DiffuseColor.y;
				MeshInfo->MeshData[ModelData->MaterialRefrences[i].MaterialIndex].Color.z = ModelData->Materials[ModelData->Vertices[j].MaterialIndex].DiffuseColor.z;
				MeshInfo->MeshData[ModelData->MaterialRefrences[i].MaterialIndex].Color.w = ModelData->Materials[ModelData->Vertices[j].MaterialIndex].Dissolve;

				MeshInfo->MeshData[ModelData->MaterialRefrences[i].MaterialIndex].AlbedoIndex = Albedos[ModelData->Vertices[j].MaterialIndex];
				MeshInfo->MeshData[ModelData->MaterialRefrences[i].MaterialIndex].NormalIndex = Normals[ModelData->Vertices[j].MaterialIndex];
				MeshInfo->MeshData[ModelData->MaterialRefrences[i].MaterialIndex].MetallicIndex = Metallics[ModelData->Vertices[j].MaterialIndex];
				MeshInfo->MeshData[ModelData->MaterialRefrences[i].MaterialIndex].RoughnessIndex = Roughnesses[ModelData->Vertices[j].MaterialIndex];
				MeshInfo->MeshData[ModelData->MaterialRefrences[i].MaterialIndex].OcclusionIndex = Occlusions[ModelData->Vertices[j].MaterialIndex];

				MeshInfo->MeshData[ModelData->MaterialRefrences[i].MaterialIndex].VertexCount++;
			}
		}

		uint32_t j = 0;
		for (uint32_t i = 0; i < ModelData->MaterialCount; i++)
		{
			if (MeshInfo->MeshData[i].VertexCount > 0)
			{
				MeshInfo->MeshData[j].VertexBuffer = OpenVkCreateVertexBuffer(MeshInfo->MeshData[i].VertexCount * sizeof(SceneVertex), MeshInfo->MeshData[i].Vertices);
				if (MeshInfo->MeshData[j].VertexBuffer == OPENVK_ERROR)
					return false;

				j++;
			}
		}
		MeshInfo->MeshCount = j;

		free(Occlusions);
		free(Roughnesses);
		free(Metallics);
		free(Normals);
		free(Albedos);
	}
	else
	{
		MeshInfo->MeshData[0].VertexCount = ModelData->VertexCount;
		MeshInfo->MeshData[0].Vertices = (SceneVertex*)malloc(ModelData->VertexCount * sizeof(SceneVertex));

		for (uint32_t i = 0; i < ModelData->VertexCount; i++)
		{
			MeshInfo->MeshData[0].Vertices[i].Pos.x = ModelData->Vertices[i].Vertices.x;
			MeshInfo->MeshData[0].Vertices[i].Pos.y = ModelData->Vertices[i].Vertices.y;
			MeshInfo->MeshData[0].Vertices[i].Pos.z = ModelData->Vertices[i].Vertices.z;

			MeshInfo->MeshData[0].Vertices[i].Normal.x = ModelData->Vertices[i].Normals.x;
			MeshInfo->MeshData[0].Vertices[i].Normal.y = ModelData->Vertices[i].Normals.y;
			MeshInfo->MeshData[0].Vertices[i].Normal.z = ModelData->Vertices[i].Normals.z;

			MeshInfo->MeshData[0].Vertices[i].TexCoord.x = ModelData->Vertices[i].TexCoords.x;
			MeshInfo->MeshData[0].Vertices[i].TexCoord.y = ModelData->Vertices[i].TexCoords.y;
		}

		MeshInfo->MeshData[0].VertexBuffer = OpenVkCreateVertexBuffer(MeshInfo->MeshData[0].VertexCount * sizeof(SceneVertex), MeshInfo->MeshData[0].Vertices);
		if (MeshInfo->MeshData[0].VertexBuffer == OPENVK_ERROR)
			return false;


		MeshInfo->MeshData[0].Color = Vec4f(1.0);
		MeshInfo->MeshData[0].Metallic = 0.0;
		MeshInfo->MeshData[0].Roughness = 1.0;
		MeshInfo->MeshData[0].Occlusion = 1.0;

		MeshInfo->MeshData[0].AlbedoIndex = 0;
		MeshInfo->MeshData[0].NormalIndex = 0;
		MeshInfo->MeshData[0].MetallicIndex = 0;
		MeshInfo->MeshData[0].RoughnessIndex = 0;
		MeshInfo->MeshData[0].OcclusionIndex = 0;

		MeshInfo->MeshData[0].IndexCount = 0;
		MeshInfo->MeshData[0].Indices = NULL;
		MeshInfo->MeshData[0].IndexBuffer = 0;

		MeshInfo->MeshCount = 1;
	}

	return true;
}
*/

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

		SceneMesh->VertexCount = WaveMesh->VertexCount;
		SceneMesh->IndexCount = WaveMesh->IndexCount;

		SceneMesh->Vertices = (SceneVertex*)malloc(WaveMesh->VertexCount * sizeof(SceneVertex));
		SceneMesh->Indices = (uint32_t*)malloc(WaveMesh->IndexCount * sizeof(uint32_t));

		SceneMesh->Color = Vec4f(1.0);
		SceneMesh->Metallic = 0.0;
		SceneMesh->Roughness = 1.0;
		SceneMesh->Occlusion = 1.0;
		SceneMesh->AlbedoIndex = 0;
		SceneMesh->NormalIndex = 0;
		SceneMesh->MetallicIndex = 0;
		SceneMesh->RoughnessIndex = 0;
		SceneMesh->OcclusionIndex = 0;

		if (strcmp(ModelData->Materials[i].DiffuseTexture, "NoTexture") != 0) 
			SceneMesh->AlbedoIndex = AddTexture(ModelData->Materials[i].DiffuseTexture, false);
		if (strcmp(ModelData->Materials[i].NormalTexture, "NoTexture") != 0) 
			SceneMesh->NormalIndex = AddTexture(ModelData->Materials[i].NormalTexture, false);
		else if (strcmp(ModelData->Materials[i].BumpTexture, "NoTexture") != 0) 
			SceneMesh->NormalIndex = AddTexture(ModelData->Materials[i].BumpTexture, false);
		if (strcmp(ModelData->Materials[i].SpecularTexture, "NoTexture") != 0) 
			SceneMesh->RoughnessIndex = AddTexture(ModelData->Materials[i].SpecularTexture, false);

		SceneMesh->Color.x = ModelData->Materials[i].DiffuseColor.x;
		SceneMesh->Color.y = ModelData->Materials[i].DiffuseColor.y;
		SceneMesh->Color.z = ModelData->Materials[i].DiffuseColor.z;
		SceneMesh->Color.w = ModelData->Materials[i].Dissolve;

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

bool LoadModel(const char* Path, const aiScene* Scene, SceneMesh* MeshInfo)
{
	MeshInfo->Freeable = true;
	strcpy(MeshInfo->Path, Path);
	MeshInfo->MeshCount = (Scene->mNumMaterials == 0 ? 1 : Scene->mNumMaterials);
	MeshInfo->MeshData = (SceneMeshData*)malloc(MeshInfo->MeshCount * sizeof(SceneMeshData));

	printf("Num Mats: %d/%d\n", Scene->mNumMaterials, Scene->mNumMeshes);
//	exit(2134);

	
	for (uint32_t i = 0; i < MeshInfo->MeshCount; i++)
	{
		MeshInfo->MeshData[i].Color = Vec4f(1.0);
		MeshInfo->MeshData[i].Metallic = 0.0;
		MeshInfo->MeshData[i].Roughness = 1.0;
		MeshInfo->MeshData[i].Occlusion = 1.0;

		MeshInfo->MeshData[i].AlbedoIndex = 0;
		MeshInfo->MeshData[i].NormalIndex = 0;
		MeshInfo->MeshData[i].MetallicIndex = 0;
		MeshInfo->MeshData[i].RoughnessIndex = 0;
		MeshInfo->MeshData[i].OcclusionIndex = 0;

		if (Scene->mNumMaterials != 0)
		{
			aiString Path;
			aiMaterial* Material = Scene->mMaterials[i];
			if (Material->GetTexture(aiTextureType_DIFFUSE, 0, &Path, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS)
				MeshInfo->MeshData[i].AlbedoIndex = AddTexture((char*)Path.C_Str(), false);
			if (Material->GetTexture(aiTextureType_NORMAL_CAMERA, 0, &Path, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS)
				MeshInfo->MeshData[i].NormalIndex = AddTexture((char*)Path.C_Str(), false);
			if (Material->GetTexture(aiTextureType_METALNESS, 0, &Path, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS)
				MeshInfo->MeshData[i].MetallicIndex = AddTexture((char*)Path.C_Str(), false);
			if (Material->GetTexture(aiTextureType_DIFFUSE_ROUGHNESS, 0, &Path, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS)
				MeshInfo->MeshData[i].RoughnessIndex = AddTexture((char*)Path.C_Str(), false);
			if (Material->GetTexture(aiTextureType_AMBIENT_OCCLUSION, 0, &Path, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS)
				MeshInfo->MeshData[i].OcclusionIndex = AddTexture((char*)Path.C_Str(), false);
		}		
		
		MeshInfo->MeshData[i].IndexCount = 0;
		MeshInfo->MeshData[i].VertexCount = 0;
	}

//	uint32_t IndexCount = 0;
//	uint32_t VertexCount = 0;
	SceneVertex* Vertex;

	for (uint32_t i = 0; i < Scene->mNumMeshes; i++)
	{
		aiMesh* Mesh = Scene->mMeshes[i];

		MeshInfo->MeshData[Mesh->mMaterialIndex].VertexCount += Mesh->mNumVertices;
		MeshInfo->MeshData[Mesh->mMaterialIndex].IndexCount += Mesh->mNumFaces * Mesh->mFaces[0].mNumIndices;
	}

	for (uint32_t i = 0; i < MeshInfo->MeshCount; i++)
	{
		MeshInfo->MeshData[i].Vertices = (SceneVertex*)malloc(MeshInfo->MeshData[i].VertexCount * sizeof(SceneVertex));
		MeshInfo->MeshData[i].Indices = (uint32_t*)malloc(MeshInfo->MeshData[i].IndexCount * sizeof(uint32_t));

		MeshInfo->MeshData[i].VertexCount = 0;
		MeshInfo->MeshData[i].IndexCount = 0;
	}

	for (uint32_t i = 0; i < Scene->mNumMeshes; i++)
	{
		aiMesh* Mesh = Scene->mMeshes[i];
		
		printf("Mat: %d\n", Mesh->mMaterialIndex);

		for (uint32_t j = 0; j < Mesh->mNumVertices; j++)
		{
			Vertex = &MeshInfo->MeshData[Mesh->mMaterialIndex].Vertices[MeshInfo->MeshData[Mesh->mMaterialIndex].VertexCount];
			
			Vertex->Pos.x = Mesh->mVertices[j].x;
			Vertex->Pos.y = Mesh->mVertices[j].y;
			Vertex->Pos.z = Mesh->mVertices[j].z;

			if (Mesh->mTextureCoords[0])
			{
				Vertex->TexCoord.x = Mesh->mTextureCoords[0][j].x;
				Vertex->TexCoord.y = Mesh->mTextureCoords[0][j].y;
			}
			else
			{
				Vertex->TexCoord.x = 0.0;
				Vertex->TexCoord.y = 0.0;
			}

			Vertex->Normal.x = Mesh->mNormals[j].x;
			Vertex->Normal.y = Mesh->mNormals[j].y;
			Vertex->Normal.z = Mesh->mNormals[j].z;

			MeshInfo->MeshData[Mesh->mMaterialIndex].VertexCount++;
		}

		for (uint32_t j = 0; j < Mesh->mNumFaces; j++)
		{
			aiFace Face = Mesh->mFaces[j];

			//Error handling
		//	if (Face.mNumIndices != 3)
		//	{
		//		for (uint32_t k = 0; k < MeshInfo->MeshCount; k++)
		//		{
		//			free(MeshInfo->MeshData[k].Vertices);
		//			free(MeshInfo->MeshData[k].Indices);
		//			free(MeshInfo->MeshData);
		//			return false;
		//		}
		//	}

			for (uint32_t k = 0; k < Face.mNumIndices; k++)
			{
				MeshInfo->MeshData[Mesh->mMaterialIndex].Indices[MeshInfo->MeshData[Mesh->mMaterialIndex].IndexCount] = Face.mIndices[k];
				MeshInfo->MeshData[Mesh->mMaterialIndex].IndexCount++;
			}
		}
	}

	for (uint32_t i = 0; i < MeshInfo->MeshCount; i++)
	{
		printf("%d VertexCount: %d\n", i, MeshInfo->MeshData[i].VertexCount);
		printf("%d IndexCount: %d\n", i, MeshInfo->MeshData[i].IndexCount);

		MeshInfo->MeshData[i].VertexBuffer = OpenVkCreateVertexBuffer(MeshInfo->MeshData[i].VertexCount * sizeof(SceneVertex), MeshInfo->MeshData[i].Vertices);
		if (MeshInfo->MeshData[i].VertexBuffer == OPENVK_ERROR)
		{
			for (uint32_t j = 0; j < i; j++)
			{
				OpenVkDestroyBuffer(MeshInfo->MeshData[j].VertexBuffer);
				OpenVkDestroyBuffer(MeshInfo->MeshData[j].IndexBuffer);
			}
			free(MeshInfo->MeshData[i].Vertices);
			free(MeshInfo->MeshData[i].Indices);
			free(MeshInfo->MeshData);
			return false;
		}

		MeshInfo->MeshData[i].IndexBuffer = OpenVkCreateIndexBuffer(MeshInfo->MeshData[i].IndexCount * sizeof(uint32_t), MeshInfo->MeshData[i].Indices);
		if (MeshInfo->MeshData[i].IndexBuffer == OPENVK_ERROR)
		{
			for (uint32_t j = 0; j < i; j++)
			{
				OpenVkDestroyBuffer(MeshInfo->MeshData[j].VertexBuffer);
				OpenVkDestroyBuffer(MeshInfo->MeshData[j].IndexBuffer);
			}
			OpenVkDestroyBuffer(MeshInfo->MeshData[i].VertexBuffer);

			free(MeshInfo->MeshData[i].Vertices);
			free(MeshInfo->MeshData[i].Indices);
			free(MeshInfo->MeshData);
			return false;
		}
	}

	return true;
}

void LoadModel(uint32_t Settings)
{
	char Path[MAX_CHAR_PATH_LENGTH];
	char CombinedPath[MAX_CHAR_PATH_LENGTH];

	uint16_t Offset;	
	if (WaveOpenFileDialog(Path, true, &Offset, "All Models(obj, gltf, stl)\0*.obj;*.gltf;*.stl\0"))
	{
		printf("Opened\n");

		uint32_t ModelCount = 0;

		char* PTR = Path + Offset;
		size_t Length = (strlen(PTR) + 1);

		char* CountPTR = PTR;
		while (*(CountPTR += (strlen(CountPTR) + 1)))
			ModelCount++;

		while (*PTR)
		{
			if (ModelCount == 0)
				memcpy(CombinedPath, Path, MAX_CHAR_PATH_LENGTH);
			else
				sprintf(CombinedPath, "%s\\%s", Path, PTR);
			/*
			{
				SceneMesh MeshInfo;

				uint32_t LastTexture = SelectedTexture;

				Assimp::Importer Importer;

				const aiScene* Scene = Importer.ReadFile(CombinedPath, aiProcess_JoinIdenticalVertices |
					aiProcess_ValidateDataStructure |
					aiProcess_ImproveCacheLocality |
					aiProcess_RemoveRedundantMaterials |
					aiProcess_FindInvalidData |
					aiProcess_GenUVCoords |
					aiProcess_TransformUVCoords |
					aiProcess_LimitBoneWeights |
					aiProcess_OptimizeMeshes |
					aiProcess_ForceGenNormals |
					//		aiProcess_GenNormals |
					aiProcess_SplitLargeMeshes |
					aiProcess_Triangulate |
					aiProcess_RemoveComponent |
					aiProcess_SortByPType);

				printf("Loaded Model Successfully\n");
				if (!Scene)
					printf("Failed Loading Model: %s!\n", CombinedPath);
				else
				{
					if (LoadModel(CombinedPath, Scene, &MeshInfo))
						AddMesh(GetFileNameFromPath(CombinedPath), &MeshInfo);
					else
						printf("Failed to create vertex/index buffer from model\n");
				}

				Importer.FreeScene();

				SelectedTexture = LastTexture;
			}
			*/

			{
				SceneMesh MeshInfo;

				uint32_t LastTexture = SelectedTexture;


			//	if (Settings == 0)
			//		Settings = WAVE_LOAD_MATERIAL | WAVE_GEN_NORMALS | WAVE_FLIP_UVS | WAVE_GEN_UVS | WAVE_GEN_INDICES | WAVE_PRINT_DEBUG_INOFS;
			//	else 
					Settings |= WAVE_LOAD_MATERIAL | WAVE_GEN_NORMALS | WAVE_FLIP_UVS | WAVE_GEN_UVS | WAVE_GEN_INDICES | WAVE_REMOVE_REDUNDANT_MATERIALS | WAVE_PRINT_DEBUG_INOFS;
				printf("Started loading model\n");
				WaveModelData Model = WaveLoadModel(CombinedPath, Settings);

				if (LoadModelWave(CombinedPath, &Model, &MeshInfo))
					AddMesh(GetFileNameFromPath(CombinedPath), &MeshInfo);

				WaveFreeModel(&Model);

				SelectedTexture = LastTexture;
			}

			PTR += (strlen(PTR) + 1);
		}
	}
	WaveResetToLastPath();
	/*
	if (WaveOpenFileDialog(Path, false, NULL, "All Models\0*.obj;*.dae;*.fbx;*.gltf;*.glb;*.usd;*.stl\0"))
	{
		SceneMesh MeshInfo;

		uint32_t LastTexture = SelectedTexture;

		Assimp::Importer Importer;

		const aiScene* Scene = Importer.ReadFile(Path, aiProcess_JoinIdenticalVertices |
			aiProcess_ValidateDataStructure |
			aiProcess_ImproveCacheLocality |
			aiProcess_RemoveRedundantMaterials |
			aiProcess_FindInvalidData |
			aiProcess_GenUVCoords |
			aiProcess_TransformUVCoords |
			aiProcess_LimitBoneWeights |
			aiProcess_OptimizeMeshes |
			aiProcess_ForceGenNormals |
			//		aiProcess_GenNormals |
			aiProcess_GenSmoothNormals |
			aiProcess_SplitLargeMeshes |
			aiProcess_Triangulate |
			aiProcess_RemoveComponent |
			aiProcess_SortByPType);

		printf("Loaded Model Successfully\n");
		if (!Scene)
			printf("Failed Loading Model: %s!\n", Path);
		else
		{
			if (LoadModel(Path, Scene, &MeshInfo))
				AddMesh(GetFileNameFromPath(Path), &MeshInfo);
			else
				printf("Failed to create vertex/index buffer from model\n");
		}

		Importer.FreeScene();

		SelectedTexture = LastTexture;



	//	WaveFreeModel(&ModelData);
		WaveResetToLastPath();
	}
	*/
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
				if (ImGui::ImageButton(&GetDescriptorSet(Image->TextureDescriptorSet)[0], ImVec2(ThumbnailSize, ThumbnailSize)))
				{
					ImGui::SetWindowFocus("Texture Inspector");
					SelectedTexture = i;
				}
				ImGui::Text(Image->Name);
				ImGui::NextColumn();
			}
		}

		for (uint32_t i = 1; i < SceneMaterials.Size; i++)
		{
			SceneMaterial* Material = (SceneMaterial*)CMA_GetAt(&SceneMaterials, i);
			if (Material != NULL)
			{
				if (ImGuiIconButton("A", Material->Name, true, ImVec2(ThumbnailSize, ThumbnailSize)))
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
				if (ImGuiIconButton("B", Mesh->Name, true, ImVec2(ThumbnailSize, ThumbnailSize)))
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
				if (ImGuiIconButton("z", Animation->Name, true, ImVec2(ThumbnailSize, ThumbnailSize)))
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
			if (ImGuiIconButton("J", SceneScripts[i].Name, true, ImVec2(ThumbnailSize, ThumbnailSize)))
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

					MeshInfo.MeshData[0].Color = Vec4f(1.0);
					MeshInfo.MeshData[0].Metallic = 0.0;
					MeshInfo.MeshData[0].Roughness = 1.0;
					MeshInfo.MeshData[0].Occlusion = 1.0;

					MeshInfo.MeshData[0].AlbedoIndex = 0;
					MeshInfo.MeshData[0].NormalIndex = 0;
					MeshInfo.MeshData[0].MetallicIndex = 0;
					MeshInfo.MeshData[0].RoughnessIndex = 0;

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

					MeshInfo.MeshData[0].Color = Vec4f(1.0);
					MeshInfo.MeshData[0].Metallic = 0.0;
					MeshInfo.MeshData[0].Roughness = 1.0;
					MeshInfo.MeshData[0].Occlusion = 1.0;

					MeshInfo.MeshData[0].AlbedoIndex = 0;
					MeshInfo.MeshData[0].NormalIndex = 0;
					MeshInfo.MeshData[0].MetallicIndex = 0;
					MeshInfo.MeshData[0].RoughnessIndex = 0;

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

					MeshInfo.MeshData[0].Color = Vec4f(1.0);
					MeshInfo.MeshData[0].Metallic = 0.0;
					MeshInfo.MeshData[0].Roughness = 1.0;
					MeshInfo.MeshData[0].Occlusion = 1.0;

					MeshInfo.MeshData[0].AlbedoIndex = 0;
					MeshInfo.MeshData[0].NormalIndex = 0;
					MeshInfo.MeshData[0].MetallicIndex = 0;
					MeshInfo.MeshData[0].RoughnessIndex = 0;

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

					MeshInfo.MeshData[0].Color = Vec4f(1.0);
					MeshInfo.MeshData[0].Metallic = 0.0;
					MeshInfo.MeshData[0].Roughness = 1.0;
					MeshInfo.MeshData[0].Occlusion = 1.0;

					MeshInfo.MeshData[0].AlbedoIndex = 0;
					MeshInfo.MeshData[0].NormalIndex = 0;
					MeshInfo.MeshData[0].MetallicIndex = 0;
					MeshInfo.MeshData[0].RoughnessIndex = 0;

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
					if (ImGui::MenuItem("Default")) LoadModel(0);
					if (ImGui::MenuItem("Gen Smooth Normals")) LoadModel(WAVE_GEN_SMOOTH_NORMALS);
					if (ImGui::MenuItem("Gen Flat Normals")) LoadModel(WAVE_FORCE_GEN_NORMALS);
					ImGui::EndMenu();
				}

			//	if (ImGui::Selectable("Load Model"))
			//	{
			//		
			//
			//		
			//	}

				if (ImGui::Selectable("Load Texture"))
				{
					char Path[MAX_CHAR_PATH_LENGTH];
					char CombinedPath[MAX_CHAR_PATH_LENGTH];

					uint16_t Offset;
					if (WaveOpenFileDialog(Path, true, &Offset, "All Images(png, jpg, tga, jpeg, hdr, psd, bmp)\0*.png;*.jpg;*.tga;*.jpeg;*.hdr;*.psd;*.bmp\0"))
					{
						uint32_t TextureCount = 0;

						char* PTR = Path + Offset;
						size_t Length = (strlen(PTR) + 1);

						char* CountPTR = PTR;
						while (*(CountPTR += (strlen(CountPTR) + 1)))
							TextureCount++;

						while (*PTR)
						{
							if (TextureCount == 0)
								memcpy(CombinedPath, Path, MAX_CHAR_PATH_LENGTH);								
							else
								sprintf(CombinedPath, "%s\\%s", Path, PTR);

							AddTexture(CombinedPath, true);
							PTR += (strlen(PTR) + 1);
						}							
					}
						
					WaveResetToLastPath();
				}

				if (ImGui::Selectable("Load Animation"))
				{
					char Path[MAX_CHAR_PATH_LENGTH];
					char CombinedPath[MAX_CHAR_PATH_LENGTH];

					uint16_t Offset;
					if (WaveOpenFileDialog(Path, true, &Offset, "Quake Models(md2, bin)\0*.md2;*.bin\0"))
					{
						printf("Opened\n");

						uint32_t ModelCount = 0;

						char* PTR = Path + Offset;
						size_t Length = (strlen(PTR) + 1);

						char* CountPTR = PTR;
						while (*(CountPTR += (strlen(CountPTR) + 1)))
							ModelCount++;

						while (*PTR)
						{
							if (ModelCount == 0)
								memcpy(CombinedPath, Path, MAX_CHAR_PATH_LENGTH);
							else
								sprintf(CombinedPath, "%s\\%s", Path, PTR);

							AddAnimation(CombinedPath, 1024, 1024);
							PTR += (strlen(PTR) + 1);
						}
					}
				}

			}
			ImGui::EndPopup();
		}
		//	ImGui::EndChild();
	}
	ImGui::End();
}