#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#define CMA_BLOCK_SIZE 64
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
	char Name[64];//For debugging
	size_t Size;
	size_t AllocateSize;
	size_t MemSize;
	CMA_Memory* Mem;
	size_t PopCount;
	size_t PushCountNew;
	size_t PushCountReused;
} CMA_MemoryZone;

void ICMA_Resize(size_t Start, CMA_MemoryZone* Zone)
{
	for (size_t i = Start; i < Zone->AllocateSize; i++)
	{
		Zone->Mem[i].Data = malloc(Zone->MemSize);
		if (!Zone->Mem[i].Data)
		{
			Zone->AllocateSize = i;
			printf("CMA Error: Failed to allocate data for zone: %s\n", Zone->Name);
			return;
		}
		Zone->Mem[i].State = ICMA_DATA_STATE_UNUSED | ICMA_DATA_STATE_ALLOCATED;
	}
}

CMA_MemoryZone CMA_Create(size_t Size, const char* Name)
{
	CMA_MemoryZone Zone;
	strncpy(Zone.Name, Name, 64);
	Zone.Name[63] = '\0';
	Zone.Size = 0;
	Zone.AllocateSize = CMA_BLOCK_SIZE;
	Zone.MemSize = Size;
	Zone.Mem = (CMA_Memory*)malloc(CMA_BLOCK_SIZE * sizeof(CMA_Memory));
	if (!Zone.Mem)
	{
		printf("CMA Error: Failed to allocate memory for zone: %s\n", Name);
		return Zone;
	}
	ICMA_Resize(0, &Zone);
	Zone.PopCount = 0;
	Zone.PushCountNew = 0;
	Zone.PushCountReused = 0;
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
	Zone->MemSize = 0;
	Zone->Size = 0;
	Zone->AllocateSize = 0;
	Zone->PopCount = 0;
	Zone->PushCountNew = 0;
	Zone->PushCountReused = 0;
}

