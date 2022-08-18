#include "Pipelines/DebugHelper.h"
#include "Pipelines/ShadowHelper.h"
#include "Pipelines/BlurHelper.h"
#include "Pipelines/SceneHelper.h"
#include "Pipelines/SwapChainHelper.h"

#include "Helper/Helper.h"
#include "Helper/Meshes.h"
#include "Helper/DescriptorSetLayouts.h"
#include "Helper/DescriptorPool.h"
#include "Helper/Buffers.h"
#include "Helper/Sampler.h"
#include "Helper/ImGuiHelper.h"

#include "Editor/Helper.h"
#include "Editor/Camera.h"
#include "Editor/SceneManager.h"
#include "Editor/AssetBrowser.h"
#include "Editor/EntityManager.h"
#include "Editor/Inspector.h"
#include "Editor/ScriptEngine.h"
#include "Editor/Editor.h"

#include "Pipelines/BlurPipeline.h"
#include "Pipelines/DebugPipeline.h"
#include "Pipelines/ShadowPipeline.h"
#include "Pipelines/ScenePipeline.h"
#include "Pipelines/SwapChainPipeline.h"

void CreateRenderPasses()
{
	CreateBlurRenderPass();
	CreateShadowRenderPass();
	CreateSceneRenderPass();
	CreateSwapChainRenderPass();
}

void CreateGraphicsPipelines()
{
	CreateBlurPipeline();
	CreateDebugPipeline();
	CreateShadowPipeline();
	CreateScenePipeline();
	CreateSwapChainPipeline();
}

void CreateFramebuffers()
{
	CreateBlurFramebuffers();
	CreateShadowFramebuffers();
	CreateSceneFramebuffer();
	CreateSwapChainFramebuffer();
}

void CreateDescriptors()
{
	CreateDescriptorPool();
	CreateBlurDescriptorSets();
	CreateSceneDescriptorSets();
}

void CreateRenderer()
{
	SwapChain = OpenVkCreateRenderer(true, GetExtensions, GetSurface, GetWindowSize);

	CreateRenderPasses();
	CreateDescriptorSetLayout();
	CreateGraphicsPipelines();
	CreateFramebuffers();

	CreateImageSampler();
	CreateBuffers();

	CreateSceneUniformBuffer();

	CreateImGuiDescriptorPool();
	CreateDescriptors();

	InitScene();
	InitImGui();
	InitFpsCamera();
	EngineInitEditor();

	SceneFragmentUBO.LightDirection = Vec4(0.426, -0.876, -0.225, 0.0);
}

void DestroyRenderer()
{
	DestroyImGui();
	DestroyScene();
	EngineDestroyEditor();
	DestroyBuffers();
	OpenVkDestroyRenderer();
}

void RendererUpdate()
{
	//	if (!IsEditorActive)
	UpdateFpsCamera(&CameraPos, &CameraDir, &CameraUp);

	SceneFragmentUBO.CameraPosition.x = CameraPos.x;
	SceneFragmentUBO.CameraPosition.y = CameraPos.y;
	SceneFragmentUBO.CameraPosition.z = CameraPos.z;
	SceneFragmentUBO.CameraPosition.w = 0.0;
	Normalize4P(&SceneFragmentUBO.LightDirection);

	UpdateCascades();
	SceneUpdateUniformBuffer();
}

void RendererDraw()
{
	OpenVkBegineFrame();
	{
		ShadowRenderingTime = GetExecutionTime(ShadowDraw);
		SceneRenderingTime = GetExecutionTime(SceneDraw);
		SwapChainRenderingTime = GetExecutionTime(SwapChainDraw);
	}
	OpenVkEndFrame();
}

void RendererResize()
{
	OpenVkRecreateSwapChain(&WindowWidth, &WindowHeight);

	CreateRenderPasses();
	CreateFramebuffers();
	CreateDescriptors();
}

