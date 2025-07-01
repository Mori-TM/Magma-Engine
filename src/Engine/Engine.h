bool IsEditorActive = true; //Init with true to fix camera jump on first press
bool GameMode = false;
bool LuaOnStart = true;
bool ReloadShaders = false;

typedef enum : uint32_t
{
	MODEL_DELETE_NOTHING = 1,
	MODEL_DELETE_MESH = 2,
	MODEL_DELETE_TEXURES = 4,
	MODEL_DELETE_MATERIALS = 8,
	MODEL_DELETE_MESH_COUNT
} ModelDeleteOptions;

uint32_t ModelToDelete = 0;
uint32_t ModelToDeleteOptions = MODEL_DELETE_NOTHING;

uint32_t TextureToDelete = 0;
uint32_t SamplerToDelete = 0;
bool DeleteTexture = false;

int32_t Frame = 0;
int32_t FinalTime = 0;
int32_t InitTime = 0;
float FPS = 0.0;
float MS = 0.0;

// From \\ to /
void ConvertFilePath(char* Path)
{
	if (Path == NULL)
		return;

	char* FileName = Path;
	for (char* Cur = Path; *Cur != '\0'; Cur++)
	{
		if (*Cur == '\\')
			*FileName = '/';
		else
			*FileName = *Cur;

		FileName++;
	}
	*FileName = '\0';

//	Path = FileName;
}

char* GetFileNameFromPath(char* Path)
{
	if (Path == NULL)
		return NULL;

	char* FileName = Path;
	for (char* Cur = Path; *Cur != '\0'; Cur++)
	{
		if (*Cur == '/' || *Cur == '\\')
			FileName = Cur + 1;
	}

	return FileName;
}

char* GetFileExtension(char* Path)
{
	uint32_t i;
	for (i = strlen(Path) - 1; i > 0; i--)
		if (Path[i] == '.') break;
	char* Extension = Path + i + 1;
	return Extension;
}

#ifdef _WIN32
#include <direct.h>
#define GetCurrentDir _getcwd
#else
#include <unistd.h>
#define GetCurrentDir getcwd
#endif

//example:  /home/moritz/C-Projects/Magma-Engine/src/Data/Textures/spiaggia_di_mondello.jpg ->
char* ConvertAbsoluteToRelativePath(char* Path)
{
	char AbsolutePath[MAX_CHAR_PATH_LENGTH];
	GetCurrentDir(AbsolutePath, ARRAY_SIZE(AbsolutePath));
	size_t AbsoluteLength = strlen(AbsolutePath);

	if (memcmp(AbsolutePath, Path, AbsoluteLength) == 0)
	{
#ifndef _WIN32//On Unix systems add + 1 to remove /
		AbsoluteLength += 1;
#endif
		return Path + AbsoluteLength;
	}

	return Path;
}

void PushEventSDL(Uint32 Type, Uint32 Key)
{
	SDL_Event Event;
	memset(&Event, 0, sizeof(SDL_Event));
	Event.type = Type;
//	Event.key.keysym.sym = Key;
	Event.key.key = Key;
	SDL_PushEvent(&Event);
}
 
//template <typename T>
void CheckForSameNames(CMA_MemoryZone* MemZone, size_t MaxSize, const char* NameToSearch, char* NewName)
{
	uint32_t Count = 0;
	for (uint32_t i = 0; i < MemZone->Size; i++)
	{
		void* NamePtr = CMA_GetAt(MemZone, i);
		if (NamePtr != NULL)
		{
			char Name[MAX_CHAR_NAME_LENGTH];
			memcpy(Name, NamePtr, MAX_CHAR_NAME_LENGTH);
			if (strstr(Name, NameToSearch) != 0)
				Count++;
		}		
	}

	if (Count > 0)
		snprintf(NewName, MaxSize, "%s (%d)", NameToSearch, Count);
	else
		strncpy(NewName, NameToSearch, MaxSize);

	NewName[MaxSize - 1] = '\0';
}

void ResetSceneSettings()
{
	for (uint32_t i = 0; i < 4; i++)
	{
		SceneFragmentUBO.CascadeRange[i] = 2;
		SceneFragmentUBO.CascadeScale[i] = 0.6;
		SceneFragmentUBO.CascadeBias[i] = 0.0009;
	}
//	SceneFragmentUBO.CascadeBias[0] = 0.002;
//	SceneFragmentUBO.CascadeScale[0] = 1.0;

	SceneFragmentUBO.CascadeRange[0] = 2;
	SceneFragmentUBO.CascadeScale[0] = 1.044;
	SceneFragmentUBO.CascadeBias[0] = 0.002;

	SceneFragmentUBO.CascadeRange[1] = 2;
	SceneFragmentUBO.CascadeScale[1] = 0.6;
	SceneFragmentUBO.CascadeBias[1] = 0.0009;

	SceneFragmentUBO.CascadeRange[2] = 8;
	SceneFragmentUBO.CascadeScale[2] = 0.681;
	SceneFragmentUBO.CascadeBias[2] = 0.0009;

	SceneFragmentUBO.AmbientMultiplier = 0.6;
	SceneFragmentSBO.LightCount = 0;
	for (uint32_t i = 0; i < MAX_NUMBER_OF_LIGHTS; i++)
	{
		SceneFragmentSBO.LightPos[i] = Vec4f(0.0);
		SceneFragmentSBO.LightColor[i] = Vec4f(1.0);
		SceneFragmentSBO.LightCastShadow[i] = 0;
		SceneFragmentSBO.LightType[i] = 0;
	}

	CascadeSplitLambda = 0.95;
	CascadeNearClip = 0.5;
	CascadeFarClip = 48.0;

	SceneFragmentUBO.Gamma = 0.9;
	SceneFragmentUBO.Exposure = 4.0;

	ClearColor = Vec3(0.15, 0.3, 0.7);
	SceneCullMode = CULL_MODE_BACK;
	ShadowCullMode = CULL_MODE_FRONT;
//	ShadowMapHeight = ShadowMapSizeTmp;
//	ShadowMapWidth = ShadowMapHeight * SHADOW_MAP_CASCADE_COUNT;
//	ReloadShaders = true;
}

void MeasureFPS()
{
	Frame++;
	FinalTime = time(NULL);
	if (FinalTime - InitTime > 0)
	{
		FPS = Frame / (FinalTime - InitTime);
		Frame = 0;
		InitTime = FinalTime;

		MS = 1000 / FPS;
	}
}

void StartStopGameMode()
{
	GameMode = !GameMode;
	LuaOnStart = true;
}

void EngineInit()
{
	
}

void EngineDestroy()
{

}