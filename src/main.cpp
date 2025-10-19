#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdarg.h>
#include <time.h>
#include <string.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>
#include <Renderdoc/renderdoc_app.h>
#include <vulkan/vulkan.h>
#include <thread>
#include <future>
#include <mutex>
#include <chrono>
#include <condition_variable>

//#define MAGMA_ENGINE_CUSTOM_TITLE_BAR
#define MAGMA_ENGINE_TRACK_MEMORY
#define CMA_STORE_DEBUG_NAME_IN_RAM
#define OPENVK_STORE_DEBUG_NAME_IN_RAM
#define FILE_DIALOG_USE_IMGUI

#include <DynamicArray/DynamicArray.h>

#ifdef MAGMA_ENGINE_TRACK_MEMORY
#include <MallocSucks/MallocSucks.h>
#define malloc s_malloc
#define calloc s_calloc
#define realloc s_realloc
#define free s_free
#endif

//#include <stb/stb_sprintf.h>
#include <stb/dxt/stb_dxt.c>
#include <stb/stb_image_resize.h>

#include <ImGui/imconfig.h>
/*
#include <ImGui/imgui_tables.cpp>
#include <ImGui/imgui_internal.h>
#include <ImGui/imgui.cpp>
#include <ImGui/imgui_draw.cpp>
#include <ImGui/imgui_widgets.cpp>
#include <ImGui/imgui_demo.cpp>
#include <ImGui/imgui_impl_sdl3.cpp>
*/

//#define IMGUI_STB_NAMESPACE
#include <ImGui/imconfig.h>
#include <ImGui/imgui.h>
#include <ImGui/imgui_custom.h>
//#include <ImGui/imgui_internal.h>
#include <ImGui/imgui_impl_sdl3.h>
#include <ImGui/imgui_impl_vulkan_but_better.h>
#include <ImGui/ImFileDialog/ImFileDialog.h>


#include <Wave/WaveMath.h>
#include <Wave/WaveMesh.h>
#include <Wave/WaveLoader.h>
//#include <Wave/WaveAudio.h>
#include <Wave/WaveLayer.h>
#include <Wave/WavePhysics.h>

#include <ThreadJob/ThreadJob.h>
#include <Json/Json.h>
#include <Base64/Base64.h>
#include <Md2Loader/Md2Loader.h>
#include <OpenVK/OpenVK.h>

#include "Helper.h"


#include "Renderer/Renderer.h"

/*
* General improvements:
* - Conventional arrays are now that scene loading is implemented too unreliable one light type not in range and all crahses
* - Naming consistency
* - print spelling - either "Every Thing Like This" or "Every thing like this"
* - Add option for mesh saving in scene file or just file path and the same for textures
* 
* General Infos:
* Shaders only get recompiled on request in the engine
*/

void ExampleCallback(void *userdata, const char * const *filelist, int filter)
{
	if (!filelist) {
        SDL_Log("An error occured: %s", SDL_GetError());
        return;
    } else if (!*filelist) {
        SDL_Log("The user did not select any file.");
        SDL_Log("Most likely, the dialog was canceled.");
        return;
    }

    while (*filelist) {
        SDL_Log("Full path to selected file: '%s'", *filelist);
        filelist++;
    }

    if (filter < 0) {
        SDL_Log("The current platform does not support fetching "
                "the selected filter, or the user did not select"
                " any filter.");
    }

	printf("Callback\n");
}

typedef struct {
    int value;
    char name[32];
} TestStruct;

#define TEST_SIZE 1000

void BenchmarkAndTest() {
//    CMA_MemoryZone zone = CMA_Create(sizeof(TestStruct), "TestZone");
	CMA_MemoryZone zone;
	memset(&zone, 0, sizeof(CMA_MemoryZone));

    size_t indices[TEST_SIZE];

    // Push TEST_SIZE elements
    for (int i = 0; i < TEST_SIZE; i++) {
        TestStruct data;
		data.value = i;
		strcpy(data.name, "Test");

        snprintf(data.name, sizeof(data.name), "Item %d", i);
        indices[i] = CMA_Push(&zone, &data);
    }

    // Validate data correctness after push
    for (int i = 0; i < zone.Size; i++) {
        TestStruct* data = (TestStruct*)CMA_GetAt(&zone, indices[i]);
        if (data != NULL) {
            assert(data->value == i);
            printf("Before Pop: Index %zu = %s (%d)\n", indices[i], data->name, data->value);
        }
		else
		{
			assert(data != NULL);
		}
    }

	srand(22);
    // Pop every third element
    for (int i = 0; i < zone.Size; i++) {
		if (rand() % 2 == 0)
        	CMA_Pop(&zone, indices[i]);
    }

    // Validate remaining elements and check index validity
    for (int i = 0; i < zone.Size; i++) {
        TestStruct* data = (TestStruct*)CMA_GetAt(&zone, indices[i]);
        if (data == NULL) {
            printf("After Pop: Index %zu has been popped.\n", indices[i]);
        } else {
            assert(data->value == i);
            printf("After Pop: Index %zu = %s (%d)\n", indices[i], data->name, data->value);
        }
    }

    CMA_Destroy(&zone);
    printf("Benchmark and test completed successfully.\n");
}

