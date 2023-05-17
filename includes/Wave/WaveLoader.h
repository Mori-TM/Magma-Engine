#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdarg.h>
#include <ctype.h>
#include <float.h>
#include <math.h>
#ifndef WAVE_NO_MULTI_THREDED_LOADING
#if _HAS_CXX23
#include <stdatomic.h>
atomic_int WaveLoadingStatus;
#else
int WaveLoadingStatus;
#endif
#else
int WaveLoadingStatus;
#endif // WAVE_NO_MULTI_THREDED_LOADING

#define WAVE_MAX_ALLOCATION_SIZE 100000//388608

#ifndef WAVE_BASIC_MATH
#define WAVE_BASIC_MATH

#define WAVEMAX(x, y) (((x) > (y)) ? (x) : (y))
#define WAVEMIN(x, y) (((x) < (y)) ? (x) : (y))

typedef struct
{
	float x;
	float y;
	float z;
} WaveVec3;

float WaveFastInverseSqrt(float number)
{
	long i;
	float x2, y;
	const float threehalfs = 1.5f;

	x2 = number * 0.5f;
	y = number;
	i = *(long*)&y;              // evil floating point bit level hacking
	i = 0x5f3759df - (i >> 1);   // what the fuck?
	y = *(float*)&i;
	y = y * (threehalfs - (x2 * y * y));   // 1st iteration
//	y = y * (threehalfs - (x2 * y * y));   // 2nd iteration, this can be removed - it's for more accuracy

	return y;
}

extern inline WaveVec3 WaveNormalize(WaveVec3 v)
{
	float length = WaveFastInverseSqrt(v.x * v.x + v.y * v.y + v.z * v.z);
	v.x *= length;
	v.y *= length;
	v.z *= length;

	return v;
}

extern inline WaveVec3 WaveCross(WaveVec3 a, WaveVec3 b)
{
	WaveVec3 r;
	r.x = a.y * b.z - a.z * b.y;
	r.y = a.z * b.x - a.x * b.z;
	r.z = a.x * b.y - a.y * b.x;

	return r;
}

extern inline float WaveDot(WaveVec3 a, WaveVec3 b)
{
	return a.x * b.x + a.y * b.y + a.z * b.z;
}

extern inline WaveVec3 WaveAdd(WaveVec3 a, WaveVec3 b)
{
	WaveVec3 r;
	r.x = a.x + b.x;
	r.y = a.y + b.y;
	r.z = a.z + b.z;

	return r;
}

extern inline WaveVec3 WaveSub(WaveVec3 a, WaveVec3 b)
{
	WaveVec3 r;
	r.x = a.x - b.x;
	r.y = a.y - b.y;
	r.z = a.z - b.z;

	return r;
}

extern inline WaveVec3 WaveMul(WaveVec3 a, WaveVec3 b)
{
	WaveVec3 r;
	r.x = a.x * b.x;
	r.y = a.y * b.y;
	r.z = a.z * b.z;

	return r;
}

extern inline WaveVec3 WaveDiv(WaveVec3 a, WaveVec3 b)
{
	WaveVec3 r;
	r.x = a.x / b.x;
	r.y = a.y / b.y;
	r.z = a.z / b.z;

	return r;
}

extern inline float WaveLength(WaveVec3 v)
{
	return sqrtf(v.x * v.x + v.y * v.y + v.z * v.z);
}

extern inline float WaveInvLength(WaveVec3 v)
{
	return WaveFastInverseSqrt(v.x * v.x + v.y * v.y + v.z * v.z);
}

extern inline WaveVec3 WaveVec3f(float v)
{
	WaveVec3 r;
	r.x = v;
	r.y = v;
	r.z = v;

	return r;
}
#endif

typedef enum
{
	WAVE_NONE = 0,
	WAVE_GEN_UVS = 1,
	WAVE_GEN_NORMALS = 2,
	WAVE_FLIP_UVS = 4,
	WAVE_LOAD_MATERIAL = 8,
	WAVE_GEN_INDICES = 16,
	WAVE_GEN_SMOOTH_NORMALS = 32,
	WAVE_FORCE_GEN_NORMALS = 64,
	WAVE_REMOVE_REDUNDANT_MATERIALS = 128,
	WAVE_MATERIAL_USE_MODEL_PATH = 256,
	WAVE_PRINT_DEBUG_INOFS = 512,
	WAVE_MAX = 0xffffffffui32,
} WaveSettings;

typedef enum
{
	WAVE_STATUS_OPENED = 1,
	WAVE_STATUS_CLOSED = 2,
	WAVE_STATUS_START_PPP = 3,
	WAVE_STATUS_GEN_NORMALS = 4,
	WAVE_STATUS_GEN_SMOOTH_NORMALS = 5,
	WAVE_STATUS_GEN_INDICES = 6,
	WAVE_STATUS_REMOVE_REDUDANT_MATERIALS = 7,
	WAVE_STATUS_PARSE_MODEL = 8,
	WAVE_STATUS_PARSE_MATERIAL = 9,
	WAVE_STATUS_ALLOCATE_VERTICES = 10,
	WAVE_STATUS_REDUCE_SIZE = 11,
	WAVE_STATUS_FINISHED = 12,
} WaveLoadingStatusOptions;

typedef struct
{
	char MaterialName[2048];

	WaveVec3 AmbientColor;
	WaveVec3 DiffuseColor;
	WaveVec3 SpecularColor;
	float SpecularExponent;
	float Dissolve;
	float Emissive;
	char AmbientTexture[4096];
	char DiffuseTexture[4096];
	char SpecularTexture[4096];
	char EmissiveTexture[4096];
	char AlphaTexture[4096];
	char BumpTexture[4096];
	char DisplacmentTexture[4096];
	char HeightTexture[4096];
	//PBR-Extension
	float Roughness;
	float Metallic;
	float Sheen;
	char RoughnessTexture[4096];
	char MetallicTexture[4096];
	char SheenTexture[4096];
	char NormalTexture[4096];
} WaveModelMaterial;

typedef struct
{
	uint32_t VertexIndex;
	uint32_t NewVertexIndex;

	WaveVec3 Vertices;
	WaveVec3 TexCoords;
	WaveVec3 Normals;
	WaveVec3 VertexColor;
} WaveVertexData;

typedef struct
{
	WaveVec3 Min;
	WaveVec3 Max;
} WaveAABBData;

typedef struct
{
	uint32_t VertexSize;
	uint32_t IndexSize;

	WaveModelMaterial* Material;

	uint32_t VertexCount;
	WaveVertexData* Vertices;

	uint32_t IndexCount;
	uint32_t* Indices;

	WaveAABBData AABB;
} WaveMeshData;

typedef struct
{
	uint32_t MaterialCount;
	WaveModelMaterial* Materials;

	uint32_t MeshCount;
	WaveMeshData* Meshes;
} WaveModelData;

typedef struct
{
	uint32_t Length;
	char* Line;
} WaveLine;

char* WaveLoadFile(const char* Path, size_t* Length)
{
	FILE* File = fopen(Path, "rb");
	if (!File)
		return NULL;

	fseek(File, 0, SEEK_END);
	size_t FileSize = ftell(File);
	fseek(File, 0, SEEK_SET);

	char* Buffer = (char*)malloc(FileSize + 1);
	fread(Buffer, FileSize, 1, File);
	Buffer[FileSize] = '\0';
	fclose(File);

	*Length = FileSize + 1;

	return Buffer;
}

WaveLine* WaveGetLinesFromBuffer(size_t Length, char* Buffer, uint32_t* LineCount)
{
	uint32_t Count = 0;
	for (size_t i = 0; i < Length; i++)
		if (Buffer[i] == '\n')
			Count++;

	WaveLine* Lines = (WaveLine*)calloc(Count, sizeof(WaveLine));

	Count = 0;
	uint32_t LineLength = 0;
	for (size_t i = 0; i < Length; i++)
	{
		LineLength++;
		if (Buffer[i] == '\n')
		{
			Lines[Count].Length = LineLength;
			Lines[Count].Line = (char*)calloc(LineLength, sizeof(char));
			memcpy(Lines[Count].Line, Buffer + i - LineLength + 1, LineLength - 1);
			Lines[Count].Line[LineLength - 1] = '\0';

			LineLength = 0;
			Count++;
		}
	}

	*LineCount = Count;

	return Lines;
}

extern inline char* WaveStrtok(char* Str, const char Delimiter)
{
	while (*Str != Delimiter && *Str != '\0' && *Str != '\n') Str++;

	return Str + 1;
}

void WaveGenUVs(WaveVertexData* Vertex)
{
	float MinX = 1000000000.0;
	float MaxX = -1000000000.0;

	float MinY = 1000000000.0;
	float MaxY = -1000000000.0;

	float TempUVsqrt = sqrtf(Vertex->Vertices.x * Vertex->Vertices.x + Vertex->Vertices.y * Vertex->Vertices.y + Vertex->Vertices.z * Vertex->Vertices.z);
	Vertex->TexCoords.x = sinf(cosf(Vertex->Vertices.x / TempUVsqrt));
	Vertex->TexCoords.y = sinf(sinf(Vertex->Vertices.y / TempUVsqrt));
}

