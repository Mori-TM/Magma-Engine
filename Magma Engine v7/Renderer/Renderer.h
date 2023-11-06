#include "Pipelines/DebugHelper.h"
#include "Pipelines/ShadowHelper.h"
#include "Pipelines/BlurHelper.h"
#include "Pipelines/GBufferHelper.h"
#include "Pipelines/SSAOHelper.h"
#include "Pipelines/SSAOBlurHelper.h"
#include "Pipelines/SSRHelper.h"
#include "Pipelines/SceneHelper.h"
#include "Pipelines/FXAAHelper.h"
#include "Pipelines/SwapChainHelper.h"

#include "Helper/RendererHelper.h"
#include "Helper/Meshes.h"
#include "Helper/DescriptorSetLayouts.h"
#include "Helper/DescriptorPool.h"
#include "Helper/Buffers.h"
#include "Helper/Sampler.h"
#include "Helper/GUI.h"

#include "../Engine/Engine.h"
#include "../Engine/Physics.h"
#include "../Engine/ImageCompression.h"
#include "../Engine/ECS.h"
#include "../Engine/Scene.h"
#include "../Engine/FpsCamera.h"
#include "../Engine/ScriptEngine.h"
#include "../Engine/FrustumCulling.h"

#include "../Editor/Editor.h"
#include "../Editor/SceneManager.h"
#include "../Editor/AssetBrowser.h"
#include "../Editor/EntityManager.h"
#include "../Editor/Inspector.h"
#include "../Editor/EditorUI.h"

#include "Pipelines/BlurPipeline.h"
#include "Pipelines/DebugPipeline.h"
#include "Pipelines/ShadowPipeline.h"
#include "Pipelines/GBufferPipeline.h"
#include "Pipelines/SSAOPipeline.h"
#include "Pipelines/SSAOBlurPipeline.h"
#include "Pipelines/ScenePipeline.h"
#include "Pipelines/SSRPipeline.h"
#include "Pipelines/FXAAPipeline.h"
#include "Pipelines/SwapChainPipeline.h"

void CreateRenderPasses()
{
	CreateBlurRenderPass();
	CreateShadowRenderPass();
	CreateGBufferRenderPass();
	CreateSSAORenderPass();
	CreateSSAOBlurRenderPass();
	CreateSceneRenderPass();
	CreateSSRRenderPass();
	CreateFXAARenderPass();
	CreateSwapChainRenderPass();
}

void CreatePipelineLayouts()
{
	CreateBlurLayout();
	CreateShadowLayout();
	CreateGBufferLayout();
	CreateSSAOLayout();
	CreateSSAOBlurLayout();
	CreateSceneLayout();
	CreateDebugLayout();
	CreateSSRLayout();
	CreateFXAALayout();
	CreateSwapChainLayout();
	
}

void CreateGraphicsPipelines()
{
	CreateBlurPipeline();
	CreateShadowPipeline();	
	CreateGBufferPipeline();
	CreateSSAOPipeline();
	CreateSSAOBlurPipeline();
	CreateScenePipeline();
	CreateDebugPipeline();	
	CreateSSRPipeline();
	CreateFXAAPipeline();
	CreateSwapChainPipeline();
}

void CreateFramebuffers()
{
	CreateBlurFramebuffers();
	CreateShadowFramebuffers();
	CreateGBufferFramebuffer();
	CreateSSAOFramebuffer();
	CreateSSAOBlurFramebuffer();
	CreateSceneFramebuffer();
	CreateSSRFramebuffer();
	CreateFXAAFramebuffer();
	CreateSwapChainFramebuffer();
}

void CreateDescriptors()
{
	CreateDescriptorPool();
	CreateBlurDescriptorSets();
	CreateShadowDescriptorSet();
	CreateGBufferDescriptorSet();
	CreateSSAODescriptorSets();
	CreateSSAOBlurDescriptorSet();
	CreateSceneDescriptorSets();
	CreateDebugDescriptorSets();
	CreateSSRDescriptorSet();
	CreateFXAADescriptorSet();
}

