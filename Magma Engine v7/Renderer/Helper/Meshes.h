/*
const QuadVertex QuadVertices[] =
{
//	{ {-1.0,-1.0 }, { 1.0, 0.0 }, },
//	{ { 1.0,-1.0 }, { 0.0, 0.0 }, },
//	{ { 1.0, 1.0 }, { 0.0, 1.0 }, },
//	{ {-1.0, 1.0 }, { 1.0, 1.0 }, }

	{ {-1.0f,  1.0f }, { 0.0f, 1.0f } },
	{ {-1.0f, -1.0f }, { 0.0f, 0.0f } },
	{ { 1.0f,  1.0f }, { 1.0f, 1.0f } },
	{ { 1.0f, -1.0f }, { 1.0f, 0.0f } },
};

const uint32_t QuadIndices[] =
{
	0, 1, 2,
	2, 3, 0
};
*/
DebugVertex GridVertices[] =
{
	{ {-10.0, 0.0, -10.0},{0.3, 0.3, 0.3} },
	{ {-10.0, 0.0, 10.0}, {0.3, 0.3, 0.3} },
	{ {-9.0, 0.0, -10.0}, {0.3, 0.3, 0.3} },
	{ {-9.0, 0.0, 10.0},  {0.3, 0.3, 0.3} },
	{ {-8.0, 0.0, -10.0}, {0.3, 0.3, 0.3} },
	{ {-8.0, 0.0, 10.0},  {0.3, 0.3, 0.3} },
	{ {-7.0, 0.0, -10.0}, {0.3, 0.3, 0.3} },
	{ {-7.0, 0.0, 10.0},  {0.3, 0.3, 0.3} },
	{ {-6.0, 0.0, -10.0}, {0.3, 0.3, 0.3} },
	{ {-6.0, 0.0, 10.0},  {0.3, 0.3, 0.3} },
	{ {-5.0, 0.0, -10.0}, {0.3, 0.3, 0.3} },
	{ {-5.0, 0.0, 10.0},  {0.3, 0.3, 0.3} },
	{ {-4.0, 0.0, -10.0}, {0.3, 0.3, 0.3} },
	{ {-4.0, 0.0, 10.0},  {0.3, 0.3, 0.3} },
	{ {-3.0, 0.0, -10.0}, {0.3, 0.3, 0.3} },
	{ {-3.0, 0.0, 10.0},  {0.3, 0.3, 0.3} },
	{ {-2.0, 0.0, -10.0}, {0.3, 0.3, 0.3} },
	{ {-2.0, 0.0, 10.0},  {0.3, 0.3, 0.3} },
	{ {-1.0, 0.0, -10.0}, {0.3, 0.3, 0.3} },
	{ {-1.0, 0.0, 10.0},  {0.3, 0.3, 0.3} },
	{ {-0.0, 0.0, -10.0}, {0.3, 0.3, 0.3} },
	{ {-0.0, 0.0, 10.0},  {0.3, 0.3, 0.3} },

	{ {1.0, 0.0, -10.0},  {0.3, 0.3, 0.3} },
	{ {1.0, 0.0, 10.0},   {0.3, 0.3, 0.3} },
	{ {2.0, 0.0, -10.0},  {0.3, 0.3, 0.3} },
	{ {2.0, 0.0, 10.0},   {0.3, 0.3, 0.3} },
	{ {3.0, 0.0, -10.0},  {0.3, 0.3, 0.3} },
	{ {3.0, 0.0, 10.0},   {0.3, 0.3, 0.3} },
	{ {4.0, 0.0, -10.0},  {0.3, 0.3, 0.3} },
	{ {4.0, 0.0, 10.0},   {0.3, 0.3, 0.3} },
	{ {5.0, 0.0, -10.0},  {0.3, 0.3, 0.3} },
	{ {5.0, 0.0, 10.0},   {0.3, 0.3, 0.3} },
	{ {6.0, 0.0, -10.0},  {0.3, 0.3, 0.3} },
	{ {6.0, 0.0, 10.0},   {0.3, 0.3, 0.3} },
	{ {7.0, 0.0, -10.0},  {0.3, 0.3, 0.3} },
	{ {7.0, 0.0, 10.0},   {0.3, 0.3, 0.3} },
	{ {8.0, 0.0, -10.0},  {0.3, 0.3, 0.3} },
	{ {8.0, 0.0, 10.0},   {0.3, 0.3, 0.3} },
	{ {9.0, 0.0, -10.0},  {0.3, 0.3, 0.3} },
	{ {9.0, 0.0, 10.0},   {0.3, 0.3, 0.3} },
	{ {10.0, 0.0, -10.0}, {0.3, 0.3, 0.3} },
	{ {10.0, 0.0, 10.0},  {0.3, 0.3, 0.3} },

	{ {-10.0, 0.0, -10.0},{0.3, 0.3, 0.3} },
	{ { 10.0, 0.0, -10.0},{0.3, 0.3, 0.3} },
	{ {-10.0, 0.0, -9.0}, {0.3, 0.3, 0.3} },
	{ { 10.0, 0.0, -9.0}, {0.3, 0.3, 0.3} },
	{ {-10.0, 0.0, -8.0}, {0.3, 0.3, 0.3} },
	{ { 10.0, 0.0, -8.0}, {0.3, 0.3, 0.3} },
	{ {-10.0, 0.0, -7.0}, {0.3, 0.3, 0.3} },
	{ { 10.0, 0.0, -7.0}, {0.3, 0.3, 0.3} },
	{ {-10.0, 0.0, -6.0}, {0.3, 0.3, 0.3} },
	{ { 10.0, 0.0, -6.0}, {0.3, 0.3, 0.3} },
	{ {-10.0, 0.0, -5.0}, {0.3, 0.3, 0.3} },
	{ { 10.0, 0.0, -5.0}, {0.3, 0.3, 0.3} },
	{ {-10.0, 0.0, -4.0}, {0.3, 0.3, 0.3} },
	{ { 10.0, 0.0, -4.0}, {0.3, 0.3, 0.3} },
	{ {-10.0, 0.0, -3.0}, {0.3, 0.3, 0.3} },
	{ { 10.0, 0.0, -3.0}, {0.3, 0.3, 0.3} },
	{ {-10.0, 0.0, -2.0}, {0.3, 0.3, 0.3} },
	{ { 10.0, 0.0, -2.0}, {0.3, 0.3, 0.3} },
	{ {-10.0, 0.0, -1.0}, {0.3, 0.3, 0.3} },
	{ { 10.0, 0.0, -1.0}, {0.3, 0.3, 0.3} },
	{ {-10.0, 0.0,  0.0}, {0.3, 0.3, 0.3} },
	{ { 10.0, 0.0,  0.0}, {0.3, 0.3, 0.3} },

	{ {-10.0, 0.0, 1.0},  {0.3, 0.3, 0.3} },
	{ { 10.0, 0.0, 1.0},  {0.3, 0.3, 0.3} },
	{ {-10.0, 0.0, 2.0},  {0.3, 0.3, 0.3} },
	{ { 10.0, 0.0, 2.0},  {0.3, 0.3, 0.3} },
	{ {-10.0, 0.0, 3.0},  {0.3, 0.3, 0.3} },
	{ { 10.0, 0.0, 3.0},  {0.3, 0.3, 0.3} },
	{ {-10.0, 0.0, 4.0},  {0.3, 0.3, 0.3} },
	{ { 10.0, 0.0, 4.0},  {0.3, 0.3, 0.3} },
	{ {-10.0, 0.0, 5.0},  {0.3, 0.3, 0.3} },
	{ { 10.0, 0.0, 5.0},  {0.3, 0.3, 0.3} },
	{ {-10.0, 0.0, 6.0},  {0.3, 0.3, 0.3} },
	{ { 10.0, 0.0, 6.0},  {0.3, 0.3, 0.3} },
	{ {-10.0, 0.0, 7.0},  {0.3, 0.3, 0.3} },
	{ { 10.0, 0.0, 7.0},  {0.3, 0.3, 0.3} },
	{ {-10.0, 0.0, 8.0},  {0.3, 0.3, 0.3} },
	{ { 10.0, 0.0, 8.0},  {0.3, 0.3, 0.3} },
	{ {-10.0, 0.0, 9.0},  {0.3, 0.3, 0.3} },
	{ { 10.0, 0.0, 9.0},  {0.3, 0.3, 0.3} },
	{ {-10.0, 0.0, 10.0}, {0.3, 0.3, 0.3} },
	{ { 10.0, 0.0, 10.0}, {0.3, 0.3, 0.3} },
};

