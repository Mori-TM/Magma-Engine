#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#define WAVE_MAX_LENGTH 4096

#ifdef _WIN32
#pragma comment(lib, "dsound.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "winmm.lib")

#include <Windows.h>
#include <mmsystem.h>
#include <dsound.h>
#include <psapi.h>
#include <Commdlg.h>
HWND WaveHwnd = NULL;
#ifdef UNICODE
WCHAR LastPath[WAVE_MAX_LENGTH];
#else
char LastPath[WAVE_MAX_LENGTH];
#endif // UNICODE

//OFN_ALLOWMULTISELECT | OFN_EXPLORER

LPWSTR WaveToLPWSTR(const char* String)
{
	WCHAR Out[WAVE_MAX_LENGTH];
	memset(Out, 0, WAVE_MAX_LENGTH * sizeof(WCHAR));

	size_t Length = strlen(String);
	for (size_t i = 0; i < Length; i++)
		Out[i] = String[i];

	return Out;
}

char* WaveToChar(LPWSTR String)
{
	char Out[WAVE_MAX_LENGTH];
	memset(Out, 0, WAVE_MAX_LENGTH * sizeof(char));

	size_t Length = lstrlenW(String);
	for (size_t i = 0; i < Length; i++)
		Out[i] = (char)String[i];

	return Out;
}


int32_t WaveOpenFileDialog(char* Path, int MultiSelect, unsigned short* Offset, const char* FileExtensions)
{
	OPENFILENAME OFN;

	ZeroMemory(&OFN, sizeof(OPENFILENAME));

#ifdef UNICODE
	WCHAR WinPath[WAVE_MAX_LENGTH];
	OFN.lpstrFile = WinPath;
	OFN.lpstrFilter = WaveToLPWSTR(FileExtensions);
//	wprintf(OFN.lpstrFilter);
#else
	OFN.lpstrFile = Path;
	OFN.lpstrFilter = FileExtensions;

#endif 
	
	OFN.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

	if (MultiSelect)
		OFN.Flags |= OFN_EXPLORER | OFN_ALLOWMULTISELECT;

	OFN.lStructSize = sizeof(OPENFILENAME);
	OFN.hwndOwner = WaveHwnd;	
	OFN.lpstrFile[0] = '\0';
	OFN.nMaxFile = WAVE_MAX_LENGTH;
	OFN.nFilterIndex = 1;

	GetCurrentDirectory(WAVE_MAX_LENGTH, LastPath);

	int32_t Ret = GetOpenFileName(&OFN);

	if (Offset != NULL)
		*Offset = OFN.nFileOffset;

#ifdef WAVE_AUTO_PATH_RESET
	SetCurrentDirectory(CurrentPath);
#endif

#ifdef UNICODE
	char* Out = WaveToChar(WinPath);
	strcpy(Path, Out);
#endif 

	return Ret;
}

int32_t WaveSaveFileDialog(char* Path, const char* FileExtensions)
{
	OPENFILENAME OFN;

	ZeroMemory(&OFN, sizeof(OPENFILENAME));

#ifdef UNICODE
	WCHAR WinPath[WAVE_MAX_LENGTH];
	OFN.lpstrFile = WinPath;
	OFN.lpstrFilter = WaveToLPWSTR(FileExtensions);

#else
	OFN.lpstrFile = Path;
	OFN.lpstrFilter = FileExtensions;

#endif 

	OFN.lStructSize = sizeof(OPENFILENAME);
	OFN.hwndOwner = WaveHwnd;
	OFN.lpstrFile[0] = '\0';
	OFN.nMaxFile = WAVE_MAX_LENGTH;
	OFN.nFilterIndex = 1;
	OFN.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

	GetCurrentDirectory(WAVE_MAX_LENGTH, LastPath);
	int32_t Ret = GetSaveFileName(&OFN);

#ifdef WAVE_AUTO_PATH_RESET
	SetCurrentDirectory(CurrentPath);
#endif

#ifdef UNICODE
	char* Out = WaveToChar(WinPath);
	strcpy(Path, Out);
#endif 
	return Ret;
}

size_t WaveGetUsedMemory()
{
	PROCESS_MEMORY_COUNTERS Info;
	GetProcessMemoryInfo(GetCurrentProcess(), &Info, sizeof(Info));
	
	return (size_t)Info.WorkingSetSize;
}

void WaveResetToLastPath()
{
	SetCurrentDirectory(LastPath);
}

void WaveSetPath(char* Path)
{
#ifdef UNICODE
	SetCurrentDirectory(WaveToLPWSTR(Path));

#else
	SetCurrentDirectory(Path);
#endif 
}

void WaveGetPath(char* OutPath)
{
#ifdef UNICODE
	WCHAR Path[WAVE_MAX_LENGTH];
	GetCurrentDirectory(WAVE_MAX_LENGTH, LastPath);
	strcpy(OutPath, WaveToChar(Path));
#else
	GetCurrentDirectory(WAVE_MAX_LENGTH, OutPath);
#endif 
}

void WavePlayAudio()
{
	
}

#elif __linux__
char LastPath[WAVE_MAX_LENGTH];

int32_t WaveOpenFileDialog(char* Path, int MultiSelect, unsigned short* Offset, const char* FileExtensions)
{
	const char ZenityPath[] = "/usr/bin/zenity";
  
	sprintf(Call, "%s  --file-selection --modal --title=\"%s\" ", ZenityPath, "Open");

	FILE *File = popen(Call, "r");
	if (!File)
		return 0;

	fgets(Path, WAVE_MAX_LENGTH, File);

	pclose(File);

	return 1;
}

int32_t WaveSaveFileDialog(char* Path, const char* FileExtensions)
{

}

size_t WaveGetUsedMemory()
{
	long Mem = 0L;

	FILE* File = fopen("/proc/self/statm", "r");
	if (!File)
		return (size_t)0L;

	if (fscanf(File, "%*s%ld", &Mem) != 1)
	{
		fclose(File);
		return (size_t)0L;
	}
	fclose(File);

	return (size_t)Mem * (size_t)sysconf(_SC_PAGESIZE);
}

void WaveResetToLastPath()
{
	
}

void WaveSetPath(char* Path)
{

}
#elif __APPLE__
#include <unistd.h>
#include <sys/resource.h>
#include <mach/mach.h>

char LastPath[WAVE_MAX_LENGTH];

int32_t WaveOpenFileDialog(char* Path, int MultiSelect, unsigned short* Offset, const char* FileExtensions)
{
	
}

int32_t WaveSaveFileDialog(char* Path, const char* FileExtensions)
{

}

size_t WaveGetUsedMemory()
{
	struct mach_task_basic_info Info;
	mach_msg_type_number_t InfoCount = MACH_TASK_BASIC_INFO_COUNT;

	if (task_info(mach_task_self(), MACH_TASK_BASIC_INFO, (task_info_t)&Info, &InfoCount) != KERN_SUCCESS)
		return (size_t)0L;

	return (size_t)Info.resident_size;
}

void WaveResetToLastPath()
{

}

void WaveSetPath(char* Path)
{

}

#endif
