struct
{
	PFN_vkGetBufferDeviceAddressKHR vkGetBufferDeviceAddress;
	PFN_vkCreateAccelerationStructureKHR vkCreateAccelerationStructure;
	PFN_vkDestroyAccelerationStructureKHR vkDestroyAccelerationStructure;
	PFN_vkGetAccelerationStructureBuildSizesKHR vkGetAccelerationStructureBuildSizes;
	PFN_vkGetAccelerationStructureDeviceAddressKHR vkGetAccelerationStructureDeviceAddress;
	PFN_vkCmdBuildAccelerationStructuresKHR vkCmdBuildAccelerationStructures;
	PFN_vkBuildAccelerationStructuresKHR vkBuildAccelerationStructures;
	PFN_vkCmdTraceRaysKHR vkCmdTraceRays;
	PFN_vkGetRayTracingShaderGroupHandlesKHR vkGetRayTracingShaderGroupHandles;
	PFN_vkCreateRayTracingPipelinesKHR vkCreateRayTracingPipelines;
} KHR;

typedef struct
{
	uint64_t DeviceAddress;
	VkBuffer Handle;
	VkDeviceMemory Memory;
} VkRaytracingScratchBuffer;

typedef struct
{
	VkAccelerationStructureKHR Handle;
	uint64_t DeviceAddress;
	VkDeviceMemory Memory;
	VkBuffer Buffer;
} VkAccelerationStructure;

typedef struct
{
	uint32_t VertexCount;
	uint32_t IndexCount;
	VkAccelerationStructureGeometryKHR AccelerationStructureGeometry;
} VkRaytracingGeometryInfo;

typedef struct
{
	VkPhysicalDeviceRayTracingPipelinePropertiesKHR  RayTracingPipelineProperties;
	VkPhysicalDeviceAccelerationStructureFeaturesKHR AccelerationStructureFeatures;

	VkPhysicalDeviceBufferDeviceAddressFeatures EnabledBufferDeviceAddresFeatures;
	VkPhysicalDeviceRayTracingPipelineFeaturesKHR EnabledRayTracingPipelineFeatures;
	VkPhysicalDeviceAccelerationStructureFeaturesKHR EnabledAccelerationStructureFeatures;

	VkPhysicalDeviceFeatures2 PhysicalDeviceFeatures2;

	CMA_MemoryZone Geometry;
	CMA_MemoryZone Instances;
	VkAccelerationStructureInstanceKHR* InstanceStorage;

	CMA_MemoryZone BottomLevelAS;
	CMA_MemoryZone TopLevelAS;

	uint32_t ShaderGroupCount;
	VkRayTracingShaderGroupCreateInfoKHR* ShaderGroups;

	uint32_t ShaderBindingCount;
	uint32_t* ShaderBindings;
} VkRaytracerInfo;

VkRaytracerInfo VkRaytracer;

/*
* ....
* 
if (vkCreateDevice(VkRenderer.PhysicalDevice, &CreateInfo, NULL, &VkRenderer.Device) != VK_SUCCESS)
	return OpenVkRuntimeError("Failed to Create Device");
*/
void VkGetRaytracingFeatures(VkDeviceCreateInfo* DeviceCreateInfo)
{
	memset(&VkRaytracer, 0, sizeof(VkRaytracerInfo));
	VkRaytracer.Geometry = CMA_Create(sizeof(VkRaytracingGeometryInfo));
	VkRaytracer.Instances = CMA_Create(sizeof(VkAccelerationStructureInstanceKHR));
	VkRaytracer.BottomLevelAS = CMA_Create(sizeof(VkAccelerationStructure));
	VkRaytracer.TopLevelAS = CMA_Create(sizeof(VkAccelerationStructure));

	VkRaytracer.EnabledBufferDeviceAddresFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_BUFFER_DEVICE_ADDRESS_FEATURES;
	VkRaytracer.EnabledBufferDeviceAddresFeatures.bufferDeviceAddress = VK_TRUE;

	VkRaytracer.EnabledRayTracingPipelineFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_PIPELINE_FEATURES_KHR;
	VkRaytracer.EnabledRayTracingPipelineFeatures.rayTracingPipeline = VK_TRUE;
	VkRaytracer.EnabledRayTracingPipelineFeatures.pNext = &VkRaytracer.EnabledBufferDeviceAddresFeatures;

	VkRaytracer.EnabledAccelerationStructureFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ACCELERATION_STRUCTURE_FEATURES_KHR;
	VkRaytracer.EnabledAccelerationStructureFeatures.accelerationStructure = VK_TRUE;
	VkRaytracer.EnabledAccelerationStructureFeatures.pNext = &VkRaytracer.EnabledRayTracingPipelineFeatures;

	VkRenderer.DeviceExtensions[VkRenderer.DeviceExtensionCount++] = VK_KHR_ACCELERATION_STRUCTURE_EXTENSION_NAME;
	VkRenderer.DeviceExtensions[VkRenderer.DeviceExtensionCount++] = VK_KHR_RAY_TRACING_PIPELINE_EXTENSION_NAME;
	VkRenderer.DeviceExtensions[VkRenderer.DeviceExtensionCount++] = VK_KHR_BUFFER_DEVICE_ADDRESS_EXTENSION_NAME;
	VkRenderer.DeviceExtensions[VkRenderer.DeviceExtensionCount++] = VK_KHR_DEFERRED_HOST_OPERATIONS_EXTENSION_NAME;
	VkRenderer.DeviceExtensions[VkRenderer.DeviceExtensionCount++] = VK_EXT_DESCRIPTOR_INDEXING_EXTENSION_NAME;
	VkRenderer.DeviceExtensions[VkRenderer.DeviceExtensionCount++] = VK_KHR_SPIRV_1_4_EXTENSION_NAME;
	VkRenderer.DeviceExtensions[VkRenderer.DeviceExtensionCount++] = VK_KHR_SHADER_FLOAT_CONTROLS_EXTENSION_NAME;

	for (uint32_t i = 0; i < VkRenderer.DeviceExtensionCount; i++)
		printf("Ext: %s\n", VkRenderer.DeviceExtensions[i]);

	DeviceCreateInfo->enabledExtensionCount = VkRenderer.DeviceExtensionCount;
	DeviceCreateInfo->ppEnabledExtensionNames = VkRenderer.DeviceExtensions;

	VkRaytracer.PhysicalDeviceFeatures2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
	VkRaytracer.PhysicalDeviceFeatures2.pNext = &VkRaytracer.EnabledAccelerationStructureFeatures;
	VkRaytracer.PhysicalDeviceFeatures2.features = *DeviceCreateInfo->pEnabledFeatures;

	DeviceCreateInfo->pEnabledFeatures = NULL;
	DeviceCreateInfo->pNext = &VkRaytracer.PhysicalDeviceFeatures2;
}

