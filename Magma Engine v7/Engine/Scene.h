uint32_t SceneCreateDefaultTexture(SceneTextureImage* Image)
{
	OpenVkDeviceWaitIdle();
	Image->Format = OPENVK_FORMAT_RGBA;
	Image->Width = 2;
	Image->Height = 2;

	unsigned char* Pixels = (unsigned char*)malloc(16);
	if (!Pixels)
	{
		printf("Error, your system doesn't have enough ram for 16 bytes for default texture, buy more than 1 mb of ram\n");
		exit(0);
	}

	memset(Pixels, 255, 16);

	OpenVkTextureCreateInfo TextureCreateInfo;
	TextureCreateInfo.Pixels = &Pixels;
	TextureCreateInfo.Width = Image->Width;
	TextureCreateInfo.Height = Image->Height;
	TextureCreateInfo.Format = Image->Format;
	TextureCreateInfo.MipLevels = 0;
	TextureCreateInfo.GenerateMipmaps = OpenVkFalse;
	TextureCreateInfo.UseCustomMipmaps = OpenVkFalse;
	Image->TextureImage = OpenVkCreateTexture(&TextureCreateInfo);
	free(Pixels);
	Image->MipLevels = TextureCreateInfo.MipLevels;

	Image->TextureSampler = OpenVkCreateImageSampler(OPENVK_FILTER_NEAREST, OPENVK_ADDRESS_MODE_REPEAT);

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

	return OpenVkCreateDescriptorSet(&DescriptorSetCreateInfo);
}

