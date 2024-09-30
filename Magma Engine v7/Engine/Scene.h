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

	fprintf(File, "\t\"Scene\": {\n");
	{
		fprintf(File, "\t\t\"CameraFOV\": %f,\n", CameraFOV);
		fprintf(File, "\t\t\"CameraZoomFOV\": %f,\n", CameraZoomFOV);
		fprintf(File, "\t\t\"CameraNormalFOV\": %f,\n", CameraNormalFOV);
		fprintf(File, "\t\t\"CameraNearZ\": %f,\n", CameraNearZ);
		fprintf(File, "\t\t\"CameraFarZ\": %f,\n", CameraFarZ);
		fprintf(File, "\t\t\"CameraYaw\": %f,\n", CameraInfo.Yaw);
		fprintf(File, "\t\t\"CameraPitch\": %f,\n", CameraInfo.Pitch);
		fprintf(File, "\t\t\"CameraPos\": [%f, %f, %f],\n", CameraPos.x, CameraPos.y, CameraPos.z);
		fprintf(File, "\t\t\"CameraDir\": [%f, %f, %f],\n", CameraDir.x, CameraDir.y, CameraDir.z);
		fprintf(File, "\t\t\"CameraUp\": [%f, %f, %f],\n", CameraUp.x, CameraUp.y, CameraUp.z);
		fprintf(File, "\t\t\"CameraRight\": [%f, %f, %f],\n", CameraRight.x, CameraRight.y, CameraRight.z);
		fprintf(File, "\t\t\"ClearColor\": [%f, %f, %f]\n", ClearColor.x, ClearColor.y, ClearColor.z);

	}
	fprintf(File, "\t},\n\n");

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
					fprintf(File, "\t\t\t\"Vertices\": \"%s\",\n", Base64EncodeBuffer);
				}
				if (Mesh->Indices)
				{
					DWORD Length = 0;
					Base64Encode((BYTE*)Mesh->Indices, (size_t)Mesh->TotalIndexCount * sizeof(uint32_t), Base64EncodeBuffer, &Length);
					fprintf(File, "\t\t\t\"Indices\": \"%s\",\n", Base64EncodeBuffer);
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

	fprintf(File, "\t\"Entities\": [\n");
	for (uint32_t i = 0; i < EntityCount; i++)
	{
		EntityInfo* Entity = &Entities[i];

		fprintf(File, "\t\t{\n");
		{
			fprintf(File, "\t\t\t\"Name\": \"%s\",\n", Entity->Name);
			fprintf(File, "\t\t\t\"Translate\": [%f, %f, %f],\n", Entity->Translate.x, Entity->Translate.y, Entity->Translate.z);
			fprintf(File, "\t\t\t\"Rotate\": [%f, %f, %f],\n", Entity->Rotate.x, Entity->Rotate.y, Entity->Rotate.z);
			fprintf(File, "\t\t\t\"Scale\": [%f, %f, %f],\n", Entity->Scale.x, Entity->Scale.y, Entity->Scale.z);
			fprintf(File, "\t\t\t\"Selected\": %s,\n", Entity->Selected == true ? "true" : "false");
			fprintf(File, "\t\t\t\"UsedComponents\": [");

			uint32_t LastEnabledCompInArray = 0;
			for (uint32_t j = COMPONENT_COUNT - 1; j > 0; j--)
			{
				if (Entity->UsedComponents[j] == true)
				{
					LastEnabledCompInArray = j;
					break;
				}			
			}

			for (uint32_t j = 0; j < COMPONENT_COUNT; j++)
			{
				if (Entity->UsedComponents[j] == true)
					fprintf(File, "\"%s\"%s", ComponentNames[j], j == LastEnabledCompInArray ? "" : ", ");
			}
			fprintf(File, "],\n");

			fprintf(File, "\t\t\t\"MeshComponent\": {\n");
			fprintf(File, "\t\t\t\t\"Name\": \"%s\",\n", Entity->Mesh.Name);
			fprintf(File, "\t\t\t\t\"MeshIndex\": %d\n", Entity->Mesh.MeshIndex);
			fprintf(File, "\t\t\t},\n");

			fprintf(File, "\t\t\t\"MaterialComponent\": {\n");
			fprintf(File, "\t\t\t\t\"Name\": \"%s\",\n", Entity->Material.Name);
			fprintf(File, "\t\t\t\t\"MaterialIndex\": %d\n", Entity->Material.MaterialIndex);
			fprintf(File, "\t\t\t},\n");

			fprintf(File, "\t\t\t\"CameraComponent\": {\n");
			fprintf(File, "\t\t\t\t\"Name\": \"%s\",\n", Entity->Camera.Name);
			fprintf(File, "\t\t\t\t\"FOV\": %f,\n", Entity->Camera.FOV);
			fprintf(File, "\t\t\t\t\"NearPlane\": %f,\n", Entity->Camera.NearPlane);
			fprintf(File, "\t\t\t\t\"FarPlane\": %f\n", Entity->Camera.FarPlane);
			fprintf(File, "\t\t\t},\n");

			//FIX - Audio Component

			fprintf(File, "\t\t\t\"AnimationComponent\": {\n");
			fprintf(File, "\t\t\t\t\"Name\": \"%s\",\n", Entity->Animation.Name);
			fprintf(File, "\t\t\t\t\"AnimationIndex\": %d\n", Entity->Animation.AnimationIndex);
			fprintf(File, "\t\t\t},\n");

			fprintf(File, "\t\t\t\"LightComponent\": {\n");
			fprintf(File, "\t\t\t\t\"Name\": \"%s\",\n", Entity->Light.Name);
			fprintf(File, "\t\t\t\t\"Color\": [%f, %f, %f],\n", Entity->Light.Color.r, Entity->Light.Color.g, Entity->Light.Color.b);
			fprintf(File, "\t\t\t\t\"Strength\": %f,\n", Entity->Light.Strength);
			fprintf(File, "\t\t\t\t\"Type\": \"%s\",\n", LightNames[Entity->Light.Type]);
			fprintf(File, "\t\t\t\t\"CastShadow\": %s,\n", Entity->Light.CastShadow == true ? "true" : "false");
			fprintf(File, "\t\t\t},\n");

			fprintf(File, "\t\t\t\"ColliderComponent\": {\n");
			fprintf(File, "\t\t\t\t\"Name\": \"%s\",\n", Entity->Collider.Name);
			fprintf(File, "\t\t\t\t\"Type\": \"%s\",\n", ColliderNames[Entity->Collider.Collider]);
			
			fprintf(File, "\t\t\t\t\"Friction\": %f,\n", Entity->Collider.Friction);
			fprintf(File, "\t\t\t\t\"Bounciness\": %f\n", Entity->Collider.Bounciness);
			
			fprintf(File, "\t\t\t},\n");

			fprintf(File, "\t\t\t\"RigidbodyComponent\": {\n");
			fprintf(File, "\t\t\t\t\"Name\": \"%s\",\n", Entity->Rigidbody.Name);
			fprintf(File, "\t\t\t\t\"Mass\": %f\n", Entity->Rigidbody.Mass);
			fprintf(File, "\t\t\t}\n");

		//	fprintf(File, "\t\t\t\"Path\": \"%s\",\n", Texture->Path);
		//	fprintf(File, "\t\t\t\"Width\": %d,\n", Texture->Width);
		//	fprintf(File, "\t\t\t\"Height\": %d,\n", Texture->Height);
		//	fprintf(File, "\t\t\t\"Format\": %d,\n", Texture->Format);
		//	fprintf(File, "\t\t\t\"MipLevels\": %d,\n", Texture->MipLevels);
		//	fprintf(File, "\t\t\t\"Data\": null%s\n", i == (SceneTextures.Size - 1) ? "" : ",");
		}
		fprintf(File, "\t\t}%s\n", i == (EntityCount - 1) ? "" : ",");
	}
	fprintf(File, "\t]\n");

	fprintf(File, "}\n");

	fclose(File);

	free(Base64EncodeBuffer);

	printf("Saved Scene: %s\n", FileName);
}