void CreateRenderer()
{
	OpenVkInitThreads();
	SwapChain = OpenVkCreateRenderer(OPENVK_VULKAN | OPENVK_VALIDATION_LAYER, GetExtensions, GetSurface, GetWindowSize);

	CreateRenderPasses();
	/*
	* 80, "C:/Windows/Fonts/RAGE.TTF"
	* 30, "Data/Fonts/Roboto-Medium.TTF"
	*/
	OpenVkGUIInit(WindowWidth, WindowHeight, SwapChainRenderPass, 1, 30, "Data/Fonts/Roboto-Medium.TTF", GetMousePos);
	CreateDescriptorSetLayout();
	CreateSSAONoiseImage();
	CreatePipelineLayouts();
	CreateGraphicsPipelines();
	
	CreateFramebuffers();

	CreateImageSampler();
	CreateBuffers();

	CreateGBufferUniformBuffer();
	CreateSSAOUniformBuffer();
	CreateSceneUniformBuffer();
	CreateSceneStorageBuffer();
	CreateSSRUniformBuffer();
	
	CreateImGuiDescriptorPool();
	CreateDescriptors();

	EntitiesInit();
	SceneInit();
	
	EngineInit();
	EngineInitEditor();

	LuaInit();
	ImGuiInit();
	FpsCameraInit();
	
	OpenVkRuntimeInfo("Engine was initilaized", "");
//	exit(2);
	//Set up deafult test scene
//	SceneMesh Mesh;
	
	/*
	uint32_t EntityIndex = AddEntity(COMPONENT_TYPE_MESH);
	uint32_t MeshIndex = AddPlane();
	SceneMesh* Mesh = (SceneMesh*)CMA_GetAt(&SceneMeshes, MeshIndex);
	Entities[EntityIndex].Mesh.MeshIndex = MeshIndex;
	Entities[EntityIndex].Scale = Vec3f(8.0);
	strcpy(Entities[EntityIndex].Mesh.Name, Mesh->Name);


	EntityIndex = AddEntity(COMPONENT_TYPE_MESH);
	MeshIndex = AddCube();
	Mesh = (SceneMesh*)CMA_GetAt(&SceneMeshes, MeshIndex);
	Entities[EntityIndex].Mesh.MeshIndex = MeshIndex;
	Entities[EntityIndex].Translate = Vec3(0.0, 2.0, 0.0);
	strcpy(Entities[EntityIndex].Mesh.Name, Mesh->Name);
	*/

//	uint32_t PlaneIndex = AddPlane();
//	uint32_t CubeIndex = AddBean();
//
//	SceneMesh* PlaneMesh = (SceneMesh*)CMA_GetAt(&SceneMeshes, PlaneIndex);
//	SceneMesh* CubeMesh = (SceneMesh*)CMA_GetAt(&SceneMeshes, CubeIndex);
//
//	int32_t Count = 8;
//
//	for (int32_t j = -Count; j < Count; j++)
//	{
//		for (int32_t i = -Count; i < Count; i++)
//		{
//			uint32_t EntityIndex = AddEntity(COMPONENT_TYPE_MESH);
//			Entities[EntityIndex].Mesh.MeshIndex = PlaneIndex;
//			Entities[EntityIndex].Scale = Vec3f(8.0);
//			Entities[EntityIndex].Translate = Vec3((float)j * 10.0, 0.0, (float)i * 10.0);
//			strcpy(Entities[EntityIndex].Mesh.Name, PlaneMesh->Name);
//
//
//			EntityIndex = AddEntity(COMPONENT_TYPE_MESH);
//		//	printf("%d\n", EntityIndex);			
//			Entities[EntityIndex].Mesh.MeshIndex = CubeIndex;
//			Entities[EntityIndex].Translate = Vec3((float)j * 10.0, 2.0, (float)i * 10.0);
//			strcpy(Entities[EntityIndex].Mesh.Name, CubeMesh->Name);
//		}
//
//	}
	
	int32_t RaageXZ = 100;
	int32_t RaageY = 150;
	
	for (uint32_t i = 0; i < MAX_NUMBER_OF_LIGHTS - 1; i++)
	{
		uint32_t EntityIndex = AddEntity(COMPONENT_TYPE_LIGHT);
		ResetEntityLight(&Entities[EntityIndex]);
		Entities[EntityIndex].Light.CastShadow = false;
		Entities[EntityIndex].Light.Type = 0;
		Entities[EntityIndex].Light.Strength = (float)RandomInt(80, 80000) / 10.0;
		Entities[EntityIndex].Light.Color = Vec3((float)RandomInt(1, 100) / 100.0, (float)RandomInt(1, 100) / 100.0, (float)RandomInt(1, 100) / 100.0);
		Entities[EntityIndex].Translate = Vec3((float)RandomInt(-RaageXZ, RaageXZ) / 10.0, (float)RandomInt(0, RaageY) / 10.0, (float)RandomInt(-RaageXZ, RaageXZ) / 10.0);
		
		strcpy(Entities[EntityIndex].Light.Name, "Point Light");
	//	OpenVkRuntimeInfo("Scene was initilaized", "");
	}

//	uint32_t EntityIndex = AddEntity(COMPONENT_TYPE_LIGHT);
//	ResetEntityLight(&Entities[EntityIndex]);
//	Entities[EntityIndex].Light.CastShadow = true;
//	Entities[EntityIndex].Light.Type = 1;
//	Entities[EntityIndex].Light.Strength = 5.8;
//	Entities[EntityIndex].Translate = Vec3(2.0, 16.5, 2.25);
//	strcpy(Entities[EntityIndex].Light.Name, "Dir Light");
//	OpenVkRuntimeInfo("Scene was initilaized", "");
//	
//	AddModel(0, "D:/3D Models/Buildings/ccity-building-set-1/source/City.obj");

	uint32_t ModelIndex= AddModel(0, "D:/3D Models/Sponza-master/Sponza2.obj");
	AddEntity(COMPONENT_TYPE_MESH);
	SceneMesh* Mesh = (SceneMesh*)CMA_GetAt(&SceneMeshes, ModelIndex);
	Entities[SelectedEntity].Mesh.MeshIndex = ModelIndex;
	strcpy(Entities[SelectedEntity].Mesh.Name, Mesh->Name);
	
}