void SceneInit()
{
	ResetSceneSettings();
	/*
	AddTexture((char*)"Data/Textures/Default.png", false);
	AddMaterial("None");

	SceneMeshes = CMA_Create(sizeof(SceneMesh));
	SceneMesh Mesh;
	Mesh.MeshData = (SceneMeshData*)malloc(1 * sizeof(SceneMeshData));
	Mesh.Freeable = false;
	strcpy(Mesh.Name, "None");
	strcpy(Mesh.Path, "None");
	Mesh.MeshCount = 0;

	SetDefaultMaterial(&Mesh.MeshData[0].Material, "MESH");
	Mesh.MeshData[0].VertexCount = 0;
	Mesh.MeshData[0].Vertices = NULL;
	Mesh.MeshData[0].VertexBuffer = 0;
	Mesh.MeshData[0].IndexCount = 0;
	Mesh.MeshData[0].Indices = NULL;
	Mesh.MeshData[0].IndexBuffer = 0;
	CMA_Push(&SceneMeshes, &Mesh);

	SceneAnimations = CMA_Create(sizeof(SceneAnimation));
	SceneAnimation Animation;
	Animation.Start = 0;
	Animation.End = 1;
	Animation.Speed = 25.0;
	Animation.MeshData.NumFrames = 1;
	Animation.Vertices = NULL;
	Animation.VertexBuffer = 0;
	CMA_Push(&SceneAnimations, &Animation);
	*/

	//Texture
	SceneTextures = CMA_Create(sizeof(SceneTextureImage), "Magma Engine, Scene Textures");
	SceneTextureImage Image;
	Image.ShowInAssetBrowser = false;
	Image.TextureDescriptorSet = SceneCreateDefaultTexture(&Image);
	strcpy(Image.Name, "None");
	strcpy(Image.Path, "None");
	size_t DefaultTextureIndex = CMA_Push(&SceneTextures, &Image);
	
	//FIX - add a check if raytracing is even compatibale with hardware or enabled
	RaytracingAddTexture(DefaultTextureIndex);

	//	LoadTextureCompressed = true;
	//	GenerateMipMaps = true;
	//	Image.ShowInAssetBrowser = true;
	//	Image.TextureDescriptorSet = LoadTexture((char*)"Data/Textures/Default2.png", &Image);
	//	strcpy(Image.Name, "UvGrid");
	//	strcpy(Image.Path, "None");
	//	CMA_Push(&SceneTextures, &Image);
	//
	//	LoadTextureCompressed = true;
	//	GenerateMipMaps = true;
	//	Image.ShowInAssetBrowser = true;
	//	Image.TextureDescriptorSet = LoadTexture((char*)"Data/Textures/Default2.png", &Image);
	//	strcpy(Image.Name, "UvGrid");
	//	strcpy(Image.Path, "None");
	//	CMA_Push(&SceneTextures, &Image);
	//
	//	LoadTextureCompressed = true;
	//	GenerateMipMaps = true;
	//	Image.ShowInAssetBrowser = true;
	//	Image.TextureDescriptorSet = LoadTexture((char*)"Data/Textures/WoodFloor.jpg", &Image);
	//	strcpy(Image.Name, "UvGrid");
	//	strcpy(Image.Path, "None");
	//	CMA_Push(&SceneTextures, &Image);
	//
	//	LoadTextureCompressed = false;
	//	GenerateMipMaps = true;
	//	Image.ShowInAssetBrowser = true;
	//	Image.TextureDescriptorSet = LoadTexture((char*)"Data/Textures/WoodFloor.jpg", &Image);
	//	strcpy(Image.Name, "UvGrid");
	//	strcpy(Image.Path, "None");
	//	CMA_Push(&SceneTextures, &Image);

		//Material
	SceneMaterials = CMA_Create(sizeof(SceneMaterial), "Magma Engine, Scene Materials");
	/*
	SceneMaterial Material;
	strcpy(Material.Name, "None");
	Material.Color = Vec4f(1.0);
	Material.Metallic = 0.0;
	Material.Roughness = 1.0;
	Material.Occlusion = 1.0;

	Material.AlbedoIndex = 0;
	Material.NormalIndex = 0;
	Material.MetallicIndex = 0;
	Material.RoughnessIndex = 0;
	Material.OcclusionIndex = 0;
	CMA_Push(&SceneMaterials, &Material);
	*/
	SceneMaterial Material;
	SetDefaultMaterial(&Material, "None");
	AddMaterial(&Material);

	//Mesh
	SceneMeshes = CMA_Create(sizeof(SceneMesh), "Magma Engine, Scene Meshes");
	SceneMesh Mesh;
	Mesh.MeshData = (SceneMeshData*)malloc(1 * sizeof(SceneMeshData));
	strcpy(Mesh.Name, "None");
	strcpy(Mesh.Path, "None");
	Mesh.MeshCount = 0;
	Mesh.Destroyable = false;

//	SetDefaultMaterial(&Mesh.MeshData[0].Material, "MESH");
	strcpy(Mesh.MeshData[0].Name, "None");
	Mesh.MeshData[0].VertexOffset = 0;
	Mesh.MeshData[0].IndexOffset = 0;
	Mesh.MeshData[0].VertexCount = 0;
	Mesh.MeshData[0].IndexCount = 0;
	Mesh.MeshData[0].MaterialIndex = 0;
	Mesh.VertexBuffer = OPENVK_ERROR;
	Mesh.IndexBuffer = OPENVK_ERROR;
	CMA_Push(&SceneMeshes, &Mesh);

	SceneAnimations = CMA_Create(sizeof(SceneAnimation), "Magma Engine, Scene Animations");
	SceneAnimation Animation;
	Animation.Start = 0;
	Animation.End = 1;
	Animation.Speed = 25.0;
	Animation.MeshData.NumFrames = 1;
	Animation.Vertices = NULL;
	Animation.VertexBuffer = 0;
	CMA_Push(&SceneAnimations, &Animation);
}

void SceneDestroy()
{
	for (size_t i = 0; i < SceneMeshes.Size; i++)
	{
		SceneMesh* Mesh = (SceneMesh*)CMA_GetAt(&SceneMeshes, i);
		if (Mesh)
		{
			if (Mesh->Destroyable)
			{
				if (Mesh->Vertices) free(Mesh->Vertices);
				if (Mesh->Indices) free(Mesh->Indices);
			}			

			free(Mesh->MeshData);			
		}
	}

	CMA_Destroy(&SceneTextures);
	CMA_Destroy(&SceneMaterials);
	CMA_Destroy(&SceneMeshes);
	CMA_Destroy(&SceneAnimations);
	free(SceneScripts);
}

