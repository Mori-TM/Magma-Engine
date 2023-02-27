#define COMPONENT_COUNT 5

typedef enum
{
	COMPONENT_TYPE_MESH = 0,
	COMPONENT_TYPE_MATERIAL = 1,
	COMPONENT_TYPE_CAMERA = 2,
	COMPONENT_TYPE_AUDIO = 3,
	COMPONENT_TYPE_ANIMATION = 4,
} ComponentTypes;

typedef struct
{
	char Name[MAX_CHAR_NAME_LENGTH];
	uint32_t MeshIndex;
} MeshComponent;

typedef struct
{
	char Name[MAX_CHAR_NAME_LENGTH];
	uint32_t MaterialIndex;
	vec3 Color;
} MaterialComponent;

typedef struct
{
	char Name[MAX_CHAR_NAME_LENGTH];
	float FOV;
	float NearPlane;
	float FarPlane;
} CameraComponent;

typedef struct  
{
	char Name[MAX_CHAR_NAME_LENGTH];
	SoLoud::Wav Audio;
	SoLoud::handle Handle;
} AudioComponent;

typedef struct
{
	char Name[MAX_CHAR_NAME_LENGTH];
	uint32_t AnimationIndex;
} AnimationComponent;

typedef struct
{
	char Name[MAX_CHAR_NAME_LENGTH];
	vec3 Translate;
	vec3 Rotate;
	vec3 Scale;
	bool Selected;

	bool UsedComponents[COMPONENT_COUNT];
	MeshComponent Mesh;
	MaterialComponent Material;
	CameraComponent Camera;
	AudioComponent Audio;
	AnimationComponent Animation;
} EntityInfo;

uint32_t SelectedEntity = 0;
uint32_t SelectedTexture = 0;
uint32_t SelectedMaterial = 0;
uint32_t SelectedMesh = 0;
uint32_t SelectedAudio = 0;
uint32_t SelectedAnimation = 0;
uint32_t SelectedScript = 0;

uint32_t EntityCount = 0;
EntityInfo* Entities = NULL;

bool IsEditorActive = false;
bool GameMode = false;
bool LuaOnStart = true;
bool ReloadShaders = false;

uint32_t MeshToDelete = 0;
bool DeleteMesh = false;
bool DeleteMeshWithTextures = false;

uint32_t TextureToDelete = 0;
uint32_t SamplerToDelete = 0;
bool DeleteTexture = false;

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

typedef enum
{
	ICON_BUTTON_TYPE_LARGE = 0x0,
	ICON_BUTTON_TYPE_SMALL = 0x1,
	ICON_BUTTON_TYPE_EXT = 0x2,
} IconButtonTypes;

bool ImGuiIconButton(const char* Icon, const char* ID, uint32_t IconType, const ImVec2& Size = ImVec2(0, 0))
{
	ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(0, 0, 0, 255));
	if (IconType == ICON_BUTTON_TYPE_LARGE)
		ImGui::PushFont(IconFontLarge);
	else if (IconType == ICON_BUTTON_TYPE_SMALL)
		ImGui::PushFont(IconFontSmall);
	else if (IconType == ICON_BUTTON_TYPE_EXT)
		ImGui::PushFont(IconFontExt);
	ImGui::PushID(ID);
	bool Value = ImGui::Button(Icon, Size);
	ImGui::PopID();
	ImGui::PopFont();
	ImGui::PopStyleColor();

	return Value;
}
 
//template <typename T>
void CheckForSameNames(CMA_MemoryZone* MemZone, const char* NameToSearch, char* NewName)
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
		sprintf(NewName, "%s (%d)", NameToSearch, Count);
	else
		strcpy(NewName, NameToSearch);
}

void ResetSceneSettings()
{
	for (uint32_t i = 0; i < 4; i++)
	{
		SceneFragmentUBO.CascadeRange[i] = 2;
		SceneFragmentUBO.CascadeScale[i] = 0.6;
		SceneFragmentUBO.CascadeBias[i] = 0.0009;
	}
	SceneFragmentUBO.CascadeBias[0] = 0.002;
	SceneFragmentUBO.CascadeScale[0] = 1.0;

	CascadeSplitLambda = 0.95;
	CascadeNearClip = 0.5;
	CascadeFarClip = 48.0;

	SceneFragmentUBO.Gamma = 1.3;
	SceneFragmentUBO.Exposure = 4.0;

	ClearColor = Vec3(0.15, 0.3, 0.7);
	SceneFragmentUBO.LightDirection = Vec4(0.426, -0.876, -0.225, 0.0);
	SceneCullMode = CULL_MODE_BACK;
	ShadowCullMode = CULL_MODE_FRONT;
	ShadowMapHeight = ShadowMapSizeTmp;
	ShadowMapWidth = ShadowMapHeight * SHADOW_MAP_CASCADE_COUNT;
	ReloadShaders = true;
}
