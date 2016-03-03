// gb_math.h - v0.03 - public domain C math library - no warranty implied; use at your own risk
// A C math library geared towards game development

/*
Version History:
	0.03  - Complete Replacement
	0.01  - Initial Version

LICENSE
	This software is in the public domain. Where that dedication is not
	recognized, you are granted a perpetual, irrevocable license to copy,
	distribute, and modify this file as you see fit.

WARNING
	- This library is _slightly_ experimental and features may not work as expected.
	- This also means that many functions are not documented.

CONTENTS
	- Common Macros
	- Types
		- Vec(2,3,4)
		- Mat(2,3,4)
		- Float(2,3,4)
		- Rect(2,3)
		- Aabb(2,3)
	- Operations
	- Functions
	- Type Functions
	- Random

TODO
	- Complex
	- Quaternion
	- More Math Functions
*/

#ifndef GB_MATH_INCLUDE_GB_MATH_H
#define GB_MATH_INCLUDE_GB_MATH_H

#include <stddef.h>

#ifdef GB_MATH_STATIC
#define GB_MATH_DEF static
#else
#define GB_MATH_DEF extern
#endif

typedef union Vec2
{
	struct { float x, y; };
	float e[2];
} Vec2;

typedef union Vec3
{
	struct { float x, y, z; };
	struct { float r, g, b; };

	Vec2 xy;
	float e[2];
} Vec3;

typedef union Vec4
{
	struct { float x, y, z, w; };
	struct { float r, g, b, a; };
	struct { Vec2 xy, zw; };
	Vec3 xyz;
	Vec3 rgb;
	float e[4];
} Vec4;



typedef union Mat2
{
	struct { Vec2 x, y; };
	Vec4 col[2];
	float e[4];
} Mat2;

typedef union Mat3
{
	struct { Vec3 x, y, z; };
	Vec4 col[3];
	float e[9];
} Mat3;

typedef union Mat4
{
	struct { Vec4 x, y, z, w; };
	Vec4 col[4];
	float e[16];
} Mat4;




typedef float Float2[2];
typedef float Float3[3];
typedef float Float4[4];


typedef struct Rect2 { Vec2 pos, dim; } Rect2;
typedef struct Rect3 { Vec3 pos, dim; } Rect3;

typedef struct Aabb2 { Vec2 center, half_size; } Aabb2;
typedef struct Aabb3 { Vec3 center, half_size; } Aabb3;

#if defined(_MSC_VER)
	typedef unsigned __int8  gb_math_u8;
	typedef unsigned __int32 gb_math_u32;
	typedef unsigned __int64 gb_math_u64;
#else
	#include <stdint.h>
	typedef uint8_t  gb_math_u8;
	typedef uint32_t gb_math_u32;
	typedef uint64_t gb_math_u64;
#endif

// Constants
#define MATH_TAU 6.28318530718f