//This criminal macro could also be a function using a template
#define SCENE_DECLARE_TYPE(type) \
void SceneLoadArray##type(type *DstArray, JsonObject* ArrayObj) { \
    for (size_t j = 0; j < ArrayObj->Refrences.Size; j++) \
	{ \
		JsonVariables* Var = (JsonVariables*)DynamicArrayGetAt(&ArrayObj->Refrences, j); \
		switch (Var->Type) \
		{ \
		case JSON_INT: \
			DstArray[j] = (type)Var->Data.Int; \
			break; \
		case JSON_DUB: \
			DstArray[j] = (type)Var->Data.Double; \
			break; \
		case JSON_BOOL: \
			DstArray[j] = (type)Var->Data.Bool; \
			break; \
		default:\
			break;\
		} \
	} \
}

SCENE_DECLARE_TYPE(int32_t)
SCENE_DECLARE_TYPE(float)
SCENE_DECLARE_TYPE(bool)

void SceneLoadScene(JsonObject* Object)
{
	JsonVariables* Variable = (JsonVariables*)Object;
	if (strcmp(Variable->Name, "CameraFOV") == 0) CameraFOV = Variable->Data.Double;
	else if (strcmp(Variable->Name, "CameraZoomFOV") == 0) CameraZoomFOV = Variable->Data.Double;
	else if (strcmp(Variable->Name, "CameraNormalFOV") == 0) CameraNormalFOV = Variable->Data.Double;
	else if (strcmp(Variable->Name, "CameraNearZ") == 0) CameraNearZ = Variable->Data.Double;
	else if (strcmp(Variable->Name, "CameraFarZ") == 0) CameraFarZ = Variable->Data.Double;
	else if (strcmp(Variable->Name, "CameraYaw") == 0) CameraInfo.Yaw = Variable->Data.Double;
	else if (strcmp(Variable->Name, "CameraPitch") == 0) CameraInfo.Pitch = Variable->Data.Double;
	else if (strcmp(Variable->Name, "CameraPos") == 0) SceneLoadArrayfloat(CameraPos.Arr, (JsonObject*)Variable);
	else if (strcmp(Variable->Name, "CameraDir") == 0) SceneLoadArrayfloat(CameraDir.Arr, (JsonObject*)Variable);
	else if (strcmp(Variable->Name, "CameraUp") == 0) SceneLoadArrayfloat(CameraUp.Arr, (JsonObject*)Variable);
	else if (strcmp(Variable->Name, "CameraRight") == 0) SceneLoadArrayfloat(CameraRight.Arr, (JsonObject*)Variable);
	else if (strcmp(Variable->Name, "ClearColor") == 0) SceneLoadArrayfloat(ClearColor.Arr, (JsonObject*)Variable);
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

typedef struct
{
	union
	{
			char* Str;
			int32_t Int32;
			float Float32;
			bool Bool;


		/*
		union
		{
			char* Str;
		} StringArray;

		union
		{
			int32_t Int32;
		} Int32Array;

		union
		{
			float Float32;
		} Float32Array;

		union
		{
			bool Bool;
		} BoolArray;
		*/
	};
	
} SceneArrayTypes;

/*
//doesn't allocate string - yet?
void SceneLoadArray(SceneArrayTypes* DstArray, JsonObject* ArrayObj)
{	

	for (size_t j = 0; j < ArrayObj->Refrences.Size; j++)
	{
		JsonVariables* Var = (JsonVariables*)DynamicArrayGetAt(&ArrayObj->Refrences, j);
		switch (Var->Type)
		{
		case JSON_STR:
			DstArray[j].Str = Var->Data.Str;
			break;
		case JSON_INT:
			DstArray[j].Int32 = Var->Data.Int;
			break;
		case JSON_DUB:
			DstArray[j].Float32 = Var->Data.Double;
			break;
		case JSON_BOOL:
			DstArray[j].Bool = Var->Data.Bool;
			break;
		
		}
	}
}
*/

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
		//	JsonObject* ColorObj = (JsonObject*)Variable;
		//	for (size_t j = 0; j < ColorObj->Refrences.Size; j++)
		//	{
		//		JsonVariables* Color = (JsonVariables*)DynamicArrayGetAt(&ColorObj->Refrences, j);
		//		Material.Color.Arr[j] = Color->Data.Double;
		//	}

			SceneLoadArrayfloat(Material.Color.Arr, (JsonObject*)Variable);
		}
		else if (strcmp(Variable->Name, "Metallic") == 0)	Material.Metallic = Variable->Data.Double;
		else if (strcmp(Variable->Name, "Roughness") == 0)	Material.Roughness = Variable->Data.Double;
		else if (strcmp(Variable->Name, "Occlusion") == 0)	Material.Occlusion = Variable->Data.Double;

	}
	AddMaterial(&Material);
}

