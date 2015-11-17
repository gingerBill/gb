// gb_math.hpp - v0.01 - public domain C++11 math library - no warranty implied; use at your own risk
// (Experimental) A C++11 math library geared towards game development
// This is meant to be used the gb.hpp library but it doesn't have to be

/*
Version History:
	0.01  - Initial Version

LICENSE
	This software is in the public domain. Where that dedication is not
	recognized, you are granted a perpetual, irrevocable license to copy,
	distribute, and modify this file as you see fit.

WARNING
	- This library is _highly_ experimental and features may not work as expected.
	- This also means that many functions are not documented.

Context:
	- Common Macros
	- Assert
	- Types
		- Vector(2,3,4)
		- Complex
		- Quaternion
		- Matrix(2,3,4)
		- Euler_Angles
		- Transform
		- Aabb
		- Sphere
		- Plane
	- Operations
	- Functions & Constants
	- Type Functions
	- Random
*/

#ifndef GB_MATH_INCLUDE_GB_MATH_HPP
#define GB_MATH_INCLUDE_GB_MATH_HPP

#if !defined(__cplusplus) && __cplusplus >= 201103L
	#error This library is only for C++11 and above
#endif

// NOTE(bill): Because static means three different things in C/C++
//             Great Design(!)
#ifndef global
#define global        static
#define internal      static
#define local_persist static
#endif

#if defined(_MSC_VER)
	#define _ALLOW_KEYWORD_MACROS

	#ifndef alignof // Needed for MSVC 2013 'cause Microsoft "loves" standards
	#define alignof(x) __alignof(x)
	#endif
#endif


////////////////////////////////
///                          ///
/// System OS                ///
///                          ///
////////////////////////////////
#if defined(_WIN32) || defined(_WIN64)
	#ifndef GB_SYSTEM_WINDOWS
	#define GB_SYSTEM_WINDOWS 1
	#endif
#elif defined(__APPLE__) && defined(__MACH__)
	#ifndef GB_SYSTEM_OSX
	#define GB_SYSTEM_OSX 1
	#endif
#elif defined(__unix__)
	#ifndef GB_SYSTEM_UNIX
	#define GB_SYSTEM_UNIX 1
	#endif

	#if defined(__linux__)
		#ifndef GB_SYSTEM_LINUX
		#define GB_SYSTEM_LINUX 1
		#endif
	#elif defined(__FreeBSD__) || defined(__FreeBSD_kernel__)
		#ifndef GB_SYSTEM_FREEBSD
		#define GB_SYSTEM_FREEBSD 1
		#endif
	#else
		#error This UNIX operating system is not supported by gb.hpp
	#endif
#else
	#error This operating system is not supported by gb.hpp
#endif

////////////////////////////////
///                          ///
/// Environment Bit Size     ///
///                          ///
////////////////////////////////
#if defined(_WIN32) || defined(_WIN64)
	#if defined(_WIN64)
		#ifndef GB_ARCH_64_BIT
		#define GB_ARCH_64_BIT 1
		#endif
	#else
		#ifndef GB_ARCH_32_BIT
		#define GB_ARCH_32_BIT 1
		#endif
	#endif
#endif

// TODO(bill): Check if this KEPLER_ENVIRONMENT works on clang
#if defined(__GNUC__)
	#if defined(__x86_64__) || defined(__ppc64__)
		#ifndef GB_ARCH_64_BIT
		#define GB_ARCH_64_BIT 1
		#endif
	#else
		#ifndef GB_ARCH_32_BIT
		#define GB_ARCH_32_BIT 1
		#endif
	#endif
#endif

// TODO(bill): Get this to work
// #if !defined(GB_LITTLE_EDIAN) && !defined(GB_BIG_EDIAN)

// 	// Source: http://sourceforge.net/p/predef/wiki/Endianness/
// 	#if defined(__BYTE_ORDER) && __BYTE_ORDER == __BIG_ENDIAN || \
// 		defined(__BIG_ENDIAN__)                               || \
// 		defined(__ARMEB__)                                    || \
// 		defined(__THUMBEB__)                                  || \
// 		defined(__AARCH64EB__)                                || \
// 		defined(_MIBSEB) || defined(__MIBSEB) || defined(__MIBSEB__)
// 	// It's a big-endian target architecture
// 		#define GB_BIG_EDIAN 1

// 	#elif defined(__BYTE_ORDER) && __BYTE_ORDER == __LITTLE_ENDIAN || \
// 		defined(__LITTLE_ENDIAN__)                                 || \
// 		defined(__ARMEL__)                                         || \
// 		defined(__THUMBEL__)                                       || \
// 		defined(__AARCH64EL__)                                     || \
// 		defined(_MIPSEL) || defined(__MIPSEL) || defined(__MIPSEL__)
// 	// It's a little-endian target architecture
// 		#define GB_LITTLE_EDIAN 1

// 	#else
// 		#error I don't know what architecture this is!
// 	#endif
// #endif


#define GB_IS_POWER_OF_TWO(x) ((x) != 0) && !((x) & ((x) - 1))

#include <math.h>
#include <stdio.h>

#if !defined(GB_HAS_NO_CONSTEXPR)
	#if defined(_GNUC_VER) && _GNUC_VER < 406  // Less than gcc 4.06
		#define GB_HAS_NO_CONSTEXPR 1
	#elif defined(_MSC_VER) && _MSC_VER < 1900 // Less than Visual Studio 2015/MSVC++ 14.0
		#define GB_HAS_NO_CONSTEXPR 1
	#elif !defined(__GXX_EXPERIMENTAL_CXX0X__) && __cplusplus < 201103L
		#define GB_HAS_NO_CONSTEXPR 1
	#endif
#endif

#if defined(GB_HAS_NO_CONSTEXPR)
	#define GB_CONSTEXPR
#else
	#define GB_CONSTEXPR constexpr
#endif

#ifndef GB_FORCE_INLINE
	#if defined(_MSC_VER)
		#define GB_FORCE_INLINE __forceinline
	#else
		#define GB_FORCE_INLINE __attribute__ ((__always_inline__))
	#endif
#endif

#if defined(GB_SYSTEM_WINDOWS)
	#define NOMINMAX            1
	#define VC_EXTRALEAN        1
	#define WIN32_EXTRA_LEAN    1
	#define WIN32_LEAN_AND_MEAN 1

	#include <windows.h>
	#include <mmsystem.h> // Time functions
	#include <wincrypt.h>

	#undef NOMINMAX
	#undef VC_EXTRALEAN
	#undef WIN32_EXTRA_LEAN
	#undef WIN32_LEAN_AND_MEAN

	#include <intrin.h>
#else
	#include <pthread.h>
	#include <sys/time.h>
#endif

#ifndef GB_ARRAY_BOUND_CHECKING
#define GB_ARRAY_BOUND_CHECKING 1
#endif


#ifndef GB_DISABLE_COPY
#define GB_DISABLE_COPY(Type) \
	Type(const Type&) = delete;      \
	Type& operator=(const Type&) = delete
#endif

#if !defined(GB_ASSERT)
	#if !defined(NDEBUG)
		#define GB_ASSERT(x, ...) ((void)(::gb__assert_handler((x), #x, __FILE__, __LINE__, ##__VA_ARGS__)))

		/// Helper function used as a better alternative to assert which allows for
		/// optional printf style error messages
		extern "C" inline void
		gb__assert_handler(bool condition, const char* condition_str,
						   const char* filename, size_t line,
						   const char* error_text = nullptr, ...)
		{
			if (condition)
				return;

			fprintf(stderr, "ASSERT! %s(%lu): %s", filename, line, condition_str);
			if (error_text)
			{
				fprintf(stderr, " - ");

				va_list args;
				va_start(args, error_text);
				vfprintf(stderr, error_text, args);
				va_end(args);
			}
			fprintf(stderr, "\n");
			// TODO(bill): Get a better way to abort
			*(int*)0 = 0;
		}

	#else
		#define GB_ASSERT(x, ...) ((void)sizeof(x))
	#endif
#endif

#if !defined(__GB_NAMESPACE_PREFIX) && !defined(GB_NO_GB_NAMESPACE)
	#define __GB_NAMESPACE_PREFIX gb
#else
	#define __GB_NAMESPACE_PREFIX
#endif

#if defined(GB_NO_GB_NAMESPACE)
	#define __GB_NAMESPACE_START
	#define __GB_NAMESPACE_END
#else
	#define __GB_NAMESPACE_START namespace __GB_NAMESPACE_PREFIX {
	#define __GB_NAMESPACE_END   } // namespace __GB_NAMESPACE_PREFIX
#endif


#if !defined(GB_BASIC_WITHOUT_NAMESPACE)
__GB_NAMESPACE_START
#endif // GB_BASIC_WITHOUT_NAMESPACE

////////////////////////////////
///                          ///
/// Types                    ///
///                          ///
////////////////////////////////


#ifndef GB_BASIC_TYPES
#define GB_BASIC_TYPES
	#if defined(_MSC_VER)
		using u8  = unsigned __int8;
		using s8  =   signed __int8;
		using u16 = unsigned __int16;
		using s16 =   signed __int16;
		using u32 = unsigned __int32;
		using s32 =   signed __int32;
		using u64 = unsigned __int64;
		using s64 =   signed __int64;
	#else
		using u8  = unsigned char;
		using s8  =   signed char;
		using u16 = unsigned short;
		using s16 =   signed short;
		using u32 = unsigned int;
		using s32 =   signed int;
		using u64 = unsigned long long;
		using s64 =   signed long long;
	#endif

	static_assert( sizeof(u8) == 1,  "u8 is not  8 bits");
	static_assert(sizeof(u16) == 2, "u16 is not 16 bits");
	static_assert(sizeof(u32) == 4, "u32 is not 32 bits");
	static_assert(sizeof(u64) == 8, "u64 is not 64 bits");

	using f32 = float;
	using f64 = double;

	#if defined(GB_B8_AS_BOOL)
		using b8 = bool;
	#else
		using b8 = s8;
	#endif
	using b32 = s32;

	// NOTE(bill): (std::)size_t is not used not because it's a bad concept but on
	// the platforms that I will be using:
	// sizeof(size_t) == sizeof(usize) == sizeof(s64)
	// NOTE(bill): This also allows for a signed version of size_t which is similar
	// to ptrdiff_t
	// NOTE(bill): If (u)intptr is a better fit, please use that.
	// NOTE(bill): Also, I hate the `_t` suffix
	#if defined(GB_ARCH_64_BIT)
		using ssize = s64;
		using usize = u64;
	#elif defined(GB_ARCH_32_BIT)
		using usize = s32;
		using usize = u32;
	#else
		#error Unknown architecture bit size
	#endif

	static_assert(sizeof(usize) == sizeof(size_t),
				  "`usize` is not the same size as `size_t`");
	static_assert(sizeof(ssize) == sizeof(usize),
				  "`ssize` is not the same size as `usize`");

	using intptr  = intptr_t;
	using uintptr = uintptr_t;

	using ptrdiff = ptrdiff_t;

#endif

#if !defined(GB_U8_MIN)
	#define GB_U8_MIN 0u
	#define GB_U8_MAX 0xffu
	#define GB_S8_MIN (-0x7f - 1)
	#define GB_S8_MAX 0x7f

	#define GB_U16_MIN 0u
	#define GB_U16_MAX 0xffffu
	#define GB_S16_MIN (-0x7fff - 1)
	#define GB_S16_MAX 0x7fff

	#define GB_U32_MIN 0u
	#define GB_U32_MAX 0xffffffffu
	#define GB_S32_MIN (-0x7fffffff - 1)
	#define GB_S32_MAX 0x7fffffff

	#define GB_U64_MIN 0ull
	#define GB_U64_MAX 0xffffffffffffffffull
	#define GB_S64_MIN (-0x7fffffffffffffffll - 1)
	#define GB_S64_MAX 0x7fffffffffffffffll
#endif

#if defined(GB_ARCH_64_BIT) && !defined(GB_USIZE_MIX)
	#define GB_USIZE_MIX GB_U64_MIN
	#define GB_USIZE_MAX GB_U64_MAX

	#define GB_SSIZE_MIX GB_S64_MIN
	#define GB_SSIZE_MAX GB_S64_MAX
#elif defined(GB_ARCH_32_BIT) && !defined(GB_USIZE_MIX)
	#define GB_USIZE_MIX GB_U32_MIN
	#define GB_USIZE_MAX GB_U32_MAX

	#define GB_SSIZE_MIX GB_S32_MIN
	#define GB_SSIZE_MAX GB_S32_MAX
#endif

#if defined(GB_BASIC_WITHOUT_NAMESPACE) && !defined(U8_MIN)
	#define U8_MIN 0u
	#define U8_MAX 0xffu
	#define S8_MIN (-0x7f - 1)
	#define S8_MAX 0x7f

	#define U16_MIN 0u
	#define U16_MAX 0xffffu
	#define S16_MIN (-0x7fff - 1)
	#define S16_MAX 0x7fff

	#define U32_MIN 0u
	#define U32_MAX 0xffffffffu
	#define S32_MIN (-0x7fffffff - 1)
	#define S32_MAX 0x7fffffff

	#define U64_MIN 0ull
	#define U64_MAX 0xffffffffffffffffull
	#define S64_MIN (-0x7fffffffffffffffll - 1)
	#define S64_MAX 0x7fffffffffffffffll

	#if defined(GB_ARCH_64_BIT) && !defined(GB_USIZE_MIX)
		#define USIZE_MIX U64_MIN
		#define USIZE_MAX U64_MAX

		#define SSIZE_MIX S64_MIN
		#define SSIZE_MAX S64_MAX
	#elif defined(GB_ARCH_32_BIT) && !defined(GB_USIZE_MIX)
		#define USIZE_MIX U32_MIN
		#define USIZE_MAX U32_MAX

		#define SSIZE_MIX S32_MIN
		#define SSIZE_MAX S32_MAX
	#endif
#endif



#if !defined(GB_BASIC_WITHOUT_NAMESPACE)
__GB_NAMESPACE_END
#endif // GB_BASIC_WITHOUT_NAMESPACE

