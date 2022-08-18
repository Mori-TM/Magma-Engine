#include <math.h>

//vector
typedef struct
{
	float x;
	float y;
} vec2;

typedef struct
{
	float x;
	float y;
	float z;
} vec3;

typedef struct
{
	float x;
	float y;
	float z;
	float w;
} vec4;

float FastInverseSqrt(float number)
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
	y = y * (threehalfs - (x2 * y * y));   // 2nd iteration, this can be removed - it's for more accuracy

	return y;
}

vec2 Normalize2(vec2 v)
{
	register float length = FastInverseSqrt(v.x * v.x + v.y * v.y);
	v.x *= length;
	v.y *= length;
	return v;
}

vec3 Normalize3(vec3 v)
{
	register float length = FastInverseSqrt(v.x * v.x + v.y * v.y + v.z * v.z);
	v.x *= length;
	v.y *= length;
	v.z *= length;
	return v;
}

vec4 Normalize4(vec4 v)
{
	register float length = FastInverseSqrt(v.x * v.x + v.y * v.y + v.z * v.z + v.w * v.w);
	v.x *= length;
	v.y *= length;
	v.z *= length;
	v.w *= length;
	return v;
}

void Normalize2P(vec2* v)
{
	register float length = FastInverseSqrt(v->x * v->x + v->y * v->y);
	v->x *= length;
	v->y *= length;
}

void Normalize3P(vec3* v)
{
	register float length = FastInverseSqrt(v->x * v->x + v->y * v->y + v->z * v->z);
	v->x *= length;
	v->y *= length;
	v->z *= length;
}

void Normalize4P(vec4* v)
{
	register float length = FastInverseSqrt(v->x * v->x + v->y * v->y + v->z * v->z + v->w * v->w);
	v->x *= length;
	v->y *= length;
	v->z *= length;
	v->w *= length;
}

extern inline vec3 Cross3P(vec3* a, vec3* b)
{
	vec3 r;
	r.x = a->y * b->z - a->z * b->y;
	r.y = a->z * b->x - a->x * b->z;
	r.z = a->x * b->y - a->y * b->x;
	return r;
}

extern inline float Dot2P(vec2* a, vec2* b)
{
	return a->x * b->x + a->y * b->y;
}

extern inline float Dot3P(vec3* a, vec3* b)
{
	return a->x * b->x + a->y * b->y + a->z * b->z;
}

extern inline vec2 Add2P(vec2* a, vec2* b)
{
	vec2 r;
	r.x = a->x + b->x;
	r.y = a->y + b->y;
	return r;
}

extern inline vec2 Sub2P(vec2* a, vec2* b)
{
	vec2 r;
	r.x = a->x - b->x;
	r.y = a->y - b->y;
	return r;
}

extern inline vec2 Mul2P(vec2* a, vec2* b)
{
	vec2 r;
	r.x = a->x * b->x;
	r.y = a->y * b->y;
	return r;
}

extern inline vec2 Div2P(vec2* a, vec2* b)
{
	vec2 r;
	r.x = a->x / b->x;
	r.y = a->y / b->y;
	return r;
}

extern inline vec3 Add3P(vec3* a, vec3* b)
{
	vec3 r;
	r.x = a->x + b->x;
	r.y = a->y + b->y;
	r.z = a->z + b->z;
	return r;
}

extern inline vec3 Sub3P(vec3* a, vec3* b)
{
	vec3 r;
	r.x = a->x - b->x;
	r.y = a->y - b->y;
	r.z = a->z - b->z;
	return r;
}

extern inline vec3 Mul3P(vec3* a, vec3* b)
{
	vec3 r;
	r.x = a->x * b->x;
	r.y = a->y * b->y;
	r.z = a->z * b->z;
	return r;
}

extern inline vec3 Div3P(vec3* a, vec3* b)
{
	vec3 r;
	r.x = a->x / b->x;
	r.y = a->y / b->y;
	r.z = a->z / b->z;
	return r;
}

extern inline vec4 Add4P(vec4* a, vec4* b)
{
	vec4 r;
	r.x = a->x + b->x;
	r.y = a->y + b->y;
	r.z = a->z + b->z;
	r.w = a->w + b->w;
	return r;
}

extern inline vec4 Sub4P(vec4* a, vec4* b)
{
	vec4 r;
	r.x = a->x - b->x;
	r.y = a->y - b->y;
	r.z = a->z - b->z;
	r.w = a->w - b->w;
	return r;
}

extern inline vec4 Mul4P(vec4* a, vec4* b)
{
	vec4 r;
	r.x = a->x * b->x;
	r.y = a->y * b->y;
	r.z = a->z * b->z;
	r.w = a->w * b->w;
	return r;
}

extern inline vec4 Div4P(vec4* a, vec4* b)
{
	vec4 r;
	r.x = a->x / b->x;
	r.y = a->y / b->y;
	r.z = a->z / b->z;
	r.w = a->w / b->w;
	return r;
}


extern inline vec3 Cross3(vec3 a, vec3 b)
{
	vec3 r;
	r.x = a.y * b.z - a.z * b.y;
	r.y = a.z * b.x - a.x * b.z;
	r.z = a.x * b.y - a.y * b.x;
	return r;
}

extern inline float Dot2(vec2 a, vec2 b)
{
	return a.x * b.x + a.y * b.y;
}

extern inline float Dot3(vec3 a, vec3 b)
{
	return a.x * b.x + a.y * b.y + a.z * b.z;
}

extern inline vec2 Add2(vec2 a, vec2 b)
{
	vec2 r;
	r.x = a.x + b.x;
	r.y = a.y + b.y;
	return r;
}