void DestroyRenderer()
{
	ImGuiDestroy();
	SceneDestroy();
	LuaDestroy();
	EntitiesDestroy();

	EngineDestroy();
	EngineDestroyEditor();
	
	OpenVkGUIDestroy();
	OpenVkDestroyRenderer();
	OpenVkFreeThreads();
}

void RendererUpdate()
{
	//	if (!IsEditorActive)
	UpdateFpsCamera(&CameraPos, &CameraDir, &CameraUp);

	if (SDL_GetKeyboardState(NULL)[SDL_SCANCODE_C])
		CameraFOV = CameraZoomFOV;
	else
		CameraFOV = CameraNormalFOV;
	
//	if (RenderShadows || EffectFrame == 2)

	GBufferUpdateUniformBuffer();
	UpdateCascades();
	SSAOUpdateUniformBuffer();
	SceneUpdateUniformBuffer();
	SceneUpdateStorageBuffer();
	SSRUpdateUniformBuffer();
	
	/*
	Mutex.lock();
	mat4 ViewProj = MultiplyMat4P(&SceneVertexUBO.Projection, &SceneVertexUBO.View);
	Mutex.unlock();

	RunFrustumCulling(ViewProj, RENDER_TYPE_DEFAULT);
	for (uint32_t i = 0; i < SHADOW_MAP_CASCADE_COUNT; i++)
	{
		Mutex.lock();
		mat4 ViewProj = CullingCascades[i];
		Mutex.unlock();
		RunFrustumCulling(ViewProj, i + 1);
	}
	*/
//	SSRUpdateUniform();	
}



