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
	WAVE_GEN_UVS = 1,
	WAVE_GEN_NORMALS = 2,
	WAVE_FLIP_UVS = 4,
	WAVE_LOAD_MATERIAL = 8
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
	uint32_t MaterialIndex;
	uint32_t VertexIndex;
} WaveMaterialRefrence;

typedef struct
{
	uint32_t MaterialIndex;
	WaveVec3 Vertices;
	WaveVec3 TexCoords;
	WaveVec3 Normals;
	WaveVec3 VertexColor;
} WaveVertexData;

typedef struct
{
	uint32_t MaterialCount;
	WaveModelMaterial* Materials;

	uint32_t MaterialRefrenceCount;
	WaveMaterialRefrence* MaterialRefrences;

	uint32_t VertexCount;
	WaveVertexData* Vertices;
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

void WaveGenUVs(WaveModelData* Data, uint32_t i)
{
	float MinX = 1000000000.0;
	float MaxX = -1000000000.0;

	float MinY = 1000000000.0;
	float MaxY = -1000000000.0;

	float TempUVsqrt = sqrtf(Data->Vertices[i].Vertices.x * Data->Vertices[i].Vertices.x + Data->Vertices[i].Vertices.y * Data->Vertices[i].Vertices.y + Data->Vertices[i].Vertices.z * Data->Vertices[i].Vertices.z);
	Data->Vertices[i].TexCoords.x = sinf(cosf(Data->Vertices[i].Vertices.x / TempUVsqrt));
	Data->Vertices[i].TexCoords.y = sinf(sinf(Data->Vertices[i].Vertices.y / TempUVsqrt));
}

void WaveGenSmoothNormals(WaveModelData* Data)
{
	WaveVec3 Normals;
	Normals.x = 0.0;
	Normals.y = 0.0;
	Normals.z = 0.0;

	for (uint32_t i = 0; i < Data->VertexCount; i++)
	{
		WaveVec3* Vertex = &Data->Vertices[i].Vertices;
		WaveVec3* Normal = &Data->Vertices[i].Normals;

		for (uint32_t j = 0; j < Data->VertexCount; j++)
		{
			WaveVec3* Vertex2 = &Data->Vertices[j].Vertices;

			WaveVec3 Edge = WaveSub(*Vertex2, *Vertex);
			float Length = WaveInvLength(Edge);

			if (Length > 0.0)
			{
				WaveVec3 x = { Length, Length, Length };
				Edge = WaveMul(Edge, x);

				float DotProduct = WaveDot(Edge, *Normal);

				if (DotProduct < 0.0)
				{
					Normals.x += -Edge.x;
					Normals.y += -Edge.y;
					Normals.z += -Edge.z;
				}
			}
		}

		Normals = WaveNormalize(Normals);

		Data->Vertices[i].Normals = Normals;
	}
}

void WaveGenNormals(WaveModelData* Data)
{
	for (uint32_t i = 0; i < Data->VertexCount; i += 3)
	{
		WaveVec3 N = WaveCross(WaveSub(Data->Vertices[i + 1].Vertices, Data->Vertices[i].Vertices), WaveSub(Data->Vertices[i + 2].Vertices, Data->Vertices[i].Vertices));

		Data->Vertices[i].Normals = N;
		Data->Vertices[i + 1].Normals = N;
		Data->Vertices[i + 2].Normals = N;

		Data->Vertices[i].Normals = WaveNormalize(Data->Vertices[i].Normals);
		Data->Vertices[i + 1].Normals = WaveNormalize(Data->Vertices[i + 1].Normals);
		Data->Vertices[i + 2].Normals = WaveNormalize(Data->Vertices[i + 2].Normals);
	}
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

void WaveLoadMTL(const char* Path, WaveModelData* Data)
{
	size_t MatLength = 0;
	char* MatBuffer = WaveLoadFile(Path, &MatLength);
	if (MatBuffer == NULL || MatLength == 0)
		printf("Failed to load material file: %s\n", Path);
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
	Data.MaterialRefrenceCount = 0;
	Data.MaterialRefrences = (WaveMaterialRefrence*)calloc(MaterialCount, sizeof(WaveMaterialRefrence));
	Data.Vertices = (WaveVertexData*)calloc(FaceCount * 4 * 2, sizeof(WaveVertexData));
	Data.VertexCount = 0;

	VertexCount = 0;
	VertexTextureCount = 0;
	VertexNormalCount = 0;
	MaterialCount = 0;

	uint32_t VertexReferenceIndex = 0;
	WaveVertexReference VertexReferences[4];

	char FoundMaterialFile = 0;

	uint32_t CurrentMaterialIndex = 0;

	Line = WaveGetLine(Buffer, &OldBuffer);
	while (Line)
	{
		if (Line[0] == 'm' && Line[1] == 't' && (Settings & WAVE_LOAD_MATERIAL))
		{
			char MaterialFile[1024];
			WaveScan(Line, "mtllib", "%[^\r\n]%*c\r\n", MaterialFile);
	
			WaveLoadMTL(MaterialFile, &Data);
		}

		if (Line[0] == 'u' && Line[1] == 's' && Data.MaterialCount != 0 && Settings & WAVE_LOAD_MATERIAL)
		{
			char MaterialName[2048];
			WaveScan(Line, "usemtl", "%s", MaterialName);

			for (uint32_t j = 0; j < Data.MaterialCount; j++)
				if (strcmp(Data.Materials[j].MaterialName, MaterialName) == 0)
					CurrentMaterialIndex = j;

			Data.MaterialRefrences[Data.MaterialRefrenceCount].VertexIndex = Data.VertexCount;
			Data.MaterialRefrences[Data.MaterialRefrenceCount].MaterialIndex = CurrentMaterialIndex;
			Data.MaterialRefrenceCount++;
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
					Data.Vertices[Data.VertexCount].Vertices = Vertices[p[j]->V - 1];
					Data.Vertices[Data.VertexCount].TexCoords = p[j]->VT != 0 ? VertexTextures[p[j]->VT - 1] : NullVec;
					Data.Vertices[Data.VertexCount].Normals = p[j]->VN != 0 ? VertexNormals[p[j]->VN - 1] : NullVec;
					Data.Vertices[Data.VertexCount].VertexColor.x = 1.0;
					Data.Vertices[Data.VertexCount].VertexColor.y = 1.0;
					Data.Vertices[Data.VertexCount].VertexColor.z = 1.0;
					Data.Vertices[Data.VertexCount].MaterialIndex = CurrentMaterialIndex;

					if ((Settings & WAVE_GEN_UVS) && p[j]->VT == 0)
						WaveGenUVs(&Data, Data.VertexCount);
					if ((Settings & WAVE_GEN_NORMALS) && p[j]->VN == 0)
						Data.Vertices[Data.VertexCount].Normals = FaceNormal;

					Data.VertexCount++;
				}
			}
		}

		Line = WaveGetLine(NULL, &OldBuffer);
	}

	

	Data.Vertices = (WaveVertexData*)realloc(Data.Vertices, Data.VertexCount * sizeof(WaveVertexData));

	free(VertexNormals);
	free(VertexTextures);
	free(Vertices);

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

	WaveModelData Data;
	Data.MaterialCount = 0;
	Data.Materials = &WaveEmptyMaterial;
	Data.MaterialRefrenceCount = 0;
	Data.MaterialRefrences = NULL;
	Data.Vertices = (WaveVertexData*)calloc(VertexCount, sizeof(WaveVertexData));
	Data.VertexCount = VertexCount;

	WaveVec3 DefaultColor = { 1.0, 1.0, 1.0 };

	uint32_t j = 0;
	for (uint32_t i = 0; i < Description->Triangles; i++)
	{
		Data.Vertices[j].Vertices = VertexArray[i].Vertex1;
		Data.Vertices[j + 1].Vertices = VertexArray[i].Vertex2;
		Data.Vertices[j + 2].Vertices = VertexArray[i].Vertex3;

		Data.Vertices[j].Normals = VertexArray[i].Normal;
		Data.Vertices[j + 1].Normals = VertexArray[i].Normal;
		Data.Vertices[j + 2].Normals = VertexArray[i].Normal;

		Data.Vertices[j].VertexColor = DefaultColor;
		Data.Vertices[j + 1].VertexColor = DefaultColor;
		Data.Vertices[j + 2].VertexColor = DefaultColor;

		if (Settings & WAVE_GEN_UVS)
		{
			WaveGenUVs(&Data, j);
			WaveGenUVs(&Data, j + 1);
			WaveGenUVs(&Data, j + 2);
		}

		j += 3;
	}

	//	WaveGenSmoothNormals(&Data);

	return Data;
}

