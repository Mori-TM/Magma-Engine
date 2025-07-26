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


//using namespace ngs::imgui;

typedef struct
{
	char Title[MAX_CHAR_NAME_LENGTH_SHORT];
	char Key[MAX_CHAR_NAME_LENGTH_SHORT];
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

// From "Image file (*.png;*.jpg;*.jpeg;*.bmp;*.tga){.png,.jpg,.jpeg,.bmp,.tga},.*" to "Image file (*.png, *.jpg, *.jpeg, *.bmp, *.tga)\0*.png;*.jpg;*.jpeg;*.bmp;*.tga\0"
void FileDialogConvertExtensions(char* FileExtensions, char* DstExtensions)
{
	strcpy(DstExtensions, FileExtensions);

	//change , to ;
	char* p = DstExtensions;
	while (*p)
	{
		if (*p == ',')
			*p = ';';
		p++;
	}

	FileExtensions += strlen(FileExtensions) + 1;

	printf("Ftitle: \"%s\"\n", DstExtensions);
	printf("Fex: \"%s\"\n", FileExtensions);

	

	size_t offset = strlen(DstExtensions);
	p = DstExtensions + strlen(DstExtensions);

	*(p++) = ' ';
	*(p++) = '{';
	while (*FileExtensions)
	{
		if (*FileExtensions != '*')
		{
			*p = *FileExtensions;

			if (*p == ';') *p = ',';

			p++;
		}
		
//		if ((*p) == ';')
//			(*p) = ',';

		FileExtensions++;
	}

	strcpy(p, "},.*");
	//},.*
//	*p = '\0';

	printf("Fcex: \"%s\"\n", DstExtensions);
//	;
}

void FileDialogAddInstance(const char* Title, const char* FileExtensions, bool OpenDialog, bool IsMultiSelect)
{
	FileDialogInfo FD;
	size_t j = 0;
	memset(FD.Key, 0, ARRAY_SIZE(FD.Key));
	for (size_t i = 0; i < strlen(Title); i++)
	{
		if (Title[i] != ' ')
			FD.Key[j++] = Title[i];
	}
	FD.Key[j] = '\0';

	
	sstrcpy(FD.Title, Title);
	for (size_t i = 0; i < ARRAY_SIZE(FD.FileExtensions); i++)
	{
		FD.FileExtensions[i] = FileExtensions[i];

		if (FileExtensions[i] == '\0' &&
			FileExtensions[i + 1] == '\0')
			break;
	}
//	sstrcpy(FD.FileExtensions, FileExtensions);
	FD.OpenDialog = OpenDialog;
	FD.MultiSelect = IsMultiSelect;
	FD.HadCheck = false;
	memset(FD.Path, 0, ARRAY_SIZE(FD.Path));
	

#ifdef FILE_DIALOG_USE_IMGUI	
	char ConvertedExtensions[MAX_CHAR_NAME_LENGTH]; 
	FileDialogConvertExtensions((char*)FileExtensions, ConvertedExtensions);
	memcpy(FD.FileExtensions, ConvertedExtensions, MAX_CHAR_NAME_LENGTH);
	
#endif

	DynamicArrayPush(&FileDialogQueue, &FD);
}



void FileDialogCheck()
{
	for (size_t i = 0; i < FileDialogQueue.Size; i++)
	{
		FileDialogInfo* FD = (FileDialogInfo*)DynamicArrayGetAt(&FileDialogQueue, i);
		if (FD)
		{
#ifndef FILE_DIALOG_USE_IMGUI

			OpenVkDeviceWaitIdle();

			if (FD->OpenDialog)
			{
				if (FD->HadCheck == true || !WaveOpenFileDialog(FD->Title, FD->MultiSelect, FD->FileExtensions, FD->Path))
					DynamicArrayPop(&FileDialogQueue, i);
				else
					FD->HadCheck = true;
			}
			else
			{
				if (FD->HadCheck == true || !WaveSaveFileDialog(FD->Path, FD->FileExtensions))
					DynamicArrayPop(&FileDialogQueue, i);
				else
					FD->HadCheck = true;
			}
#endif
		}		
	}
}

//during ImGui rendering
void FileDialogImGuiCheck(size_t i, FileDialogInfo* FD)
{
#ifdef FILE_DIALOG_USE_IMGUI
//	if (FD->OpenDialog)
	{
	//	ifd::FileDialog::Instance().Open("TextureOpenDialog", "Open a texture", "Image file (*.png;*.jpg;*.jpeg;*.bmp;*.tga){.png,.jpg,.jpeg,.bmp,.tga},.*");

		if (FD->OpenDialog)
			ifd::FileDialog::Instance().Open(FD->Key, FD->Title, FD->FileExtensions, FD->MultiSelect);
		else
			ifd::FileDialog::Instance().Save(FD->Key, FD->Title, FD->FileExtensions);
			

//		if (FD->HadCheck == true || !)
//			DynamicArrayPop(&FileDialogQueue, i);

		if (ifd::FileDialog::Instance().IsDone(FD->Key))
		{
			if (ifd::FileDialog::Instance().HasResult()) 
			{
				uint32_t j = 0;
				for (size_t i = 0; i < ifd::FileDialog::Instance().GetResults().size(); i++)
				{
					std::string Res = ifd::FileDialog::Instance().GetResults()[i].u8string();
					printf("IN Path: \"%s\"\n", ifd::FileDialog::Instance().GetResults()[i].u8string().c_str());

					for (size_t k = 0; k < Res.length(); k++)//strlen(Res)
					{
						FD->Path[j++] = Res[k];
					}

					FD->Path[j++] = '\0';
					
					
					if (j >= ARRAY_SIZE(FD->Path) - 1)
						break;
				}
				FD->Path[j++] = '\0';

				printf("FD Path: \"%s\"\n", FD->Path);
			}
			else
				DynamicArrayPop(&FileDialogQueue, i);

			ifd::FileDialog::Instance().Close();

			

			FD->HadCheck = true;
		}
	}
//	else
	{
		
	}
#endif
}

/*Example for a Path(it's windows style): "C:/textures/sky.png\0C:/textures/skin.png\0\0"*/
//must happen during ImGui rendering!!!
//FIX - can become performance botleneck with too many dialog - hash the titles for faster comparing
bool FileDialogGetResult(const char* Title, size_t MaxPathLength, char* Path)
{


	for (size_t i = 0; i < FileDialogQueue.Size; i++)
	{
		FileDialogInfo* FD = (FileDialogInfo*)DynamicArrayGetAt(&FileDialogQueue, i);
		if (FD)
		{
			


			if (memcmp(FD->Title, Title, strlen(Title)) == 0)
			{
				FileDialogImGuiCheck(i, FD);

				if (FD->HadCheck == true)
				{
					//	strncpy(Path, FD->Path, MIN(MaxPathLength, ARRAY_SIZE(FD->Path)));
					for (size_t i = 0; i < MIN(MaxPathLength, ARRAY_SIZE(FD->Path)); i++)
					{
						Path[i] = FD->Path[i];
	/*
						if (FD->Path[i] == '\0' &&
							FD->Path[i + 1] == '\0')
							{
								Path[i + 1] = FD->Path[i + 1];
								break;
							}
	*/
					}

					DynamicArrayPop(&FileDialogQueue, i);
					return true;
				}

			
			}
		}		
	}

	return false;
}