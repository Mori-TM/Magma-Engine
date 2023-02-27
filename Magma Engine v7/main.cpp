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

#define IMGUI_DEFINE_MATH_OPERATORS
#include <ImGui/imconfig.h>
#include <ImGui/imgui_tables.cpp>
#include <ImGui/imgui_internal.h>
#include <ImGui/imgui.cpp>
#include <ImGui/imgui_draw.cpp>
#include <ImGui/imgui_widgets.cpp>
#include <ImGui/imgui_demo.cpp>
#include <ImGui/imgui_impl_sdl.cpp>
#include <ImGui/imgui_impl_vulkan_but_better.h>
//#include <ImGui/imgui_filedialog.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <soloud/soloud.h>
#include <soloud/soloud_wav.h>

#include <MojoAL/mojoal.c>

#define WAVE_NO_MULTI_THREDED_LOADING
#include <Wave/WaveMath.h>
#include <Wave/WaveMesh.h>
#include <Wave/WaveLoader.h>
#include <Wave/WaveAudio.h>
/*
* Replace file dialog with imgui stack modal file dialog | done :)
*/
#include <Wave/WaveLayer.h>
#include <Wave/WavePhysics.h>

#include "Md2Loader.h"

#include <OpenVK/OpenVK.h>
#include "Helper.h"

#include <ImGui/FileDialog/ImFileDialog.cpp>

#include "Renderer/Renderer.h"

constexpr int32_t FloatToFixed(float a)
{
	return (int32_t)(a * 4096.0f);
}

constexpr float FixedToFloat(int32_t a)
{
	return (float(a) * 0.000244140625f);
}

int32_t main(int32_t argc, char** argv)
{
	constexpr int32_t a = FloatToFixed(5.6);
	constexpr int32_t b = FloatToFixed(2.7);
	constexpr int32_t c = a + b;
	constexpr int32_t d = (int64_t(a) * int64_t(b)) >> 12;
	constexpr float r = FixedToFloat(d);

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
		//	WaveSetPath((char*)"D:/3D Models/GLTF/");
		//	WaveModelData Data = WaveLoadModel("D:/3D Models/GLTF/CubeSep.gltf", WAVE_LOAD_MATERIAL | WAVE_GEN_NORMALS | WAVE_FLIP_UVS | WAVE_GEN_UVS | WAVE_GEN_INDICES | WAVE_PRINT_DEBUG_INOFS);
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
	Window = SDL_CreateWindow("Magma Engine v7 ImGui " IMGUI_VERSION, 0, 0, WindowWidth, WindowHeight, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_VULKAN | SDL_WINDOW_BORDERLESS);
	SDL_SetWindowMinimumSize(Window, 800, 540);
	SDL_GetWindowSize(Window, (int*)&WindowWidth, (int*)&WindowHeight);
	SDL_SetWindowPosition(Window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
#ifdef _WIN32
	SDL_SysWMinfo WmInfo;
	SDL_VERSION(&WmInfo.version);
	SDL_GetWindowWMInfo(Window, &WmInfo);
	WaveHwnd = WmInfo.info.win.window;
#endif	
	
	CreateRenderer();

	bool Run = true;
	while (Run)
	{
		while (SDL_PollEvent(&Event))
		{
			RendererEvent();

			if (Event.type == SDL_QUIT)
				Run = false;
		}			

		RendererRender();
	}

	DestroyRenderer();
	SDL_DestroyWindow(Window);
	SDL_Quit();
	printf("%f\n", WaveGetUsedMemory() * 0.000001);

	return 0;
}
