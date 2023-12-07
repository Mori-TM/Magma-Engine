typedef struct
{
	vec3 Normal;
	float Distance;
} Plane;

typedef enum
{
	LEFT_PLANE = 0,
	RIGHT_PLANE = 1,
	TOP_PLANE = 2,
	BOTTOM_PLANE = 3,
	NEAR_PLANE = 4,
	FAR_PLANE = 5,
} FrustumPlanes;

typedef struct
{
	vec4 FrustumPlane[6];
} Frustum;

vec4 FrustumPlane[6];

void GetFrustum(mat4 ViewProj)
{
	float* ClipMat = (float*)&ViewProj;

	FrustumPlane[RIGHT_PLANE].x	 = ClipMat[3]  - ClipMat[0];
	FrustumPlane[RIGHT_PLANE].y	 = ClipMat[7]  - ClipMat[4];
	FrustumPlane[RIGHT_PLANE].z	 = ClipMat[11] - ClipMat[8];
	FrustumPlane[RIGHT_PLANE].w	 = ClipMat[15] - ClipMat[12];
	FrustumPlane[LEFT_PLANE].x	 = ClipMat[3]  + ClipMat[0];
	FrustumPlane[LEFT_PLANE].y	 = ClipMat[7]  + ClipMat[4];
	FrustumPlane[LEFT_PLANE].z	 = ClipMat[11] + ClipMat[8];
	FrustumPlane[LEFT_PLANE].w	 = ClipMat[15] + ClipMat[12];
	FrustumPlane[BOTTOM_PLANE].x = ClipMat[3]  + ClipMat[1];
	FrustumPlane[BOTTOM_PLANE].y = ClipMat[7]  + ClipMat[5];
	FrustumPlane[BOTTOM_PLANE].z = ClipMat[11] + ClipMat[9];
	FrustumPlane[BOTTOM_PLANE].w = ClipMat[15] + ClipMat[13];
	FrustumPlane[TOP_PLANE].x	 = ClipMat[3]  - ClipMat[1];
	FrustumPlane[TOP_PLANE].y	 = ClipMat[7]  - ClipMat[5];
	FrustumPlane[TOP_PLANE].z	 = ClipMat[11] - ClipMat[9];
	FrustumPlane[TOP_PLANE].w	 = ClipMat[15] - ClipMat[13];
	FrustumPlane[FAR_PLANE].x	 = ClipMat[3]  - ClipMat[2];
	FrustumPlane[FAR_PLANE].y	 = ClipMat[7]  - ClipMat[6];
	FrustumPlane[FAR_PLANE].z	 = ClipMat[11] - ClipMat[10];
	FrustumPlane[FAR_PLANE].w	 = ClipMat[15] - ClipMat[14];
	FrustumPlane[NEAR_PLANE].x	 = ClipMat[3]  + ClipMat[2];
	FrustumPlane[NEAR_PLANE].y	 = ClipMat[7]  + ClipMat[6];
	FrustumPlane[NEAR_PLANE].z	 = ClipMat[11] + ClipMat[10];
	FrustumPlane[NEAR_PLANE].w	 = ClipMat[15] + ClipMat[14];

	Normalize4P(&FrustumPlane[LEFT_PLANE]);
	Normalize4P(&FrustumPlane[RIGHT_PLANE]);
	Normalize4P(&FrustumPlane[TOP_PLANE]);
	Normalize4P(&FrustumPlane[BOTTOM_PLANE]);
	Normalize4P(&FrustumPlane[NEAR_PLANE]);
	Normalize4P(&FrustumPlane[FAR_PLANE]);

//	mat4 clipSpaceMatrix = TransposeMat4(&ViewProj);
//	FrustumPlane[0] = clipSpaceMatrix[3] + clipSpaceMatrix[0]; // left plane
//	FrustumPlane[1] = clipSpaceMatrix[3] - clipSpaceMatrix[0]; // right plane
//	FrustumPlane[3] = clipSpaceMatrix[3] - clipSpaceMatrix[1]; // top plane
//	FrustumPlane[2] = clipSpaceMatrix[3] + clipSpaceMatrix[1]; // bottom plane	
//	FrustumPlane[4] = clipSpaceMatrix[3] + clipSpaceMatrix[2]; // near plane
//	FrustumPlane[5] = clipSpaceMatrix[3] - clipSpaceMatrix[2]; // far plane
//
//	// Normalize the planes
//	for (int i = 0; i < 6; i++) {
//		frustumPlanes[i] /= glm::length(glm::vec3(frustumPlanes[i]));
//	}
	/*
	ViewProj = TransposeMat4(&ViewProj);
	FrustumPlane[LEFT_PLANE].x = ViewProj.m[3][0] + ViewProj.m[0][0];
	FrustumPlane[LEFT_PLANE].y = ViewProj.m[3][1] + ViewProj.m[0][1];
	FrustumPlane[LEFT_PLANE].z = ViewProj.m[3][2] + ViewProj.m[0][2];
	FrustumPlane[LEFT_PLANE].w = ViewProj.m[3][3] + ViewProj.m[0][3];
	Normalize4P(&FrustumPlane[LEFT_PLANE]);

	FrustumPlane[RIGHT_PLANE].x = ViewProj.m[3][0] - ViewProj.m[0][0];
	FrustumPlane[RIGHT_PLANE].y = ViewProj.m[3][1] - ViewProj.m[0][1];
	FrustumPlane[RIGHT_PLANE].z = ViewProj.m[3][2] - ViewProj.m[0][2];
	FrustumPlane[RIGHT_PLANE].w = ViewProj.m[3][3] - ViewProj.m[0][3];
	Normalize4P(&FrustumPlane[RIGHT_PLANE]);

	FrustumPlane[TOP_PLANE].x = ViewProj.m[3][0] - ViewProj.m[1][0];
	FrustumPlane[TOP_PLANE].y = ViewProj.m[3][1] - ViewProj.m[1][1];
	FrustumPlane[TOP_PLANE].z = ViewProj.m[3][2] - ViewProj.m[1][2];
	FrustumPlane[TOP_PLANE].w = ViewProj.m[3][3] - ViewProj.m[1][3];
	Normalize4P(&FrustumPlane[TOP_PLANE]);

	FrustumPlane[BOTTOM_PLANE].x = ViewProj.m[3][0] + ViewProj.m[1][0];
	FrustumPlane[BOTTOM_PLANE].y = ViewProj.m[3][1] + ViewProj.m[1][1];
	FrustumPlane[BOTTOM_PLANE].z = ViewProj.m[3][2] + ViewProj.m[1][2];
	FrustumPlane[BOTTOM_PLANE].w = ViewProj.m[3][3] + ViewProj.m[1][3];
	Normalize4P(&FrustumPlane[BOTTOM_PLANE]);

	FrustumPlane[NEAR_PLANE].x = ViewProj.m[3][0] + ViewProj.m[2][0];
	FrustumPlane[NEAR_PLANE].y = ViewProj.m[3][1] + ViewProj.m[2][1];
	FrustumPlane[NEAR_PLANE].z = ViewProj.m[3][2] + ViewProj.m[2][2];
	FrustumPlane[NEAR_PLANE].w = ViewProj.m[3][3] + ViewProj.m[2][3];
	Normalize4P(&FrustumPlane[NEAR_PLANE]);

	FrustumPlane[FAR_PLANE].x = ViewProj.m[3][0] - ViewProj.m[2][0];
	FrustumPlane[FAR_PLANE].y = ViewProj.m[3][1] - ViewProj.m[2][1];
	FrustumPlane[FAR_PLANE].z = ViewProj.m[3][2] - ViewProj.m[2][2];
	FrustumPlane[FAR_PLANE].w = ViewProj.m[3][3] - ViewProj.m[2][3];
	Normalize4P(&FrustumPlane[FAR_PLANE]);
	*/
	/*
	FrustumPlane[LEFT_PLANE].x = ViewProj.m[0][3] + ViewProj.m[0][0];
	FrustumPlane[LEFT_PLANE].y = ViewProj.m[1][3] + ViewProj.m[1][0];
	FrustumPlane[LEFT_PLANE].z = ViewProj.m[2][3] + ViewProj.m[2][0];
	FrustumPlane[LEFT_PLANE].w = ViewProj.m[3][3] + ViewProj.m[3][0];
	Normalize4P(&FrustumPlane[LEFT_PLANE]);

	FrustumPlane[RIGHT_PLANE].x = ViewProj.m[0][3] - ViewProj.m[0][0];
	FrustumPlane[RIGHT_PLANE].y = ViewProj.m[1][3] - ViewProj.m[1][0];
	FrustumPlane[RIGHT_PLANE].z = ViewProj.m[2][3] - ViewProj.m[2][0];
	FrustumPlane[RIGHT_PLANE].w = ViewProj.m[3][3] - ViewProj.m[3][0];
	Normalize4P(&FrustumPlane[RIGHT_PLANE]);

	FrustumPlane[TOP_PLANE].x = ViewProj.m[0][3] - ViewProj.m[0][1];
	FrustumPlane[TOP_PLANE].y = ViewProj.m[1][3] - ViewProj.m[1][1];
	FrustumPlane[TOP_PLANE].z = ViewProj.m[2][3] - ViewProj.m[2][1];
	FrustumPlane[TOP_PLANE].w = ViewProj.m[3][3] - ViewProj.m[3][1];
	Normalize4P(&FrustumPlane[TOP_PLANE]);

	FrustumPlane[BOTTOM_PLANE].x = ViewProj.m[0][3] + ViewProj.m[0][1];
	FrustumPlane[BOTTOM_PLANE].y = ViewProj.m[1][3] + ViewProj.m[1][1];
	FrustumPlane[BOTTOM_PLANE].z = ViewProj.m[2][3] + ViewProj.m[2][1];
	FrustumPlane[BOTTOM_PLANE].w = ViewProj.m[3][3] + ViewProj.m[3][1];
	Normalize4P(&FrustumPlane[BOTTOM_PLANE]);

	FrustumPlane[NEAR_PLANE].x = ViewProj.m[0][2];
	FrustumPlane[NEAR_PLANE].y = ViewProj.m[1][2];
	FrustumPlane[NEAR_PLANE].z = ViewProj.m[2][2];
	FrustumPlane[NEAR_PLANE].w = ViewProj.m[3][2];
	Normalize4P(&FrustumPlane[NEAR_PLANE]);

	FrustumPlane[FAR_PLANE].x = ViewProj.m[0][3] - ViewProj.m[0][2];
	FrustumPlane[FAR_PLANE].y = ViewProj.m[1][3] - ViewProj.m[1][2];
	FrustumPlane[FAR_PLANE].z = ViewProj.m[2][3] - ViewProj.m[2][2];
	FrustumPlane[FAR_PLANE].w = ViewProj.m[3][3] - ViewProj.m[3][2];
	Normalize4P(&FrustumPlane[FAR_PLANE]);
	*/
	/*
	FrustumPlane[LEFT_PLANE].x = ViewProj.m[3][0] + ViewProj.m[0][0];
	FrustumPlane[LEFT_PLANE].y = ViewProj.m[3][1] + ViewProj.m[0][1];
	FrustumPlane[LEFT_PLANE].z = ViewProj.m[3][2] + ViewProj.m[0][2];
	FrustumPlane[LEFT_PLANE].w = ViewProj.m[3][3] + ViewProj.m[0][3];
	Normalize4P(&FrustumPlane[LEFT_PLANE]);

	FrustumPlane[RIGHT_PLANE].x = ViewProj.m[3][0] - ViewProj.m[0][0];
	FrustumPlane[RIGHT_PLANE].y = ViewProj.m[3][1] - ViewProj.m[0][1];
	FrustumPlane[RIGHT_PLANE].z = ViewProj.m[3][2] - ViewProj.m[0][2];
	FrustumPlane[RIGHT_PLANE].w = ViewProj.m[3][3] - ViewProj.m[0][3];
	Normalize4P(&FrustumPlane[RIGHT_PLANE]);

	FrustumPlane[TOP_PLANE].x = ViewProj.m[3][0] - ViewProj.m[1][0];
	FrustumPlane[TOP_PLANE].y = ViewProj.m[3][1] - ViewProj.m[1][1];
	FrustumPlane[TOP_PLANE].z = ViewProj.m[3][2] - ViewProj.m[1][2];
	FrustumPlane[TOP_PLANE].w = ViewProj.m[3][3] - ViewProj.m[1][3];
	Normalize4P(&FrustumPlane[TOP_PLANE]);

	FrustumPlane[BOTTOM_PLANE].x = ViewProj.m[3][0] + ViewProj.m[1][0];
	FrustumPlane[BOTTOM_PLANE].y = ViewProj.m[3][1] + ViewProj.m[1][1];
	FrustumPlane[BOTTOM_PLANE].z = ViewProj.m[3][2] + ViewProj.m[1][2];
	FrustumPlane[BOTTOM_PLANE].w = ViewProj.m[3][3] + ViewProj.m[1][3];
	Normalize4P(&FrustumPlane[BOTTOM_PLANE]);

	FrustumPlane[NEAR_PLANE].x = ViewProj.m[2][0];
	FrustumPlane[NEAR_PLANE].y = ViewProj.m[2][1];
	FrustumPlane[NEAR_PLANE].z = ViewProj.m[2][2];
	FrustumPlane[NEAR_PLANE].w = ViewProj.m[2][3];
	Normalize4P(&FrustumPlane[NEAR_PLANE]);

	FrustumPlane[FAR_PLANE].x = ViewProj.m[3][0] - ViewProj.m[2][0];
	FrustumPlane[FAR_PLANE].y = ViewProj.m[3][1] - ViewProj.m[2][1];
	FrustumPlane[FAR_PLANE].z = ViewProj.m[3][2] - ViewProj.m[2][2];
	FrustumPlane[FAR_PLANE].w = ViewProj.m[3][3] - ViewProj.m[2][3];
	Normalize4P(&FrustumPlane[FAR_PLANE]);
	*/
}