extern inline vec2 Sub2(vec2 a, vec2 b)
{
	vec2 r;
	r.x = a.x - b.x;
	r.y = a.y - b.y;
	return r;
}

extern inline vec2 Mul2(vec2 a, vec2 b)
{
	vec2 r;
	r.x = a.x * b.x;
	r.y = a.y * b.y;
	return r;
}

extern inline vec2 Div2(vec2 a, vec2 b)
{
	vec2 r;
	r.x = a.x / b.x;
	r.y = a.y / b.y;
	return r;
}

extern inline vec3 Add3(vec3 a, vec3 b)
{
	vec3 r;
	r.x = a.x + b.x;
	r.y = a.y + b.y;
	r.z = a.z + b.z;
	return r;
}

extern inline vec3 Sub3(vec3 a, vec3 b)
{
	vec3 r;
	r.x = a.x - b.x;
	r.y = a.y - b.y;
	r.z = a.z - b.z;
	return r;
}

extern inline vec3 Mul3(vec3 a, vec3 b)
{
	vec3 r;
	r.x = a.x * b.x;
	r.y = a.y * b.y;
	r.z = a.z * b.z;
	return r;
}

extern inline vec3 Div3(vec3 a, vec3 b)
{
	vec3 r;
	r.x = a.x / b.x;
	r.y = a.y / b.y;
	r.z = a.z / b.z;
	return r;
}

extern inline vec4 Add4(vec4 a, vec4 b)
{
	vec4 r;
	r.x = a.x + b.x;
	r.y = a.y + b.y;
	r.z = a.z + b.z;
	r.w = a.w + b.w;
	return r;
}

extern inline vec4 Sub4(vec4 a, vec4 b)
{
	vec4 r;
	r.x = a.x - b.x;
	r.y = a.y - b.y;
	r.z = a.z - b.z;
	r.w = a.w - b.w;
	return r;
}

extern inline vec4 Mul4(vec4 a, vec4 b)
{
	vec4 r;
	r.x = a.x * b.x;
	r.y = a.y * b.y;
	r.z = a.z * b.z;
	r.w = a.w * b.w;
	return r;
}

extern inline vec4 Div4(vec4 a, vec4 b)
{
	vec4 r;
	r.x = a.x / b.x;
	r.y = a.y / b.y;
	r.z = a.z / b.z;
	r.w = a.w / b.w;
	return r;
}

extern inline float Length3(vec3 v)
{
	return sqrtf(v.x * v.x + v.y * v.y + v.z * v.z);
}

extern inline float Length3P(vec3* v)
{
	return sqrtf((v->x * v->x) + (v->y * v->y) + (v->z * v->z));
}

extern inline vec2 Reflect2(vec2* v, vec2* n)
{
	vec2 r;
	r.x = v->x - 2 * Dot2P(v, n) * n->x;
	r.y = v->y - 2 * Dot2P(v, n) * n->y;
	return r;
}

extern inline vec3 Reflect3(vec3* v, vec3* n)
{
	vec3 r;
	r.x = v->x - 2 * Dot3P(v, n) * n->x;
	r.y = v->y - 2 * Dot3P(v, n) * n->y;
	r.z = v->z - 2 * Dot3P(v, n) * n->z;
	return r;
}

float GetDistanceVec2(vec2* a, vec2* b)
{
	register float DX = a->x - b->x;
	register float DY = a->y - b->y;

	DX *= DX;
	DY *= DY;

	return sqrtf(DX + DY);
}

float GetDistanceVec3(vec3 a, vec3 b)
{
	register float DX = a.x - b.x;
	register float DY = a.y - b.y;
	register float DZ = a.z - b.z;

	DX *= DX;
	DY *= DY;
	DZ *= DZ;

	return sqrtf(DX + DY + DZ);
}

float GetDistanceVec3P(vec3* a, vec3* b)
{
	register float DX = a->x - b->x;
	register float DY = a->y - b->y;
	register float DZ = a->z - b->z;

	DX *= DX;
	DY *= DY;
	DZ *= DZ;

	return sqrtf(DX + DY + DZ);
}

extern inline vec2 Vec2(float x, float y)
{
	vec2 r = { x, y };
	return r;
}

extern inline vec3 Vec3(float x, float y, float z)
{
	vec3 r = { x, y, z };
	return r;
}

extern inline vec4 Vec4(float x, float y, float z, float w)
{
	vec4 r = { x, y, z, w };
	return r;
}

extern inline vec2 Vec2f(float x)
{
	vec2 r = { x, x };
	return r;
}

extern inline vec3 Vec3f(float x)
{
	vec3 r = { x, x, x };
	return r;
}

extern inline vec4 Vec4f(float x)
{
	vec4 r = { x, x, x, x };
	return r;
}

//matrix
typedef struct
{
	float m[4][4];
} mat4;

void LoadMat4IdentityP(mat4* Matrix)
{
	Matrix->m[0][0] = 1.0;
	Matrix->m[1][0] = 0.0;
	Matrix->m[2][0] = 0.0;
	Matrix->m[3][0] = 0.0;

	Matrix->m[0][1] = 0.0;
	Matrix->m[1][1] = 1.0;
	Matrix->m[2][1] = 0.0;
	Matrix->m[3][1] = 0.0;

	Matrix->m[0][2] = 0.0;
	Matrix->m[1][2] = 0.0;
	Matrix->m[2][2] = 1.0;
	Matrix->m[3][2] = 0.0;

	Matrix->m[0][3] = 0.0;
	Matrix->m[1][3] = 0.0;
	Matrix->m[2][3] = 0.0;
	Matrix->m[3][3] = 1.0;
}