#if defined(__cplusplus)
extern "C" {
#endif

// Vector

GB_MATH_DEF Vec2 vec2_zero(void);
GB_MATH_DEF Vec2 vec2(float x, float y);
GB_MATH_DEF Vec2 vec2v(float x[2]);

GB_MATH_DEF Vec3 vec3_zero(void);
GB_MATH_DEF Vec3 vec3(float x, float y, float z);
GB_MATH_DEF Vec3 vec3v(float x[3]);

GB_MATH_DEF Vec4 vec4_zero(void);
GB_MATH_DEF Vec4 vec4(float x, float y, float z, float w);
GB_MATH_DEF Vec4 vec4v(float x[4]);

GB_MATH_DEF void vec2_add(Vec2 *d, Vec2 v0, Vec2 v1);
GB_MATH_DEF void vec2_sub(Vec2 *d, Vec2 v0, Vec2 v1);
GB_MATH_DEF void vec2_mul(Vec2 *d, Vec2 v,  float s);
GB_MATH_DEF void vec2_div(Vec2 *d, Vec2 v,  float s);

GB_MATH_DEF void vec3_add(Vec3 *d, Vec3 v0, Vec3 v1);
GB_MATH_DEF void vec3_sub(Vec3 *d, Vec3 v0, Vec3 v1);
GB_MATH_DEF void vec3_mul(Vec3 *d, Vec3 v,  float s);
GB_MATH_DEF void vec3_div(Vec3 *d, Vec3 v,  float s);

GB_MATH_DEF void vec4_add(Vec4 *d, Vec4 v0, Vec4 v1);
GB_MATH_DEF void vec4_sub(Vec4 *d, Vec4 v0, Vec4 v1);
GB_MATH_DEF void vec4_mul(Vec4 *d, Vec4 v,  float s);
GB_MATH_DEF void vec4_div(Vec4 *d, Vec4 v,  float s);

GB_MATH_DEF void vec2_addeq(Vec2 *d, Vec2 v);
GB_MATH_DEF void vec2_subeq(Vec2 *d, Vec2 v);
GB_MATH_DEF void vec2_muleq(Vec2 *d, float s);
GB_MATH_DEF void vec2_diveq(Vec2 *d, float s);

GB_MATH_DEF void vec3_addeq(Vec3 *d, Vec3 v);
GB_MATH_DEF void vec3_subeq(Vec3 *d, Vec3 v);
GB_MATH_DEF void vec3_muleq(Vec3 *d, float s);
GB_MATH_DEF void vec3_diveq(Vec3 *d, float s);

GB_MATH_DEF void vec4_addeq(Vec4 *d, Vec4 v);
GB_MATH_DEF void vec4_subeq(Vec4 *d, Vec4 v);
GB_MATH_DEF void vec4_muleq(Vec4 *d, float s);
GB_MATH_DEF void vec4_diveq(Vec4 *d, float s);

GB_MATH_DEF float vec2_dot(Vec2 v0, Vec2 v1);
GB_MATH_DEF float vec3_dot(Vec3 v0, Vec3 v1);
GB_MATH_DEF float vec4_dot(Vec4 v0, Vec4 v1);

GB_MATH_DEF void vec2_cross(float *d, Vec2 v0, Vec2 v1);
GB_MATH_DEF void vec3_cross(Vec3 *d, Vec3 v0, Vec3 v1);

GB_MATH_DEF float vec2_mag2(Vec2 v);
GB_MATH_DEF float vec3_mag2(Vec3 v);
GB_MATH_DEF float vec4_mag2(Vec4 v);

GB_MATH_DEF float vec2_mag(Vec2 v);
GB_MATH_DEF float vec3_mag(Vec3 v);
GB_MATH_DEF float vec4_mag(Vec4 v);

GB_MATH_DEF void vec2_norm(Vec2 *d, Vec2 v);
GB_MATH_DEF void vec3_norm(Vec3 *d, Vec3 v);
GB_MATH_DEF void vec4_norm(Vec4 *d, Vec4 v);

GB_MATH_DEF float vec2_aspect(Vec2 v);

// Matrix

GB_MATH_DEF void mat2_identity(Mat2 *m);
GB_MATH_DEF void float22_identity(float m[2][2]);

GB_MATH_DEF void mat2_transpose(Mat2 *m);
GB_MATH_DEF void mat2_mul(Mat2 *out, Mat2 *m1, Mat2 *m2);
GB_MATH_DEF void mat2_mul_vec2(Vec2 *out, Mat2 *m, Vec2 in);

GB_MATH_DEF Mat2 *mat2_v(Vec2 m[2]);
GB_MATH_DEF Mat2 *mat2_f(float m[2][2]);
GB_MATH_DEF Float2 *float22_m(Mat2 *m);
GB_MATH_DEF Float2 *float22_v(Vec2 m[2]);
GB_MATH_DEF Float2 *float22_4(float m[4]);

GB_MATH_DEF void float22_transpose(float (*vec)[2]);
GB_MATH_DEF void float22_mul(float (*out)[2], float (*mat1)[2], float (*mat2)[2]);
GB_MATH_DEF void float22_mul_vec2(Vec2 *out, float m[2][2], Vec2 in);


GB_MATH_DEF void mat3_identity(Mat3 *m);
GB_MATH_DEF void float33_identity(float m[3][3]);

GB_MATH_DEF void mat3_transpose(Mat3 *m);
GB_MATH_DEF void mat3_mul(Mat3 *out, Mat3 *m1, Mat3 *m2);
GB_MATH_DEF void mat3_mul_vec3(Vec3 *out, Mat3 *m, Vec3 in);

GB_MATH_DEF Mat3 *mat3_v(Vec3 m[3]);
GB_MATH_DEF Mat3 *mat3_f(float m[3][3]);
GB_MATH_DEF Float3 *float33_m(Mat3 *m);
GB_MATH_DEF Float3 *float33_v(Vec3 m[3]);
GB_MATH_DEF Float3 *float33_9(float m[9]);

GB_MATH_DEF void float33_transpose(float (*vec)[3]);
GB_MATH_DEF void float33_mul(float (*out)[3], float (*mat1)[3], float (*mat2)[3]);
GB_MATH_DEF void float33_mul_vec3(Vec3 *out, float m[3][3], Vec3 in);


GB_MATH_DEF void mat4_identity(Mat4 *m);
GB_MATH_DEF void float44_identity(float m[4][4]);

GB_MATH_DEF void mat4_transpose(Mat4 *m);
GB_MATH_DEF void mat4_mul(Mat4 *out, Mat4 *m1, Mat4 *m2);
GB_MATH_DEF void mat4_mul_vec4(Vec4 *out, Mat4 *m, Vec4 in);

GB_MATH_DEF Mat4 *mat4_v(Vec4 m[4]);
GB_MATH_DEF Mat4 *mat4_f(float m[4][4]);
GB_MATH_DEF Float4 *float44_m(Mat4 *m);
GB_MATH_DEF Float4 *float44_v(Vec4 m[4]);
GB_MATH_DEF Float4 *float44_16(float m[16]);

GB_MATH_DEF void float44_transpose(float (*vec)[4]);
GB_MATH_DEF void float44_mul(float (*out)[4], float (*mat1)[4], float (*mat2)[4]);
GB_MATH_DEF void float44_mul_vec4(Vec4 *out, float m[4][4], Vec4 in);


// Hermite Interpolations
GB_MATH_DEF float lerp(float a, float b, float t);
GB_MATH_DEF float smooth_step(float a, float b, float t);
GB_MATH_DEF float smoother_step(float a, float b, float t);

GB_MATH_DEF void vec2_lerp(Vec2 *d, Vec2 a, Vec2 b, float t);
GB_MATH_DEF void vec3_lerp(Vec3 *d, Vec3 a, Vec3 b, float t);
GB_MATH_DEF void vec4_lerp(Vec4 *d, Vec4 a, Vec4 b, float t);


// Angles
GB_MATH_DEF float to_radians(float degrees);
GB_MATH_DEF float to_degrees(float radians);

// Projections
// NOTE(bill): Uses OpenGL Projection Conventions (-1 to 1)
// NOTE(bill): If Direct3D is needed, use custom projection to shift-z to (0 to 1)
GB_MATH_DEF void mat4_ortho2d(Mat4 *out, float left, float right, float bottom, float top);
GB_MATH_DEF void mat4_ortho3d(Mat4 *out, float left, float right, float bottom, float top, float z_near, float z_far);

#ifndef	MURMUR64_SEED
#define MURMUR64_SEED 0x9747b28c
#endif
// Hashing
GB_MATH_DEF gb_math_u64 hash_murmur64(void const *key, size_t num_bytes);

// Random
// TODO(bill): Use a generator for the random numbers
GB_MATH_DEF float random_range_float(float min_inc, float max_inc);
GB_MATH_DEF int random_range_int(int min_inc, int max_inc);


#if defined(__cplusplus)
}
#endif