/*
* After OpenVk create renderer
*/
void VkGetRaytracingInfos()
{
	VkRaytracer.RayTracingPipelineProperties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_PIPELINE_PROPERTIES_KHR;
	VkPhysicalDeviceProperties2 DeviceProperties2;
	DeviceProperties2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
	DeviceProperties2.pNext = &VkRaytracer.RayTracingPipelineProperties;
	vkGetPhysicalDeviceProperties2(VkRenderer.PhysicalDevice, &DeviceProperties2);

	VkRaytracer.AccelerationStructureFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ACCELERATION_STRUCTURE_FEATURES_KHR;
	VkPhysicalDeviceFeatures2 DeviceFeatures2;
	DeviceFeatures2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
	DeviceFeatures2.pNext = &VkRaytracer.AccelerationStructureFeatures;
	vkGetPhysicalDeviceFeatures2(VkRenderer.PhysicalDevice, &DeviceFeatures2);

	KHR.vkGetBufferDeviceAddress = (PFN_vkGetBufferDeviceAddressKHR)(vkGetDeviceProcAddr(VkRenderer.Device, "vkGetBufferDeviceAddressKHR"));
	KHR.vkCmdBuildAccelerationStructures = (PFN_vkCmdBuildAccelerationStructuresKHR)(vkGetDeviceProcAddr(VkRenderer.Device, "vkCmdBuildAccelerationStructuresKHR"));
	KHR.vkBuildAccelerationStructures = (PFN_vkBuildAccelerationStructuresKHR)(vkGetDeviceProcAddr(VkRenderer.Device, "vkBuildAccelerationStructuresKHR"));
	KHR.vkCreateAccelerationStructure = (PFN_vkCreateAccelerationStructureKHR)(vkGetDeviceProcAddr(VkRenderer.Device, "vkCreateAccelerationStructureKHR"));
	KHR.vkDestroyAccelerationStructure = (PFN_vkDestroyAccelerationStructureKHR)(vkGetDeviceProcAddr(VkRenderer.Device, "vkDestroyAccelerationStructureKHR"));
	KHR.vkGetAccelerationStructureBuildSizes = (PFN_vkGetAccelerationStructureBuildSizesKHR)(vkGetDeviceProcAddr(VkRenderer.Device, "vkGetAccelerationStructureBuildSizesKHR"));
	KHR.vkGetAccelerationStructureDeviceAddress = (PFN_vkGetAccelerationStructureDeviceAddressKHR)(vkGetDeviceProcAddr(VkRenderer.Device, "vkGetAccelerationStructureDeviceAddressKHR"));
	KHR.vkCmdTraceRays = (PFN_vkCmdTraceRaysKHR)(vkGetDeviceProcAddr(VkRenderer.Device, "vkCmdTraceRaysKHR"));
	KHR.vkGetRayTracingShaderGroupHandles = (PFN_vkGetRayTracingShaderGroupHandlesKHR)(vkGetDeviceProcAddr(VkRenderer.Device, "vkGetRayTracingShaderGroupHandlesKHR"));
	KHR.vkCreateRayTracingPipelines = (PFN_vkCreateRayTracingPipelinesKHR)(vkGetDeviceProcAddr(VkRenderer.Device, "vkCreateRayTracingPipelinesKHR"));

}

/*
* Raytracing Pipline
* Shader Binding Table
* Top Level Acceleration Structure
* Bottom Level Acceleration Structure
* get Buffer Device Address
* Acceleration Structure Buffer
* create/delete ScratchBuffer
*/