mat4 LoadMat4Identity()
{
	mat4 Matrix;
	Matrix.m[0][0] = 1.0;
	Matrix.m[1][0] = 0.0;
	Matrix.m[2][0] = 0.0;
	Matrix.m[3][0] = 0.0;
		  
	Matrix.m[0][1] = 0.0;
	Matrix.m[1][1] = 1.0;
	Matrix.m[2][1] = 0.0;
	Matrix.m[3][1] = 0.0;
		  
	Matrix.m[0][2] = 0.0;
	Matrix.m[1][2] = 0.0;
	Matrix.m[2][2] = 1.0;
	Matrix.m[3][2] = 0.0;
		  
	Matrix.m[0][3] = 0.0;
	Matrix.m[1][3] = 0.0;
	Matrix.m[2][3] = 0.0;
	Matrix.m[3][3] = 1.0;
	return Matrix;
}

mat4 MultiplyMat4P(mat4* a, mat4* b)
{
	mat4 r;
	r.m[0][0] = a->m[0][0] * b->m[0][0] + a->m[1][0] * b->m[0][1] + a->m[2][0] * b->m[0][2] + a->m[3][0] * b->m[0][3];
	r.m[0][1] = a->m[0][1] * b->m[0][0] + a->m[1][1] * b->m[0][1] + a->m[2][1] * b->m[0][2] + a->m[3][1] * b->m[0][3];
	r.m[0][2] = a->m[0][2] * b->m[0][0] + a->m[1][2] * b->m[0][1] + a->m[2][2] * b->m[0][2] + a->m[3][2] * b->m[0][3];
	r.m[0][3] = a->m[0][3] * b->m[0][0] + a->m[1][3] * b->m[0][1] + a->m[2][3] * b->m[0][2] + a->m[3][3] * b->m[0][3];
	 
	r.m[1][0] = a->m[0][0] * b->m[1][0] + a->m[1][0] * b->m[1][1] + a->m[2][0] * b->m[1][2] + a->m[3][0] * b->m[1][3];
	r.m[1][1] = a->m[0][1] * b->m[1][0] + a->m[1][1] * b->m[1][1] + a->m[2][1] * b->m[1][2] + a->m[3][1] * b->m[1][3];
	r.m[1][2] = a->m[0][2] * b->m[1][0] + a->m[1][2] * b->m[1][1] + a->m[2][2] * b->m[1][2] + a->m[3][2] * b->m[1][3];
	r.m[1][3] = a->m[0][3] * b->m[1][0] + a->m[1][3] * b->m[1][1] + a->m[2][3] * b->m[1][2] + a->m[3][3] * b->m[1][3];
	 
	r.m[2][0] = a->m[0][0] * b->m[2][0] + a->m[1][0] * b->m[2][1] + a->m[2][0] * b->m[2][2] + a->m[3][0] * b->m[2][3];
	r.m[2][1] = a->m[0][1] * b->m[2][0] + a->m[1][1] * b->m[2][1] + a->m[2][1] * b->m[2][2] + a->m[3][1] * b->m[2][3];
	r.m[2][2] = a->m[0][2] * b->m[2][0] + a->m[1][2] * b->m[2][1] + a->m[2][2] * b->m[2][2] + a->m[3][2] * b->m[2][3];
	r.m[2][3] = a->m[0][3] * b->m[2][0] + a->m[1][3] * b->m[2][1] + a->m[2][3] * b->m[2][2] + a->m[3][3] * b->m[2][3];
	 
	r.m[3][0] = a->m[0][0] * b->m[3][0] + a->m[1][0] * b->m[3][1] + a->m[2][0] * b->m[3][2] + a->m[3][0] * b->m[3][3];
	r.m[3][1] = a->m[0][1] * b->m[3][0] + a->m[1][1] * b->m[3][1] + a->m[2][1] * b->m[3][2] + a->m[3][1] * b->m[3][3];
	r.m[3][2] = a->m[0][2] * b->m[3][0] + a->m[1][2] * b->m[3][1] + a->m[2][2] * b->m[3][2] + a->m[3][2] * b->m[3][3];
	r.m[3][3] = a->m[0][3] * b->m[3][0] + a->m[1][3] * b->m[3][1] + a->m[2][3] * b->m[3][2] + a->m[3][3] * b->m[3][3];

	return r;
}

