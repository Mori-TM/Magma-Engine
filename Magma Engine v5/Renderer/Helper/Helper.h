float CameraFOV = 75.0;
const float CameraZoomFOV = 10.0;
const float CameraNormalFOV = 75.0;
float CameraNearZ = 0.01;
float CameraFarZ = 1000.0;
vec3 CameraPos = { 0.0, 2.0, 5.0 };
vec3 CameraDir = { 0.0, 0.0, 1.0 };
vec3 CameraUp = { 0.0, 1.0, 0.0 };
vec3 ClearColor = { 0.15, 0.3, 0.7 };
bool ForceResizeEvent = false;
bool FullScreen = false;
bool ForceFullScreenEvent = false;

uint32_t MsaaSamples = 4;

double ShadowRenderingTime = 0;
double SceneRenderingTime = 0;
double SwapChainRenderingTime = 0;
double SSRRenderingTime = 0;
double BeginFrameTime = 0;
double EndFrameTime = 0;
double FrameTime = 0;

const char** GetExtensions(uint32_t* ExtensionCount)
{
	SDL_Vulkan_GetInstanceExtensions(Window, ExtensionCount, NULL);
	const char** Extensions = (const char**)malloc(*ExtensionCount * sizeof(char*));
	SDL_Vulkan_GetInstanceExtensions(Window, ExtensionCount, Extensions);
	return Extensions;
}

VkSurfaceKHR GetSurface(VkInstance* Instance)
{
	VkSurfaceKHR Surface;
	SDL_Vulkan_CreateSurface(Window, *Instance, &Surface);
	return Surface;
}

void GetWindowSize(uint32_t* Width, uint32_t* Height)
{
	SDL_GetWindowSize(Window, (int*)Width, (int*)Height);
}

uint32_t GetMousePos(int32_t* x, int32_t* y)
{
	return SDL_GetMouseState(x, y);
}