__GB_NAMESPACE_START
#ifndef GB_SPECIAL_CASTS
#define GB_SPECIAL_CASTS
	// IMPORTANT NOTE(bill): Very similar to doing `*(T*)(&u)` but easier/clearer to write
	// however, it can be dangerous if sizeof(T) > sizeof(U) e.g. unintialized memory, undefined behavior
	// *(T*)(&u) ~~ pseudo_cast<T>(u)
	template <typename T, typename U>
	inline T
	pseudo_cast(const U& u)
	{
		return reinterpret_cast<const T&>(u);
	}

	// NOTE(bill): Very similar to doing `*(T*)(&u)`
	template <typename Dest, typename Source>
	inline Dest
	bit_cast(const Source& source)
	{
		static_assert(sizeof(Dest) <= sizeof(Source),
		              "bit_cast<Dest>(const Source&) - sizeof(Dest) <= sizeof(Source)");
		Dest dest;
		::memcpy(&dest, &source, sizeof(Dest));
		return dest;
	}
#endif
// FORENOTE(bill): There used to be a magic_cast that was equivalent to
// a C-style cast but I removed it as I could not get it work as intented
// for everything using only C++ style casts

#if !defined(GB_CASTS_WITHOUT_NAMESPACE)
__GB_NAMESPACE_END
#endif // GB_CASTS_WITHOUT_NAMESPACE

__GB_NAMESPACE_START
////////////////////////////////
///                          ///
/// Math Types               ///
///                          ///
////////////////////////////////

// TODO(bill): Should the math part be a separate library?

struct Vector2
{
	union
	{
		struct { f32 x, y; };
		f32 data[2];
	};

	inline const f32& operator[](usize index) const { return data[index]; }
	inline       f32& operator[](usize index)       { return data[index]; }
};

struct Vector3
{
	union
	{
		struct { f32 x, y, z; };
		struct { f32 r, g, b; };
		Vector2 xy;
		f32     data[3];
	};

	inline const f32& operator[](usize index) const { return data[index]; }
	inline       f32& operator[](usize index)       { return data[index]; }
};

struct Vector4
{
	union
	{
		struct { f32 x, y, z, w; };
		struct { f32 r, g, b, a; };
		struct { Vector2 xy, zw; };
		Vector3 xyz;
		Vector3 rgb;
		f32     data[4];
	};

	inline const f32& operator[](usize index) const { return data[index]; }
	inline       f32& operator[](usize index)       { return data[index]; }
};

struct Complex
{
	union
	{
		struct { f32 x, y; };
		struct { f32 real, imag; };
		f32 data[2];
	};

	inline const f32& operator[](usize index) const { return data[index]; }
	inline       f32& operator[](usize index)       { return data[index]; }
};

struct Quaternion
{
	union
	{
		struct { f32 x, y, z, w; };
		Vector3 xyz;
		f32     data[4];
	};

	inline const f32& operator[](usize index) const { return data[index]; }
	inline       f32& operator[](usize index)       { return data[index]; }
};

struct Matrix2
{
	union
	{
		struct { Vector2 x, y; };
		Vector2 columns[2];
		f32     data[4];
	};

	inline const Vector2& operator[](usize index) const { return columns[index]; }
	inline       Vector2& operator[](usize index)       { return columns[index]; }
};

struct Matrix3
{
	union
	{
		struct { Vector3 x, y, z; };
		Vector3 columns[3];
		f32     data[9];
	};

	inline const Vector3& operator[](usize index) const { return columns[index]; }
	inline       Vector3& operator[](usize index)       { return columns[index]; }
};

struct Matrix4
{
	union
	{
		struct { Vector4 x, y, z, w; };
		Vector4 columns[4];
		f32     data[16];
	};

	inline const Vector4& operator[](usize index) const { return columns[index]; }
	inline       Vector4& operator[](usize index)       { return columns[index]; }
};

struct Angle
{
	f32 radians;
};

struct Euler_Angles
{
	Angle pitch, yaw, roll;
};

struct Transform
{
	Vector3    position;
	Quaternion orientation;
	f32        scale;
	// NOTE(bill): Scale is only f32 to make sizeof(Transform) == 32 bytes
};

struct Aabb
{
	Vector3 center;
	Vector3 half_size;
};

struct Oobb
{
	Matrix4 transform;
	Aabb    aabb;
};

struct Sphere
{
	Vector3 center;
	f32     radius;
};

struct Plane
{
	Vector3 normal;
	f32     distance; // negative distance to origin
};


namespace angle
{
Angle radians(f32 radians);
Angle degrees(f32 degrees);

f32 as_radians(Angle angle);
f32 as_degrees(Angle angle);
} // namespace angle

////////////////////////////////
///                          ///
/// Math Type Op Overloads   ///
///                          ///
////////////////////////////////

// Vector2 Operators
bool operator==(const Vector2& a, const Vector2& b);
bool operator!=(const Vector2& a, const Vector2& b);

Vector2 operator-(const Vector2& a);

Vector2 operator+(const Vector2& a, const Vector2& b);
Vector2 operator-(const Vector2& a, const Vector2& b);

Vector2 operator*(const Vector2& a, f32 scalar);
Vector2 operator*(f32 scalar, const Vector2& a);

Vector2 operator/(const Vector2& a, f32 scalar);

Vector2 operator*(const Vector2& a, const Vector2& b); // Hadamard Product
Vector2 operator/(const Vector2& a, const Vector2& b); // Hadamard Product

Vector2& operator+=(Vector2& a, const Vector2& b);
Vector2& operator-=(Vector2& a, const Vector2& b);
Vector2& operator*=(Vector2& a, f32 scalar);
Vector2& operator/=(Vector2& a, f32 scalar);

// Vector3 Operators
bool operator==(const Vector3& a, const Vector3& b);
bool operator!=(const Vector3& a, const Vector3& b);

Vector3 operator-(const Vector3& a);

Vector3 operator+(const Vector3& a, const Vector3& b);
Vector3 operator-(const Vector3& a, const Vector3& b);

Vector3 operator*(const Vector3& a, f32 scalar);
Vector3 operator*(f32 scalar, const Vector3& a);

Vector3 operator/(const Vector3& a, f32 scalar);

Vector3 operator*(const Vector3& a, const Vector3& b); // Hadamard Product
Vector3 operator/(const Vector3& a, const Vector3& b); // Hadamard Product

Vector3& operator+=(Vector3& a, const Vector3& b);
Vector3& operator-=(Vector3& a, const Vector3& b);
Vector3& operator*=(Vector3& a, f32 scalar);
Vector3& operator/=(Vector3& a, f32 scalar);

// Vector4 Operators
bool operator==(const Vector4& a, const Vector4& b);
bool operator!=(const Vector4& a, const Vector4& b);

Vector4 operator-(const Vector4& a);

Vector4 operator+(const Vector4& a, const Vector4& b);
Vector4 operator-(const Vector4& a, const Vector4& b);

Vector4 operator*(const Vector4& a, f32 scalar);
Vector4 operator*(f32 scalar, const Vector4& a);

Vector4 operator/(const Vector4& a, f32 scalar);

Vector4 operator*(const Vector4& a, const Vector4& b); // Hadamard Product
Vector4 operator/(const Vector4& a, const Vector4& b); // Hadamard Product

Vector4& operator+=(Vector4& a, const Vector4& b);
Vector4& operator-=(Vector4& a, const Vector4& b);
Vector4& operator*=(Vector4& a, f32 scalar);
Vector4& operator/=(Vector4& a, f32 scalar);

// Complex Operators
bool operator==(const Complex& a, const Complex& b);
bool operator!=(const Complex& a, const Complex& b);

Complex operator-(const Complex& a);

Complex operator+(const Complex& a, const Complex& b);
Complex operator-(const Complex& a, const Complex& b);

Complex operator*(const Complex& a, const Complex& b);
Complex operator*(const Complex& a, f32 s);
Complex operator*(f32 s, const Complex& a);

Complex operator/(const Complex& a, f32 s);

// Quaternion Operators
bool operator==(const Quaternion& a, const Quaternion& b);
bool operator!=(const Quaternion& a, const Quaternion& b);

Quaternion operator-(const Quaternion& a);

Quaternion operator+(const Quaternion& a, const Quaternion& b);
Quaternion operator-(const Quaternion& a, const Quaternion& b);

Quaternion operator*(const Quaternion& a, const Quaternion& b);
Quaternion operator*(const Quaternion& a, f32 s);
Quaternion operator*(f32 s, const Quaternion& a);

Quaternion operator/(const Quaternion& a, f32 s);

Vector3 operator*(const Quaternion& a, const Vector3& v); // Rotate v by a

// Matrix2 Operators
bool operator==(const Matrix2& a, const Matrix2& b);
bool operator!=(const Matrix2& a, const Matrix2& b);

Matrix2 operator+(const Matrix2& a, const Matrix2& b);
Matrix2 operator-(const Matrix2& a, const Matrix2& b);

Matrix2 operator*(const Matrix2& a, const Matrix2& b);
Vector2 operator*(const Matrix2& a, const Vector2& v);
Matrix2 operator*(const Matrix2& a, f32 scalar);
Matrix2 operator*(f32 scalar, const Matrix2& a);

Matrix2 operator/(const Matrix2& a, f32 scalar);

Matrix2& operator+=(Matrix2& a, const Matrix2& b);
Matrix2& operator-=(Matrix2& a, const Matrix2& b);
Matrix2& operator*=(Matrix2& a, const Matrix2& b);

// Matrix3 Operators
bool operator==(const Matrix3& a, const Matrix3& b);
bool operator!=(const Matrix3& a, const Matrix3& b);

Matrix3 operator+(const Matrix3& a, const Matrix3& b);
Matrix3 operator-(const Matrix3& a, const Matrix3& b);

Matrix3 operator*(const Matrix3& a, const Matrix3& b);
Vector3 operator*(const Matrix3& a, const Vector3& v);
Matrix3 operator*(const Matrix3& a, f32 scalar);
Matrix3 operator*(f32 scalar, const Matrix3& a);

Matrix3 operator/(const Matrix3& a, f32 scalar);

Matrix3& operator+=(Matrix3& a, const Matrix3& b);
Matrix3& operator-=(Matrix3& a, const Matrix3& b);
Matrix3& operator*=(Matrix3& a, const Matrix3& b);

// Matrix4 Operators
bool operator==(const Matrix4& a, const Matrix4& b);
bool operator!=(const Matrix4& a, const Matrix4& b);

Matrix4 operator+(const Matrix4& a, const Matrix4& b);
Matrix4 operator-(const Matrix4& a, const Matrix4& b);

Matrix4 operator*(const Matrix4& a, const Matrix4& b);
Vector4 operator*(const Matrix4& a, const Vector4& v);
Matrix4 operator*(const Matrix4& a, f32 scalar);
Matrix4 operator*(f32 scalar, const Matrix4& a);

Matrix4 operator/(const Matrix4& a, f32 scalar);

Matrix4& operator+=(Matrix4& a, const Matrix4& b);
Matrix4& operator-=(Matrix4& a, const Matrix4& b);
Matrix4& operator*=(Matrix4& a, const Matrix4& b);

// Angle Operators
bool operator==(Angle a, Angle b);
bool operator!=(Angle a, Angle b);

Angle operator-(Angle a);

Angle operator+(Angle a, Angle b);
Angle operator-(Angle a, Angle b);

Angle operator*(Angle a, f32 scalar);
Angle operator*(f32 scalar, Angle a);

Angle operator/(Angle a, f32 scalar);

f32 operator/(Angle a, Angle b);

Angle& operator+=(Angle& a, Angle b);
Angle& operator-=(Angle& a, Angle b);
Angle& operator*=(Angle& a, f32 scalar);
Angle& operator/=(Angle& a, f32 scalar);

// Transform Operators
// World = Parent * Local
Transform operator*(const Transform& ps, const Transform& ls);
Transform& operator*=(Transform* ps, const Transform& ls);
// Local = World / Parent
Transform operator/(const Transform& ws, const Transform& ps);
Transform& operator/=(Transform& ws, const Transform& ps);

//////////////////////////////////
///                            ///
/// Math Functions & Constants ///
///                            ///
//////////////////////////////////
extern const Vector2      VECTOR2_ZERO;
extern const Vector3      VECTOR3_ZERO;
extern const Vector4      VECTOR4_ZERO;
extern const Complex      COMPLEX_ZERO;
extern const Quaternion   QUATERNION_IDENTITY;
extern const Matrix2      MATRIX2_IDENTITY;
extern const Matrix3      MATRIX3_IDENTITY;
extern const Matrix4      MATRIX4_IDENTITY;
extern const Euler_Angles EULER_ANGLES_ZERO;
extern const Transform    TRANSFORM_IDENTITY;

