#include "Pipelines/DebugHelper.h"
#include "Pipelines/ShadowHelper.h"
#include "Pipelines/BlurHelper.h"
#include "Pipelines/GBufferHelper.h"
#include "Pipelines/SSAOHelper.h"
#include "Pipelines/SSAOBlurHelper.h"
#include "Pipelines/SSRHelper.h"
#include "Pipelines/SceneHelper.h"
#include "Pipelines/FXAAHelper.h"
#include "Pipelines/ForwardHelper.h"
#include "Pipelines/SwapChainHelper.h"

#include "Helper/RendererHelper.h"
#include "Helper/Meshes.h"
#include "Helper/DescriptorSetLayouts.h"
#include "Helper/DescriptorPool.h"
#include "Helper/Buffers.h"
#include "Helper/Sampler.h"
#include "Helper/GUI.h"

#include "Raytracing/RaytracingHelper.h"
#include "PassBuilder/PassBuilder.h"

#include "../Engine/Engine.h"
#include "../Engine/CameraPath.h"
#include "../Engine/Physics.h"
#include "../Engine/ImageCompression.h"
#include "../Engine/ECS.h"
#include "../Engine/FpsCamera.h"
#include "../Engine/Scene.h"
#include "../Engine/ResourceManager.h"
#ifdef LINUX_PORT
#include "../Engine/ScriptEngine.h"
#endif
#include "../Engine/FrustumCulling.h"
#include "../Engine/FileDialog.h"

#include "../Editor/Editor.h"
#include "../Editor/SceneManager.h"
#include "../Editor/AssetBrowser.h"
#include "../Editor/EntityManager.h"
#include "../Editor/Inspector.h"
#include "../Editor/EditorUI.h"

#include "Raytracing/Raytracing.h"

#include "Pipelines/BlurPipeline.h"
#include "Pipelines/DebugPipeline.h"
#include "Pipelines/ShadowPipeline.h"
#include "Pipelines/GBufferPipeline.h"
#include "Pipelines/SSAOPipeline.h"
#include "Pipelines/SSAOBlurPipeline.h"
#include "Pipelines/ScenePipeline.h"
#include "Pipelines/SSRPipeline.h"
#include "Pipelines/FXAAPipeline.h"
#include "Pipelines/ForwardPipeline.h"
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
	CreateForwardRenderPass();
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
	CreateForwardLayout();
	CreateSwapChainLayout();
	
}

void CreateGraphicsPipelines()
{
	double Time = GetExecutionTime(ShaderCompilerWait);
	printf("Shader Compiler Time: %f\n", Time);

	CreateBlurPipeline();
	CreateShadowPipeline();	
	CreateGBufferPipeline();
	CreateSSAOPipeline();
	CreateSSAOBlurPipeline();
	CreateScenePipeline();
	CreateDebugPipeline();	
	CreateSSRPipeline();
	CreateFXAAPipeline();
	CreateForwardPipeline();
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
	CreateForwardFramebuffer();
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
	CreateForwardDescriptorSet();
}

