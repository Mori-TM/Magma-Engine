typedef enum
{
	FILE_TYPE_OBJ = 0,
	FILE_TYPE_STL = 1,
	FILE_TYPE_GLTF = 2,
	FILE_TYPE_GLB = 4,
	
	FILE_TYPE_PNG = 8,
	FILE_TYPE_JPG = 16,
	FILE_TYPE_JPEG = 32,
	FILE_TYPE_TGA = 64,
	FILE_TYPE_HDR = 128,
	FILE_TYPE_PSD = 256,
	FILE_TYPE_BMP = 512,
	
	FILE_TYPE_MD2 = 1024,
	FILE_TYPE_BIN = 2048,

	FILE_TYPE_COUNT,
} FileTypes;

//#define FILE_DIALOG_USE_IMGUI

typedef struct
{
	char Title[MAX_CHAR_NAME_LENGTH_SHORT];
	char FileExtensions[MAX_CHAR_NAME_LENGTH]; 
	char Path[MAX_CHAR_PATH_LENGTH];
	bool MultiSelect;
	bool OpenDialog;
	bool HadCheck;
} FileDialogInfo;

DynamicArray FileDialogQueue;

void FileDialogInit()
{
	DynamicArrayCreate(&FileDialogQueue, sizeof(FileDialogInfo), "FileDialogQueue");
}

void FileDialogDestroy()
{
	DynamicArrayDestroy(&FileDialogQueue);
}

void FileDialogAddInstance(const char* Title, const char* FileExtensions, bool OpenDialog, bool IsMultiSelect)
{
#ifdef FILE_DIALOG_USE_IMGUI 

	uint32_t Index = 0;
	char Filter[MAX_CHAR_NAME_LENGTH];
//	strcpycut(Filter, FileNames);

//	ssprintf(Filter, "%s ()")

	
	
//	if (Files & FILE_TYPE_OBJ)

#ifdef LINUX_PORT
	ifd::FileDialog::Instance().Open(Key, Title, "Quake Models (*.md2;*.bin){.md2,.bin},.*", true);
#endif
#else

	FileDialogInfo FD;
	strcpycut(FD.Title, Title);
	for (size_t i = 0; i < ARRAY_SIZE(FD.FileExtensions); i++)
	{
		FD.FileExtensions[i] = FileExtensions[i];

		if (FileExtensions[i] == '\0' &&
			FileExtensions[i + 1] == '\0')
			break;
	}
//	strcpycut(FD.FileExtensions, FileExtensions);
	FD.OpenDialog = OpenDialog;
	FD.MultiSelect = IsMultiSelect;
	FD.HadCheck = false;
	memset(FD.Path, 0, ARRAY_SIZE(FD.Path));
	DynamicArrayPush(&FileDialogQueue, &FD);
//	FD.Title

#endif

}

void FileDialogCheck()
{
	for (size_t i = 0; i < FileDialogQueue.Size; i++)
	{
		FileDialogInfo* FD = (FileDialogInfo*)DynamicArrayGetAt(&FileDialogQueue, i);
		if (FD)
		{
			OpenVkDeviceWaitIdle();
		//	if (FD->HadCheck == true || !WaveOpenFileDialog(FD->Title, FD->MultiSelect, FD->FileExtensions, FD->Path))
			if (FD->HadCheck == true || !WaveOpenFileDialog(FD->Title, FD->MultiSelect, FD->FileExtensions, FD->Path))
				DynamicArrayPop(&FileDialogQueue, i);
			else
				FD->HadCheck = true;
		}		
	}
}

bool FileDialogGetResult(const char* Title, size_t MaxPathLength, char* Path)
{
	for (size_t i = 0; i < FileDialogQueue.Size; i++)
	{
		FileDialogInfo* FD = (FileDialogInfo*)DynamicArrayGetAt(&FileDialogQueue, i);
		if (FD)
		{
			if (FD->HadCheck == true && memcmp(FD->Title, Title, strlen(Title)) == 0)
			{
			//	strncpy(Path, FD->Path, MIN(MaxPathLength, ARRAY_SIZE(FD->Path)));
				for (size_t i = 0; i < MIN(MaxPathLength, ARRAY_SIZE(FD->Path)); i++)
				{
					Path[i] = FD->Path[i];

					if (FD->Path[i] == '\0' &&
						FD->Path[i + 1] == '\0')
						break;
				}

				DynamicArrayPop(&FileDialogQueue, i);
				return true;
			}
		}		
	}

	return false;
}