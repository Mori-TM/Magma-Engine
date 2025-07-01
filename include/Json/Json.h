#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <inttypes.h>
#include <stdarg.h>
#include <ctype.h>
#ifndef DYNAMIC_ARRAY_IMPLEMENTATION
#error you need DynamicArray for MallocSucks to work
#endif

//typedef uint32_t JsonRet;

typedef enum : uint32_t
{
	JSON_FALSE = 0,
	JSON_TRUE,
	JSON_ERROR = UINT32_MAX
} JsonRet;


JsonRet JsonRuntimeError(const char* Msg, ...)
{
	char Buf[2048];
	snprintf(Buf, 2048, "\x1B[91m[Renderer Error]\033[0m\t%s\n", Msg);
	Buf[2047] = '\0';

	va_list ArgList;
	va_start(ArgList, Msg);
	vprintf(Buf, ArgList);
	va_end(ArgList);

	return JSON_ERROR;
}

typedef enum : uint32_t
{
	JSON_INT = 0,
	JSON_DUB,
	JSON_STR,
	JSON_BOOL,
	JSON_OBJ,
	JSON_ARY,
	JSON_NULL,
	JSON_TYPE_COUNT
} JsonType;

typedef struct
{
	char* Name;
	JsonType Type;

	union
	{
		char* Str;
		int64_t Int;
		double Double;
		bool Boolean;
	//	JsonObject* Obj;
	//	JsonObject* Ary;
	} Data;
} JsonVariables;

typedef struct JsonObject_t
{
	char* Name;
	JsonType Type;
	JsonObject_t* PrevObj;
	DynamicArray Refrences;
} JsonObject;



typedef struct
{
	DynamicArray Objects;
} Json;

char* JsonSetName(const char* Name)
{
	size_t StrLen = strlen(Name);
	char* Str = (char*)malloc(StrLen + 2);
	if (!Str)
		return NULL;

	if (StrLen > 0)
		memcpy(Str, Name, StrLen);

	Str[StrLen] = 0;
	return Str;
}


void JsonPrintJsonTree(JsonObject* Root, size_t Level)
{
	if (Root == NULL)
		return;

	for (size_t j = 0; j < Level; j++)
		printf("\t");

	if (Root->Type == JSON_OBJ ||
		Root->Type == JSON_ARY)
	{
		if (Root->Type == JSON_OBJ)
			printf("%s {%zu}:\n", Root->Name, Root->Refrences.Size);
		else
			printf("%s [%zu]:\n", Root->Name, Root->Refrences.Size);
	}		
	else
	{
		JsonVariables* pVar = (JsonVariables*)Root;

		if (pVar->Type == JSON_STR)
			printf("Var[Str] : %s: %s\n", pVar->Name, pVar->Data.Str);
		else if (pVar->Type == JSON_DUB)
			printf("Var[Dub] : %s: %f\n", pVar->Name, pVar->Data.Double);
		else if (pVar->Type == JSON_INT)
			printf("Var[Int] : %s: %lld\n", pVar->Name, pVar->Data.Int);
		else if (pVar->Type == JSON_BOOL)
			printf("Var[Bool]: %s: %s\n", pVar->Name, (pVar->Data.Boolean ? "true" : "false"));
		else if (pVar->Type == JSON_NULL)
			printf("Var[NULL]: %s: NULL\n", pVar->Name);
	}

	// Recur on each child
	if (Root->Type == JSON_OBJ ||
		Root->Type == JSON_ARY)
	{
		for (size_t i = 0; i < Root->Refrences.Size; i++) 
		{
			JsonObject* Obj = (JsonObject*)DynamicArrayGetAt(&Root->Refrences, i);			
			JsonPrintJsonTree(Obj, Level + 1);
		}

		
	}
}

#define JSON_NO_NAME "Unamed"

