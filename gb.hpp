// gb.hpp - v0.01 - public domain C++11 helper library - no warranty implied; use at your own risk
// (Experimental) A C++11 helper library without STL geared towards game development
//
// Version History:
//     0.01 - Initial Version
//
// LICENSE
//
//     This software is in the public domain. Where that dedication is not
//     recognized, you are granted a perpetual, irrevocable license to copy,
//     distribute, and modify this file as you see fit.
//
// WARNING
//
//     This library is highly experimental and features may not work as expected.
//     This also means that many functions are not documented.
//
#ifndef GB_INCLUDE_GB_HPP
#define GB_INCLUDE_GB_HPP

#if !defined(__cplusplus) && __cplusplus >= 201103L
#error This library is only for C++11 and above
#endif

// NOTE(bill): Because static means three different things in C/C++
//             Great Design(!)
#define global        static
#define internal      static
#define local_persist static

#if defined(_MSC_VER)
#define _ALLOW_KEYWORD_MACROS

	#if !defined(alignof) // Needed for MSVC 2013
	#define alignof(x) __alignof(x)
	#endif
#endif

////////////////////////////////
/// System OS                ///
////////////////////////////////
#define WIN32_LEAN_AND_MEAN 1

#if defined(_WIN32) || defined(_WIN64)
#define GB_SYSTEM_WINDOWS
#define NOMINMAX

#elif defined(__APPLE__) && defined(__MACH__)
#define GB_SYSTEM_OSX

#elif defined(__unix__)
#define GB_SYSTEM_UNIX

	#if defined(__linux__)
	#define GB_SYSTEM_LINUX
	#elif defined(__FreeBSD__) || defined(__FreeBSD_kernel__)
	#define GB_SYSTEM_FREEBSD
	#else
	#error This UNIX operating system is not supported by gb.hpp
	#endif
#else
#error This operating system is not supported by gb.hpp
#endif

////////////////////////////////
/// Environment Bit Size     ///
////////////////////////////////
#if defined(_WIN32) || defined(_WIN64)
	#if defined(_WIN64)
	#define GB_ARCH_64_BIT
	#else
	#define GB_ARCH_32_BIT
	#endif
#endif

// TODO(bill): Check if this KEPLER_ENVIRONMENT works on clang
#if defined(__GNUC__)
	#if defined(__x86_64__) || defined(__ppc64__)
	#define GB_ARCH_64_BIT
	#else
	#define GB_ARCH_32_BIT
	#endif
#endif

#define GB_IS_POWER_OF_TWO(x) ((x) != 0) && !((x) & ((x) - 1))


#ifndef NDEBUG
#define GB_ASSERT(x, ...) ((void)(gb__assert_handler((x), #x, __FILE__, __LINE__, ##__VA_ARGS__)))
#else
#define GB_ASSERT(x, ...) ((void)sizeof(x))
#endif

extern "C" void
gb__assert_handler(bool condition, const char* condition_str,
                   const char* filename, size_t line,
                   const char* error_text = nullptr, ...);

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef GB_SYSTEM_WINDOWS
#include <windows.h>
#else
#include <pthread.h>
#endif

namespace gb
{
////////////////////////////////
/// Types                    ///
////////////////////////////////

using u8  = uint8_t;
using s8  = int8_t;
using u16 = uint16_t;
using s16 = int16_t;
using u32 = uint32_t;
using s32 = int32_t;

#if defined(_MSC_VER)
using s64 = signed __int64;
using u64 = unsigned __int64;
#else
using s64 = int64_t;
using u64 = uint64_t;
#endif

using f32 = float;
using f64 = double;

#ifdef GB_B8_AS_BOOL
using b8 = bool;
#else
using b8 = s8;
#endif
using b32 = s32;

// NOTE(bill): (std::)size_t is not used not because it's a bad concept but on
// the platforms that I will be using:
// sizeof(size_t) == sizeof(usize) == sizeof(ssize)
// NOTE(bill): This also allows for a signed version of size_t which is similar
// to ptrdiff_t
// NOTE(bill): If (u)intptr is a better fit, please use that.
// NOTE(bill): Also, I hate the `_t` suffix
#if defined(GB_ARCH_64_BIT)
using ssize = s64;
using usize = u64;
#elif defined(GB_ARCH_32_BIT)
using ssize = s32;
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


////////////////////////////////
/// C++11 Move Semantics     ///
////////////////////////////////
template <typename T> struct Remove_Reference      { using Type = T; };
template <typename T> struct Remove_Reference<T&>  { using Type = T; };
template <typename T> struct Remove_Reference<T&&> { using Type = T; };

template <typename T>
inline T&&
forward(typename Remove_Reference<T>::Type& t)
{
	return static_cast<T &&>(t);
}

template <typename T>
inline T&&
forward(typename Remove_Reference<T>::Type&& t)
{
	return static_cast<T &&>(t);
}

template <typename T>
inline typename Remove_Reference<T>::Type&&
move(T&& t)
{
	return static_cast<typename Remove_Reference<T>::Type&&>(t);
}

////////////////////////////////
/// Defer                    ///
////////////////////////////////
namespace impl
{
template <typename Fn>
struct Defer
{
	Fn fn;

	Defer(Fn&& fn) : fn{forward<Fn>(fn)} {}
	~Defer() { fn(); };
};

template <typename Fn>
Defer<Fn>
defer_fn(Fn&& fn) { return Defer<Fn>(forward<Fn>(fn)); }
} // namespace impl
} // namespace gb

#define GB_DEFER_1(x, y) x##y
#define GB_DEFER_2(x, y) GB_DEFER_1(x, y)
#define GB_DEFER_3(x)    GB_DEFER_2(GB_DEFER_2(x, __COUNTER__), __LINE__)
#define defer(code) auto GB_DEFER_3(_defer_) = gb::impl::defer_fn([&](){code;})

namespace gb
{
////////////////////////////////
/// Memory                   ///
////////////////////////////////

struct Mutex
{
#ifdef GB_SYSTEM_WINDOWS
	HANDLE win32_mutex;
#else
	pthread_mutex_t posix_mutex;
#endif