namespace math
{
extern const f32 ZERO;
extern const f32 ONE;
extern const f32 THIRD;
extern const f32 TWO_THIRDS;
extern const f32 E;
extern const f32 PI;
extern const f32 TAU;
extern const f32 SQRT_2;
extern const f32 SQRT_3;
extern const f32 SQRT_5;

extern const f32 F32_PRECISION;

// Power
f32 sqrt(f32 x);
f32 pow(f32 x, f32 y);
f32 cbrt(f32 x);
f32 fast_inv_sqrt(f32 x);

// Trigonometric
f32 sin(Angle a);
f32 cos(Angle a);
f32 tan(Angle a);

Angle arcsin(f32 x);
Angle arccos(f32 x);
Angle arctan(f32 x);
Angle arctan2(f32 y, f32 x);

// Hyperbolic
f32 sinh(f32 x);
f32 cosh(f32 x);
f32 tanh(f32 x);

f32 arsinh(f32 x);
f32 arcosh(f32 x);
f32 artanh(f32 x);

// Rounding
f32 ceil(f32 x);
f32 floor(f32 x);
f32 mod(f32 x, f32 y);
f32 truncate(f32 x);
f32 round(f32 x);

s32 sign(s32 x);
s64 sign(s64 x);
f32 sign(f32 x);

// Other
f32 abs(f32 x);
s8  abs( s8 x);
s16 abs(s16 x);
s32 abs(s32 x);
s64 abs(s64 x);

bool is_infinite(f32 x);
bool is_nan(f32 x);

#undef min
#undef max
s32 min(s32 a, s32 b);
s64 min(s64 a, s64 b);
f32 min(f32 a, f32 b);

s32 max(s32 a, s32 b);
s64 max(s64 a, s64 b);
f32 max(f32 a, f32 b);


s32 clamp(s32 x, s32 min, s32 max);
s64 clamp(s64 x, s64 min, s64 max);
f32 clamp(f32 x, f32 min, f32 max);

template <typename T>
T lerp(const T& x, const T& y, f32 t);

bool equals(f32 a, f32 b, f32 precision = F32_PRECISION);

template <typename T>
void swap(T* a, T* b);

template <typename T, usize N>
void swap(T (& a)[N], T (& b)[N]);

// Vector2 functions
f32 dot(const Vector2& a, const Vector2& b);
f32 cross(const Vector2& a, const Vector2& b);

f32 magnitude(const Vector2& a);
Vector2 normalize(const Vector2& a);

Vector2 hadamard(const Vector2& a, const Vector2& b);

f32 aspect_ratio(const Vector2& a);

// Vector3 functions
f32 dot(const Vector3& a, const Vector3& b);
Vector3 cross(const Vector3& a, const Vector3& b);

f32 magnitude(const Vector3& a);
Vector3 normalize(const Vector3& a);

Vector3 hadamard(const Vector3& a, const Vector3& b);

// Vector4 functions
f32 dot(const Vector4& a, const Vector4& b);

f32 magnitude(const Vector4& a);
Vector4 normalize(const Vector4& a);

Vector4 hadamard(const Vector4& a, const Vector4& b);

// Complex functions
f32 dot(const Complex& a, const Complex& b);

f32 magnitude(const Complex& a);
f32 norm(const Complex& a);
Complex normalize(const Complex& a);

Complex conjugate(const Complex& a);
Complex inverse(const Complex& a);

f32 complex_angle(const Complex& a);
inline f32 complex_argument(const Complex& a) { return complex_angle(a); }
Complex magnitude_angle(f32 magnitude, Angle a);
inline Complex complex_polar(f32 magnitude, Angle a) { return magnitude_angle(magnitude, a); }

// Quaternion functions
f32 dot(const Quaternion& a, const Quaternion& b);
Quaternion cross(const Quaternion& a, const Quaternion& b);

f32 magnitude(const Quaternion& a);
f32 norm(const Quaternion& a);
Quaternion normalize(const Quaternion& a);

Quaternion conjugate(const Quaternion& a);
Quaternion inverse(const Quaternion& a);

Angle quaternion_angle(const Quaternion& a);
Vector3 quaternion_axis(const Quaternion& a);
Quaternion axis_angle(const Vector3& axis, Angle a);

Angle quaternion_roll(const Quaternion& a);
Angle quaternion_pitch(const Quaternion& a);
Angle quaternion_yaw(const Quaternion& a);

Euler_Angles quaternion_to_euler_angles(const Quaternion& a);
Quaternion euler_angles_to_quaternion(const Euler_Angles& e,
									  const Vector3& x_axis = {1, 0, 0},
									  const Vector3& y_axis = {0, 1, 0},
									  const Vector3& z_axis = {0, 0, 1});

// Spherical Linear Interpolation
Quaternion slerp(const Quaternion& x, const Quaternion& y, f32 t);

// Shoemake's Quaternion Curves
// Sqherical Cubic Interpolation
Quaternion squad(const Quaternion& p,
				 const Quaternion& a,
				 const Quaternion& b,
				 const Quaternion& q,
				 f32 t);
// Matrix2 functions
Matrix2 transpose(const Matrix2& m);
f32 determinant(const Matrix2& m);
Matrix2 inverse(const Matrix2& m);
Matrix2 hadamard(const Matrix2& a, const Matrix2&b);
Matrix4 matrix2_to_matrix4(const Matrix2& m);

// Matrix3 functions
Matrix3 transpose(const Matrix3& m);
f32 determinant(const Matrix3& m);
Matrix3 inverse(const Matrix3& m);
Matrix3 hadamard(const Matrix3& a, const Matrix3&b);
Matrix4 matrix3_to_matrix4(const Matrix3& m);

// Matrix4 functions
Matrix4 transpose(const Matrix4& m);
f32 determinant(const Matrix4& m);
Matrix4 inverse(const Matrix4& m);
Matrix4 hadamard(const Matrix4& a, const Matrix4&b);
bool is_affine(const Matrix4& m);

Matrix4 quaternion_to_matrix4(const Quaternion& a);
Quaternion matrix4_to_quaternion(const Matrix4& m);

Matrix4 translate(const Vector3& v);
Matrix4 rotate(const Vector3& v, Angle angle);
Matrix4 scale(const Vector3& v);
Matrix4 ortho(f32 left, f32 right, f32 bottom, f32 top);
Matrix4 ortho(f32 left, f32 right, f32 bottom, f32 top, f32 z_near, f32 z_far);
Matrix4 perspective(Angle fovy, f32 aspect, f32 z_near, f32 z_far);
Matrix4 infinite_perspective(Angle fovy, f32 aspect, f32 z_near);

Matrix4
look_at_matrix4(const Vector3& eye, const Vector3& center, const Vector3& up = {0, 1, 0});

Quaternion
look_at_quaternion(const Vector3& eye, const Vector3& center, const Vector3& up = {0, 1, 0});

// Transform Functions
Vector3 transform_point(const Transform& transform, const Vector3& point);
Transform inverse(const Transform& t);
Matrix4 transform_to_matrix4(const Transform& t);
} // namespace math

namespace aabb
{
Aabb calculate(const void* vertices, usize num_vertices, usize stride, usize offset);

f32 surface_area(const Aabb& aabb);
f32 volume(const Aabb& aabb);

Sphere to_sphere(const Aabb& aabb);

bool contains(const Aabb& aabb, const Vector3& point);
bool contains(const Aabb& a, const Aabb& b);
bool intersects(const Aabb& a, const Aabb& b);

Aabb transform_affine(const Aabb& aabb, const Matrix4& m);
} // namespace aabb

namespace sphere
{
Sphere calculate_min_bounding_sphere(const void* vertices, usize num_vertices, usize stride, usize offset, f32 step);
Sphere calculate_max_bounding_sphere(const void* vertices, usize num_vertices, usize stride, usize offset);

f32 surface_area(const Sphere& s);
f32 volume(const Sphere& s);

Aabb to_aabb(const Sphere& sphere);

bool contains_point(const Sphere& s, const Vector3& point);

f32 ray_intersection(const Vector3& from, const Vector3& dir, const Sphere& s);
} // namespace sphere

namespace plane
{
f32 ray_intersection(const Vector3& from, const Vector3& dir, const Plane& p);

bool intersection3(const Plane& p1, const Plane& p2, const Plane& p3, Vector3* ip);
} // namespace plane



namespace random
{
struct Random // NOTE(bill): Mt19937_64
{
	s64 seed;
	u32 index;
	s64 mt[312];
};

Random make(s64 seed);

void set_seed(Random* r, s64 seed);

s64 next(Random* r);

void next_from_device(void* buffer, u32 length_in_bytes);

s32 next_s32(Random* r);
u32 next_u32(Random* r);
f32 next_f32(Random* r);
s64 next_s64(Random* r);
u64 next_u64(Random* r);
f64 next_f64(Random* r);

s32 uniform_s32(Random* r, s32 min_inc, s32 max_inc);
u32 uniform_u32(Random* r, u32 min_inc, u32 max_inc);
f32 uniform_f32(Random* r, f32 min_inc, f32 max_inc);
s64 uniform_s64(Random* r, s64 min_inc, s64 max_inc);
u64 uniform_u64(Random* r, u64 min_inc, u64 max_inc);
f64 uniform_f64(Random* r, f64 min_inc, f64 max_inc);

f32 perlin3(f32 x, f32 y, f32 z, s32 x_wrap = 0, s32 y_wrap = 0, s32 z_wrap = 0);

} // namespace random
__GB_NAMESPACE_END

#endif // GB_INCLUDE_GB_HPP

///
///
///
///
///
///
///
///
///
///
///
///
///
///
///
///
///
///
///
///
///
///
///
///
///
///
///
///
///
///
///
///
///
///
///
///
///
///
///
///
///
///
///
///
///
///
///
///
///
///
///
///
///
///
///
///
/// So long and thanks for all the fish!
///
///
///
///
///
////////////////////////////////
///                          ///
/// Implemenation            ///
///                          ///
////////////////////////////////
#if defined(GB_MATH_IMPLEMENTATION)
__GB_NAMESPACE_START

////////////////////////////////
///                          ///
/// Math                     ///
///                          ///
////////////////////////////////

const Vector2      VECTOR2_ZERO        = Vector2{0, 0};
const Vector3      VECTOR3_ZERO        = Vector3{0, 0, 0};
const Vector4      VECTOR4_ZERO        = Vector4{0, 0, 0, 0};
const Complex      COMPLEX_ZERO        = Complex{0, 0};
const Quaternion   QUATERNION_IDENTITY = Quaternion{0, 0, 0, 1};
const Matrix2      MATRIX2_IDENTITY    = Matrix2{1, 0,
										         0, 1};
const Matrix3      MATRIX3_IDENTITY    = Matrix3{1, 0, 0,
										         0, 1, 0,
										         0, 0, 1};
const Matrix4      MATRIX4_IDENTITY    = Matrix4{1, 0, 0, 0,
										         0, 1, 0, 0,
										         0, 0, 1, 0,
										         0, 0, 0, 1};
const Euler_Angles EULER_ANGLES_ZERO   = Euler_Angles{0, 0, 0};
const Transform    TRANSFORM_IDENTITY  = Transform{VECTOR3_ZERO, QUATERNION_IDENTITY, 1};

////////////////////////////////
/// Math Type Op Overloads   ///
////////////////////////////////

// Vector2 Operators
bool
operator==(const Vector2& a, const Vector2& b)
{
	return (a.x == b.x) && (a.y == b.y);
}

bool
operator!=(const Vector2& a, const Vector2& b)
{
	return !operator==(a, b);
}

Vector2
operator-(const Vector2& a)
{
	return {-a.x, -a.y};
}

Vector2
operator+(const Vector2& a, const Vector2& b)
{
	return {a.x + b.x, a.y + b.y};
}

Vector2
operator-(const Vector2& a, const Vector2& b)
{
	return {a.x - b.x, a.y - b.y};
}

Vector2
operator*(const Vector2& a, f32 scalar)
{
	return {a.x * scalar, a.y * scalar};
}

Vector2
operator*(f32 scalar, const Vector2& a)
{
	return {a.x * scalar, a.y * scalar};
}

Vector2
operator/(const Vector2& a, f32 scalar)
{
	return {a.x / scalar, a.y / scalar};
}

Vector2
operator*(const Vector2& a, const Vector2& b) // Hadamard Product
{
	return {a.x * b.x, a.y * b.y};
}

Vector2
operator/(const Vector2& a, const Vector2& b) // Hadamard Product
{
	return {a.x / b.x, a.y / b.y};
}

Vector2&
operator+=(Vector2& a, const Vector2& b)
{
	a.x += b.x;
	a.y += b.y;

	return a;
}

Vector2&
operator-=(Vector2& a, const Vector2& b)
{
	a.x -= b.x;
	a.y -= b.y;

	return a;
}

Vector2&
operator*=(Vector2& a, f32 scalar)
{
	a.x *= scalar;
	a.y *= scalar;

	return a;
}

Vector2&
operator/=(Vector2& a, f32 scalar)
{
	a.x /= scalar;
	a.y /= scalar;

	return a;
}

// Vector3 Operators
bool
operator==(const Vector3& a, const Vector3& b)
{
	return (a.x == b.x) && (a.y == b.y) && (a.z == b.z);
}

bool
operator!=(const Vector3& a, const Vector3& b)
{
	return !operator==(a, b);
}

Vector3
operator-(const Vector3& a)
{
	return {-a.x, -a.y, -a.z};
}

Vector3
operator+(const Vector3& a, const Vector3& b)
{
	return {a.x + b.x, a.y + b.y, a.z + b.z};
}

Vector3
operator-(const Vector3& a, const Vector3& b)
{
	return {a.x - b.x, a.y - b.y, a.z - b.z};
}

Vector3
operator*(const Vector3& a, f32 scalar)
{
	return {a.x * scalar, a.y * scalar, a.z * scalar};
}

Vector3
operator*(f32 scalar, const Vector3& a)
{
	return {a.x * scalar, a.y * scalar, a.z * scalar};
}

Vector3
operator/(const Vector3& a, f32 scalar)
{
	return {a.x / scalar, a.y / scalar, a.z / scalar};
}

Vector3
operator*(const Vector3& a, const Vector3& b) // Hadamard Product
{
	return {a.x * b.x, a.y * b.y, a.z * b.z};
}

Vector3
operator/(const Vector3& a, const Vector3& b) // Hadamard Product
{
	return {a.x / b.x, a.y / b.y, a.z / b.z};
}

Vector3&
operator+=(Vector3& a, const Vector3& b)
{
	a.x += b.x;
	a.y += b.y;
	a.z += b.z;

	return a;
}

Vector3&
operator-=(Vector3& a, const Vector3& b)
{
	a.x -= b.x;
	a.y -= b.y;
	a.z -= b.z;

	return a;
}

Vector3&
operator*=(Vector3& a, f32 scalar)
{
	a.x *= scalar;
	a.y *= scalar;
	a.z *= scalar;

	return a;
}

Vector3&
operator/=(Vector3& a, f32 scalar)
{
	a.x /= scalar;
	a.y /= scalar;
	a.z /= scalar;

	return a;
}

// Vector4 Operators
bool
operator==(const Vector4& a, const Vector4& b)
{
	return (a.x == b.x) && (a.y == b.y) && (a.z == b.z) && (a.w == b.w);
}

bool
operator!=(const Vector4& a, const Vector4& b)
{
	return !operator==(a, b);
}

Vector4
operator-(const Vector4& a)
{
	return {-a.x, -a.y, -a.z, -a.w};
}