typedef struct
{
	size_t BufBegin;
	size_t BufEnd;
	size_t ConBegin;
	size_t ConEnd;
	size_t Length;
} WaveNode;

WaveNode WaveGetNode(const char* Begin, const char* End, size_t Start, size_t Length, char* Buffer)
{
	WaveNode Node;
	Node.BufBegin = 0;
	Node.BufEnd = 0;
	Node.ConBegin = 0;
	Node.ConEnd = 0;
	Node.Length = 0;

	size_t BeginLength = strlen(Begin);
	size_t EndLength = strlen(End);

	size_t BufBeginNode = 0;
	size_t BufEndNode = 0;
	size_t ConBeginNode = 0;
	size_t ConEndNode = 0;
	size_t LengthNode = 0;

	for (size_t i = Start; i < Length; i++)
	{
		if (BufBeginNode == 0)
			for (size_t j = 0; j < BeginLength; j++)
			{
				if (Begin[j] == Buffer[i])
					BufBeginNode = i - j;
				else
				{
					BufBeginNode = 0;
					break;
				}
				i++;
			}



		if (BufEndNode == 0)
			for (size_t j = 0; j < EndLength; j++)
			{
				if (End[j] == Buffer[i])
					BufEndNode = i + 1;
				else
				{
					BufEndNode = 0;
					break;
				}
				i++;
			}



		if (BufBeginNode != 0 && BufEndNode != 0)
		{
			LengthNode = BufEndNode - BufBeginNode;
			break;
		}
	}

	//	printf("%d %d %d\n", BufBeginNode, BufEndNode, LengthNode);

	if (LengthNode == 0)
		return Node;

	Node.BufBegin = BufBeginNode;
	Node.BufEnd = BufEndNode;
	Node.Length = LengthNode;

	for (size_t i = Node.BufBegin + 1; i < Node.BufEnd; i++)
	{
		if (Buffer[i] == '>')
			ConBeginNode = i + 1;
		if (Buffer[i] == '<')
			ConEndNode = i;

		if (ConBeginNode != 0 && ConEndNode != 0)
			break;
	}

	Node.ConBegin = ConBeginNode;
	Node.ConEnd = ConEndNode;

	return Node;
}