	Mutex();
	~Mutex();
};

void lock_mutex(Mutex& mutex);
bool try_lock_mutex(Mutex& mutex);
void unlock_mutex(Mutex& mutex);

#define GB_DEFAULT_ALIGNMENT 4

inline void*
align_forward(void* ptr, usize align)
{
	GB_ASSERT(GB_IS_POWER_OF_TWO(align));

	uintptr p = (uintptr)ptr;

	const usize modulo = p % align;
	if (modulo)
		p += (uintptr)(align - modulo);

	return (void*)p;
}

struct Allocator
{
	Allocator() {}
	virtual ~Allocator() {}

	virtual void* alloc(usize size, usize align = GB_DEFAULT_ALIGNMENT) = 0;
	virtual void  dealloc(void* ptr) = 0;
	virtual ssize allocated_size(const void* ptr) = 0;
	virtual ssize total_allocated() = 0;

private:
	// Delete copying
	Allocator(const Allocator&) = delete;
	Allocator& operator=(const Allocator&) = delete;
};

inline void*
alloc(Allocator& a, usize size, usize align = GB_DEFAULT_ALIGNMENT)
{
	return a.alloc(size, align);
}

inline void
dealloc(Allocator& a, void* ptr)
{
	return a.dealloc(ptr);
}

template <typename T>
inline T*
alloc_struct(Allocator& a)
{
	return static_cast<T*>a.alloc(sizeof(T), alignof(T));
}


template <typename T>
inline T*
alloc_array(Allocator& a, usize count)
{
	return static_cast<T*>(alloc(a, count * sizeof(T), alignof(T)));
}

#define GB_HEAP_ALLOCATOR_HEADER_PAD_VALUE (usize)(-1)

struct Heap_Allocator : Allocator
{
	struct Header
	{
		ssize size;
	};

	Mutex mutex = Mutex{};
	ssize total_allocated_count = 0;
	ssize allocation_count      = 0;

	Heap_Allocator() = default;

	virtual ~Heap_Allocator();

	virtual void* alloc(usize size, usize align = GB_DEFAULT_ALIGNMENT);
	virtual void  dealloc(void* ptr);
	virtual ssize allocated_size(const void* ptr);
	virtual ssize total_allocated();
};


struct Arena_Allocator : Allocator
{
	ssize base_size;
	u8*   base;
	ssize total_allocated_count;
	ssize temp_count;

	virtual void* alloc(usize size, usize align = GB_DEFAULT_ALIGNMENT);
	virtual void  dealloc(void* ptr);
	virtual ssize allocated_size(const void* ptr);
	virtual ssize total_allocated();

	virtual usize get_alignment_offset(usize align = GB_DEFAULT_ALIGNMENT);
	virtual usize get_remaining_space(usize align = GB_DEFAULT_ALIGNMENT);
	void check();
};

inline void
init_arena_allocator(Arena_Allocator& arena, void* base, usize base_size)
{
	arena.base_size  = base_size;
	arena.base       = (u8*)base;
	arena.temp_count = 0;
	arena.total_allocated_count = 0;
}

struct Temporary_Arena_Memory
{
	Arena_Allocator* arena;
	ssize original_count;

	explicit Temporary_Arena_Memory(Arena_Allocator& arena_)
	: arena(&arena_)
	, original_count(arena_.total_allocated_count)
	{
	}

	~Temporary_Arena_Memory()
	{
		GB_ASSERT(arena->total_allocated() >= original_count);
		arena->total_allocated_count = original_count;
		GB_ASSERT(arena->temp_count > 0);
		arena->temp_count--;
	}
};

inline Temporary_Arena_Memory
make_temporary_arena_memory(Arena_Allocator& arena)
{
	return Temporary_Arena_Memory{arena};
}

////////////////////////////////
/// Array                    ///
////////////////////////////////

template <typename T>
struct Array
{
	Allocator* allocator;
	ssize count;
	ssize allocation;
	T*    data;

	virtual ~Array() { if (allocator) dealloc(*allocator, data); }

	const T& operator[](usize index) const { return data[index]; }
	      T& operator[](usize index)       { return data[index]; }
};

template <typename T> Array<T> make_array(Allocator& allocator, usize count = 0);
template <typename T> void     free_array(Array<T>& array);

template <typename T> void append_array(Array<T>& a, const T& item);
template <typename T> void append_array(Array<T>& a, const T* items, usize count);

template <typename T> void pop_back_array(Array<T>& a);

template <typename T> inline       T* begin(Array<T>& a)       { return a.data; }
template <typename T> inline const T* begin(const Array<T>& a) { return a.data; }

template <typename T> inline       T* end(Array<T>& a)         { return a.data + a.count; }
template <typename T> inline const T* end(const Array<T>& a)   { return a.data + a.count; }

template <typename T> void clear_array(Array<T>& a);
template <typename T> void resize_array(Array<T>& a, usize count);
template <typename T> void reserve_array(Array<T>& a, usize allocation);
template <typename T> void set_array_allocation(Array<T>& a, usize allocation);
template <typename T> void grow_array(Array<T>& a, usize min_allocation = 0);


template <typename T>
inline Array<T>
make_array(Allocator& allocator, usize count)
{
	Array<T> array = {};
	array.allocator = &allocator;
	if (count > 0)
	{
		array.data = alloc_array<T>(allocator, count);
		if (array.data)
		{
			array.count = array.allocation = count;
		}
	}

	return array;
}

template <typename T>
inline void
dealloc_array(Array<T>& array)
{
	if (array.allocator)
		dealloc(*array.allocator, array.data);
}

template <typename T>
inline void
append_array(Array<T>& a, const T& item)
{
	if (a.allocation < a.count + 1)
		grow_array(a);
	a.data[a.count++] = item;
}

template <typename T>
inline void
append_array(Array<T>& a, const T* items, usize count)
{
	if (a.allocation <= a.count + count)
		grow_array(a, a.count + count);

	memcpy(&a.data[a.count], items, count * sizeof(T));
	a.count += count;
}

template <typename T>
inline void
pop_back_array(Array<T>& a)
{
	GB_ASSERT(a.count > 0);

	a.count--;
}

template <typename T>
inline void
clear_array(Array<T>& a)
{
	resize_array(a, 0);
}

template <typename T>
inline void
resize_array(Array<T>& a, usize count)
{
	if (a.allocation < (ssize)count)
		grow_array(a, count);
	a.count = count;
}

template <typename T>
inline void
reserve_array(Array<T>& a, usize allocation)
{
	if (a.allocation < (ssize)allocation)
		set_array_allocation(a, allocation);
}

template <typename T>
inline void
set_array_allocation(Array<T>& a, usize allocation)
{
	if ((ssize)allocation == a.allocation)
		return;

	if ((ssize)allocation < a.count)
		resize_array(a, allocation);

	T* data = nullptr;
	if (allocation > 0)
	{
		data = alloc_array<T>(*a.allocator, allocation);
		memcpy(data, a.data, a.count * sizeof(T));
	}
	dealloc(*a.allocator, a.data);
	a.data = data;
	a.allocation = allocation;
}

template <typename T>
inline void
grow_array(Array<T>& a, usize min_allocation)
{
	usize allocation = 2 * a.allocation + 2;
	if (allocation < min_allocation)
		allocation = min_allocation;
	set_array_allocation(a, allocation);
}




////////////////////////////////
/// Math Types               ///
////////////////////////////////

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
		Vector2 xy;
		f32 data[3];
	};

	inline const f32& operator[](usize index) const { return data[index]; }
	inline       f32& operator[](usize index)       { return data[index]; }
};

