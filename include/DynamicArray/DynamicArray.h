#ifndef DYNAMIC_ARRAY_IMPLEMENTATION
#define DYNAMIC_ARRAY_IMPLEMENTATION
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#define DYNAMIC_ARRAY_BLOCK_COUNT 32//32
#define DYNAMIC_ARRAY_MAX_GARBAGE_COUNT 16
#define DYNAMIC_ARRAY_MAX_NAME_LENGTH 32
//#define DYNAMIC_ARRAY_PRINT_DEBUG_INFO

typedef struct
{
	void* Data;
	size_t SizeAllocated;
	size_t Size;
	size_t SizeOfBlock;
	char Name[DYNAMIC_ARRAY_MAX_NAME_LENGTH];
} DynamicArray;

bool DynamicArrayCreate(DynamicArray* Arr, size_t SizeOfBlock, const char* Name)
{
	Arr->SizeAllocated = DYNAMIC_ARRAY_BLOCK_COUNT;
	Arr->SizeOfBlock = SizeOfBlock;
	Arr->Data = malloc(Arr->SizeAllocated * Arr->SizeOfBlock);
	Arr->Size = 0;
	strncpy(Arr->Name, Name, DYNAMIC_ARRAY_MAX_NAME_LENGTH);
	Arr->Name[DYNAMIC_ARRAY_MAX_NAME_LENGTH - 1] = '\0';

	if (Arr->Data == NULL)
	{
		Arr->SizeAllocated = 0;
		printf("[Dynamic Array]: Failed to init: %s\n", Name);
		return false;
	}
	
	return true;
}

bool DynamicArrayReplace(DynamicArray* Array, void* Data, size_t Index)
{
	if (Index > Array->Size)
	{
		printf("[Dynamic Array]: Failed to replace at: %zu\n", Index);
		return false;
	}

	char* DataPTR = (char*)Array->Data;
	DataPTR += Index * Array->SizeOfBlock;

	memcpy(DataPTR, Data, Array->SizeOfBlock);

	return true;
}

bool DynamicArrayPush(DynamicArray* Array, void* Data)
{
	if (Array->SizeAllocated <= Array->Size)
	{
		Array->SizeAllocated += DYNAMIC_ARRAY_BLOCK_COUNT;
		void* TempData = realloc(Array->Data, Array->SizeAllocated * Array->SizeOfBlock);

		if (!TempData)
		{
			printf("[Dynamic Array]: Failed to resize: %s, old size: %zu, new size: %zu\n", Array->Name, (Array->SizeAllocated - DYNAMIC_ARRAY_BLOCK_COUNT), Array->SizeAllocated);
			return false;
		}

#ifdef DYNAMIC_ARRAY_PRINT_DEBUG_INFO
		printf("[Dynamic Array Debug]: Push resize: %zu\n", Array->SizeAllocated);
#endif

		Array->Data = TempData;
	}

	char* DataPTR = (char*)Array->Data;
	DataPTR += Array->Size * Array->SizeOfBlock;

	if (Data)
		memcpy(DataPTR, Data, Array->SizeOfBlock);

	Array->Size++;

	return true;
}

bool DynamicArrayPop(DynamicArray* Array, size_t Index)
{
//	char* DataPTR = (char*)Array->Data;
//	DataPTR += Index * Array->SizeOfBlock;

	if (Array->Size == 0)
		return false;

	for (size_t i = Index; i < Array->Size - 1; i++)
	{
		char* DataDst = ((char*)Array->Data) + (i * Array->SizeOfBlock);
		char* DataSrc = ((char*)Array->Data) + ((i + 1) * Array->SizeOfBlock);

		memcpy(DataDst, DataSrc, Array->SizeOfBlock);
	}

	Array->Size--;

	if (Array->Size < Array->SizeAllocated - DYNAMIC_ARRAY_MAX_GARBAGE_COUNT)
	{
		Array->SizeAllocated -= DYNAMIC_ARRAY_MAX_GARBAGE_COUNT;
		void* TempData = realloc(Array->Data, Array->SizeAllocated * Array->SizeOfBlock);

		if (!TempData)
		{
			printf("[Dynamic Array]: Failed to resize: %s, old size: %zu, new size: %zu\n", Array->Name, (Array->SizeAllocated + DYNAMIC_ARRAY_MAX_GARBAGE_COUNT), Array->SizeAllocated);
			return false;
		}

#ifdef DYNAMIC_ARRAY_PRINT_DEBUG_INFO
		printf("[Dynamic Array Debug]: Pop resize: %zu\n", Array->SizeAllocated);
#endif

		Array->Data = TempData;
	}

	return true;
}

void DynamicArrayClear(DynamicArray* Array)
{
	Array->Size = 0;
}

void* DynamicArrayGetAt(DynamicArray* Array, size_t Index)
{
	if (Index > Array->Size)
	{
		printf("[Dynamic Array]: \"%s\" Index out of range: %zu, max size is: %zu\n", Array->Name, Index, Array->Size);
		return NULL;
	}

	char* DataDst = ((char*)Array->Data) + (Index * Array->SizeOfBlock);
	return DataDst;
}

void DynamicArrayDestroy(DynamicArray* Array)
{
	free(Array->Data);
	memset(Array, 0, sizeof(DynamicArray));
}
#endif