size_t CMA_Push(CMA_MemoryZone* Zone, void* Data)
{
	if (Zone->Mem == NULL)
	{
		printf("CMA Error: Failed to find zone meory: %s\n", Zone->Name);
		return 0;
	}
		
	//Check for unsued slots that can be used
	for (size_t i = 0; i < Zone->Size; i++)
	{
		if (Zone->Mem[i].State & ICMA_DATA_STATE_UNUSED &&
			Zone->Mem[i].State & ICMA_DATA_STATE_ALLOCATED)
		{
			Zone->Mem[i].State = ICMA_DATA_STATE_USED | ICMA_DATA_STATE_ALLOCATED;
			memcpy(Zone->Mem[i].Data, Data, Zone->MemSize);
			Zone->PushCountReused++;
			return i;
		}
		else if (Zone->Mem[i].State & ICMA_DATA_STATE_UNUSED &&
				 Zone->Mem[i].State & ICMA_DATA_STATE_UNALLOCATED)
		{
			Zone->Mem[i].State = ICMA_DATA_STATE_USED | ICMA_DATA_STATE_ALLOCATED;
			Zone->Mem[i].Data = malloc(Zone->MemSize);
			memcpy(Zone->Mem[i].Data, Data, Zone->MemSize);
			Zone->PushCountReused++;
			return i;
		}
	}

	if (Zone->Size >= Zone->AllocateSize)
	{
	//	printf("CMA: Resized memory zone\n");
		CMA_Memory* Memory = (CMA_Memory*)realloc(Zone->Mem, (Zone->AllocateSize + CMA_BLOCK_SIZE) * sizeof(CMA_Memory));
		if (Memory == NULL)
			printf("CMA Error: Failed to resize memory zone: %s\n", Zone->Name);
		else
		{
			Zone->Mem = Memory;
			Zone->AllocateSize += CMA_BLOCK_SIZE;
			ICMA_Resize(Zone->AllocateSize - CMA_BLOCK_SIZE, Zone);
		}

		if (Zone->Size >= Zone->AllocateSize)
		{
			printf("CMA Error: Failed to push your data: %s\n", Zone->Name);
			return SIZE_MAX;
		}
	}

	if (Zone->Mem[Zone->Size].State & ICMA_DATA_STATE_UNUSED &&
		Zone->Mem[Zone->Size].State & ICMA_DATA_STATE_ALLOCATED)
	{
		Zone->Mem[Zone->Size].State = ICMA_DATA_STATE_USED | ICMA_DATA_STATE_ALLOCATED;
		memcpy(Zone->Mem[Zone->Size].Data, Data, Zone->MemSize);
		Zone->PushCountNew++;
	}
	else if (Zone->Mem[Zone->Size].State & ICMA_DATA_STATE_UNUSED &&
			 Zone->Mem[Zone->Size].State & ICMA_DATA_STATE_UNALLOCATED)
	{
		Zone->Mem[Zone->Size].State = ICMA_DATA_STATE_USED | ICMA_DATA_STATE_ALLOCATED;
		Zone->Mem[Zone->Size].Data = malloc(Zone->MemSize);
		if (!Zone->Mem[Zone->Size].Data)
		{
			printf("CMA Error: Failed to allocate data for your push data: %s\n", Zone->Name);
			return SIZE_MAX;
		}
		memcpy(Zone->Mem[Zone->Size].Data, Data, Zone->MemSize);
		Zone->PushCountNew++;
	}

	return Zone->Size++;
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
	/*
	if (Zone->PopCount >= CMA_MAX_GARBAGE_COUNT)
	{
		char ReduceSize = 1;
		for (size_t i = Zone->Size - 1; i > 0; i--)
		{
		//	if (i <= CMA_BLOCK_SIZE)
		//		break;

			if (Zone->Mem[i].State & ICMA_DATA_STATE_USED)
				ReduceSize = 0;

			if (Zone->Mem[i].State & ICMA_DATA_STATE_UNUSED &&
				Zone->Mem[i].State & ICMA_DATA_STATE_ALLOCATED)
			{
				printf("CMA: Freed poped memory\n");
				free(Zone->Mem[i].Data);
				Zone->Mem[i].Data = NULL;
				Zone->Mem[i].State = ICMA_DATA_STATE_UNUSED | ICMA_DATA_STATE_UNALLOCATED;
			}

			if (ReduceSize)
				Zone->Size--;
		}

		Zone->PopCount = 0;

	//	if (Zone->PopCount > 0)
	//		Zone->PopCount--;
	//	if (Zone->PushCountNew > 0)
	//		Zone->PushCountNew--;
		
	//	Zone->PushCountReused = 0;
		if (Zone->AllocateSize > CMA_BLOCK_SIZE && Zone->AllocateSize != Zone->Size)
		{
			Zone->AllocateSize = Zone->Size;
			Zone->Mem = (CMA_Memory*)realloc(Zone->Mem, Zone->AllocateSize * sizeof(CMA_Memory));
			printf("CMA: Resized poped memory zone\n");
		}
	}
	*/

	

	if (Zone->PopCount >= CMA_MAX_GARBAGE_COUNT)
	{
		Zone->PopCount = 0;

		for (size_t i = Zone->Size - 1; i > 0; i--)
		{
			if (Zone->Mem[i].State & ICMA_DATA_STATE_USED)
				break;

			free(Zone->Mem[i].Data);
			Zone->Mem[i].Data = NULL;
			Zone->Mem[i].State = ICMA_DATA_STATE_UNUSED | ICMA_DATA_STATE_UNALLOCATED;
			Zone->Size--;
		}

		if (Zone->AllocateSize != Zone->Size)
		{
			
			CMA_Memory* Memory = (CMA_Memory*)realloc(Zone->Mem, Zone->Size * sizeof(CMA_Memory));
			if (!Memory)
			{
				printf("CMA Error: Failed to srink memory zone: %s\n", Zone->Name);
			}
			else
			{
				Zone->Mem = Memory;
				Zone->AllocateSize = Zone->Size;
			}				
		}
	}

	/*
	for (size_t i = Zone->Size - 1; i > 0; i--)
	{
		if (Zone->Mem[i].State & ICMA_DATA_STATE_USED)
			break;

		free(Zone->Mem[i].Data);
		Zone->Mem[i].Data = NULL;
		Zone->Mem[i].State = ICMA_DATA_STATE_UNUSED | ICMA_DATA_STATE_UNALLOCATED;
		Zone->Size--;
	}

	if (Zone->PopCount >= CMA_MAX_GARBAGE_COUNT)
	{
		Zone->PopCount = 0;
		if (Zone->AllocateSize != Zone->Size)
		{
			Zone->AllocateSize = Zone->Size;
			Zone->Mem = (CMA_Memory*)realloc(Zone->Mem, Zone->AllocateSize * sizeof(CMA_Memory));
		}		
	}
	*/

	Zone->PopCount++;
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