struct Vector4
{
	union
	{
		struct { f32 x, y, z, w; };
		struct { Vector2 xy, zw; };
		Vector3 xyz;
		f32 data[4];
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
		f32 data[4];
	};
};


struct Matrix4
{
	union
	{
		struct { Vector4 x, y, z, w; };
		Vector4 column[4];
		f32     data[16];
	};

	inline const Vector4& operator[](usize index) const { return column[index]; }
	inline       Vector4& operator[](usize index)       { return column[index]; }
};


struct Euler_Angles
{
	// NOTE(bill): All angles in radians
	f32 pitch;
	f32 yaw;
	f32 roll;
};

extern const Vector2    VECTOR2_ZERO;
extern const Vector3    VECTOR3_ZERO;
extern const Vector4    VECTOR4_ZERO;
extern const Quaternion QUATERNION_IDENTITY;
extern const Matrix4    MATRIX4_IDENTITY;

////////////////////////////////
/// Math Type Op Overloads   ///
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

//////////////////////////////////
/// Math Functions & Constants ///
//////////////////////////////////

namespace math
{
extern const f32 EPSILON;
extern const f32 ZERO;
extern const f32 ONE;
extern const f32 THIRD;
extern const f32 TWO_THIRDS;
extern const f32 E;
extern const f32 PI;
extern const f32 TAU;
extern const f32 SQRT_2;
extern const f32 SQRT_3;


// Power
f32 sqrt(f32 x);
f32 pow(f32 x, f32 y);
f32 cbrt(f32 x);
f32 fast_inv_sqrt(f32 x);

// Trigonometric
f32 sin(f32 radians);
f32 cos(f32 radians);
f32 tan(f32 radians);

f32 asin(f32 x);
f32 acos(f32 x);
f32 atan(f32 x);
f32 atan2(f32 y, f32 x);

f32 radians(f32 degrees);
f32 degrees(f32 radians);

// Hyperbolic
f32 sinh(f32 x);
f32 cosh(f32 x);
f32 tanh(f32 x);

f32 asinh(f32 x);
f32 acosh(f32 x);
f32 atanh(f32 x);

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


// Vector2 functions
f32 dot(const Vector2& a, const Vector2& b);
f32 cross(const Vector2& a, const Vector2& b);

f32 magnitude(const Vector2& a);
Vector2 normalize(const Vector2& a);

Vector2 hadamard_product(const Vector2& a, const Vector2& b);

// Vector3 functions
f32 dot(const Vector3& a, const Vector3& b);
Vector3 cross(const Vector3& a, const Vector3& b);

f32 magnitude(const Vector3& a);
Vector3 normalize(const Vector3& a);

Vector3 hadamard_product(const Vector3& a, const Vector3& b);

// Vector4 functions
f32 dot(const Vector4& a, const Vector4& b);

f32 magnitude(const Vector4& a);
Vector4 normalize(const Vector4& a);

Vector4 hadamard_product(const Vector4& a, const Vector4& b);

// Quaternion functions
f32 dot(const Quaternion& a, const Quaternion& b);
Quaternion cross(const Quaternion& a, const Quaternion& b);

f32 magnitude(const Quaternion& a);
Quaternion normalize(const Quaternion& a);

Quaternion conjugate(const Quaternion& a);
Quaternion inverse(const Quaternion& a);

Vector3 operator*(const Quaternion& a, const Vector3& v); // Rotate v by a

f32 quaternion_angle(const Quaternion& a);
Vector3 quaternion_axis(const Quaternion& a);
Quaternion axis_angle(const Vector3& axis, f32 radians);

f32 quaternion_roll(const Quaternion& a);
f32 quaternion_pitch(const Quaternion& a);
f32 quaternion_yaw(const Quaternion& a);

Euler_Angles quaternion_to_euler_angles(const Quaternion& a);
Quaternion euler_angles_to_quaternion(const Euler_Angles& e,
                                      const Vector3& x_axis = {1, 0, 0},
                                      const Vector3& y_axis = {0, 1, 0},
                                      const Vector3& z_axis = {0, 0, 1});

// Matrix4 functions
Matrix4 transpose(const Matrix4& m);
f32 determinant(const Matrix4& m);

Matrix4 inverse(const Matrix4& m);

Matrix4 hadamard_product(const Matrix4& a, const Matrix4&b);

Matrix4 quaternion_to_matrix4(const Quaternion& a);
Quaternion matrix4_to_quaternion(const Matrix4& m);

Matrix4 translate(const Vector3& v);
Matrix4 rotate(const Vector3& v, f32 radians);
Matrix4 scale(const Vector3& v);
Matrix4 ortho(f32 left, f32 right, f32 bottom, f32 top);
Matrix4 ortho(f32 left, f32 right, f32 bottom, f32 top, f32 z_near, f32 z_far);
Matrix4 perspective(f32 fovy_radians, f32 aspect, f32 z_near, f32 z_far);
Matrix4 infinite_perspective(f32 fovy_radians, f32 aspect, f32 z_near);

Matrix4
look_at_matrix4(const Vector3& eye, const Vector3& center, const Vector3& up = {0, 1, 0});

Quaternion
look_at_quaternion(const Vector3& eye, const Vector3& center, const Vector3& up = {0, 1, 0});


} // namespace math
} // namespace gb
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
/// It's a long way to Tipperary
///
///
///
///
///
////////////////////////////////
/// Implemenation            ///
////////////////////////////////
#ifdef GB_IMPLEMENTATION

