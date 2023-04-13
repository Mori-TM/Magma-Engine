#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define MIN(x, y) (((x) < (y)) ? (x) : (y))
#define ARRAY_SIZE(x)  (sizeof(x) / sizeof((x)[0]))
#define SWAP(T, a, b) do { T tmp = a; a = b; b = tmp; } while (0)

#define MAX_CHAR_NAME_LENGTH_SHORT 64
#define MAX_CHAR_NAME_LENGTH 2048
#define MAX_CHAR_PATH_LENGTH 4096
#define MAX_CHAR_SCRIPT_LENGTH 1048576

#define true 1
#define false 0
#ifndef __cplusplus
typedef uint32_t bool;
#endif

#define E 2.71828182846
#define PI 3.141592653589793238
#define PI2 6.28318530718

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

SDL_Window* Window;
SDL_Event Event;

uint32_t WindowWidth = 1920;
uint32_t WindowHeight = 1110;

float DeltaTime = 0.0;
float LastTime = 0.0;

class Mtx
{
public:
	void lock()
	{

	}
	void unlock()
	{

	}
};

std::mutex Mutex;

float GetDeltaTime()
{
	float CurrentTime = (float)SDL_GetTicks() / 1000.0;
	DeltaTime = CurrentTime - LastTime;
	LastTime = CurrentTime;
	
	return DeltaTime;
}

VkDescriptorSet* GetDescriptorSet(uint32_t DescriptorSet)
{
	VkDescriptorSetInfo* DescriptorSetInfo = (VkDescriptorSetInfo*)CMA_GetAt(&VkRenderer.DescriptorSets, DescriptorSet);
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

double GetExecutionTimeOpenVkRender(OpenVkBool(*Func)(void (*RenderFunc)(void), void (*ResizeFunc)(void), void (*UpdateFunc)(void)), void(*RenderFunc)(void), void(*ResizeFunc)(void), void(*UpdateFunc)(void))
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

void* SaveMalloc(size_t Size)
{
	void* Mem = malloc(Size);
	if (Mem == NULL)
		printf("Warning: Failed to allcoate Memory: %zu!\n", Size);

	return Mem;
}

void* SaveRealloc(void* Data, size_t Size)
{
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
/*
#define malloc SaveMalloc
#define realloc SaveRealloc
#define free SaveFree
*/