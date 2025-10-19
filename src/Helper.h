#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define MIN(x, y) (((x) < (y)) ? (x) : (y))
#define ARRAY_SIZE(x)  (sizeof(x) / sizeof((x)[0]))
#define SWAP(T, a, b) do { T tmp = a; a = b; b = tmp; } while (0)

#define MAX_CHAR_NAME_LENGTH_SHORT 64
#define MAX_CHAR_NAME_LENGTH 1024
#define MAX_CHAR_PATH_LENGTH FILENAME_MAX
#define MAX_CHAR_LINE_LENGTH 128
//#define MAX_CHAR_SCRIPT_LENGTH 1048576

//Just use for stack allocated strings in dst!!!
//This is like strcpy but it checks if buffer size is not overrun
//safe string copy
#define sstrcpy(dst, src) do { strncpy(dst, src, ARRAY_SIZE(dst)); dst[ARRAY_SIZE(dst) - 1] = '\0'; } while (0)

//Just use for stack allocated strings in Buffer!!!
//This one always adds a null terminator at the end!
#define ssprintf(Buffer, Format, ...) do { snprintf(Buffer, ARRAY_SIZE(Buffer), Format, __VA_ARGS__); Buffer[ARRAY_SIZE(Buffer) - 1] = '\0'; } while (0)

#define ERROR32 UINT32_MAX

#define true 1
#define false 0
#ifndef __cplusplus
typedef uint32_t bool;
#endif

#define E 2.71828182846
#define PI 3.141592653589793238
#define PI2 6.283185307179586

typedef enum
{
	CULL_MODE_NONE = 0x0,
	CULL_MODE_BACK = 0x1,
	CULL_MODE_FRONT = 0x2,
} CullModes;

typedef struct
{
	vec3 Min;
	vec3 Max;
} AABBData;

typedef struct
{
	SDL_Window* Wnd;//cause X.h steels Window
	SDL_Event Event;

	uint32_t Width;// = 1920;
	uint32_t Height;// = 1110;
} WindowHandle;

WindowHandle MainHWnd;
int32_t Argc = 0;
char** Argv = NULL;

#ifdef _WIN32
void ShaderCompilerRequest()
{
	system("GLSLCompiler.bat");
}

void ShaderCompilerWait()
{

}
#else
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
//#define GetCurrentDir getcwd

const char* ShaderCompilerCommands[] =
{
	"glslc Data/Shader/OffscreenVertex.vert -o Data/Shader/OffscreenVertex.spv",
	"glslc Data/Shader/DepthPrePassVertex.vert -o Data/Shader/DepthPrePassVertex.spv",
	"glslc Data/Shader/DepthPrePassFragment.frag -o Data/Shader/DepthPrePassFragment.spv",
	"glslc Data/Shader/SceneVertex.vert -o Data/Shader/SceneVertex.spv",
	"glslc Data/Shader/SceneFragment.frag -o Data/Shader/SceneFragment.spv",
	"glslc Data/Shader/ShadowVertex.vert -o Data/Shader/ShadowVertex.spv",
	"glslc Data/Shader/ShadowFragment.frag -o Data/Shader/ShadowFragment.spv",
	"glslc Data/Shader/DebugVertex.vert -o Data/Shader/DebugVertex.spv",
	"glslc Data/Shader/DebugFragment.frag -o Data/Shader/DebugFragment.spv",
	"glslc Data/Shader/GUIVertex.vert -o Data/Shader/GUIVertex.spv",
	"glslc Data/Shader/GUIFragment.frag -o Data/Shader/GUIFragment.spv",
	"glslc Data/Shader/GBufferVertex.vert -o Data/Shader/GBufferVertex.spv",
	"glslc Data/Shader/GBufferFragment.frag -o Data/Shader/GBufferFragment.spv",
	"glslc Data/Shader/SwapChainFragment.frag -o Data/Shader/SwapChainFragment.spv",
	"glslc Data/Shader/BlurFragment.frag -o Data/Shader/BlurFragment.spv",
	"glslc Data/Shader/SSRFragment.frag -o Data/Shader/SSRFragment.spv",
	"glslc Data/Shader/SSAOFragment.frag -o Data/Shader/SSAOFragment.spv",
	"glslc Data/Shader/SSAOBlurFragment.frag -o Data/Shader/SSAOBlurFragment.spv",
	"glslc Data/Shader/FXAAFragment.frag -o Data/Shader/FXAAFragment.spv",
	"glslc Data/Shader/ForwardVertex.vert -o Data/Shader/ForwardVertex.spv",
	"glslc Data/Shader/ForwardFragment.frag -o Data/Shader/ForwardFragment.spv",
	"glslc Data/Shader/Raytracing/Vertex.vert -o Data/Shader/Raytracing/Vertex.spv",
	"glslc Data/Shader/Raytracing/Fragment.frag -o Data/Shader/Raytracing/Fragment.spv",
	"glslc Data/Shader/Raytracing/closesthit.rchit --target-env=vulkan1.2 -o Data/Shader/Raytracing/closesthit.spv",
	"glslc Data/Shader/Raytracing/miss.rmiss --target-env=vulkan1.2 -o Data/Shader/Raytracing/miss.spv",
	"glslc Data/Shader/Raytracing/shadow.rmiss --target-env=vulkan1.2 -o Data/Shader/Raytracing/shadow.spv",
	"glslc Data/Shader/Raytracing/raygen.rgen --target-env=vulkan1.2 -o Data/Shader/Raytracing/raygen.spv"
};

