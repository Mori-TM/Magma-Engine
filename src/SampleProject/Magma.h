#include <stdio.h>
#include <stdint.h>

#ifdef _WIN32
#define MAGMA_FUNCTION __declspec(dllexport)
#else
#define MAGMA_FUNCTION
#endif

typedef enum
{
	COMPONENT_TYPE_EMPTY = 0,
	COMPONENT_TYPE_MESH,
	COMPONENT_TYPE_MATERIAL,
	COMPONENT_TYPE_CAMERA,
	COMPONENT_TYPE_AUDIO,
	COMPONENT_TYPE_ANIMATION,
	COMPONENT_TYPE_LIGHT,
	COMPONENT_TYPE_COLLIDER,
	COMPONENT_TYPE_RIGIDBODY,
	COMPONENT_COUNT
} ComponentTypes;

static uint32_t (*AddEntity)(uint32_t) = NULL;

MAGMA_FUNCTION void MagmaLoad_AddEntity(uint32_t (*callback)(uint32_t)) 
{
    AddEntity = callback;
    // Now you can use the callback elsewhere in your DLL
    if (!AddEntity)
        printf("Failed to load: AddEntity\n");
}
