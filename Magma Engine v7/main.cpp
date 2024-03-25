#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdarg.h>
#include <time.h>
#include <string.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_vulkan.h>
#include <vulkan/vulkan.h>
#include <thread>
#include <future>
#include <mutex>
#include <chrono>
#include <condition_variable>

extern "C"
{
#include <lua/lua.h>
#include <lua/lauxlib.h>
#include <lua/lualib.h>
}

//#include <stb/stb_sprintf.h>
#include <stb/dxt/stb_dxt.c>
#include <stb/stb_image_resize.h>

#include <ImGui/imconfig.h>
#include <ImGui/imgui_tables.cpp>
#include <ImGui/imgui_internal.h>
#include <ImGui/imgui.cpp>
#include <ImGui/imgui_draw.cpp>
#include <ImGui/imgui_widgets.cpp>
#include <ImGui/imgui_demo.cpp>
#include <ImGui/imgui_impl_sdl.cpp>
#include <ImGui/imgui_impl_vulkan_but_better.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <soloud/soloud.h>
#include <soloud/soloud_wav.h>

#include <MojoAL/mojoal.c>

#include <Wave/WaveMath.h>
#include <Wave/WaveMesh.h>
#include <Wave/WaveLoader.h>
#include <Wave/WaveAudio.h>
#include <Wave/WaveLayer.h>
#include <Wave/WavePhysics.h>

#include "Md2Loader.h"

#include <OpenVK/OpenVK.h>
#include "Helper.h"

#include <ImGui/FileDialog/ImFileDialog.cpp>

#include "Renderer/Renderer.h"

static std::atomic<bool> Run = true;
static std::atomic<bool> Init = false;

void RenderThread()
{
Restart:
#ifdef _WIN32
	system("GLSLCompiler.bat");
#endif
#ifdef __linux__
	system("clear");
	system("./GLSLCompiler.sh");
#endif
	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_EVENTS | SDL_INIT_TIMER);
	Window = SDL_CreateWindow("Magma Engine v7 ImGui " IMGUI_VERSION, 0, 0, WindowWidth, WindowHeight, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_INPUT_FOCUS | SDL_WINDOW_VULKAN | SDL_WINDOW_BORDERLESS);
	SDL_SetWindowMinimumSize(Window, 800, 540);
	SDL_GetWindowSize(Window, (int*)&WindowWidth, (int*)&WindowHeight);
	SDL_SetWindowPosition(Window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
#ifdef _WIN32
	SDL_SysWMinfo WmInfo;
	SDL_VERSION(&WmInfo.version);
	SDL_GetWindowWMInfo(Window, &WmInfo);
	WaveHwnd = WmInfo.info.win.window;
#endif	

	if (RenderGamePreview)
	{
		SceneWidth = WindowWidth;
		SceneHeight = WindowHeight;
	}
	
	CreateRenderer();
	Init = true;

	while (Run)
	{
		if (RestartEngine)
			Run = false;

		while (SDL_PollEvent(&Event))
		{
			RendererEvent();

			if (Event.type == SDL_QUIT)
				Run = false;
		}
		
		RendererRender();
	//	exit(3666);
		FrameCount++;
	}

	DestroyRenderer();
	SDL_DestroyWindow(Window);
	SDL_Quit();
	printf("%f\n", WaveGetUsedMemory() * 0.000001);

	if (RestartEngine)
	{
		RestartEngine = false;
		Run = true;
		goto Restart;
	}
}

void CullingThread()
{
	auto lastTime = std::chrono::steady_clock::now();
	double frameTime = 1.0 / 60.0;

	while (Run)
	{
		if (Init)
		{
			

			auto currentTime = std::chrono::steady_clock::now();
			auto deltaTime = std::chrono::duration_cast<std::chrono::duration<double>>(currentTime - lastTime).count();

			if (deltaTime >= frameTime) {
				Mutex.lock();
				mat4 ViewProj = MultiplyMat4P(&GBufferVertexUBO.Projection, &GBufferVertexUBO.View);
				Mutex.unlock();

				RunFrustumCulling(ViewProj, RENDER_TYPE_DEFAULT);
				for (uint32_t i = 0; i < SHADOW_MAP_CASCADE_COUNT; i++)
				{
					Mutex.lock();
					mat4 ViewProj = CullingCascades[i];
					Mutex.unlock();
					RunFrustumCulling(ViewProj, i + 1);
				}
				lastTime = currentTime;
			}

			// sleep for a short duration to avoid using too much CPU
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
		}
		
	}
//	while (Run)
//		RunFrustumCulling();
}