void RendererEvent()
{
	ImGuiEvent();

	if (Event.window.event == SDL_WINDOWEVENT_RESIZED || ForceResizeEvent)
	{
		ForceResizeEvent = false;

		SceneWidth = ImGuiSceneWidth;
		SceneHeight = ImGuiSceneHeight;
		RendererResize();
	}

	if (Event.type == SDL_KEYDOWN && Event.key.keysym.sym == SDLK_F11)
		SDL_SetWindowFullscreen(Window, (FullScreen = !FullScreen) ? SDL_WINDOW_FULLSCREEN_DESKTOP : false);

	if (Event.type == SDL_KEYDOWN && Event.key.keysym.sym == SDLK_F12)
	{
		char Path[MAX_CHAR_PATH_LENGTH];
		if (WaveOpenFileDialog(Path, "All Audios\0*.wav\0"))
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
			OpenVkDestroyTextureImage(Image->TextureImage);
			OpenVkDestroySampler(Image->TextureSampler);
			CMA_Pop(&SceneTextures, TextureIndex);
		}
	}
}

void RendererRender()
{
	if (DeleteTexture)
	{
		DeleteTexture = false;

		OpenVkDestroyTextureImage(TextureToDelete);
		OpenVkDestroySampler(SamplerToDelete);

		RendererResize();
	}
	if (DeleteMesh)
	{
		DeleteMesh = false;

		SceneMesh* Mesh = (SceneMesh*)CMA_GetAt(&SceneMeshes, MeshToDelete);
		if (Mesh != NULL && Mesh->Freeable)
		{
			for (uint32_t i = 0; i < Mesh->MeshCount; i++)
			{
				SceneTextureImage* Albedo = (SceneTextureImage*)CMA_GetAt(&SceneTextures, Mesh->MeshData[i].AlbedoIndex);
				SceneTextureImage* Normal = (SceneTextureImage*)CMA_GetAt(&SceneTextures, Mesh->MeshData[i].NormalIndex);
				SceneTextureImage* Metallic = (SceneTextureImage*)CMA_GetAt(&SceneTextures, Mesh->MeshData[i].MetallicIndex);
				SceneTextureImage* Roughness = (SceneTextureImage*)CMA_GetAt(&SceneTextures, Mesh->MeshData[i].RoughnessIndex);
				SceneTextureImage* Occlusion = (SceneTextureImage*)CMA_GetAt(&SceneTextures, Mesh->MeshData[i].OcclusionIndex);

				DeleteMeshTexture(Albedo->TextureImage, Mesh->MeshData[i].AlbedoIndex);
				DeleteMeshTexture(Albedo->TextureImage, Mesh->MeshData[i].NormalIndex);
				DeleteMeshTexture(Albedo->TextureImage, Mesh->MeshData[i].MetallicIndex);
				DeleteMeshTexture(Albedo->TextureImage, Mesh->MeshData[i].RoughnessIndex);
				DeleteMeshTexture(Albedo->TextureImage, Mesh->MeshData[i].OcclusionIndex);

				if (Mesh->MeshData[i].Vertices != NULL)
					OpenVkDestroyBuffer(Mesh->MeshData[i].VertexBuffer);
				if (Mesh->MeshData[i].Indices != NULL)
					OpenVkDestroyBuffer(Mesh->MeshData[i].IndexBuffer);
			}
		}

		CMA_Pop(&SceneMeshes, MeshToDelete);

		for (uint32_t i = 1; i < SceneMeshes.Size; i++)
			if (CMA_GetAt(&SceneMeshes, i) != NULL)
				SelectedMesh = i;

		RendererResize();
	}
	if (ReloadShaders)
	{
		ReloadShaders = false;

		system("GLSLCompiler.bat");

		vkDeviceWaitIdle(VkRenderer.Device);

		for (uint32_t i = 0; i < VkRenderer.PipelineCount; i++)
		{
			vkDestroyPipelineLayout(VkRenderer.Device, VkRenderer.PipelineLayouts[i], NULL);
			vkDestroyPipeline(VkRenderer.Device, VkRenderer.Pipelines[i], NULL);
		}

		VkRenderer.PipelineCount = 0;

		CreateGraphicsPipelines();
		RendererResize();
	}

	GetDeltaTime();
	OpenVkDrawFrame(RendererDraw, RendererResize, RendererUpdate);
}