#include <float.h>
#include <math.h>
#include <stdarg.h>

inline void
gb__assert_handler(bool condition, const char* condition_str,
                    const char* filename, size_t line,
                    const char* error_text, ...)
{
	if (condition)
		return;

	fprintf(stderr, "ASSERT! %s(%d): %s", filename, line, condition_str);
	if (error_text)
	{
		fprintf(stderr, " - ");

		va_list args;
		va_start(args, error_text);
		vfprintf(stderr, error_text, args);
		va_end(args);
	}
	fprintf(stderr, "\n");

	*(int*)0 = 0; // TODO(bill): Use a better way to assert
}



namespace gb
{
////////////////////////////////
/// Memory                   ///
////////////////////////////////

Mutex::Mutex()
{
#ifdef GB_SYSTEM_WINDOWS
	win32_mutex = CreateMutex(0, 0, 0);
#else
	pthread_mutex_init(&posix_mutex, nullptr);
#endif
}

Mutex::~Mutex()
{
#ifdef GB_SYSTEM_WINDOWS
	CloseHandle(win32_mutex);
#else
	pthread_mutex_destroy(&posix_mutex);
#endif
}

void lock_mutex(Mutex& mutex)
{
#ifdef GB_SYSTEM_WINDOWS
	WaitForSingleObject(mutex.win32_mutex, INFINITE);
#else
	pthread_mutex_lock(&mutex.posix_mutex);
#endif
}

bool try_lock_mutex(Mutex& mutex)
{
#ifdef GB_SYSTEM_WINDOWS
	return WaitForSingleObject(mutex.win32_mutex, 0) == WAIT_OBJECT_0;
#else
	return pthread_mutex_trylock(&mutex.posix_mutex) == 0;
#endif
}


void unlock_mutex(Mutex& mutex)
{
#ifdef GB_SYSTEM_WINDOWS
	ReleaseMutex(mutex.win32_mutex);
#else
	pthread_mutex_unlock(&mutex.posix_mutex);
#endif
}


Heap_Allocator::~Heap_Allocator()
{
	GB_ASSERT(allocation_count == 0 && total_allocated() == 0,
	          "Heap Allocator: allocation count = %lld; total allocated = %lld",
	          allocation_count, total_allocated());
}

void*
Heap_Allocator::alloc(usize size, usize align)
{
	lock_mutex(mutex);
	defer(unlock_mutex(mutex));

	const usize total = size + align + sizeof(Header);
	Header* h = (Header*)malloc(total);
	h->size   = total;

	void* data = align_forward(h + 1, align);
	{ // Pad header
		usize* ptr = (usize*)(h+1);

		while (ptr != data)
			*ptr++ = GB_HEAP_ALLOCATOR_HEADER_PAD_VALUE;
	}
	total_allocated_count += total;
	allocation_count++;

	return data;
}

void
Heap_Allocator::dealloc(void* ptr)
{
	if (!ptr)
		return;

	lock_mutex(mutex);
	defer(unlock_mutex(mutex));

	const usize* data = ((usize*)ptr) - 1;

	while (*data == GB_HEAP_ALLOCATOR_HEADER_PAD_VALUE)
		data--;

	Header* h = (Header*)data;

	total_allocated_count -= h->size;
	allocation_count--;

	free(h);
}

ssize
Heap_Allocator::allocated_size(const void* ptr)
{
	lock_mutex(mutex);
	defer(unlock_mutex(mutex));

	const usize* data = (usize*)ptr - 1;

	while (*data == GB_HEAP_ALLOCATOR_HEADER_PAD_VALUE)
		data--;

	return ((Header*)ptr)->size;
}

ssize
Heap_Allocator::total_allocated()
{
	return total_allocated_count;
}


void* Arena_Allocator::alloc(usize size_init, usize align)
{
	usize size = size_init;

	usize alignment_offset = get_alignment_offset(align);
	size += alignment_offset;

	GB_ASSERT(size >= size_init);
	GB_ASSERT(total_allocated_count + size <= (usize)base_size);

	void* ptr = base + total_allocated_count + alignment_offset;
	total_allocated_count += size;

	return ptr;
}

ssize Arena_Allocator::allocated_size(const void* ptr)
{
	return -1;
}

ssize Arena_Allocator::total_allocated()
{
	return total_allocated_count;
}

usize Arena_Allocator::get_alignment_offset(usize align)
{
	usize offset = 0;

	usize result_pointer = (usize)((uintptr)base + total_allocated_count);
	usize alignment_mask = align - 1;
	if (result_pointer & alignment_mask)
		offset = align - (result_pointer & alignment_mask);

	return offset;
}

usize Arena_Allocator::get_remaining_space(usize align)
{
	return base_size - (total_allocated_count + get_alignment_offset(align));
}

void Arena_Allocator::check()
{
	GB_ASSERT(temp_count == 0);
}




////////////////////////////////
/// Math                     ///
////////////////////////////////


const Vector2    VECTOR2_ZERO        = {0, 0};
const Vector3    VECTOR3_ZERO        = {0, 0, 0};
const Vector4    VECTOR4_ZERO        = {0, 0, 0, 0};
const Quaternion QUATERNION_IDENTITY = {0, 0, 0, 1};
const Matrix4    MATRIX4_IDENTITY    = {1, 0, 0, 0,
                                        0, 1, 0, 0,
                                        0, 0, 1, 0,
                                        0, 0, 0, 1};


////////////////////////////////
/// Math Type Op Overloads   ///
////////////////////////////////

// Vector2 Operators
bool operator==(const Vector2& a, const Vector2& b)
{
	return (a.x == b.x) && (a.y == b.y);
}

bool operator!=(const Vector2& a, const Vector2& b)
{
	return !operator==(a, b);
}

Vector2 operator-(const Vector2& a)
{
	return {-a.x, -a.y};
}

Vector2 operator+(const Vector2& a, const Vector2& b)
{
	return {a.x + b.x, a.y + b.y};
}

Vector2 operator-(const Vector2& a, const Vector2& b)
{
	return {a.x - b.x, a.y - b.y};
}

Vector2 operator*(const Vector2& a, f32 scalar)
{
	return {a.x * scalar, a.y * scalar};
}

Vector2 operator*(f32 scalar, const Vector2& a)
{
	return {a.x * scalar, a.y * scalar};
}

Vector2 operator/(const Vector2& a, f32 scalar)
{
	return {a.x / scalar, a.y / scalar};
}

Vector2 operator*(const Vector2& a, const Vector2& b) // Hadamard Product
{
	return {a.x * b.x, a.y * b.y};
}

Vector2 operator/(const Vector2& a, const Vector2& b) // Hadamard Product
{
	return {a.x / b.x, a.y / b.y};
}

Vector2& operator+=(Vector2& a, const Vector2& b)
{
	a.x += b.x;
	a.y += b.y;

	return a;
}

Vector2& operator-=(Vector2& a, const Vector2& b)
{
	a.x -= b.x;
	a.y -= b.y;

	return a;
}

Vector2& operator*=(Vector2& a, f32 scalar)
{
	a.x *= scalar;
	a.y *= scalar;

	return a;
}

Vector2& operator/=(Vector2& a, f32 scalar)
{
	a.x /= scalar;
	a.y /= scalar;

	return a;
}

// Vector3 Operators
bool operator==(const Vector3& a, const Vector3& b)
{
	return (a.x == b.x) && (a.y == b.y) && (a.z == b.z);
}

bool operator!=(const Vector3& a, const Vector3& b)
{
	return !operator==(a, b);
}

Vector3 operator-(const Vector3& a)
{
	return {-a.x, -a.y, -a.z};
}

Vector3 operator+(const Vector3& a, const Vector3& b)
{
	return {a.x + b.x, a.y + b.y, a.z + b.z};
}

Vector3 operator-(const Vector3& a, const Vector3& b)
{
	return {a.x - b.x, a.y - b.y, a.z - b.z};
}

Vector3 operator*(const Vector3& a, f32 scalar)
{
	return {a.x * scalar, a.y * scalar, a.z * scalar};
}

Vector3 operator*(f32 scalar, const Vector3& a)
{
	return {a.x * scalar, a.y * scalar, a.z * scalar};
}

Vector3 operator/(const Vector3& a, f32 scalar)
{
	return {a.x / scalar, a.y / scalar, a.z / scalar};
}

Vector3 operator*(const Vector3& a, const Vector3& b) // Hadamard Product
{
	return {a.x * b.x, a.y * b.y, a.z * b.z};
}

Vector3 operator/(const Vector3& a, const Vector3& b) // Hadamard Product
{
	return {a.x / b.x, a.y / b.y, a.z / b.z};
}

Vector3& operator+=(Vector3& a, const Vector3& b)
{
	a.x += b.x;
	a.y += b.y;
	a.z += b.z;

	return a;
}

Vector3& operator-=(Vector3& a, const Vector3& b)
{
	a.x -= b.x;
	a.y -= b.y;
	a.z -= b.z;

	return a;
}

Vector3& operator*=(Vector3& a, f32 scalar)
{
	a.x *= scalar;
	a.y *= scalar;
	a.z *= scalar;

	return a;
}

Vector3& operator/=(Vector3& a, f32 scalar)
{
	a.x /= scalar;
	a.y /= scalar;
	a.z /= scalar;

	return a;
}

// Vector4 Operators
bool operator==(const Vector4& a, const Vector4& b)
{
	return (a.x == b.x) && (a.y == b.y) && (a.z == b.z) && (a.w == b.w);
}

bool operator!=(const Vector4& a, const Vector4& b)
{
	return !operator==(a, b);
}

Vector4 operator-(const Vector4& a)
{
	return {-a.x, -a.y, -a.z, -a.w};
}

Vector4 operator+(const Vector4& a, const Vector4& b)
{
	return {a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w};
}

Vector4 operator-(const Vector4& a, const Vector4& b)
{
	return {a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w};
}

Vector4 operator*(const Vector4& a, f32 scalar)
{
	return {a.x * scalar, a.y * scalar, a.z * scalar, a.w * scalar};
}

Vector4 operator*(f32 scalar, const Vector4& a)
{
	return {a.x * scalar, a.y * scalar, a.z * scalar, a.w * scalar};
}

Vector4 operator/(const Vector4& a, f32 scalar)
{
	return {a.x / scalar, a.y / scalar, a.z / scalar, a.w / scalar};
}

Vector4 operator*(const Vector4& a, const Vector4& b) // Hadamard Product
{
	return {a.x * b.x, a.y * b.y, a.z * b.z, a.w * b.w};
}

Vector4 operator/(const Vector4& a, const Vector4& b) // Hadamard Product
{
	return {a.x / b.x, a.y / b.y, a.z / b.z, a.w / b.w};
}

Vector4& operator+=(Vector4& a, const Vector4& b)
{
	a.x += b.x;
	a.y += b.y;
	a.z += b.z;
	a.w += b.w;

	return a;
}

Vector4& operator-=(Vector4& a, const Vector4& b)
{
	a.x -= b.x;
	a.y -= b.y;
	a.z -= b.z;
	a.w -= b.w;

	return a;
}

Vector4& operator*=(Vector4& a, f32 scalar)
{
	a.x *= scalar;
	a.y *= scalar;
	a.z *= scalar;
	a.w *= scalar;

	return a;
}

Vector4& operator/=(Vector4& a, f32 scalar)
{
	a.x /= scalar;
	a.y /= scalar;
	a.z /= scalar;
	a.w /= scalar;

	return a;
}

// Quaternion Operators
bool operator==(const Quaternion& a, const Quaternion& b)
{
	return (a.x == b.x) && (a.y == b.y) && (a.z == b.z) && (a.w == b.w);
}

bool operator!=(const Quaternion& a, const Quaternion& b)
{
	return !operator==(a, b);
}

Quaternion operator-(const Quaternion& a)
{
	return {-a.x, -a.y, -a.z, -a.w};
	return {-a.x, -a.y, -a.z, -a.w};
}

Quaternion operator+(const Quaternion& a, const Quaternion& b)
{
	return {a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w};
}

Quaternion operator-(const Quaternion& a, const Quaternion& b)
{
	return {a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w};

}

Quaternion operator*(const Quaternion& a, const Quaternion& b)
{
	Quaternion q = {};

	q.x = a.w * b.x + a.x * b.w + a.y * b.z - a.z * b.y;
	q.y = a.w * b.y - a.x * b.z + a.y * b.w + a.z * b.x;
	q.z = a.w * b.z + a.x * b.y - a.y * b.x + a.z * b.w;
	q.w = a.w * b.w - a.x * b.x - a.y * b.y - a.z * b.z;

	return q;
}

Quaternion operator*(const Quaternion& a, f32 s)
{
	return {a.x * s, a.y * s, a.z * s, a.w * s};
}

Quaternion operator*(f32 s, const Quaternion& a)
{
	return {a.x * s, a.y * s, a.z * s, a.w * s};
}

Quaternion operator/(const Quaternion& a, f32 s)
{
	return {a.x / s, a.y / s, a.z / s, a.w / s};
}

// Matrix4 Operators
bool operator==(const Matrix4& a, const Matrix4& b)
{
	for (usize i = 0; i < 4; i++)
	{
		if (a[i] != b[i])
			return false;
	}
	return true;
}

bool operator!=(const Matrix4& a, const Matrix4& b)
{
	return !operator==(a, b);
}

Matrix4 operator+(const Matrix4& a, const Matrix4& b)
{
	Matrix4 mat;
	for (usize i = 0; i < 4; i++)
		mat[i] = a[i] + b[i];
	return mat;
}

Matrix4 operator-(const Matrix4& a, const Matrix4& b)
{
	Matrix4 mat;
	for (usize i = 0; i < 4; i++)
		mat[i] = a[i] - b[i];
	return mat;
}

Matrix4 operator*(const Matrix4& a, const Matrix4& b)
{
	Matrix4 result;
	result[0] = a[0] * b[0][0] + a[1] * b[0][1] + a[2] * b[0][2] + a[3] * b[0][3];
	result[1] = a[0] * b[1][0] + a[1] * b[1][1] + a[2] * b[1][2] + a[3] * b[1][3];
	result[2] = a[0] * b[2][0] + a[1] * b[2][1] + a[2] * b[2][2] + a[3] * b[2][3];
	result[3] = a[0] * b[3][0] + a[1] * b[3][1] + a[2] * b[3][2] + a[3] * b[3][3];
	return result;
}

Vector4 operator*(const Matrix4& a, const Vector4& v)
{
	Vector4 mul0 = a[0] * v[0];
	Vector4 mul1 = a[1] * v[1];
	Vector4 mul2 = a[2] * v[2];
	Vector4 mul3 = a[3] * v[3];

	Vector4 add0 = mul0 + mul1;
	Vector4 add1 = mul2 + mul3;

	return add0 + add1;
}

Matrix4 operator*(const Matrix4& a, f32 scalar)
{
	Matrix4 mat;
	for (usize i = 0; i < 4; i++)
		mat[i] = a[i] * scalar;
	return mat;
}

Matrix4 operator*(f32 scalar, const Matrix4& a)
{
	Matrix4 mat;
	for (usize i = 0; i < 4; i++)
		mat[i] = a[i] * scalar;
	return mat;
}

Matrix4 operator/(const Matrix4& a, f32 scalar)
{
	Matrix4 mat;
	for (usize i = 0; i < 4; i++)
		mat[i] = a[i] / scalar;
	return mat;
}

Matrix4& operator+=(Matrix4& a, const Matrix4& b)
{
	return (a = a + b);
}

Matrix4& operator-=(Matrix4& a, const Matrix4& b)
{
	return (a = a - b);
}

Matrix4& operator*=(Matrix4& a, const Matrix4& b)
{
	return (a = a * b);
}

////////////////////////////////
/// Math Functions           ///
////////////////////////////////


namespace math
{
const f32 EPSILON    = FLT_EPSILON;
const f32 ZERO       = 0.0f;
const f32 ONE        = 1.0f;
const f32 THIRD      = 0.33333333f;
const f32 TWO_THIRDS = 0.66666667f;
const f32 E          = 2.718281828f;
const f32 PI         = 3.141592654f;
const f32 TAU        = 6.283185307f;
const f32 SQRT_2     = 1.414213562f;
const f32 SQRT_3     = 1.732050808f;

// Power
inline f32 sqrt(f32 x) { return ::sqrtf(x); }
inline f32 pow(f32 x, f32 y) { return (f32)::powf(x, y); }
inline f32 cbrt(f32 x) { return (f32)::cbrtf(x); }

inline f32 fast_inv_sqrt(f32 x)
{
	const f32 three_halfs = 1.5f;

	f32 x2 = x * 0.5f;
	f32 y  = x;
	u32 i  = *(u32*)&y;                       // Evil floating point bit level hacking
	//	i = 0x5f3759df - (i >> 1);            // What the fuck? Old
	i = 0x5f375a86 - (i >> 1);                // What the fuck? Improved!
	y = *(f32*)&i;
	y = y * (three_halfs - (x2 * y * y));     // 1st iteration
	//	y = y * (three_halfs - (x2 * y * y)); // 2nd iteration, this can be removed

	return y;
}

// Trigonometric
inline f32 sin(f32 radians) { return ::sinf(radians); }
inline f32 cos(f32 radians) { return ::cosf(radians); }
inline f32 tan(f32 radians) { return ::tanf(radians); }

inline f32 asin(f32 x)         { return ::asinf(x); }
inline f32 acos(f32 x)         { return ::acosf(x); }
inline f32 atan(f32 x)         { return ::atanf(x); }
inline f32 atan2(f32 y, f32 x) { return ::atan2f(y, x); }

inline f32 radians(f32 degrees) { return TAU * degrees / 360.0f; }
inline f32 degrees(f32 radians) { return 360.0f * radians / TAU; }

// Hyperbolic
inline f32 sinh(f32 x) { return ::sinhf(x); }
inline f32 cosh(f32 x) { return ::coshf(x); }
inline f32 tanh(f32 x) { return ::tanhf(x); }

inline f32 asinh(f32 x) { return ::asinhf(x); }
inline f32 acosh(f32 x) { return ::acoshf(x); }
inline f32 atanh(f32 x) { return ::atanhf(x); }

// Rounding
inline f32 ceil(f32 x)       { return ::ceilf(x); }
inline f32 floor(f32 x)      { return ::floorf(x); }
inline f32 mod(f32 x, f32 y) { return ::fmodf(x, y); }
inline f32 truncate(f32 x)   { return ::truncf(x); }
inline f32 round(f32 x)      { return ::roundf(x); }

inline s32 sign(s32 x) { return x >= 0 ? +1 : -1; }
inline s64 sign(s64 x) { return x >= 0 ? +1 : -1; }
inline f32 sign(f32 x) { return x >= 0 ? +1 : -1; }

// Other
inline f32 abs(f32 x)
{
	u32 i = reinterpret_cast<const u32&>(x);
	i &= 0x7FFFFFFFul;
	return reinterpret_cast<const f32&>(i);
}

inline s8 abs(s8 x)
{
u8 i = reinterpret_cast<const u8&>(x);
	i &= 0x7Fu;
	return reinterpret_cast<const s8&>(i);
}

inline s16 abs(s16 x)
{
	u16 i = reinterpret_cast<const u16&>(x);
	i &= 0x7FFFu;
	return reinterpret_cast<const s16&>(i);
}

inline s32 abs(s32 x)
{
	u32 i = reinterpret_cast<const u32&>(x);
	i &= 0x7FFFFFFFul;
	return reinterpret_cast<const s32&>(i);
}

inline s64 abs(s64 x)
{
	u64 i = reinterpret_cast<const u64&>(x);
	i &= 0x7FFFFFFFFFFFFFFFull;
	return reinterpret_cast<const s64&>(i);
}




// Vector2 functions
f32 dot(const Vector2& a, const Vector2& b)
{
	return a.x * b.x + a.y * b.y;
}

f32 cross(const Vector2& a, const Vector2& b)
{
	return a.x * b.y - a.y * b.x;
}

f32 magnitude(const Vector2& a)
{
	return math::sqrt(math::dot(a, a));
}

Vector2 normalize(const Vector2& a)
{
	f32 m = 1.0f / magnitude(a);
	return a * m;
}

Vector2 hadamard_product(const Vector2& a, const Vector2& b)
{
	return {a.x * b.x, a.y * b.y};
}

// Vector3 functions
f32 dot(const Vector3& a, const Vector3& b)
{
	return a.x * b.x + a.y * b.y + a.z * b.z;
}

Vector3 cross(const Vector3& a, const Vector3& b)
{
	return {
	    a.y * b.z - b.y * a.z, // x
	    a.z * b.x - b.z * a.x, // y
	    a.x * b.y - b.x * a.y  // z
	};
}

f32 magnitude(const Vector3& a)
{
	return math::sqrt(math::dot(a, a));
}

Vector3 normalize(const Vector3& a)
{
	f32 m = 1.0f / magnitude(a);
	return a * m;
}

Vector3 hadamard_product(const Vector3& a, const Vector3& b)
{
	return {a.x * b.x, a.y * b.y, a.z * b.z};
}

// Vector4 functions
f32 dot(const Vector4& a, const Vector4& b)
{
	return a.x*b.x + a.y*b.y + a.z*b.z + a.w*b.w;
}

f32 magnitude(const Vector4& a)
{
	return math::sqrt(math::dot(a, a));
}

Vector4 normalize(const Vector4& a)
{
	f32 m = 1.0f / magnitude(a);
	return a * m;
}

Vector4 hadamard_product(const Vector4& a, const Vector4& b)
{
	return {a.x * b.x, a.y * b.y, a.z * b.z, a.w * b.w};
}

// Quaternion functions
f32 dot(const Quaternion& a, const Quaternion& b)
{
	return math::dot(a.xyz, b.xyz) + a.w*b.w;
}

Quaternion cross(const Quaternion& a, const Quaternion& b)
{
	return {a.w * b.x + a.x * b.w + a.y * b.z - a.z * b.y,
            a.w * b.y + a.y * b.w + a.z * b.x - a.x * b.z,
            a.w * b.z + a.z * b.w + a.x * b.y - a.y * b.x,
            a.w * b.w - a.x * b.x - a.y * b.y - a.z * b.z};
}

f32 magnitude(const Quaternion& a)
{
	return math::sqrt(math::dot(a, a));
}

Quaternion normalize(const Quaternion& a)
{
	f32 m = 1.0f / magnitude(a);
	return a * m;
}

Quaternion conjugate(const Quaternion& a)
{
	return {-a.x, -a.y, -a.z, a.w};
}

Quaternion inverse(const Quaternion& a)
{
	f32 m = 1.0f / dot(a, a);
	return math::conjugate(a) * m;
}

Vector3 operator*(const Quaternion& a, const Vector3& v) // Rotate v by q
{
	// return (q * Quaternion(v, 0) * conjugate(q)).xyz; // More Expensive
	const Vector3 t = 2.0f * cross(a.xyz, v);
	return (v + a.w * t + cross(a.xyz, t));
}

f32 quaternion_angle(const Quaternion& a)
{
	return 2.0f * math::acos(a.w);
}

Vector3 quaternion_axis(const Quaternion& a)
{
	f32 s2 = 1.0f - a.w * a.w;

	if (s2 <= 0.0f)
		return {0, 0, 1};

	f32 invs2 = 1.0f / math::sqrt(s2);

	return a.xyz * invs2;
}

Quaternion axis_angle(const Vector3& axis, f32 radians)
{
	Vector3 a = math::normalize(axis);
	f32 s = math::sin(0.5f * radians);

	Quaternion q;
	q.xyz = a * s;
	q.w = math::cos(0.5f * radians);

	return q;
}

f32 quaternion_roll(const Quaternion& a)
{
	return math::atan2(2.0f * a.x * a.y + a.z * a.w,
	                   a.x * a.x + a.w * a.w - a.y * a.y - a.z * a.z);
}

f32 quaternion_pitch(const Quaternion& a)
{
	return math::atan2(2.0f * a.y * a.z + a.w * a.x,
	                   a.w * a.w - a.x * a.x - a.y * a.y + a.z * a.z);
}

f32 quaternion_yaw(const Quaternion& a)
{
	return math::asin(-2.0f * (a.x * a.z - a.w * a.y));

}

Euler_Angles quaternion_to_euler_angles(const Quaternion& a)
{
	return {quaternion_pitch(a), quaternion_yaw(a), quaternion_roll(a)};
}

Quaternion euler_angles_to_quaternion(const Euler_Angles& e,
                                      const Vector3& x_axis,
                                      const Vector3& y_axis,
                                      const Vector3& z_axis)
{
	Quaternion p = axis_angle(x_axis, e.pitch);
	Quaternion y = axis_angle(y_axis, e.yaw);
	Quaternion r = axis_angle(z_axis, e.roll);

	return y * p * r;
}


// Matrix4 functions
Matrix4 transpose(const Matrix4& m)
{
	Matrix4 result;

	for (usize i = 0; i < 4; i++)
	{
		for (usize j = 0; j < 4; j++)
			result[i][j] = m[j][i];
	}
	return result;
}

f32 determinant(const Matrix4& m)
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

Matrix4 inverse(const Matrix4& m)
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

Matrix4 hadamard_product(const Matrix4& a, const Matrix4& b)
{
	Matrix4 result;

	for (usize i = 0; i < 4; i++)
		result[i] = a[i] * b[i];

	return result;
}

Matrix4 quaternion_to_matrix4(const Quaternion& q)
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
	mat[0][1] = 2.0f * (xy + wz);
	mat[0][2] = 2.0f * (xz - wy);

