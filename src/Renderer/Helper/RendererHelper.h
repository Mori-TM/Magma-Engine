float CameraFOV = 75.0;
float CameraZoomFOV = 10.0;
float CameraNormalFOV = 75.0;
float CameraNearZ = 0.01;
float CameraFarZ = 1000.0;
vec3 CameraPos = { 0.0, 2.0, 5.0 };
vec3 CameraDir = { 0.0, 0.0, 1.0 };
vec3 CameraUp = { 0.0, 1.0, 0.0 };
vec3 CameraRight = { 1.0, 0.0, 0.0 };
vec3 ClearColor = { 0.15, 0.3, 0.7 };
bool ForceResizeEvent = false;
bool FullScreen = false;
bool ForceFullScreenEvent = false;
bool RenderDebug = true;
bool RenderGamePreview = false;
bool GamePerformanceOverlay = false;

const char* RenderModeNames[] =
{
	"Forward",
	"Deferred",
	"Raytraced"
};

typedef enum
{
	RENDER_MODE_FORWARD = 0,
	RENDER_MODE_DEFERRED,
	RENDER_MODE_RAYTRACED,
	RENDER_MODE_COUNT
} RenderModes;

//RenderModes
uint32_t RenderMode = RENDER_MODE_DEFERRED;
bool RenderDepthPrePass = false;
//bool RenderRaytraced = false;
//bool RenderDeferred = false;
bool RenderShadows = true;
bool RenderSSAO = true;
bool RenderSSAOBlur = true;
bool RenderSSR = true;
bool RenderFXAA = false;

bool ForceRenderOnce = true;//Used so that after every resize there are no errors with undefined image layouts

uint64_t FrameCount = 0;
double ShadowRenderingTime = 0;
double GBufferRenderingTime = 0;
double SSAORenderingTime = 0;
double SSAOBlurRenderingTime = 0;
double SceneRenderingTime = 0;
double SSRRenderingTime = 0;
double FXAARenderingTime = 0;
double SwapChainRenderingTime = 0;
double BeginFrameTime = 0;
double EndFrameTime = 0;
double FrameTime = 0;

const char** GetExtensions(uint32_t* ExtensionCount)
{
	SDL_Vulkan_GetInstanceExtensions(ExtensionCount);
	const char** Extensions = (const char**)malloc((*ExtensionCount) * sizeof(char*));
	char const * const * SDL_Extensions = SDL_Vulkan_GetInstanceExtensions(ExtensionCount);
	memcpy(Extensions, SDL_Extensions, (*ExtensionCount) * sizeof(char*));
	return Extensions;
}

VkSurfaceKHR GetSurface(VkInstance* Instance)
{
	VkSurfaceKHR Surface;
	SDL_Vulkan_CreateSurface(MainHWnd.Wnd, *Instance, NULL, &Surface);
	return Surface;
}

void GetWindowSize(uint32_t* Width, uint32_t* Height)
{
	SDL_GetWindowSize(MainHWnd.Wnd, (int*)Width, (int*)Height);
}

uint32_t GetMousePos(int32_t* x, int32_t* y)
{
	float mx;
	float my;

	SDL_MouseButtonFlags ret = SDL_GetMouseState(&mx, &my);

	*x = (int32_t)mx;
	*y = (int32_t)my;

	return ret;
}

//Not the best place to store this function
void GenerateAABB(AABBData* AABB, uint32_t VertexCount, SceneVertex* Vertices)
{
	SceneVertex* Data = &Vertices[0];
	AABB->Min = Vec3f(FLT_MAX);
	AABB->Max = Vec3f(-FLT_MAX);

	for (uint32_t i = 0; i < VertexCount; i++)
	{
		Data = &Vertices[i];
		AABB->Max.x = MAX(AABB->Max.x, Data->PosTexX.x);
		AABB->Max.y = MAX(AABB->Max.y, Data->PosTexX.y);
		AABB->Max.z = MAX(AABB->Max.z, Data->PosTexX.z);

		AABB->Min.x = MIN(AABB->Min.x, Data->PosTexX.x);
		AABB->Min.y = MIN(AABB->Min.y, Data->PosTexX.y);
		AABB->Min.z = MIN(AABB->Min.z, Data->PosTexX.z);
	}
}

void FillUintArray(uint32_t* Dst, uint32_t Value, uint32_t Count)
{
	for (uint32_t i = 0; i < Count; i++)
		Dst[i] = Value;
}