Vector4
operator+(const Vector4& a, const Vector4& b)
{
	return {a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w};
}

Vector4
operator-(const Vector4& a, const Vector4& b)
{
	return {a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w};
}

Vector4
operator*(const Vector4& a, f32 scalar)
{
	return {a.x * scalar, a.y * scalar, a.z * scalar, a.w * scalar};
}

Vector4
operator*(f32 scalar, const Vector4& a)
{
	return {a.x * scalar, a.y * scalar, a.z * scalar, a.w * scalar};
}

Vector4
operator/(const Vector4& a, f32 scalar)
{
	return {a.x / scalar, a.y / scalar, a.z / scalar, a.w / scalar};
}

Vector4
operator*(const Vector4& a, const Vector4& b) // Hadamard Product
{
	return {a.x * b.x, a.y * b.y, a.z * b.z, a.w * b.w};
}

Vector4
operator/(const Vector4& a, const Vector4& b) // Hadamard Product
{
	return {a.x / b.x, a.y / b.y, a.z / b.z, a.w / b.w};
}

Vector4&
operator+=(Vector4& a, const Vector4& b)
{
	a.x += b.x;
	a.y += b.y;
	a.z += b.z;
	a.w += b.w;

	return a;
}

Vector4&
operator-=(Vector4& a, const Vector4& b)
{
	a.x -= b.x;
	a.y -= b.y;
	a.z -= b.z;
	a.w -= b.w;

	return a;
}

Vector4&
operator*=(Vector4& a, f32 scalar)
{
	a.x *= scalar;
	a.y *= scalar;
	a.z *= scalar;
	a.w *= scalar;

	return a;
}

Vector4&
operator/=(Vector4& a, f32 scalar)
{
	a.x /= scalar;
	a.y /= scalar;
	a.z /= scalar;
	a.w /= scalar;

	return a;
}

// Complex Operators
bool
operator==(const Complex& a, const Complex& b)
{
	return (a.x == b.x) && (a.y == b.y);
}

bool
operator!=(const Complex& a, const Complex& b)
{
	return
	operator==(a, b);
}

Complex
operator-(const Complex& a)
{
	return {-a.x, -a.y};
}

Complex
operator+(const Complex& a, const Complex& b)
{
	return {a.x + b.x, a.y + b.y};
}

Complex
operator-(const Complex& a, const Complex& b)
{
	return {a.x - b.x, a.y - b.y};

}

Complex
operator*(const Complex& a, const Complex& b)
{
	Complex c = {};

	c.x = a.x * b.x - a.y * b.y;
	c.y = a.y * b.x - a.y * b.x;

	return c;
}

Complex
operator*(const Complex& a, f32 s)
{
	return {a.x * s, a.y * s};
}

Complex
operator*(f32 s, const Complex& a)
{
	return {a.x * s, a.y * s};
}

Complex
operator/(const Complex& a, f32 s)
{
	return {a.x / s, a.y / s};
}

// Quaternion Operators
bool
operator==(const Quaternion& a, const Quaternion& b)
{
	return (a.x == b.x) && (a.y == b.y) && (a.z == b.z) && (a.w == b.w);
}

bool
operator!=(const Quaternion& a, const Quaternion& b)
{
	return !operator==(a, b);
}

Quaternion
operator-(const Quaternion& a)
{
	return {-a.x, -a.y, -a.z, -a.w};
}

Quaternion
operator+(const Quaternion& a, const Quaternion& b)
{
	return {a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w};
}

Quaternion
operator-(const Quaternion& a, const Quaternion& b)
{
	return {a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w};

}

Quaternion
operator*(const Quaternion& a, const Quaternion& b)
{
	Quaternion q = {};

	q.x = a.w * b.x + a.x * b.w + a.y * b.z - a.z * b.y;
	q.y = a.w * b.y - a.x * b.z + a.y * b.w + a.z * b.x;
	q.z = a.w * b.z + a.x * b.y - a.y * b.x + a.z * b.w;
	q.w = a.w * b.w - a.x * b.x - a.y * b.y - a.z * b.z;

	return q;
}

Quaternion
operator*(const Quaternion& a, f32 s)
{
	return {a.x * s, a.y * s, a.z * s, a.w * s};
}

Quaternion
operator*(f32 s, const Quaternion& a)
{
	return {a.x * s, a.y * s, a.z * s, a.w * s};
}

Quaternion
operator/(const Quaternion& a, f32 s)
{
	return {a.x / s, a.y / s, a.z / s, a.w / s};
}

Vector3
operator*(const Quaternion& a, const Vector3& v) // Rotate v by q
{
	// return (q * Quaternion{v.x, v.y, v.z, 0} * math::conjugate(q)).xyz; // More Expensive
	const Vector3 t = 2.0f * math::cross(a.xyz, v);
	return (v + a.w * t + math::cross(a.xyz, t));
}

// Matrix2 Operators
bool
operator==(const Matrix2& a, const Matrix2& b)
{
	for (usize i = 0; i < 4; i++)
	{
		if (a[i] != b[i])
			return false;
	}
	return true;
}

bool
operator!=(const Matrix2& a, const Matrix2& b)
{
	return !operator==(a, b);
}

Matrix2
operator+(const Matrix2& a, const Matrix2& b)
{
	Matrix2 mat;
	mat[0] = a[0] + b[0];
	mat[1] = a[1] + b[1];
	return mat;
}

Matrix2
operator-(const Matrix2& a, const Matrix2& b)
{
	Matrix2 mat;
	mat[0] = a[0] - b[0];
	mat[1] = a[1] - b[1];
	return mat;
}

Matrix2
operator*(const Matrix2& a, const Matrix2& b)
{
	Matrix2 result;
	result[0] = a[0] * b[0][0] + a[1] * b[0][1];
	result[1] = a[0] * b[1][0] + a[1] * b[1][1];
	return result;
}

Vector2
operator*(const Matrix2& a, const Vector2& v)
{
	return Vector2{a[0][0] * v.x + a[1][0] * v.y,
				   a[0][1] * v.x + a[1][1] * v.y};
}

Matrix2
operator*(const Matrix2& a, f32 scalar)
{
	Matrix2 mat;
	mat[0] = a[0] * scalar;
	mat[1] = a[1] * scalar;
	return mat;
}

Matrix2
operator*(f32 scalar, const Matrix2& a)
{
	Matrix2 mat;
	mat[0] = a[0] * scalar;
	mat[1] = a[1] * scalar;
	return mat;
}

Matrix2
operator/(const Matrix2& a, f32 scalar)
{
	Matrix2 mat;
	mat[0] = a[0] / scalar;
	mat[1] = a[1] / scalar;
	return mat;
}

Matrix2&
operator+=(Matrix2& a, const Matrix2& b)
{
	return (a = a + b);
}

Matrix2&
operator-=(Matrix2& a, const Matrix2& b)
{
	return (a = a - b);
}

Matrix2&
operator*=(Matrix2& a, const Matrix2& b)
{
	return (a = a * b);
}


// Matrix3 Operators
bool
operator==(const Matrix3& a, const Matrix3& b)
{
	for (usize i = 0; i < 3; i++)
	{
		if (a[i] != b[i])
			return false;
	}
	return true;
}

bool
operator!=(const Matrix3& a, const Matrix3& b)
{
	return !operator==(a, b);
}

Matrix3
operator+(const Matrix3& a, const Matrix3& b)
{
	Matrix3 mat;
	mat[0] = a[0] + b[0];
	mat[1] = a[1] + b[1];
	mat[2] = a[2] + b[2];
	return mat;
}

Matrix3
operator-(const Matrix3& a, const Matrix3& b)
{
	Matrix3 mat;
	mat[0] = a[0] - b[0];
	mat[1] = a[1] - b[1];
	mat[2] = a[2] - b[2];
	return mat;
}

Matrix3
operator*(const Matrix3& a, const Matrix3& b)
{
	Matrix3 result;
	result[0] = a[0] * b[0][0] + a[1] * b[0][1] + a[2] * b[0][2];
	result[1] = a[0] * b[1][0] + a[1] * b[1][1] + a[2] * b[1][2];
	result[2] = a[0] * b[2][0] + a[1] * b[2][1] + a[2] * b[2][2];
	return result;
}

Vector3
operator*(const Matrix3& a, const Vector3& v)
{
	return Vector3{a[0][0] * v.x + a[1][0] * v.y + a[2][0] * v.z,
				   a[0][1] * v.x + a[1][1] * v.y + a[2][1] * v.z,
				   a[0][2] * v.x + a[1][2] * v.y + a[2][2] * v.z};
}

Matrix3
operator*(const Matrix3& a, f32 scalar)
{
	Matrix3 mat;
	mat[0] = a[0] * scalar;
	mat[1] = a[1] * scalar;
	mat[2] = a[2] * scalar;
	return mat;
}

Matrix3
operator*(f32 scalar, const Matrix3& a)
{
	Matrix3 mat;
	mat[0] = a[0] * scalar;
	mat[1] = a[1] * scalar;
	mat[2] = a[2] * scalar;
	return mat;
}

Matrix3
operator/(const Matrix3& a, f32 scalar)
{
	Matrix3 mat;
	mat[0] = a[0] / scalar;
	mat[1] = a[1] / scalar;
	mat[2] = a[2] / scalar;
	return mat;
}

Matrix3&
operator+=(Matrix3& a, const Matrix3& b)
{
	return (a = a + b);
}

Matrix3&
operator-=(Matrix3& a, const Matrix3& b)
{
	return (a = a - b);
}

Matrix3&
operator*=(Matrix3& a, const Matrix3& b)
{
	return (a = a * b);
}




// Matrix4 Operators
bool
operator==(const Matrix4& a, const Matrix4& b)
{
	for (usize i = 0; i < 4; i++)
	{
		if (a[i] != b[i])
			return false;
	}
	return true;
}

bool
operator!=(const Matrix4& a, const Matrix4& b)
{
	return !operator==(a, b);
}

Matrix4
operator+(const Matrix4& a, const Matrix4& b)
{
	Matrix4 mat;
	mat[0] = a[0] + b[0];
	mat[1] = a[1] + b[1];
	mat[2] = a[2] + b[2];
	mat[3] = a[3] + b[3];
	return mat;
}

Matrix4
operator-(const Matrix4& a, const Matrix4& b)
{
	Matrix4 mat;
	mat[0] = a[0] - b[0];
	mat[1] = a[1] - b[1];
	mat[2] = a[2] - b[2];
	mat[3] = a[3] - b[3];
	return mat;
}

Matrix4
operator*(const Matrix4& a, const Matrix4& b)
{
	Matrix4 result;
	result[0] = a[0] * b[0][0] + a[1] * b[0][1] + a[2] * b[0][2] + a[3] * b[0][3];
	result[1] = a[0] * b[1][0] + a[1] * b[1][1] + a[2] * b[1][2] + a[3] * b[1][3];
	result[2] = a[0] * b[2][0] + a[1] * b[2][1] + a[2] * b[2][2] + a[3] * b[2][3];
	result[3] = a[0] * b[3][0] + a[1] * b[3][1] + a[2] * b[3][2] + a[3] * b[3][3];
	return result;
}

Vector4
operator*(const Matrix4& a, const Vector4& v)
{
	return Vector4{a[0][0] * v.x + a[1][0] * v.y + a[2][0] * v.z + a[3][0] * v.w,
				   a[0][1] * v.x + a[1][1] * v.y + a[2][1] * v.z + a[3][1] * v.w,
				   a[0][2] * v.x + a[1][2] * v.y + a[2][2] * v.z + a[3][2] * v.w,
				   a[0][3] * v.x + a[1][3] * v.y + a[2][3] * v.z + a[3][3] * v.w};
}

Matrix4
operator*(const Matrix4& a, f32 scalar)
{
	Matrix4 mat;
	mat[0] = a[0] * scalar;
	mat[1] = a[1] * scalar;
	mat[2] = a[2] * scalar;
	mat[3] = a[3] * scalar;
	return mat;
}

Matrix4
operator*(f32 scalar, const Matrix4& a)
{
	Matrix4 mat;
	mat[0] = a[0] * scalar;
	mat[1] = a[1] * scalar;
	mat[2] = a[2] * scalar;
	mat[3] = a[3] * scalar;
	return mat;
}

Matrix4
operator/(const Matrix4& a, f32 scalar)
{
	Matrix4 mat;
	mat[0] = a[0] / scalar;
	mat[1] = a[1] / scalar;
	mat[2] = a[2] / scalar;
	mat[3] = a[3] / scalar;
	return mat;
}

Matrix4&
operator+=(Matrix4& a, const Matrix4& b)
{
	return (a = a + b);
}

Matrix4&
operator-=(Matrix4& a, const Matrix4& b)
{
	return (a = a - b);
}

Matrix4&
operator*=(Matrix4& a, const Matrix4& b)
{
	return (a = a * b);
}

// Angle Operators
bool
operator==(Angle a, Angle b)
{
	return a.radians == b.radians;
}

bool
operator!=(Angle a, Angle b)
{
	return !operator==(a, b);
}

Angle
operator-(Angle a)
{
	return {-a.radians};
}

Angle
operator+(Angle a, Angle b)
{
	return {a.radians + b.radians};
}

Angle
operator-(Angle a, Angle b)
{
	return {a.radians - b.radians};
}

Angle
operator*(Angle a, f32 scalar)
{
	return {a.radians * scalar};
}

Angle
operator*(f32 scalar, Angle a)
{
	return {a.radians * scalar};
}

Angle
operator/(Angle a, f32 scalar)
{
	return {a.radians / scalar};
}

f32
operator/(Angle a, Angle b)
{
	return a.radians / b.radians;
}

Angle&
operator+=(Angle& a, Angle b)
{
	return (a = a + b);
}

Angle&
operator-=(Angle& a, Angle b)
{
	return (a = a - b);
}

Angle&
operator*=(Angle& a, f32 scalar)
{
	return (a = a * scalar);
}

Angle&
operator/=(Angle& a, f32 scalar)
{
	return (a = a / scalar);
}


