#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#define CMA_BLOCK_SIZE 128
#define CMA_MAX_GARBAGE_COUNT 4

typedef enum
{
	ICMA_DATA_STATE_UNUSED = 1,
	ICMA_DATA_STATE_USED = 2,
	ICMA_DATA_STATE_ALLOCATED = 4,
	ICMA_DATA_STATE_UNALLOCATED = 8,
} ICMA_DataState;

typedef struct
{
	char State;
	void* Data;
} CMA_Memory;

typedef struct
{
	size_t Size;
	size_t AllocateSize;
	size_t MemSize;
	CMA_Memory* Mem;
	size_t GarbageCount;
} CMA_MemoryZone;

void ICMA_Resize(size_t Start, CMA_MemoryZone* Zone)
{
	for (size_t i = Start; i < Zone->AllocateSize; i++)
	{
		Zone->Mem[i].Data = malloc(Zone->MemSize);
		Zone->Mem[i].State = ICMA_DATA_STATE_UNUSED | ICMA_DATA_STATE_ALLOCATED;
	}
}

CMA_MemoryZone CMA_Create(size_t Size)
{
	CMA_MemoryZone Zone;
	Zone.Size = 0;
	Zone.AllocateSize = CMA_BLOCK_SIZE;
	Zone.MemSize = Size;
	Zone.Mem = (CMA_Memory*)malloc(CMA_BLOCK_SIZE * sizeof(CMA_Memory));
	ICMA_Resize(0, &Zone);
	Zone.GarbageCount = 0;
	return Zone;
}

void CMA_Destroy(CMA_MemoryZone* Zone)
{
	for (size_t i = 0; i < Zone->AllocateSize; i++)
	{
		if (Zone->Mem[i].State & ICMA_DATA_STATE_ALLOCATED)
			free(Zone->Mem[i].Data);
		Zone->Mem[i].Data = NULL;
		Zone->Mem[i].State = 0;
	}
	free(Zone->Mem);
	Zone->Mem = NULL;
	Zone->Size = 0;
	Zone->AllocateSize = 0;
	Zone->GarbageCount = 0;
}

size_t CMA_Push(CMA_MemoryZone* Zone, void* Data)
{
	if (Zone->Size >= Zone->AllocateSize)
	{
	//	printf("wtf\n");
		Zone->Mem = (CMA_Memory*)realloc(Zone->Mem, (Zone->AllocateSize + CMA_BLOCK_SIZE) * sizeof(CMA_Memory));
		Zone->AllocateSize += CMA_BLOCK_SIZE;
		ICMA_Resize(Zone->AllocateSize - CMA_BLOCK_SIZE, Zone);
	}


	for (size_t i = 0; i < Zone->Size; i++)
	{
		if (Zone->Mem[i].State & ICMA_DATA_STATE_UNUSED &&
			Zone->Mem[i].State & ICMA_DATA_STATE_ALLOCATED)
		{
			Zone->Mem[i].State = ICMA_DATA_STATE_USED | ICMA_DATA_STATE_ALLOCATED;
			memcpy(Zone->Mem[i].Data, Data, Zone->MemSize);
			return i;
		}
		else if (Zone->Mem[i].State & ICMA_DATA_STATE_UNUSED &&
			Zone->Mem[i].State & ICMA_DATA_STATE_UNALLOCATED)
		{
			Zone->Mem[i].State = ICMA_DATA_STATE_USED | ICMA_DATA_STATE_ALLOCATED;
			Zone->Mem[i].Data = malloc(Zone->MemSize);
			memcpy(Zone->Mem[i].Data, Data, Zone->MemSize);
			return i;
		}
	}

	if (Zone->Mem[Zone->Size].State & ICMA_DATA_STATE_UNUSED &&
		Zone->Mem[Zone->Size].State & ICMA_DATA_STATE_ALLOCATED)
	{
		Zone->Mem[Zone->Size].State = ICMA_DATA_STATE_USED | ICMA_DATA_STATE_ALLOCATED;
		memcpy(Zone->Mem[Zone->Size].Data, Data, Zone->MemSize);
	}
	else if (Zone->Mem[Zone->Size].State & ICMA_DATA_STATE_UNUSED &&
		Zone->Mem[Zone->Size].State & ICMA_DATA_STATE_UNALLOCATED)
	{
		Zone->Mem[Zone->Size].State = ICMA_DATA_STATE_USED | ICMA_DATA_STATE_ALLOCATED;
		Zone->Mem[Zone->Size].Data = malloc(Zone->MemSize);
		memcpy(Zone->Mem[Zone->Size].Data, Data, Zone->MemSize);
	}

	Zone->Size++;
	return Zone->Size - 1;
}

size_t CMA_GetSize(CMA_MemoryZone* Zone)
{
	return Zone->Size;
}

void* CMA_GetAt(CMA_MemoryZone* Zone, size_t Index)
{
	if (Zone->Mem != NULL && Index < Zone->Size && Zone->Mem[Index].State & ICMA_DATA_STATE_USED)
		return Zone->Mem[Index].Data;

	return NULL;
}

void CMA_Pop(CMA_MemoryZone* Zone, size_t Index)
{
	Zone->Mem[Index].State = ICMA_DATA_STATE_UNUSED | ICMA_DATA_STATE_ALLOCATED;

	for (size_t i = Zone->Size - 1; i > 0; i--)
	{
		if (Zone->Mem[i].State & ICMA_DATA_STATE_USED)
			break;

		free(Zone->Mem[i].Data);
		Zone->Mem[i].Data = NULL;
		Zone->Mem[i].State = ICMA_DATA_STATE_UNUSED | ICMA_DATA_STATE_UNALLOCATED;
		Zone->Size--;
	}

	if (Zone->GarbageCount >= CMA_MAX_GARBAGE_COUNT)
	{
		Zone->GarbageCount = 0;
		Zone->AllocateSize = Zone->Size;
		Zone->Mem = (CMA_Memory*)realloc(Zone->Mem, Zone->AllocateSize * sizeof(CMA_Memory));
	}

	Zone->GarbageCount++;
}

uint32_t CMA_Compare(CMA_MemoryZone* Zone0, size_t Index0, CMA_MemoryZone* Zone1, size_t Index1)
{
	void* Buf0 = CMA_GetAt(Zone0, Index0);
	void* Buf1 = CMA_GetAt(Zone1, Index1);

	if (Buf0 == NULL || Buf1 == NULL)
		return 0;

	if (memcmp(Buf0, Buf1, Zone0->MemSize) == 0)
		return 1;

	return 0;
}