JsonRet JsonParseBuffer(const size_t Length, const char* Buffer, Json* Jsn)
{
//	printf(Buffer);

	DynamicArray Objects; DynamicArrayCreate(&Objects, sizeof(JsonObject), "Json Objects");
	JsonObject Obj;
//	Obj.Name = JsonSetName("Default");
	Obj.Name = NULL;
	Obj.Type = JSON_OBJ;
	Obj.PrevObj = NULL;
	memset(&Obj.Refrences, 0, sizeof(DynamicArray));
//	DynamicArrayPush(&Objects, &Obj);

//	DynamicArray Variables = DynamicArrayCreate(sizeof(JsonVariables), "Json Variables");
	JsonVariables Var;
	Var.Type = JSON_TYPE_COUNT;
	Var.Data.Str = NULL;
//	DynamicArrayPush(&Variables, &Var);

	JsonObject* CurObj = NULL;
	JsonObject* PrevObj = NULL;

	size_t StrLen = 0;
	size_t StrAllocSize = 128;
	char* String = (char*)malloc(StrAllocSize);

	DynamicArray Names; DynamicArrayCreate(&Names, sizeof(char**), "Json Names");
//	char* pNameDefault = JsonSetName("Default");
//	DynamicArrayPush(&Names, (char**)&pNameDefault);

	bool ObjWithName = false;
	bool ObjWithName2 = false;

	for (size_t i = 0; i < Length; i++)
	{
		JsonObject Obj;
		Obj.Name = NULL;//Mem leak
		Obj.Type = JSON_OBJ;
		Obj.PrevObj = NULL;
		memset(&Obj.Refrences, 0, sizeof(DynamicArray));

		JsonVariables Var;
		Var.Type = JSON_TYPE_COUNT;
		Var.Data.Str = NULL;

		if (Buffer[i] == ':')
		{
			JsonVariables* Var = (JsonVariables*)DynamicArrayGetAt(&CurObj->Refrences, CurObj->Refrences.Size - 1);
			DynamicArrayPush(&Names, (char**)&Var->Data.Str);
			free(Var->Name);
			DynamicArrayPop(&CurObj->Refrences, CurObj->Refrences.Size - 1);

			ObjWithName = true;
			ObjWithName2 = true;
			while (Buffer[++i] == ' ');
		}

		char* p;

		{
			char Type = Buffer[i];
			switch (Type)
			{
			case '{':
			case '[':
				{
					if (ObjWithName && Names.Size > 0)
					{
						char* Name = *(char**)DynamicArrayGetAt(&Names, Names.Size - 1);
						Obj.Name = (Name);
						DynamicArrayPop(&Names, Names.Size - 1);
						ObjWithName = false;
					}
					else
					{
						Obj.Name = JsonSetName(JSON_NO_NAME);
					}

					if (Type == '{')
						Obj.Type = JSON_OBJ;
					else
						Obj.Type = JSON_ARY;

					DynamicArrayCreate(&Obj.Refrences, sizeof(JsonObject), "JS Obj in Obj");
					Obj.PrevObj = CurObj;

					if (Objects.Size == 0 || CurObj == NULL)
					{
						DynamicArrayPush(&Objects, &Obj);
						CurObj = (JsonObject*)DynamicArrayGetAt(&Objects, Objects.Size - 1);
					}
					else
					{
						DynamicArrayPush(&CurObj->Refrences, &Obj);
						CurObj = (JsonObject*)DynamicArrayGetAt(&CurObj->Refrences, CurObj->Refrences.Size - 1);
					}
				}				
				break;
			case '\"':
				{
					if (ObjWithName && Names.Size > 0)
					{
						char* Name = *(char**)DynamicArrayGetAt(&Names, Names.Size - 1);
						Var.Name = Name;
					}
					Var.Type = JSON_STR;

					StrLen = 0;
					while (Buffer[++i] != '\"')
					{
						if (Buffer[i] == '\\')
						{
							i++;
						}
						String[StrLen++] = Buffer[i];

						if (StrLen >= StrAllocSize)
						{
							StrAllocSize += 8192;
							char* TmpChr = (char*)realloc(String, StrAllocSize);
							if (TmpChr)
								String = TmpChr;
							else
							{
								while (Buffer[i] != '\"') i++;
								break;
							}
								
						}
					}
				//	while (StrLen == 31 && Buffer[i] != '\"') i++;
					String[StrLen] = 0;
					//	if (StrLen > 0)

					p = JsonSetName(String);
					Var.Data.Str = p;

					if (ObjWithName && Names.Size > 0)
					{
						DynamicArrayPop(&Names, Names.Size - 1);

						ObjWithName = false;

					//	DynamicArrayPush(&Variables, &Var);
					}
					else
					{
						Var.Name = JsonSetName(JSON_NO_NAME);
					}

					DynamicArrayPush(&CurObj->Refrences, &Var);
				}			
				break;
			case '-':
			case '+':
			case '0':
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9':
				{
					if (Names.Size > 0 && ObjWithName)
					{
						ObjWithName = false;
						char* Name = *(char**)DynamicArrayGetAt(&Names, Names.Size - 1);

						Var.Name = Name;
						DynamicArrayPop(&Names, Names.Size - 1);
					}
					else
					{
						Var.Name = JsonSetName(JSON_NO_NAME);
					}


					bool IsFloatingPointNum = false;

					StrLen = 0;
					while ((Buffer[i] != ' ' && Buffer[i] != '\n' && Buffer[i] != '\r' && Buffer[i] != ',' && Buffer[i] != '}' && Buffer[i] != ']') && StrLen < 31)
					{
						String[StrLen++] = Buffer[i++];
						if (Buffer[i] == '.' || Buffer[i] == 'e')
							IsFloatingPointNum = true;
					}
					while (StrLen == 31 && Buffer[i] != ' ' && Buffer[i] != '\n' && Buffer[i] != '\r' && Buffer[i] != ',' && Buffer[i] != '}' && Buffer[i] != ']') i++;
					String[StrLen] = 0;

					if (IsFloatingPointNum)
					{
						Var.Type = JSON_DUB;
						Var.Data.Double = strtod(String, &p);
					//	printf("HEy heehey: %s\n", String);
					}
					else
					{
						Var.Type = JSON_INT;
						Var.Data.Int = atoll(String);

					}


				//	DynamicArrayPush(&Variables, &Var);
					DynamicArrayPush(&CurObj->Refrences, &Var);
				}
				break;
			case 't':
			case 'f':
				{
					bool IsBoolTrue = (Buffer[i + 1] == 'r' && Buffer[i + 2] == 'u' && Buffer[i + 3] == 'e');
					bool IsBoolFalse = (Buffer[i + 1] == 'a' && Buffer[i + 2] == 'l' && Buffer[i + 3] == 's');

					if (IsBoolTrue || IsBoolFalse)
					{
						if (ObjWithName && Names.Size > 0)
						{
							ObjWithName = false;
							char* Name = *(char**)DynamicArrayGetAt(&Names, Names.Size - 1);
							Var.Name = Name;
							DynamicArrayPop(&Names, Names.Size - 1);
						}
						else
						{
							Var.Name = JsonSetName(JSON_NO_NAME);
						}

						Var.Type = JSON_BOOL;
						Var.Data.Boolean = IsBoolTrue;

					//	DynamicArrayPush(&Variables, &Var);
						DynamicArrayPush(&CurObj->Refrences, &Var);
						if (IsBoolTrue)
							i += 3;
						else
							i += 4;
					}
				}
				break;
					
			case 'n':
				{
					if (Buffer[i + 1] == 'u' && Buffer[i + 2] == 'l' && Buffer[i + 3] == 'l')
					{

						if (ObjWithName && Names.Size > 0)
						{
							ObjWithName = false;

							char* Name = *(char**)DynamicArrayGetAt(&Names, Names.Size - 1);
							Var.Name = Name;
							DynamicArrayPop(&Names, Names.Size - 1);
						}
						else
						{
							Var.Name = JsonSetName(JSON_NO_NAME);
						}

						Var.Type = JSON_NULL;
						Var.Data.Str = NULL;

					//	DynamicArrayPush(&Variables, &Var);

						DynamicArrayPush(&CurObj->Refrences, &Var);
						i += 3;
					}
				}				
				break;

			default:
				break;
			}
		}
	
		if (Buffer[i] == '}' ||
			Buffer[i] == ']')
		{
			ObjWithName2 = false;
			CurObj = CurObj->PrevObj;
		}
	}
	
	DynamicArrayDestroy(&Names);
	free(String);


	JsonObject* pObj = NULL;
	pObj = (JsonObject*)DynamicArrayGetAt(&Objects, 0);
//	JsonPrintJsonTree(pObj, 0);

	Jsn->Objects = Objects;

	return JSON_TRUE;
}