// Transform Operators
// World = Parent * Local
Transform
operator*(const Transform& ps, const Transform& ls)
{
	Transform ws;

	ws.position    = ps.position + ps.orientation * (ps.scale * ls.position);
	ws.orientation = ps.orientation * ls.orientation;
	// ws.scale       = ps.scale * (ps.orientation * ls.scale); // Vector3 scale
	ws.scale       = ps.scale * ls.scale;

	return ws;
}

Transform&
operator*=(Transform& ps, const Transform& ls)
{
	return (ps = ps * ls);
}

// Local = World / Parent
Transform
operator/(const Transform& ws, const Transform& ps)
{
	Transform ls;

	const Quaternion ps_conjugate = math::conjugate(ps.orientation);

	ls.position    = (ps_conjugate * (ws.position - ps.position)) / ps.scale;
	ls.orientation = ps_conjugate * ws.orientation;
	// ls.scale       = ps_conjugate * (ws.scale / ps.scale); // Vector3 scale
	ls.scale       = ws.scale / ps.scale;

	return ls;
}

Transform&
operator/=(Transform& ws, const Transform& ps)
{
	return (ws = ws / ps);
}


namespace angle
{
inline Angle
radians(f32 r)
{
	return {r};
}

inline Angle
degrees(f32 d)
{
	return {d * math::TAU / 360.0f};
}

inline f32
as_radians(Angle angle)
{
	return angle.radians;
}

inline f32
as_degrees(Angle angle)
{
	return angle.radians * 360.0f / math::TAU;
}
} // namespace angle

////////////////////////////////
///                          ///
/// Math Functions           ///
///                          ///
////////////////////////////////