void WaveParseBumpParameters(char* Buffer)
{
	char* Old = Buffer;

	if (*(Buffer++) == '-' &&
		*(Buffer++) == 'b' && 
		*(Buffer++) == 'm')
	{
		while (isdigit(*Buffer) || *(Buffer++) == ' ');
		
		memmove(Old, Buffer, strlen(Buffer) + 1);
	}
	else
		return;

	
}

int32_t main(int32_t argc, char** argv)
{
	/*
	char res[4096];
	char Line[] = "map_Bump -bm 0.500000 col_head_1stfloor_Normal.png";
	WaveScan(Line, "map_Bump", "%[^\r\n]%*c\r\n", res);
	printf(res);
	WaveParseBumpParameters(res);
	printf(res);

	return 0;

	Argc = argc;
	Argv = argv;
	printf("Argv: %s\n", Argv[0]);
	
*/

//	char Path[MAX_CHAR_PATH_LENGTH];
//	sstrcpy(Path, "/home/moritz/C-Projects/Magma-Engine/src/Data/Textures/spiaggia_di_mondello.jpg");
//	char* NewPath = ConvertAbsoluteToRelativePath(Path);
//	printf("Old: %s\nNew: %s\n", Path, NewPath);
//	exit(0);

Restart:
#ifdef MAGMA_ENGINE_TRACK_MEMORY
	s_init();
#endif

//	return 0;
/*
	{
		BenchmarkAndTest();
		BenchmarkAndTest();
		BenchmarkAndTest();
		BenchmarkAndTest();
		s_checkForLeaks();
		s_destroy();
		return 0;
	}
	*/
//	system("echo $PATH");
//	execlp("sh", "sh", "-c", "glslc /home/moritz/C-Projects/Magma-Engine/src/Data/Shader/FXAAFragment.frag -o /home/moritz/C-Projects/Magma-Engine/src/Data/Shader/FXAAFragment.spv", NULL);
//	exit(0);

	
	ShaderCompilerRequest();
	//CompileShader();
//	exit(0);
	MainHWnd.Width = 1920;//1920;
	MainHWnd.Height = 1110;//1110;

//	SDL_SetMemoryFunctions(malloc, calloc, realloc, free);

	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_EVENTS);
#ifdef MAGMA_ENGINE_CUSTOM_TITLE_BAR
	SDL_WindowFlags WindowFlags = SDL_WINDOW_RESIZABLE | SDL_WINDOW_INPUT_FOCUS | SDL_WINDOW_VULKAN | SDL_WINDOW_BORDERLESS;
#else
	SDL_WindowFlags WindowFlags = SDL_WINDOW_RESIZABLE | SDL_WINDOW_INPUT_FOCUS | SDL_WINDOW_VULKAN;
#endif
	MainHWnd.Wnd = SDL_CreateWindow("Magma Engine v7 ImGui " IMGUI_VERSION, MainHWnd.Width, MainHWnd.Height, WindowFlags);
	SDL_SetWindowMinimumSize(MainHWnd.Wnd, 800, 540);
	SDL_GetWindowSize(MainHWnd.Wnd, (int*)&MainHWnd.Width, (int*)&MainHWnd.Height);
	SDL_SetWindowPosition(MainHWnd.Wnd, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
#ifdef _WIN32
/*
	SDL_SysWMinfo WmInfo;
	SDL_VERSION(&WmInfo.version);
	SDL_GetWindowWMInfo(MainHWnd.Wnd, &WmInfo);
	WaveHwnd = WmInfo.info.win.window;
*/
	
	//FIX - Needs testing on windows but should work
	SDL_PropertiesID Properties = SDL_GetWindowProperties(MainHWnd.Wnd);
	if (!Properties)
		WaveHwnd = NULL;
	else
		WaveHwnd = (HWND)SDL_GetPointerProperty(Properties, SDL_PROP_WINDOW_WIN32_HWND_POINTER, NULL);
#endif	

/*
	const SDL_DialogFileFilter Filter[] = {
    { "PNG images",  "png" },
    { "JPEG images", "jpg;jpeg" },
    { "All images",  "png;jpg;jpeg" },
    { "All files",   "*" }
};

	SDL_ShowOpenFileDialog(ExampleCallback, NULL, MainHWnd.Wnd, Filter, ARRAY_SIZE(Filter), NULL, true);
*/

	if (RenderGamePreview)
	{
		SceneWidth = MainHWnd.Width;
		SceneHeight = MainHWnd.Height;
	}

	RendererCreate();
	bool Run = true;

//	AddModel(0, "/home/moritz/Documents/GitHub/Magma-Engine/src/Room.obj");

//	SceneLoad("PhysicsScene (copy).lva");//ExampleScene//PhysicsScene//SponzaScene

	while (Run)
	{
		if (RestartEngine)
			Run = false;

		while (SDL_PollEvent(&MainHWnd.Event))
		{
			RendererEvent();

			if (MainHWnd.Event.type == SDL_EVENT_QUIT)
				Run = false;
		}


		RendererRun();


		FileDialogCheck();
		FrameCount++;
	}
	
//	SceneSave("PhysicsScene.lva");

	

	RendererDestroy();
	SDL_DestroyWindow(MainHWnd.Wnd);
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
