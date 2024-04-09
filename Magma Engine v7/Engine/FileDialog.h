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

#define FILE_DIALOG_USE_IMGUI

void FileDialogOpenInstance(const char* Title, const char* Key, const char* FileNames, uint32_t Files, bool MultiSelect)
{
#ifdef FILE_DIALOG_USE_IMGUI 

	uint32_t Index = 0;
	char Filter[MAX_CHAR_NAME_LENGTH];
//	strcpycut(Filter, FileNames);

//	ssprintf(Filter, "%s ()")

	
	
//	if (Files & FILE_TYPE_OBJ)


	ifd::FileDialog::Instance().Open(Key, Title, "Quake Models (*.md2;*.bin){.md2,.bin},.*", true);
#else
#endif

}