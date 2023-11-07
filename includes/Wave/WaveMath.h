#include <stdio.h>
#include <stdlib.h>
#include <math.h>

//For cross platform cpu define WAVE_CROSS_PLATFORM

#if defined(__clang__)
#define WAVE_CLANG
#define WAVE_INLINE inline
#elif defined(__GNUC__)
#define WAVE_GCC
#define WAVE_INLINE __attribute__((always_inline))
#elif defined(_MSC_VER)
#define WAVE_MSVC
#define WAVE_INLINE __forceinline
#else
#define WAVE_INLINE inline
#endif

#if defined(__x86_64__) || defined(_M_X64) || defined(__i386__) || defined(_M_IX86)

	#define WAVE_X86

	#if defined(__x86_64__) || defined(_M_X64)
		#define WAVE_ADDRESS_BITS 64
	#else
		#define WAVE_ADDRESS_BITS 32
	#endif

	#define WAVE_HAS_SSE

	#define WAVE_VECTOR_ALIGNMENT 16
	#define WAVE_DVECTOR_ALIGNMENT 32

	// Detect enabled instruction sets
	#if defined(__AVX512F__) && defined(__AVX512VL__) && defined(__AVX512DQ__) && !defined(WAVE_HAS_AVX512)
	#define WAVE_HAS_AVX512
	#endif
	#if (defined(__AVX2__) || defined(WAVE_HAS_AVX512)) && !defined(WAVE_HAS_AVX2)
	#define WAVE_HAS_AVX2
	#endif
	#if (defined(__AVX__) || defined(WAVE_HAS_AVX2)) && !defined(WAVE_HAS_AVX)
	#define WAVE_HAS_AVX
	#endif
	#if (defined(__SSE4_2__) || defined(WAVE_HAS_AVX)) && !defined(WAVE_HAS_SSE4_2)
	#define WAVE_HAS_SSE4_2
	#endif
	#if (defined(__SSE4_1__) || defined(WAVE_HAS_SSE4_2)) && !defined(WAVE_HAS_SSE4_1)
	#define WAVE_HAS_SSE4_1
	#endif
	#if (defined(__F16C__) || defined(WAVE_HAS_AVX2)) && !defined(WAVE_HAS_F16C)
	#define WAVE_HAS_F16C
	#endif
	#if (defined(__LZCNT__) || defined(WAVE_HAS_AVX2)) && !defined(WAVE_HAS_LZCNT)
	#define WAVE_HAS_LZCNT
	#endif
	#if (defined(__BMI__) || defined(WAVE_HAS_AVX2)) && !defined(WAVE_HAS_TZCNT)
	#define WAVE_HAS_TZCNT
	#endif
	#ifndef WAVE_CROSS_PLATFORM // FMA is not compatible with cross platform determinism
	#if defined(WAVE_CLANG) || defined(WAVE_GCC)
	#if defined(__FMA__) && !defined(WAVE_HAS_FMADD)
	#define WAVE_HAS_FMADD
	#endif
	#elif defined(WAVE_MSVC)
	#if defined(__AVX2__) && !defined(WAVE_HAS_FMADD) // AVX2 also enables fHASd multiply add
	#define WAVE_HAS_FMADD
	#endif
	#else
	#error Undefined compiler
	#endif
	#endif

#elif defined(__aarch64__) || defined(_M_ARM64) || defined(__arm__) || defined(_M_ARM)
#define WAVE_ARM
#if defined(__aarch64__) || defined(_M_ARM64)
#define WAVE_ADDRESS_BITS 64
#define WAVE_HAS_NEON
#define WAVE_VECTOR_ALIGNMENT 16
#define WAVE_DVECTOR_ALIGNMENT 32
#else
#define WAVE_ADDRESS_BITS 32
#define WAVE_VECTOR_ALIGNMENT 8
#define WAVE_DVECTOR_ALIGNMENT 8
#endif