void WaveGenSmoothNormals(WaveModelData* ModelData)
{
	WaveLoadingStatus = WAVE_STATUS_GEN_SMOOTH_NORMALS;
	for (uint32_t j = 0; j < ModelData->MeshCount; j++)
	{
		WaveMeshData* Data = &ModelData->Meshes[j];

		if (Data->IndexCount == 0)
			return;

		for (uint32_t i = 0; i < Data->VertexCount; i++)
		{
			Data->Vertices[i].Normals.x = 0.0;
			Data->Vertices[i].Normals.y = 0.0;
			Data->Vertices[i].Normals.z = 0.0;
		}

		for (uint32_t i = 0; i < Data->IndexCount; i += 3)
		{
			WaveVec3* V0 = &Data->Vertices[Data->Indices[i]].Vertices;
			WaveVec3* V1 = &Data->Vertices[Data->Indices[i + 1]].Vertices;
			WaveVec3* V2 = &Data->Vertices[Data->Indices[i + 2]].Vertices;

			WaveVec3 A = WaveSub(*V2, *V1);
			WaveVec3 B = WaveSub(*V0, *V1);
			WaveVec3 Normal = WaveCross(A, B);

			Data->Vertices[Data->Indices[i]].Normals.x += Normal.x;
			Data->Vertices[Data->Indices[i]].Normals.y += Normal.y;
			Data->Vertices[Data->Indices[i]].Normals.z += Normal.z;

			Data->Vertices[Data->Indices[i + 1]].Normals.x += Normal.x;
			Data->Vertices[Data->Indices[i + 1]].Normals.y += Normal.y;
			Data->Vertices[Data->Indices[i + 1]].Normals.z += Normal.z;

			Data->Vertices[Data->Indices[i + 2]].Normals.x += Normal.x;
			Data->Vertices[Data->Indices[i + 2]].Normals.y += Normal.y;
			Data->Vertices[Data->Indices[i + 2]].Normals.z += Normal.z;
		}

		for (uint32_t i = 0; i < Data->VertexCount; i++)
			Data->Vertices[i].Normals = WaveNormalize(Data->Vertices[i].Normals);
	}
}

void WaveGenNormals(WaveModelData* ModelData)
{
	WaveLoadingStatus = WAVE_STATUS_GEN_NORMALS;
	for (uint32_t j = 0; j < ModelData->MeshCount; j++)
	{
		WaveMeshData* Data = &ModelData->Meshes[j];

		for (uint32_t i = 0; i < Data->VertexCount - 3; i += 3)
		{
			WaveVec3 N = WaveNormalize(WaveCross(WaveSub(Data->Vertices[i + 1].Vertices, Data->Vertices[i].Vertices), WaveSub(Data->Vertices[i + 2].Vertices, Data->Vertices[i].Vertices)));

			Data->Vertices[i].Normals = N;
			Data->Vertices[i + 1].Normals = N;
			Data->Vertices[i + 2].Normals = N;
		}
	}
}

#define WAVE_FLOAT_CMP_PRECISION 0.001f