void SceneLoadMeshData(SceneMeshData* MeshData, JsonObject* MeshDataElements)
{
	memset(MeshData, 0, sizeof(SceneMeshData));

	for (size_t j = 0; j < MeshDataElements->Refrences.Size; j++)
	{
		JsonVariables* MeshDataVars = (JsonVariables*)DynamicArrayGetAt(&MeshDataElements->Refrences, j);

		if (strcmp(MeshDataVars->Name, "Name") == 0)		strcpycut(MeshData->Name, MeshDataVars->Data.Str);
		else if (strcmp(MeshDataVars->Name, "MaterialIndex") == 0)	MeshData->MaterialIndex = MeshDataVars->Data.Int;
		else if (strcmp(MeshDataVars->Name, "VertexOffset") == 0)	MeshData->VertexOffset = MeshDataVars->Data.Int;
		else if (strcmp(MeshDataVars->Name, "VertexCount") == 0)	MeshData->VertexCount = MeshDataVars->Data.Int;
		else if (strcmp(MeshDataVars->Name, "IndexOffset") == 0)	MeshData->IndexOffset = MeshDataVars->Data.Int;
		else if (strcmp(MeshDataVars->Name, "IndexCount") == 0)		MeshData->IndexCount = MeshDataVars->Data.Int;
		else if (strcmp(MeshDataVars->Name, "Min") == 0)	SceneLoadArrayfloat(MeshData->AABB.Min.Arr, (JsonObject*)MeshDataVars);
		else if (strcmp(MeshDataVars->Name, "Max") == 0)	SceneLoadArrayfloat(MeshData->AABB.Max.Arr, (JsonObject*)MeshDataVars);
		else if (strcmp(MeshDataVars->Name, "Render") == 0)	SceneLoadArraybool(MeshData->Render, (JsonObject*)MeshDataVars);
	}
}