void RendererDraw()
{
	if (!RenderDebugDescriptorSet)
	{
		if (RenderFXAA)
			SceneRenderDescriptorSet = FXAADescriptorSet;
		else if (!RenderFXAA && RenderSSR)
			SceneRenderDescriptorSet = SSROutputDescriptorSet;
		else
			SceneRenderDescriptorSet = SceneOutputDescriptorSet;
	}	

	BeginFrameTime = GetExecutionTimeOpenVkBool(OpenVkBeginFrame);
	{
		if (ForceRenderOnce || RenderShadows)					ShadowRenderingTime = GetExecutionTime(ShadowDraw);
																GBufferRenderingTime = GetExecutionTime(GBufferDraw);
		if (ForceRenderOnce || RenderSSAO)						SSAORenderingTime = GetExecutionTime(SSAODraw);
		if (ForceRenderOnce || RenderSSAO && RenderSSAOBlur)	SSAOBlurRenderingTime = GetExecutionTime(SSAOBlurDraw);
							 									SceneRenderingTime = GetExecutionTime(SceneDraw);
		if (ForceRenderOnce || RenderSSR)						SSRRenderingTime = GetExecutionTime(SSRDraw);
		if (ForceRenderOnce || RenderFXAA)						FXAARenderingTime = GetExecutionTime(FXAADraw);
																SwapChainRenderingTime = GetExecutionTime(SwapChainDraw);
	}
	EndFrameTime = GetExecutionTimeOpenVkBool(OpenVkEndFrame);

	ForceRenderOnce = false;
}

void RendererResize()
{
	OpenVkRecreateSwapChain(&WindowWidth, &WindowHeight);

	CreateRenderPasses();
	CreateFramebuffers();
	OpenVkDestroyDescriptorPool(DescriptorPool);
	CreateDescriptors();
	ForceRenderOnce = true;
}