inline char WaveFloatEqual(float f1, float f2)
{
	if (((f1 - WAVE_FLOAT_CMP_PRECISION) < f2) &&
		((f1 + WAVE_FLOAT_CMP_PRECISION) > f2))
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

char WaveCmpWithNormal = 1;
inline int WaveCompareVertices(WaveVertexData* a, WaveVertexData* b)
{
	/*
	if (WaveFloatEqual(a->Vertices.x, b->Vertices.x) &&
		WaveFloatEqual(a->Vertices.y, b->Vertices.y) &&
		WaveFloatEqual(a->Vertices.z, b->Vertices.z) &&
		WaveFloatEqual(a->TexCoords.x, b->TexCoords.x) &&
		WaveFloatEqual(a->TexCoords.y, b->TexCoords.y))
		return 0;

	if (a->Vertices.x > b->Vertices.x)
		return 1;

	return -1;
	*/
	char* A = (char*)a + 8;
	char* B = (char*)b + 8;
	
	size_t Offset = sizeof(WaveVertexData) - 8 - sizeof(WaveVec3);
	if (!WaveCmpWithNormal)
		Offset -= sizeof(WaveVec3);

	return memcmp(A, B, Offset);
}

int WaveCompareFunc(const void* a, const void* b)
{
	return WaveCompareVertices((WaveVertexData*)a, (WaveVertexData*)b);
}

int WaveCompareFunc1(const void* a, const void* b)
{
	WaveVertexData* A = (WaveVertexData*)a;
	WaveVertexData* B = (WaveVertexData*)b;
	return A->VertexIndex - B->VertexIndex;
}

void WaveGenIndices(WaveModelData* ModelData)
{
	WaveLoadingStatus = WAVE_STATUS_GEN_INDICES;
	for (uint32_t k = 0; k < ModelData->MeshCount; k++)
	{
		WaveMeshData* Data = &ModelData->Meshes[k];

		Data->IndexCount = 0;

		Data->Indices = (uint32_t*)calloc(Data->VertexCount, sizeof(uint32_t));
		qsort(Data->Vertices, Data->VertexCount, sizeof(WaveVertexData), WaveCompareFunc);

		uint32_t VertexCount = 1;
		WaveVertexData* NewVertices = (WaveVertexData*)calloc(Data->VertexCount, sizeof(WaveVertexData));
		NewVertices[0] = Data->Vertices[0];

		uint32_t j = 0;
		for (uint32_t i = 0; i < Data->VertexCount - 1; i++)
		{
			WaveVertexData* A = ((WaveVertexData*)&Data->Vertices[i]);
			WaveVertexData* B = ((WaveVertexData*)&Data->Vertices[i + 1]);


			A->NewVertexIndex = j;

			int Res = WaveCompareVertices(A, B);
			if (Res != 0)
			{
				NewVertices[VertexCount++] = *B;
				j++;
			}

			B->NewVertexIndex = j;
		}

		qsort(Data->Vertices, Data->VertexCount, sizeof(WaveVertexData), WaveCompareFunc1);

		for (uint32_t i = 0; i < Data->VertexCount; i++)
		{
			WaveVertexData* A = ((WaveVertexData*)&Data->Vertices[i]);
			Data->Indices[Data->IndexCount++] = A->NewVertexIndex;
		}

		free(Data->Vertices);

		Data->VertexCount = VertexCount;
		Data->Vertices = NewVertices;
		Data->Vertices = (WaveVertexData*)realloc(Data->Vertices, Data->VertexCount * sizeof(WaveVertexData));
	}
}

int WaveCompareMaterials(const WaveModelMaterial* a, const WaveModelMaterial* b)
{
	char* A = (char*)a + 2048;
	char* B = (char*)b + 2048;

	return memcmp(A, B, sizeof(WaveModelMaterial) - 2048);
}

int WaveCompareMeshMaterials(const void* a, const void* b)
{
	WaveMeshData* A = (WaveMeshData*)a;
	WaveMeshData* B = (WaveMeshData*)b;

	char* AA = (char*)A->Material + 2048;
	char* BB = (char*)B->Material + 2048;

	return memcmp(AA, BB, sizeof(WaveModelMaterial) - 2048);
}

void WaveCombineMeshes(WaveMeshData* A, WaveMeshData* B, WaveMeshData* Dst)
{
//	uint32_t OldVertexCount = A->VertexCount;
//	uint32_t OldIndexCount = A->IndexCount;
	Dst->VertexCount = A->VertexCount + B->VertexCount;
	Dst->VertexSize = Dst->VertexCount;
	Dst->Material = B->Material;
	Dst->Vertices = (WaveVertexData*)malloc(Dst->VertexCount * sizeof(WaveVertexData));

	memcpy(Dst->Vertices, A->Vertices, A->VertexCount * sizeof(WaveVertexData));
	memcpy(Dst->Vertices + A->VertexCount, B->Vertices, B->VertexCount * sizeof(WaveVertexData));
//	free(A->Vertices);

	if (A->IndexCount != 0 && B->IndexCount != 0)
	{
		Dst->IndexCount = A->IndexCount + B->IndexCount;
		Dst->IndexSize = Dst->IndexCount;
		Dst->Indices = (uint32_t*)malloc(Dst->IndexCount * sizeof(uint32_t));

		memcpy(Dst->Indices, A->Indices, A->IndexCount * sizeof(uint32_t));
		memcpy(Dst->Indices + A->IndexCount, B->Indices, B->IndexCount * sizeof(uint32_t));
	}	

//	memcpy(A->Indices + OldIndexCount, B->Indices, B->IndexCount * sizeof(uint32_t));
}

void WaveRemoveRedundantMaterials(WaveModelData* ModelData)
{
	WaveLoadingStatus = WAVE_STATUS_REMOVE_REDUDANT_MATERIALS;
	qsort(ModelData->Meshes, ModelData->MeshCount, sizeof(WaveMeshData), WaveCompareMeshMaterials);

	for (uint32_t i = 0; i < ModelData->MeshCount; i++)
	{
		printf("%s\n", ModelData->Meshes[i].Material->DiffuseTexture);
	}
	
	printf("Mesh Count: %d, Material Count: %d\n", ModelData->MeshCount, ModelData->MaterialCount);

	uint32_t MeshCount = 1;
	WaveMeshData* NewMeshes = (WaveMeshData*)calloc(ModelData->MeshCount, sizeof(WaveMeshData));
	NewMeshes[0] = ModelData->Meshes[0];
//	for (uint32_t i = 0; i < ModelData->MeshCount; i++)
//		NewMeshes[i] = ModelData->Meshes[i];

//	NewMeshes[0] = ModelData->Meshes[0];
//	NewMaterials[0] = ModelData->Materials[0];

	printf("\n\n");

	WaveMeshData RefMesh;
	WaveMeshData RefMesh1;
	RefMesh = ModelData->Meshes[0];

	for (uint32_t i = 0; i < ModelData->MeshCount - 1; i++)
	{
		WaveMeshData* A = ((WaveMeshData*)&NewMeshes[MeshCount - 1]);
		WaveMeshData* B = ((WaveMeshData*)&ModelData->Meshes[i + 1]);

		if (WaveCompareMaterials(A->Material, B->Material) == 0) //Are the same
		{
			WaveCombineMeshes(A, B, &RefMesh);

			RefMesh1 = NewMeshes[MeshCount - 1];
			WaveCombineMeshes(&RefMesh1, &RefMesh, &NewMeshes[MeshCount - 1]);
		}
		else
		{
			NewMeshes[MeshCount++] = *B;
		}
	}

	uint32_t MaterialCount = MeshCount;
	WaveModelMaterial* NewMaterials = (WaveModelMaterial*)calloc(ModelData->MaterialCount, sizeof(WaveModelMaterial));

	for (uint32_t i = 0; i < MeshCount; i++)
	{
		NewMaterials[i] = *NewMeshes[i].Material;
		NewMeshes[i].Material = &NewMaterials[i];
		printf("Tex %s\n", NewMeshes[i].Material->DiffuseTexture);
	}
		

	free(ModelData->Meshes);
	free(ModelData->Materials);
	uint32_t Diff = (ModelData->MaterialCount - MeshCount);
	printf("Removed material: %d\n", Diff);

	ModelData->MaterialCount = MaterialCount;
	ModelData->MeshCount = MeshCount;
	ModelData->Meshes = NewMeshes;
	ModelData->Materials = NewMaterials;

	if (Diff != 0)
		ModelData->Meshes = (WaveMeshData*)realloc(ModelData->Meshes, ModelData->MeshCount * sizeof(WaveMeshData));

	for (uint32_t j = 0; j < ModelData->MeshCount; j++)
	{
		WaveMeshData* Data = &ModelData->Meshes[j];

		for (uint32_t i = 0; i < Data->VertexCount; i++)
			Data->Vertices[i].VertexIndex = i;
	}

	uint32_t j = 0;
	/*
	for (uint32_t i = 1; i < ModelData->MeshCount - 1; i += 2)
	{
		WaveMeshData* A = ((WaveMeshData*)&ModelData->Meshes[i]);
		WaveMeshData* B = ((WaveMeshData*)&ModelData->Meshes[i + 1]);

		if (WaveCompareMaterials(A->Material, B->Material) == 0) //Are the same
		{
			WaveCombineMeshes(A, B, &RefMesh);

			if (WaveCompareMaterials(NewMeshes[MeshCount].Material, RefMesh.Material) == 0)
			{
				RefMesh1 = NewMeshes[MeshCount];
				WaveCombineMeshes(&RefMesh1, &RefMesh, &NewMeshes[MeshCount]);
				NewMeshes[MeshCount].Material = &NewMaterials[MaterialCount];
			}				
			else
			{
				WaveCombineMeshes(A, B, &NewMeshes[++MeshCount]);
				NewMaterials[++MaterialCount] = *A->Material;
				NewMeshes[MeshCount].Material = &NewMaterials[MaterialCount];
			}
				
		}
		else
		{

			NewMeshes[++MeshCount] = *A;	
			NewMaterials[++MaterialCount] = *A->Material;
			NewMeshes[MeshCount].Material = &NewMaterials[MaterialCount];

			NewMeshes[++MeshCount] = *B;	
			NewMaterials[++MaterialCount] = *B->Material;
			NewMeshes[MeshCount].Material = &NewMaterials[MaterialCount];
		}
	}
	*/

	/*
	uint32_t j = 0;
	
			//FIX in gen indices also set i = 1
	for (uint32_t i = 0; i < ModelData->MeshCount - 1; i++)
	{
		WaveMeshData* A = ((WaveMeshData*)&ModelData->Meshes[i]);
		WaveMeshData* B = ((WaveMeshData*)&ModelData->Meshes[i + 1]);

	//	WaveCombineMeshes(A, B, &NewMeshes[MeshCount]);
		

		if (WaveCompareMaterials(A->Material, B->Material) != 0)
		{
			printf("%s\n", B->Material->DiffuseTexture);
			if (i == 0)
			{
				NewMeshes[0] = *A;
				NewMaterials[MaterialCount++] = *A->Material;
			}
				

			MeshCount++;

			NewMaterials[MaterialCount++] = *B->Material;
		}
		else if (i == 0)
		{
			NewMeshes[MeshCount++] = *A;
			NewMaterials[MaterialCount++] = *B->Material;
		}

		printf("\t%d\n", MeshCount - 1);
		WaveMeshData Tmp = NewMeshes[MeshCount - 1];
		WaveCombineMeshes(&Tmp, B, &NewMeshes[MeshCount - 1]);

	//	A->NewVertexIndex = j;
	//
	//	int Res = WaveCompareVertices(A, B);
	//	if (Res != 0)
	//	{
	//		NewVertices[VertexCount++] = *B;
	//		j++;
	//	}
	//
	//	B->NewVertexIndex = j;
	}
	*/
	/*
	free(ModelData->Meshes);
	free(ModelData->Materials);
	printf("Removed material: %d\n", (ModelData->MaterialCount - MeshCount));
	ModelData->MaterialCount = MaterialCount;
	ModelData->MeshCount = MeshCount;
	ModelData->Meshes = NewMeshes;
	ModelData->Materials = NewMaterials;

	ModelData->Materials = (WaveModelMaterial*)realloc(ModelData->Materials, ModelData->MaterialCount * sizeof(WaveModelMaterial));
	ModelData->Meshes = (WaveMeshData*)realloc(ModelData->Meshes, ModelData->MeshCount * sizeof(WaveMeshData));
	
	for (uint32_t j = 0; j < ModelData->MeshCount; j++)
	{
		WaveMeshData* Data = &ModelData->Meshes[j];

		for (uint32_t i = 0; i < Data->VertexCount; i++)
			Data->Vertices[i].VertexIndex = i;
	}

	for (uint32_t i = 0; i < ModelData->MeshCount; i++)
	{
		printf("%s\n", ModelData->Meshes[i].Material->DiffuseTexture);
	}
	*/

//	qsort(ModelData->Materials, ModelData->MaterialCount, sizeof(WaveModelMaterial), WaveCompareMaterials);
}

void WaveGenAABBs(WaveModelData* ModelData)
{
	for (uint32_t i = 0; i < ModelData->MeshCount; i++)
	{
		WaveMeshData* Data = &ModelData->Meshes[i];

		Data->AABB.Min = Data->Vertices[0].Vertices;
		Data->AABB.Max = Data->Vertices[0].Vertices;
		for (uint32_t i = 1; i < Data->VertexCount; i++)
		{
			if (Data->Vertices[i].Vertices.x > Data->AABB.Max.x) Data->AABB.Max.x = Data->Vertices[i].Vertices.x;
			if (Data->Vertices[i].Vertices.y > Data->AABB.Max.y) Data->AABB.Max.y = Data->Vertices[i].Vertices.y;
			if (Data->Vertices[i].Vertices.z > Data->AABB.Max.z) Data->AABB.Max.z = Data->Vertices[i].Vertices.z;

			if (Data->Vertices[i].Vertices.x < Data->AABB.Min.x) Data->AABB.Min.x = Data->Vertices[i].Vertices.x;
			if (Data->Vertices[i].Vertices.y < Data->AABB.Min.y) Data->AABB.Min.y = Data->Vertices[i].Vertices.y;
			if (Data->Vertices[i].Vertices.z < Data->AABB.Min.z) Data->AABB.Min.z = Data->Vertices[i].Vertices.z;
		}
	}
}

void WaveRunPPP(WaveModelData* ModelData, uint32_t Settings)
{
	if (Settings & WAVE_FORCE_GEN_NORMALS)
	{
		WaveLoadingStatus = WAVE_STATUS_GEN_NORMALS;
		WaveCmpWithNormal = 1;
		WaveGenNormals(ModelData);
	}
	else
		WaveCmpWithNormal = 0;
	if (!(Settings & WAVE_GEN_SMOOTH_NORMALS))
		WaveCmpWithNormal = 1;

	if (Settings & WAVE_REMOVE_REDUNDANT_MATERIALS && ModelData->MeshCount > 1)
		WaveRemoveRedundantMaterials(ModelData);

	if (Settings & WAVE_GEN_INDICES)
		WaveGenIndices(ModelData);

	if (Settings & WAVE_GEN_SMOOTH_NORMALS)
		WaveGenSmoothNormals(ModelData);

	WaveGenAABBs(ModelData);
}

WaveModelMaterial WaveEmptyMaterial =
{
	"Default",
	{ 1.0, 1.0, 1.0 },
	{ 1.0, 1.0, 1.0 },
	{ 1.0, 1.0, 1.0 },
	1.0,
	1.0,
	1.0,
	"NoTexture",
	"NoTexture",
	"NoTexture",
	"NoTexture",
	"NoTexture",
	"NoTexture",
	"NoTexture",
	"NoTexture",
	1.0,
	0.0,
	1.0,
	"NoTexture",
	"NoTexture",
	"NoTexture",
	"NoTexture"
};

typedef struct
{
	int32_t V;
	int32_t VT;
	int32_t VN;
} WaveVertexReference;

uint32_t WaveGetStringCount(uint32_t StringLength, const char* String, uint32_t LineCount, WaveLine* Lines)
{
	uint32_t Count = 0;
	for (uint32_t i = 0; i < LineCount; i++)
	{
		char Fount = 0;
		for (uint32_t j = 0; j < StringLength; j++)
		{
			if (Lines[i].Line[j] == String[j])
				Fount = 1;
			else
			{
				Fount = 0;
				break;
			}

		}
		if (Fount == 1)
			Count++;
	}

	return Count;
}

void WaveScan(char* Buffer, const char* Token, const char* Format, ...)
{
	uint32_t AddCount = strlen(Token);
	Buffer += AddCount;
	while (*Buffer == ' ')
	{
		Buffer++;
	}

	va_list Args;
	va_start(Args, Format);
	vsscanf(Buffer, Format, Args);
	va_end(Args);
}

char* WaveGetLine(char* Buffer, char** OldBuffer)
{
	if (Buffer == NULL)
		Buffer = *OldBuffer;

	uint32_t i = 0;
	char Line[2048];
	memset(Line, 0, 2048);

	while (i < 2048 &&
		*Buffer != '\n' &&
		*Buffer != '\r' &&
		*Buffer != '\0')
	{

		Line[i] = *Buffer;
		i++;

		Buffer++;
	}

	if (i == 0 && *Buffer == '\0')
		return NULL;

	*OldBuffer = Buffer + 1;
	return Line;
}

char WaveLineEqual(char* Line, const char* Str)
{
	size_t Length = strlen(Str);

	for (size_t i = 0; i < Length; i++)
		if (tolower(Line[i]) != tolower(Str[i]))
			return 0;

	return 1;
}

//use memset 0
void WaveCombinePath(const char* Path, const char* FileName, char* PathCombined)
{
	strcpy(PathCombined, Path);
	char* PC = PathCombined;
	memcpy(PC + strlen(PathCombined), FileName, strlen(FileName));
}

void WaveCombinePathDst(const char* Path, char* DstFileName)
{
	char PathCombined[4096];
	memset(PathCombined, 0, 4096);
	WaveCombinePath(Path, DstFileName, PathCombined);
	strcpy(DstFileName, PathCombined);
}

char WaveLoadMTL(const char* FilePath, const char* FileName, WaveModelData* Data, uint32_t Settings)
{
	char PathCombined[2048];
	memset(PathCombined, 0, 2048);

	if (strstr(FileName, "/") ||
		strstr(FileName, "\\") ||
		!(Settings & WAVE_MATERIAL_USE_MODEL_PATH))
		strcpy(PathCombined, FileName);
	else 
		WaveCombinePath(FilePath, FileName, PathCombined);

	size_t MatLength = 0;
	char* MatBuffer = WaveLoadFile(PathCombined, &MatLength);
	if (MatBuffer == NULL || MatLength == 0)
	{
		printf("Failed to load material file: %s\n", PathCombined);
		return 0;
	}		
	else
	{
		int32_t MaterialCount = 0;

		char* OldBuffer;
		char* Line = WaveGetLine(MatBuffer, &OldBuffer);
		while (Line)
		{
			if (Line[0] == 'n' && Line[1] == 'e')
				MaterialCount++;

			Line = WaveGetLine(NULL, &OldBuffer);
		}

		if (MaterialCount == 0)
			return 0;

		Data->Materials = (WaveModelMaterial*)calloc(MaterialCount, sizeof(WaveModelMaterial));
		MaterialCount = 0;

		WaveModelMaterial* CurMaterial = &Data->Materials[0];

		Line = WaveGetLine(MatBuffer, &OldBuffer);
		while (Line)
		{
			if (WaveLineEqual(Line, "newmtl "))
			{
				CurMaterial = &Data->Materials[MaterialCount++];

				memcpy(CurMaterial, &WaveEmptyMaterial, sizeof(WaveModelMaterial));
				WaveScan(Line, "newmtl", "%[^\r\n]%*c\r\n", CurMaterial->MaterialName);
			}
			else if (WaveLineEqual(Line, "Ka "))		WaveScan(Line, "Ka", "%f %f %f\n", &CurMaterial->AmbientColor.x, &CurMaterial->AmbientColor.y, &CurMaterial->AmbientColor.z);
			else if (WaveLineEqual(Line, "Kd "))		WaveScan(Line, "Kd", "%f %f %f\n", &CurMaterial->DiffuseColor.x, &CurMaterial->DiffuseColor.y, &CurMaterial->DiffuseColor.z);
			else if (WaveLineEqual(Line, "Ks "))		WaveScan(Line, "Ks", "%f %f %f\n", &CurMaterial->SpecularColor.x, &CurMaterial->SpecularColor.y, &CurMaterial->SpecularColor.z);
			else if (WaveLineEqual(Line, "Ke "))		WaveScan(Line, "Ke", "%f\n", &CurMaterial->Emissive);
			else if (WaveLineEqual(Line, "Ns "))		WaveScan(Line, "Ns", "%f\n", &CurMaterial->SpecularExponent);
			else if (WaveLineEqual(Line, "d "))			WaveScan(Line, "d", "%f\n", &CurMaterial->Dissolve);
			else if (WaveLineEqual(Line, "map_Ka "))	WaveScan(Line, "map_Ka", "%[^\r\n]%*c\r\n", CurMaterial->AmbientTexture);
			else if (WaveLineEqual(Line, "map_Kd "))	WaveScan(Line, "map_Kd", "%[^\r\n]%*c\r\n", CurMaterial->DiffuseTexture);
			else if (WaveLineEqual(Line, "map_Ks "))	WaveScan(Line, "map_Ks", "%[^\r\n]%*c\r\n", CurMaterial->SpecularTexture);
			else if (WaveLineEqual(Line, "map_Ke "))	WaveScan(Line, "map_Ke", "%[^\r\n]%*c\r\n", CurMaterial->EmissiveTexture);
			else if (WaveLineEqual(Line, "map_d "))		WaveScan(Line, "map_d", "%[^\r\n]%*c\r\n", CurMaterial->AlphaTexture);
			else if (WaveLineEqual(Line, "map_Bump "))	WaveScan(Line, "map_Bump", "%[^\r\n]%*c\r\n", CurMaterial->BumpTexture);
			else if (WaveLineEqual(Line, "map_Disp "))	WaveScan(Line, "map_Disp", "%[^\r\n]%*c\r\n", CurMaterial->DisplacmentTexture);
			else if (WaveLineEqual(Line, "disp "))		WaveScan(Line, "disp", "%[^\r\n]%*c\r\n", CurMaterial->HeightTexture);			
			//PBR-Extension
			else if (WaveLineEqual(Line, "Pr "))		WaveScan(Line, "Pr", "%f\n", &CurMaterial->Roughness);
			else if (WaveLineEqual(Line, "Pm "))		WaveScan(Line, "Pm", "%f\n", &CurMaterial->Metallic);
			else if (WaveLineEqual(Line, "Ps "))		WaveScan(Line, "Ps", "%f\n", &CurMaterial->Sheen);
			else if (WaveLineEqual(Line, "map_Pr "))	WaveScan(Line, "map_Pr", "%[^\r\n]%*c\r\n", CurMaterial->RoughnessTexture);
			else if (WaveLineEqual(Line, "map_Pm "))	WaveScan(Line, "map_Pm", "%[^\r\n]%*c\r\n", CurMaterial->MetallicTexture);
			else if (WaveLineEqual(Line, "map_Ps "))	WaveScan(Line, "map_Ps", "%[^\r\n]%*c\r\n", CurMaterial->SheenTexture);
			else if (WaveLineEqual(Line, "norm "))		WaveScan(Line, "norm", "%[^\r\n]%*c\r\n", CurMaterial->NormalTexture);

			Line = WaveGetLine(NULL, &OldBuffer);
		}

		Data->MaterialCount = MaterialCount;
		if (Settings & WAVE_MATERIAL_USE_MODEL_PATH)
		{
			for (uint32_t i = 0; i < Data->MaterialCount; i++)
			{
				printf("Ha: %s\n", Data->Materials[i].DiffuseTexture);
				
				if (strcmp(Data->Materials[i].AmbientTexture, "NoTexture") != 0		&& Data->Materials[i].AmbientTexture[1] != ':')		WaveCombinePathDst(FilePath, Data->Materials[i].AmbientTexture);
				if (strcmp(Data->Materials[i].DiffuseTexture, "NoTexture") != 0		&& Data->Materials[i].DiffuseTexture[1] != ':')		WaveCombinePathDst(FilePath, Data->Materials[i].DiffuseTexture);
				if (strcmp(Data->Materials[i].SpecularTexture, "NoTexture") != 0	&& Data->Materials[i].SpecularTexture[1] != ':')	WaveCombinePathDst(FilePath, Data->Materials[i].SpecularTexture);
				if (strcmp(Data->Materials[i].EmissiveTexture, "NoTexture") != 0	&& Data->Materials[i].EmissiveTexture[1] != ':')	WaveCombinePathDst(FilePath, Data->Materials[i].EmissiveTexture);
				if (strcmp(Data->Materials[i].AlphaTexture, "NoTexture") != 0		&& Data->Materials[i].AlphaTexture[1] != ':')		WaveCombinePathDst(FilePath, Data->Materials[i].AlphaTexture);
				if (strcmp(Data->Materials[i].BumpTexture, "NoTexture") != 0		&& Data->Materials[i].BumpTexture[1] != ':')		WaveCombinePathDst(FilePath, Data->Materials[i].BumpTexture);
				if (strcmp(Data->Materials[i].DisplacmentTexture, "NoTexture") != 0 && Data->Materials[i].DisplacmentTexture[1] != ':')	WaveCombinePathDst(FilePath, Data->Materials[i].DisplacmentTexture);
				if (strcmp(Data->Materials[i].HeightTexture, "NoTexture") != 0		&& Data->Materials[i].HeightTexture[1] != ':')		WaveCombinePathDst(FilePath, Data->Materials[i].HeightTexture);
				if (strcmp(Data->Materials[i].RoughnessTexture, "NoTexture") != 0	&& Data->Materials[i].RoughnessTexture[1] != ':')	WaveCombinePathDst(FilePath, Data->Materials[i].RoughnessTexture);
				if (strcmp(Data->Materials[i].MetallicTexture, "NoTexture") != 0	&& Data->Materials[i].MetallicTexture[1] != ':')	WaveCombinePathDst(FilePath, Data->Materials[i].MetallicTexture);
				if (strcmp(Data->Materials[i].SheenTexture, "NoTexture") != 0		&& Data->Materials[i].SheenTexture[1] != ':')		WaveCombinePathDst(FilePath, Data->Materials[i].SheenTexture);
				if (strcmp(Data->Materials[i].NormalTexture, "NoTexture") != 0		&& Data->Materials[i].NormalTexture[1] != ':')		WaveCombinePathDst(FilePath, Data->Materials[i].NormalTexture);
			}
		}
	}

	return 1;
}

WaveModelData WaveLoadOBJ(const char* FilePath, size_t Length, char* Buffer, uint32_t Settings)
{
	WaveModelData Data;

	//	uint32_t LineCount = 0;
	//	WaveLine* Lines = WaveGetLinesFromBuffer(Length, Buffer, &LineCount);

	uint32_t VertexCount = 0;
	uint32_t VertexTextureCount = 0;
	uint32_t VertexNormalCount = 0;
	uint32_t FaceCount = 0;
	uint32_t MaterialCount = 0;

	WaveLoadingStatus = WAVE_STATUS_PARSE_MODEL;
	char* OldBuffer;
	char* Line = WaveGetLine(Buffer, &OldBuffer);
	while (Line)
	{
		if (Line[0] == 'u' && Line[1] == 's')
			MaterialCount++;
		else if (Line[0] == 'v' && Line[1] == ' ')
			VertexCount++;
		else if (Line[0] == 'v' && Line[1] == 't')
			VertexTextureCount++;
		else if (Line[0] == 'v' && Line[1] == 'n')
			VertexNormalCount++;
		else if (Line[0] == 'f' && Line[1] == ' ')
			FaceCount++;

		Line = WaveGetLine(NULL, &OldBuffer);
	}

	WaveLoadingStatus = WAVE_STATUS_ALLOCATE_VERTICES;
	const WaveVec3 NullVec = { 0.0, 0.0, 0.0 };
	WaveVec3* Vertices = (WaveVec3*)calloc(VertexCount, sizeof(WaveVec3));
	WaveVec3* VertexTextures = (WaveVec3*)calloc(VertexTextureCount, sizeof(WaveVec3));
	WaveVec3* VertexNormals = (WaveVec3*)calloc(VertexNormalCount, sizeof(WaveVec3));

	Data.Materials = &WaveEmptyMaterial;
	Data.MaterialCount = 0;

	VertexCount = 0;
	VertexTextureCount = 0;
	VertexNormalCount = 0;
	MaterialCount = 0;

	uint32_t VertexReferenceIndex = 0;
	uint32_t VertexReferenceSize = 64;
	WaveVertexReference* VertexReferences = (WaveVertexReference*)malloc(VertexReferenceSize * sizeof(WaveVertexReference));

	char FoundMaterialFile = 0;

	uint32_t CurrentMaterialIndex = 0;

	WaveMeshData* Mesh = NULL;
	Data.MeshCount = 1;
	Data.Meshes = (WaveMeshData*)malloc(Data.MeshCount * sizeof(WaveMeshData));
	
	Data.Meshes[0].VertexSize = FaceCount * 4 * 2;
	if (Data.Meshes[0].VertexSize > WAVE_MAX_ALLOCATION_SIZE)
		Data.Meshes[0].VertexSize = WAVE_MAX_ALLOCATION_SIZE;
	Data.Meshes[0].Vertices = (WaveVertexData*)calloc(Data.Meshes[0].VertexSize, sizeof(WaveVertexData));
	Data.Meshes[0].VertexCount = 0;
	Data.Meshes[0].IndexCount = 0;
	Data.Meshes[0].IndexSize = 0;
	Data.Meshes[0].Indices = NULL;
	Mesh = &Data.Meshes[0];

	Line = WaveGetLine(Buffer, &OldBuffer);
	while (Line)
	{
		if (Line[0] == 'm' && Line[1] == 't' && (Settings & WAVE_LOAD_MATERIAL))
		{
			char MaterialFile[1024];
			WaveScan(Line, "mtllib", "%[^\r\n]%*c\r\n", MaterialFile);

			WaveLoadingStatus = WAVE_STATUS_PARSE_MATERIAL;
			if (WaveLoadMTL(FilePath, MaterialFile, &Data, Settings))
			{
				Data.MeshCount = Data.MaterialCount;
				free(Data.Meshes[0].Vertices);//assuming that the model just loads one material file
				free(Data.Meshes);
				Data.Meshes = (WaveMeshData*)malloc(Data.MeshCount * sizeof(WaveMeshData));

				for (uint32_t i = 0; i < Data.MeshCount; i++)
				{
					Data.Meshes[i].VertexSize = FaceCount * 3;
					if (Data.Meshes[i].VertexSize > WAVE_MAX_ALLOCATION_SIZE)
						Data.Meshes[i].VertexSize = WAVE_MAX_ALLOCATION_SIZE;
					Data.Meshes[i].Vertices = (WaveVertexData*)calloc(Data.Meshes[i].VertexSize, sizeof(WaveVertexData));
					if (Data.Meshes[i].Vertices == NULL)
						printf("Failed to allocate Mesh: %d\n", i);
					Data.Meshes[i].VertexCount = 0;
					Data.Meshes[i].IndexCount = 0;
					Data.Meshes[i].IndexSize = 0;
					Data.Meshes[i].Indices = NULL;					
				}
				Mesh = &Data.Meshes[0];
			}	
			WaveLoadingStatus = WAVE_STATUS_PARSE_MODEL;
		}

		if (Line[0] == 'u' && Line[1] == 's' && Data.MaterialCount != 0 && Settings & WAVE_LOAD_MATERIAL)
		{
			char MaterialName[2048];
			WaveScan(Line, "usemtl", "%s", MaterialName);

			for (uint32_t j = 0; j < Data.MaterialCount; j++)
				if (strcmp(Data.Materials[j].MaterialName, MaterialName) == 0)
					CurrentMaterialIndex = j;

			Mesh = &Data.Meshes[CurrentMaterialIndex];
			Mesh->Material = &Data.Materials[CurrentMaterialIndex];
		}
		else if (Line[0] == 'v' && Line[1] == ' ')
		{
			WaveScan(Line, "v", "%f %f %f\n", &Vertices[VertexCount].x, &Vertices[VertexCount].y, &Vertices[VertexCount].z);
			VertexCount++;
		}
		else if (Line[0] == 'v' && Line[1] == 't')
		{
			WaveScan(Line, "vt", "%f %f\n", &VertexTextures[VertexTextureCount].x, &VertexTextures[VertexTextureCount].y);
			VertexTextureCount++;
		}
		else if (Line[0] == 'v' && Line[1] == 'n')
		{
			WaveScan(Line, "vn", "%f %f %f\n", &VertexNormals[VertexNormalCount].x, &VertexNormals[VertexNormalCount].y, &VertexNormals[VertexNormalCount].z);
			VertexNormalCount++;
		}
		else if (Line[0] == 'f' && Line[1] == ' ')
		{
			VertexReferenceIndex = 0;

			//	char* Line = Line + 1;

			char* TempLine = strtok(Line + 1, " ");
			while (TempLine)
			{
				char* StrV = TempLine;
				char* StrVT = WaveStrtok(StrV, '/');
				char* StrVN = WaveStrtok(StrVT, '/');

				int32_t v = atoi(StrV);
				int32_t vt = atoi(StrVT);
				int32_t vn = atoi(StrVN);

				v = (v >= 0 ? v : VertexCount + v + 1);
				vt = (vt >= 0 ? vt : VertexTextureCount + vt + 1);
				vn = (vn >= 0 ? vn : VertexNormalCount + vn + 1);

				VertexReferences[VertexReferenceIndex].V = v;
				VertexReferences[VertexReferenceIndex].VT = vt;
				VertexReferences[VertexReferenceIndex].VN = vn;

				VertexReferenceIndex++;

				if (VertexReferenceIndex >= VertexReferenceSize)
				{
					VertexReferenceSize += 64;
					VertexReferences = (WaveVertexReference*)realloc(VertexReferences, VertexReferenceSize * sizeof(WaveVertexReference));
				}

				TempLine = strtok(NULL, " ");
			}

			for (uint32_t m = 1; m + 1 < VertexReferenceIndex; m++)
			{
				WaveVertexReference* p[3] = { &VertexReferences[0], &VertexReferences[m], &VertexReferences[m + 1] };

				for (uint32_t j = 0; j < 3; j++)
				{
					WaveVertexData Vertex;

					if (Mesh->VertexCount >= Mesh->VertexSize)
					{
						uint32_t TmpCount = FaceCount * 3;
						if (TmpCount > WAVE_MAX_ALLOCATION_SIZE)
							TmpCount = WAVE_MAX_ALLOCATION_SIZE;

						Mesh->VertexSize += TmpCount;
						Mesh->Vertices = (WaveVertexData*)realloc(Mesh->Vertices, Mesh->VertexSize * sizeof(WaveVertexData));
						if (Mesh->Vertices == NULL)
							printf("Wave Error: Failed to resize vertex buffer!\n");
					}						

					Vertex.VertexIndex = Mesh->VertexCount;
					Vertex.Vertices = Vertices[p[j]->V - 1];
					Vertex.TexCoords = p[j]->VT != 0 ? VertexTextures[p[j]->VT - 1] : NullVec;
					Vertex.Normals = p[j]->VN != 0 ? VertexNormals[p[j]->VN - 1] : NullVec;
					Vertex.VertexColor.x = 1.0;
					Vertex.VertexColor.y = 1.0;
					Vertex.VertexColor.z = 1.0;

					if (Settings & WAVE_FLIP_UVS)
						Vertex.TexCoords.y = 1.0 - Vertex.TexCoords.y;
					if ((Settings & WAVE_GEN_UVS) && p[j]->VT == 0)
						WaveGenUVs(&Vertex);
					if ((Settings & WAVE_GEN_NORMALS) && p[j]->VN == 0)
					{
						WaveVec3 U = { WaveSub(Vertices[p[1]->V - 1], Vertices[p[0]->V - 1]) };
						WaveVec3 V = { WaveSub(Vertices[p[2]->V - 1], Vertices[p[0]->V - 1]) };
						WaveVec3 FaceNormal = WaveNormalize(WaveCross(U, V));

						Vertex.Normals = FaceNormal;
					}						

					Mesh->Vertices[Mesh->VertexCount++] = Vertex;
				}
			}
		}

		Line = WaveGetLine(NULL, &OldBuffer);
	}

	WaveLoadingStatus = WAVE_STATUS_REDUCE_SIZE;

	free(VertexReferences);
	free(VertexNormals);
	free(VertexTextures);
	free(Vertices);

	for (uint32_t i = 0; i < Data.MeshCount; i++)
	{
		Data.Meshes[i].VertexSize = Data.Meshes[i].VertexCount;
		Data.Meshes[i].Vertices = (WaveVertexData*)realloc(Data.Meshes[i].Vertices, Data.Meshes[i].VertexCount * sizeof(WaveVertexData));
	}

	return Data;
}

typedef struct
{
	uint8_t Header[80];
	uint32_t Triangles;
} WaveSTLDescription;

#pragma pack(push)
#pragma pack(1)

typedef struct
{
	WaveVec3 Normal;
	WaveVec3 Vertex1;
	WaveVec3 Vertex2;
	WaveVec3 Vertex3;
	int16_t ByteCount;
} WaveSTLVertex;

#pragma pack(pop)

WaveModelData WaveLoadSTL(const char* FilePath, size_t Length, char* Buffer, uint32_t Settings)
{
	WaveLoadingStatus = WAVE_STATUS_PARSE_MODEL;
	WaveSTLDescription* Description = (WaveSTLDescription*)Buffer;
	Buffer += sizeof(WaveSTLDescription);
	WaveSTLVertex* VertexArray = (WaveSTLVertex*)Buffer;

	uint32_t VertexCount = Description->Triangles * 3;

	WaveLoadingStatus = WAVE_STATUS_ALLOCATE_VERTICES;
	WaveModelData ModelData;
	ModelData.MaterialCount = 0;
	ModelData.Materials = &WaveEmptyMaterial;
	ModelData.MeshCount = 1;
	ModelData.Meshes = (WaveMeshData*)malloc(ModelData.MeshCount * sizeof(WaveMeshData));
	ModelData.Meshes[0].VertexSize = VertexCount;
	ModelData.Meshes[0].Vertices = (WaveVertexData*)calloc(VertexCount, sizeof(WaveVertexData));
	ModelData.Meshes[0].VertexCount = VertexCount;
	ModelData.Meshes[0].Material = &WaveEmptyMaterial;
	ModelData.Meshes[0].IndexCount = 0;
	ModelData.Meshes[0].IndexSize = 0;
	ModelData.Meshes[0].Indices = NULL;

	WaveVec3 DefaultColor = { 1.0, 1.0, 1.0 };

	WaveVertexData* Vertices = ModelData.Meshes[0].Vertices;

	WaveLoadingStatus = WAVE_STATUS_PARSE_MODEL;
	uint32_t j = 0;
	for (uint32_t i = 0; i < Description->Triangles; i++)
	{		
		Vertices[j].VertexIndex = j;
		Vertices[j + 1].VertexIndex = j + 1;
		Vertices[j + 2].VertexIndex = j + 2;
		
		Vertices[j].Vertices = VertexArray[i].Vertex1;
		Vertices[j + 1].Vertices = VertexArray[i].Vertex2;
		Vertices[j + 2].Vertices = VertexArray[i].Vertex3;
		
		Vertices[j].Normals = VertexArray[i].Normal;
		Vertices[j + 1].Normals = VertexArray[i].Normal;
		Vertices[j + 2].Normals = VertexArray[i].Normal;
		
		Vertices[j].VertexColor = DefaultColor;
		Vertices[j + 1].VertexColor = DefaultColor;
		Vertices[j + 2].VertexColor = DefaultColor;

		if (Settings & WAVE_GEN_UVS)
		{
			WaveGenUVs(&Vertices[j]);
			WaveGenUVs(&Vertices[j + 1]);
			WaveGenUVs(&Vertices[j + 2]);
		}

		j += 3;
	}

	return ModelData;
}

//typedef struct WaveGLTFArrayType WaveGLTFArray;
//typedef struct WaveGLTFObjectType WaveGLTFObject;
/*
typedef struct
{
	char* Name;

	double Number;
	char* String;
	char Boolean;
	WaveGLTFObject* Object;
	WaveGLTFArray* Array;
} WaveGLTFDataTypes;

struct WaveGLTFObjectType
{
	char* Name;
	uint32_t DataCount;
	WaveGLTFDataTypes* Data;
};

struct WaveGLTFArrayType
{
	char* Name;
	uint32_t DataCount;
	WaveGLTFDataTypes* Data;
};
*/
typedef enum
{
	WAVE_JSON_DATA_TYPE_OBJECT = 0x0,
	WAVE_JSON_DATA_TYPE_ARRAY = 0x1,
	WAVE_JSON_DATA_TYPE_STRING = 0x2,
	WAVE_JSON_DATA_TYPE_BOOLEAN = 0x3,
	WAVE_JSON_DATA_TYPE_NUMBER = 0x4,
} WaveJsonDataType;




#define WAVE_GLTF_DEFAULT_STR_LENGTH 64

const char base64_pad = '=';

const char base64_table[] = {
	'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M',
	'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
	'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm',
	'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',
	'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '+', '/', '\0'
};

unsigned char* WaveBase64Decode(const char* data, size_t input_length, size_t* output_length) {
	if (input_length % 4 != 0) {
		return NULL;
	}

	*output_length = input_length / 4 * 3;
	if (data[input_length - 1] == base64_pad) (*output_length)--;
	if (data[input_length - 2] == base64_pad) (*output_length)--;

	unsigned char* decoded_data = (unsigned char*)malloc(*output_length);
	if (decoded_data == NULL) return NULL;

	for (int i = 0, j = 0; i < input_length;) {
		uint32_t sextet_a = data[i] == base64_pad ? 0 & i++ : base64_table[data[i++]];
		uint32_t sextet_b = data[i] == base64_pad ? 0 & i++ : base64_table[data[i++]];
		uint32_t sextet_c = data[i] == base64_pad ? 0 & i++ : base64_table[data[i++]];
		uint32_t sextet_d = data[i] == base64_pad ? 0 & i++ : base64_table[data[i++]];

		uint32_t triple = (sextet_a << 3 * 6)
			+ (sextet_b << 2 * 6)
			+ (sextet_c << 1 * 6)
			+ (sextet_d << 0 * 6);

		if (j < *output_length) decoded_data[j++] = (triple >> 2 * 8) & 0xFF;
		if (j < *output_length) decoded_data[j++] = (triple >> 1 * 8) & 0xFF;
		if (j < *output_length) decoded_data[j++] = (triple >> 0 * 8) & 0xFF;
	}

	return decoded_data;
}

void WaveGetJsonString(size_t* Index, char* Buffer, char** Name, uint32_t* NameAllocated, uint32_t* NameLength)
{
	char* TempName = *Name;
	uint32_t TempNameAllocated = *NameAllocated; 
	uint32_t TempNameLength = *NameLength;
	size_t i = *Index;
	if (TempNameAllocated != WAVE_GLTF_DEFAULT_STR_LENGTH)
	{
		TempNameAllocated = WAVE_GLTF_DEFAULT_STR_LENGTH;
		TempName = (char*)realloc(TempName, TempNameAllocated);
	}

	TempNameLength = 0;
	i++;
	while (Buffer[i] != '"')
	{
		TempName[TempNameLength++] = Buffer[i++];
		if (TempNameLength + 1 > TempNameAllocated)
		{
			TempNameAllocated += WAVE_GLTF_DEFAULT_STR_LENGTH;
			TempName = (char*)realloc(TempName, TempNameAllocated);
		}
	}
	TempName[TempNameLength++] = '\0';
	i++;

	*Index = i;
	*Name = TempName;
	*NameAllocated = TempNameAllocated;
	*NameLength = TempNameLength;
}

char WaveGetJsonDataType(size_t* Index, char* Buffer)
{
	char DataType = -1;
	size_t i = *Index;
	while (1)
	{		
		if (Buffer[i] == '{')
		{
			DataType = WAVE_JSON_DATA_TYPE_OBJECT;
			break;
		}			
		else if (Buffer[i] == '[')
		{
			DataType = WAVE_JSON_DATA_TYPE_ARRAY;
			break;
		}			
		else if (Buffer[i] == '"')
		{
			DataType = WAVE_JSON_DATA_TYPE_STRING;
			break;
		}
		else if (Buffer[i] == 't' || Buffer[i] == 'f')
		{
			DataType = WAVE_JSON_DATA_TYPE_BOOLEAN;
			break;
		}
		else if (Buffer[i] != ' ' && Buffer[i] != ':')
		{
			DataType = WAVE_JSON_DATA_TYPE_NUMBER;
			break;
		}
		else if (Buffer[i] == ',')
			break;

		i++;
	}
	*Index = i;

	return DataType;
}

typedef struct
{
	char IsArray;
	char* Name;
	uint32_t Index;
	void* PrevObject;
} WaveGLTFObject;

#define WAVE_GLTF_ALLOCATION_COUNT 64

typedef struct
{
	size_t i;
	size_t Length;
	const char* Buffer; 
	char NextName[64];

	uint32_t ObjectCount;
	uint32_t ObjectSize;
	WaveGLTFObject* Objects; 
	WaveGLTFObject* CurObject;
} WaveGLTFInfo;

/*
void ParseRecursion(WaveGLTFInfo* Inf)
{
//	printf("Hey\n");
	uint32_t Allignment = 0;

	for (Inf->i; Inf->i < Inf->Length; Inf->i++)
	{
		if (Inf->Buffer[Inf->i] == '"')
		{
			//Get name for next object or string
			
			Inf->i++;
			uint16_t j = 0;
			for (j; j < 64; j++)//assume all strings are max 64, make this dynamic
			{
				if (Inf->Buffer[Inf->i] == '"')
					break;
				Inf->NextName[j] = Inf->Buffer[Inf->i++];
			}
			Inf->NextName[j] = '\0';
			Inf->i++;
			ParseRecursion(Inf);
		}
		else if (Inf->Buffer[Inf->i] == '{')
		{
			//Add new Object and make this current, check for overflow
			char IsInArray = Inf->CurObject->Array;
			
			Inf->CurObject = &Inf->Objects[Inf->ObjectCount++];
			

		//	Inf->CurObject->Index = Inf->ObjectCount - 1;
			if (Inf->ObjectCount > 1)
				Inf->CurObject->PrevObject = &Inf->Objects[Inf->ObjectCount - 1];
			else
				Inf->CurObject->PrevObject = NULL;
			if (IsInArray)
				strcpy(Inf->CurObject->Name, "Default");
			else
				strcpy(Inf->CurObject->Name, Inf->NextName);

			for (uint32_t k = 0; k < Allignment; k++)
				printf("\t");
			printf("Object %d: %s\n", (Inf->ObjectCount - 1), Inf->CurObject->Name);

			Allignment++;
		}
		else if (Inf->Buffer[Inf->i] == '[')
		{
			//treat array the same as an object
			Inf->CurObject = &Inf->Objects[Inf->ObjectCount++];

			Inf->CurObject->Array = 1;
		//	Inf->CurObject->Index = Inf->ObjectCount - 1;
			Inf->CurObject->PrevObject = &Inf->Objects[Inf->ObjectCount - 1];
			strcpy(Inf->CurObject->Name, Inf->NextName);
			

			for (uint32_t k = 0; k < Allignment; k++)
				printf("\t");
			printf("Array %d: %s\n", (Inf->ObjectCount - 1), Inf->CurObject->Name);

			Allignment++;
		}
		else if (Inf->Buffer[Inf->i] == '}' ||
				 Inf->Buffer[Inf->i] == ']')
		{
			//Make previous object current;
		//	printf("wtf\n");
			if (Inf->CurObject->PrevObject == NULL)
				break; //Json parsing finished or something wrong

		//	Inf->CurObject->Index
			WaveGLTFObject* ObjTmp = (WaveGLTFObject*)Inf->CurObject->PrevObject;
			Inf->CurObject = ObjTmp;
			if (Allignment > 0)
				Allignment--;
		}
	//	else if (Inf->Buffer[Inf->i] == 't' || Inf->Buffer[Inf->i] == 'f')
	//	{
	//		//Add boolean to current object
	//	}

	}
}

size_t WaveParseJson(char* Buffer, size_t Length)
{
	WaveGLTFInfo Inf;

	Inf.i = 0;
	for (Inf.i; Inf.i < Length; Inf.i++)
	{
		if (Buffer[Inf.i] == '{')
		{
		//	Inf.i++;
			break;
		}
	}

	for (size_t i = Length - 1; i > 0; i--)
	{
		if (Buffer[Inf.i] == '}')
		{
			Buffer[Inf.i + 1] = '\0';
			Length = Inf.i + 1;
			break;
		}
	}

	Inf.Length = Length;
	Inf.Buffer = Buffer;
	Inf.ObjectCount = 0;
	Inf.ObjectSize = WAVE_GLTF_ALLOCATION_COUNT;
	Inf.Objects = (WaveGLTFObject*)malloc(Inf.ObjectSize * sizeof(WaveGLTFObject));

	Inf.CurObject = &Inf.Objects[0];

	
			
	printf("%s\n", Buffer + Inf.i);
	//Init first default object
//	Inf.CurObject->Array = 0;
//	strcpy(Inf.CurObject->Name, "Default");
//	Inf.CurObject->PrevObject = NULL;

	ParseRecursion(&Inf);
//	printf("%s\n", Buffer + Inf.i);
	
//	for (i; i < Length; i++)
//	{
//		if (Buffer[i] == '"')
//		{
//			//Get name for next object or string
//			uint16_t j = 0;
//			for (i++; i < 64; i++)
//			{
//				if (Buffer[i] == '"')
//					break;
//				NextName[j++] = Buffer[i];
//			}
//				
//		}
//		else if (Buffer[i] == '{')
//		{
//			//Add new Object and make this current
//		}
//		else if (Buffer[i] == '}')
//		{
//			//Make previous object current;
//			printf("wtf\n");
//			if (CurObject->PrevObject == NULL)
//				break; //Json parsing finished or something wrong
//
//			WaveGLTFObject* ObjTmp = (WaveGLTFObject*)CurObject->PrevObject;
//			CurObject = ObjTmp;
//		}
//		else if (Buffer[i] == 't' || Buffer[i] == 'f')
//		{
//			//Add boolean to current object
//		}
//		
//	}
	

	return Inf.i;
}
*/

size_t WaveParseJson(char* Buffer, size_t Length)
{
	size_t i = 0;
	for (i; i < Length; i++)
		if (Buffer[i] == '{')
			break;

	size_t BufferStart = i + 1;

	for (i = Length - 1; i > 0; i--)
	{
		if (Buffer[i] == '}')
		{
			Buffer[i] = '\0';
			Length = i;
			break;
		}
	}

	size_t BufferEnd = i;

	uint32_t ObjectCount = 1;
	uint32_t ObjectSize = WAVE_GLTF_ALLOCATION_COUNT;
	WaveGLTFObject* Objects = (WaveGLTFObject*)malloc(ObjectSize * sizeof(WaveGLTFObject));
	WaveGLTFObject* ObjectsCur = &Objects[0];

	Objects[0].Index = 0;
	Objects[0].IsArray = 0;
	Objects[0].PrevObject = NULL;
	Objects[0].Name = NULL;

	uint32_t StringCurSize = 0;
	uint32_t StringSize = WAVE_GLTF_ALLOCATION_COUNT;
	char* StringTmp = (char*)malloc(StringSize);

	for (i = BufferStart; i < BufferEnd; i++)
	{
		if (Buffer[i] == '\"')
		{
			StringCurSize = 0;

			while (Buffer[++i] != '\"')
			{
				StringTmp[StringCurSize] = Buffer[i];
				StringCurSize++;
				if (StringCurSize + 1 >= StringSize)
				{
					StringSize += WAVE_GLTF_ALLOCATION_COUNT;
					StringTmp = (char*)realloc(StringTmp, StringSize);
				}
			}
			StringTmp[StringCurSize++] = '\0';

		//	printf("%s\n", StringTmp);
		}

		else 
		if (Buffer[i] == '{' ||
			Buffer[i] == '[')
		{
			if (Buffer[i] == '[')
				Objects[ObjectCount].IsArray = 1;
			else
				Objects[ObjectCount].IsArray = 0;
			Objects[ObjectCount].Index = 0;			
			Objects[ObjectCount].PrevObject = (WaveGLTFObject*)&Objects[ObjectCount - 1];
			if (ObjectsCur->IsArray == 1)
			{
				Objects[ObjectCount].Name = NULL;
			}
			else
			{
				Objects[ObjectCount].Name = (char*)malloc(StringCurSize);
				memcpy(Objects[ObjectCount].Name, StringTmp, StringCurSize);
			}
			ObjectsCur = &Objects[ObjectCount];

			ObjectCount++;

			if (ObjectCount >= ObjectSize)
			{
				ObjectSize += WAVE_GLTF_ALLOCATION_COUNT;
				Objects = (WaveGLTFObject*)realloc(Objects, ObjectSize * sizeof(WaveGLTFObject));
			}

			if (Objects[ObjectCount - 1].Name != NULL)
				printf("%s\n", Objects[ObjectCount - 1].Name);
		}

		else
		if (Buffer[i] == '}' ||
			Buffer[i] == ']')
		{
			WaveGLTFObject* Prev = (WaveGLTFObject*)ObjectsCur->PrevObject;
			ObjectsCur = Prev;
		}
	}

	return 0;
}

void WaveParseBas64(const char* Buffer, size_t Length)
{

}

void WaveParseGLTF(WaveGLTFObject* Objects)
{

}

WaveModelData WaveLoadGLTF(const char* FilePath, size_t Length, char* Buffer, uint32_t Settings)
{
	WaveModelData ModelData;
	ModelData.MaterialCount = 0;
	ModelData.Materials = &WaveEmptyMaterial;
	ModelData.MeshCount = 0;
	ModelData.Meshes = (WaveMeshData*)malloc(ModelData.MeshCount * sizeof(WaveMeshData));
	
	size_t JsonLength = Length;
	char* JsonPTR = Buffer;
	
	if (Buffer[0] == 'g' &&
		Buffer[1] == 'l' &&
		Buffer[2] == 'T' &&
		Buffer[3] == 'F')
	{
		JsonPTR = Buffer + 20;
		JsonLength -= 20;
	}
	
//	printf("%s\n", JsonPTR);

	size_t Offset = WaveParseJson(JsonPTR, JsonLength);
//	if (Offset < Length)
//		printf("%s\n", Buffer + Offset);

	size_t Base64Length = 0;
	char* Base64PTR = NULL;
	/*
	uint32_t ObjectCount = 0;
	uint32_t ObjectsAllocated = WAVE_GLTF_DEFAULT_STR_LENGTH;
	WaveGLTFObject* Objects = (WaveGLTFObject*)malloc(ObjectsAllocated * sizeof(WaveGLTFObject));
	WaveGLTFObject* CurrentObject = &Objects[0];

	size_t i;
	for (i = Length - 1; i > 0; i--)
		if (Buffer[i] == '}')
		{
			Length = i - 1;
			break;
		}
			
	//Find the first no name object
	for (i = 0; i < Length; i++)
	{
		if (Buffer[i] == '{')
		{
			CurrentObject = &Objects[ObjectCount];
			memset(&Objects[ObjectCount++], 0, sizeof(WaveGLTFObject));
			CurrentObject->Name = (char*)malloc(1);
			i++;
			break;
		}
	}

	char Begin = 0;
	uint32_t TempNameLength = 0;
	uint32_t TempNameAllocated = WAVE_GLTF_DEFAULT_STR_LENGTH;
	char* TempName = (char*)malloc(TempNameAllocated);

	for (; i < Length; i++)
	{
		if (Buffer[i] == '"')
		{
			WaveGetJsonString(&i, Buffer, &TempName, &TempNameAllocated, &TempNameLength);
			char DataType = WaveGetJsonDataType(&i, Buffer);
			printf(Buffer + i);
			exit(22);
		}
	}

	size_t j = 0;
	printf("Decode: %s\n", WaveBase64Decode("AACAPwAAgD8AAIC/AACAPwAAgD8AAIC/AACAPwAAgD8AAIC/AACAPwAAgL8AAIC/AACAPwAAgL8AAIC/AACAPwAAgL8AAIC/AACAPwAAgD8AAIA/AACAPwAAgD8AAIA/AACAPwAAgD8AAIA/AACAPwAAgL8AAIA/AACAPwAAgL8AAIA/AACAPwAAgL8AAIA/AACAvwAAgD8AAIC/AACAvwAAgD8AAIC/AACAvwAAgD8AAIC/AACAvwAAgL8AAIC/AACAvwAAgL8AAIC/AACAvwAAgL8AAIC/AACAvwAAgD8AAIA/AACAvwAAgD8AAIA/AACAvwAAgD8AAIA/AACAvwAAgL8AAIA/AACAvwAAgL8AAIA/AACAvwAAgL8AAIA/AAAAAAAAAAAAAIC/AAAAAAAAgD8AAACAAACAPwAAAAAAAACAAAAAAAAAgL8AAACAAAAAAAAAAAAAAIC/AACAPwAAAAAAAACAAAAAAAAAAAAAAIA/AAAAAAAAgD8AAACAAACAPwAAAAAAAACAAAAAAAAAgL8AAACAAAAAAAAAAAAAAIA/AACAPwAAAAAAAACAAACAvwAAAAAAAACAAAAAAAAAAAAAAIC/AAAAAAAAgD8AAACAAACAvwAAAAAAAACAAAAAAAAAgL8AAACAAAAAAAAAAAAAAIC/AACAvwAAAAAAAACAAAAAAAAAAAAAAIA/AAAAAAAAgD8AAACAAACAvwAAAAAAAACAAAAAAAAAgL8AAACAAAAAAAAAAAAAAIA/AAAgPwAAAD8AACA/AAAAPwAAID8AAAA/AADAPgAAAD8AAMA+AAAAPwAAwD4AAAA/AAAgPwAAgD4AACA/AACAPgAAID8AAIA+AADAPgAAgD4AAMA+AACAPgAAwD4AAIA+AAAgPwAAQD8AACA/AABAPwAAYD8AAAA/AADAPgAAQD8AAAA+AAAAPwAAwD4AAEA/AAAgPwAAgD8AACA/AAAAAAAAYD8AAIA+AADAPgAAgD8AAAA+AACAPgAAwD4AAAAAAQAOABQAAQAUAAcACgAGABMACgATABcAFQASAAwAFQAMAA8AEAADAAkAEAAJABYABQACAAgABQAIAAsAEQANAAAAEQAAAAQA", 1120, &j));

	free(TempName);
	*/
	return ModelData;
}

WaveModelData WaveLoadModel(const char* Path, uint32_t Settings)
{
	WaveLoadingStatus = 0;
	WaveModelData ModelData;
	uint32_t i;
	for (i = strlen(Path) - 1; i > 0; i--)
		if (Path[i] == '.') break;
	const char* Extension = Path + i;

	size_t FileSize = 0;
	char* Buffer = WaveLoadFile(Path, &FileSize);
	if (Buffer == NULL || FileSize == 0)
	{
		printf("Failed to open Model: %s\n", Path);

		ModelData.MaterialCount = 0;
		ModelData.Materials = NULL;
		ModelData.MeshCount = 0;
		ModelData.Meshes = NULL;
		return ModelData;
	}

	char* FilePath = (char*)malloc(strlen(Path));

	for (i = strlen(Path) - 1; i > 0; i--)
		if (Path[i] == '/' ||
			Path[i] == '\\') break;

	memcpy(FilePath, Path, i + 1);
	FilePath[i + 1] = '\0';

	WaveLoadingStatus = WAVE_STATUS_OPENED;

	if (strcmp(Extension + 1, "obj") == 0)
		ModelData = WaveLoadOBJ(FilePath, FileSize, Buffer, Settings);

	else if (strcmp(Extension + 1, "stl") == 0)
		ModelData = WaveLoadSTL(FilePath, FileSize, Buffer, Settings);

	else if (strcmp(Extension + 1, "gltf") == 0 ||
			 strcmp(Extension + 1, "glb") == 0)
		ModelData = WaveLoadGLTF(FilePath, FileSize, Buffer, Settings);
	else
		printf("%s format is not supported\n", Extension + 1);

	free(FilePath);
	free(Buffer);
	WaveLoadingStatus = WAVE_STATUS_CLOSED;

	WaveLoadingStatus = WAVE_STATUS_START_PPP;
	WaveRunPPP(&ModelData, Settings);
	WaveLoadingStatus = WAVE_STATUS_FINISHED;

	return ModelData;
}

void WaveFreeModel(WaveModelData* ModelData)
{
	if (ModelData->MaterialCount > 0)
		free(ModelData->Materials);

	for (uint32_t i = 0; i < ModelData->MeshCount; i++)
	{
		WaveMeshData* Mesh = &ModelData->Meshes[i];
		free(Mesh->Vertices);
		if (Mesh->IndexCount > 0)
			free(Mesh->Indices);
	}
	free(ModelData->Meshes);
}