#endif // GB_MATH_INCLUDE_GB_MATH_H

////////////////////
//                //
// Implementation //
//                //
////////////////////

#if defined(GB_MATH_IMPLEMENTATION)

#include <math.h>

#define VEC2_2OP(a,c,post)  \
	a->x =        c.x post; \
	a->y =        c.y post;

#define VEC2_3OP(a,b,op,c,post) \
	a->x = b.x op c.x post;     \
	a->y = b.y op c.y post;

#define VEC3_2OP(a,c,post)  \
	a->x =        c.x post; \
	a->y =        c.y post; \
	a->z =        c.z post;

#define VEC3_3OP(a,b,op,c,post) \
	a->x = b.x op c.x post;     \
	a->y = b.y op c.y post;     \
	a->z = b.z op c.z post;

#define VEC4_2OP(a,c,post)  \
	a->x =        c.x post; \
	a->y =        c.y post; \
	a->z =        c.z post; \
	a->w =        c.w post;

#define VEC4_3OP(a,b,op,c,post) \
	a->x = b.x op c.x post;     \
	a->y = b.y op c.y post;     \
	a->z = b.z op c.z post;     \
	a->w = b.w op c.w post;


Vec2 vec2_zero(void)        { Vec2 v = {0, 0};       return v; }
Vec2 vec2(float x, float y) { Vec2 v = {x, y};       return v; }
Vec2 vec2v(float x[2])      { Vec2 v = {x[0], x[1]}; return v; }