#ifdef WAVE_MSVC
WAVE_INLINE float32x4_t NeonShuffleFloat32x4(float32x4_t inV1, float32x4_t inV2)
{
	float32x4_t ret;
	ret = vmovq_n_f32(vgetq_lane_f32(I1 >= 4 ? inV2 : inV1, I1 & 0b11));
	ret = vsetq_lane_f32(vgetq_lane_f32(I2 >= 4 ? inV2 : inV1, I2 & 0b11), ret, 1);
	ret = vsetq_lane_f32(vgetq_lane_f32(I3 >= 4 ? inV2 : inV1, I3 & 0b11), ret, 2);
	ret = vsetq_lane_f32(vgetq_lane_f32(I4 >= 4 ? inV2 : inV1, I4 & 0b11), ret, 3);
	return ret;
}

//doesn't work, I guess?!
#define WAVE_SHUFFLE(vec1, vec2, index1, index2, index3, index4) NeonShuffleFloat32x4(vec1, vec2)
#else
#define WAVE_SHUFFLE(vec1, vec2, index1, index2, index3, index4) __builtin_shufflevector(vec1, vec2, index1, index2, index3, index4)
#endif
#endif

#if defined(WAVE_HAS_SSE)
#include <immintrin.h>
#elif defined(WAVE_HAS_NEON)
#ifdef WAVE_MSVC
#include <intrin.h>
#include <arm64_neon.h>
#else
#include <arm_neon.h>
#endif
#endif

#if defined(WAVE_HAS_SSE)
typedef __m128 Wave128;
#elif defined(WAVE_HAS_NEON)
typedef float32x4_t Wave128;
#else
typedef float Wave128[4];
#endif

//vector
typedef struct
{
	union
	{
		float x;
		float u;
		float s;
		float r;
	};

	union
	{
		float y;
		float t;
		float v;
		float g;
	};
} vec2;

typedef struct
{
	union
	{
		float x;
		float u;
		float s;
		float r;
	};

	union
	{
		float y;
		float v;
		float t;
		float g;
	};

	union
	{
		float z;
		float p;
		float b;
	};
} vec3;
/*
typedef struct
{
	union
	{
		Wave128 Fast;
		float	mF32[4];

		union
		{
			float x;
			float u;
			float s;
			float r;
		};

		union
		{
			float y;
			float v;
			float t;
			float g;
		};

		union
		{
			float z;
			float p;
			float b;
		};

		union
		{
			float w;
			float q;
			float a;
		};
		
	};	
} vec4;
*/

