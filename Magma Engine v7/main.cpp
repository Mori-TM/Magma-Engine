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

#define MAGMA_ENGINE_TRACK_MEMORY
#define CMA_STORE_DEBUG_NAME_IN_RAM
#define OPENVK_STORE_DEBUG_NAME_IN_RAM

#ifdef MAGMA_ENGINE_TRACK_MEMORY
#include <MallocSucks/MallocSucks.h>
#define malloc s_malloc
#define calloc s_calloc
#define realloc s_realloc
#define free s_free
#endif

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
#include <ImGui/imgui_impl_sdl2.cpp>
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

#include <Json/Json.h>
#include <Base64/Base64.h>
#include <Md2Loader/Md2Loader.h>
#include <OpenVK/OpenVK.h>

#include "Helper.h"

#include <ImGui/FileDialog/ImFileDialog.cpp>

#include "Renderer/Renderer.h"

int32_t main(int32_t argc, char** argv)
{
Restart:
#ifdef MAGMA_ENGINE_TRACK_MEMORY
	s_init();
#endif
	/*
	DynamicArray Arr = DynamicArrayCreate(sizeof(SceneTextureImage), "Texture Images");

	for (uint32_t i = 0; i < 3245; i++)
	{
		SceneTextureImage Data;
		Data.Format = i;

		DynamicArrayPush(&Data, &Arr);
	}

	for (uint32_t i = 0; i < 453; i++)
	{
		DynamicArrayPop(0, &Arr);
	}

	DynamicArrayDestroy(&Arr);

	return 0;
	*/

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

	RendererCreate();
	bool Run = true;

//	SceneLoad("ExampleScene.lva");

//	Run = false;
//	exit(3666);

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

		RendererRun();
		//	exit(3666);
		FrameCount++;
	}
	
	SceneSave("ExampleScene.lva");

	RendererDestroy();
	SDL_DestroyWindow(Window);
	SDL_Quit();
	printf("%f\n", WaveGetUsedMemory() * 0.000001);

#ifdef MAGMA_ENGINE_TRACK_MEMORY
	s_checkForLeaks();
	s_destroy();
#endif

	if (RestartEngine)
	{
		RestartEngine = false;
		Run = true;
		goto Restart;
	}

	return 0;
}