void SceneSave(const char* FileName)
{
	

	FILE* File = fopen(FileName, "wb");

	fprintf(File, "{\n");

	fprintf(File, "\t\"Textures\": [\n");
	for (size_t i = 1; i < SceneTextures.Size; i++)
	{
		SceneTextureImage* Texture = (SceneTextureImage*)CMA_GetAt(&SceneTextures, i);
		if (Texture)
		{
			fprintf(File, "\t\t{\n");
			{
				fprintf(File, "\t\t\t\"Name\": \"%s\",\n", Texture->Name);
				fprintf(File, "\t\t\t\"Path\": \"%s\",\n", Texture->Path);
				fprintf(File, "\t\t\t\"ShowInAssetBrowser\": %s,\n", Texture->ShowInAssetBrowser == true ? "true" : "false");
				fprintf(File, "\t\t\t\"Width\": %d,\n", Texture->Width);
				fprintf(File, "\t\t\t\"Height\": %d,\n", Texture->Height);
				fprintf(File, "\t\t\t\"Format\": %d,\n", Texture->Format);
				fprintf(File, "\t\t\t\"MipLevels\": %d,\n", Texture->MipLevels);
				fprintf(File, "\t\t\t\"Data\": null%s\n", i == (SceneTextures.Size - 1) ? "" : ",");
			}			
			fprintf(File, "\t\t}%s\n", i == (SceneTextures.Size - 1) ? "" : ",");
		}		
		
	}
	fprintf(File, "\t],\n\n");

	fprintf(File, "\t\"Materials\": [\n");
	for (size_t i = 1; i < SceneMaterials.Size; i++)
	{
		SceneMaterial* Material = (SceneMaterial*)CMA_GetAt(&SceneMaterials, i);
		if (Material)
		{
			fprintf(File, "\t\t{\n");
			{
				fprintf(File, "\t\t\t\"Name\": \"%s\",\n", Material->Name);
				fprintf(File, "\t\t\t\"AlbedoIndex\": %d,\n", Material->AlbedoIndex);
				fprintf(File, "\t\t\t\"NormalIndex\": %d,\n", Material->NormalIndex);
				fprintf(File, "\t\t\t\"MetallicIndex\": %d,\n", Material->MetallicIndex);
				fprintf(File, "\t\t\t\"RoughnessIndex\": %d,\n", Material->RoughnessIndex);
				fprintf(File, "\t\t\t\"OcclusionIndex\": %d,\n", Material->OcclusionIndex);

				fprintf(File, "\t\t\t\"Color\": [%f, %f, %f, %f],\n", Material->Color.r, Material->Color.g, Material->Color.b, Material->Color.a);
				fprintf(File, "\t\t\t\"Metallic\": %f,\n", Material->Metallic);
				fprintf(File, "\t\t\t\"Roughness\": %f,\n", Material->Roughness);
				fprintf(File, "\t\t\t\"Occlusion\": %f%s\n", Material->Occlusion, i == (SceneTextures.Size - 1) ? "" : ",");

			}			
			fprintf(File, "\t\t}%s\n", i == (SceneTextures.Size - 1) ? "" : ",");
		}

	}
	fprintf(File, "\t],\n\n");

	size_t Base64EncodeBufferSize = Bas64CalcRequiredSize(512 * sizeof(SceneVertex));
	BYTE* Base64EncodeBuffer = (BYTE*)malloc(Base64EncodeBufferSize);
	if (!Base64EncodeBuffer)
	{
		printf("Failed to fully save scene, allocation of base64 encode buffer failed!\n");
		fclose(File);

		return;
	}

	fprintf(File, "\t\"Meshes\": [\n");
	for (size_t i = 1; i < SceneMeshes.Size; i++)
	{
		

		SceneMesh* Mesh = (SceneMesh*)CMA_GetAt(&SceneMeshes, i);
		if (Mesh)
		{
			fprintf(File, "\t\t{\n");
			{
				fprintf(File, "\t\t\t\"Name\": \"%s\",\n", Mesh->Name);
				fprintf(File, "\t\t\t\"Path\": \"%s\",\n", Mesh->Path);
				fprintf(File, "\t\t\t\"MeshCount\": %d,\n", Mesh->MeshCount);
				fprintf(File, "\t\t\t\"Destroyable\": %s,\n", Mesh->Destroyable == true ? "true" : "false");
				fprintf(File, "\t\t\t\"TotalVertexCount\": %d,\n", Mesh->TotalVertexCount);
				fprintf(File, "\t\t\t\"TotalIndexCount\": %d,\n", Mesh->TotalIndexCount);

				bool ResizeBas64 = false;
				if (Bas64CalcRequiredSize((size_t)Mesh->TotalVertexCount * sizeof(SceneVertex)) > Base64EncodeBufferSize)
				{
					Base64EncodeBufferSize = Bas64CalcRequiredSize((size_t)Mesh->TotalVertexCount * sizeof(SceneVertex));
					ResizeBas64 = true;				
				}
				else if (Bas64CalcRequiredSize((size_t)Mesh->TotalIndexCount * sizeof(uint32_t)) > Base64EncodeBufferSize)
				{
					Base64EncodeBufferSize = Bas64CalcRequiredSize((size_t)Mesh->TotalIndexCount * sizeof(uint32_t));
					ResizeBas64 = true;
				}

				if (ResizeBas64)
				{
					Base64EncodeBuffer = (BYTE*)realloc(Base64EncodeBuffer, Base64EncodeBufferSize);
					if (!Base64EncodeBuffer)
					{
						printf("Failed to fully save scene, reallocation of base64 encode buffer failed!\n");
						fclose(File);
						return;
					}
				}

				if (Mesh->Vertices)
				{
					DWORD Length = 0;
					Base64Encode((BYTE*)Mesh->Vertices, (size_t)Mesh->TotalVertexCount * sizeof(SceneVertex), Base64EncodeBuffer, &Length);
					fprintf(File, "\t\t\t\"Vertices\": %s,\n", Base64EncodeBuffer);
				}
				if (Mesh->Indices)
				{
					DWORD Length = 0;
					Base64Encode((BYTE*)Mesh->Indices, (size_t)Mesh->TotalIndexCount * sizeof(uint32_t), Base64EncodeBuffer, &Length);
					fprintf(File, "\t\t\t\"Indices\": %s,\n", Base64EncodeBuffer);
				}
					

				fprintf(File, "\t\t\t\"MeshData\": [\n");
				for (uint32_t j = 0; j < Mesh->MeshCount; j++)
				{
					fprintf(File, "\t\t\t\t{\n");
					{
						fprintf(File, "\t\t\t\t\t\"Name\": \"%s\",\n", Mesh->MeshData[j].Name);
						fprintf(File, "\t\t\t\t\t\"MaterialIndex\": %d,\n", Mesh->MeshData[j].MaterialIndex);
						fprintf(File, "\t\t\t\t\t\"VertexOffset\": %d,\n", Mesh->MeshData[j].VertexOffset);
						fprintf(File, "\t\t\t\t\t\"VertexCount\": %d,\n", Mesh->MeshData[j].VertexCount);
						fprintf(File, "\t\t\t\t\t\"IndexOffset\": %d,\n", Mesh->MeshData[j].IndexOffset);
						fprintf(File, "\t\t\t\t\t\"IndexCount\": %d,\n", Mesh->MeshData[j].IndexCount);

						fprintf(File, "\t\t\t\t\t\"Min\": [%f, %f, %f],\n", Mesh->MeshData[j].AABB.Min.x, Mesh->MeshData[j].AABB.Min.y, Mesh->MeshData[j].AABB.Min.z);
						fprintf(File, "\t\t\t\t\t\"Max\": [%f, %f, %f],\n", Mesh->MeshData[j].AABB.Max.x, Mesh->MeshData[j].AABB.Max.y, Mesh->MeshData[j].AABB.Max.z);
						fprintf(File, "\t\t\t\t\t\"Render\": [%s, %s, %s, %s, %s],\n", 
							Mesh->MeshData[j].Render[0] ? "true" : "false",
							Mesh->MeshData[j].Render[1] ? "true" : "false",
							Mesh->MeshData[j].Render[2] ? "true" : "false",
							Mesh->MeshData[j].Render[3] ? "true" : "false",
							Mesh->MeshData[j].Render[4] ? "true" : "false");
					}
					fprintf(File, "\t\t\t\t}%s\n", j == (Mesh->MeshCount - 1) ? "" : ",");
				}
				fprintf(File, "\t\t\t]\n");
				//	fprintf(File, "\t\t\t\"OcclusionTexture\": \"%s\",\n", Mesh->OcclusionIndex);
				//
				//	fprintf(File, "\t\t\t\"Color\": [%f, %f, %f, %f],\n", Material->Color.r, Material->Color.g, Material->Color.b, Material->Color.a);
				//	fprintf(File, "\t\t\t\"MetallicTexture\": %f,\n", Material->Metallic);
				//	fprintf(File, "\t\t\t\"RoughnessTexture\": %f,\n", Material->Roughness);
				//	fprintf(File, "\t\t\t\"OcclusionTexture\": %f%s\n", Material->Occlusion, i == (SceneTextures.Size - 1) ? "" : ",");
			}			
			fprintf(File, "\t\t}%s\n", i == (SceneTextures.Size - 1) ? "" : ",");
		}

	}
	fprintf(File, "\t],\n\n");

	fprintf(File, "}\n");

	fclose(File);

	free(Base64EncodeBuffer);

	printf("Saved Scene: %s\n", FileName);
}