const DebugVertex DirectionVertices[] =
{
	{ { 0.0,  0.0,  10.0 }, { 0.0, 0.0, 1.0 } },
	{ { 0.0,  0.0, -10.0 }, { 0.0, 0.0, 1.0 } },
	{ { 10.0, 0.0,  0.0  }, { 1.0, 0.0, 0.0 } },
	{ {-10.0, 0.0,  0.0  }, { 1.0, 0.0, 0.0 } },
	{ { 0.0,  10.0, 0.0  }, { 0.0, 1.0, 0.0 } },
	{ { 0.0, -10.0, 0.0  }, { 0.0, 1.0, 0.0 } },
};

const DebugVertex CameraVertices[] =
{
	{ { 1.125,  -3.0,  0.75 }, { 0.0, 0.0, 0.0 } },
	{ { 1.125,  -3.0,  -0.75 }, { 0.0, 0.0, 0.0 } },
	{ { -1.125,  -3.0,  -0.75 }, { 0.0, 0.0, 0.0 } },
	{ { -1.125,  -3.0,  0.75 }, { 0.0, 0.0, 0.0 } },	
	{ { 3.0,  3.0,  2.0 }, { 0.0, 0.0, 0.0 } },
	{ { 3.0,  3.0,  -2.0 }, { 0.0, 0.0, 0.0 } },
	{ { -3.0,  3.0,  -2.0 }, { 0.0, 0.0, 0.0 } },
	{ { -3.0,  3.0,  2.0 }, { 0.0, 0.0, 0.0 } },
};

