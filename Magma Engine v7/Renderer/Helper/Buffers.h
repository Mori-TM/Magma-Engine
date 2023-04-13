uint32_t GridVertexBuffer;
uint32_t DirectionVertexBuffer;

uint32_t CameraVertexBuffer;
uint32_t CameraIndexBuffer;

uint32_t CubeVertexBuffer;

uint32_t PlaneVertexBuffer;
uint32_t PlaneIndexBuffer;

uint32_t SphereVertexBuffer;
uint32_t SphereIndexBuffer;

uint32_t BeanVertexBuffer;
uint32_t BeanIndexBuffer;

void CreateBuffers()
{	
	GridVertexBuffer = OpenVkCreateVertexBuffer(ARRAY_SIZE(GridVertices) * sizeof(DebugVertex), GridVertices);
	DirectionVertexBuffer = OpenVkCreateVertexBuffer(ARRAY_SIZE(DirectionVertices) * sizeof(DebugVertex), DirectionVertices);

	CameraVertexBuffer = OpenVkCreateVertexBuffer(ARRAY_SIZE(CameraVertices) * sizeof(DebugVertex), CameraVertices);
	CameraIndexBuffer = OpenVkCreateIndexBuffer(ARRAY_SIZE(CameraIndices) * sizeof(uint32_t), CameraIndices);
	
	CubeVertexBuffer = OpenVkCreateVertexBuffer(ARRAY_SIZE(CubeVertices) * sizeof(SceneVertex), CubeVertices);

	PlaneVertexBuffer = OpenVkCreateVertexBuffer(ARRAY_SIZE(PlaneVertices) * sizeof(SceneVertex), PlaneVertices);
	PlaneIndexBuffer = OpenVkCreateIndexBuffer(ARRAY_SIZE(PlaneIndices) * sizeof(uint32_t), PlaneIndices);

	CreateSphere(24, 24);
	
	SphereVertexBuffer = OpenVkCreateVertexBuffer(SphereVertexCount * sizeof(SceneVertex), SphereVertices);
	SphereIndexBuffer = OpenVkCreateIndexBuffer(SphereIndexCount * sizeof(uint32_t), SphereIndices);

	CreateBean(24, 24);

	BeanVertexBuffer = OpenVkCreateVertexBuffer(BeanVertexCount * sizeof(SceneVertex), BeanVertices);
	BeanIndexBuffer = OpenVkCreateIndexBuffer(BeanIndexCount * sizeof(uint32_t), BeanIndices);
}

void DestroyBuffers()
{
	free(SphereVertices);
	free(SphereIndices);
	free(BeanVertices);
	free(BeanIndices);
}