void RendererCreate()
{
	SwapChain = OpenVkCreateRenderer(OPENVK_VULKAN | OPENVK_VALIDATION_LAYER, GetExtensions, GetSurface, GetWindowSize);
	
	if (OpenVkHasRaytracingSupport())
	{
		RaytracingInit();
		OpenVkRuntimeInfo("Raytracing was initilaized", "");
	}

	CreateRenderPasses();
	/*
	* 80, "C:/Windows/Fonts/RAGE.TTF"
	* 30, "Data/Fonts/Roboto-Medium.TTF"
	*/
	OpenVkGUIInit(MainHWnd.Width, MainHWnd.Height, SwapChainRenderPass, 1, 30, "Data/Fonts/Roboto-Medium.ttf", GetMousePos);
	
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
	CreateForwardUniformBuffer();
	
	CreateImGuiDescriptorPool();
	
	CreateDescriptors();

	FileDialogInit();
	EntitiesInit();
	SceneInit();
	
	RenderWorkflowInit();

	EngineInit();
	EngineInitEditor();
	
	#ifdef LINUX_PORT
	LuaInit();
	#endif
	ImGuiInit();
	FpsCameraInit();
/*
	{
	//	system("gcc -fPIC -shared -o Sample.so Sample.c");
		SDL_SharedObject* myHandle = NULL;
		const char* myFunctionName = "MagmaLoad_AddEntity";
		typedef void (*dll_function_t)(uint32_t (*callback)(uint32_t));

		
		myHandle = SDL_LoadObject("SampleProject/Sample.so");

		
		dll_function_t dll_function = (dll_function_t)SDL_LoadFunction(myHandle, myFunctionName);
		SDL_FunctionPointer dll_Start = (SDL_FunctionPointer)SDL_LoadFunction(myHandle, "Start");

		
		if (dll_function != NULL)
			dll_function(AddDefaultEntity);
		else
			printf("Fuck\n");

		if (dll_Start != NULL)
			dll_Start();
		else
			printf("Fuck2\n");

	}
*/
	OpenVkRuntimeInfo("Engine was initilaized", "");

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
	/*
	int32_t RaageXZ = 100;
	int32_t RaageY = 150;
	
	for (uint32_t i = 0; i < 10; i++)//MAX_NUMBER_OF_LIGHTS - 1
	{
		
		uint32_t EntityIndex = AddDefaultEntity(COMPONENT_TYPE_LIGHT);
		ResetEntityLight(&Entities[EntityIndex]);
		Entities[EntityIndex].Light.CastShadow = false;
		Entities[EntityIndex].Light.Type = LIGHT_TYPE_POINT;
		Entities[EntityIndex].Light.Strength = (float)RandomInt(80, 80000) / 10.0;
		Entities[EntityIndex].Light.Color = Vec3((float)RandomInt(1, 100) / 100.0, (float)RandomInt(1, 100) / 100.0, (float)RandomInt(1, 100) / 100.0);
		Entities[EntityIndex].Translate = Vec3((float)RandomInt(-RaageXZ, RaageXZ) / 10.0, (float)RandomInt(0, RaageY) / 10.0, (float)RandomInt(-RaageXZ, RaageXZ) / 10.0);
		
		
	//	OpenVkRuntimeInfo("Scene was initilaized", "");
	}
	*/
	/*
//	uint32_t ModelIndex = AddModel(0, "D:/3D Models/Buildings/ccity-building-set-1/source/City.obj");
//	uint32_t ModelIndex = AddModel(0, "C:/Users/Moritz Laptop/Downloads/Sponza-master/sponza2.obj");
//	uint32_t ModelIndex = AddModel(0, "C:/Users/Moritz Laptop/Downloads/TestMesh.obj");
	uint32_t ModelIndex = AddModel(0, "D:/3D Models/Sponza-master/Sponza2.obj");
		AddDefaultEntity(COMPONENT_TYPE_MESH);
		AddMeshToEntity(SelectedEntity, ModelIndex);
		
	//	ModelIndex = AddModel(0, "C:/Users/Moritz Laptop/Downloads/TestMesh.obj");
		ModelIndex = AddModel(0, "D:/3D Models/Teapot.obj");
		uint32_t EntityIndex = AddDefaultEntity(COMPONENT_TYPE_MESH);
		Entities[EntityIndex].Translate = Vec3(0.0, 0.0, 25.0);
		AddMeshToEntity(SelectedEntity, ModelIndex);
		
		EntityIndex = AddDefaultEntity(COMPONENT_TYPE_LIGHT);
		ResetEntityLight(&Entities[EntityIndex]);
		Entities[EntityIndex].Light.CastShadow = true;
		Entities[EntityIndex].Light.Type = LIGHT_DIRECTIONAL;
		Entities[EntityIndex].Light.Strength = 5.8;
		Entities[EntityIndex].Translate = Vec3(-3.6, 13.5, 2.75);
		strcpy(Entities[EntityIndex].Light.Name, "Dir Light");
		*/
	OpenVkRuntimeInfo("Scene was initilaized", "");

	
}