const uint32_t CameraIndices[] =
{
	0, 1,
	1, 2,
	2, 3,
	3, 0,
	0, 4,
	1, 5,
	2, 6,
	3, 7,
	6, 5,
	5, 4,
	4, 7,
	7, 6,
	
};

const SceneVertex CubeVertices[] =
{
	{ {-0.5, -0.5,  0.5 }, { 0.0, 0.0 }, { 0.0, 0.0, 1.0 } },
	{ { 0.5, -0.5,  0.5 }, { 1.0, 0.0 }, { 0.0, 0.0, 1.0 } },
	{ {-0.5,  0.5,  0.5 }, { 0.0, 1.0 }, { 0.0, 0.0, 1.0 } },
	{ {-0.5,  0.5,  0.5 }, { 0.0, 1.0 }, { 0.0, 0.0, 1.0 } },
	{ { 0.5, -0.5,  0.5 }, { 1.0, 0.0 }, { 0.0, 0.0, 1.0 } },
	{ { 0.5,  0.5,  0.5 }, { 1.0, 1.0 }, { 0.0, 0.0, 1.0 } },//	
	{ {-0.5,  0.5,  0.5 }, { 0.0, 0.0 }, { 0.0, 1.0, 0.0 } },
	{ { 0.5,  0.5,  0.5 }, { 1.0, 0.0 }, { 0.0, 1.0, 0.0 } },
	{ {-0.5,  0.5, -0.5 }, { 0.0, 1.0 }, { 0.0, 1.0, 0.0 } },
	{ {-0.5,  0.5, -0.5 }, { 0.0, 1.0 }, { 0.0, 1.0, 0.0 } },
	{ { 0.5,  0.5,  0.5 }, { 1.0, 0.0 }, { 0.0, 1.0, 0.0 } },
	{ { 0.5,  0.5, -0.5 }, { 1.0, 1.0 }, { 0.0, 1.0, 0.0 } },//	
	{ {-0.5,  0.5, -0.5 }, { 1.0, 1.0 }, { 0.0, 0.0,-1.0 } },
	{ { 0.5,  0.5, -0.5 }, { 0.0, 1.0 }, { 0.0, 0.0,-1.0 } },
	{ {-0.5, -0.5, -0.5 }, { 1.0, 0.0 }, { 0.0, 0.0,-1.0 } },
	{ {-0.5, -0.5, -0.5 }, { 1.0, 0.0 }, { 0.0, 0.0,-1.0 } },
	{ { 0.5,  0.5, -0.5 }, { 0.0, 1.0 }, { 0.0, 0.0,-1.0 } },
	{ { 0.5, -0.5, -0.5 }, { 0.0, 0.0 }, { 0.0, 0.0,-1.0 } },//	
	{ {-0.5, -0.5, -0.5 }, { 0.0, 0.0 }, { 0.0,-1.0, 0.0 } },
	{ { 0.5, -0.5, -0.5 }, { 1.0, 0.0 }, { 0.0,-1.0, 0.0 } },
	{ {-0.5, -0.5,  0.5 }, { 0.0, 1.0 }, { 0.0,-1.0, 0.0 } },
	{ {-0.5, -0.5,  0.5 }, { 0.0, 1.0 }, { 0.0,-1.0, 0.0 } },
	{ { 0.5, -0.5, -0.5 }, { 1.0, 0.0 }, { 0.0,-1.0, 0.0 } },
	{ { 0.5, -0.5,  0.5 }, { 1.0, 1.0 }, { 0.0,-1.0, 0.0 } },//	
	{ { 0.5, -0.5,  0.5 }, { 0.0, 0.0 }, { 1.0, 0.0, 0.0 } },
	{ { 0.5, -0.5, -0.5 }, { 1.0, 0.0 }, { 1.0, 0.0, 0.0 } },
	{ { 0.5,  0.5,  0.5 }, { 0.0, 1.0 }, { 1.0, 0.0, 0.0 } },
	{ { 0.5,  0.5,  0.5 }, { 0.0, 1.0 }, { 1.0, 0.0, 0.0 } },
	{ { 0.5, -0.5, -0.5 }, { 1.0, 0.0 }, { 1.0, 0.0, 0.0 } },
	{ { 0.5,  0.5, -0.5 }, { 1.0, 1.0 }, { 1.0, 0.0, 0.0 } },//	
	{ {-0.5, -0.5, -0.5 }, { 0.0, 0.0 }, {-1.0, 0.0, 0.0 } },
	{ {-0.5, -0.5,  0.5 }, { 1.0, 0.0 }, {-1.0, 0.0, 0.0 } },
	{ {-0.5,  0.5, -0.5 }, { 0.0, 1.0 }, {-1.0, 0.0, 0.0 } },
	{ {-0.5,  0.5, -0.5 }, { 0.0, 1.0 }, {-1.0, 0.0, 0.0 } },
	{ {-0.5, -0.5,  0.5 }, { 1.0, 0.0 }, {-1.0, 0.0, 0.0 } },
	{ {-0.5,  0.5,  0.5 }, { 1.0, 1.0 }, {-1.0, 0.0, 0.0 } },
};