void SceneLoadTexture(JsonObject* Object)
{
	char* Path = NULL;
	bool ShowInAssetBrowser = false;

	for (size_t i = 0; i < Object->Refrences.Size; i++)
	{
		JsonVariables* Variable = (JsonVariables*)DynamicArrayGetAt(&Object->Refrences, i);
		if (strcmp(Variable->Name, "Path") == 0)
			Path = Variable->Data.Str;
		else if (strcmp(Variable->Name, "ShowInAssetBrowser") == 0)
			ShowInAssetBrowser = Variable->Data.Bool;
	}
	AddTexture(Path, ShowInAssetBrowser);
}

void SceneLoadMaterial(JsonObject* Object)
{
	SceneMaterial Material;

	for (size_t i = 0; i < Object->Refrences.Size; i++)
	{
		JsonVariables* Variable = (JsonVariables*)DynamicArrayGetAt(&Object->Refrences, i);
		if (strcmp(Variable->Name, "Name") == 0)
			strcpycut(Material.Name, Variable->Data.Str);
		else if (strcmp(Variable->Name, "AlbedoIndex") == 0) Material.AlbedoIndex = Variable->Data.Int;
		else if (strcmp(Variable->Name, "NormalIndex") == 0) Material.NormalIndex = Variable->Data.Int;
		else if (strcmp(Variable->Name, "MetallicIndex") == 0) Material.MetallicIndex = Variable->Data.Int;
		else if (strcmp(Variable->Name, "RoughnessIndex") == 0) Material.RoughnessIndex = Variable->Data.Int;
		else if (strcmp(Variable->Name, "OcclusionIndex") == 0)	Material.OcclusionIndex = Variable->Data.Int;
		else if (strcmp(Variable->Name, "Color") == 0)
		{
			JsonObject* ColorObj = (JsonObject*)Variable;
			for (size_t j = 0; j < ColorObj->Refrences.Size; j++)
			{
				JsonVariables* Color = (JsonVariables*)DynamicArrayGetAt(&ColorObj->Refrences, j);
				Material.Color.Arr[j] = Color->Data.Double;
			}
			
		}
		else if (strcmp(Variable->Name, "Metallic") == 0)	Material.Metallic = Variable->Data.Double;
		else if (strcmp(Variable->Name, "Roughness") == 0)	Material.Roughness = Variable->Data.Double;
		else if (strcmp(Variable->Name, "Occlusion") == 0)	Material.Occlusion = Variable->Data.Double;

	}
	AddMaterial(&Material);
}

