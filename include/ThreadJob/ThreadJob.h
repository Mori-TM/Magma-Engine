#ifndef THREAD_JOB_IMPLEMENTATION
#define THREAD_JOB_IMPLEMENTATION

/*
#ifdef _WIN32
#include "ThreadJobWin32.h"
#else
//#error your platform is currently not implemented
#include "ThreadJobLinux.h"
#endif
*/

#include <SDL3/SDL.h>

typedef enum
{
	TJ_PUSH_BACK_DEFAULT = 0,
	TJ_PUSH_BACK_HIGHEST,
	TJ_PUSH_BACK_HIGH,
	TJ_PUSH_BACK_LOW,
	TJ_PUSH_BACK_LOWEST
} TJ_PushBackOptions;

typedef enum
{
	TJ_THREAD_STATUS_UNSET = 0,
	TJ_THREAD_STATUS_RUNNING,
	TJ_THREAD_STATUS_FINISHED
} TJ_ThreadStatus;

typedef struct
{
	SDL_Thread* ThreadID;
} TJ_Job;

/*
typedef enum SDL_ThreadState
{
    SDL_THREAD_STATE_ALIVE,
    SDL_THREAD_STATE_DETACHED,
    SDL_THREAD_STATE_ZOMBIE,
    SDL_THREAD_STATE_CLEANED,
} SDL_ThreadState;

typedef struct SDL_error
{
    SDL_free_func free_func;
} SDL_error;

#ifdef SDL_PLATFORM_WIN32
typedef HANDLE SYS_ThreadHandle;
#elif SDL_PLATFORM_UNIX
typedef pthread_t SYS_ThreadHandle;
#else
typedef int SYS_ThreadHandle;
#error Threadhandle not validated
#endif

typedef struct 
{
    SDL_ThreadID threadid;
    SYS_ThreadHandle handle;
    int status;
    SDL_AtomicInt state;
    SDL_error errbuf;
    char *name;
    size_t stacksize; // 0 for default, >0 for user-specified stack size.
    int(SDLCALL *userfunc)(void *);
    void *userdata;
    void *data;
    SDL_FunctionPointer endfunc; // only used on some platforms.
} SDL_ThreadIntern;
*/

int TJ_GetStatus(TJ_Job* Job)
{
	return SDL_GetThreadState(Job->ThreadID);
//	SDL_ThreadIntern* Intern = (SDL_ThreadIntern*)Job->ThreadID;
//	return SDL_GetAtomicInt(&Intern->state);
}


bool TJ_AddJob(int (*Function)(void* Data), void* Args, DynamicArray* JobCenter)
{
	TJ_Job Job;
	memset(&Job, 0, sizeof(TJ_Job));
	
//	Job.Function = Function;
//	Job.Args = Args;
	Job.ThreadID = SDL_CreateThread(Function, "Thread", Args);
//	SDL_SetAtomicInt(&Job->Status, TJ_THREAD_STATUS_UNSET);
//	Job->UserData = Args;
//	void** ThreadID = _beginthread(Function, 0, Args);
	return DynamicArrayPush(JobCenter, &Job);
}

//Also destroys the given jobcenter
void TJ_WaitForJobCenter(DynamicArray* JobCenter)
{
	for (size_t i = 0; i < JobCenter->Size; i++)
	{
		TJ_Job* Job = (TJ_Job*)DynamicArrayGetAt(JobCenter, i);
		if (Job)
		{
		//	if (Job->ThreadID != NULL)
			{
				int ThreadReturnValue;
				SDL_WaitThread(Job->ThreadID, &ThreadReturnValue);
			}

		//	WaitForSingleObject(Job->ThreadID, INFINITE);
			
		//	CloseHandle(Job->ThreadID);
		}
		else
		{
			printf("[TJ Error]: Failed to find job %zu\n", i);
		}
	}

	DynamicArrayDestroy(JobCenter);
}
/*
void TJ_Async(int (*Function)(void* Data), void* Args, TJ_Job* Job)
{
	Job->ThreadID = SDL_CreateThread(Function, "AThread", Args);
//	SDL_SetAtomicInt(&Job->Status, TJ_THREAD_STATUS_UNSET);
//	Job->UserData = Args;
//	return SDL_CreateThread(Function, "AThread", Args);
}

void TJ_Wait(TJ_Job* Job)
{
	if (Job && Job->ThreadID)
	{
		int ThreadReturnValue;
		SDL_WaitThread(Job->ThreadID, &ThreadReturnValue);
	}
}
*/
#endif