	mat[1][0] = 2.0f * (xy - wz);
	mat[1][1] = 1.0f - 2.0f * (xx + zz);
	mat[1][2] = 2.0f * (yz + wx);

	mat[2][0] = 2.0f * (xz + wy);
	mat[2][1] = 2.0f * (yz - wx);
	mat[2][2] = 1.0f - 2.0f * (xx + yy);

	return mat;
}

Quaternion matrix4_to_quaternion(const Matrix4& m)
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
		biggestIndex             = 1;
	}
	if (four_y_squared_minus_1 > four_biggest_squared_minus_1)
	{
		four_biggest_squared_minus_1 = four_y_squared_minus_1;
		biggestIndex             = 2;
	}
	if (four_z_squared_minus_1 > four_biggest_squared_minus_1)
	{
		four_biggest_squared_minus_1 = four_z_squared_minus_1;
		biggestIndex             = 3;
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


Matrix4 translate(const Vector3& v)
{
	Matrix4 result = MATRIX4_IDENTITY;
	result[3].xyz = v;
	result[3].w = 1;
	return result;
}

Matrix4 rotate(const Vector3& v, f32 radians)
{
	const f32 c = math::cos(radians);
	const f32 s = math::sin(radians);

	const Vector3 axis = math::normalize(v);
	const Vector3 t    = (1.0f - c) * axis;

	Matrix4 rot = MATRIX4_IDENTITY;
	rot[0][0]   = c + t.x * axis.x;
	rot[0][1]   = 0 + t.x * axis.y + s * axis.z;
	rot[0][2]   = 0 + t.x * axis.z - s * axis.y;
	rot[0][3]   = 0;

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

Matrix4 scale(const Vector3& v)
{
	return { v.x,   0,   0, 0,
	           0, v.y,   0, 0,
	           0,   0, v.z, 0,
	           0,   0,   0, 1 };
}

Matrix4 ortho(f32 left, f32 right, f32 bottom, f32 top)
{
	Matrix4 result = MATRIX4_IDENTITY;

	result[0][0] = 2.0f / (right - left);
	result[1][1] = 2.0f / (top - bottom);
	result[2][2] = -1.0f;
	result[3][1] = -(right + left) / (right - left);
	result[3][1] = -(top + bottom) / (top - bottom);

	return result;
}

Matrix4 ortho(f32 left, f32 right, f32 bottom, f32 top, f32 z_near, f32 z_far)
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

Matrix4 perspective(f32 fovy_radians, f32 aspect, f32 z_near, f32 z_far)
{
	GB_ASSERT(math::abs(aspect) > 0.0f,
	          "math::perspective `fovy_radians` is %f", fovy_radians);

	f32 tan_half_fovy = math::tan(0.5f * fovy_radians);

	Matrix4 result = {};
	result[0][0]   = 1.0f / (aspect * tan_half_fovy);
	result[1][1]   = 1.0f / (tan_half_fovy);
	result[2][2]   = -(z_far + z_near) / (z_far - z_near);
	result[2][3]   = -1.0f;
	result[3][2]   = -2.0f * z_far * z_near / (z_far - z_near);

	return result;
}

Matrix4 infinite_perspective(f32 fovy_radians, f32 aspect, f32 z_near)
{
	f32 range  = math::tan(0.5f * fovy_radians) * z_near;
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


Matrix4
look_at_matrix4(const Vector3& eye, const Vector3& center, const Vector3& up)
{
	const Vector3 f = math::normalize(center - eye);
	const Vector3 s = math::normalize(math::cross(f, up));
	const Vector3 u = math::cross(s, f);

	Matrix4 result = MATRIX4_IDENTITY;
	result[0][0]   = +s.x;
	result[1][0]   = +s.y;
	result[2][0]   = +s.z;

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


Quaternion
look_at_quaternion(const Vector3& eye, const Vector3& center, const Vector3& up)
{
	const f32 similar = 0.001f;

	if (magnitude(center - eye) < similar)
		return QUATERNION_IDENTITY; // You cannot look at where you are!

	// TODO(bill): Implement using just quaternions
	return matrix4_to_quaternion(look_at_matrix4(eye, center, up));
}




} // namespace math
} // namespace gb

#endif // GB_IMPLEMENTATION
