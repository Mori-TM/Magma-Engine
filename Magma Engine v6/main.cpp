#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_vulkan.h>
#include <vulkan/vulkan.h>

extern "C"
{
#include <lua/lua.h>
#include <lua/lauxlib.h>
#include <lua/lualib.h>
}

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

#include <Wave/WaveMath.h>
#include <Wave/WaveMesh.h>
#include <Wave/WaveLoader.h>
#include <Wave/WaveAudio.h>
#include <Wave/WaveLayer.h>
#include <Wave/WavePhysics.h>

#include <OpenVK/OpenVK.h>

#include "Md2Loader.h"


#include "Helper.h"
#include "Renderer/Renderer.h"

int32_t main(int32_t argc, char** argv)
{
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
//	WaveSetPath((char*)"D:/3D Models/Buildings/ccity-building-set-1/source/");
//	WaveModelData Data = WaveLoadModel("D:/3D Models/Buildings/ccity-building-set-1/source/City.obj", WAVE_LOAD_MATERIAL | WAVE_GEN_NORMALS | WAVE_FLIP_UVS | WAVE_GEN_UVS | WAVE_GEN_INDICES | WAVE_PRINT_DEBUG_INOFS);
//	//	WaveModelData Data = WaveLoadModel("C:/Users/Moritz Desktop/source/repos/Vulkan C99/Vulkan C99/vulkan.obj", 0);
//	uint32_t EndTime = SDL_GetTicks();
//	printf("Loading time: %d\n", EndTime - BeginTime);
//	exit(0);
//	printf("Vertices: %d\n", Data.VertexCount);
//	//	printf("Sorted vertices: %d\n", Data.SortedVertexCount);
//	printf("Indices: %d\n", Data.IndexCount);
//	printf("Tirangles: %d\n", Data.IndexCount / 3);
//	printf("Removed vertices: %d\n", Data.IndexCount - Data.VertexCount);
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
	Window = SDL_CreateWindow("Magma Engine v6 ImGui " IMGUI_VERSION, 0, 30, WindowWidth, WindowHeight, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_VULKAN | SDL_WINDOW_MAXIMIZED);
	SDL_SetWindowMinimumSize(Window, 800, 540);

#ifdef _WIN32
	SDL_SysWMinfo WmInfo;
	SDL_VERSION(&WmInfo.version);
	SDL_GetWindowWMInfo(Window, &WmInfo);
	WaveHwnd = WmInfo.info.win.window;
#endif	
	
	CreateRenderer();

	while (1)
	{
		while (SDL_PollEvent(&Event))
		{
			RendererEvent();

			if (Event.type == SDL_QUIT)
			{
				DestroyRenderer();
				SDL_DestroyWindow(Window);
				SDL_Quit();
				printf("%f\n", WaveGetUsedMemory() * 0.000001);
				return 1;
			}
		}			

		RendererRender();
	}

	DestroyRenderer();
	SDL_DestroyWindow(Window);
	SDL_Quit();

	return 0;
}