void RendererEvent()
{
	ImGuiEvent();

	if (ForceFullScreenEvent || (Event.type == SDL_KEYDOWN && Event.key.keysym.sym == SDLK_F11))
	{
		ForceFullScreenEvent = false;
		SDL_SetWindowFullscreen(Window, (FullScreen = !FullScreen) ? SDL_WINDOW_FULLSCREEN_DESKTOP : false);
	}

	if (Event.type == SDL_KEYDOWN && Event.key.keysym.sym == SDLK_F5)
	{
		RenderGamePreview = !RenderGamePreview;
		ForceResizeEvent = true;
	}

	if (Event.type == SDL_KEYDOWN && Event.key.keysym.sym == SDLK_F3)
		GamePerformanceOverlay = !GamePerformanceOverlay;

	if (Event.window.event == SDL_WINDOWEVENT_RESIZED || ForceResizeEvent)
	{
		ForceResizeEvent = false;

		const float Scale = ((float)SceneScaling / 100);

		if (RenderGamePreview)
		{
		//	SceneWidth = 128 * ((float)WindowWidth / (float)WindowHeight);
		//	SceneHeight = 128;
			SceneWidth = WindowWidth * Scale;
			SceneHeight = WindowHeight * Scale;
		}
		else
		{
			SceneWidth = ImGuiSceneWidth * Scale;
			SceneHeight = ImGuiSceneHeight * Scale;
		}
	//	printf("Event\n");
		RendererResize();
	}

	if (Event.type == SDL_KEYDOWN && Event.key.keysym.sym == SDLK_F12)
	{
		char Path[MAX_CHAR_PATH_LENGTH];
		if (WaveOpenFileDialog(Path, false, NULL, "All Audios\0*.wav\0"))
		{
			WaveAudio Audio = WaveLoadAudio(Path);
			WavePlayAudio(&Audio, 1);
			WaveResetToLastPath();
		}
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

void RendererRender()
{
	if (ImGuiTexturesToDelete.size() != 0)
	{
		OpenVkDeviceWaitIdle();

		for (size_t i = 0; i < ImGuiTexturesToDelete.size(); i++)
		{
			//should check if texture is vaild
			OpenVkFreeDescriptorSet(ImGuiDescriptorPool, ImGuiTexturesToDelete[i].DescriptorSet);
			OpenVkDestroyImage(ImGuiTexturesToDelete[i].Image);
			OpenVkDestroySampler(ImGuiTexturesToDelete[i].Sampler);
			
		}

		ImGuiTexturesToDelete.clear();
	}	

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

		RendererResize();
	}
	if (DeleteMesh || DeleteMeshWithTextures)
	{
		Mutex.lock();
		SceneMesh* Mesh = (SceneMesh*)CMA_GetAt(&SceneMeshes, MeshToDelete);
		if (Mesh != NULL)
		{
			if (DeleteMeshWithTextures)
			{
				for (uint32_t i = 0; i < Mesh->MeshCount; i++)
				{
					SceneTextureImage* Albedo = (SceneTextureImage*)CMA_GetAt(&SceneTextures, Mesh->MeshData[i].Material.AlbedoIndex);
					SceneTextureImage* Normal = (SceneTextureImage*)CMA_GetAt(&SceneTextures, Mesh->MeshData[i].Material.NormalIndex);
					SceneTextureImage* Metallic = (SceneTextureImage*)CMA_GetAt(&SceneTextures, Mesh->MeshData[i].Material.MetallicIndex);
					SceneTextureImage* Roughness = (SceneTextureImage*)CMA_GetAt(&SceneTextures, Mesh->MeshData[i].Material.RoughnessIndex);
					SceneTextureImage* Occlusion = (SceneTextureImage*)CMA_GetAt(&SceneTextures, Mesh->MeshData[i].Material.OcclusionIndex);

					DeleteMeshTexture(Albedo->TextureImage, Mesh->MeshData[i].Material.AlbedoIndex);
					DeleteMeshTexture(Albedo->TextureImage, Mesh->MeshData[i].Material.NormalIndex);
					DeleteMeshTexture(Albedo->TextureImage, Mesh->MeshData[i].Material.MetallicIndex);
					DeleteMeshTexture(Albedo->TextureImage, Mesh->MeshData[i].Material.RoughnessIndex);
					DeleteMeshTexture(Albedo->TextureImage, Mesh->MeshData[i].Material.OcclusionIndex);
				}

				if (Mesh->Destroyable)
				{
					if (Mesh->VertexBuffer != OPENVK_ERROR)
						OpenVkDestroyBuffer(Mesh->VertexBuffer);
					if (Mesh->IndexBuffer != OPENVK_ERROR)
						OpenVkDestroyBuffer(Mesh->IndexBuffer);
				}
			}
		}

		CMA_Pop(&SceneMeshes, MeshToDelete);

		for (uint32_t i = 1; i < SceneMeshes.Size; i++)
			if (CMA_GetAt(&SceneMeshes, i) != NULL)
				SelectedMesh = i;

		Mutex.unlock();
		
		DeleteMesh = false;
		DeleteMeshWithTextures = false;

		RendererResize();
	}
	if (ReloadShaders)
	{
		ReloadShaders = false;

		system("GLSLCompiler.bat");

		OpenVkDeviceWaitIdle();

		for (uint32_t i = 0; i < VkRenderer.PipelineCount; i++)
		{
		//	vkDestroyPipelineLayout(VkRenderer.Device, VkRenderer.PipelineLayouts[i], NULL);
		//	if (i != OpenVkGUI.Pipeline)
				vkDestroyPipeline(VkRenderer.Device, VkRenderer.Pipelines[i], NULL);
		}
		
		VkRenderer.PipelineCount = 0;
		
		OpenVkGUIRecreatePipeline();
		CreateGraphicsPipelines();
		RendererResize();
	}

	GetDeltaTime();
//	OpenVkDrawFrame(RendererDraw, RendererResize, RendererUpdate);
	FrameTime = GetExecutionTimeOpenVkRender(OpenVkDrawFrame, RendererDraw, RendererResize, RendererUpdate);
//	exit(22);
}