#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#define CMA_BLOCK_SIZE 256

typedef struct
{
	size_t Size;
	void* Data;
} CMA_Memory;

typedef struct
{
	size_t Size;
	CMA_Memory* Mem;
} CMA_MemoryZone;

CMA_MemoryZone CMA_Create()
{
	CMA_MemoryZone Zone;
	Zone.Size = 0;
	Zone.Mem = (CMA_Memory*)calloc(CMA_BLOCK_SIZE, sizeof(CMA_Memory));
	return Zone;
}

void CMA_Destroy(CMA_MemoryZone* Zone)
{
	for (size_t i = 0; i < Zone->Size; i++)
	{
		free(Zone->Mem[i].Data);
		Zone->Mem[i].Data = NULL;
	}
	free(Zone->Mem);
	Zone->Mem = NULL;
}

size_t CMA_Push(CMA_MemoryZone* Mem, size_t Size, void* Data)
{
	if (Mem->Size % CMA_BLOCK_SIZE == 0)
		Mem->Mem = (CMA_Memory*)realloc(Mem->Mem, (Mem->Size + CMA_BLOCK_SIZE) * sizeof(CMA_Memory));

	for (size_t i = 0; i < Mem->Size; i++)
	{
		if (Mem->Mem[i].Data == NULL && Mem->Mem[i].Size == 0)
		{
			free(Mem->Mem[i].Data);
			Mem->Mem[i].Size = Size;
			Mem->Mem[i].Data = malloc(Size);
			memcpy(Mem->Mem[i].Data, Data, Size);
			return i;
		}
	}
	Mem->Mem[Mem->Size].Data = malloc(Size);
	memcpy(Mem->Mem[Mem->Size].Data, Data, Size);

	Mem->Size++;
	return Mem->Size - 1;
}

size_t CMA_GetSize(CMA_MemoryZone* Mem)
{
	if (Mem->Mem != NULL)
		return Mem->Size;

	return 0;
}

void* CMA_GetAt(CMA_MemoryZone* Mem, size_t Index)
{
	if (Mem->Mem != NULL && Index <= Mem->Size)
		return Mem->Mem[Index].Data;

	return NULL;
}

void CMA_Pop(CMA_MemoryZone* Mem, size_t Index)
{
	Mem->Mem[Index].Data = NULL;
	Mem->Mem[Index].Size = 0;

	for (uint32_t i = CMA_GetSize(Mem) - 1; i > 0; i--)
	{
		if (CMA_GetAt(Mem, i) != NULL)
			break;

		free(Mem->Mem[i].Data);
		Mem->Mem[i].Data = NULL;
		Mem->Mem[i].Size = 0;
		Mem->Size--;
	}
}