mat4 MultiplyMat4(mat4 a, mat4 b)
{
	mat4 r;

	r.m[0][0] = a.m[0][0] * b.m[0][0] + a.m[1][0] * b.m[0][1] + a.m[2][0] * b.m[0][2] + a.m[3][0] * b.m[0][3];
	r.m[0][1] = a.m[0][1] * b.m[0][0] + a.m[1][1] * b.m[0][1] + a.m[2][1] * b.m[0][2] + a.m[3][1] * b.m[0][3];
	r.m[0][2] = a.m[0][2] * b.m[0][0] + a.m[1][2] * b.m[0][1] + a.m[2][2] * b.m[0][2] + a.m[3][2] * b.m[0][3];
	r.m[0][3] = a.m[0][3] * b.m[0][0] + a.m[1][3] * b.m[0][1] + a.m[2][3] * b.m[0][2] + a.m[3][3] * b.m[0][3];
	 
	r.m[1][0] = a.m[0][0] * b.m[1][0] + a.m[1][0] * b.m[1][1] + a.m[2][0] * b.m[1][2] + a.m[3][0] * b.m[1][3];
	r.m[1][1] = a.m[0][1] * b.m[1][0] + a.m[1][1] * b.m[1][1] + a.m[2][1] * b.m[1][2] + a.m[3][1] * b.m[1][3];
	r.m[1][2] = a.m[0][2] * b.m[1][0] + a.m[1][2] * b.m[1][1] + a.m[2][2] * b.m[1][2] + a.m[3][2] * b.m[1][3];
	r.m[1][3] = a.m[0][3] * b.m[1][0] + a.m[1][3] * b.m[1][1] + a.m[2][3] * b.m[1][2] + a.m[3][3] * b.m[1][3];
	 
	r.m[2][0] = a.m[0][0] * b.m[2][0] + a.m[1][0] * b.m[2][1] + a.m[2][0] * b.m[2][2] + a.m[3][0] * b.m[2][3];
	r.m[2][1] = a.m[0][1] * b.m[2][0] + a.m[1][1] * b.m[2][1] + a.m[2][1] * b.m[2][2] + a.m[3][1] * b.m[2][3];
	r.m[2][2] = a.m[0][2] * b.m[2][0] + a.m[1][2] * b.m[2][1] + a.m[2][2] * b.m[2][2] + a.m[3][2] * b.m[2][3];
	r.m[2][3] = a.m[0][3] * b.m[2][0] + a.m[1][3] * b.m[2][1] + a.m[2][3] * b.m[2][2] + a.m[3][3] * b.m[2][3];
	 
	r.m[3][0] = a.m[0][0] * b.m[3][0] + a.m[1][0] * b.m[3][1] + a.m[2][0] * b.m[3][2] + a.m[3][0] * b.m[3][3];
	r.m[3][1] = a.m[0][1] * b.m[3][0] + a.m[1][1] * b.m[3][1] + a.m[2][1] * b.m[3][2] + a.m[3][1] * b.m[3][3];
	r.m[3][2] = a.m[0][2] * b.m[3][0] + a.m[1][2] * b.m[3][1] + a.m[2][2] * b.m[3][2] + a.m[3][2] * b.m[3][3];
	r.m[3][3] = a.m[0][3] * b.m[3][0] + a.m[1][3] * b.m[3][1] + a.m[2][3] * b.m[3][2] + a.m[3][3] * b.m[3][3];
	
	return r;
}

vec4 MultiplyVec4Mat4P(vec4* v, mat4* m)
{
	vec4 r;
	r.x = v->x * m->m[0][0] + v->y * m->m[1][0] + v->z * m->m[2][0] + v->w * m->m[3][0];
	r.y = v->x * m->m[0][1] + v->y * m->m[1][1] + v->z * m->m[2][1] + v->w * m->m[3][1];
	r.z = v->x * m->m[0][2] + v->y * m->m[1][2] + v->z * m->m[2][2] + v->w * m->m[3][2];
	r.w = v->x * m->m[0][3] + v->y * m->m[1][3] + v->z * m->m[2][3] + v->w * m->m[3][3];
	return r;
}

vec4 MultiplyVec4Mat4(mat4 m, vec4 v)
{
	vec4 r;
	r.x = v.x * m.m[0][0] + v.y * m.m[1][0] + v.z * m.m[2][0] + v.w * m.m[3][0];
	r.y = v.x * m.m[0][1] + v.y * m.m[1][1] + v.z * m.m[2][1] + v.w * m.m[3][1];
	r.z = v.x * m.m[0][2] + v.y * m.m[1][2] + v.z * m.m[2][2] + v.w * m.m[3][2];
	r.w = v.x * m.m[0][3] + v.y * m.m[1][3] + v.z * m.m[2][3] + v.w * m.m[3][3];
	return r;
}

mat4 TranslateMat4P(mat4* m, vec3* v)
{
	mat4 Matrix;

	Matrix.m[0][0] = 1.0;
	Matrix.m[1][0] = 0.0;
	Matrix.m[2][0] = 0.0;
	Matrix.m[3][0] = v->x;

	Matrix.m[0][1] = 0.0;
	Matrix.m[1][1] = 1.0;
	Matrix.m[2][1] = 0.0;
	Matrix.m[3][1] = v->y;

	Matrix.m[0][2] = 0.0;
	Matrix.m[1][2] = 0.0;
	Matrix.m[2][2] = 1.0;
	Matrix.m[3][2] = v->z;

	Matrix.m[0][3] = 0.0;
	Matrix.m[1][3] = 0.0;
	Matrix.m[2][3] = 0.0;
	Matrix.m[3][3] = 1.0;

	return MultiplyMat4P(&Matrix, m);
}

mat4 TranslateMat4(mat4 m, vec3 v)
{
	mat4 Matrix;

	Matrix.m[0][0] = 1.0;
	Matrix.m[1][0] = 0.0;
	Matrix.m[2][0] = 0.0;
	Matrix.m[3][0] = v.x;

	Matrix.m[0][1] = 0.0;
	Matrix.m[1][1] = 1.0;
	Matrix.m[2][1] = 0.0;
	Matrix.m[3][1] = v.y;

	Matrix.m[0][2] = 0.0;
	Matrix.m[1][2] = 0.0;
	Matrix.m[2][2] = 1.0;
	Matrix.m[3][2] = v.z;

	Matrix.m[0][3] = 0.0;
	Matrix.m[1][3] = 0.0;
	Matrix.m[2][3] = 0.0;
	Matrix.m[3][3] = 1.0;

	return MultiplyMat4P(&Matrix, &m);
}