Vec3 vec3_zero(void)                 { Vec3 v = {0, 0, 0};          return v; }
Vec3 vec3(float x, float y, float z) { Vec3 v = {x, y, z};          return v; }
Vec3 vec3v(float x[3])               { Vec3 v = {x[0], x[1], x[2]}; return v; }

Vec4 vec4_zero(void)                          { Vec4 v = {0, 0, 0, 0};             return v; }
Vec4 vec4(float x, float y, float z, float w) { Vec4 v = {x, y, z, w};             return v; }
Vec4 vec4v(float x[4])                        { Vec4 v = {x[0], x[1], x[2], x[3]}; return v; }


void vec2_add(Vec2 *d, Vec2 v0, Vec2 v1) { VEC2_3OP(d,v0,+,v1,+0); }
void vec2_sub(Vec2 *d, Vec2 v0, Vec2 v1) { VEC2_3OP(d,v0,-,v1,+0); }
void vec2_mul(Vec2 *d, Vec2 v,  float s) { VEC2_2OP(d,v,* s);      }
void vec2_div(Vec2 *d, Vec2 v,  float s) { VEC2_2OP(d,v,/ s);      }

void vec3_add(Vec3 *d, Vec3 v0, Vec3 v1) { VEC3_3OP(d,v0,+,v1,+0); }
void vec3_sub(Vec3 *d, Vec3 v0, Vec3 v1) { VEC3_3OP(d,v0,-,v1,+0); }
void vec3_mul(Vec3 *d, Vec3 v,  float s) { VEC3_2OP(d,v,* s);      }
void vec3_div(Vec3 *d, Vec3 v,  float s) { VEC3_2OP(d,v,/ s);      }

void vec4_add(Vec4 *d, Vec4 v0, Vec4 v1) { VEC4_3OP(d,v0,+,v1,+0); }
void vec4_sub(Vec4 *d, Vec4 v0, Vec4 v1) { VEC4_3OP(d,v0,-,v1,+0); }
void vec4_mul(Vec4 *d, Vec4 v,  float s) { VEC4_2OP(d,v,* s);      }
void vec4_div(Vec4 *d, Vec4 v,  float s) { VEC4_2OP(d,v,/ s);      }


void vec2_addeq(Vec2 *d, Vec2 v)  { VEC2_3OP(d,(*d),+,v,+0); }
void vec2_subeq(Vec2 *d, Vec2 v)  { VEC2_3OP(d,(*d),-,v,+0); }
void vec2_muleq(Vec2 *d, float s) { VEC2_2OP(d,(*d),* s);    }
void vec2_diveq(Vec2 *d, float s) { VEC2_2OP(d,(*d),/ s);    }

void vec3_addeq(Vec3 *d, Vec3 v)  { VEC3_3OP(d,(*d),+,v,+0); }
void vec3_subeq(Vec3 *d, Vec3 v)  { VEC3_3OP(d,(*d),-,v,+0); }
void vec3_muleq(Vec3 *d, float s) { VEC3_2OP(d,(*d),* s);    }
void vec3_diveq(Vec3 *d, float s) { VEC3_2OP(d,(*d),/ s);    }