const SceneVertex PlaneVertices[] =
{
	{ {-0.5,  0.0,  0.5, },	{ 1.0, 1.0, }, { 0.0,  1.0,  0.0, } },
	{ { 0.5,  0.0,  0.5, },	{ 0.0, 1.0, }, { 0.0,  1.0,  0.0, } },
	{ { 0.5,  0.0, -0.5, },	{ 0.0, 0.0, }, { 0.0,  1.0,  0.0, } },
	{ {-0.5,  0.0, -0.5, },	{ 1.0, 0.0, }, { 0.0,  1.0,  0.0, } },
	
	
};

uint32_t PlaneIndices[] =
{
	0, 1, 2,
	2, 3, 0
};

uint32_t SphereVertexCount = 0;
SceneVertex* SphereVertices = NULL;
uint32_t SphereIndexCount = 0;
uint32_t* SphereIndices = NULL;

bool CreateSphere(uint32_t xSegments, uint32_t ySegments)
{
	SphereVertexCount = 0;
	SphereIndexCount = 0;
	SphereVertices = (SceneVertex*)malloc(((xSegments + 1) * (ySegments + 1)) * sizeof(SceneVertex));
	if (!SphereVertices)
	{
		printf("Failed to allocate vertices for sphere\n");
		return false;
	}
	SphereIndices = (uint32_t*)malloc((xSegments * ySegments * 6) * sizeof(uint32_t));
	if (!SphereIndices)
	{
		printf("Failed to allocate indices for sphere\n");
		free(SphereVertices);
		return false;
	}

	for (uint32_t y = 0; y <= ySegments; y++)
	{
		for (uint32_t x = 0; x <= xSegments; x++)
		{
			float xSegment = (float)x / (float)xSegments;
			float ySegment = (float)y / (float)ySegments;
			float xPos = cosf(xSegment * PI2) * sinf(ySegment * PI);
			float yPos = cosf(ySegment * PI);
			float zPos = sinf(xSegment * PI2) * sinf(ySegment * PI);

			SphereVertices[SphereVertexCount].Pos = Vec3(xPos * 0.5, yPos * 0.5, zPos * 0.5);
			SphereVertices[SphereVertexCount].TexCoord = Vec2(xSegment, ySegment);
			SphereVertices[SphereVertexCount].Normal = Vec3(xPos, yPos, zPos);
			SphereVertexCount++;
		}
	}

	for (uint32_t y = 0; y < ySegments; y++)
	{
		for (uint32_t x = 0; x < xSegments; x++)
		{
			SphereIndices[SphereIndexCount    ] = (y + 1) * (xSegments + 1) + x;
			SphereIndices[SphereIndexCount + 1] = y * (xSegments + 1) + x;
			SphereIndices[SphereIndexCount + 2] = y * (xSegments + 1) + x + 1;

			SphereIndices[SphereIndexCount + 3] = (y + 1) * (xSegments + 1) + x;
			SphereIndices[SphereIndexCount + 4] = y * (xSegments + 1) + x + 1;
			SphereIndices[SphereIndexCount + 5] = (y + 1) * (xSegments + 1) + x + 1;

			SphereIndexCount += 6;
		}
	}
}

