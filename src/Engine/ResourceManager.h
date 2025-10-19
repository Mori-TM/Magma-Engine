void RendererResize(OpenVkBool RecreateSwapChain);
void CreateGraphicsPipelines();
void CreateDescriptors();

void CheckForTextureDeletion()
{
	if (DeleteTexture)
	{
		DeleteTexture = false;

		SceneTextureImage* DefaultImage = (SceneTextureImage*)CMA_GetAt(&SceneTextures, 0);
		if (DefaultImage == NULL)
		{ 
			printf("WTF is the default texture?\n");
			exit(0);
		}

		if (TextureToDelete != DefaultImage->TextureImage)	 OpenVkDestroyImage(TextureToDelete);
		if (SamplerToDelete != DefaultImage->TextureSampler) OpenVkDestroySampler(SamplerToDelete);

	//	RendererResize(false);
		OpenVkDeviceWaitIdle();
		OpenVkDestroyDescriptorPool(DescriptorPool);
		CreateDescriptors();
	}
}

void DeleteMeshTexture(uint32_t TextureImage, uint32_t TextureIndex)
{
	if (TextureIndex != 0)
	{
		SceneTextureImage* Image = (SceneTextureImage*)CMA_GetAt(&SceneTextures, TextureIndex);
		if (Image != NULL && Image->TextureImage != 0 && Image->TextureSampler != 0)
		{
			SceneTextureImage* DefaultImage = (SceneTextureImage*)CMA_GetAt(&SceneTextures, 0);
			if (DefaultImage == NULL)
			{
				printf("WTF is the default texture?\n");
				exit(0);
			}

			if (Image->TextureImage != DefaultImage->TextureImage)		OpenVkDestroyImage(Image->TextureImage);
			if (Image->TextureSampler != DefaultImage->TextureSampler)  OpenVkDestroySampler(Image->TextureSampler);

//			OpenVkDestroyImage(Image->TextureImage);
//			OpenVkDestroySampler(Image->TextureSampler);
			CMA_Pop(&SceneTextures, TextureIndex);
		}
	}
}

void CheckForModelDeletion()
{
	if (ModelToDeleteOptions != MODEL_DELETE_NOTHING)
	{
		SceneMesh* Mesh = (SceneMesh*)CMA_GetAt(&SceneMeshes, ModelToDelete);
		if (Mesh != NULL)
		{
			if (ModelToDeleteOptions & MODEL_DELETE_TEXURES)
			{
				for (uint32_t i = 0; i < Mesh->MeshCount; i++)
				{
					SceneMaterial* Material = (SceneMaterial*)CMA_GetAt(&SceneMaterials, Mesh->MeshData[i].MaterialIndex);
					if (Material)
					{
						SceneTextureImage* Albedo	= (SceneTextureImage*)CMA_GetAt(&SceneTextures, Material->AlbedoIndex);
						SceneTextureImage* Normal	= (SceneTextureImage*)CMA_GetAt(&SceneTextures, Material->NormalIndex);
						SceneTextureImage* Metallic = (SceneTextureImage*)CMA_GetAt(&SceneTextures, Material->MetallicIndex);
						SceneTextureImage* Roughness= (SceneTextureImage*)CMA_GetAt(&SceneTextures, Material->RoughnessIndex);
						SceneTextureImage* Occlusion= (SceneTextureImage*)CMA_GetAt(&SceneTextures, Material->OcclusionIndex);

						if (Albedo) 	DeleteMeshTexture(Albedo->TextureImage,		Material->AlbedoIndex);
						if (Normal) 	DeleteMeshTexture(Normal->TextureImage,		Material->NormalIndex);
						if (Metallic) 	DeleteMeshTexture(Metallic->TextureImage,	Material->MetallicIndex);
						if (Roughness) 	DeleteMeshTexture(Roughness->TextureImage,	Material->RoughnessIndex);
						if (Occlusion) 	DeleteMeshTexture(Occlusion->TextureImage,	Material->OcclusionIndex);

						Material->AlbedoIndex = 0;
						Material->NormalIndex = 0;
						Material->MetallicIndex = 0;
						Material->RoughnessIndex = 0;
						Material->OcclusionIndex = 0;
					}					
				}				
			}

			if (ModelToDeleteOptions & MODEL_DELETE_MATERIALS)
			{
				for (uint32_t i = 0; i < Mesh->MeshCount; i++)
				{
					SceneMaterial* Material = (SceneMaterial*)CMA_GetAt(&SceneMaterials, Mesh->MeshData[i].MaterialIndex);
					if (Material)
						DeleteMaterial(Mesh->MeshData[i].MaterialIndex);
				}
			}

			if (ModelToDeleteOptions & MODEL_DELETE_MESH && Mesh->Destroyable)
			{
				if (Mesh->VertexBuffer != OPENVK_ERROR)
					OpenVkDestroyBuffer(Mesh->VertexBuffer);
				if (Mesh->IndexBuffer != OPENVK_ERROR)
					OpenVkDestroyBuffer(Mesh->IndexBuffer);

				if (Mesh->Vertices)
					free(Mesh->Vertices);
				if (Mesh->Indices)
					free(Mesh->Indices);
			}

			free(Mesh->MeshData);
		}

		printf("Oi: %zu\n", SceneMeshes.Size);
		CMA_Pop(&SceneMeshes, ModelToDelete);
		SelectedMesh = 0;
		printf("steve: %zu\n", SceneMeshes.Size);

		for (uint32_t i = 1; i < SceneMeshes.Size; i++)
			if (CMA_GetAt(&SceneMeshes, i) != NULL)
				SelectedMesh = i;

		ModelToDeleteOptions = MODEL_DELETE_NOTHING;

		RendererResize(false);
	}
}

void CheckForShaderReloading()
{
	if (ReloadShaders)
	{
		ReloadShaders = false;

		ShaderCompilerRequest();
		
		OpenVkDeviceWaitIdle();

		//FIX - Use OpenVk function!!!
		for (uint32_t i = 0; i < VkRenderer.Pipelines.Size; i++)
		{
			if (i != OpenVkGUI.Pipeline)
			{
				//FIX - check if works the if statement
				if (!(OpenVkHasRaytracingSupport() && i == RTR.RaytracingPipeline))
				{
					VkPipeline* Pipeline = (VkPipeline*)CMA_GetAt(&VkRenderer.Pipelines, i);
					if (Pipeline != NULL)
						vkDestroyPipeline(VkRenderer.Device, *Pipeline, NULL);

					CMA_Pop(&VkRenderer.Pipelines, i);
				}
			}		
		}
		
	//	OpenVkGUIRecreatePipeline();
		CreateGraphicsPipelines();
		RendererResize(false);
	}
}

void CheckForTextureUpdates()
{

}