void vec4_addeq(Vec4 *d, Vec4 v)  { VEC4_3OP(d,(*d),+,v,+0); }
void vec4_subeq(Vec4 *d, Vec4 v)  { VEC4_3OP(d,(*d),-,v,+0); }
void vec4_muleq(Vec4 *d, float s) { VEC4_2OP(d,(*d),* s);    }
void vec4_diveq(Vec4 *d, float s) { VEC4_2OP(d,(*d),/ s);    }


float vec2_dot(Vec2 v0, Vec2 v1) { return v0.x*v1.x + v0.y*v1.y; }
float vec3_dot(Vec3 v0, Vec3 v1) { return v0.x*v1.x + v0.y*v1.y + v0.z*v1.z; }
float vec4_dot(Vec4 v0, Vec4 v1) { return v0.x*v1.x + v0.y*v1.y + v0.z*v1.z + v0.w*v1.w; }

void vec2_cross(float *d, Vec2 v0, Vec2 v1) { *d = v0.x*v1.y - v1.x*v0.y; }
void vec3_cross(Vec3 *d, Vec3 v0, Vec3 v1)  { d->x = v0.y*v1.z - v0.z*v1.y;
	                                          d->y = v0.z*v1.x - v0.x*v1.z;
	                                          d->z = v0.x*v1.y - v0.y*v1.x; }

float vec2_mag2(Vec2 v) { return vec2_dot(v, v); }
float vec3_mag2(Vec3 v) { return vec3_dot(v, v); }
float vec4_mag2(Vec4 v) { return vec4_dot(v, v); }

// TODO(bill): Create custom sqrt function
float vec2_mag(Vec2 v) { return sqrtf(vec2_dot(v, v)); }
float vec3_mag(Vec3 v) { return sqrtf(vec3_dot(v, v)); }
float vec4_mag(Vec4 v) { return sqrtf(vec4_dot(v, v)); }

// TODO(bill): Should I calculate inv_sqrt directly?
void vec2_norm(Vec2 *d, Vec2 v) { vec2_div(d, v, vec2_mag(v)); }
void vec3_norm(Vec3 *d, Vec3 v) { vec3_div(d, v, vec3_mag(v)); }
void vec4_norm(Vec4 *d, Vec4 v) { vec4_div(d, v, vec4_mag(v)); }

float vec2_aspect(Vec2 v)
{
	if (v.y < 0.0001f)
		return 0.0f;
	return v.x/v.y;
}





void mat2_transpose(Mat2 *m) { float22_transpose(float22_m(m)); }
void mat2_identity(Mat2 *m)  { float22_identity(float22_m(m));  }
void mat2_mul(Mat2 *out, Mat2 *m1, Mat2 *m2) { float22_mul(float22_m(out), float22_m(m1), float22_m(m2)); }

void
float22_identity(float m[2][2])
{
	m[0][0] = 1; m[0][1] = 0;
	m[1][0] = 0; m[1][1] = 1;
}

void
mat2_mul_vec2(Vec2 *out, Mat2 *m, Vec2 in)
{
	float22_mul_vec2(out, float22_m(m), in);
}

Mat2 *mat2_v(Vec2 m[2])     { return (Mat2 *)m; }
Mat2 *mat2_f(float m[2][2]) { return (Mat2 *)m; }

Float2 *float22_m(Mat2 *m)    { return (Float2 *)m; }
Float2 *float22_v(Vec2 m[2])  { return (Float2 *)m; }
Float2 *float22_4(float m[4]) { return (Float2 *)m; }

void
float22_transpose(float (*vec)[2])
{
	int i, j;
	for (j = 0; j < 2; j++) {
		for (i = j + 1; i < 2; i++) {
			float t = vec[i][j];
			vec[i][j] = vec[j][i];
			vec[j][i] = t;
		}
	}
}

