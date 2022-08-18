#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#ifdef _WIN32
#include <Windows.h>
#include <psapi.h>
#include <Commdlg.h>
HWND WaveHwnd;
char LastPath[4096];

int32_t WaveOpenFileDialog(char* Path, const char* FileExtensions)
{
//	char FinalExtensions[2048];
//	sprintf(FinalExtensions, "%s\0%s\0", FileTypes, FileExtensions);
//	printf("%s\n", FinalExtensions);
//	printf("%s\n", OBJDAE);

	OPENFILENAME ofn;

	ZeroMemory(&ofn, sizeof(OPENFILENAME));

	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = WaveHwnd;
	ofn.lpstrFile = Path;
	ofn.lpstrFile[0] = '\0';
	ofn.nMaxFile = 1000;
	ofn.lpstrFilter = FileExtensions;
	ofn.nFilterIndex = 1;
	
//	char CurrentPath[4096];
	GetCurrentDirectory(4096, LastPath);
	int32_t Ret = GetOpenFileName(&ofn);
//	SetCurrentDirectory(CurrentPath);

	return Ret;
}

int32_t WaveSaveFileDialog(char* Path, const char* FileExtensions)
{
	OPENFILENAME ofn;

	ZeroMemory(&ofn, sizeof(OPENFILENAME));

	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = WaveHwnd;
	ofn.lpstrFile = Path;
	ofn.lpstrFile[0] = '\0';
	ofn.nMaxFile = 1000;
	ofn.lpstrFilter = FileExtensions;
	ofn.nFilterIndex = 1;

	
	GetCurrentDirectory(4096, LastPath);
	int32_t Ret = GetSaveFileName(&ofn);
//	SetCurrentDirectory(CurrentPath);

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

#elif __linux__
int32_t WaveOpenFileDialog(char* Path, const char* FileExtensions)
{
	const char ZenityPath[] = "/usr/bin/zenity";
  
	sprintf(Call, "%s  --file-selection --modal --title=\"%s\" ", ZenityPath, "Open");

	FILE *File = popen(Call, "r");
	if (!File)
		return 0;

	fgets(Path, 4096, File); 

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

#elif __APPLE__
#include <unistd.h>
#include <sys/resource.h>
#include <mach/mach.h>

size_t WaveGetUsedMemory()
{
	struct mach_task_basic_info Info;
	mach_msg_type_number_t InfoCount = MACH_TASK_BASIC_INFO_COUNT;

	if (task_info(mach_task_self(), MACH_TASK_BASIC_INFO, (task_info_t)&Info, &InfoCount) != KERN_SUCCESS)
		return (size_t)0L;

	return (size_t)Info.resident_size;
}

#endif