typedef struct
{
	uint32_t FloatCount;
	float* Floats;
} WaveFloatArray;

WaveFloatArray WaveGetFloatsFromString(size_t Start, size_t Length, char* Buffer)
{
	WaveFloatArray FloatArray;
	FloatArray.FloatCount = 0;
	FloatArray.Floats = NULL;

	uint32_t FloatCount = 0;
	float* Floats = NULL;

	size_t FloatBufferSize = Length - Start;
	char* FloatBuffer = (char*)malloc(FloatBufferSize + 1);
	memcpy(FloatBuffer, Buffer + Start, FloatBufferSize);
	FloatBuffer[FloatBufferSize] = '\0';

	char* p = strtok(FloatBuffer, " ");
	while (p)
	{
		FloatCount++;

		p = strtok(NULL, " ");
	}

	if (FloatCount == 0)
		return FloatArray;

	Floats = (float*)calloc(FloatCount, sizeof(float));

	memcpy(FloatBuffer, Buffer + Start, FloatBufferSize);
	FloatBuffer[FloatBufferSize] = '\0';

	FloatCount = 0;

	p = strtok(FloatBuffer, " ");
	while (p)
	{
		Floats[FloatCount] = atof(p);
		FloatCount++;

		p = strtok(NULL, " ");
	}

	FloatArray.FloatCount = FloatCount;
	FloatArray.Floats = Floats;

	free(FloatBuffer);

	return FloatArray;
}