JsonRet JsonParseFile(const char* FileName, Json* Jsn)
{
	FILE* File = fopen(FileName, "rb");
	if (!File)
		return JsonRuntimeError("Failed to open: %s", FileName);

	fseek(File, 0, SEEK_END);
	size_t Length = ftell(File);
	fseek(File, 0, SEEK_SET);

	char* Buffer = (char*)malloc(Length + 1);
	if (!Buffer)
		return JsonRuntimeError("Failed to allocate buffer for: %s", FileName);

	fread(Buffer, 1, Length, File);
	Buffer[Length] = '\0';

	fclose(File);


	JsonRet Ret = JsonParseBuffer(Length, Buffer, Jsn);
	free(Buffer);
	return Ret;
}

void JsonFreeTree(JsonObject* Root)
{
	if (Root == NULL)
		return;

	if (Root->Type != JSON_OBJ &&
		Root->Type != JSON_ARY)
	{
		JsonVariables* pVar = (JsonVariables*)Root;

		if (pVar->Type == JSON_STR)
			free(pVar->Data.Str);
	}

//	printf("Nayme: %s\n", Root->Name);

	free(Root->Name);

	// Recur on each child
	if (Root->Type == JSON_OBJ ||
		Root->Type == JSON_ARY)
	{
		for (size_t i = 0; i < Root->Refrences.Size; i++)
		{
			JsonObject* Obj = (JsonObject*)DynamicArrayGetAt(&Root->Refrences, i);
			JsonFreeTree(Obj);
		}
		DynamicArrayDestroy(&Root->Refrences);
	}
}

void JsonDestroy(Json* Jsn)
{
	JsonObject* pObj = NULL;
	pObj = (JsonObject*)DynamicArrayGetAt(&Jsn->Objects, 0);
	JsonFreeTree(pObj);
	DynamicArrayDestroy(&Jsn->Objects);
}