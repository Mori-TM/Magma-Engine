#define OPENVK_FRAMEWORK
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdarg.h>
#include <math.h>

//#define VK_NO_PROTOTYPES
#include <vulkan/vulkan.h>

void* Malloc(size_t Size)
{
	printf("Malloc\n");
	return malloc(Size);
}

void* Realloc(void* Data, size_t Size)
{
	printf("Realloc\n");
	return realloc(Data, Size);
}

void Free(void* Data)
{
	printf("Free\n");
	free(Data);
}

//#define free Free
//#define realloc Realloc
//#define malloc Malloc
#include "CMA.h"
//#undef free
//#undef realloc
//#undef malloc
#ifndef STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#endif
#ifndef STB_TRUETYPE_IMPLEMENTATION
#define STB_TRUETYPE_IMPLEMENTATION
#include "stb_truetype.h"
#endif

#include "OpenVK_Defines.h"
#include "OpenVK_Helper.h"

//FIX Vulkan
//don't use VK_PIPELINE_STAGE_ALL_COMMANDS_BIT 
#include "OpenVK_VkHelperFunc.h"
#include "OpenVK_VkRaytracing.h"
#include "OpenVK_VkRenderer.h"

#include "OpenVK_Functions.h"
#include "OpenVK_GUI.h"