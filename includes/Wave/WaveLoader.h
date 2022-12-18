#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdarg.h>
#include <float.h>
#include <math.h>

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
	WAVE_PRINT_DEBUG_INOFS = 256,
	WAVE_MAX = 0xffffffffui32,
} WaveSettings;

typedef struct
{
	char MaterialName[2048];

	WaveVec3 AmbientColor;
	WaveVec3 DiffuseColor;
	WaveVec3 SpecularColor;
	float SpecularExponent;
	float Dissolve;
	char AmbientTexture[4096];
	char DiffuseTexture[4096];
	char SpecularTexture[4096];
	char AlphaTexture[4096];
	char BumpTexture[4096];
	char NormalTexture[4096];
	char HeightTexture[4096];
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
	uint32_t VertexSize;
	uint32_t IndexSize;

	WaveModelMaterial* Material;

	uint32_t VertexCount;
	WaveVertexData* Vertices;

	uint32_t IndexCount;
	uint32_t* Indices;
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
	free(A->Vertices);

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
	qsort(ModelData->Meshes, ModelData->MeshCount, sizeof(WaveMeshData), WaveCompareMeshMaterials);

	for (uint32_t i = 0; i < ModelData->MeshCount; i++)
	{
		printf("%s\n", ModelData->Meshes[i].Material->DiffuseTexture);
	}
	

	uint32_t MeshCount = 0;
	WaveMeshData* NewMeshes = (WaveMeshData*)calloc(ModelData->MeshCount, sizeof(WaveMeshData));

	uint32_t MaterialCount = 0;
	WaveModelMaterial* NewMaterials = (WaveModelMaterial*)calloc(ModelData->MaterialCount, sizeof(WaveModelMaterial));

//	NewMeshes[0] = ModelData->Meshes[0];
//	NewMaterials[0] = ModelData->Materials[0];
	printf("\n\n");
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
		//	uint32_t OldVertexCount = A->VertexCount;
		//	uint32_t OldIndexCount = A->IndexCount;
		//
		//	A->VertexCount += B->VertexCount;
		//	A->IndexCount += B->IndexCount;
		//	A->VertexSize = A->VertexCount;
		//	A->IndexSize = A->IndexCount;
		//	A->Vertices = (WaveVertexData*)realloc(A->Vertices, A->VertexCount * sizeof(WaveVertexData));
		//	A->Indices = (uint32_t*)realloc(A->Indices, A->IndexCount * sizeof(uint32_t));
		//
		//	memcpy(A->Vertices + OldVertexCount, B->Vertices, B->VertexCount * sizeof(WaveVertexData));
		//	memcpy(A->Indices + OldIndexCount, B->Indices, B->IndexCount * sizeof(uint32_t));
			if (i == 0)
				NewMeshes[0] = *A;
			MeshCount++;
		//	WaveCombineMeshes(A, B, &NewMeshes[MeshCount++]);
		//	NewMeshes[MeshCount++] = *B;
			//	WaveMeshData* Mesh = &NewMeshes[MeshCount++];
			//	uint32_t OldVertexCount = Mesh->VertexCount;
			//	uint32_t OldIndexCount = Mesh->IndexCount;
			//	Mesh->VertexCount += B->VertexCount;
			//	Mesh->IndexCount += B->IndexCount;
			//	Mesh->VertexSize = Mesh->VertexCount;
			//	Mesh->IndexSize = Mesh->IndexCount;
			//	Mesh->Vertices = (WaveVertexData*)realloc(Mesh->Vertices, Mesh->VertexCount * sizeof(WaveVertexData));
			//	Mesh->Indices = (uint32_t*)realloc(Mesh->Indices, Mesh->IndexCount * sizeof(uint32_t));
			//	
			//	memcpy(Mesh->Vertices + OldVertexCount, B->Vertices, B->VertexCount * sizeof(WaveVertexData));
			//	memcpy(Mesh->Indices + OldIndexCount, B->Indices, B->IndexCount * sizeof(uint32_t));

			NewMaterials[MaterialCount++] = *B->Material;
		}
		else if (i == 0)
		{
			NewMeshes[MeshCount++] = *A;
			//MeshCount++;
			//NewMeshes[MeshCount++] = *B;
		//	WaveMeshData Tmp = NewMeshes[MeshCount];
		//	WaveCombineMeshes(A, B, &NewMeshes[MeshCount]);
		//	MeshCount++;
		//	WaveCombineMeshes(A, B, &NewMeshes[MeshCount++]);
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

//	qsort(ModelData->Materials, ModelData->MaterialCount, sizeof(WaveModelMaterial), WaveCompareMaterials);
}