void RendererDestroy()
{
	ImGuiDestroy();
	SceneDestroy();
	#ifdef LINUX_PORT
	LuaDestroy();
	#endif
	EntitiesDestroy();

	EngineDestroy();
	EngineDestroyEditor();
	CameraDestroyPath();
	RaytracingDestroy();
	FileDialogDestroy();
	RenderWorkflowDestroy();

	OpenVkGUIDestroy();
	OpenVkDestroyRenderer();
}

void RendererUpdate()
{
	if (IsEditorActive)
	{
		UpdateFpsCamera(&CameraPos, &CameraDir, &CameraUp);
		CameraUpdateRecordingPath();

		if (SDL_GetKeyboardState(NULL)[SDL_SCANCODE_C])
			CameraFOV = CameraZoomFOV;
		else
			CameraFOV = CameraNormalFOV;
	}

	GBufferUpdateUniformBuffer();
	UpdateCascades();
	SSAOUpdateUniformBuffer();
	SceneUpdateUniformBuffer();
	SceneUpdateStorageBuffer();
	SSRUpdateUniformBuffer();
	ForwardUpdateUniformBuffer();
	RaytracingUpdate();
}

void RendererDraw()
{
	BeginFrameTime = GetExecutionTimeOpenVkBool(OpenVkBeginFrame);
	{
		if (RenderMode != RENDER_MODE_RAYTRACED)
			if (ForceRenderOnce || RenderShadows)					
				ShadowRenderingTime = GetExecutionTime(ShadowDraw);

		if (RenderMode == RENDER_MODE_FORWARD)
		{
			//Forward
			if (RenderFXAA)
				SceneRenderDescriptorSet = FXAADescriptorSet;
			else
				SceneRenderDescriptorSet = SceneOutputDescriptorSet;

		//	if (ForceRenderOnce || RenderShadows) ShadowRenderingTime = GetExecutionTime(ShadowDraw);
			ForwardDraw();
			
		}
		else if (RenderMode == RENDER_MODE_DEFERRED)
		{
			//Deferred Rendering
			if (!RenderDebugDescriptorSet)
			{
				if (RenderFXAA)
					SceneRenderDescriptorSet = FXAADescriptorSet;
				else if (!RenderFXAA && RenderSSR)
					SceneRenderDescriptorSet = SSROutputDescriptorSet;
				else
					SceneRenderDescriptorSet = SceneOutputDescriptorSet;
			}

			
			GBufferRenderingTime = GetExecutionTime(GBufferDraw);
			if (ForceRenderOnce || RenderSSAO)						SSAORenderingTime = GetExecutionTime(SSAODraw);
			if (ForceRenderOnce || RenderSSAO && RenderSSAOBlur)	SSAOBlurRenderingTime = GetExecutionTime(SSAOBlurDraw);
			SceneRenderingTime = GetExecutionTime(SceneDraw);
			if (ForceRenderOnce || RenderSSR)						SSRRenderingTime = GetExecutionTime(SSRDraw);
		}
		else if (RenderMode == RENDER_MODE_RAYTRACED)
		{
			if (RenderFXAA)
				SceneRenderDescriptorSet = FXAADescriptorSet;
			else
				SceneRenderDescriptorSet = SceneOutputDescriptorSet;

			RaytracingDraw();
		}
		

		if (ForceRenderOnce || RenderFXAA)						FXAARenderingTime = GetExecutionTime(FXAADraw);
		
		SwapChainRenderingTime = GetExecutionTime(SwapChainDraw);
		
	}
	EndFrameTime = GetExecutionTimeOpenVkBool(OpenVkEndFrame);

	ForceRenderOnce = false;
}

void RendererResize(OpenVkBool RecreateSwapChain)
{
	if (RecreateSwapChain)
		OpenVkRecreateSwapChain(&MainHWnd.Width, &MainHWnd.Height);
	else
		OpenVkDestroySwapChainRelatives();

	printf("Width: %d, Height: %d\n", MainHWnd.Width, MainHWnd.Height);

	CreateRenderPasses();
	CreateFramebuffers();
	OpenVkDestroyDescriptorPool(DescriptorPool);
	CreateDescriptors();

	{
		for (size_t i = 0; i < SceneMaterials.Size; i++)
		{
			SceneMaterial* Material = (SceneMaterial*)CMA_GetAt(&SceneMaterials, i);
			if (Material != NULL)
			{
				
				Material->DescriptorSet = UpdatePBRTextureDescriptorSet(OPENVK_ERROR, Material->AlbedoIndex, Material->NormalIndex, Material->MetallicIndex, Material->RoughnessIndex, Material->OcclusionIndex);
			}
		}
	}
	
	RaytracingResize();
	ForceRenderOnce = true;
}