int32_t main(int32_t argc, char** argv)
{
	/*
	int32_t Width = 0;;
	int32_t Height = 0;
	unsigned char* Pixels = NULL;

	if (OpenVkLoadTexture("Data/Textures/UvGrid.png", false, &Pixels, &Width, &Height, OPENVK_FORMAT_RGBA) == OpenVkTrue)
	{
		printf("Loaded\n");

		int32_t Size = Width * Height;

		void* Block = malloc(Size);
		printf("%d\n", Size);
		printf("%d\n", Width * Height * 4);

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
		printf("%d\n", i);
	}


	

	return 0;
	*/
//	char msg[] = "All Images\0*.png;*.jpg;*.tga;*.jpeg;*.hdr;*.bmp\0";
//	LPWSTR msgwide = WaveToLPWSTR(msg);
//	WCHAR msgwide[] = L"All Images\0*.png;*.jpg;*.tga;*.jpeg;*.hdr;*.bmp\0";
//	wprintf(msgwide);
//	char* msgsmall = WaveToChar(msgwide);
//	printf(msgsmall);
//	return 0;
//	printf("%.3f\n", 1.23342124543f);
//	//C:/Users/Moritz Desktop/source/repos/Vulkan C99/Vulkan C99/Cube.dae
//	WaveLoadModel("C:/Users/Moritz Desktop/source/repos/Vulkan C99/Vulkan C99/Cube.dae", 0);
		//	uint32_t BeginTime = SDL_GetTicks();
		////	WaveSetPath((char*)"D:/3D Models/GLTF/");
		//	WaveModelData Data = WaveLoadModel("D:/3D Models/GLTF/CubeSep.gltf", WAVE_LOAD_MATERIAL | WAVE_GEN_NORMALS | WAVE_FLIP_UVS | WAVE_GEN_UVS | WAVE_GEN_INDICES | WAVE_REMOVE_REDUNDANT_MATERIALS | WAVE_MATERIAL_USE_MODEL_PATH | WAVE_PRINT_DEBUG_INOFS);
		//	//	WaveModelData Data = WaveLoadModel("C:/Users/Moritz Desktop/source/repos/Vulkan C99/Vulkan C99/vulkan.obj", 0);
		//	uint32_t EndTime = SDL_GetTicks();
		//	printf("Loading time: %d\n", EndTime - BeginTime);
		//	exit(0);
		//
		//	return 22;
	
//	uint32_t BeginTime = SDL_GetTicks();
//	WaveModelData Data = WaveLoadModel("C:/Users/Moritz Desktop/source/repos/Vulkan C99/Vulkan C99/cessna.stl", 0);
//	uint32_t EndTime = SDL_GetTicks();
//	printf("Loading time: %d\n", EndTime - BeginTime);
//	printf("Vertices: %d\n", Data.VertexCount);
//	
//		return 22;
	
//	char* s1 = (char*)"1/2/3";  
//	char* s2 = WaveStrtok(s1, '/');
//	char* s3 = WaveStrtok(s2, '/');
//
//	printf("%s\n%s\n%s\n", s1, s2, s3);
//	return 0;
	
//	std::future Rendering = std::async(std::launch::async, RenderThread);
//	std::future Culling = std::async(std::launch::async, CullingThread);
//
//	Rendering.wait();
//	Culling.wait();
//	std::thread t1(RenderThread);
//	std::thread t2(CullingThread);

	RenderThread();
//	t1.join();
//	t2.join();

	return 0;
}