namespace math
{
const f32 ZERO       = 0.0f;
const f32 ONE        = 1.0f;
const f32 THIRD      = 0.33333333f;
const f32 TWO_THIRDS = 0.66666667f;
const f32 E          = 2.718281828f;
const f32 PI         = 3.141592654f;
const f32 TAU        = 6.283185307f;
const f32 SQRT_2     = 1.414213562f;
const f32 SQRT_3     = 1.732050808f;
const f32 SQRT_5     = 2.236067978f;

const f32 F32_PRECISION = 1.0e-7f;

// Power
inline f32 sqrt(f32 x)       { return ::sqrtf(x);        }
inline f32 pow(f32 x, f32 y) { return static_cast<f32>(::powf(x, y)); }
inline f32 cbrt(f32 x)       { return static_cast<f32>(::cbrtf(x));   }

inline f32
fast_inv_sqrt(f32 x)
{
	const f32 THREE_HALFS = 1.5f;

	const f32 x2 = x * 0.5f;
	f32 y  = x;
	u32 i  = bit_cast<u32>(y);             // Evil floating point bit level hacking
	//	i = 0x5f3759df - (i >> 1);            // What the fuck? Old
	i = 0x5f375a86 - (i >> 1);                // What the fuck? Improved!
	y = bit_cast<f32>(i);
	y = y * (THREE_HALFS - (x2 * y * y));     // 1st iteration
	//	y = y * (THREE_HALFS - (x2 * y * y)); // 2nd iteration, this can be removed

	return y;
}

// Trigonometric
inline f32 sin(Angle a) { return ::sinf(angle::as_radians(a)); }
inline f32 cos(Angle a) { return ::cosf(angle::as_radians(a)); }
inline f32 tan(Angle a) { return ::tanf(angle::as_radians(a)); }

inline Angle arcsin(f32 x)         { return angle::radians(::asinf(x));     }
inline Angle arccos(f32 x)         { return angle::radians(::acosf(x));     }
inline Angle arctan(f32 x)         { return angle::radians(::atanf(x));     }
inline Angle arctan2(f32 y, f32 x) { return angle::radians(::atan2f(y, x)); }

// Hyperbolic
inline f32 sinh(f32 x) { return ::sinhf(x); }
inline f32 cosh(f32 x) { return ::coshf(x); }
inline f32 tanh(f32 x) { return ::tanhf(x); }

inline f32 arsinh(f32 x) { return ::asinhf(x); }
inline f32 arcosh(f32 x) { return ::acoshf(x); }
inline f32 artanh(f32 x) { return ::atanhf(x); }

// Rounding
inline f32 ceil(f32 x)       { return ::ceilf(x);    }
inline f32 floor(f32 x)      { return ::floorf(x);   }
inline f32 mod(f32 x, f32 y) { return ::fmodf(x, y); }
inline f32 truncate(f32 x)   { return ::truncf(x);   }
inline f32 round(f32 x)      { return ::roundf(x);   }

inline s32 sign(s32 x) { return x >= 0 ? +1 : -1; }
inline s64 sign(s64 x) { return x >= 0 ? +1 : -1; }
inline f32 sign(f32 x) { return x >= 0.0f ? +1.0f : -1.0f; }

// Other
inline f32
abs(f32 x)
{
	u32 i = bit_cast<u32>(x);
	i &= 0x7FFFFFFFul;
	return bit_cast<f32>(i);
}

inline s8
abs(s8 x)
{
	u8 i = bit_cast<u8>(x);
	i &= 0x7Fu;
	return bit_cast<s8>(i);
}

inline s16
abs(s16 x)
{
	u16 i = bit_cast<u16>(x);
	i &= 0x7FFFu;
	return bit_cast<s16>(i);
}

inline s32
abs(s32 x)
{
	u32 i = bit_cast<u32>(x);
	i &= 0x7FFFFFFFul;
	return bit_cast<s32>(i);
}

inline s64
abs(s64 x)
{
	u64 i = bit_cast<u64>(x);
	i &= 0x7FFFFFFFFFFFFFFFull;
	return bit_cast<s64>(i);
}

inline bool
is_infinite(f32 x)
{
	return isinf(x);
}

inline bool
is_nan(f32 x)
{
	return isnan(x);
}

inline s32 min(s32 a, s32 b) { return a < b ? a : b; }
inline s64 min(s64 a, s64 b) { return a < b ? a : b; }
inline f32 min(f32 a, f32 b) { return a < b ? a : b; }

inline s32 max(s32 a, s32 b) { return a > b ? a : b; }
inline s64 max(s64 a, s64 b) { return a > b ? a : b; }
inline f32 max(f32 a, f32 b) { return a > b ? a : b; }

inline s32
clamp(s32 x, s32 min, s32 max)
{
	if (x < min)
		return min;
	if (x > max)
		return max;
	return x;
}

inline s64
clamp(s64 x, s64 min, s64 max)
{
	if (x < min)
		return min;
	if (x > max)
		return max;
	return x;
}

inline f32
clamp(f32 x, f32 min, f32 max)
{
	if (x < min)
		return min;
	if (x > max)
		return max;
	return x;
}

template <typename T>
inline T
lerp(const T& x, const T& y, f32 t)
{
	return x + (y - x) * t;
}

inline bool
equals(f32 a, f32 b, f32 precision)
{
	return ((b <= (a + precision)) && (b >= (a - precision)));
}

template <typename T>
inline void
swap(T* a, T* b)
{
	T c = gb::move(*a);
	*a  = gb::move(*b);
	*b  = gb::move(c);
}

template <typename T, usize N>
inline void
swap(T (& a)[N], T (& b)[N])
{
	for (usize i = 0; i < N; i++)
		math::swap(&a[i], &b[i]);
}

// Vector2 functions
inline f32
dot(const Vector2& a, const Vector2& b)
{
	return a.x * b.x + a.y * b.y;
}

inline f32
cross(const Vector2& a, const Vector2& b)
{
	return a.x * b.y - a.y * b.x;
}

inline f32
magnitude(const Vector2& a)
{
	return math::sqrt(math::dot(a, a));
}

inline Vector2
normalize(const Vector2& a)
{
	f32 m = magnitude(a);
	if (m > 0)
		return a * (1.0f / m);
	return {};
}

inline Vector2
hadamard(const Vector2& a, const Vector2& b)
{
	return {a.x * b.x, a.y * b.y};
}

inline f32
aspect_ratio(const Vector2& a)
{
	return a.x / a.y;
}


inline Matrix4
matrix2_to_matrix4(const Matrix2& m)
{
	Matrix4 result = MATRIX4_IDENTITY;
	result[0][0] = m[0][0];
	result[0][1] = m[0][1];
	result[1][0] = m[1][0];
	result[1][1] = m[1][1];
	return result;
}

// Vector3 functions
inline f32
dot(const Vector3& a, const Vector3& b)
{
	return a.x * b.x + a.y * b.y + a.z * b.z;
}

inline Vector3
cross(const Vector3& a, const Vector3& b)
{
	return Vector3{
		a.y * b.z - b.y * a.z, // x
		a.z * b.x - b.z * a.x, // y
		a.x * b.y - b.x * a.y  // z
	};
}

inline f32
magnitude(const Vector3& a)
{
	return math::sqrt(math::dot(a, a));
}

inline Vector3
normalize(const Vector3& a)
{
	f32 m = magnitude(a);
	if (m > 0)
		return a * (1.0f / m);
	return {};
}

inline Vector3
hadamard(const Vector3& a, const Vector3& b)
{
	return {a.x * b.x, a.y * b.y, a.z * b.z};
}

inline Matrix4
matrix3_to_matrix4(const Matrix3& m)
{
	Matrix4 result = MATRIX4_IDENTITY;
	result[0][0] = m[0][0];
	result[0][1] = m[0][1];
	result[0][2] = m[0][2];
	result[1][0] = m[1][0];
	result[1][1] = m[1][1];
	result[1][2] = m[1][2];
	result[2][0] = m[2][0];
	result[2][1] = m[2][1];
	result[2][2] = m[2][2];
	return result;
}

// Vector4 functions
inline f32
dot(const Vector4& a, const Vector4& b)
{
	return a.x*b.x + a.y*b.y + a.z*b.z + a.w*b.w;
}

inline f32
magnitude(const Vector4& a)
{
	return math::sqrt(math::dot(a, a));
}

inline Vector4
normalize(const Vector4& a)
{
	f32 m = magnitude(a);
	if (m > 0)
		return a * (1.0f / m);
	return {};
}

inline Vector4
hadamard(const Vector4& a, const Vector4& b)
{
	return {a.x * b.x, a.y * b.y, a.z * b.z, a.w * b.w};
}

// Complex Functions
inline f32
dot(const Complex& a, const Complex& b)
{
	return a.real * b.real + a.imag * b.imag;
}

inline f32
magnitude(const Complex& a)
{
	return math::sqrt(norm(a));
}

inline f32
norm(const Complex& a)
{
	return math::dot(a, a);
}

inline Complex
normalize(const Complex& a)
{
	f32 m = magnitude(a);
	if (m > 0)
		return a / magnitude(a);
	return COMPLEX_ZERO;
}

inline Complex
conjugate(const Complex& a)
{
	return {a.real, -a.imag};
}

inline Complex
inverse(const Complex& a)
{
	f32 m = norm(a);
	if (m > 0)
		return conjugate(a) / norm(a);
	return COMPLEX_ZERO;
}

inline f32
complex_angle(const Complex& a)
{
	return atan2(a.imag, a.real);
}

inline Complex
magnitude_angle(f32 magnitude, Angle a)
{
	f32 real = magnitude * math::cos(a);
	f32 imag = magnitude * math::sin(a);
	return {real, imag};
}

// Quaternion functions
inline f32
dot(const Quaternion& a, const Quaternion& b)
{
	return math::dot(a.xyz, b.xyz) + a.w*b.w;
}

inline Quaternion
cross(const Quaternion& a, const Quaternion& b)
{
	return Quaternion{a.w * b.x + a.x * b.w + a.y * b.z - a.z * b.y,
					  a.w * b.y + a.y * b.w + a.z * b.x - a.x * b.z,
					  a.w * b.z + a.z * b.w + a.x * b.y - a.y * b.x,
					  a.w * b.w - a.x * b.x - a.y * b.y - a.z * b.z};
}

inline f32
magnitude(const Quaternion& a)
{
	return math::sqrt(math::dot(a, a));
}

inline f32
norm(const Quaternion& a)
{
	return math::dot(a, a);
}

inline Quaternion
normalize(const Quaternion& a)
{
	f32 m = magnitude(a);
	if (m > 0)
		return a * (1.0f / m);
	return {};
}

inline Quaternion
conjugate(const Quaternion& a)
{
	return {-a.x, -a.y, -a.z, a.w};
}

inline Quaternion
inverse(const Quaternion& a)
{
	f32 m = 1.0f / dot(a, a);
	return math::conjugate(a) * m;
}

inline Angle
quaternion_angle(const Quaternion& a)
{
	return 2.0f * math::arccos(a.w);
}

inline Vector3
quaternion_axis(const Quaternion& a)
{
	f32 s2 = 1.0f - a.w * a.w;

	if (s2 <= 0.0f)
		return {0, 0, 1};

	f32 invs2 = 1.0f / math::sqrt(s2);

	return a.xyz * invs2;
}

inline Quaternion
axis_angle(const Vector3& axis, Angle angle)
{
	Vector3 a = math::normalize(axis);
	f32 s = math::sin(0.5f * angle);

	Quaternion q;
	q.xyz = a * s;
	q.w = math::cos(0.5f * angle);

	return q;
}

inline Angle
quaternion_roll(const Quaternion& a)
{
	return math::arctan2(2.0f * a.x * a.y + a.z * a.w,
					     a.x * a.x + a.w * a.w - a.y * a.y - a.z * a.z);
}

inline Angle
quaternion_pitch(const Quaternion& a)
{
	return math::arctan2(2.0f * a.y * a.z + a.w * a.x,
					     a.w * a.w - a.x * a.x - a.y * a.y + a.z * a.z);
}

inline Angle
quaternion_yaw(const Quaternion& a)
{
	return math::arcsin(-2.0f * (a.x * a.z - a.w * a.y));

}

inline Euler_Angles
quaternion_to_euler_angles(const Quaternion& a)
{
	return {quaternion_pitch(a), quaternion_yaw(a), quaternion_roll(a)};
}

inline Quaternion
euler_angles_to_quaternion(const Euler_Angles& e,
						   const Vector3& x_axis,
						   const Vector3& y_axis,
						   const Vector3& z_axis)
{
	Quaternion p = axis_angle(x_axis, e.pitch);
	Quaternion y = axis_angle(y_axis, e.yaw);
	Quaternion r = axis_angle(z_axis, e.roll);

	return y * p * r;
}


// Spherical Linear Interpolation
inline Quaternion
slerp(const Quaternion& x, const Quaternion& y, f32 t)
{
	Quaternion z = y;

	f32 cos_theta = dot(x, y);

	if (cos_theta < 0.0f)
	{
		z = -y;
		cos_theta = -cos_theta;
	}

	if (cos_theta > 1.0f)
	{
		return Quaternion{lerp(x.x, y.x, t),
						  lerp(x.y, y.y, t),
						  lerp(x.z, y.z, t),
						  lerp(x.w, y.w, t)};
	}

	Angle angle = math::arccos(cos_theta);

	Quaternion result = math::sin(angle::radians(1.0f) - (t * angle)) * x + math::sin(t * angle) * z;
	return result * (1.0f / math::sin(angle));
}

// Shoemake's Quaternion Curves
// Sqherical Cubic Interpolation
inline Quaternion
squad(const Quaternion& p,
	  const Quaternion& a,
	  const Quaternion& b,
	  const Quaternion& q,
	  f32 t)
{
	return slerp(slerp(p, q, t), slerp(a, b, t), 2.0f * t * (1.0f - t));
}

// Matrix2 functions
inline Matrix2
transpose(const Matrix2& m)
{
	Matrix2 result;
	for (usize i = 0; i < 2; i++)
	{
		for (usize j = 0; j < 2; j++)
			result[i][j] = m[j][i];
	}
	return result;
}

inline f32
determinant(const Matrix2& m)
{
	return m[0][0] * m[1][1] - m[1][0] * m[0][1];
}

inline Matrix2
inverse(const Matrix2& m)
{
	f32 inv_det = 1.0f / (m[0][0] * m[1][1] - m[1][0] * m[0][1]);
	Matrix2 result;
	result[0][0] =  m[1][1] * inv_det;
	result[0][1] = -m[0][1] * inv_det;
	result[1][0] = -m[1][0] * inv_det;
	result[1][1] =  m[0][0] * inv_det;
	return result;
}

inline Matrix2
hadamard(const Matrix2& a, const Matrix2&b)
{
	Matrix2 result;
	result[0] = a[0] * b[0];
	result[1] = a[1] * b[1];
	return result;
}

// Matrix3 functions
inline Matrix3
transpose(const Matrix3& m)
{
	Matrix3 result;

	for (usize i = 0; i < 3; i++)
	{
		for (usize j = 0; j < 3; j++)
			result[i][j] = m[j][i];
	}
	return result;
}

inline f32
determinant(const Matrix3& m)
{
	return ( m[0][0] * (m[1][1] * m[2][2] - m[2][1] * m[1][2])
			-m[1][0] * (m[0][1] * m[2][2] - m[2][1] * m[0][2])
			+m[2][0] * (m[0][1] * m[1][2] - m[1][1] * m[0][2]));
}

inline Matrix3
inverse(const Matrix3& m)
{
	f32 inv_det = 1.0f / (
		+ m[0][0] * (m[1][1] * m[2][2] - m[2][1] * m[1][2])
		- m[1][0] * (m[0][1] * m[2][2] - m[2][1] * m[0][2])
		+ m[2][0] * (m[0][1] * m[1][2] - m[1][1] * m[0][2]));

	Matrix3 result;

	result[0][0] = +(m[1][1] * m[2][2] - m[2][1] * m[1][2]) * inv_det;
	result[1][0] = -(m[1][0] * m[2][2] - m[2][0] * m[1][2]) * inv_det;
	result[2][0] = +(m[1][0] * m[2][1] - m[2][0] * m[1][1]) * inv_det;
	result[0][1] = -(m[0][1] * m[2][2] - m[2][1] * m[0][2]) * inv_det;
	result[1][1] = +(m[0][0] * m[2][2] - m[2][0] * m[0][2]) * inv_det;
	result[2][1] = -(m[0][0] * m[2][1] - m[2][0] * m[0][1]) * inv_det;
	result[0][2] = +(m[0][1] * m[1][2] - m[1][1] * m[0][2]) * inv_det;
	result[1][2] = -(m[0][0] * m[1][2] - m[1][0] * m[0][2]) * inv_det;
	result[2][2] = +(m[0][0] * m[1][1] - m[1][0] * m[0][1]) * inv_det;

	return result;
}

inline Matrix3
hadamard(const Matrix3& a, const Matrix3&b)
{
	Matrix3 result;
	result[0] = a[0] * b[0];
	result[1] = a[1] * b[1];
	result[2] = a[2] * b[2];
	return result;
}

// Matrix4 functions
inline Matrix4
transpose(const Matrix4& m)
{
	Matrix4 result;

	for (usize i = 0; i < 4; i++)
	{
		for (usize j = 0; j < 4; j++)
			result[i][j] = m[j][i];
	}
	return result;
}

f32
determinant(const Matrix4& m)
{
	f32 coef00 = m[2][2] * m[3][3] - m[3][2] * m[2][3];
	f32 coef02 = m[1][2] * m[3][3] - m[3][2] * m[1][3];
	f32 coef03 = m[1][2] * m[2][3] - m[2][2] * m[1][3];

	f32 coef04 = m[2][1] * m[3][3] - m[3][1] * m[2][3];
	f32 coef06 = m[1][1] * m[3][3] - m[3][1] * m[1][3];
	f32 coef07 = m[1][1] * m[2][3] - m[2][1] * m[1][3];

	f32 coef08 = m[2][1] * m[3][2] - m[3][1] * m[2][2];
	f32 coef10 = m[1][1] * m[3][2] - m[3][1] * m[1][2];
	f32 coef11 = m[1][1] * m[2][2] - m[2][1] * m[1][2];

	f32 coef12 = m[2][0] * m[3][3] - m[3][0] * m[2][3];
	f32 coef14 = m[1][0] * m[3][3] - m[3][0] * m[1][3];
	f32 coef15 = m[1][0] * m[2][3] - m[2][0] * m[1][3];

	f32 coef16 = m[2][0] * m[3][2] - m[3][0] * m[2][2];
	f32 coef18 = m[1][0] * m[3][2] - m[3][0] * m[1][2];
	f32 coef19 = m[1][0] * m[2][2] - m[2][0] * m[1][2];

	f32 coef20 = m[2][0] * m[3][1] - m[3][0] * m[2][1];
	f32 coef22 = m[1][0] * m[3][1] - m[3][0] * m[1][1];
	f32 coef23 = m[1][0] * m[2][1] - m[2][0] * m[1][1];

	Vector4 fac0 = {coef00, coef00, coef02, coef03};
	Vector4 fac1 = {coef04, coef04, coef06, coef07};
	Vector4 fac2 = {coef08, coef08, coef10, coef11};
	Vector4 fac3 = {coef12, coef12, coef14, coef15};
	Vector4 fac4 = {coef16, coef16, coef18, coef19};
	Vector4 fac5 = {coef20, coef20, coef22, coef23};

	Vector4 vec0 = {m[1][0], m[0][0], m[0][0], m[0][0]};
	Vector4 vec1 = {m[1][1], m[0][1], m[0][1], m[0][1]};
	Vector4 vec2 = {m[1][2], m[0][2], m[0][2], m[0][2]};
	Vector4 vec3 = {m[1][3], m[0][3], m[0][3], m[0][3]};

	Vector4 inv0 = vec1 * fac0 - vec2 * fac1 + vec3 * fac2;
	Vector4 inv1 = vec0 * fac0 - vec2 * fac3 + vec3 * fac4;
	Vector4 inv2 = vec0 * fac1 - vec1 * fac3 + vec3 * fac5;
	Vector4 inv3 = vec0 * fac2 - vec1 * fac4 + vec2 * fac5;

	Vector4 signA = {+1, -1, +1, -1};
	Vector4 signB = {-1, +1, -1, +1};
	Matrix4 inverse = {inv0 * signA, inv1 * signB, inv2 * signA, inv3 * signB};

	Vector4 row0 = {inverse[0][0], inverse[1][0], inverse[2][0], inverse[3][0]};

	Vector4 dot0 = m[0] * row0;
	f32 dot1 = (dot0[0] + dot0[1]) + (dot0[2] + dot0[3]);
	return dot1;
}

Matrix4
inverse(const Matrix4& m)
{
	f32 coef00 = m[2][2] * m[3][3] - m[3][2] * m[2][3];
	f32 coef02 = m[1][2] * m[3][3] - m[3][2] * m[1][3];
	f32 coef03 = m[1][2] * m[2][3] - m[2][2] * m[1][3];
	f32 coef04 = m[2][1] * m[3][3] - m[3][1] * m[2][3];
	f32 coef06 = m[1][1] * m[3][3] - m[3][1] * m[1][3];
	f32 coef07 = m[1][1] * m[2][3] - m[2][1] * m[1][3];
	f32 coef08 = m[2][1] * m[3][2] - m[3][1] * m[2][2];
	f32 coef10 = m[1][1] * m[3][2] - m[3][1] * m[1][2];
	f32 coef11 = m[1][1] * m[2][2] - m[2][1] * m[1][2];
	f32 coef12 = m[2][0] * m[3][3] - m[3][0] * m[2][3];
	f32 coef14 = m[1][0] * m[3][3] - m[3][0] * m[1][3];
	f32 coef15 = m[1][0] * m[2][3] - m[2][0] * m[1][3];
	f32 coef16 = m[2][0] * m[3][2] - m[3][0] * m[2][2];
	f32 coef18 = m[1][0] * m[3][2] - m[3][0] * m[1][2];
	f32 coef19 = m[1][0] * m[2][2] - m[2][0] * m[1][2];
	f32 coef20 = m[2][0] * m[3][1] - m[3][0] * m[2][1];
	f32 coef22 = m[1][0] * m[3][1] - m[3][0] * m[1][1];
	f32 coef23 = m[1][0] * m[2][1] - m[2][0] * m[1][1];

	Vector4 fac0 = {coef00, coef00, coef02, coef03};
	Vector4 fac1 = {coef04, coef04, coef06, coef07};
	Vector4 fac2 = {coef08, coef08, coef10, coef11};
	Vector4 fac3 = {coef12, coef12, coef14, coef15};
	Vector4 fac4 = {coef16, coef16, coef18, coef19};
	Vector4 fac5 = {coef20, coef20, coef22, coef23};

	Vector4 vec0 = {m[1][0], m[0][0], m[0][0], m[0][0]};
	Vector4 vec1 = {m[1][1], m[0][1], m[0][1], m[0][1]};
	Vector4 vec2 = {m[1][2], m[0][2], m[0][2], m[0][2]};
	Vector4 vec3 = {m[1][3], m[0][3], m[0][3], m[0][3]};

	Vector4 inv0 = vec1 * fac0 - vec2 * fac1 + vec3 * fac2;
	Vector4 inv1 = vec0 * fac0 - vec2 * fac3 + vec3 * fac4;
	Vector4 inv2 = vec0 * fac1 - vec1 * fac3 + vec3 * fac5;
	Vector4 inv3 = vec0 * fac2 - vec1 * fac4 + vec2 * fac5;

	Vector4 signA = {+1, -1, +1, -1};
	Vector4 signB = {-1, +1, -1, +1};
	Matrix4 inverse = {inv0 * signA, inv1 * signB, inv2 * signA, inv3 * signB};

	Vector4 row0 = {inverse[0][0], inverse[1][0], inverse[2][0], inverse[3][0]};

	Vector4 dot0 = m[0] * row0;
	f32 dot1 = (dot0[0] + dot0[1]) + (dot0[2] + dot0[3]);

	f32 oneOverDeterminant = 1.0f / dot1;

	return inverse * oneOverDeterminant;
}

inline Matrix4
hadamard(const Matrix4& a, const Matrix4& b)
{
	Matrix4 result;

	result[0] = a[0] * b[0];
	result[1] = a[1] * b[1];
	result[2] = a[2] * b[2];
	result[3] = a[3] * b[3];

	return result;
}

inline bool
is_affine(const Matrix4& m)
{
	// E.g. No translation
	return (equals(m.columns[3].x, 0)) &
		   (equals(m.columns[3].y, 0)) &
		   (equals(m.columns[3].z, 0)) &
		   (equals(m.columns[3].w, 1.0f));
}


inline Matrix4
quaternion_to_matrix4(const Quaternion& q)
{
	Matrix4 mat = MATRIX4_IDENTITY;

	Quaternion a = math::normalize(q);

	f32 xx = a.x * a.x;
	f32 yy = a.y * a.y;
	f32 zz = a.z * a.z;
	f32 xy = a.x * a.y;
	f32 xz = a.x * a.z;
	f32 yz = a.y * a.z;
	f32 wx = a.w * a.x;
	f32 wy = a.w * a.y;
	f32 wz = a.w * a.z;

	mat[0][0] = 1.0f - 2.0f * (yy + zz);
	mat[0][1] =        2.0f * (xy + wz);
	mat[0][2] =        2.0f * (xz - wy);

	mat[1][0] =        2.0f * (xy - wz);
	mat[1][1] = 1.0f - 2.0f * (xx + zz);
	mat[1][2] =        2.0f * (yz + wx);

	mat[2][0] =        2.0f * (xz + wy);
	mat[2][1] =        2.0f * (yz - wx);
	mat[2][2] = 1.0f - 2.0f * (xx + yy);

	return mat;
}

Quaternion
matrix4_to_quaternion(const Matrix4& m)
{
	f32 four_x_squared_minus_1 = m[0][0] - m[1][1] - m[2][2];
	f32 four_y_squared_minus_1 = m[1][1] - m[0][0] - m[2][2];
	f32 four_z_squared_minus_1 = m[2][2] - m[0][0] - m[1][1];
	f32 four_w_squared_minus_1 = m[0][0] + m[1][1] + m[2][2];

	s32 biggestIndex = 0;
	f32 four_biggest_squared_minus_1 = four_w_squared_minus_1;
	if (four_x_squared_minus_1 > four_biggest_squared_minus_1)
	{
		four_biggest_squared_minus_1 = four_x_squared_minus_1;
		biggestIndex = 1;
	}
	if (four_y_squared_minus_1 > four_biggest_squared_minus_1)
	{
		four_biggest_squared_minus_1 = four_y_squared_minus_1;
		biggestIndex = 2;
	}
	if (four_z_squared_minus_1 > four_biggest_squared_minus_1)
	{
		four_biggest_squared_minus_1 = four_z_squared_minus_1;
		biggestIndex = 3;
	}

	f32 biggestVal = math::sqrt(four_biggest_squared_minus_1 + 1.0f) * 0.5f;
	f32 mult       = 0.25f / biggestVal;

	Quaternion q = QUATERNION_IDENTITY;

	switch (biggestIndex)
	{
	case 0:
	{
		q.w = biggestVal;
		q.x = (m[1][2] - m[2][1]) * mult;
		q.y = (m[2][0] - m[0][2]) * mult;
		q.z = (m[0][1] - m[1][0]) * mult;
	}
	break;
	case 1:
	{
		q.w = (m[1][2] - m[2][1]) * mult;
		q.x = biggestVal;
		q.y = (m[0][1] + m[1][0]) * mult;
		q.z = (m[2][0] + m[0][2]) * mult;
	}
	break;
	case 2:
	{
		q.w = (m[2][0] - m[0][2]) * mult;
		q.x = (m[0][1] + m[1][0]) * mult;
		q.y = biggestVal;
		q.z = (m[1][2] + m[2][1]) * mult;
	}
	break;
	case 3:
	{
		q.w = (m[0][1] - m[1][0]) * mult;
		q.x = (m[2][0] + m[0][2]) * mult;
		q.y = (m[1][2] + m[2][1]) * mult;
		q.z = biggestVal;
	}
	break;
	default: // Should never actually get here. Just for sanities sake.
	{
		GB_ASSERT(false, "How did you get here?!");
	}
	break;
	}

	return q;
}


inline Matrix4
translate(const Vector3& v)
{
	Matrix4 result = MATRIX4_IDENTITY;
	result[3].xyz = v;
	result[3].w = 1;
	return result;
}

inline Matrix4
rotate(const Vector3& v, Angle angle)
{
	const f32 c = math::cos(angle);
	const f32 s = math::sin(angle);

	const Vector3 axis = math::normalize(v);
	const Vector3 t    = (1.0f - c) * axis;

	Matrix4 rot = MATRIX4_IDENTITY;

	rot[0][0] = c + t.x * axis.x;
	rot[0][1] = 0 + t.x * axis.y + s * axis.z;
	rot[0][2] = 0 + t.x * axis.z - s * axis.y;
	rot[0][3] = 0;

	rot[1][0] = 0 + t.y * axis.x - s * axis.z;
	rot[1][1] = c + t.y * axis.y;
	rot[1][2] = 0 + t.y * axis.z + s * axis.x;
	rot[1][3] = 0;

	rot[2][0] = 0 + t.z * axis.x + s * axis.y;
	rot[2][1] = 0 + t.z * axis.y - s * axis.x;
	rot[2][2] = c + t.z * axis.z;
	rot[2][3] = 0;

	return rot;
}

inline Matrix4
scale(const Vector3& v)
{
	return { v.x,   0,   0, 0,
			   0, v.y,   0, 0,
			   0,   0, v.z, 0,
			   0,   0,   0, 1 };
}

inline Matrix4
ortho(f32 left, f32 right, f32 bottom, f32 top)
{
	return ortho(left, right, bottom, top, -1.0f, 1.0f);
}

inline Matrix4
ortho(f32 left, f32 right, f32 bottom, f32 top, f32 z_near, f32 z_far)
{
	Matrix4 result = MATRIX4_IDENTITY;

	result[0][0] = 2.0f / (right - left);
	result[1][1] = 2.0f / (top - bottom);
	result[2][2] = -2.0f / (z_far - z_near);
	result[3][0] = -(right + left) / (right - left);
	result[3][1] = -(top + bottom) / (top - bottom);
	result[3][2] = -(z_far + z_near) / (z_far - z_near);

	return result;
}

inline Matrix4
perspective(Angle fovy, f32 aspect, f32 z_near, f32 z_far)
{
	GB_ASSERT(math::abs(aspect) > 0.0f,
			  "math::perspective `fovy` is %f rad", angle::as_radians(fovy));

	f32 tan_half_fovy = math::tan(0.5f * fovy);

	Matrix4 result = {};
	result[0][0]   = 1.0f / (aspect * tan_half_fovy);
	result[1][1]   = 1.0f / (tan_half_fovy);
	result[2][2]   = -(z_far + z_near) / (z_far - z_near);
	result[2][3]   = -1.0f;
	result[3][2]   = -2.0f * z_far * z_near / (z_far - z_near);

	return result;
}

inline Matrix4
infinite_perspective(Angle fovy, f32 aspect, f32 z_near)
{
	f32 range  = math::tan(0.5f * fovy) * z_near;
	f32 left   = -range * aspect;
	f32 right  =  range * aspect;
	f32 bottom = -range;
	f32 top    =  range;

	Matrix4 result = {};

	result[0][0] = (2.0f * z_near) / (right - left);
	result[1][1] = (2.0f * z_near) / (top - bottom);
	result[2][2] = -1.0f;
	result[2][3] = -1.0f;
	result[3][2] = -2.0f * z_near;

	return result;
}


inline Matrix4
look_at_matrix4(const Vector3& eye, const Vector3& center, const Vector3& up)
{
	const Vector3 f = math::normalize(center - eye);
	const Vector3 s = math::normalize(math::cross(f, up));
	const Vector3 u = math::cross(s, f);

	Matrix4 result = MATRIX4_IDENTITY;

	result[0][0] = +s.x;
	result[1][0] = +s.y;
	result[2][0] = +s.z;

	result[0][1] = +u.x;
	result[1][1] = +u.y;
	result[2][1] = +u.z;

	result[0][2] = -f.x;
	result[1][2] = -f.y;
	result[2][2] = -f.z;

	result[3][0] = -math::dot(s, eye);
	result[3][1] = -math::dot(u, eye);
	result[3][2] = +math::dot(f, eye);

	return result;
}


inline Quaternion
look_at_quaternion(const Vector3& eye, const Vector3& center, const Vector3& up)
{
	if (math::equals(math::magnitude(center - eye), 0, 0.001f))
		return QUATERNION_IDENTITY; // You cannot look at where you are!

#if 1
	return matrix4_to_quaternion(look_at_matrix4(eye, center, up));
#else
	// TODO(bill): Thoroughly test this look_at_quaternion!
	// Is it more efficient that that a converting a Matrix4 to a Quaternion?
	Vector3 forward_l = math::normalize(center - eye);
	Vector3 forward_w = {1, 0, 0};
	Vector3 axis = math::cross(forward_l, forward_w);

	f32 angle = math::acos(math::dot(forward_l, forward_w));

	Vector3 third = math::cross(axis, forward_w);
	if (math::dot(third, forward_l) < 0)
		angle = -angle;

	Quaternion q1 = math::axis_angle(axis, angle);

	Vector3 up_l  = q1 * math::normalize(up);
	Vector3 right = math::normalize(math::cross(forward_l, up));
	Vector3 up_w  = math::normalize(math::cross(right, forward_l));

	Vector3 axis2  = math::cross(up_l, up_w);
	f32     angle2 = math::acos(math::dot(up_l, up_w));

	Quaternion q2 = math::axis_angle(axis2, angle2);

	return q2 * q1;
#endif
}

// Transform Functions
inline Vector3
transform_point(const Transform& transform, const Vector3& point)
{
	return (math::conjugate(transform.orientation) * (transform.position - point)) / transform.scale;
}

inline Transform
inverse(const Transform& t)
{
	const Quaternion inv_orientation = math::conjugate(t.orientation);

	Transform inv_transform;

	inv_transform.position    = (inv_orientation * -t.position) / t.scale;
	inv_transform.orientation = inv_orientation;
	// inv_transform.scale       = inv_orientation * (Vector3{1, 1, 1} / t.scale); // Vector3 scale
	inv_transform.scale       = 1.0f / t.scale;

	return inv_transform;
}

inline Matrix4
transform_to_matrix4(const Transform& t)
{
	return math::translate(t.position) *
		   math::quaternion_to_matrix4(t.orientation) *
		   math::scale({t.scale, t.scale, t.scale});
}
} // namespace math


