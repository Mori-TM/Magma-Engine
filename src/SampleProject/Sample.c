#include <stdio.h>
#include "Magma.h"

MAGMA_FUNCTION void Start()
{
    AddEntity(COMPONENT_TYPE_MESH);

    printf("Whats up from dll\n");
}

MAGMA_FUNCTION void Update()
{
}
