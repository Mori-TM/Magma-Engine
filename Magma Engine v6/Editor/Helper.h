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

bool ImGuiIconButton(const char* Icon, const char* ID, bool Large, const ImVec2& Size = ImVec2(0, 0))
{
	ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(0, 0, 0, 255));
	if (Large)
		ImGui::PushFont(IconFontLarge);
	else
		ImGui::PushFont(IconFontSmall);
	ImGui::PushID(ID);
	bool Value = ImGui::Button(Icon, Size);
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