namespace aabb
{
inline Aabb
calculate(const void* vertices, usize num_vertices, usize stride, usize offset)
{
	Vector3 min;
	Vector3 max;
	const u8* vertex = reinterpret_cast<const u8*>(vertices);
	vertex += offset;
	Vector3 position = pseudo_cast<Vector3>(vertex);
	min.x = max.x = position.x;
	min.y = max.y = position.y;
	min.z = max.z = position.z;
	vertex += stride;

	for (usize i = 1; i < num_vertices; i++)
	{
		position = pseudo_cast<Vector3>(vertex);
		vertex += stride;

		Vector3 p = position;
		min.x = math::min(p.x, min.x);
		min.y = math::min(p.y, min.y);
		min.z = math::min(p.z, min.z);
		max.x = math::max(p.x, max.x);
		max.y = math::max(p.y, max.y);
		max.z = math::max(p.z, max.z);
	}

	Aabb aabb;

	aabb.center    = 0.5f * (min + max);
	aabb.half_size = 0.5f * (max - min);

	return aabb;
}

inline f32
surface_area(const Aabb& aabb)
{
	Vector3 h = aabb.half_size * 2.0f;
	f32 s = 0.0f;
	s += h.x * h.y;
	s += h.y * h.z;
	s += h.z * h.x;
	s *= 3.0f;
	return s;
}

inline f32
volume(const Aabb& aabb)
{
	Vector3 h = aabb.half_size * 2.0f;
	return h.x * h.y * h.z;
}

inline Sphere
to_sphere(const Aabb& aabb)
{
	Sphere s;
	s.center = aabb.center;
	s.radius = math::magnitude(aabb.half_size);
	return s;
}


inline bool
contains(const Aabb& aabb, const Vector3& point)
{
	Vector3 distance = aabb.center - point;

	// NOTE(bill): & is faster than &&
	return (math::abs(distance.x) <= aabb.half_size.x) &
		   (math::abs(distance.y) <= aabb.half_size.y) &
		   (math::abs(distance.z) <= aabb.half_size.z);
}

inline bool
contains(const Aabb& a, const Aabb& b)
{
	Vector3 dist = a.center - b.center;

	// NOTE(bill): & is faster than &&
	return (math::abs(dist.x) + b.half_size.x <= a.half_size.x) &
		   (math::abs(dist.y) + b.half_size.y <= a.half_size.y) &
		   (math::abs(dist.z) + b.half_size.z <= a.half_size.z);
}


inline bool
intersects(const Aabb& a, const Aabb& b)
{
	Vector3 dist = a.center - b.center;
	Vector3 sum_half_sizes = a.half_size + b.half_size;

	// NOTE(bill): & is faster than &&
	return (math::abs(dist.x) <= sum_half_sizes.x) &
		   (math::abs(dist.y) <= sum_half_sizes.y) &
		   (math::abs(dist.z) <= sum_half_sizes.z);
}

inline Aabb
transform_affine(const Aabb& aabb, const Matrix4& m)
{
	GB_ASSERT(math::is_affine(m),
			  "Passed Matrix4 must be an affine matrix");

	Aabb result;
	Vector4 ac;
	ac.xyz = aabb.center;
	ac.w   = 1;
	result.center = (m * ac).xyz;

	Vector3 hs = aabb.half_size;
	f32 x = math::abs(m[0][0] * hs.x + math::abs(m[0][1]) * hs.y + math::abs(m[0][2]) * hs.z);
	f32 y = math::abs(m[1][0] * hs.x + math::abs(m[1][1]) * hs.y + math::abs(m[1][2]) * hs.z);
	f32 z = math::abs(m[2][0] * hs.x + math::abs(m[2][1]) * hs.y + math::abs(m[2][2]) * hs.z);

	result.half_size.x = math::is_infinite(math::abs(hs.x)) ? hs.x : x;
	result.half_size.y = math::is_infinite(math::abs(hs.y)) ? hs.y : y;
	result.half_size.z = math::is_infinite(math::abs(hs.z)) ? hs.z : z;

	return result;
}
} // namespace aabb

namespace sphere
{
inline Sphere
calculate_min_bounding(const void* vertices, usize num_vertices, usize stride, usize offset, f32 step)
{
	auto gen = random::make(0);

	const u8* vertex = reinterpret_cast<const u8*>(vertices);
	vertex += offset;

	Vector3 position = pseudo_cast<Vector3>(vertex[0]);
	Vector3 center = position;
	center += pseudo_cast<Vector3>(vertex[1 * stride]);
	center *= 0.5f;

	Vector3 d = position - center;
	f32 max_dist_sq = math::dot(d, d);
	f32 radius_step = step * 0.37f;

	bool done;
	do
	{
		done = true;
		for (u32 i = 0, index = random::uniform_u32(&gen, 0, num_vertices-1);
			 i < num_vertices;
			 i++, index = (index + 1)%num_vertices)
		{
			Vector3 position = pseudo_cast<Vector3>(vertex[index * stride]);

			d = position - center;
			f32 dist_sq = math::dot(d, d);

			if (dist_sq > max_dist_sq)
			{
				done = false;

				center = d * radius_step;
				max_dist_sq = math::lerp(max_dist_sq, dist_sq, step);

				break;
			}
		}
	}
	while (!done);

	Sphere result;

	result.center = center;
	result.radius = math::sqrt(max_dist_sq);

	return result;
}

inline Sphere
calculate_max_bounding(const void* vertices, usize num_vertices, usize stride, usize offset)
{
	Aabb aabb = aabb::calculate(vertices, num_vertices, stride, offset);

	Vector3 center = aabb.center;

	f32 max_dist_sq = 0.0f;
	const u8* vertex = reinterpret_cast<const u8*>(vertices);
	vertex += offset;

	for (usize i = 0; i < num_vertices; i++)
	{
		Vector3 position = pseudo_cast<Vector3>(vertex);
		vertex += stride;

		Vector3 d = position - center;
		f32 dist_sq = math::dot(d, d);
		max_dist_sq = math::max(dist_sq, max_dist_sq);
	}

	Sphere sphere;
	sphere.center = center;
	sphere.radius = math::sqrt(max_dist_sq);

	return sphere;
}

inline f32
surface_area(const Sphere& s)
{
	return 2.0f * math::TAU * s.radius * s.radius;
}

inline f32
volume(const Sphere& s)
{
	return math::TWO_THIRDS * math::TAU * s.radius * s.radius * s.radius;
}

inline Aabb
to_aabb(const Sphere& s)
{
	Aabb a;
	a.center = s.center;
	a.half_size.x = s.radius * math::SQRT_3;
	a.half_size.y = s.radius * math::SQRT_3;
	a.half_size.z = s.radius * math::SQRT_3;
	return a;
}

inline bool
contains_point(const Sphere& s, const Vector3& point)
{
	Vector3 dr = point - s.center;
	f32 distance = math::dot(dr, dr);
	return distance < s.radius * s.radius;
}

inline f32
ray_intersection(const Vector3& from, const Vector3& dir, const Sphere& s)
{
	Vector3 v = s.center - from;
	f32 b = math::dot(v, dir);
	f32 det = (s.radius * s.radius) - math::dot(v, v) + (b * b);

	if (det < 0.0 || b < s.radius)
		return -1.0f;
	return b - math::sqrt(det);
}
} // namespace sphere