int ExectuteCompilerCommand(void* Data)
{
	const char* Command = (const char*)Data;

	pid_t Pid;
	int Status;

	Pid = fork();  // Create a child process

	if (Pid < 0) 
	{
		// Fork failed
		perror("fork failed");
		exit(EXIT_FAILURE);
	} 
	else if (Pid == 0) 
	{

		execlp("sh", "sh", "-c", Command, (char *)NULL);

		perror("execlp failed");
		exit(EXIT_FAILURE);
	} 
	else {
		// This is the parent process
		waitpid(Pid, &Status, 0);  // Wait for the child to finish

		if (!WIFEXITED(Status))
		{
			printf("Child process did not terminate normally\n");
		}
	}
	return 0;
}

DynamicArray ShaderCompilerJobCenter;

void ShaderCompilerRequest()
{
		DynamicArrayCreate(&ShaderCompilerJobCenter, sizeof(TJ_Job), "GLSLCompiler");

		for (int i = 0; i < ARRAY_SIZE(ShaderCompilerCommands); i++)
			TJ_AddJob(ExectuteCompilerCommand, (void*)ShaderCompilerCommands[i], &ShaderCompilerJobCenter);

/*
	TJ_Job* Job = (TJ_Job*)DynamicArrayGetAt(&ShaderCompilerJobCenter, 0);
	int status = TJ_GetStatus(Job);
	while (status == SDL_THREAD_ALIVE)
	{
		status = TJ_GetStatus(Job);
		printf("Status is: %d\n", status);
		SDL_Delay(200);
	}
*/		
}

bool ShaderCompilerFinished()
{

}

void ShaderCompilerWait()
{
	TJ_WaitForJobCenter(&ShaderCompilerJobCenter);

	if (ShaderCompilerJobCenter.Size != 0)
		printf("Daa heilll: %zu\n", ShaderCompilerJobCenter.Size);
}

#endif


float DeltaTime = 0.0;
float LastTime = 0.0;

float GetDeltaTime()
{
	float CurrentTicks = (float)SDL_GetTicks() / 1000.0f;
	DeltaTime = CurrentTicks - LastTime;
	LastTime = CurrentTicks;
	
	return DeltaTime;
}

VkDescriptorSet* GetDescriptorSet(uint32_t DescriptorSet)
{
	VkDescriptorSetInfo* DescriptorSetInfo = (VkDescriptorSetInfo*)CMA_GetAt(&VkRenderer.DescriptorSets, DescriptorSet);
	if (!DescriptorSetInfo)
	{
		printf("Failed to find requested Descriptor Set\n");
		return NULL;
	}
	return DescriptorSetInfo->DescriptorSets;
}