void SceneLoadMesh(JsonObject* Object)
{
	SceneMesh MeshInfo;
	memset(&MeshInfo, 0, sizeof(SceneMesh));

	for (size_t i = 0; i < Object->Refrences.Size; i++)
	{
		JsonVariables* Variable = (JsonVariables*)DynamicArrayGetAt(&Object->Refrences, i);
		if (strcmp(Variable->Name, "Name") == 0)		strcpycut(MeshInfo.Name, Variable->Data.Str);
		else if (strcmp(Variable->Name, "Path") == 0)	strcpycut(MeshInfo.Path, Variable->Data.Str);
		else if (strcmp(Variable->Name, "MeshCount") == 0)
		{
			MeshInfo.MeshCount = Variable->Data.Int;
			MeshInfo.MeshData = (SceneMeshData*)malloc(MeshInfo.MeshCount * sizeof(SceneMeshData));
			if (MeshInfo.MeshData == NULL)
			{
				printf("Failed to allocate MeshData from scene load\n");
				return;
			}

			//For later array looping is set to zero here
			MeshInfo.MeshCount = 0;
		}
		else if (strcmp(Variable->Name, "Destroyable") == 0) MeshInfo.Destroyable = Variable->Data.Bool;
		else if (strcmp(Variable->Name, "TotalVertexCount") == 0) MeshInfo.TotalVertexCount = Variable->Data.Int;
		else if (strcmp(Variable->Name, "TotalIndexCount") == 0) MeshInfo.TotalIndexCount = Variable->Data.Int;
		else if (strcmp(Variable->Name, "Vertices") == 0)
		{
			MeshInfo.Vertices = (SceneVertex*)malloc(MeshInfo.TotalVertexCount * sizeof(SceneVertex));
			if (MeshInfo.Vertices == NULL)
			{
				printf("Failed to allocate vertices for: %s while scene loading\n", MeshInfo.Name);
				return;
			}
			printf("Jason derulo\n");
			
			DWORD Length = 0;
			Base64Decode((BYTE*)Variable->Data.Str, (BYTE*)MeshInfo.Vertices, &Length);
			if (Length != (DWORD)(MeshInfo.TotalVertexCount * sizeof(SceneVertex)))
				printf("Base64 parse length not the same as vertex size would guess\n");

			MeshInfo.VertexBuffer = OpenVkCreateVertexBuffer(MeshInfo.TotalVertexCount * sizeof(SceneVertex), MeshInfo.Vertices);
			if (MeshInfo.VertexBuffer == OPENVK_ERROR)
			{
				printf("Failed to allocate scene Vertex buffer for: %s, count: %d\n", MeshInfo.Path, MeshInfo.TotalVertexCount);
				free(MeshInfo.Vertices);
				return;
			}
		}
		else if (strcmp(Variable->Name, "Indices") == 0)
		{
			MeshInfo.Indices = (uint32_t*)malloc(MeshInfo.TotalIndexCount * sizeof(uint32_t));
			if (MeshInfo.Indices == NULL)
			{
				printf("Failed to allocate indices for: %s while scene loading\n", MeshInfo.Name);
				return;
			}

		
			DWORD Length = 0;
			Base64Decode((BYTE*)Variable->Data.Str, (BYTE*)MeshInfo.Indices, &Length);
			if (Length != (DWORD)(MeshInfo.TotalIndexCount * sizeof(uint32_t)))
				printf("Base64 parse length not the same as index size would guess\n");

			MeshInfo.IndexBuffer = OpenVkCreateIndexBuffer(MeshInfo.TotalIndexCount * sizeof(uint32_t), MeshInfo.Indices);
			if (MeshInfo.IndexBuffer == OPENVK_ERROR)
			{
				printf("Failed to allocate scene Index buffer for: %s, count: %d\n", MeshInfo.Path, MeshInfo.TotalIndexCount);
				free(MeshInfo.Vertices);
				free(MeshInfo.Indices);
				return;
			}

		}
		else if (strcmp(Variable->Name, "MeshData") == 0)
		{
			JsonObject* MeshData = (JsonObject*)Variable;
			

			for (size_t j = 0; j < MeshData->Refrences.Size; j++)
			{
				JsonObject* MeshDataElements = (JsonObject*)DynamicArrayGetAt(&MeshData->Refrences, j);
	
				SceneLoadMeshData(&MeshInfo.MeshData[MeshInfo.MeshCount++], MeshDataElements);
			}
		}
	}

	printf("Yeaj\n");

	uint32_t MeshIndex = AddMesh(&MeshInfo);
	RaytracingAddGeometry(MeshIndex);
}
/*
void SceneLoadComponent(JsonObject* Variable)
{
	JsonObject* Components = (JsonObject*)DynamicArrayGetAt(&((JsonObject*)Variable)->Refrences, i);
	for (size_t j = 0; j < Components->Refrences.Size; j++)
	{
		JsonVariables* Comp = (JsonVariables*)DynamicArrayGetAt(&Components->Refrences, j);
		if (strcmp(Variable->Name, "Name") == 0) strcpycut(Entity.Mesh.Name, Variable->Data.Str);
		else if (strcmp(Variable->Name, "MeshIndex") == 0) Entity.Mesh.MeshIndex = Variable->Data.Int;
	}
}
*/