void RendererEvent()
{
	ImGuiEvent();

	if (MainHWnd.Event.type == SDL_EVENT_KEY_DOWN && MainHWnd.Event.key.key == SDLK_F1)
	{
		OpenVkDeviceWaitIdle();
		RenderMode++;

		if (RenderMode >= RENDER_MODE_COUNT ||
			(RenderMode == RENDER_MODE_RAYTRACED && !OpenVkHasRaytracingSupport()))
			RenderMode = 0;
	}
		
	if (MainHWnd.Event.type == SDL_EVENT_KEY_DOWN && MainHWnd.Event.key.key == SDLK_KP_0)
		StartStopGameMode();

	if (ForceFullScreenEvent || (MainHWnd.Event.type == SDL_EVENT_KEY_DOWN && MainHWnd.Event.key.key == SDLK_F11))
	{
		ForceFullScreenEvent = false;
		SDL_SetWindowFullscreen(MainHWnd.Wnd, (FullScreen = !FullScreen) ? SDL_WINDOW_FULLSCREEN : false);
	}

	if (MainHWnd.Event.type == SDL_EVENT_KEY_DOWN && MainHWnd.Event.key.key == SDLK_F5)
	{
		RenderGamePreview = !RenderGamePreview;
		ForceResizeEvent = true;
	}

	if (MainHWnd.Event.type == SDL_EVENT_KEY_DOWN && MainHWnd.Event.key.key == SDLK_F3)
		GamePerformanceOverlay = !GamePerformanceOverlay;

	if (MainHWnd.Event.type == SDL_EVENT_WINDOW_RESIZED || ForceResizeEvent)
	{
		ForceResizeEvent = false;

		const float Scale = ((float)SceneScaling / 100);

		if (RenderGamePreview)
		{
		//	SceneWidth = 128 * ((float)MainHWnd.Width / (float)MainHWnd.Height);
		//	SceneHeight = 128;
			SceneWidth = MainHWnd.Width * Scale;
			SceneHeight = MainHWnd.Height * Scale;
		}
		else
		{
			SceneWidth = ImGuiSceneWidth * Scale;
			SceneHeight = ImGuiSceneHeight * Scale;
		}
	//	printf("Event\n");
		RendererResize((MainHWnd.Event.type == SDL_EVENT_WINDOW_RESIZED));
	}

	if (MainHWnd.Event.type == SDL_EVENT_KEY_DOWN && MainHWnd.Event.key.key == SDLK_F12)
	{
		OpenVkDeviceWaitIdle();

		char Path[MAX_CHAR_PATH_LENGTH];
		if (WaveOpenFileDialog(Path, false, NULL, "All Audios\0*.wav\0"))
		{
			printf("Audio: %s\n", Path);
		//	WaveAudio Audio = WaveLoadAudio(Path);
		//	WavePlayAudio(&Audio, 1);
		//	WaveResetToLastPath();
			
		}
	}
}



void RendererRun()
{
	#ifdef LINUX_PORT
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
	#endif

	CheckForTextureDeletion();
	CheckForModelDeletion();
	CheckForShaderReloading();
	CheckForTextureUpdates();

	GetDeltaTime();
	
	if (OpenVkHasRaytracingSupport())
	{
		RaytracingRestBuild();
		RaytracingFillBuild();
		RaytracingBuild();
	}
	
	
	//	OpenVkDrawFrame(RendererDraw, RendererResize, RendererUpdate);
	FrameTime = GetExecutionTimeOpenVkRender(OpenVkDrawFrame, RendererDraw, RendererResize, RendererUpdate);
}