double GetExecutionTime(void (*Func)(void))
{
	struct timespec TimerStartTime;
	struct timespec TimerEndTime;

	timespec_get(&TimerStartTime, TIME_UTC);
	Func();
	timespec_get(&TimerEndTime, TIME_UTC);

	double Time = (double)((TimerEndTime.tv_sec) * 1000.0 + (TimerEndTime.tv_nsec) / 1000000.0) - ((TimerStartTime.tv_sec) * 1000.0 + (TimerStartTime.tv_nsec) / 1000000.0);

	return Time;
}

double GetExecutionTime(OpenVkBool (*Func)(void))
{
	struct timespec TimerStartTime;
	struct timespec TimerEndTime;

	timespec_get(&TimerStartTime, TIME_UTC);
	Func();
	timespec_get(&TimerEndTime, TIME_UTC);

	double Time = (double)((TimerEndTime.tv_sec) * 1000.0 + (TimerEndTime.tv_nsec) / 1000000.0) - ((TimerStartTime.tv_sec) * 1000.0 + (TimerStartTime.tv_nsec) / 1000000.0);

	return Time;
}

double GetExecutionTimeOpenVkRender(OpenVkBool(*Func)(void (*RenderFunc)(void), void (*ResizeFunc)(OpenVkBool RecreateSwapChain), void (*UpdateFunc)(void)), void(*RenderFunc)(void), void(*ResizeFunc)(OpenVkBool RecreateSwapChain), void(*UpdateFunc)(void))
{
	struct timespec TimerStartTime;
	struct timespec TimerEndTime;

	timespec_get(&TimerStartTime, TIME_UTC);
	Func(RenderFunc, ResizeFunc, UpdateFunc);
	timespec_get(&TimerEndTime, TIME_UTC);

	double Time = (double)((TimerEndTime.tv_sec) * 1000.0 + (TimerEndTime.tv_nsec) / 1000000.0) - ((TimerStartTime.tv_sec) * 1000.0 + (TimerStartTime.tv_nsec) / 1000000.0);

	return Time;
}

double GetExecutionTimeOpenVkBool(OpenVkBool(*Func)(void))
{
	struct timespec TimerStartTime;
	struct timespec TimerEndTime;

	timespec_get(&TimerStartTime, TIME_UTC);
	Func();
	timespec_get(&TimerEndTime, TIME_UTC);

	double Time = (double)((TimerEndTime.tv_sec) * 1000.0 + (TimerEndTime.tv_nsec) / 1000000.0) - ((TimerStartTime.tv_sec) * 1000.0 + (TimerStartTime.tv_nsec) / 1000000.0);

	return Time;
}

bool RestartEngine = false;

/*
const size_t MEMORY_POOL_SIZE = 1024 * 1024 * 100;
size_t MemoryUsedSize = 0;

void* SaveMalloc(size_t Size)
{
	MemoryUsedSize += Size;
	if (MemoryUsedSize > MEMORY_POOL_SIZE)
		return NULL;

	void* Mem = malloc(Size);
	if (Mem == NULL)
		printf("Warning: Failed to allcoate Memory: %zu!\n", Size);

	return Mem;
}

void* SaveRealloc(void* Data, size_t Size)
{
	MemoryUsedSize += Size;
	if (MemoryUsedSize > MEMORY_POOL_SIZE)
		return NULL;

	void* Mem = realloc(Data, Size);	
	if (Mem == NULL)
		printf("Warning: Failed reallcoate Memory: %x %zu!\n", Data, Size);

	return Mem;
} 

void SaveFree(void* Data)
{
	free(Data);
	Data = NULL;
}

//Just for testing how engine reacts if no more ram is available
#define malloc SaveMalloc
#define realloc SaveRealloc
#define free SaveFree
*/