typedef union
{
	Wave128 Fast;

	struct
	{
		float x;
		float y;
		float z;
		float w;
	};

	struct
	{
		float r;
		float g;
		float b;
		float a;
	};

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

void Normalize4P(vec4* v)
{
#if defined(WAVE_HAS_SSE4_1)
	Wave128 len_sq = _mm_dp_ps(v->Fast, v->Fast, 0x7f);
	Wave128 is_zero = _mm_cmpeq_ps(len_sq, _mm_setzero_ps());

	v->Fast = (Wave128)_mm_blendv_ps(_mm_div_ps(v->Fast, _mm_sqrt_ps(len_sq)), v->Fast, is_zero);
#elif defined(WAVE_HAS_NEON)
	float32x4_t mul = vmulq_f32(v->Fast, v->Fast);
	mul = vsetq_lane_f32(0, mul, 3);
	float32x4_t sum = vdupq_n_f32(vaddvq_f32(mul));
	float32x4_t len = vsqrtq_f32(sum);
	float32x4_t is_zero = vceqq_f32(len, vdupq_n_f32(0));

	v->Fast = (Wave128)vbslq_f32(is_zero, v->Fast, vdivq_f32(v->Fast, len));
#else
	register float length = FastInverseSqrt(v.x * v.x + v.y * v.y + v.z * v.z + v.w * v.w);
	v.x *= length;
	v.y *= length;
	v.z *= length;
	v.w *= length;
	return v;
#endif
}

WAVE_INLINE vec4 Normalize4(vec4 v)
{
	Normalize4P(&v);
	return v;
}

//Basically just cross 3
void Cross4Perf(vec4* a, vec4* b, vec4* r)
{
#if defined(WAVE_HAS_SSE)
	Wave128 t1 = _mm_shuffle_ps(b->Fast, b->Fast, _MM_SHUFFLE(0, 0, 2, 1)); // Assure Z and W are the same
	t1 = _mm_mul_ps(t1, a->Fast);
	Wave128 t2 = _mm_shuffle_ps(a->Fast, a->Fast, _MM_SHUFFLE(0, 0, 2, 1)); // Assure Z and W are the same
	t2 = _mm_mul_ps(t2, b->Fast);
	Wave128 t3 = _mm_sub_ps(t1, t2);
	r->Fast = _mm_shuffle_ps(t3, t3, _MM_SHUFFLE(0, 0, 2, 1)); // Assure Z and W are the same
#elif defined(WAVE_HAS_NEON)
	printf("Cross4 may not work correctly on amd\n");
	Wave128 t1 = vextq_f32(b->Fast, b->Fast, 1); // Assure Z and W are the same
	t1 = vmulq_f32(t1, a->Fast);
	Wave128 t2 = vextq_f32(a->Fast, a->Fast, 1); // Assure Z and W are the same
	t2 = vmulq_f32(t2, b->Fast);
	Wave128 t3 = vsubq_f32(t1, t2);
	return vextq_f32(t3, t3, 1); // Assure Z and W are the same
#else
	r->x = a->y * b->z - a->z * b->y;
	r->y = a->z * b->x - a->x * b->z;
	r->z = a->x * b->y - a->y * b->x;
	r->z = 1.0;
#endif
}

WAVE_INLINE vec4 Cross4P(vec4* a, vec4* b)
{
	vec4 r;
	Cross4Perf(a, b, &r);
	return r;
}


WAVE_INLINE vec4 Cross4(vec4 a, vec4 b)
{
	vec4 r;
	Cross4Perf(&a, &b, &r);
	return r;
}

//Basically dot 3
WAVE_INLINE float Dot4P(vec4* a, vec4* b)
{
#if defined(WAVE_HAS_SSE4_1)
	return _mm_cvtss_f32(_mm_dp_ps(a->Fast, b->Fast, 0x7f));
#elif defined(WAVE_HAS_NEON)
	float32x4_t mul = vmulq_f32(a->Fast, b->Fast);
	mul = vsetq_lane_f32(0, mul, 3);
	return vaddvq_f32(mul);
#else
	return a->x * b->x + a->y * b->y + a->z * b->z;
#endif
}

//Basically dot 3
WAVE_INLINE float Dot4(vec4 a, vec4 b)
{
	return Dot4P(&a, &b);
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

WAVE_INLINE vec3 Cross3P(vec3* a, vec3* b)
{
	vec3 r;
	r.x = a->y * b->z - a->z * b->y;
	r.y = a->z * b->x - a->x * b->z;
	r.z = a->x * b->y - a->y * b->x;
	return r;
}

WAVE_INLINE float Dot2P(vec2* a, vec2* b)
{
	return a->x * b->x + a->y * b->y;
}

WAVE_INLINE float Dot3P(vec3* a, vec3* b)
{
	return a->x * b->x + a->y * b->y + a->z * b->z;
}

WAVE_INLINE vec2 Add2P(vec2* a, vec2* b)
{
	vec2 r;
	r.x = a->x + b->x;
	r.y = a->y + b->y;
	return r;
}

WAVE_INLINE vec2 Sub2P(vec2* a, vec2* b)
{
	vec2 r;
	r.x = a->x - b->x;
	r.y = a->y - b->y;
	return r;
}

WAVE_INLINE vec2 Mul2P(vec2* a, vec2* b)
{
	vec2 r;
	r.x = a->x * b->x;
	r.y = a->y * b->y;
	return r;
}

WAVE_INLINE vec2 Div2P(vec2* a, vec2* b)
{
	vec2 r;
	r.x = a->x / b->x;
	r.y = a->y / b->y;
	return r;
}

WAVE_INLINE vec3 Add3P(vec3* a, vec3* b)
{
	vec3 r;
	r.x = a->x + b->x;
	r.y = a->y + b->y;
	r.z = a->z + b->z;
	return r;
}

WAVE_INLINE vec3 Sub3P(vec3* a, vec3* b)
{
	vec3 r;
	r.x = a->x - b->x;
	r.y = a->y - b->y;
	r.z = a->z - b->z;
	return r;
}

WAVE_INLINE vec3 Mul3P(vec3* a, vec3* b)
{
	vec3 r;
	r.x = a->x * b->x;
	r.y = a->y * b->y;
	r.z = a->z * b->z;
	return r;
}

WAVE_INLINE vec3 Div3P(vec3* a, vec3* b)
{
	vec3 r;
	r.x = a->x / b->x;
	r.y = a->y / b->y;
	r.z = a->z / b->z;
	return r;
}

WAVE_INLINE vec4 Add4P(vec4* a, vec4* b)
{
#if defined(WAVE_HAS_SSE)
	vec4 r;
	r.Fast = _mm_add_ps(a->Fast, b->Fast);
	return r;
#elif defined(WAVE_HAS_NEON)
	vec4 r;
	r.Fast = vaddq_f32(a->Fast, b->Fast);
	return r;
#else
	vec4 r;
	r.x = a->x + b->x;
	r.y = a->y + b->y;
	r.z = a->z + b->z;
	r.w = a->w + b->w;
	return r;
#endif
}

WAVE_INLINE vec4 Sub4P(vec4* a, vec4* b)
{
#if defined(WAVE_HAS_SSE)
	vec4 r;
	r.Fast = _mm_sub_ps(a->Fast, b->Fast);
	return r;
#elif defined(WAVE_HAS_NEON)
	vec4 r;
	r.Fast = vsubq_f32(a->Fast, b->Fast);
	return r;
#else
	vec4 r;
	r.x = a->x - b->x;
	r.y = a->y - b->y;
	r.z = a->z - b->z;
	r.w = a->w - b->w;
	return r;
#endif
}

WAVE_INLINE vec4 Mul4P(vec4* a, vec4* b)
{
#if defined(WAVE_HAS_SSE)
	vec4 r;
	r.Fast = _mm_mul_ps(a->Fast, b->Fast);
	return r;
#elif defined(WAVE_HAS_NEON)
	vec4 r;
	r.Fast = vmulq_f32(a->Fast, b->Fast);
	return r;
#else
	vec4 r;
	r.x = a->x * b->x;
	r.y = a->y * b->y;
	r.z = a->z * b->z;
	r.w = a->w * b->w;
	return r;
#endif

	
}

WAVE_INLINE vec4 Div4P(vec4* a, vec4* b)
{
#if defined(WAVE_HAS_SSE)
	vec4 r;
	r.Fast = _mm_div_ps(a->Fast, b->Fast);
	return r;
#elif defined(WAVE_HAS_NEON)
	vec4 r;
	r.Fast = vdivq_f32(a->Fast, b->Fast);
	return r;
#else
	vec4 r;
	r.x = a->x / b->x;
	r.y = a->y / b->y;
	r.z = a->z / b->z;
	r.w = a->w / b->w;
	return r;
#endif
}


WAVE_INLINE vec3 Cross3(vec3 a, vec3 b)
{
	return Cross3P(&a, &b);
}

WAVE_INLINE float Dot2(vec2 a, vec2 b)
{
	return Dot2P(&a, &b);
}

WAVE_INLINE float Dot3(vec3 a, vec3 b)
{
	return Dot3P(&a, &b);
}

WAVE_INLINE vec2 Add2(vec2 a, vec2 b)
{
	return Add2P(&a, &b);
}

WAVE_INLINE vec2 Sub2(vec2 a, vec2 b)
{
	return Sub2P(&a, &b);
}

WAVE_INLINE vec2 Mul2(vec2 a, vec2 b)
{
	return Mul2P(&a, &b);
}

WAVE_INLINE vec2 Div2(vec2 a, vec2 b)
{
	return Div2P(&a, &b);
}

WAVE_INLINE vec3 Add3(vec3 a, vec3 b)
{
	return Add3P(&a, &b);
}

WAVE_INLINE vec3 Sub3(vec3 a, vec3 b)
{
	return Sub3P(&a, &b);
}

WAVE_INLINE vec3 Mul3(vec3 a, vec3 b)
{
	return Mul3P(&a, &b);
}

WAVE_INLINE vec3 Div3(vec3 a, vec3 b)
{
	return Div3P(&a, &b);
}

WAVE_INLINE vec4 Add4(vec4 a, vec4 b)
{
	return Add4P(&a, &b);
}

WAVE_INLINE vec4 Sub4(vec4 a, vec4 b)
{
	return Add4P(&a, &b);
}

WAVE_INLINE vec4 Mul4(vec4 a, vec4 b)
{
	return Add4P(&a, &b);
}

WAVE_INLINE vec4 Div4(vec4 a, vec4 b)
{
	return Add4P(&a, &b);
}

WAVE_INLINE float Length3P(vec3* v)
{
	return sqrtf((v->x * v->x) + (v->y * v->y) + (v->z * v->z));
}

WAVE_INLINE float Length3(vec3 v)
{
	return Length3P(&v);
}

WAVE_INLINE float Length4P(vec4* v)
{
#if defined(WAVE_HAS_SSE4_1)
	return _mm_cvtss_f32(_mm_sqrt_ss(_mm_dp_ps(v->Fast, v->Fast, 0xff)));
#elif defined(WAVE_HAS_NEON)
	float32x4_t mul = vmulq_f32(v->Fast, v->Fast);
	float32x2_t sum = vdup_n_f32(vaddvq_f32(mul));
	return vget_lane_f32(vsqrt_f32(sum), 0);
#else
	return sqrtf((v->x * v->x) + (v->y * v->y) + (v->z * v->z) + (v->w * v->w));
#endif	
}

WAVE_INLINE float Length4(vec4 v)
{
	return Length4P(&v);
}

WAVE_INLINE vec2 Reflect2(vec2* v, vec2* n)
{
	vec2 r;
	r.x = v->x - 2 * Dot2P(v, n) * n->x;
	r.y = v->y - 2 * Dot2P(v, n) * n->y;
	return r;
}

WAVE_INLINE vec3 Reflect3(vec3* v, vec3* n)
{
	vec3 r;
	r.x = v->x - 2 * Dot3P(v, n) * n->x;
	r.y = v->y - 2 * Dot3P(v, n) * n->y;
	r.z = v->z - 2 * Dot3P(v, n) * n->z;
	return r;
}

float GetDistanceVec2P(vec2* a, vec2* b)
{
	register float DX = a->x - b->x;
	register float DY = a->y - b->y;

	DX *= DX;
	DY *= DY;

	return sqrtf(DX + DY);
}

WAVE_INLINE float GetDistanceVec2(vec2 a, vec2 b)
{
	return GetDistanceVec2P(&a, &b);
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

WAVE_INLINE float GetDistanceVec3(vec3 a, vec3 b)
{
	return GetDistanceVec3P(&a, &b);
}



WAVE_INLINE vec2 Vec2(float x, float y)
{
	vec2 r = { x, y };
	return r;
}

WAVE_INLINE vec3 Vec3(float x, float y, float z)
{
	vec3 r = { x, y, z };
	return r;
}

WAVE_INLINE void Vec4P(float x, float y, float z, float w, Wave128* r)
{
#if defined(WAVE_HAS_SSE)
	*r = _mm_set_ps(w, z, y, x);
#elif defined(WAVE_HAS_NEON)
	uint32x2_t xy = vcreate_f32(static_cast<uint64>(*reinterpret_cast<uint32*>(&x)) | (static_cast<uint64>(*reinterpret_cast<uint32*>(&y)) << 32));
	uint32x2_t zw = vcreate_f32(static_cast<uint64>(*reinterpret_cast<uint32*>(&z)) | (static_cast<uint64>(*reinterpret_cast<uint32*>(&w)) << 32));
	*r = vcombine_f32(xy, zw);
#else
	vec4 a;
	a.x = x;
	a.y = y;
	a.z = z;
	a.w = w;
	*r = a.Fast;
#endif
}

WAVE_INLINE vec4 Vec4(float x, float y, float z, float w)
{
	vec4 r;
	Vec4P(x, y, z, w, &r.Fast);
	return r;
}

WAVE_INLINE vec2 Vec2f(float x)
{
	vec2 r = { x, x };
	return r;
}

WAVE_INLINE vec3 Vec3f(float x)
{
	vec3 r = { x, x, x };
	return r;
}

WAVE_INLINE vec4 Vec4f(float x)
{
#if defined(WAVE_HAS_SSE)
	vec4 r;
	r.Fast = _mm_set_ps1(x);
	return r; 
#elif defined(WAVE_HAS_NEON)
	vec4 r;
	r.Fast = vdupq_n_f32(x);
	return r; 
#else
	vec4 r = { x, x, x, x };
	return Vec4(x, x, x, x);
#endif

	return r;
}

//matrix
/*
typedef struct
{
	float m[4][4];
} mat4;
*/
typedef union
{
	Wave128 Fast[4];
	float m[4][4];
} mat4;

void LoadMat4IdentityP(mat4* Matrix)
{
	Vec4P(1.0, 0.0, 0.0, 0.0, &Matrix->Fast[0]);
	Vec4P(0.0, 1.0, 0.0, 0.0, &Matrix->Fast[1]);
	Vec4P(0.0, 0.0, 1.0, 0.0, &Matrix->Fast[2]);
	Vec4P(0.0, 0.0, 0.0, 1.0, &Matrix->Fast[3]);


//	Matrix->m[0][0] = 1.0;
//	Matrix->m[1][0] = 0.0;
//	Matrix->m[2][0] = 0.0;
//	Matrix->m[3][0] = 0.0;
//
//	Matrix->m[0][1] = 0.0;
//	Matrix->m[1][1] = 1.0;
//	Matrix->m[2][1] = 0.0;
//	Matrix->m[3][1] = 0.0;
//
//	Matrix->m[0][2] = 0.0;
//	Matrix->m[1][2] = 0.0;
//	Matrix->m[2][2] = 1.0;
//	Matrix->m[3][2] = 0.0;
//
//	Matrix->m[0][3] = 0.0;
//	Matrix->m[1][3] = 0.0;
//	Matrix->m[2][3] = 0.0;
//	Matrix->m[3][3] = 1.0;
}

mat4 LoadMat4Identity()
{
	mat4 Matrix;
	LoadMat4IdentityP(&Matrix);
	return Matrix;
}

mat4 MultiplyMat4P(mat4* a, mat4* b)
{
	mat4 r;
#if defined(WAVE_HAS_SSE)
	for (int i = 0; i < 4; ++i)
	{
		__m128 c = b->Fast[i];
		__m128 t = _mm_mul_ps(a->Fast[0], _mm_shuffle_ps(c, c, _MM_SHUFFLE(0, 0, 0, 0)));
		t = _mm_add_ps(t, _mm_mul_ps(a->Fast[1], _mm_shuffle_ps(c, c, _MM_SHUFFLE(1, 1, 1, 1))));
		t = _mm_add_ps(t, _mm_mul_ps(a->Fast[2], _mm_shuffle_ps(c, c, _MM_SHUFFLE(2, 2, 2, 2))));
		t = _mm_add_ps(t, _mm_mul_ps(a->Fast[3], _mm_shuffle_ps(c, c, _MM_SHUFFLE(3, 3, 3, 3))));
		r.Fast[i] = t;
	}
#elif defined(WAVE_HAS_NEON)
	for (int i = 0; i < 4; ++i)
	{
		Type c = b->Fast[i];
		Type t = vmulq_f32(a->Fast[0], vdupq_laneq_f32(c, 0));
		t = vmlaq_f32(t, a->Fast[1], vdupq_laneq_f32(c, 1));
		t = vmlaq_f32(t, a->Fast[2], vdupq_laneq_f32(c, 2));
		t = vmlaq_f32(t, a->Fast[3], vdupq_laneq_f32(c, 3));
		r.Fast[i] = t;
	}
#else
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

#endif

	
	return r;
}

mat4 MultiplyMat4(mat4 a, mat4 b)
{
	return MultiplyMat4P(&a, &b);
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
	/*
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
	*/

	Vec4P(1.0, 0.0, 0.0, 0.0, &Matrix.Fast[0]);
	Vec4P(0.0, 1.0, 0.0, 0.0, &Matrix.Fast[1]);
	Vec4P(0.0, 0.0, 1.0, 0.0, &Matrix.Fast[2]);
	Vec4P(v->x, v->y, v->z, 1.0, &Matrix.Fast[3]);

	return MultiplyMat4P(&Matrix, m);
}

WAVE_INLINE mat4 TranslateMat4(mat4 m, vec3 v)
{
	/*
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
	*/

	return TranslateMat4P(&m, &v);
}

mat4 ScaleMat4P(mat4* m, vec3* v)
{
	mat4 Matrix;
	/*
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
	*/

	Vec4P(v->x, 0.0, 0.0, 0.0, &Matrix.Fast[0]);
	Vec4P(0.0, v->y, 0.0, 0.0, &Matrix.Fast[1]);
	Vec4P(0.0, 0.0, v->z, 0.0, &Matrix.Fast[2]);
	Vec4P(0.0, 0.0, 0.0, 1.0, &Matrix.Fast[3]);

	return MultiplyMat4P(&Matrix, m);
}

WAVE_INLINE mat4 ScaleMat4(mat4 m, vec3 v)
{
	/*
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
	*/

	return ScaleMat4P(&m, &v);
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

	Vec4P(1.0, 0.0, 0.0, 0.0, &Matrix.Fast[0]);
	Vec4P(0.0, Cos, Sin, 0.0, &Matrix.Fast[1]);
	Vec4P(0.0, -Sin, Cos, 0.0, &Matrix.Fast[2]);
	Vec4P(0.0, 0.0, 0.0, 1.0, &Matrix.Fast[3]);

	return MultiplyMat4P(&Matrix, m);
}

mat4 RotateYMat4P(mat4* m, float Angle)
{
	mat4 Matrix;

	register float Cos = cosf(Angle);
	register float Sin = sinf(Angle);

	Vec4P(Cos, 0.0, -Sin, 0.0, &Matrix.Fast[0]);
	Vec4P(0.0, 1.0, 0.0, 0.0, &Matrix.Fast[1]);
	Vec4P(Sin, 0.0, Cos, 0.0, &Matrix.Fast[2]);
	Vec4P(0.0, 0.0, 0.0, 1.0, &Matrix.Fast[3]);

	return MultiplyMat4P(&Matrix, m);
}

mat4 RotateZMat4P(mat4* m, float Angle)
{
	mat4 Matrix;

	register float Cos = cosf(Angle);
	register float Sin = sinf(Angle);

	Vec4P(Cos, Sin, 0.0, 0.0, &Matrix.Fast[0]);
	Vec4P(-Sin, Cos, 0.0, 0.0, &Matrix.Fast[1]);
	Vec4P(0.0, 0.0, 1.0, 0.0, &Matrix.Fast[2]);
	Vec4P(0.0, 0.0, 0.0, 1.0, &Matrix.Fast[3]);

	return MultiplyMat4P(&Matrix, m);
}

WAVE_INLINE mat4 RotateXMat4(mat4 m, float Angle)
{
	return RotateXMat4P(&m, Angle);
}

WAVE_INLINE mat4 RotateYMat4(mat4 m, float Angle)
{
	return RotateYMat4P(&m, Angle);
}

WAVE_INLINE mat4 RotateZMat4(mat4 m, float Angle)
{
	return RotateZMat4P(&m, Angle);
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


mat4 LookAtMat4(vec3 Pos, vec3 LookAt, vec3 Up)
{
	return LookAtMat4P(&Pos, &LookAt, &Up);
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


mat4 PerspectiveMat4(float Aspect, float FOV, float NearPlane, float FarPlane)
{
	mat4 m;
	PerspectiveMat4P(Aspect, FOV, NearPlane, FarPlane, &m);
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

mat4 OrthoMat4(float Left, float Right, float Bottom, float Top, float NearZ, float FarZ)
{
	mat4 m;
	OrthoMat4P(Left, Right, Bottom, Top, NearZ, FarZ, &m);
	return m;
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

mat4 TransposeMat4(mat4* Mat)
{
	mat4 Matrix =
	{
		Mat->m[0][0], Mat->m[1][0], Mat->m[2][0], Mat->m[3][0],
		Mat->m[0][1], Mat->m[1][1], Mat->m[2][1], Mat->m[3][1],
		Mat->m[0][2], Mat->m[1][2], Mat->m[2][2], Mat->m[3][2],
		Mat->m[0][3], Mat->m[1][3], Mat->m[2][3], Mat->m[3][3]
	};

	return Matrix;
}

//helper
WAVE_INLINE void PrintVec2(vec2* a)
{
	printf("%f %f\n", a->x, a->y);
}

WAVE_INLINE void PrintVec3(vec3* a)
{
	printf("%f %f %f\n", a->x, a->y, a->z);
}

WAVE_INLINE void PrintVec4(vec4* a)
{
	printf("%f %f %f %f\n", a->x, a->y, a->z, a->w);
}

WAVE_INLINE int RandomInt(int Min, int Max)
{
	return (rand() % (Max - Min + 1)) + Min;
}

WAVE_INLINE float ToRadians(float Angle)
{
	return 0.01745329251 * Angle;
}