void SceneLoadEntity(JsonObject* Object)
{
	EntityInfo Entity;
	memset(&Entity, 0, sizeof(EntityInfo));

	for (size_t i = 0; i < Object->Refrences.Size; i++)
	{
		JsonVariables* Variable = (JsonVariables*)DynamicArrayGetAt(&Object->Refrences, i);
		if (strcmp(Variable->Name, "Name") == 0) strcpycut(Entity.Name, Variable->Data.Str);
		else if (strcmp(Variable->Name, "Translate") == 0) SceneLoadArrayfloat(Entity.Translate.Arr, (JsonObject*)Variable);
		else if (strcmp(Variable->Name, "Rotate") == 0) SceneLoadArrayfloat(Entity.Rotate.Arr, (JsonObject*)Variable);
		else if (strcmp(Variable->Name, "Scale") == 0) SceneLoadArrayfloat(Entity.Scale.Arr, (JsonObject*)Variable);
		else if (strcmp(Variable->Name, "Selected") == 0) Entity.Selected = Variable->Data.Bool;
		else if (strcmp(Variable->Name, "UsedComponents") == 0)
		{
			JsonObject* Components = (JsonObject*)Variable;
			for (size_t j = 0; j < Components->Refrences.Size; j++)
			{
				JsonVariables* Comp = (JsonVariables*)DynamicArrayGetAt(&Components->Refrences, j);
				for (size_t k = 0; k < COMPONENT_COUNT; k++)
				{
				//	if (strcmp(Components->Name, ComponentNames[k]) == 0)
						Entity.UsedComponents[k] = (strcmp(Comp->Data.Str, ComponentNames[k]) == 0) ? true : false;
					
				}
				
				/*
				if (strcmp(Variable->Name, "Empty") == 0) Entity.UsedComponents[COMPONENT_TYPE_EMPTY] = Variable->Data.Bool;
				else if (strcmp(Variable->Name, "Mesh") == 0) Entity.UsedComponents[COMPONENT_TYPE_MESH] = Variable->Data.Bool;
				else if (strcmp(Variable->Name, "Material") == 0) Entity.UsedComponents[COMPONENT_TYPE_MATERIAL] = Variable->Data.Bool;
				else if (strcmp(Variable->Name, "Camera") == 0) Entity.UsedComponents[COMPONENT_TYPE_CAMERA] = Variable->Data.Bool;
				else if (strcmp(Variable->Name, "Audio") == 0) Entity.UsedComponents[COMPONENT_TYPE_AUDIO] = Variable->Data.Bool;
				else if (strcmp(Variable->Name, "Animation") == 0) Entity.UsedComponents[COMPONENT_TYPE_ANIMATION] = Variable->Data.Bool;
				else if (strcmp(Variable->Name, "Light") == 0) Entity.UsedComponents[COMPONENT_TYPE_LIGHT] = Variable->Data.Bool;
				else if (strcmp(Variable->Name, "Collider") == 0) Entity.UsedComponents[COMPONENT_TYPE_COLLIDER] = Variable->Data.Bool;
				else if (strcmp(Variable->Name, "Rigidbody") == 0) Entity.UsedComponents[COMPONENT_TYPE_RIGIDBODY] = Variable->Data.Bool;
				*/
			}
		}
		//FIX - most of this could be shortended via a general function 
		else if (strcmp(Variable->Name, "MeshComponent") == 0)
		{
			JsonObject* Components = (JsonObject*)Variable;
			for (size_t j = 0; j < Components->Refrences.Size; j++)
			{
				JsonVariables* Comp = (JsonVariables*)DynamicArrayGetAt(&Components->Refrences, j);
				if (strcmp(Comp->Name, "Name") == 0) strcpycut(Entity.Mesh.Name, Comp->Data.Str);
				else if (strcmp(Comp->Name, "MeshIndex") == 0) Entity.Mesh.MeshIndex = Comp->Data.Int;
			}

			RaytracingAddMesh(Entity.Mesh.MeshIndex);
		}
		else if (strcmp(Variable->Name, "MaterialComponent") == 0)
		{
			JsonObject* Components = (JsonObject*)Variable;
			for (size_t j = 0; j < Components->Refrences.Size; j++)
			{
				JsonVariables* Comp = (JsonVariables*)DynamicArrayGetAt(&Components->Refrences, j);
				if (strcmp(Comp->Name, "Name") == 0) strcpycut(Entity.Material.Name, Comp->Data.Str);
				else if (strcmp(Comp->Name, "MaterialIndex") == 0) Entity.Material.MaterialIndex = Comp->Data.Int;
			}
		}
		else if (strcmp(Variable->Name, "CameraComponent") == 0)
		{
			JsonObject* Components = (JsonObject*)Variable;
			for (size_t j = 0; j < Components->Refrences.Size; j++)
			{
				JsonVariables* Comp = (JsonVariables*)DynamicArrayGetAt(&Components->Refrences, j);
				if (strcmp(Comp->Name, "Name") == 0) strcpycut(Entity.Camera.Name, Comp->Data.Str);
				else if (strcmp(Comp->Name, "FOV") == 0) Entity.Camera.FOV = Comp->Data.Double;
				else if (strcmp(Comp->Name, "NearPlane") == 0) Entity.Camera.NearPlane = Comp->Data.Double;
				else if (strcmp(Comp->Name, "FarPlane") == 0) Entity.Camera.FarPlane = Comp->Data.Double;
			}
		}
		else if (strcmp(Variable->Name, "AnimationComponent") == 0)
		{
			JsonObject* Components = (JsonObject*)Variable;
			for (size_t j = 0; j < Components->Refrences.Size; j++)
			{
				JsonVariables* Comp = (JsonVariables*)DynamicArrayGetAt(&Components->Refrences, j);
				if (strcmp(Comp->Name, "Name") == 0) strcpycut(Entity.Animation.Name, Comp->Data.Str);
				else if (strcmp(Comp->Name, "AnimationIndex") == 0) Entity.Animation.AnimationIndex = Comp->Data.Int;
			}
		}
		else if (strcmp(Variable->Name, "LightComponent") == 0)
		{
			JsonObject* Components = (JsonObject*)Variable;
			for (size_t j = 0; j < Components->Refrences.Size; j++)
			{
				JsonVariables* Comp = (JsonVariables*)DynamicArrayGetAt(&Components->Refrences, j);
				if (strcmp(Comp->Name, "Name") == 0) strcpycut(Entity.Light.Name, Comp->Data.Str);
				else if (strcmp(Comp->Name, "Color") == 0) SceneLoadArrayfloat(Entity.Light.Color.Arr, (JsonObject*)Comp);
				else if (strcmp(Comp->Name, "Strength") == 0) Entity.Light.Strength = Comp->Data.Double;
				else if (strcmp(Comp->Name, "Type") == 0)
				{
					for (uint32_t k = 0; k < LIGHT_COUNT; k++)
					{
						if (strcmp(Comp->Data.Str, LightNames[k]) == 0)
						{
							Entity.Light.Type = (LightTypes)k;
							break;
						}
					}
					/*
						 if (strcmp(Comp->Name, "Point") == 0)			Entity.Light.Type = LIGHT_POINT;
					else if (strcmp(Comp->Name, "Directional") == 0)	Entity.Light.Type = LIGHT_DIRECTIONAL;
					else if (strcmp(Comp->Name, "Spot") == 0)			Entity.Light.Type = LIGHT_SPOT;
					*/
				}
				else if (strcmp(Comp->Name, "CastShadow") == 0) Entity.Light.CastShadow = Comp->Data.Bool;
			}
		}
		else if (strcmp(Variable->Name, "ColliderComponent") == 0)
		{
			JsonObject* Components = (JsonObject*)Variable;
			for (size_t j = 0; j < Components->Refrences.Size; j++)
			{
				JsonVariables* Comp = (JsonVariables*)DynamicArrayGetAt(&Components->Refrences, j);
				if (strcmp(Comp->Name, "Name") == 0) strcpycut(Entity.Collider.Name, Comp->Data.Str);
				else if (strcmp(Comp->Name, "Type") == 0)
				{
					for (uint32_t k = 0; k < COLLIDER_COUNT; k++)
					{
						if (strcmp(Comp->Data.Str, ColliderNames[k]) == 0)
						{
							Entity.Collider.Collider = (ColliderTypes)k;
							break;
						}
					}
				}
				else if (strcmp(Comp->Name, "Friction") == 0) Entity.Collider.Friction = Comp->Data.Double;
				else if (strcmp(Comp->Name, "Bounciness") == 0) Entity.Collider.Bounciness = Comp->Data.Double;
			}
		}
		else if (strcmp(Variable->Name, "RigidbodyComponent") == 0)
		{
			JsonObject* Components = (JsonObject*)Variable;
			for (size_t j = 0; j < Components->Refrences.Size; j++)
			{
				JsonVariables* Comp = (JsonVariables*)DynamicArrayGetAt(&Components->Refrences, j);
				if (strcmp(Comp->Name, "Name") == 0) strcpycut(Entity.Rigidbody.Name, Comp->Data.Str);
				else if (strcmp(Comp->Name, "Bounciness") == 0) Entity.Rigidbody.Mass = Comp->Data.Double;
			}
		}
	}

	AddEntity(&Entity);
}

void SceneParseObjectRefernces(JsonObject* Objects, void(*LoadFunc)(JsonObject* Object))
{
	for (size_t i = 0; i < Objects->Refrences.Size; i++)
	{
		JsonObject* Object = (JsonObject*)DynamicArrayGetAt(&Objects->Refrences, i);
		LoadFunc(Object);
	}
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
			if (strcmp(Objects->Name, "Scene") == 0)
			{
				SceneParseObjectRefernces(Objects, SceneLoadScene);
			}
			else if (strcmp(Objects->Name, "Textures") == 0)
			{
				SceneParseObjectRefernces(Objects, SceneLoadTexture);				
			}
			else if(strcmp(Objects->Name, "Materials") == 0)
			{
				SceneParseObjectRefernces(Objects, SceneLoadMaterial);
			}
			else if (strcmp(Objects->Name, "Meshes") == 0)
			{
				SceneParseObjectRefernces(Objects, SceneLoadMesh);
			}
			else if (strcmp(Objects->Name, "Entities") == 0)
			{
				SceneParseObjectRefernces(Objects, SceneLoadEntity);
			}
				

			//FIX - Give Objets better names
		}


		printf("Loaded Scene: %s\n", FileName);
		JsonDestroy(&Jsn);
	}


}