bool AABBVsFrustum(AABBData* AABB)
{
	// Check each of the eight AABB corner points against all six frustum planes
	for (int i = 0; i < 6; i++) {
		vec3 normal = Vec3(FrustumPlane[i].x, FrustumPlane[i].y, FrustumPlane[i].z);
		vec3 corner = AABB->Min; // start with the AABB minimum corner

		// Adjust the corner point based on the plane normal vector
		if (normal.x >= 0.0f) corner.x = AABB->Max.x;
		if (normal.y >= 0.0f) corner.y = AABB->Max.y;
		if (normal.z >= 0.0f) corner.z = AABB->Max.z;

		// Test the corner point against the plane
		if (Dot3P(&normal, &corner) + FrustumPlane[i].w < 0.0f) {
			// The corner point is outside the plane, so the AABB is outside the frustum
			return false;
		}
	}

	// All eight corner points are inside all six planes, so the AABB is inside the frustum
	return true;
}

void TransformAABB(AABBData* AABB, uint32_t i)
{
	mat4 Model;
	LoadMat4IdentityP(&Model);
	Model = ScaleMat4P(&Model, &Entities[i].Scale);
	Model = RotateXMat4P(&Model, ToRadians(Entities[i].Rotate.x));
	Model = RotateYMat4P(&Model, ToRadians(Entities[i].Rotate.y));
	Model = RotateZMat4P(&Model, ToRadians(Entities[i].Rotate.z));
	Model = TranslateMat4P(&Model, &Entities[i].Translate);

	vec4 Min = MultiplyVec4Mat4(Vec4(AABB->Min.x, AABB->Min.y, AABB->Min.z, 1.0), Model);
	vec4 Max = MultiplyVec4Mat4(Vec4(AABB->Max.x, AABB->Max.y, AABB->Max.z, 1.0), Model);

	AABB->Min.x = Min.x;
	AABB->Min.y = Min.y;
	AABB->Min.z = Min.z;

	AABB->Max.x = Max.x;
	AABB->Max.y = Max.y;
	AABB->Max.z = Max.z;

//	printf("Pos: %f %f %f\n", Entities[i].Translate.x, Entities[i].Translate.y, Entities[i].Translate.z);
//	printf("Sca: %f %f %f\n", Entities[i].Scale.x, Entities[i].Scale.y, Entities[i].Scale.z);
//	printf("Rot: %f %f %f\n", Entities[i].Rotate.x, Entities[i].Rotate.y, Entities[i].Rotate.z);
//	printf("\tMin: %f %f %f\n", Min.x, Min.y, Min.z);
//	printf("\tMax: %f %f %f\n", Max.x, Max.y, Max.z);
}



void RunFrustumCulling(mat4 ViewProj, uint32_t RenderType)
{
	GetFrustum(ViewProj);

	Mutex.lock();
	for (uint32_t i = 0; i < EntityCount; i++)
	{
		if (Entities[i].UsedComponents[COMPONENT_TYPE_MESH])
		{
			SceneMesh* Mesh = (SceneMesh*)CMA_GetAt(&SceneMeshes, Entities[i].Mesh.MeshIndex);
			if (Mesh != NULL)
			{
				for (uint32_t m = 0; m < Mesh->MeshCount; m++)
				{
					AABBData AABB = Mesh->MeshData[m].AABB;
					TransformAABB(&AABB, i);
					Mesh->MeshData[m].Render[RenderType] = AABBVsFrustum(&AABB);
				//	printf("On screen: %d, Mesh: %d, Entity: %d\n", (int)Mesh->MeshData[m].Render[RenderType], m, i);
				}
			}
		}
	}
	Mutex.unlock();
}