uint32_t BeanVertexCount = 0;
SceneVertex* BeanVertices = NULL;
uint32_t BeanIndexCount = 0;
uint32_t* BeanIndices = NULL;

void CalculateRing(uint32_t Segments, float r, float y, float dy, float Radius, float Height) 
{
	float SegmentIncrease = 1.0 / (Segments - 1);

	for (uint32_t s = 0; s < Segments; s++) 
	{				//-cosf to change cull mode
		float x = cosf((PI * 2) * s * SegmentIncrease) * r;
		float z = sinf((PI * 2) * s * SegmentIncrease) * r;

		BeanVertices[BeanVertexCount].Pos = Vec3(Radius * x, Radius * y + Height * dy, Radius * z);
		BeanVertices[BeanVertexCount].Normal = Vec3(x, y, z);

		float u = (s * SegmentIncrease);
		float v = 0.5 - ((Radius * y + Height * dy) / (2.0 * Radius + Height));
		BeanVertices[BeanVertexCount].TexCoord = Vec2(u, 1.0 - v);

		BeanVertexCount++;
	}
}

bool CreateBean(uint32_t Segments, uint32_t SubdivisionsHeight)
{
	BeanVertexCount = 0;
	BeanIndexCount = 0;
	
	float Radius = 0.5;
	float Height = Radius * 2;

	uint32_t RingsBody = SubdivisionsHeight + 1;
	uint32_t RingsTotal = SubdivisionsHeight + RingsBody;

	uint32_t BeanVertexCountCalc = ((Segments * (SubdivisionsHeight / 2)) + (Segments * RingsBody) + (Segments * (SubdivisionsHeight / 2)));
	uint32_t BeanIndexCountCalc = (6 * (Segments - 1) * (RingsTotal - 1));

	BeanVertices = (SceneVertex*)malloc(BeanVertexCountCalc * sizeof(SceneVertex));
	if (!BeanVertices)
	{
		printf("Failed to allocate vertices for bean\n");
		return false;
	}

	BeanIndices = (uint32_t*)malloc(BeanIndexCountCalc * sizeof(uint32_t));
	if (!BeanIndices)
	{
		printf("Failed to allocate indices for bean\n");
		free(BeanVertices);
		return false;
	}

	float BodyIncrease = 1.0 / (RingsBody - 1);
	float RingIncrease = 1.0 / (SubdivisionsHeight - 1);
	for (uint32_t r = 0; r < SubdivisionsHeight / 2; r++)
		CalculateRing(Segments, sinf(PI * r * RingIncrease), sinf(PI * (r * RingIncrease - 0.5)), -0.5, Radius, Height);

	for (uint32_t r = 0; r < RingsBody; r++)
		CalculateRing(Segments, 1.0, 0.0, r * BodyIncrease - 0.5, Radius, Height);

	for (uint32_t r = SubdivisionsHeight / 2; r < SubdivisionsHeight; r++)
		CalculateRing(Segments, sinf(PI * r * RingIncrease), sinf(PI * (r * RingIncrease - 0.5)), 0.5, Radius, Height);

	for (uint32_t r = 0; r < RingsTotal - 1; r++) 
	{
		for (uint32_t s = 0; s < Segments - 1; s++) 
		{
			BeanIndices[BeanIndexCount] = r * Segments + (s + 1);
			BeanIndices[BeanIndexCount + 1] = r * Segments + (s + 0);
			BeanIndices[BeanIndexCount + 2] = (r + 1) * Segments + (s + 1);
			
			BeanIndices[BeanIndexCount + 3] = (r + 1) * Segments + (s + 0);
			BeanIndices[BeanIndexCount + 4] = (r + 1) * Segments + (s + 1);
			BeanIndices[BeanIndexCount + 5] = (r * Segments + s);

			BeanIndexCount += 6;
		}
	}
}