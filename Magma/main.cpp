#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_vulkan.h>
#include <vulkan/vulkan.h>

extern "C"
{
#ifdef _WIN32
#include <lua/lua.h>
#include <lua/lauxlib.h>
#include <lua/lualib.h>
#elif __linux__
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
#endif
}

#include "../libs/ImGui/src/imconfig.h"
#include "../libs/ImGui/src/imgui_tables.cpp"
#include "../libs/ImGui/src/imgui_internal.h"
#include "../libs/ImGui/src/imgui.cpp"
#include "../libs/ImGui/src/imgui_draw.cpp"
#include "../libs/ImGui/src/imgui_widgets.cpp"
#include "../libs/ImGui/src/imgui_demo.cpp"
#include "../libs/ImGui/src/imgui_impl_sdl.cpp"
#include "../libs/ImGui/src/imgui_impl_vulkan.h"

#ifdef _WIN32
#include <Wave/WaveMath.h>
#include <Wave/WaveLoader.h>
#include <Wave/WaveAudio.h>
#include <Wave/WaveLayer.h>
#include <Wave/WavePhysics.h>
#endif

#ifdef _WIN32
#include <OpenVK/OpenVK.h>
#endif


#include "Helper.h"
#include "Renderer/Renderer.h"

int32_t main(int32_t argc, char** argv)
{
//	//C:/Users/Moritz Desktop/source/repos/Vulkan C99/Vulkan C99/Cube.dae
////	WaveLoadModel("C:/Users/Moritz Desktop/source/repos/Vulkan C99/Vulkan C99/Cube.dae", 0);
//	uint32_t BeginTime = SDL_GetTicks();
//	WaveModelData Data = WaveLoadModel("D:/3D Models/Sponza-master/Sponza.dae", 0);
////	WaveModelData Data = WaveLoadModel("C:/Users/Moritz Desktop/source/repos/Vulkan C99/Vulkan C99/Cube.dae", 0);
//	uint32_t EndTime = SDL_GetTicks();
//	printf("Loading time: %d\n", EndTime - BeginTime);
//	printf("Vertices: %d\n", Data.VertexCount);
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
	Window = SDL_CreateWindow("Magma Engine v5 ImGui " IMGUI_VERSION, 0, 30, WindowWidth, WindowHeight, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_VULKAN | SDL_WINDOW_MAXIMIZED);
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