mat4 ScaleMat4P(mat4* m, vec3* v)
{
	mat4 Matrix;

	Matrix.m[0][0] = v->x;
	Matrix.m[1][0] = 0.0;
	Matrix.m[2][0] = 0.0;
	Matrix.m[3][0] = 0.0;

	Matrix.m[0][1] = 0.0;
	Matrix.m[1][1] = v->y;
	Matrix.m[2][1] = 0.0;
	Matrix.m[3][1] = 0.0;

	Matrix.m[0][2] = 0.0;
	Matrix.m[1][2] = 0.0;
	Matrix.m[2][2] = v->z;
	Matrix.m[3][2] = 0.0;

	Matrix.m[0][3] = 0.0;
	Matrix.m[1][3] = 0.0;
	Matrix.m[2][3] = 0.0;
	Matrix.m[3][3] = 1.0;

	return MultiplyMat4P(&Matrix, m);
}

mat4 ScaleMat4(mat4 m, vec3 v)
{
	mat4 Matrix;

	Matrix.m[0][0] = v.x;
	Matrix.m[1][0] = 0.0;
	Matrix.m[2][0] = 0.0;
	Matrix.m[3][0] = 0.0;

	Matrix.m[0][1] = 0.0;
	Matrix.m[1][1] = v.y;
	Matrix.m[2][1] = 0.0;
	Matrix.m[3][1] = 0.0;

	Matrix.m[0][2] = 0.0;
	Matrix.m[1][2] = 0.0;
	Matrix.m[2][2] = v.z;
	Matrix.m[3][2] = 0.0;

	Matrix.m[0][3] = 0.0;
	Matrix.m[1][3] = 0.0;
	Matrix.m[2][3] = 0.0;
	Matrix.m[3][3] = 1.0;

	return MultiplyMat4P(&Matrix, &m);
}
/*
void RotateMat4(mat4* m, float Angle, float x, float y, float z)
{
	mat4 Matrix;
	mat4 Tmp = *m;

	float Sin = sinf(Angle);
	float Cos = cosf(Angle);

	Matrix.m[0][0] = Cos + x * x * (1 - Cos);
	Matrix.m[0][1] = x * y * (1 - Cos) - z * Sin;
	Matrix.m[0][2] = x * z * (1 - Cos) + y * Sin;
	Matrix.m[0][3] = 0;

	Matrix.m[1][0] = y * x * (1 - Cos) + z * Sin;
	Matrix.m[1][1] = Cos + y * y * (1 - Cos);
	Matrix.m[1][2] = y * z * (1 - Cos) - x * Sin;
	Matrix.m[1][3] = 0;

	Matrix.m[2][0] = z * x * (1 - Cos) - y * Sin;
	Matrix.m[2][1] = z * y * (1 - Cos) + x * Sin;
	Matrix.m[2][2] = Cos + z * z * (1 - Cos);
	Matrix.m[2][3] = 0;

	Matrix.m[3][0] = 0;
	Matrix.m[3][1] = 0;
	Matrix.m[3][2] = 0;
	Matrix.m[3][3] = 1;

	MultiplyMat4P(&Matrix, &Tmp, m);
}
*/

mat4 RotateXMat4P(mat4* m, float Angle)
{
	mat4 Matrix;

	register float Cos = cosf(Angle);
	register float Sin = sinf(Angle);

	Matrix.m[0][0] = 1.0;
	Matrix.m[1][0] = 0.0;
	Matrix.m[2][0] = 0.0;
	Matrix.m[3][0] = 0.0;

	Matrix.m[0][1] = 0.0;
	Matrix.m[1][1] = Cos;
	Matrix.m[2][1] = -Sin;
	Matrix.m[3][1] = 0.0;

	Matrix.m[0][2] = 0.0;
	Matrix.m[1][2] = Sin;
	Matrix.m[2][2] = Cos;
	Matrix.m[3][2] = 0.0;

	Matrix.m[0][3] = 0.0;
	Matrix.m[1][3] = 0.0;
	Matrix.m[2][3] = 0.0;
	Matrix.m[3][3] = 1.0;

	return MultiplyMat4P(&Matrix, m);
}

mat4 RotateYMat4P(mat4* m, float Angle)
{
	mat4 Matrix;

	register float Cos = cosf(Angle);
	register float Sin = sinf(Angle);

	Matrix.m[0][0] = Cos;
	Matrix.m[1][0] = 0.0;
	Matrix.m[2][0] = Sin;
	Matrix.m[3][0] = 0.0;

	Matrix.m[0][1] = 0.0;
	Matrix.m[1][1] = 1.0;
	Matrix.m[2][1] = 0.0;
	Matrix.m[3][1] = 0.0;

	Matrix.m[0][2] = -Sin;
	Matrix.m[1][2] = 0.0;
	Matrix.m[2][2] = Cos;
	Matrix.m[3][2] = 0.0;

	Matrix.m[0][3] = 0.0;
	Matrix.m[1][3] = 0.0;
	Matrix.m[2][3] = 0.0;
	Matrix.m[3][3] = 1.0;

	return MultiplyMat4P(&Matrix, m);
}

mat4 RotateZMat4P(mat4* m, float Angle)
{
	mat4 Matrix;

	register float Cos = cosf(Angle);
	register float Sin = sinf(Angle);

	Matrix.m[0][0] = Cos;
	Matrix.m[1][0] = -Sin;
	Matrix.m[2][0] = 0.0;
	Matrix.m[3][0] = 0.0;

	Matrix.m[0][1] = Sin;
	Matrix.m[1][1] = Cos;
	Matrix.m[2][1] = 0.0;
	Matrix.m[3][1] = 0.0;

	Matrix.m[0][2] = 0.0;
	Matrix.m[1][2] = 0.0;
	Matrix.m[2][2] = 1.0;
	Matrix.m[3][2] = 0.0;

	Matrix.m[0][3] = 0.0;
	Matrix.m[1][3] = 0.0;
	Matrix.m[2][3] = 0.0;
	Matrix.m[3][3] = 1.0;

	return MultiplyMat4P(&Matrix, m);
}