namespace plane
{
inline f32
ray_intersection(const Vector3& from, const Vector3& dir, const Plane& p)
{
	f32 nd   = math::dot(dir,  p.normal);
	f32 orpn = math::dot(from, p.normal);
	f32 dist = -1.0f;

	if (nd < 0.0f)
		dist = (-p.distance - orpn) / nd;

	return dist > 0.0f ? dist : -1.0f;
}

inline bool
intersection3(const Plane& p1, const Plane& p2, const Plane& p3, Vector3* ip)
{
	const Vector3& n1 = p1.normal;
	const Vector3& n2 = p2.normal;
	const Vector3& n3 = p3.normal;

	f32 den = -math::dot(math::cross(n1, n2), n3);

	if (math::equals(den, 0.0f))
		return false;

	Vector3 res = p1.distance * math::cross(n2, n3)
				+ p2.distance * math::cross(n3, n1)
				+ p3.distance * math::cross(n1, n2);
	*ip = res / den;

	return true;
}
} // namespace plane

namespace random
{
inline Random
make(s64 seed)
{
	Random r = {};
	set_seed(&r, seed);
	return r;
}

void
set_seed(Random* r, s64 seed)
{
	r->seed  = seed;
	r->mt[0] = seed;
	for (u64 i = 1; i < 312; i++)
		r->mt[i] = 6364136223846793005ull * (r->mt[i-1] ^ r->mt[i-1] >> 62) + i;
}

s64
next(Random* r)
{
	const u64 MAG01[2] = {0ull, 0xb5026f5aa96619e9ull};

	u64 x;
	if (r->index > 312)
	{
		u32 i = 0;
		for (; i < 312-156; i++)
		{
			x = (r->mt[i] & 0xffffffff80000000ull) | (r->mt[i+1] & 0x7fffffffull);
			r->mt[i] = r->mt[i+156] ^ (x>>1) ^ MAG01[(u32)(x & 1ull)];
		}
		for (; i < 312-1; i++)
		{
			x = (r->mt[i] & 0xffffffff80000000ull) | (r->mt[i+1] & 0x7fffffffull);
			r->mt[i] = r->mt[i + (312-156)] ^ (x >> 1) ^ MAG01[(u32)(x & 1ull)];
		}
		x = (r->mt[312-1] & 0xffffffff80000000ull) | (r->mt[0] & 0x7fffffffull);
		r->mt[312-1] = r->mt[156-1] ^ (x>>1) ^ MAG01[(u32)(x & 1ull)];

		r->index = 0;
	}

	x = r->mt[r->index++];

	x ^= (x >> 29) & 0x5555555555555555ull;
	x ^= (x << 17) & 0x71d67fffeda60000ull;
	x ^= (x << 37) & 0xfff7eee000000000ull;
	x ^= (x >> 43);

	return x;
}

void
next_from_device(void* buffer, u32 length_in_bytes)
{
#if defined(GB_SYSTEM_WINDOWS)
	HCRYPTPROV prov;

	bool ok = CryptAcquireContext(&prov, NULL, NULL, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT);
	GB_ASSERT(ok, "CryptAcquireContext");
	ok = CryptGenRandom(prov, length_in_bytes, reinterpret_cast<u8*>(&buffer));
	GB_ASSERT(ok, "CryptGenRandom");

	CryptReleaseContext(prov, 0);

#else
	#error Implement random::next_from_device()
#endif
}

s32
next_s32(Random* r)
{
	return bit_cast<s32>(random::next(r));
}

u32
next_u32(Random* r)
{
	return bit_cast<u32>(random::next(r));
}

f32
next_f32(Random* r)
{
	return bit_cast<f32>(random::next(r));
}

s64
next_s64(Random* r)
{
	return random::next(r);
}

u64
next_u64(Random* r)
{
	return bit_cast<u64>(random::next(r));
}

f64
next_f64(Random* r)
{
	return bit_cast<f64>(random::next(r));
}

s32
uniform_s32(Random* r, s32 min_inc, s32 max_inc)
{
	return (random::next_s32(r) & (max_inc - min_inc + 1)) + min_inc;
}

u32
uniform_u32(Random* r, u32 min_inc, u32 max_inc)
{
	return (random::next_u32(r) & (max_inc - min_inc + 1)) + min_inc;
}

f32
uniform_f32(Random* r, f32 min_inc, f32 max_inc)
{
	f64 n = (random::next_s64(r) >> 11) * (1.0/4503599627370495.0);
	return static_cast<f32>(n * (max_inc - min_inc + 1.0) + min_inc);
}

s64
uniform_s64(Random* r, s64 min_inc, s64 max_inc)
{
	return (random::next_s32(r) & (max_inc - min_inc + 1)) + min_inc;
}

u64
uniform_u64(Random* r, u64 min_inc, u64 max_inc)
{
	return (random::next_u64(r) & (max_inc - min_inc + 1)) + min_inc;
}

f64
uniform_f64(Random* r, f64 min_inc, f64 max_inc)
{
	f64 n = (random::next_s64(r) >> 11) * (1.0/4503599627370495.0);
	return (n * (max_inc - min_inc + 1.0) + min_inc);
}


global s32 g_perlin_randtab[512] =
{
   23, 125, 161, 52, 103, 117, 70, 37, 247, 101, 203, 169, 124, 126, 44, 123,
   152, 238, 145, 45, 171, 114, 253, 10, 192, 136, 4, 157, 249, 30, 35, 72,
   175, 63, 77, 90, 181, 16, 96, 111, 133, 104, 75, 162, 93, 56, 66, 240,
   8, 50, 84, 229, 49, 210, 173, 239, 141, 1, 87, 18, 2, 198, 143, 57,
   225, 160, 58, 217, 168, 206, 245, 204, 199, 6, 73, 60, 20, 230, 211, 233,
   94, 200, 88, 9, 74, 155, 33, 15, 219, 130, 226, 202, 83, 236, 42, 172,
   165, 218, 55, 222, 46, 107, 98, 154, 109, 67, 196, 178, 127, 158, 13, 243,
   65, 79, 166, 248, 25, 224, 115, 80, 68, 51, 184, 128, 232, 208, 151, 122,
   26, 212, 105, 43, 179, 213, 235, 148, 146, 89, 14, 195, 28, 78, 112, 76,
   250, 47, 24, 251, 140, 108, 186, 190, 228, 170, 183, 139, 39, 188, 244, 246,
   132, 48, 119, 144, 180, 138, 134, 193, 82, 182, 120, 121, 86, 220, 209, 3,
   91, 241, 149, 85, 205, 150, 113, 216, 31, 100, 41, 164, 177, 214, 153, 231,
   38, 71, 185, 174, 97, 201, 29, 95, 7, 92, 54, 254, 191, 118, 34, 221,
   131, 11, 163, 99, 234, 81, 227, 147, 156, 176, 17, 142, 69, 12, 110, 62,
   27, 255, 0, 194, 59, 116, 242, 252, 19, 21, 187, 53, 207, 129, 64, 135,
   61, 40, 167, 237, 102, 223, 106, 159, 197, 189, 215, 137, 36, 32, 22, 5,

// Copy
   23, 125, 161, 52, 103, 117, 70, 37, 247, 101, 203, 169, 124, 126, 44, 123,
   152, 238, 145, 45, 171, 114, 253, 10, 192, 136, 4, 157, 249, 30, 35, 72,
   175, 63, 77, 90, 181, 16, 96, 111, 133, 104, 75, 162, 93, 56, 66, 240,
   8, 50, 84, 229, 49, 210, 173, 239, 141, 1, 87, 18, 2, 198, 143, 57,
   225, 160, 58, 217, 168, 206, 245, 204, 199, 6, 73, 60, 20, 230, 211, 233,
   94, 200, 88, 9, 74, 155, 33, 15, 219, 130, 226, 202, 83, 236, 42, 172,
   165, 218, 55, 222, 46, 107, 98, 154, 109, 67, 196, 178, 127, 158, 13, 243,
   65, 79, 166, 248, 25, 224, 115, 80, 68, 51, 184, 128, 232, 208, 151, 122,
   26, 212, 105, 43, 179, 213, 235, 148, 146, 89, 14, 195, 28, 78, 112, 76,
   250, 47, 24, 251, 140, 108, 186, 190, 228, 170, 183, 139, 39, 188, 244, 246,
   132, 48, 119, 144, 180, 138, 134, 193, 82, 182, 120, 121, 86, 220, 209, 3,
   91, 241, 149, 85, 205, 150, 113, 216, 31, 100, 41, 164, 177, 214, 153, 231,
   38, 71, 185, 174, 97, 201, 29, 95, 7, 92, 54, 254, 191, 118, 34, 221,
   131, 11, 163, 99, 234, 81, 227, 147, 156, 176, 17, 142, 69, 12, 110, 62,
   27, 255, 0, 194, 59, 116, 242, 252, 19, 21, 187, 53, 207, 129, 64, 135,
   61, 40, 167, 237, 102, 223, 106, 159, 197, 189, 215, 137, 36, 32, 22, 5,
};


internal f32
perlin_grad(s32 hash, f32 x, f32 y, f32 z)
{
	local_persist f32 basis[12][4] =
	{
		{ 1, 1, 0},
		{-1, 1, 0},
		{ 1,-1, 0},
		{-1,-1, 0},
		{ 1, 0, 1},
		{-1, 0, 1},
		{ 1, 0,-1},
		{-1, 0,-1},
		{ 0, 1, 1},
		{ 0,-1, 1},
		{ 0, 1,-1},
		{ 0,-1,-1},
	};

	local_persist u8 indices[64] =
	{
		0,1,2,3,4,5,6,7,8,9,10,11,
		0,9,1,11,
		0,1,2,3,4,5,6,7,8,9,10,11,
		0,1,2,3,4,5,6,7,8,9,10,11,
		0,1,2,3,4,5,6,7,8,9,10,11,
		0,1,2,3,4,5,6,7,8,9,10,11,
	};

	f32* grad = basis[indices[hash & 63]];
	return grad[0]*x + grad[1]*y + grad[2]*z;
}


inline f32
perlin3(f32 x, f32 y, f32 z, s32 x_wrap, s32 y_wrap, s32 z_wrap)
{
	u32 x_mask = (x_wrap-1) & 255;
	u32 y_mask = (y_wrap-1) & 255;
	u32 z_mask = (z_wrap-1) & 255;
	s32 px = (s32)math::floor(x);
	s32 py = (s32)math::floor(y);
	s32 pz = (s32)math::floor(z);
	s32 x0 = px & x_mask, x1 = (px+1) & x_mask;
	s32 y0 = py & y_mask, y1 = (py+1) & y_mask;
	s32 z0 = pz & z_mask, z1 = (pz+1) & z_mask;

#define GB__PERLIN_EASE(t) (((t*6-15)*t + 10) *t*t*t)
	x -= px; f32 u = GB__PERLIN_EASE(x);
	y -= py; f32 v = GB__PERLIN_EASE(y);
	z -= pz; f32 w = GB__PERLIN_EASE(z);
#undef GB__PERLIN_EASE

	s32 r0 = g_perlin_randtab[x0];
	s32 r1 = g_perlin_randtab[x1];

	s32 r00 = g_perlin_randtab[r0+y0];
	s32 r01 = g_perlin_randtab[r0+y1];
	s32 r10 = g_perlin_randtab[r1+y0];
	s32 r11 = g_perlin_randtab[r1+y1];

	f32 n000 = perlin_grad(g_perlin_randtab[r00+z0], x  , y  , z   );
	f32 n001 = perlin_grad(g_perlin_randtab[r00+z1], x  , y  , z-1 );
	f32 n010 = perlin_grad(g_perlin_randtab[r01+z0], x  , y-1, z   );
	f32 n011 = perlin_grad(g_perlin_randtab[r01+z1], x  , y-1, z-1 );
	f32 n100 = perlin_grad(g_perlin_randtab[r10+z0], x-1, y  , z   );
	f32 n101 = perlin_grad(g_perlin_randtab[r10+z1], x-1, y  , z-1 );
	f32 n110 = perlin_grad(g_perlin_randtab[r11+z0], x-1, y-1, z   );
	f32 n111 = perlin_grad(g_perlin_randtab[r11+z1], x-1, y-1, z-1 );

	f32 n00 = math::lerp(n000,n001,w);
	f32 n01 = math::lerp(n010,n011,w);
	f32 n10 = math::lerp(n100,n101,w);
	f32 n11 = math::lerp(n110,n111,w);

	f32 n0 = math::lerp(n00,n01,v);
	f32 n1 = math::lerp(n10,n11,v);

	return math::lerp(n0,n1,u);
}

} // namespace random
__GB_NAMESPACE_END

#endif // GB_MATH_IMPLEMENTATION