void SceneLoad(const char* FileName)
{
	Json Jsn;
	if (JsonParseFile(FileName, &Jsn) != JSON_ERROR)
	{
		JsonObject* Base = (JsonObject*)DynamicArrayGetAt(&Jsn.Objects, 0);

		for (size_t i = 0; i < Base->Refrences.Size; i++)
		{
			JsonObject* Objects = (JsonObject*)DynamicArrayGetAt(&Base->Refrences, i);
			printf("Object: %s\n", Objects->Name);

		//	
			
			if (strcmp(Objects->Name, "Textures") == 0)
			{
				for (size_t i = 0; i < Objects->Refrences.Size; i++)
				{
					JsonObject* Object = (JsonObject*)DynamicArrayGetAt(&Objects->Refrences, i);
					SceneLoadTexture(Object);
				}
				
			}
			else
			{
				if (strcmp(Objects->Name, "Materials") == 0)
				{
					for (size_t i = 0; i < Objects->Refrences.Size; i++)
					{
						JsonObject* Object = (JsonObject*)DynamicArrayGetAt(&Objects->Refrences, i);
						SceneLoadMaterial(Object);
					}

				}
			}
				

			//FIX - Give Objets better names
		}


		printf("Loaded Scene: %s\n", FileName);
		JsonDestroy(&Jsn);
	}


}