#define WAVE_MESH_MAX_SOURCES 32
typedef struct
{
	uint32_t Polygon;
	uint32_t ArrayCount;

	//0 = Indices
	//1 = Position
	//2 = Normals
	//3 = TexCoords
	//4 = VertexColors
	char UseNormals;
	char UseTextCoords;
	char UseVertexColors;
	WaveFloatArray MeshData[WAVE_MESH_MAX_SOURCES];
} WaveMeshDAE;

void WaveGetMeshInfos(size_t* Start, WaveNode* MeshNode, WaveMeshDAE* MeshData, char* Buffer)
{
	WaveNode Node = WaveGetNode("<triangles", "</triangles>", *Start, MeshNode->BufEnd, Buffer);
	MeshData->Polygon = 3;
	if (Node.Length == 0)
	{
		Node = WaveGetNode("<polylist", "</polylist>", *Start, MeshNode->BufEnd, Buffer);
		MeshData->Polygon = 4;
	}

	if (Node.Length != 0)
	{
		//FIX
		if (strstr(Buffer, "NORMAL") != NULL)
			MeshData->UseNormals = 1;
		if (strstr(Buffer, "TEXCOORD") != NULL)
			MeshData->UseTextCoords = 1;
		if (strstr(Buffer, "COLOR") != NULL)
			MeshData->UseVertexColors = 1;
	}
}

void WaveGetMeshData(size_t* Start, WaveNode* MeshNode, WaveMeshDAE* MeshData, char* Buffer)
{
	for (uint32_t i = 1; i < WAVE_MESH_MAX_SOURCES; i++)
	{
		WaveNode Node = WaveGetNode("<float_array", "</float_array>", *Start, MeshNode->BufEnd, Buffer);
		if (Node.Length == 0)
			break;

		MeshData->MeshData[i] = WaveGetFloatsFromString(Node.ConBegin, Node.ConEnd, Buffer);
		MeshData->ArrayCount++;

		*Start += Node.Length;
	}
}

void WaveGetMeshIndices(size_t* Start, WaveNode* MeshNode, WaveMeshDAE* MeshData, char* Buffer)
{
	WaveNode Node = WaveGetNode("<p>", "</p>", *Start, MeshNode->BufEnd, Buffer);
	MeshData->ArrayCount++;
	MeshData->MeshData[0] = WaveGetFloatsFromString(Node.ConBegin, Node.ConEnd, Buffer);
}

WaveMeshDAE WaveGetMeshDataDAE(WaveNode* MeshNode, char* Buffer)
{
	WaveMeshDAE MeshData;
	MeshData.Polygon = 0;
	MeshData.ArrayCount = 0;
	MeshData.UseNormals = 0;
	MeshData.UseTextCoords = 0;
	MeshData.UseVertexColors = 0;
	for (uint32_t i = 0; i < WAVE_MESH_MAX_SOURCES; i++)
	{
		MeshData.MeshData[i].FloatCount = 0;
		MeshData.MeshData[i].Floats = NULL;
	}

	size_t Begin = MeshNode->BufBegin;

	WaveGetMeshData(&Begin, MeshNode, &MeshData, Buffer);
	WaveGetMeshIndices(&Begin, MeshNode, &MeshData, Buffer);
	WaveGetMeshInfos(&Begin, MeshNode, &MeshData, Buffer);

	return MeshData;
}

WaveMeshDAE* WaveGetGeometry(uint32_t* MeshCount, WaveNode* Geometry, char* Buffer)
{
	WaveMeshDAE* Meshes;
	size_t MeshBegin = Geometry->BufBegin;
	uint32_t Count = 0;
	while (1)
	{
		WaveNode Node = WaveGetNode("<mesh>", "</mesh>", MeshBegin, Geometry->BufEnd, Buffer);
		if (Node.Length == 0)
			break;

		//	for (size_t i = Node.BufBegin; i < Node.BufEnd; i++)
		//		printf("%c", Buffer[i]);
		//	
		//	printf("\n");

		MeshBegin += Node.Length;
		Count++;
	}

	Meshes = (WaveMeshDAE*)calloc(Count, sizeof(WaveMeshDAE));

	Count = 0;
	MeshBegin = Geometry->BufBegin;
	while (1)
	{
		WaveNode Node = WaveGetNode("<mesh>", "</mesh>", MeshBegin, Geometry->BufEnd, Buffer);
		if (Node.Length == 0)
			break;

		Meshes[Count] = WaveGetMeshDataDAE(&Node, Buffer);

		MeshBegin += Node.Length;
		Count++;
	}

	*MeshCount = Count;

	return Meshes;
}