void
float22_mul(float (*out)[2], float (*mat1)[2], float (*mat2)[2])
{
	int i, j;
	float temp1[2][2], temp2[2][2];
	if (mat1 == out) { memcpy(temp1, mat1, sizeof(temp1)); mat1 = temp1; }
	if (mat2 == out) { memcpy(temp2, mat2, sizeof(temp2)); mat2 = temp2; }
	for (j = 0; j < 2; j++) {
		for (i = 0; i < 2; i++) {
			out[j][i] = mat1[0][i]*mat2[j][0]
			          + mat1[1][i]*mat2[j][1];
		}
	}
}

void
float22_mul_vec2(Vec2 *out, float m[2][2], Vec2 v)
{
	out->x = m[0][0] * v.x + m[0][1] * v.y;
	out->y = m[1][0] * v.x + m[1][1] * v.y;
}






void mat3_transpose(Mat3 *m) { float33_transpose(float33_m(m)); }
void mat3_identity(Mat3 *m)  { float33_identity(float33_m(m));  }
void mat3_mul(Mat3 *out, Mat3 *m1, Mat3 *m2) { float33_mul(float33_m(out), float33_m(m1), float33_m(m2)); }

void
float33_identity(float m[3][3])
{
	m[0][0] = 1; m[0][1] = 0; m[0][2] = 0;
	m[1][0] = 0; m[1][1] = 1; m[1][2] = 0;
	m[2][0] = 0; m[2][1] = 0; m[2][2] = 1;
}

void
mat3_mul_vec3(Vec3 *out, Mat3 *m, Vec3 in)
{
	float33_mul_vec3(out, float33_m(m), in);
}

Mat3 *mat3_v(Vec3 m[3])     { return (Mat3 *)m; }
Mat3 *mat3_f(float m[3][3]) { return (Mat3 *)m; }

Float3 *float33_m(Mat3 *m)     { return (Float3 *)m; }
Float3 *float33_v(Vec3 m[3])   { return (Float3 *)m; }
Float3 *float33_16(float m[9]) { return (Float3 *)m; }

void
float33_transpose(float (*vec)[3])
{
	int i, j;
	for (j = 0; j < 3; j++) {
		for (i = j + 1; i < 3; i++) {
			float t = vec[i][j];
			vec[i][j] = vec[j][i];
			vec[j][i] = t;
		}
	}
}

void
float33_mul(float (*out)[3], float (*mat1)[3], float (*mat2)[3])
{
	int i, j;
	float temp1[3][3], temp2[3][3];
	if (mat1 == out) { memcpy(temp1, mat1, sizeof(temp1)); mat1 = temp1; }
	if (mat2 == out) { memcpy(temp2, mat2, sizeof(temp2)); mat2 = temp2; }
	for (j = 0; j < 3; j++) {
		for (i = 0; i < 3; i++) {
			out[j][i] = mat1[0][i]*mat2[j][0]
			          + mat1[1][i]*mat2[j][1]
			          + mat1[2][i]*mat2[j][2];
		}
	}
}

void
float33_mul_vec3(Vec3 *out, float m[3][3], Vec3 v)
{
	out->x = m[0][0] * v.x + m[0][1] * v.y + m[0][2] * v.z;
	out->y = m[1][0] * v.x + m[1][1] * v.y + m[1][2] * v.z;
	out->z = m[2][0] * v.x + m[2][1] * v.y + m[2][2] * v.z;
}









void mat4_transpose(Mat4 *m) { float44_transpose(float44_m(m)); }
void mat4_identity(Mat4 *m)  { float44_identity(float44_m(m));  }
void mat4_mul(Mat4 *out, Mat4 *m1, Mat4 *m2) { float44_mul(float44_m(out), float44_m(m1), float44_m(m2)); }

void
float44_identity(float m[4][4])
{
	m[0][0] = 1; m[0][1] = 0; m[0][2] = 0; m[0][3] = 0;
	m[1][0] = 0; m[1][1] = 1; m[1][2] = 0; m[1][3] = 0;
	m[2][0] = 0; m[2][1] = 0; m[2][2] = 1; m[2][3] = 0;
	m[3][0] = 0; m[3][1] = 0; m[3][2] = 0; m[3][3] = 1;
}