VkRaytracingScratchBuffer VkCreateScratchBuffer(VkDeviceSize Size)
{
	VkRaytracingScratchBuffer ScratchBuffer;

	VkBufferCreateInfo BufferCreateInfo;
	BufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	BufferCreateInfo.pNext = NULL;
	BufferCreateInfo.flags = 0;
	BufferCreateInfo.size = Size;
	BufferCreateInfo.usage = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT;
	BufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;//?
	BufferCreateInfo.queueFamilyIndexCount = 0;
	BufferCreateInfo.pQueueFamilyIndices = 0;
	if (vkCreateBuffer(VkRenderer.Device, &BufferCreateInfo, NULL, &ScratchBuffer.Handle) != VK_SUCCESS)
	{
		ScratchBuffer.DeviceAddress = OpenVkRuntimeError("Failed to create scratch buffer");
		return ScratchBuffer;
	}

	VkMemoryRequirements MemoryRequirements;
	vkGetBufferMemoryRequirements(VkRenderer.Device, ScratchBuffer.Handle, &MemoryRequirements);
	
	VkMemoryAllocateFlagsInfo MemoryAllocateFlagsInfo;
	MemoryAllocateFlagsInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_FLAGS_INFO;
	MemoryAllocateFlagsInfo.pNext = NULL;
	MemoryAllocateFlagsInfo.flags = VK_MEMORY_ALLOCATE_DEVICE_ADDRESS_BIT_KHR;
	MemoryAllocateFlagsInfo.deviceMask = 0;

	VkMemoryAllocateInfo MemoryAllocateInfo;
	MemoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	MemoryAllocateInfo.pNext = &MemoryAllocateFlagsInfo;
	MemoryAllocateInfo.allocationSize = MemoryRequirements.size;
	MemoryAllocateInfo.memoryTypeIndex = VkFindMemoryType(MemoryRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
	if (vkAllocateMemory(VkRenderer.Device, &MemoryAllocateInfo, NULL, &ScratchBuffer.Memory) != VK_SUCCESS)
	{
		ScratchBuffer.DeviceAddress = OpenVkRuntimeError("Failed to allocate scratch buffer");
		return ScratchBuffer;
	}

	if (vkBindBufferMemory(VkRenderer.Device, ScratchBuffer.Handle, ScratchBuffer.Memory, 0) != VK_SUCCESS)
	{
		ScratchBuffer.DeviceAddress = OpenVkRuntimeError("Failed to bind scratch buffer");
		return ScratchBuffer;
	}

	VkBufferDeviceAddressInfoKHR BufferDeviceAddressInfo;
	BufferDeviceAddressInfo.sType = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO;
	BufferDeviceAddressInfo.pNext = NULL;
	BufferDeviceAddressInfo.buffer = ScratchBuffer.Handle;
	ScratchBuffer.DeviceAddress = KHR.vkGetBufferDeviceAddress(VkRenderer.Device, &BufferDeviceAddressInfo);

	return ScratchBuffer;
}

void VkDestroyScratchBuffer(VkRaytracingScratchBuffer* ScratchBuffer)
{
	if (ScratchBuffer->Memory != VK_NULL_HANDLE) {
		vkFreeMemory(VkRenderer.Device, ScratchBuffer->Memory, NULL);
	}
	if (ScratchBuffer->Handle != VK_NULL_HANDLE) {
		vkDestroyBuffer(VkRenderer.Device, ScratchBuffer->Handle, NULL);
	}
}

OpenVkBool VkCreateAccelerationStructureBuffer(VkAccelerationStructure* AccelerationStructure, VkAccelerationStructureBuildSizesInfoKHR BuildSizeInfo)
{
	VkBufferCreateInfo BufferCreateInfo;
	BufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	BufferCreateInfo.pNext = NULL;
	BufferCreateInfo.flags = 0;
	BufferCreateInfo.size = BuildSizeInfo.accelerationStructureSize;
	BufferCreateInfo.usage = VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_STORAGE_BIT_KHR | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT;
	BufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;//?
	BufferCreateInfo.queueFamilyIndexCount = 0;
	BufferCreateInfo.pQueueFamilyIndices = 0;
	if (vkCreateBuffer(VkRenderer.Device, &BufferCreateInfo, NULL, &AccelerationStructure->Buffer) != VK_SUCCESS)
		return OpenVkRuntimeError("Failed to create acceleration structure buffer");
	
	VkMemoryRequirements MemoryRequirements;
	vkGetBufferMemoryRequirements(VkRenderer.Device, AccelerationStructure->Buffer, &MemoryRequirements);

	VkMemoryAllocateFlagsInfo MemoryAllocateFlagsInfo;
	MemoryAllocateFlagsInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_FLAGS_INFO;
	MemoryAllocateFlagsInfo.pNext = NULL;
	MemoryAllocateFlagsInfo.flags = VK_MEMORY_ALLOCATE_DEVICE_ADDRESS_BIT_KHR;
	MemoryAllocateFlagsInfo.deviceMask = 0;

	VkMemoryAllocateInfo MemoryAllocateInfo;
	MemoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	MemoryAllocateInfo.pNext = &MemoryAllocateFlagsInfo;
	MemoryAllocateInfo.allocationSize = MemoryRequirements.size;
	MemoryAllocateInfo.memoryTypeIndex = VkFindMemoryType(MemoryRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

	if (vkAllocateMemory(VkRenderer.Device, &MemoryAllocateInfo, NULL, &AccelerationStructure->Memory) != VK_SUCCESS)
		return OpenVkRuntimeError("Failed to allocate acceleration structure buffer");

	if (vkBindBufferMemory(VkRenderer.Device, AccelerationStructure->Buffer, AccelerationStructure->Memory, 0) != VK_SUCCESS)
		return OpenVkRuntimeError("Failed to bind acceleration structure buffer");

	return OpenVkTrue;
}

uint64_t VkGetBufferDeviceAddress(VkBuffer Buffer)
{
	VkBufferDeviceAddressInfoKHR BufferDeviceAI;
	BufferDeviceAI.sType = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO;
	BufferDeviceAI.pNext = NULL;
	BufferDeviceAI.buffer = Buffer;
	return KHR.vkGetBufferDeviceAddress(VkRenderer.Device, &BufferDeviceAI);
}

uint32_t VkCreateTranformBuffer(OpenVkTransformMatrix Matrix)
{
	return VkCreateBufferExt(VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
		VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
		sizeof(OpenVkTransformMatrix), &Matrix);
}

typedef struct
{
	uint32_t VertexCount; 
	uint32_t VertexBuffer; 
	uint32_t IndexCount; 
	uint32_t IndexBuffer; 
	uint32_t TranformBuffer; 
	uint32_t VertexFormat; 
	size_t	 VertexSize;
} OpenVkRaytracingGeometryCreateInfo;

//check if CMA_GetAt is NULL
uint32_t VkCreateRaytracingGeometry(uint32_t VertexFormat, size_t VertexSize, uint32_t VertexCount, uint32_t VertexBuffer, uint32_t IndexCount, uint32_t IndexBuffer, uint32_t TranformBuffer)
{
	VkDeviceOrHostAddressConstKHR VertexBufferDeviceAddress;
	VkDeviceOrHostAddressConstKHR IndexBufferDeviceAddress;
	VkDeviceOrHostAddressConstKHR TransformBufferDeviceAddress;
	
	VkStaticBufferInfo* Vertex = (VkStaticBufferInfo*)CMA_GetAt(&VkRenderer.StaticBuffers, VertexBuffer);
	VertexBufferDeviceAddress.deviceAddress = VkGetBufferDeviceAddress(Vertex->Buffer);

	if (IndexCount != 0)
	{
		VkStaticBufferInfo* Index = (VkStaticBufferInfo*)CMA_GetAt(&VkRenderer.StaticBuffers, IndexBuffer);
		IndexBufferDeviceAddress.deviceAddress = VkGetBufferDeviceAddress(Index->Buffer);
	}
	else
	{
		//works maybe
		IndexBufferDeviceAddress.deviceAddress = 0;
		IndexBufferDeviceAddress.hostAddress = NULL;
		IndexCount = VertexCount;
	}

	VkStaticBufferInfo* Transform = (VkStaticBufferInfo*)CMA_GetAt(&VkRenderer.StaticBuffers, TranformBuffer);
	TransformBufferDeviceAddress.deviceAddress = VkGetBufferDeviceAddress(Transform->Buffer);

	VkAccelerationStructureGeometryKHR AccelerationStructureGeometry;
	memset(&AccelerationStructureGeometry, 0, sizeof(VkAccelerationStructureGeometryKHR));
	AccelerationStructureGeometry.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_KHR;
	AccelerationStructureGeometry.pNext = NULL;
	AccelerationStructureGeometry.flags = VK_GEOMETRY_OPAQUE_BIT_KHR;
	AccelerationStructureGeometry.geometryType = VK_GEOMETRY_TYPE_TRIANGLES_KHR;
	AccelerationStructureGeometry.geometry.triangles.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_TRIANGLES_DATA_KHR;
	if (VertexFormat == 1) AccelerationStructureGeometry.geometry.triangles.vertexFormat = VK_FORMAT_R32_SFLOAT;
	if (VertexFormat == 2) AccelerationStructureGeometry.geometry.triangles.vertexFormat = VK_FORMAT_R32G32_SFLOAT;
	if (VertexFormat == 3) AccelerationStructureGeometry.geometry.triangles.vertexFormat = VK_FORMAT_R32G32B32_SFLOAT;
	if (VertexFormat == 4) AccelerationStructureGeometry.geometry.triangles.vertexFormat = VK_FORMAT_R32G32B32A32_SFLOAT;
	AccelerationStructureGeometry.geometry.triangles.vertexData = VertexBufferDeviceAddress;
	AccelerationStructureGeometry.geometry.triangles.maxVertex = VertexCount;
	AccelerationStructureGeometry.geometry.triangles.vertexStride = VertexSize;
	AccelerationStructureGeometry.geometry.triangles.indexType = VK_INDEX_TYPE_UINT32;
	AccelerationStructureGeometry.geometry.triangles.indexData = IndexBufferDeviceAddress;
	AccelerationStructureGeometry.geometry.triangles.transformData.deviceAddress = 0;
	AccelerationStructureGeometry.geometry.triangles.transformData.hostAddress = NULL;
	AccelerationStructureGeometry.geometry.triangles.transformData = TransformBufferDeviceAddress;

	VkRaytracingGeometryInfo GeometryInfo;
	GeometryInfo.AccelerationStructureGeometry = AccelerationStructureGeometry;
	GeometryInfo.VertexCount = VertexCount;
	GeometryInfo.IndexCount = IndexCount;

	return CMA_Push(&VkRaytracer.Geometry, &GeometryInfo);
}

uint32_t VkCreateBottomLevelAccelerationStructure(uint32_t InGeometry, OpenVkBool AllowUpdate, uint32_t* OldBottomLevelAS)
{
	VkRaytracingGeometryInfo* Geometry = (VkRaytracingGeometryInfo*)CMA_GetAt(&VkRaytracer.Geometry, InGeometry);
	if (Geometry == NULL)
		return OpenVkRuntimeError("Failed to find geometry");

	VkBuildAccelerationStructureFlagsKHR Flags = VK_BUILD_ACCELERATION_STRUCTURE_PREFER_FAST_TRACE_BIT_KHR;
	if (AllowUpdate)
		Flags |= VK_BUILD_ACCELERATION_STRUCTURE_ALLOW_UPDATE_BIT_KHR;

	// Get size info	
	VkAccelerationStructureBuildGeometryInfoKHR AccelerationStructureBuildGeometryInfo;
	AccelerationStructureBuildGeometryInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_GEOMETRY_INFO_KHR;
	AccelerationStructureBuildGeometryInfo.pNext = NULL;
	AccelerationStructureBuildGeometryInfo.type = VK_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL_KHR;
	AccelerationStructureBuildGeometryInfo.flags = Flags;
	AccelerationStructureBuildGeometryInfo.mode = (OldBottomLevelAS ? VK_BUILD_ACCELERATION_STRUCTURE_MODE_UPDATE_KHR : VK_BUILD_ACCELERATION_STRUCTURE_MODE_BUILD_KHR);
	AccelerationStructureBuildGeometryInfo.srcAccelerationStructure = 0;
	AccelerationStructureBuildGeometryInfo.dstAccelerationStructure = 0;
	AccelerationStructureBuildGeometryInfo.geometryCount = 1;
	AccelerationStructureBuildGeometryInfo.pGeometries = &Geometry->AccelerationStructureGeometry;
	AccelerationStructureBuildGeometryInfo.ppGeometries = NULL;
	AccelerationStructureBuildGeometryInfo.scratchData.deviceAddress = 0;
	AccelerationStructureBuildGeometryInfo.scratchData.hostAddress = NULL;

	const uint32_t NumTriangles = Geometry->IndexCount / 3;
	VkAccelerationStructureBuildSizesInfoKHR AccelerationStructureBuildSizesInfo;
	AccelerationStructureBuildSizesInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_SIZES_INFO_KHR;
	AccelerationStructureBuildSizesInfo.pNext = NULL;
	AccelerationStructureBuildSizesInfo.accelerationStructureSize = 0;
	AccelerationStructureBuildSizesInfo.updateScratchSize = 0;
	AccelerationStructureBuildSizesInfo.buildScratchSize = 0;
	
	VkAccelerationStructure BottomLevelAS;
	if (OldBottomLevelAS)
	{
		VkAccelerationStructure* BLAS = (VkAccelerationStructure*)CMA_GetAt(&VkRaytracer.TopLevelAS, *OldBottomLevelAS);
		BottomLevelAS = *BLAS;
	}

	KHR.vkGetAccelerationStructureBuildSizes(VkRenderer.Device, VK_ACCELERATION_STRUCTURE_BUILD_TYPE_DEVICE_KHR, &AccelerationStructureBuildGeometryInfo, &NumTriangles, &AccelerationStructureBuildSizesInfo);
	
	if (OldBottomLevelAS == NULL)
	{
		VkCreateAccelerationStructureBuffer(&BottomLevelAS, AccelerationStructureBuildSizesInfo);

		VkAccelerationStructureCreateInfoKHR AccelerationStructureCreateInfo;
		AccelerationStructureCreateInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_CREATE_INFO_KHR;
		AccelerationStructureCreateInfo.pNext = NULL;
		AccelerationStructureCreateInfo.createFlags = 0;
		AccelerationStructureCreateInfo.buffer = BottomLevelAS.Buffer;
		AccelerationStructureCreateInfo.offset = 0;
		AccelerationStructureCreateInfo.size = AccelerationStructureBuildSizesInfo.accelerationStructureSize;
		AccelerationStructureCreateInfo.type = VK_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL_KHR;
		AccelerationStructureCreateInfo.deviceAddress = 0;
		KHR.vkCreateAccelerationStructure(VkRenderer.Device, &AccelerationStructureCreateInfo, NULL, &BottomLevelAS.Handle);
	}
	
	VkRaytracingScratchBuffer ScratchBuffer = VkCreateScratchBuffer(AccelerationStructureBuildSizesInfo.buildScratchSize);
	
	AccelerationStructureBuildGeometryInfo.srcAccelerationStructure = (OldBottomLevelAS ? BottomLevelAS.Handle : 0);
	AccelerationStructureBuildGeometryInfo.dstAccelerationStructure = BottomLevelAS.Handle;
	AccelerationStructureBuildGeometryInfo.scratchData.deviceAddress = ScratchBuffer.DeviceAddress;

	VkAccelerationStructureBuildRangeInfoKHR AccelerationStructureBuildRangeInfo;
	AccelerationStructureBuildRangeInfo.primitiveCount = NumTriangles;
	AccelerationStructureBuildRangeInfo.primitiveOffset = 0;
	AccelerationStructureBuildRangeInfo.firstVertex = 0;
	AccelerationStructureBuildRangeInfo.transformOffset = 0;
	VkAccelerationStructureBuildRangeInfoKHR* AccelerationStructureBuildRangeInfoPP = &AccelerationStructureBuildRangeInfo;

	VkCommandBuffer CommandBuffer = VkBeginSingleTimeCommands();
	KHR.vkCmdBuildAccelerationStructures(CommandBuffer, 1, &AccelerationStructureBuildGeometryInfo, &AccelerationStructureBuildRangeInfoPP);
	VkEndSingleTimeCommandBuffer(CommandBuffer);

	VkAccelerationStructureDeviceAddressInfoKHR AccelerationDeviceAddressInfo;
	AccelerationDeviceAddressInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_DEVICE_ADDRESS_INFO_KHR;
	AccelerationDeviceAddressInfo.pNext = NULL;
	AccelerationDeviceAddressInfo.accelerationStructure = BottomLevelAS.Handle;
	BottomLevelAS.DeviceAddress = KHR.vkGetAccelerationStructureDeviceAddress(VkRenderer.Device, &AccelerationDeviceAddressInfo);

	VkDestroyScratchBuffer(&ScratchBuffer);

	if (OldBottomLevelAS)
		return *OldBottomLevelAS;

	return CMA_Push(&VkRaytracer.BottomLevelAS, &BottomLevelAS);
}

uint32_t VkCreateInstance(OpenVkTransformMatrix Matrix, OpenVkBool FrontCCWCulling, uint32_t BottomLevelAccelerationStructure)
{
	VkRaytracer.InstanceStorage = (VkAccelerationStructureInstanceKHR*)OpenVkRealloc(VkRaytracer.InstanceStorage, (VkRaytracer.Instances.Size + 1) * sizeof(VkAccelerationStructureInstanceKHR));

	VkAccelerationStructure* BottomLevelAS = (VkAccelerationStructure*)CMA_GetAt(&VkRaytracer.BottomLevelAS, BottomLevelAccelerationStructure);
	if (BottomLevelAS == NULL)
		return OpenVkRuntimeError("Failed to find bottom level as");

	VkAccelerationStructureInstanceKHR Instance;
	memcpy(&Instance.transform, &Matrix, sizeof(VkTransformMatrixKHR));
	Instance.instanceCustomIndex = 0;
	Instance.mask = 0xFF;
	Instance.instanceShaderBindingTableRecordOffset = 0;
	Instance.flags = (FrontCCWCulling ? VK_GEOMETRY_INSTANCE_TRIANGLE_FRONT_COUNTERCLOCKWISE_BIT_KHR : VK_GEOMETRY_INSTANCE_TRIANGLE_FACING_CULL_DISABLE_BIT_KHR);
	Instance.accelerationStructureReference = BottomLevelAS->DeviceAddress;

	return CMA_Push(&VkRaytracer.Instances, &Instance);
}

OpenVkBool VkUpdateInstance(OpenVkTransformMatrix Matrix, OpenVkBool FrontCCWCulling, uint32_t BottomLevelAccelerationStructure, uint32_t Instance)
{
	VkAccelerationStructureInstanceKHR* InstancePTR = (VkAccelerationStructureInstanceKHR*)CMA_GetAt(&VkRaytracer.Instances, Instance);
	if (InstancePTR == NULL)
		return OpenVkRuntimeError("Failed to find instance");

	VkAccelerationStructure* BottomLevelAS = (VkAccelerationStructure*)CMA_GetAt(&VkRaytracer.BottomLevelAS, BottomLevelAccelerationStructure);
	if (BottomLevelAS == NULL)
		return OpenVkRuntimeError("Failed to find bottom level as");

	memcpy(&InstancePTR->transform, &Matrix, sizeof(VkTransformMatrixKHR));
	InstancePTR->flags = (FrontCCWCulling ? VK_GEOMETRY_INSTANCE_TRIANGLE_FRONT_COUNTERCLOCKWISE_BIT_KHR : VK_GEOMETRY_INSTANCE_TRIANGLE_FACING_CULL_DISABLE_BIT_KHR);
	InstancePTR->accelerationStructureReference = BottomLevelAS->DeviceAddress;

	return OpenVkTrue;
}

typedef struct
{
	uint32_t	InsanceCount;
	uint32_t*	Instances;
	
	OpenVkBool	Update; 
	uint32_t*	OldTopLevelAS;
} OpenVkTopLevelAccelerationStructureCreateInfo;

OpenVkBool VkCreateTopLevelAccelerationStructure(uint32_t InstanceCount, uint32_t* Instances, OpenVkBool AllowUpdate, uint32_t* OldTopLevelAS)
{
	uint32_t InstanceASCount = 0;

	for (uint32_t i = 0; i < InstanceCount; i++)
	{
		VkAccelerationStructureInstanceKHR* Instance = (VkAccelerationStructureInstanceKHR*)CMA_GetAt(&VkRaytracer.Instances, Instances[i]);
		if (Instance != NULL)
		{
			VkRaytracer.InstanceStorage[InstanceASCount++] = *Instance;
		}
	}

	VkStaticBufferInfo InstancesBuffer;
	VkCreateVkBufferExt(VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
		VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT | VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
		InstanceASCount * sizeof(VkAccelerationStructureInstanceKHR), VkRaytracer.InstanceStorage, &InstancesBuffer.Buffer, &InstancesBuffer.BufferMemory);

	VkDeviceOrHostAddressConstKHR InstanceDataDeviceAddress;
	InstanceDataDeviceAddress.deviceAddress = VkGetBufferDeviceAddress(InstancesBuffer.Buffer);

	VkAccelerationStructureGeometryKHR AccelerationStructureGeometry;
	memset(&AccelerationStructureGeometry, 0, sizeof(VkAccelerationStructureGeometryKHR));
	AccelerationStructureGeometry.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_KHR;
	AccelerationStructureGeometry.pNext = NULL;
	AccelerationStructureGeometry.geometryType = VK_GEOMETRY_TYPE_INSTANCES_KHR;
	AccelerationStructureGeometry.flags = VK_GEOMETRY_OPAQUE_BIT_KHR;
	AccelerationStructureGeometry.geometry.instances.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_INSTANCES_DATA_KHR;
	AccelerationStructureGeometry.geometry.instances.pNext = NULL;
	AccelerationStructureGeometry.geometry.instances.arrayOfPointers = VK_FALSE;
	AccelerationStructureGeometry.geometry.instances.data = InstanceDataDeviceAddress;

	VkBuildAccelerationStructureFlagsKHR Flags = VK_BUILD_ACCELERATION_STRUCTURE_PREFER_FAST_TRACE_BIT_KHR;
	if (AllowUpdate)
		Flags |= VK_BUILD_ACCELERATION_STRUCTURE_ALLOW_UPDATE_BIT_KHR;

	VkAccelerationStructureBuildGeometryInfoKHR AccelerationStructureBuildGeometryInfo;
	AccelerationStructureBuildGeometryInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_GEOMETRY_INFO_KHR;
	AccelerationStructureBuildGeometryInfo.pNext = NULL;
	AccelerationStructureBuildGeometryInfo.type = VK_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL_KHR;
	AccelerationStructureBuildGeometryInfo.flags = Flags;
	AccelerationStructureBuildGeometryInfo.mode = (OldTopLevelAS ? VK_BUILD_ACCELERATION_STRUCTURE_MODE_UPDATE_KHR : VK_BUILD_ACCELERATION_STRUCTURE_MODE_BUILD_KHR);
	AccelerationStructureBuildGeometryInfo.srcAccelerationStructure = 0;
	AccelerationStructureBuildGeometryInfo.dstAccelerationStructure = 0;
	AccelerationStructureBuildGeometryInfo.geometryCount = 1;
	AccelerationStructureBuildGeometryInfo.pGeometries = &AccelerationStructureGeometry;
	AccelerationStructureBuildGeometryInfo.ppGeometries = NULL;
	AccelerationStructureBuildGeometryInfo.scratchData.deviceAddress = 0;

	VkAccelerationStructure TopLevelAS;
	if (OldTopLevelAS)
	{
		VkAccelerationStructure* TLAS = (VkAccelerationStructure*)CMA_GetAt(&VkRaytracer.TopLevelAS, *OldTopLevelAS);
		TopLevelAS = *TLAS;
	}

	const uint32_t PrimitiveCount = InstanceCount;
	VkAccelerationStructureBuildSizesInfoKHR AccelerationStructureBuildSizesInfo;
	memset(&AccelerationStructureBuildSizesInfo, 0, sizeof(VkAccelerationStructureBuildSizesInfoKHR));
	AccelerationStructureBuildSizesInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_SIZES_INFO_KHR;

	KHR.vkGetAccelerationStructureBuildSizes(
		VkRenderer.Device,
		VK_ACCELERATION_STRUCTURE_BUILD_TYPE_DEVICE_KHR,
		&AccelerationStructureBuildGeometryInfo,
		&PrimitiveCount,
		&AccelerationStructureBuildSizesInfo);

	if (OldTopLevelAS == NULL)
	{
		VkCreateAccelerationStructureBuffer(&TopLevelAS, AccelerationStructureBuildSizesInfo);

		VkAccelerationStructureCreateInfoKHR AccelerationStructureCreateInfo;
		AccelerationStructureCreateInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_CREATE_INFO_KHR;
		AccelerationStructureCreateInfo.pNext = NULL;
		AccelerationStructureCreateInfo.createFlags = 0;
		AccelerationStructureCreateInfo.buffer = TopLevelAS.Buffer;
		AccelerationStructureCreateInfo.offset = 0;
		AccelerationStructureCreateInfo.size = AccelerationStructureBuildSizesInfo.accelerationStructureSize;
		AccelerationStructureCreateInfo.type = VK_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL_KHR;
		AccelerationStructureCreateInfo.deviceAddress = 0;
		KHR.vkCreateAccelerationStructure(VkRenderer.Device, &AccelerationStructureCreateInfo, NULL, &TopLevelAS.Handle);
	}

	VkRaytracingScratchBuffer ScratchBuffer = VkCreateScratchBuffer(AccelerationStructureBuildSizesInfo.buildScratchSize);

	AccelerationStructureBuildGeometryInfo.srcAccelerationStructure = (OldTopLevelAS ? TopLevelAS.Handle : 0);
	AccelerationStructureBuildGeometryInfo.dstAccelerationStructure = TopLevelAS.Handle;
	AccelerationStructureBuildGeometryInfo.scratchData.deviceAddress = ScratchBuffer.DeviceAddress;

	VkAccelerationStructureBuildRangeInfoKHR AccelerationStructureBuildRangeInfo;
	AccelerationStructureBuildRangeInfo.primitiveCount = InstanceCount;
	AccelerationStructureBuildRangeInfo.primitiveOffset = 0;
	AccelerationStructureBuildRangeInfo.firstVertex = 0;
	AccelerationStructureBuildRangeInfo.transformOffset = 0;
	VkAccelerationStructureBuildRangeInfoKHR* AccelerationBuildStructureRangeInfos = &AccelerationStructureBuildRangeInfo;

	VkCommandBuffer commandBuffer = VkBeginSingleTimeCommands();
	KHR.vkCmdBuildAccelerationStructures(
		commandBuffer,
		1,
		&AccelerationStructureBuildGeometryInfo,
		&AccelerationBuildStructureRangeInfos);
	VkEndSingleTimeCommandBuffer(commandBuffer);

	VkAccelerationStructureDeviceAddressInfoKHR AccelerationDeviceAddressInfo;
	AccelerationDeviceAddressInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_DEVICE_ADDRESS_INFO_KHR;
	AccelerationDeviceAddressInfo.pNext = NULL;
	AccelerationDeviceAddressInfo.accelerationStructure = TopLevelAS.Handle;
	TopLevelAS.DeviceAddress = KHR.vkGetAccelerationStructureDeviceAddress(VkRenderer.Device, &AccelerationDeviceAddressInfo);
	
	VkDestroyScratchBuffer(&ScratchBuffer);
	vkDestroyBuffer(VkRenderer.Device, InstancesBuffer.Buffer, NULL);
	vkFreeMemory(VkRenderer.Device, InstancesBuffer.BufferMemory, NULL);

	if (OldTopLevelAS)
		return *OldTopLevelAS;

	return CMA_Push(&VkRaytracer.TopLevelAS, &TopLevelAS);
}

uint32_t VkCreateRaytracingPipeline(uint32_t MaxPipelineRayRecursionDepth, uint32_t PipelineLayout, uint32_t ShaderCount, uint32_t* ShaderTypes, OpenVkFile* Shader)
{
	VkRenderer.Pipelines = (VkPipeline*)OpenVkRealloc(VkRenderer.Pipelines, (VkRenderer.PipelineCount + 1) * sizeof(VkPipeline));
	VkPipelineShaderStageCreateInfo* ShaderStages = (VkPipelineShaderStageCreateInfo*)OpenVkMalloc(ShaderCount * sizeof(VkPipelineShaderStageCreateInfo));
	VkRaytracer.ShaderGroups = (VkRayTracingShaderGroupCreateInfoKHR*)OpenVkRealloc(VkRaytracer.ShaderGroups, (VkRaytracer.ShaderGroupCount + ShaderCount) * sizeof(VkRayTracingShaderGroupCreateInfoKHR));

	for (uint32_t i = 0; i < ShaderCount; i++)
	{
		VkShaderModule ShaderModule;
		if (VkCreateShaderModule(Shader[i], &ShaderModule) == OPENVK_ERROR)
			return OpenVkRuntimeError("Failed to create raytracing shader module");

		ShaderStages[i].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		ShaderStages[i].pNext = NULL;
		ShaderStages[i].flags = 0;
		if (ShaderTypes[i] == OPENVK_SHADER_TYPE_RAYGEN) ShaderStages[i].stage = VK_SHADER_STAGE_RAYGEN_BIT_KHR;
		if (ShaderTypes[i] == OPENVK_SHADER_TYPE_MISS) ShaderStages[i].stage = VK_SHADER_STAGE_MISS_BIT_KHR;
		if (ShaderTypes[i] == OPENVK_SHADER_TYPE_CLOSEST_HIT) ShaderStages[i].stage = VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR;
		ShaderStages[i].module = ShaderModule;
		ShaderStages[i].pName = "main";
		ShaderStages[i].pSpecializationInfo = NULL;

		VkRayTracingShaderGroupCreateInfoKHR* ShaderGroup = &VkRaytracer.ShaderGroups[VkRaytracer.ShaderGroupCount];
		ShaderGroup->sType = VK_STRUCTURE_TYPE_RAY_TRACING_SHADER_GROUP_CREATE_INFO_KHR;
		ShaderGroup->pNext = NULL;
		if (ShaderTypes[i] == OPENVK_SHADER_TYPE_RAYGEN || ShaderTypes[i] == OPENVK_SHADER_TYPE_MISS)
		{
			ShaderGroup->type = VK_RAY_TRACING_SHADER_GROUP_TYPE_GENERAL_KHR;
			ShaderGroup->generalShader = i;
			ShaderGroup->closestHitShader = VK_SHADER_UNUSED_KHR;
			ShaderGroup->anyHitShader = VK_SHADER_UNUSED_KHR;
			ShaderGroup->intersectionShader = VK_SHADER_UNUSED_KHR;
			ShaderGroup->pShaderGroupCaptureReplayHandle = NULL;
		}
			
		if (ShaderTypes[i] == OPENVK_SHADER_TYPE_CLOSEST_HIT)
		{
			ShaderGroup->type = VK_RAY_TRACING_SHADER_GROUP_TYPE_TRIANGLES_HIT_GROUP_KHR;
			ShaderGroup->generalShader = VK_SHADER_UNUSED_KHR;
			ShaderGroup->closestHitShader = i;
			ShaderGroup->anyHitShader = VK_SHADER_UNUSED_KHR;
			ShaderGroup->intersectionShader = VK_SHADER_UNUSED_KHR;
			ShaderGroup->pShaderGroupCaptureReplayHandle = NULL;
		}	
		
		VkRaytracer.ShaderGroupCount++;
	}

	VkRayTracingPipelineCreateInfoKHR PipelineCreateInfo;
	PipelineCreateInfo.sType = VK_STRUCTURE_TYPE_RAY_TRACING_PIPELINE_CREATE_INFO_KHR;
	PipelineCreateInfo.pNext = NULL;
	PipelineCreateInfo.flags = 0;
	PipelineCreateInfo.stageCount = ShaderCount;
	PipelineCreateInfo.pStages = ShaderStages;
	PipelineCreateInfo.groupCount = VkRaytracer.ShaderGroupCount;
	PipelineCreateInfo.pGroups = VkRaytracer.ShaderGroups;
	PipelineCreateInfo.maxPipelineRayRecursionDepth = MaxPipelineRayRecursionDepth;
	PipelineCreateInfo.pLibraryInfo = NULL;
	PipelineCreateInfo.pLibraryInterface = NULL;
	PipelineCreateInfo.pDynamicState = NULL;
	PipelineCreateInfo.layout = VkRenderer.PipelineLayouts[PipelineLayout];
	PipelineCreateInfo.basePipelineHandle = 0;
	PipelineCreateInfo.basePipelineIndex = 0;

	if (KHR.vkCreateRayTracingPipelines(VkRenderer.Device, VK_NULL_HANDLE, VK_NULL_HANDLE, 1, &PipelineCreateInfo, NULL, &VkRenderer.Pipelines[VkRenderer.PipelineCount]) != VK_SUCCESS)
		return OpenVkRuntimeError("Failed to create ray tracing pipeline");

	for (uint32_t i = 0; i < ShaderCount; i++)
		vkDestroyShaderModule(VkRenderer.Device, ShaderStages[i].module, NULL);

	OpenVkFree(ShaderStages);

	return VkRenderer.PipelineCount++;
}

uint32_t* VkCreateShaderBindingTable(uint32_t Pipeline, uint32_t ShaderCount)
{
	VkRaytracer.ShaderBindings = (uint32_t*)OpenVkRealloc(VkRaytracer.ShaderBindings, (VkRaytracer.ShaderBindingCount + ShaderCount) * sizeof(uint32_t));

	const uint32_t HandleSize = VkRaytracer.RayTracingPipelineProperties.shaderGroupHandleSize;
	const uint32_t HandleSizeAligned = OpenVkAlignedSize(VkRaytracer.RayTracingPipelineProperties.shaderGroupHandleSize, VkRaytracer.RayTracingPipelineProperties.shaderGroupHandleAlignment);
	const uint32_t GroupCount = VkRaytracer.ShaderGroupCount;
	const uint32_t SbtSize = GroupCount * HandleSizeAligned;

	uint8_t* ShaderHandleStorage = (uint8_t*)OpenVkMalloc(SbtSize);
	if (KHR.vkGetRayTracingShaderGroupHandles(VkRenderer.Device, VkRenderer.Pipelines[Pipeline], 0, GroupCount, SbtSize, ShaderHandleStorage) != VK_SUCCESS)
	{
		OpenVkRuntimeError("Failed to get ray tracing shader group handles");
		return NULL;
	}
		

	for (uint32_t i = 0; i < ShaderCount; i++)
	{
		VkRaytracer.ShaderBindings[VkRaytracer.ShaderBindingCount] = VkCreateBufferExt(VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
			VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_SHADER_BINDING_TABLE_BIT_KHR | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
			HandleSize, ShaderHandleStorage + (HandleSizeAligned * i));

		VkRaytracer.ShaderBindingCount++;
	}


	return VkRaytracer.ShaderBindings + (VkRaytracer.ShaderBindingCount - ShaderCount);
}

void VkTraceRays(uint32_t Width, uint32_t Height, uint32_t RaygenShader, uint32_t MissShader, uint32_t HitShader)
{
	const uint32_t HandleSizeAligned = OpenVkAlignedSize(VkRaytracer.RayTracingPipelineProperties.shaderGroupHandleSize, VkRaytracer.RayTracingPipelineProperties.shaderGroupHandleAlignment);

	VkStaticBufferInfo* RayBufferInfo = (VkStaticBufferInfo*)CMA_GetAt(&VkRenderer.StaticBuffers, RaygenShader);
	VkStaticBufferInfo* MissBufferInfo = (VkStaticBufferInfo*)CMA_GetAt(&VkRenderer.StaticBuffers, MissShader);
	VkStaticBufferInfo* HitBufferInfo = (VkStaticBufferInfo*)CMA_GetAt(&VkRenderer.StaticBuffers, HitShader);

	VkStridedDeviceAddressRegionKHR RaygenShaderSbtEntry;
	RaygenShaderSbtEntry.deviceAddress = VkGetBufferDeviceAddress(RayBufferInfo->Buffer);
	RaygenShaderSbtEntry.stride = HandleSizeAligned;
	RaygenShaderSbtEntry.size = HandleSizeAligned;

	VkStridedDeviceAddressRegionKHR MissShaderSbtEntry;
	MissShaderSbtEntry.deviceAddress = VkGetBufferDeviceAddress(MissBufferInfo->Buffer);
	MissShaderSbtEntry.stride = HandleSizeAligned;
	MissShaderSbtEntry.size = HandleSizeAligned;

	VkStridedDeviceAddressRegionKHR HitShaderSbtEntry;
	HitShaderSbtEntry.deviceAddress = VkGetBufferDeviceAddress(HitBufferInfo->Buffer);
	HitShaderSbtEntry.stride = HandleSizeAligned;
	HitShaderSbtEntry.size = HandleSizeAligned;

	VkStridedDeviceAddressRegionKHR CallableShaderSbtEntry;
	memset(&CallableShaderSbtEntry, 0, sizeof(VkStridedDeviceAddressRegionKHR));

	KHR.vkCmdTraceRays(
		VkRenderer.CommandBuffers[VkRenderer.ImageIndex],
		&RaygenShaderSbtEntry,
		&MissShaderSbtEntry,
		&HitShaderSbtEntry,
		&CallableShaderSbtEntry,
		Width,
		Height,
		1);
}

void VkDestroyRaytracing()
{
	for (uint32_t i = 0; i < VkRaytracer.BottomLevelAS.Size; i++)
	{
		VkAccelerationStructure* AS = (VkAccelerationStructure*)CMA_GetAt(&VkRaytracer.BottomLevelAS, i);
		if (AS != NULL)
		{
			vkFreeMemory(VkRenderer.Device, AS->Memory, NULL);
			vkDestroyBuffer(VkRenderer.Device, AS->Buffer, NULL);
			KHR.vkDestroyAccelerationStructure(VkRenderer.Device, AS->Handle, NULL);
		}
	}

	for (uint32_t i = 0; i < VkRaytracer.TopLevelAS.Size; i++)
	{
		VkAccelerationStructure* AS = (VkAccelerationStructure*)CMA_GetAt(&VkRaytracer.TopLevelAS, i);
		if (AS != NULL)
		{
			vkFreeMemory(VkRenderer.Device, AS->Memory, NULL);
			vkDestroyBuffer(VkRenderer.Device, AS->Buffer, NULL);
			KHR.vkDestroyAccelerationStructure(VkRenderer.Device, AS->Handle, NULL);
		}
	}

	CMA_Destroy(&VkRaytracer.BottomLevelAS);
	CMA_Destroy(&VkRaytracer.TopLevelAS);
	CMA_Destroy(&VkRaytracer.Instances);
	CMA_Destroy(&VkRaytracer.Geometry);
	OpenVkFree(VkRaytracer.ShaderBindings);
	OpenVkFree(VkRaytracer.InstanceStorage);
	OpenVkFree(VkRaytracer.ShaderGroups);
}