mat4 RotateXMat4(mat4 m, float Angle)
{
	mat4 Matrix;

	register float Cos = cosf(Angle);
	register float Sin = sinf(Angle);

	Matrix.m[0][0] = 1.0;
	Matrix.m[1][0] = 0.0;
	Matrix.m[2][0] = 0.0;
	Matrix.m[3][0] = 0.0;

	Matrix.m[0][1] = 0.0;
	Matrix.m[1][1] = Cos;
	Matrix.m[2][1] = -Sin;
	Matrix.m[3][1] = 0.0;

	Matrix.m[0][2] = 0.0;
	Matrix.m[1][2] = Sin;
	Matrix.m[2][2] = Cos;
	Matrix.m[3][2] = 0.0;

	Matrix.m[0][3] = 0.0;
	Matrix.m[1][3] = 0.0;
	Matrix.m[2][3] = 0.0;
	Matrix.m[3][3] = 1.0;

	return MultiplyMat4P(&Matrix, &m);
}

mat4 RotateYMat4(mat4 m, float Angle)
{
	mat4 Matrix;

	register float Cos = cosf(Angle);
	register float Sin = sinf(Angle);

	Matrix.m[0][0] = Cos;
	Matrix.m[1][0] = 0.0;
	Matrix.m[2][0] = Sin;
	Matrix.m[3][0] = 0.0;

	Matrix.m[0][1] = 0.0;
	Matrix.m[1][1] = 1.0;
	Matrix.m[2][1] = 0.0;
	Matrix.m[3][1] = 0.0;

	Matrix.m[0][2] = -Sin;
	Matrix.m[1][2] = 0.0;
	Matrix.m[2][2] = Cos;
	Matrix.m[3][2] = 0.0;

	Matrix.m[0][3] = 0.0;
	Matrix.m[1][3] = 0.0;
	Matrix.m[2][3] = 0.0;
	Matrix.m[3][3] = 1.0;

	return MultiplyMat4P(&Matrix, &m);
}

mat4 RotateZMat4(mat4 m, float Angle)
{
	mat4 Matrix;

	register float Cos = cosf(Angle);
	register float Sin = sinf(Angle);

	Matrix.m[0][0] = Cos;
	Matrix.m[1][0] = -Sin;
	Matrix.m[2][0] = 0.0;
	Matrix.m[3][0] = 0.0;

	Matrix.m[0][1] = Sin;
	Matrix.m[1][1] = Cos;
	Matrix.m[2][1] = 0.0;
	Matrix.m[3][1] = 0.0;

	Matrix.m[0][2] = 0.0;
	Matrix.m[1][2] = 0.0;
	Matrix.m[2][2] = 1.0;
	Matrix.m[3][2] = 0.0;

	Matrix.m[0][3] = 0.0;
	Matrix.m[1][3] = 0.0;
	Matrix.m[2][3] = 0.0;
	Matrix.m[3][3] = 1.0;

	return MultiplyMat4P(&Matrix, &m);
}

mat4 LookAtMat4(vec3 Pos, vec3 LookAt, vec3 Up)
{
	mat4 Matrix;

	vec3 Z = Normalize3(Sub3P(&LookAt, &Pos));
	vec3 X = Normalize3(Cross3P(&Z, &Up));
	vec3 Y = Cross3P(&X, &Z);

	Z.x *= -1;
	Z.y *= -1;
	Z.z *= -1;

	Matrix.m[0][0] = X.x;
	Matrix.m[1][0] = X.y;
	Matrix.m[2][0] = X.z;
	Matrix.m[3][0] = -Dot3P(&X, &Pos);
	Matrix.m[0][1] = Y.x;
	Matrix.m[1][1] = Y.y;
	Matrix.m[2][1] = Y.z;
	Matrix.m[3][1] = -Dot3P(&Y, &Pos);
	Matrix.m[0][2] = Z.x;
	Matrix.m[1][2] = Z.y;
	Matrix.m[2][2] = Z.z;
	Matrix.m[3][2] = -Dot3P(&Z, &Pos);
	Matrix.m[0][3] = 0;
	Matrix.m[1][3] = 0;
	Matrix.m[2][3] = 0;
	Matrix.m[3][3] = 1.0;

	return Matrix;
}

mat4 LookAtMat4P(vec3* Pos, vec3* LookAt, vec3* Up)
{
	mat4 Matrix;

	vec3 Z = Normalize3(Sub3P(LookAt, Pos));
	vec3 X = Normalize3(Cross3P(&Z, Up));
	vec3 Y = Cross3P(&X, &Z);

	Z.x *= -1;
	Z.y *= -1;
	Z.z *= -1;

	Matrix.m[0][0] = X.x;
	Matrix.m[1][0] = X.y;
	Matrix.m[2][0] = X.z;
	Matrix.m[3][0] = -Dot3P(&X, Pos);
	Matrix.m[0][1] = Y.x;
	Matrix.m[1][1] = Y.y;
	Matrix.m[2][1] = Y.z;
	Matrix.m[3][1] = -Dot3P(&Y, Pos);
	Matrix.m[0][2] = Z.x;
	Matrix.m[1][2] = Z.y;
	Matrix.m[2][2] = Z.z;
	Matrix.m[3][2] = -Dot3P(&Z, Pos);
	Matrix.m[0][3] = 0;
	Matrix.m[1][3] = 0;
	Matrix.m[2][3] = 0;
	Matrix.m[3][3] = 1.0;

	return Matrix;
}