void
mat4_mul_vec4(Vec4 *out, Mat4 *m, Vec4 in)
{
	float44_mul_vec4(out, float44_m(m), in);
}

Mat4 *mat4_v(Vec4 m[4])     { return (Mat4 *)m; }
Mat4 *mat4_f(float m[4][4]) { return (Mat4 *)m; }

Float4 *float44_m(Mat4 *m)      { return (Float4 *)m; }
Float4 *float44_v(Vec4 m[4])    { return (Float4 *)m; }
Float4 *float44_16(float m[16]) { return (Float4 *)m; }

void
float44_transpose(float (*vec)[4])
{
	int i, j;
	for (j = 0; j < 4; j++) {
		for (i = j + 1; i < 4; i++) {
			float t = vec[i][j];
			vec[i][j] = vec[j][i];
			vec[j][i] = t;
		}
	}
}

void
float44_mul(float (*out)[4], float (*mat1)[4], float (*mat2)[4])
{
	int i, j;
	float temp1[4][4], temp2[4][4];
	if (mat1 == out) { memcpy(temp1, mat1, sizeof(temp1)); mat1 = temp1; }
	if (mat2 == out) { memcpy(temp2, mat2, sizeof(temp2)); mat2 = temp2; }
	for (j = 0; j < 4; j++) {
		for (i = 0; i < 4; i++) {
			out[j][i] = mat1[0][i]*mat2[j][0]
			          + mat1[1][i]*mat2[j][1]
			          + mat1[2][i]*mat2[j][2]
			          + mat1[3][i]*mat2[j][3];
		}
	}
}

void
float44_mul_vec4(Vec4 *out, float m[4][4], Vec4 v)
{
	out->x = m[0][0] * v.x + m[0][1] * v.y + m[0][2] * v.z + m[0][3] * v.w;
	out->y = m[1][0] * v.x + m[1][1] * v.y + m[1][2] * v.z + m[1][3] * v.w;
	out->z = m[2][0] * v.x + m[2][1] * v.y + m[2][2] * v.z + m[2][3] * v.w;
	out->w = m[3][0] * v.x + m[3][1] * v.y + m[3][2] * v.z + m[3][3] * v.w;
}




float lerp(float a, float b, float t)          { return a*(1.0f-t) + b*t; }
float smooth_step(float a, float b, float t)   { float x = (t - a)/(b - a); return x*x*(3.0f - 2.0f*x); }
float smoother_step(float a, float b, float t) { float x = (t - a)/(b - a); return x*x*x*(x*(6.0f*x - 15.0f) + 10.0f); }

#define VEC_LERPN(N, d, a, b, t) Vec##N db; vec##N##_mul(&db, b, t); vec##N##_add(d, a, db)
void vec2_lerp(Vec2 *d, Vec2 a, Vec2 b, float t) { VEC_LERPN(2, d, a, b, t); }
void vec3_lerp(Vec3 *d, Vec3 a, Vec3 b, float t) { VEC_LERPN(3, d, a, b, t); }
void vec4_lerp(Vec4 *d, Vec4 a, Vec4 b, float t) { VEC_LERPN(4, d, a, b, t); }





float to_radians(float degrees) { return degrees * MATH_TAU / 360.0f; }
float to_degrees(float radians) { return radians * 360.0f / MATH_TAU; }






void
mat4_ortho2d(Mat4 *out, float left, float right, float bottom, float top)
{
	Float4 *m;
	mat4_identity(out);
	m = float44_m(out);

	m[0][0] = 2.0f / (right - left);
	m[1][1] = 2.0f / (top - bottom);
	m[2][2] = -1.0f;
	m[3][0] = -(right + left) / (right - left);
	m[3][1] = -(top + bottom) / (top - bottom);
}

void
mat4_ortho3d(Mat4 *out, float left, float right, float bottom, float top, float z_near, float z_far)
{
	Float4 *m;
	mat4_identity(out);
	m = float44_m(out);

	m[0][0] = +2.0f / (right - left);
	m[1][1] = +2.0f / (top - bottom);
	m[2][2] = -2.0f / (z_far - z_near);
	m[3][0] = -(right + left) / (right - left);
	m[3][1] = -(top + bottom) / (top - bottom);
	m[3][2] = -(z_far + z_near) / (z_far - z_near);
}


