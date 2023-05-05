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
	SceneTextures = CMA_Create(sizeof(SceneTextureImage));
	SceneTextureImage Image;
	Image.ShowInAssetBrowser = false;
	Image.TextureDescriptorSet = LoadTexture((char*)"Data/Textures/Default.png", &Image);
	strcpy(Image.Name, "None");
	strcpy(Image.Path, "None");
	CMA_Push(&SceneTextures, &Image);

	//	LoadTextureCompressed = true;
	//	GenerateMipMaps = true;
	//	Image.ShowInAssetBrowser = true;
	//	Image.TextureDescriptorSet = LoadTexture((char*)"Data/Textures/OrangeTex.png", &Image);
	//	strcpy(Image.Name, "UvGrid");
	//	strcpy(Image.Path, "None");
	//	CMA_Push(&SceneTextures, &Image);
	//
	//	LoadTextureCompressed = true;
	//	GenerateMipMaps = false;
	//	Image.ShowInAssetBrowser = true;
	//	Image.TextureDescriptorSet = LoadTexture((char*)"Data/Textures/OrangeTex.png", &Image);
	//	strcpy(Image.Name, "UvGrid");
	//	strcpy(Image.Path, "None");
	//	CMA_Push(&SceneTextures, &Image);
	//
	//	LoadTextureCompressed = false;
	//	GenerateMipMaps = false;
	//	Image.ShowInAssetBrowser = true;
	//	Image.TextureDescriptorSet = LoadTexture((char*)"Data/Textures/OrangeTex.png", &Image);
	//	strcpy(Image.Name, "UvGrid");
	//	strcpy(Image.Path, "None");
	//	CMA_Push(&SceneTextures, &Image);
	//
	//	LoadTextureCompressed = false;
	//	GenerateMipMaps = true;
	//	Image.ShowInAssetBrowser = true;
	//	Image.TextureDescriptorSet = LoadTexture((char*)"Data/Textures/OrangeTex.png", &Image);
	//	strcpy(Image.Name, "UvGrid");
	//	strcpy(Image.Path, "None");
	//	CMA_Push(&SceneTextures, &Image);

		//Material
	SceneMaterials = CMA_Create(sizeof(SceneMaterial));
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
	SceneMeshes = CMA_Create(sizeof(SceneMesh));
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

	SceneAnimations = CMA_Create(sizeof(SceneAnimation));
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