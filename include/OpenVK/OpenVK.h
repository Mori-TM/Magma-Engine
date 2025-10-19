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

#include "CMA.h"
#include "DynamicArray.h"
#ifndef STB_IMAGE_IMPLEMENTATION
//#define STB_IMAGE_IMPLEMENTATION
#endif
#include "stb_image.h"
#ifndef STB_TRUETYPE_IMPLEMENTATION
#define STB_TRUETYPE_IMPLEMENTATION
#endif
#include "stb_truetype.h"

#include "OpenVK_Defines.h"
#include "OpenVK_Helper.h"

#include "OpenVK_VkHelperFunc.h"
#include "OpenVK_VkRaytracing.h"
#include "OpenVK_VkRenderer.h"

#include "OpenVK_GLHelperFunc.h"
#include "OpenVK_GLRenderer.h"

#include "OpenVK_Functions.h"
#include "OpenVK_GUI.h"