void WaveProccesMeshData(uint32_t MeshCount, WaveMeshDAE* Meshes, WaveModelData* Data, uint32_t Settings)
{
	uint32_t VertexReferenceIndex = 0;
	WaveVertexReference VertexReferences[4];

	for (uint32_t i = 0; i < MeshCount; i++)
		Data->VertexCount += (Meshes[i].MeshData[0].FloatCount / Meshes[i].ArrayCount) * Meshes[i].Polygon;

	Data->Vertices = (WaveVertexData*)calloc(Data->VertexCount, sizeof(WaveVertexData));
	Data->VertexCount = 0;

	for (uint32_t i = 0; i < MeshCount; i++)
	{
		WaveMeshDAE Mesh = Meshes[i];

		uint32_t Index = 0;
		for (uint32_t k = 0; k < Mesh.MeshData[0].FloatCount / (Mesh.ArrayCount - 1) / 3; k++)
		{
			int32_t v = 0;
			int32_t vt = 0;
			int32_t vn = 0;

			for (uint32_t j = 0; j < Mesh.Polygon; j++)
			{
				v = Mesh.MeshData[0].Floats[Index * 3];
				v = (v >= 0 ? v : Mesh.MeshData[1].FloatCount + v);
				VertexReferences[j].V = v;

				if (Mesh.UseNormals)
				{
					vn = Mesh.MeshData[0].Floats[Index * 3 + 1];
					vn = (vn >= 0 ? vn : Mesh.MeshData[2].FloatCount + vn);
					VertexReferences[j].VN = vn;
				}

				if (Mesh.UseTextCoords && Mesh.UseNormals)
				{
					vt = Mesh.MeshData[0].Floats[Index * 3 + 2];
					vt = (vt >= 0 ? vt : Mesh.MeshData[3].FloatCount + vt);
					VertexReferences[j].VT = vt;
				}
				else if (Mesh.UseTextCoords && !Mesh.UseNormals)
				{
					vt = Mesh.MeshData[0].Floats[Index * 3 + 1];
					vt = (vt >= 0 ? vt : Mesh.MeshData[2].FloatCount + vt);
					VertexReferences[j].VT = vt;
				}

				Index++;
			}

			for (uint32_t m = 1; m + 1 < Mesh.Polygon; m++)
			{
				WaveVertexReference* p[3] = { &VertexReferences[0], &VertexReferences[m], &VertexReferences[m + 1] };

				const WaveVec3 NullVec = { 0.0, 0.0, 0.0 };

				WaveVec3* Vertices = (WaveVec3*)Mesh.MeshData[1].Floats;
				WaveVec3* VertexNormals = NULL;
				float* VertexTextures = NULL;

				if (Mesh.UseNormals)
					VertexNormals = (WaveVec3*)Mesh.MeshData[2].Floats;
				if (Mesh.UseTextCoords && Mesh.UseNormals)
					VertexTextures = (float*)Mesh.MeshData[3].Floats;
				else if (Mesh.UseTextCoords && !Mesh.UseNormals)
					VertexTextures = (float*)Mesh.MeshData[2].Floats;

				WaveVec3 U = { WaveSub(*(Vertices + p[1]->V), *(Vertices + p[0]->V)) };
				WaveVec3 V = { WaveSub(*(Vertices + p[2]->V), *(Vertices + p[0]->V)) };
				WaveVec3 FaceNormal = WaveNormalize(WaveCross(U, V));

				for (uint32_t j = 0; j < 3; j++)
				{
					Data->Vertices[Data->VertexCount].Vertices = *(Vertices + p[j]->V);

					if (VertexTextures != NULL)
					{
						Data->Vertices[Data->VertexCount].TexCoords.x = *(VertexTextures + p[j]->VT);
						Data->Vertices[Data->VertexCount].TexCoords.y = *(VertexTextures + p[j]->VT + 1);
					}
					else
					{
						Data->Vertices[Data->VertexCount].TexCoords = NullVec;
					}

					Data->Vertices[Data->VertexCount].Normals = VertexNormals != NULL ? *(VertexNormals + p[j]->VN) : NullVec;
					//	Data->Vertices[Data->VertexCount].TexCoords = NullVec;
					//	Data->Vertices[Data->VertexCount].Normals = NullVec;
					Data->Vertices[Data->VertexCount].VertexColor.x = 1.0;
					Data->Vertices[Data->VertexCount].VertexColor.y = 1.0;
					Data->Vertices[Data->VertexCount].VertexColor.z = 1.0;

					Data->Vertices[Data->VertexCount].MaterialIndex = 0;

					//	if ((Settings & WAVE_GEN_UVS) && !Mesh.UseTextCoords)
					//		WaveGenUVs(Data, Data->VertexCount);
					//	if ((Settings & WAVE_GEN_NORMALS) && !Mesh.UseNormals)
					//		Data->Vertices[Data->VertexCount].Normals = FaceNormal;

					Data->VertexCount++;
				}
			}

		}
	}

	Data->Vertices = (WaveVertexData*)realloc(Data->Vertices, Data->VertexCount * sizeof(WaveVertexData));
}