mat4 PerspectiveMat4(float Aspect, float FOV, float NearPlane, float FarPlane)
{
	register float F = 1.0 / tanf(0.5 * FOV);

	mat4 Matrix;
	Matrix.m[0][0] = F / Aspect;
	Matrix.m[0][1] = 0.0;
	Matrix.m[0][2] = 0.0;
	Matrix.m[0][3] = 0.0;

	Matrix.m[1][0] = 0.0;
	Matrix.m[1][1] = -F;
	Matrix.m[1][2] = 0.0;
	Matrix.m[1][3] = 0.0;

	Matrix.m[2][0] = 0.0;
	Matrix.m[2][1] = 0.0;
	Matrix.m[2][2] = FarPlane / (NearPlane - FarPlane);
	Matrix.m[2][3] = -1.0;

	Matrix.m[3][0] = 0.0;
	Matrix.m[3][1] = 0.0;
	Matrix.m[3][2] = (NearPlane * FarPlane) / (NearPlane - FarPlane);
	Matrix.m[3][3] = 0.0;

	return Matrix;
}

void PerspectiveMat4P(float Aspect, float FOV, float NearPlane, float FarPlane, mat4* m)
{
	register float F = 1.0 / tanf(0.5 * FOV);

	m->m[0][0] = F / Aspect;
	m->m[0][1] = 0.0;
	m->m[0][2] = 0.0;
	m->m[0][3] = 0.0;

	m->m[1][0] = 0.0;
	m->m[1][1] = -F;
	m->m[1][2] = 0.0;
	m->m[1][3] = 0.0;

	m->m[2][0] = 0.0;
	m->m[2][1] = 0.0;
	m->m[2][2] = FarPlane / (NearPlane - FarPlane);
	m->m[2][3] = -1.0;

	m->m[3][0] = 0.0;
	m->m[3][1] = 0.0;
	m->m[3][2] = (NearPlane * FarPlane) / (NearPlane - FarPlane);
	m->m[3][3] = 0.0;
}

mat4 OrthoMat4(float Left, float Right, float Bottom, float Top, float NearZ, float FarZ)
{
	mat4 m;

	m.m[0][0] = 2 / (Right - Left);
	m.m[1][0] = 0.0;
	m.m[2][0] = 0.0;
	m.m[3][0] = -(Right + Left) / (Right - Left);

	m.m[0][1] = 0.0;
	m.m[1][1] = 2 / (Top - Bottom);
	m.m[2][1] = 0.0;
	m.m[3][1] = -(Top + Bottom) / (Top - Bottom);

	m.m[0][2] = 0.0;
	m.m[1][2] = 0.0;
	m.m[2][2] = -1 / (FarZ - NearZ);
	m.m[3][2] = -NearZ / (FarZ - NearZ);

	m.m[0][3] = 0.0;
	m.m[1][3] = 0.0;
	m.m[2][3] = 0.0;
	m.m[3][3] = 1.0;

	return m;
}

void OrthoMat4P(float Left, float Right, float Bottom, float Top, float NearZ, float FarZ, mat4* m)
{
	m->m[0][0] = 2 / (Right - Left);
	m->m[1][0] = 0.0;
	m->m[2][0] = 0.0;
	m->m[3][0] = -(Right + Left) / (Right - Left);

	m->m[0][1] = 0.0;
	m->m[1][1] = 2 / (Top - Bottom);
	m->m[2][1] = 0.0;
	m->m[3][1] = -(Top + Bottom) / (Top - Bottom);

	m->m[0][2] = 0.0;
	m->m[1][2] = 0.0;
	m->m[2][2] = -1 / (FarZ - NearZ);
	m->m[3][2] = -NearZ / (FarZ - NearZ);

	m->m[0][3] = 0.0;
	m->m[1][3] = 0.0;
	m->m[2][3] = 0.0;
	m->m[3][3] = 1.0;
}

mat4 MulMat4Float(mat4 m, float f)
{
	mat4 Result;

	Result.m[0][0] = m.m[0][0] * f;
	Result.m[0][1] = m.m[0][1] * f;
	Result.m[0][2] = m.m[0][2] * f;
	Result.m[0][3] = m.m[0][3] * f;

	Result.m[1][0] = m.m[1][0] * f;
	Result.m[1][1] = m.m[1][1] * f;
	Result.m[1][2] = m.m[1][2] * f;
	Result.m[1][3] = m.m[1][3] * f;

	Result.m[2][0] = m.m[2][0] * f;
	Result.m[2][1] = m.m[2][1] * f;
	Result.m[2][2] = m.m[2][2] * f;
	Result.m[2][3] = m.m[2][3] * f;

	Result.m[3][0] = m.m[3][0] * f;
	Result.m[3][1] = m.m[3][1] * f;
	Result.m[3][2] = m.m[3][2] * f;
	Result.m[3][3] = m.m[3][3] * f;

	return Result;
}

mat4 SetMat4Vec4(vec4 a, vec4 b, vec4 c, vec4 d)
{
	mat4 Result;

	Result.m[0][0] = a.x;
	Result.m[0][1] = a.y;
	Result.m[0][2] = a.z;
	Result.m[0][3] = a.w;

	Result.m[1][0] = b.x;
	Result.m[1][1] = b.y;
	Result.m[1][2] = b.z;
	Result.m[1][3] = b.w;

	Result.m[2][0] = c.x;
	Result.m[2][1] = c.y;
	Result.m[2][2] = c.z;
	Result.m[2][3] = c.w;

	Result.m[3][0] = d.x;
	Result.m[3][1] = d.y;
	Result.m[3][2] = d.z;
	Result.m[3][3] = d.w;

	return Result;
}