void WaveRunPPP(WaveModelData* ModelData, uint32_t Settings)
{
	if (Settings & WAVE_FORCE_GEN_NORMALS)
	{
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
}

WaveModelMaterial WaveEmptyMaterial =
{
	"Default",
	{ 1.0, 1.0, 1.0 },
	{ 1.0, 1.0, 1.0 },
	{ 1.0, 1.0, 1.0 },
	1.0,
	1.0,
	"NoTexture",
	"NoTexture",
	"NoTexture",
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

char WaveLoadMTL(const char* Path, WaveModelData* Data)
{
	size_t MatLength = 0;
	char* MatBuffer = WaveLoadFile(Path, &MatLength);
	if (MatBuffer == NULL || MatLength == 0)
	{
		printf("Failed to load material file: %s\n", Path);
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

		Data->Materials = (WaveModelMaterial*)calloc(MaterialCount, sizeof(WaveModelMaterial));
		MaterialCount = -1;

		Line = WaveGetLine(MatBuffer, &OldBuffer);
		while (Line)
		{
			if (Line[0] == 'K' && Line[1] == 'a')
				WaveScan(Line, "Ka", "%f %f %f\n", &Data->Materials[MaterialCount].AmbientColor.x, &Data->Materials[MaterialCount].AmbientColor.y, &Data->Materials[MaterialCount].AmbientColor.z);
			else if (Line[0] == 'K' && Line[1] == 'd')
				WaveScan(Line, "Kd", "%f %f %f\n", &Data->Materials[MaterialCount].DiffuseColor.x, &Data->Materials[MaterialCount].DiffuseColor.y, &Data->Materials[MaterialCount].DiffuseColor.z);
			else if (Line[0] == 'K' && Line[1] == 's')
				WaveScan(Line, "Ks", "%f %f %f\n", &Data->Materials[MaterialCount].SpecularColor.x, &Data->Materials[MaterialCount].SpecularColor.y, &Data->Materials[MaterialCount].SpecularColor.z);
			else if (Line[0] == 'N' && Line[1] == 's')
				WaveScan(Line, "Ns", "%f\n", &Data->Materials[MaterialCount].SpecularExponent);
			else if (Line[0] == 'd' && Line[1] == ' ')
				WaveScan(Line, "d", "%f\n", &Data->Materials[MaterialCount].Dissolve);
			else if (Line[0] == 'm' && Line[1] == 'a' && Line[2] == 'p' && Line[3] == '_' && Line[4] == 'K' && Line[5] == 'a' && Line[6] == ' ')
				WaveScan(Line, "map_Ka", "%s\n", Data->Materials[MaterialCount].AmbientTexture);
			else if (Line[0] == 'm' && Line[1] == 'a' && Line[2] == 'p' && Line[3] == '_' && Line[4] == 'K' && Line[5] == 'd' && Line[6] == ' ')
				WaveScan(Line, "map_Kd", "%s\n", Data->Materials[MaterialCount].DiffuseTexture);
			else if (Line[0] == 'm' && Line[1] == 'a' && Line[2] == 'p' && Line[3] == '_' && Line[4] == 'K' && Line[5] == 's' && Line[6] == ' ')
				WaveScan(Line, "map_Ks", "%s\n", Data->Materials[MaterialCount].SpecularTexture);
			else if (Line[0] == 'm' && Line[1] == 'a' && Line[2] == 'p' && Line[3] == '_' && Line[4] == 'd' && Line[5] == ' ')
				WaveScan(Line, "map_d", "%s\n", Data->Materials[MaterialCount].AlphaTexture);
			else if (Line[0] == 'm' && Line[1] == 'a' && Line[2] == 'p' && Line[3] == '_' && Line[4] == 'B' && Line[5] == 'u')
				WaveScan(Line, "map_Bump", "%s\n", Data->Materials[MaterialCount].BumpTexture);
			else if (Line[0] == 'm' && Line[1] == 'a' && Line[2] == 'p' && Line[3] == '_' && Line[4] == 'D' && Line[5] == 'i')
				WaveScan(Line, "map_Disp", "%s\n", Data->Materials[MaterialCount].NormalTexture);
			else if (Line[0] == 'd' && Line[1] == 'i' && Line[2] == 's')
				WaveScan(Line, "disp", "%s\n", Data->Materials[MaterialCount].HeightTexture);
			else if (Line[0] == 'n' && Line[1] == 'e')
			{
				MaterialCount++;
				Data->Materials[MaterialCount] = WaveEmptyMaterial;
				WaveScan(Line, "newmtl", "%s\n", Data->Materials[MaterialCount].MaterialName);

			}

			Line = WaveGetLine(NULL, &OldBuffer);
		}

		Data->MaterialCount = MaterialCount + 1;
	}

	return 1;
}

WaveModelData WaveLoadOBJ(size_t Length, char* Buffer, uint32_t Settings)
{
	WaveModelData Data;

	//	uint32_t LineCount = 0;
	//	WaveLine* Lines = WaveGetLinesFromBuffer(Length, Buffer, &LineCount);

	uint32_t VertexCount = 0;
	uint32_t VertexTextureCount = 0;
	uint32_t VertexNormalCount = 0;
	uint32_t FaceCount = 0;
	uint32_t MaterialCount = 0;

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
	WaveVertexReference VertexReferences[4];

	char FoundMaterialFile = 0;

	uint32_t CurrentMaterialIndex = 0;

	WaveMeshData* Mesh = NULL;

	Line = WaveGetLine(Buffer, &OldBuffer);
	while (Line)
	{
		if (Line[0] == 'm' && Line[1] == 't' && (Settings & WAVE_LOAD_MATERIAL))
		{
			char MaterialFile[1024];
			WaveScan(Line, "mtllib", "%[^\r\n]%*c\r\n", MaterialFile);

			if (WaveLoadMTL(MaterialFile, &Data))
			{
				Data.MeshCount = Data.MaterialCount;
				Data.Meshes = (WaveMeshData*)malloc(Data.MeshCount * sizeof(WaveMeshData));

				for (uint32_t i = 0; i < Data.MeshCount; i++)
				{
					Data.Meshes[i].VertexSize = FaceCount * 3;
					Data.Meshes[i].Vertices = (WaveVertexData*)calloc(Data.Meshes[i].VertexSize, sizeof(WaveVertexData));
					Data.Meshes[i].VertexCount = 0;
					Data.Meshes[i].IndexCount = 0;
					Data.Meshes[i].IndexSize = 0;
					Data.Meshes[i].Indices = NULL;
				}
			}	
			else
			{
				Data.MeshCount = 1;
				Data.Meshes = (WaveMeshData*)malloc(Data.MeshCount * sizeof(WaveMeshData));
				Data.Meshes[0].VertexSize = FaceCount * 4 * 2;
				Data.Meshes[0].Vertices = (WaveVertexData*)calloc(Data.Meshes[0].VertexSize, sizeof(WaveVertexData));
				Data.Meshes[0].VertexCount = 0;
				Data.Meshes[0].IndexCount = 0;
				Data.Meshes[0].IndexSize = 0;
				Data.Meshes[0].Indices = NULL;
				Mesh = &Data.Meshes[0];
			}
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
				TempLine = strtok(NULL, " ");
			}

			for (uint32_t m = 1; m + 1 < VertexReferenceIndex; m++)
			{
				WaveVertexReference* p[3] = { &VertexReferences[0], &VertexReferences[m], &VertexReferences[m + 1] };
				//	printf("%d %d %d\n", VertexReferences[0].V, VertexReferences[m].V, VertexReferences[m + 1].V);

				WaveVec3 U = { WaveSub(Vertices[p[1]->V - 1], Vertices[p[0]->V - 1]) };
				WaveVec3 V = { WaveSub(Vertices[p[2]->V - 1], Vertices[p[0]->V - 1]) };
				WaveVec3 FaceNormal = WaveNormalize(WaveCross(U, V));

				for (uint32_t j = 0; j < 3; j++)
				{
					WaveVertexData Vertex;

					if (Mesh->VertexCount + 1 >= Mesh->VertexSize)
					{
						Mesh->VertexSize += FaceCount * 3;
						Mesh->Vertices = (WaveVertexData*)realloc(Mesh->Vertices, Mesh->VertexSize * sizeof(WaveVertexData));
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
						Vertex.Normals = FaceNormal;

					Mesh->Vertices[Mesh->VertexCount++] = Vertex;
				}
			}
		}

		Line = WaveGetLine(NULL, &OldBuffer);
	}

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

WaveModelData WaveLoadSTL(size_t Length, char* Buffer, uint32_t Settings)
{
	WaveSTLDescription* Description = (WaveSTLDescription*)Buffer;
	Buffer += sizeof(WaveSTLDescription);
	WaveSTLVertex* VertexArray = (WaveSTLVertex*)Buffer;

	uint32_t VertexCount = Description->Triangles * 3;

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


WaveModelData WaveLoadGLTF(size_t Length, char* Buffer, uint32_t Settings)
{
	WaveModelData ModelData;
	ModelData.MaterialCount = 0;
	ModelData.Materials = &WaveEmptyMaterial;
	ModelData.MeshCount = 0;
	ModelData.Meshes = (WaveMeshData*)malloc(ModelData.MeshCount * sizeof(WaveMeshData));
	
	for (size_t i = 0; i < Length; i++)
	{
		if (Buffer[i] == '{')
		{

		}

		if (Buffer[i] == '}')
		{

		}
	}

	return ModelData;
}

WaveModelData WaveLoadModel(const char* Path, uint32_t Settings)
{
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

	if (strcmp(Extension + 1, "obj") == 0)
		ModelData = WaveLoadOBJ(FileSize, Buffer, Settings);

	else if (strcmp(Extension + 1, "stl") == 0)
		ModelData = WaveLoadSTL(FileSize, Buffer, Settings);

	else if (strcmp(Extension + 1, "gltf") == 0)
		ModelData = WaveLoadGLTF(FileSize, Buffer, Settings);
	else
		printf("%s format is not supported\n", Extension + 1);

	free(Buffer);

	if (Settings & WAVE_PRINT_DEBUG_INOFS)
	{
		printf("Loaded Model\n");
		printf("Start post processing pipeline\n");

		WaveRunPPP(&ModelData, Settings);

		printf("Finished post processing pipeline\n");

	//	printf("Vertices: %d\n", ModelData.VertexCount);
	//	printf("Indices: %d\n\n", ModelData.IndexCount);
	}
	else
		WaveRunPPP(&ModelData, Settings);
	

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
//	free(ModelData->Vertices);
//	if (ModelData->IndexCount > 0)
//		free(ModelData->Indices);
}