WaveModelData WaveLoadDAE(size_t Length, char* Buffer, uint32_t Settings)
{
	WaveModelData Data;
	Data.MaterialCount = 0;
	Data.Materials = &WaveEmptyMaterial;
	Data.MaterialRefrenceCount = 0;
	Data.MaterialRefrences = NULL;
	Data.Vertices = NULL;
	Data.VertexCount = 0;

	WaveNode Geometry = WaveGetNode("<library_geometries>", "</library_geometries>", 0, Length, Buffer);
	uint32_t MeshCount = 0;
	WaveMeshDAE* Meshes = WaveGetGeometry(&MeshCount, &Geometry, Buffer);

	WaveProccesMeshData(MeshCount, Meshes, &Data, Settings);
	printf("MeshCount: %d\n", MeshCount);
	free(Meshes);

	//	for (uint32_t i = 0; i < MeshCount; i++)
	//	{
	//		WaveMeshDAE Mesh = Meshes[i];
	//		
	//		for (uint32_t j = 0; j < Mesh.ArrayCount; j++)
	//			for (uint32_t k = 0; k < Mesh.MeshData[j].FloatCount; k++)
	//				printf("%f", Mesh.MeshData[j].Floats[k]);
	//
	//		printf("\n");
	//	}

	return Data;
}

extern inline WaveModelData WaveLoadModel(const char* Path, uint32_t Settings)
{
	WaveModelData ModelData;
	const char* Extension = strchr(Path, '.');

	size_t FileSize = 0;
	char* Buffer = WaveLoadFile(Path, &FileSize);
	if (Buffer == NULL || FileSize == 0)
	{
		printf("Failed to open Model: %s\n", Path);

		ModelData.MaterialCount = 0;
		ModelData.Materials = NULL;
		ModelData.VertexCount = 0;
		ModelData.Vertices = NULL;
		return ModelData;
	}

	if (strcmp(Extension + 1, "obj") == 0)
		ModelData = WaveLoadOBJ(FileSize, Buffer, Settings);

	if (strcmp(Extension + 1, "stl") == 0)
		ModelData = WaveLoadSTL(FileSize, Buffer, Settings);

	if (strcmp(Extension + 1, "dae") == 0)
		ModelData = WaveLoadDAE(FileSize, Buffer, Settings);

	free(Buffer);

	return ModelData;
}

extern inline void WaveFreeModel(WaveModelData* ModelData)
{
	if (ModelData->MaterialCount > 0)
		free(ModelData->Materials);
	if (ModelData->MaterialRefrenceCount > 0)
		free(ModelData->MaterialRefrences);
	free(ModelData->Vertices);
}