mat4 InverseMat4(mat4 m)
{
	float Coef00 = m.m[2][2] * m.m[3][3] - m.m[3][2] * m.m[2][3];
	float Coef02 = m.m[1][2] * m.m[3][3] - m.m[3][2] * m.m[1][3];
	float Coef03 = m.m[1][2] * m.m[2][3] - m.m[2][2] * m.m[1][3];

	float Coef04 = m.m[2][1] * m.m[3][3] - m.m[3][1] * m.m[2][3];
	float Coef06 = m.m[1][1] * m.m[3][3] - m.m[3][1] * m.m[1][3];
	float Coef07 = m.m[1][1] * m.m[2][3] - m.m[2][1] * m.m[1][3];

	float Coef08 = m.m[2][1] * m.m[3][2] - m.m[3][1] * m.m[2][2];
	float Coef10 = m.m[1][1] * m.m[3][2] - m.m[3][1] * m.m[1][2];
	float Coef11 = m.m[1][1] * m.m[2][2] - m.m[2][1] * m.m[1][2];

	float Coef12 = m.m[2][0] * m.m[3][3] - m.m[3][0] * m.m[2][3];
	float Coef14 = m.m[1][0] * m.m[3][3] - m.m[3][0] * m.m[1][3];
	float Coef15 = m.m[1][0] * m.m[2][3] - m.m[2][0] * m.m[1][3];

	float Coef16 = m.m[2][0] * m.m[3][2] - m.m[3][0] * m.m[2][2];
	float Coef18 = m.m[1][0] * m.m[3][2] - m.m[3][0] * m.m[1][2];
	float Coef19 = m.m[1][0] * m.m[2][2] - m.m[2][0] * m.m[1][2];

	float Coef20 = m.m[2][0] * m.m[3][1] - m.m[3][0] * m.m[2][1];
	float Coef22 = m.m[1][0] * m.m[3][1] - m.m[3][0] * m.m[1][1];
	float Coef23 = m.m[1][0] * m.m[2][1] - m.m[2][0] * m.m[1][1];

	vec4 Fac0 = Vec4(Coef00, Coef00, Coef02, Coef03);
	vec4 Fac1 = Vec4(Coef04, Coef04, Coef06, Coef07);
	vec4 Fac2 = Vec4(Coef08, Coef08, Coef10, Coef11);
	vec4 Fac3 = Vec4(Coef12, Coef12, Coef14, Coef15);
	vec4 Fac4 = Vec4(Coef16, Coef16, Coef18, Coef19);
	vec4 Fac5 = Vec4(Coef20, Coef20, Coef22, Coef23);

	vec4 Vec0 = Vec4(m.m[1][0], m.m[0][0], m.m[0][0], m.m[0][0]);
	vec4 Vec1 = Vec4(m.m[1][1], m.m[0][1], m.m[0][1], m.m[0][1]);
	vec4 Vec2 = Vec4(m.m[1][2], m.m[0][2], m.m[0][2], m.m[0][2]);
	vec4 Vec3 = Vec4(m.m[1][3], m.m[0][3], m.m[0][3], m.m[0][3]);

	vec4 Inv0 = Add4(Sub4(Mul4(Vec1, Fac0), Mul4(Vec2, Fac1)), Mul4(Vec3, Fac2));
	vec4 Inv1 = Add4(Sub4(Mul4(Vec0, Fac0), Mul4(Vec2, Fac3)), Mul4(Vec3, Fac4));
	vec4 Inv2 = Add4(Sub4(Mul4(Vec0, Fac1), Mul4(Vec1, Fac3)), Mul4(Vec3, Fac5));
	vec4 Inv3 = Add4(Sub4(Mul4(Vec0, Fac2), Mul4(Vec1, Fac4)), Mul4(Vec2, Fac5));

	vec4 SignA = Vec4(+1, -1, +1, -1);
	vec4 SignB = Vec4(-1, +1, -1, +1);
	mat4 Inverse = SetMat4Vec4(Mul4(Inv0, SignA), Mul4(Inv1, SignB), Mul4(Inv2, SignA), Mul4(Inv3, SignB));

	vec4 Row0 = Vec4(Inverse.m[0][0], Inverse.m[1][0], Inverse.m[2][0], Inverse.m[3][0]);

	//???
	vec4 Dot0 = Mul4(Vec4(m.m[0][0], m.m[0][1], m.m[0][2], m.m[0][3]), Row0);

	float Dot1 = (Dot0.x + Dot0.y) + (Dot0.z + Dot0.w);

	float OneOverDeterminant = 1 / Dot1;

	//???
	return MulMat4Float(Inverse, OneOverDeterminant);
}

//helper
extern inline void PrintVec2(vec2* a)
{
	printf("%f %f\n", a->x, a->y);
}

extern inline void PrintVec3(vec3* a)
{
	printf("%f %f %f\n", a->x, a->y, a->z);
}

extern inline void PrintVec4(vec4* a)
{
	printf("%f %f %f %f\n", a->x, a->y, a->z, a->w);
}

extern inline int RandomInt(int Min, int Max)
{
	return (rand() % (Max - Min + 1)) + Min;
}

extern inline float ToRadians(float Angle)
{
	return 0.01745329251 * Angle;
}