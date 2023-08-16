uint32_t SceneCreateDefaultTexture(SceneTextureImage* Image)
{
	OpenVkDeviceWaitIdle();
	Image->Format = OPENVK_FORMAT_RGBA;
	Image->Width = 2;
	Image->Height = 2;

	unsigned char* Pixels = (unsigned char*)malloc(16);
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
	CMA_Push(&SceneTextures, &Image);

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
	//Mesh
	SceneMeshes = CMA_Create(sizeof(SceneMesh), "Magma Engine, Scene Meshes");
	SceneMesh Mesh;
	Mesh.MeshData = (SceneMeshData*)malloc(1 * sizeof(SceneMeshData));
	strcpy(Mesh.Name, "None");
	strcpy(Mesh.Path, "None");
	Mesh.MeshCount = 0;
	Mesh.Destroyable = false;

	SetDefaultMaterial(&Mesh.MeshData[0].Material, "MESH");
	Mesh.MeshData[0].VertexOffset = 0;
	Mesh.MeshData[0].IndexOffset = 0;
	Mesh.MeshData[0].VertexCount = 0;
	Mesh.MeshData[0].IndexCount = 0;
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
	CMA_Destroy(&SceneTextures);
	CMA_Destroy(&SceneMaterials);
	CMA_Destroy(&SceneMeshes);
	CMA_Destroy(&SceneAnimations);
	free(SceneScripts);
}