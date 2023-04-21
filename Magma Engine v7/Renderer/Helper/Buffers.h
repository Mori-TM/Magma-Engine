uint32_t GridVertexBuffer;
uint32_t DirectionVertexBuffer;

uint32_t CameraVertexBuffer;
uint32_t CameraIndexBuffer;

uint32_t CubeVertexBuffer;
AABBData CubeAABB;

uint32_t PlaneVertexBuffer;
uint32_t PlaneIndexBuffer;
AABBData PlaneAABB;

uint32_t SphereVertexBuffer;
uint32_t SphereIndexBuffer;
AABBData SphereAABB;

uint32_t BeanVertexBuffer;
uint32_t BeanIndexBuffer;
AABBData BeanAABB;

void CreateBuffers()
{	
	GridVertexBuffer = OpenVkCreateVertexBuffer(ARRAY_SIZE(GridVertices) * sizeof(DebugVertex), GridVertices);
	DirectionVertexBuffer = OpenVkCreateVertexBuffer(ARRAY_SIZE(DirectionVertices) * sizeof(DebugVertex), DirectionVertices);

	CameraVertexBuffer = OpenVkCreateVertexBuffer(ARRAY_SIZE(CameraVertices) * sizeof(DebugVertex), CameraVertices);
	CameraIndexBuffer = OpenVkCreateIndexBuffer(ARRAY_SIZE(CameraIndices) * sizeof(uint32_t), CameraIndices);
	
	CubeVertexBuffer = OpenVkCreateVertexBuffer(ARRAY_SIZE(CubeVertices) * sizeof(SceneVertex), CubeVertices);
	GenerateAABB(&CubeAABB, ARRAY_SIZE(CubeVertices), (SceneVertex*)CubeVertices);

	PlaneVertexBuffer = OpenVkCreateVertexBuffer(ARRAY_SIZE(PlaneVertices) * sizeof(SceneVertex), PlaneVertices);
	PlaneIndexBuffer = OpenVkCreateIndexBuffer(ARRAY_SIZE(PlaneIndices) * sizeof(uint32_t), PlaneIndices);
	GenerateAABB(&PlaneAABB, ARRAY_SIZE(PlaneVertices), (SceneVertex*)PlaneVertices);

	CreateSphere(24, 24);
	SphereVertexBuffer = OpenVkCreateVertexBuffer(SphereVertexCount * sizeof(SceneVertex), SphereVertices);
	SphereIndexBuffer = OpenVkCreateIndexBuffer(SphereIndexCount * sizeof(uint32_t), SphereIndices);
	GenerateAABB(&SphereAABB, SphereVertexCount, SphereVertices);
	free(SphereVertices);
	free(SphereIndices);

	CreateBean(24, 24);
	BeanVertexBuffer = OpenVkCreateVertexBuffer(BeanVertexCount * sizeof(SceneVertex), BeanVertices);
	BeanIndexBuffer = OpenVkCreateIndexBuffer(BeanIndexCount * sizeof(uint32_t), BeanIndices);
	GenerateAABB(&BeanAABB, BeanVertexCount, BeanVertices);
	free(BeanVertices);
	free(BeanIndices);
}