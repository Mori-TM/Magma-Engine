#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

#ifndef WAVE_BASIC_MATH
#define WAVE_BASIC_MATH

#define WAVEMAX(x, y) (((x) > (y)) ? (x) : (y))
#define WAVEMIN(x, y) (((x) < (y)) ? (x) : (y))

typedef struct
{
	float x;
	float y;
	float z;
} WaveVec3;

float WaveFastInverseSqrt(float number)
{
	long i;
	float x2, y;
	const float threehalfs = 1.5f;

	x2 = number * 0.5f;
	y = number;
	i = *(long*)&y;              // evil floating point bit level hacking
	i = 0x5f3759df - (i >> 1);   // what the fuck?
	y = *(float*)&i;
	y = y * (threehalfs - (x2 * y * y));   // 1st iteration
//	y = y * (threehalfs - (x2 * y * y));   // 2nd iteration, this can be removed - it's for more accuracy

	return y;
}

extern inline WaveVec3 WaveNormalize(WaveVec3 v)
{
	float length = WaveFastInverseSqrt(v.x * v.x + v.y * v.y + v.z * v.z);
	v.x *= length;
	v.y *= length;
	v.z *= length;

	return v;
}

extern inline WaveVec3 WaveCross(WaveVec3 a, WaveVec3 b)
{
	WaveVec3 r;
	r.x = a.y * b.z - a.z * b.y;
	r.y = a.z * b.x - a.x * b.z;
	r.z = a.x * b.y - a.y * b.x;

	return r;
}

extern inline float WaveDot(WaveVec3 a, WaveVec3 b)
{
	return a.x * b.x + a.y * b.y + a.z * b.z;
}

extern inline WaveVec3 WaveAdd(WaveVec3 a, WaveVec3 b)
{
	WaveVec3 r;
	r.x = a.x + b.x;
	r.y = a.y + b.y;
	r.z = a.z + b.z;

	return r;
}

extern inline WaveVec3 WaveSub(WaveVec3 a, WaveVec3 b)
{
	WaveVec3 r;
	r.x = a.x - b.x;
	r.y = a.y - b.y;
	r.z = a.z - b.z;

	return r;
}

extern inline WaveVec3 WaveMul(WaveVec3 a, WaveVec3 b)
{
	WaveVec3 r;
	r.x = a.x * b.x;
	r.y = a.y * b.y;
	r.z = a.z * b.z;

	return r;
}

extern inline WaveVec3 WaveDiv(WaveVec3 a, WaveVec3 b)
{
	WaveVec3 r;
	r.x = a.x / b.x;
	r.y = a.y / b.y;
	r.z = a.z / b.z;

	return r;
}

extern inline float WaveLength(WaveVec3 v)
{
	return sqrtf(v.x * v.x + v.y * v.y + v.z * v.z);
}

extern inline WaveVec3 WaveVec3f(float v)
{
	WaveVec3 r;
	r.x = v;
	r.y = v;
	r.z = v;

	return r;
}
#endif

typedef enum
{
	WAVE_RIGID_BODY_TYPE_PLANE = 0,
	WAVE_RIGID_BODY_TYPE_SPHERE,
} WaveRigidBodyType;

typedef struct
{
	float Mass;
	WaveVec3 Velocity;
	WaveVec3 Force;

	WaveVec3 Position;
	WaveVec3 Scale;
	WaveVec3 Rotation;
	uint32_t Type;
} WaveRigidBody;

typedef struct
{
	WaveVec3 A; //Furthest point of A into B
	WaveVec3 B; //Furthest point of B into A
	WaveVec3 Normal; // B – A normalized
	float Depth; // Length of B – A
	bool HasCollision;
} WaveCollisionPoints;

bool WaveSphereSphereCollision(WaveRigidBody* SphereA, WaveRigidBody* SphereB)
{
	WaveVec3 Direction = WaveSub(SphereA->Position, SphereB->Position);
	float Distance = WaveLength(Direction);
	float RadiusSum = SphereA->Scale.x + SphereB->Scale.x;
	
	if (Distance < RadiusSum)
		return true;

	return false;
}

WaveCollisionPoints WaveSpherePlaneCollision(WaveRigidBody* Sphere, WaveRigidBody* Plane)
{
	return {};
}

uint32_t WaveRigidBodyCount = 0;
WaveRigidBody** WaveRigidBodys = NULL;

void WaveAddRigidBody(WaveRigidBody* RigidBody)
{
	WaveRigidBodys = (WaveRigidBody**)realloc(WaveRigidBodys, (WaveRigidBodyCount + 1) * sizeof(WaveRigidBody*));
	WaveRigidBodys[WaveRigidBodyCount] = RigidBody;
	WaveRigidBodyCount++;
}

void WaveRemoveRigidBody(WaveRigidBody* RigidBody)
{
	for (uint32_t i = 0; i < WaveRigidBodyCount; i++)
	{
		if (WaveRigidBodys[i] == RigidBody)
		{
			WaveRigidBodyCount--;
			for (uint32_t j = i; j < WaveRigidBodyCount; j++)
			{
				WaveRigidBodys[j] = WaveRigidBodys[j + 1];
			}
			break;
		}
	}
	
	WaveRigidBodys = (WaveRigidBody**)realloc(WaveRigidBodys, WaveRigidBodyCount * sizeof(WaveRigidBody*));
}

void WaveStepRidgidBodys(float DeltaTime)
{
	const WaveVec3 Gravity = { 0.0, -9.8, 0.0 };
	const WaveVec3 DeltaTimeV = { DeltaTime, DeltaTime, DeltaTime };
	
	for (uint32_t i = 0; i < WaveRigidBodyCount; i++)
	{
		const WaveVec3 Mass = { WaveRigidBodys[i]->Mass, WaveRigidBodys[i]->Mass, WaveRigidBodys[i]->Mass };
		WaveRigidBodys[i]->Force = WaveAdd(WaveRigidBodys[i]->Force, WaveMul(Mass, Gravity));

		WaveRigidBodys[i]->Velocity = WaveAdd(WaveRigidBodys[i]->Velocity, WaveMul(WaveDiv(WaveRigidBodys[i]->Force, Mass), DeltaTimeV));
		WaveRigidBodys[i]->Position = WaveAdd(WaveRigidBodys[i]->Position, WaveMul(WaveRigidBodys[i]->Velocity, DeltaTimeV));

		WaveRigidBodys[i]->Force.x = 0.0;
		WaveRigidBodys[i]->Force.y = 0.0;
		WaveRigidBodys[i]->Force.z = 0.0;
	}
	
	for (uint32_t i = 0; i < WaveRigidBodyCount; i++)
	{
		for (uint32_t j = 0; j < WaveRigidBodyCount; j++)
		{
			if (WaveRigidBodys[i]->Type == WAVE_RIGID_BODY_TYPE_SPHERE && WaveRigidBodys[j]->Type == WAVE_RIGID_BODY_TYPE_SPHERE)
			{
				if (WaveSphereSphereCollision(WaveRigidBodys[i], WaveRigidBodys[j]))
				{
					//respond to collision
					
					
				
					printf("Collision\n");
				}
				
			}
		}
	}
}