#if defined(__x86_64__) || defined(__ppc64__)

	u64
	hash_murmur64(void const *key, size_t num_bytes)
	{
		u64 const m = 0xc6a4a7935bd1e995ULL;
		int const r = 47;

		u64 h = MURMUR64_SEED ^ (num_bytes * m);

		u64 *data = (u64 *)(key);
		u64 *end = data + (num_bytes / 8);
		u8 *data2;

		while (data != end) {
			u64 k = *data++;
			k *= m;
			k ^= k >> r;
			k *= m;
			h ^= k;
			h *= m;
		}

		data2 = (u8 *)data;

		switch (num_bytes & 7) {
		case 7: h ^= (u64)data2[6] << 48;
		case 6: h ^= (u64)data2[5] << 40;
		case 5: h ^= (u64)data2[4] << 32;
		case 4: h ^= (u64)data2[3] << 24;
		case 3: h ^= (u64)data2[2] << 16;
		case 2: h ^= (u64)data2[1] << 8;
		case 1: h ^= (u64)data2[0];
			h *= m;
		};

		h ^= h >> r;
		h *= m;
		h ^= h >> r;

		return h;
	}
#else
	gb_math_u64
	hash_murmur64(void const *key, size_t num_bytes)
	{
		gb_math_u32 const m = 0x5bd1e995;
		gb_math_u32 const r = 24;

		gb_math_u64 h  = 0;
		gb_math_u32 h1 = (gb_math_u32)MURMUR64_SEED ^ (gb_math_u32)num_bytes;
		gb_math_u32 h2 = (gb_math_u32)((gb_math_u64)MURMUR64_SEED >> 32);

		gb_math_u32 *data = (gb_math_u32 *)key;


		while (num_bytes >= 8) {
			gb_math_u32 k1, k2;
			k1 = *data++;
			k1 *= m;
			k1 ^= k1 >> r;
			k1 *= m;
			h1 *= m;
			h1 ^= k1;
			num_bytes -= 4;

			k2 = *data++;
			k2 *= m;
			k2 ^= k2 >> r;
			k2 *= m;
			h2 *= m;
			h2 ^= k2;
			num_bytes -= 4;
		}

		if (num_bytes >= 4) {
			gb_math_u32 k1 = *data++;
			k1 *= m;
			k1 ^= k1 >> r;
			k1 *= m;
			h1 *= m;
			h1 ^= k1;
			num_bytes -= 4;
		}

		switch (num_bytes) {
		case 3: h2 ^= (gb_math_u32)((gb_math_u8 *)(data)[2]) << 16;
		case 2: h2 ^= (gb_math_u32)((gb_math_u8 *)(data)[1]) <<  8;
		case 1: h2 ^= (gb_math_u32)((gb_math_u8 *)(data)[0]) <<  0;
			h2 *= m;
		};

		h1 ^= h2 >> 18;
		h1 *= m;
		h2 ^= h1 >> 22;
		h2 *= m;
		h1 ^= h2 >> 17;
		h1 *= m;
		h2 ^= h1 >> 19;
		h2 *= m;

		h = (gb_math_u64)(h << 32) | (gb_math_u64)h2;

		return h;
	}
#endif


// TODO(bill): Make better random number generators
float
random_range_float(float min_inc, float max_inc)
{
	static int random_value = 0xdeadbeef; // Random Value
	float result;
	random_value = random_value * 2147001325 + 715136305; // BCPL generator
	result = *(float *)&random_value; //  bits
	result /= 4294967295.0f;
	result *= (max_inc - min_inc);
	result += min_inc;

	return result;
}

int
random_range_int(int min_inc, int max_inc)
{
	static int random_value = 0xdeadbeef; // Random Value
	int diff, result;
	random_value = random_value * 2147001325 + 715136305; // BCPL generator
	diff = max_inc - min_inc + 1;
	result = random_value % diff;
	result += min_inc;

	return result;
}


#endif // GB_MATH_IMPLEMENTATION
