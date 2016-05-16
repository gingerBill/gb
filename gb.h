/* gb.h - v0.15c - Ginger Bill's C Helper Library - public domain
                 - no warranty implied; use at your own risk

	This is a single header file with a bunch of useful stuff
	to replace the C/C++ standard library

===========================================================================
	YOU MUST

		#define GB_IMPLEMENTATION

	in EXACTLY _one_ C or C++ file that includes this header, BEFORE the
	include like this:

		#define GB_IMPLEMENTATION
		#include "gb.h"

	All other files should just #include "gb.h" without #define


	For the Platform Layer

		#define GB_PLATFORM

	BEFORE the include like this:

		#define GB_PLATFORM
		#include "gb.h"

===========================================================================

Conventions used:
	gbTypesAreLikeThis (None core types)
	gb_functions_and_variables_like_this
	Prefer C90 Comments
	Never use _t suffix for types (I think they are stupid...)


Version History:
	0.15c - Linux Experimental Support (DON'T USE IT)
	0.15b - C90 Support
	0.15a - gb_atomic(32|64)_spin_(lock|unlock)
	0.15  - Recursive "Mutex"; Key States; gbRandom
	0.14  - Better File Handling and better printf (WIN32 Only)
	0.13  - Highly experimental platform layer (WIN32 Only)
	0.12b - Fix minor file bugs
	0.12a - Compile as C++
	0.12  - New File Handing System! No stdio or stdlib! (WIN32 Only)
	0.11a - Add string precision and width (experimental)
	0.11  - Started making stdio & stdlib optional (Not tested much)
	0.10c - Fix gb_endian_swap32()
	0.10b - Probable timing bug for gb_time_now()
	0.10a - Work on multiple compilers
	0.10  - Scratch Memory Allocator
	0.09a - Faster Mutex and the Free List is slightly improved
	0.09  - Basic Virtual Memory System and Dreadful Free List allocator
	0.08a - Fix *_appendv bug
	0.08  - Huge Overhaul!
	0.07a - Fix alignment in gb_heap_allocator_proc
	0.07  - Hash Table and Hashing Functions
	0.06c - Better Documentation
	0.06b - OS X Support
	0.06a - Linux Support
	0.06  - Windows GCC Support and MSVC x86 Support
	0.05b - Formatting
	0.05a - Minor function name changes
	0.05  - Radix Sort for unsigned integers (TODO: Other primitives)
	0.04  - Better UTF support and search/sort procs
	0.03  - Completely change procedure naming convention
	0.02a - Bug fixes
	0.02  - Change naming convention and gbArray(Type)
	0.01  - Initial Version

LICENSE
	This software is dual-licensed to the public domain and under the following
	license: you are granted a perpetual, irrevocable license to copy, modify,
	publish, and distribute this file as you see fit.

WARNING
	- This library is _slightly_ experimental and features may not work as expected.
	- This also means that many functions are not documented.

CREDITS
	Written by Ginger Bill

TODOS
	- Remove CRT dependency for people who want that
		- But do I really?
	- Older compiler support?
		- How old do you wanna go?
	- File handling
		- All files to be UTF-8 (even on windows)
	- Better Virtual Memory handling
	- Generic Heap Allocator (tcmalloc/dlmalloc/?)
	- Fixed Heap Allocator
	- Better UTF support and conversion
	- Free List, best fit rather than first fit
	- More date & time functions
	- Platform Layer?

*/


#ifndef GB_INCLUDE_GB_H
#define GB_INCLUDE_GB_H

#if defined(__cplusplus)
extern "C" {
#endif


#ifndef GB_EXTERN
	#if defined(__cplusplus)
		#define GB_EXTERN extern "C"
	#else
		#define GB_EXTERN extern
	#endif
#endif

#ifndef GB_DLL_EXPORT
#define GB_DLL_EXPORT GB_EXTERN __declspec(dllexport)
#endif

#ifndef GB_DLL_IMPORT
#define GB_DLL_IMPORT GB_EXTERN __declspec(dllimport)
#endif


/* NOTE(bill): Redefine for DLL, etc. */
#ifndef GB_DEF
	#ifdef GB_STATIC
		#define GB_DEF static
	#else
		#define GB_DEF extern
	#endif
#endif

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

/* TODO(bill): Check if this works on clang */
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

#ifndef GB_EDIAN_ORDER
#define GB_EDIAN_ORDER
	#define GB_IS_BIG_EDIAN    (!*(u8*)&(u16){1})
	#define GB_IS_LITTLE_EDIAN (!GB_IS_BIG_EDIAN)
#endif


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
		#error This UNIX operating system is not supported
	#endif
#else
	#error This operating system is not supported
#endif


#ifndef GB_STATIC_ASSERT
	#define GB_STATIC_ASSERT3(cond, msg) typedef char static_assertion_##msg[(!!(cond))*2-1]
	/* NOTE(bill): Token pasting madness!! */
	#define GB_STATIC_ASSERT2(cond, line) GB_STATIC_ASSERT3(cond, static_assertion_at_line_##line)
	#define GB_STATIC_ASSERT1(cond, line) GB_STATIC_ASSERT2(cond, line)
	#define GB_STATIC_ASSERT(cond)        GB_STATIC_ASSERT1(cond, __LINE__)
#endif



/***************************************************************
 *
 * Headers
 *
 */

#if defined(_WIN32) && !defined(__MINGW32__)
	#ifndef _CRT_SECURE_NO_WARNINGS
	#define _CRT_SECURE_NO_WARNINGS
	#endif
#endif

#if defined(GB_SYSTEM_UNIX)
	#define _GNU_SOURCE
	#define _LARGEFILE64_SOURCE
#endif


/* TODO(bill): How many of these headers do I really need? */
#include <stdarg.h>
#include <stddef.h>
#include <math.h> /* fmod */

#if defined(GB_SYSTEM_WINDOWS)
	#define NOMINMAX            1
	#define WIN32_LEAN_AND_MEAN 1
	#define WIN32_MEAN_AND_LEAN 1
	#define VC_EXTRALEAN        1
	#include <windows.h>
 	#undef NOMINMAX
	#undef WIN32_LEAN_AND_MEAN
	#undef WIN32_MEAN_AND_LEAN
	#undef VC_EXTRALEAN

	#include <malloc.h> /* NOTE(bill): _aligned_*() */
	#include <intrin.h>
#else

	#include <stdlib.h> /* NOTE(bill): malloc */
	#include <dlfcn.h>
	#include <fcntl.h>
	#include <pthread.h>
	#include <sys/mman.h>
	#include <sys/stat.h>
	#include <sys/time.h>
	#include <sys/types.h>
	#include <time.h>
	#include <unistd.h>
	#include <errno.h>
#endif

#if defined(GB_SYSTEM_OSX)
#include <mach/mach.h>
#include <mach/mach_time.h>
#endif

#if defined(GB_SYSTEM_UNIX)
#include <semaphore.h>
#endif


/***************************************************************
 *
 * Base Types
 *
 */

#if defined(_MSC_VER)
	#if _MSC_VER < 1300
	typedef unsigned char     u8;
	typedef   signed char     i8;
	typedef unsigned short   u16;
	typedef   signed short   i16;
	typedef unsigned int     u32;
	typedef   signed int     i32;
	#else
	typedef unsigned __int8   u8;
	typedef   signed __int8   i8;
	typedef unsigned __int16 u16;
	typedef   signed __int16 i16;
	typedef unsigned __int32 u32;
	typedef   signed __int32 i32;
	#endif
	typedef unsigned __int64 u64;
	typedef   signed __int64 i64;
#else
	#include <stdint.h>
	typedef uint8_t   u8;
	typedef  int8_t   i8;
	typedef uint16_t u16;
	typedef  int16_t i16;
	typedef uint32_t u32;
	typedef  int32_t i32;
	typedef uint64_t u64;
	typedef  int64_t i64;
#endif

GB_STATIC_ASSERT(sizeof(u8)  == sizeof(i8));
GB_STATIC_ASSERT(sizeof(u16) == sizeof(i16));
GB_STATIC_ASSERT(sizeof(u32) == sizeof(i32));
GB_STATIC_ASSERT(sizeof(u64) == sizeof(i64));

GB_STATIC_ASSERT(sizeof(u8)  == 1);
GB_STATIC_ASSERT(sizeof(u16) == 2);
GB_STATIC_ASSERT(sizeof(u32) == 4);
GB_STATIC_ASSERT(sizeof(u64) == 8);

typedef size_t    usize;
typedef ptrdiff_t isize;

GB_STATIC_ASSERT(sizeof(usize) == sizeof(isize));

#if defined(_WIN64)
	typedef signed   __int64  intptr;
	typedef unsigned __int64 uintptr;
#elif defined(_WIN32)
	/* NOTE(bill); To mark types changing their size, e.g. intptr */
	#ifndef _W64
		#if !defined(__midl) && (defined(_X86_) || defined(_M_IX86)) && _MSC_VER >= 1300
			#define _W64 __w64
		#else
			#define _W64
		#endif
	#endif

	typedef _W64   signed int  intptr;
	typedef _W64 unsigned int uintptr;
#else
	typedef uintptr_t uintptr;
	typedef  intptr_t  intptr;
#endif

typedef float  f32;
typedef double f64;

GB_STATIC_ASSERT(sizeof(f32) == 4);
GB_STATIC_ASSERT(sizeof(f64) == 8);

typedef char char8;  /* NOTE(bill): Probably redundant but oh well! */
typedef u16  char16;
typedef u32  char32;

/* NOTE(bill): I think C99 and C++ `bool` is stupid for numerous reasons but there are too many
 * to write in this small comment.
 */
typedef i8  b8;
typedef i16 b16;
typedef i32 b32; /* NOTE(bill): Prefer this!!! */

/* NOTE(bill): Get true and false */
#if !defined(__cplusplus)
	#if (defined(_MSC_VER) && _MSC_VER <= 1800) || !defined(__STDC_VERSION__)
		#ifndef true
		#define true  (0 == 0)
		#endif
		#ifndef false
		#define false (0 != 0)
		#endif
	#else
		#include <stdbool.h>
	#endif
#endif

/* NOTE(bill): These do are not prefixed with gb because the types are not. */
#ifndef U8_MIN
#define U8_MIN 0u
#define U8_MAX 0xffu
#define I8_MIN (-0x7f - 1)
#define I8_MAX 0x7f

#define U16_MIN 0u
#define U16_MAX 0xffffu
#define I16_MIN (-0x7fff - 1)
#define I16_MAX 0x7fff

#define U32_MIN 0u
#define U32_MAX 0xffffffffu
#define I32_MIN (-0x7fffffff - 1)
#define I32_MAX 0x7fffffff

#define U64_MIN 0ull
#define U64_MAX 0xffffffffffffffffull
#define I64_MIN (-0x7fffffffffffffffll - 1)
#define I64_MAX 0x7fffffffffffffffll

#if defined(GB_ARCH_32_BIT)
	#define USIZE_MIX U32_MIN
	#define USIZE_MAX U32_MAX

	#define ISIZE_MIX S32_MIN
	#define ISIZE_MAX S32_MAX
#elif defined(GB_ARCH_64_BIT)
	#define USIZE_MIX U64_MIN
	#define USIZE_MAX U64_MAX

	#define ISIZE_MIX I64_MIN
	#define ISIZE_MAX I64_MAX
#else
	#error Unknown architecture size. This library only supports 32 bit and 64 bit architectures.
#endif

#define F32_MIN 1.17549435e-38f
#define F32_MAX 3.40282347e+38f

#define F64_MIN 2.2250738585072014e-308
#define F64_MAX 1.7976931348623157e+308


#endif



#ifndef NULL
	#if defined(__cplusplus)
		#if __cplusplus >= 201103L
			#define NULL nullptr
		#else
			#define NULL 0
		#endif
	#else
		#define NULL ((void *)0)
	#endif
#endif


/* TODO(bill): Is this enough to get inline working? */
#if !defined(__cplusplus)
	#if defined(_MSC_VER) && _MSC_VER <= 1800
	#define inline __inline
	#elif !defined(__STDC_VERSION__)
	#define inline __inline__
	#else
	#define inline
	#endif
#endif


#if !defined(gb_restrict)
	#if defined(_MSC_VER)
		#define gb_restrict __restrict
	#elif defined(__STDC_VERSION__)
		#define gb_restrict restrict
	#else
		#define gb_restrict
	#endif
#endif

/* TODO(bill): Should force inline be a separate keyword and gb_inline be inline? */
#if !defined(gb_inline)
	#if defined(_MSC_VER)
		#if _MSC_VER < 1300
		#define gb_inline
		#else
		#define gb_inline __forceinline
		#endif
	#else
		#define gb_inline __attribute__ ((__always_inline__))
	#endif
#endif

#if !defined(gb_no_inline)
	#if defined(_MSC_VER)
		#define gb_no_inline __declspec(noinline)
	#else
		#define gb_no_inline __attribute__ ((noinline))
	#endif
#endif

/* NOTE(bill): Easy to grep */
/* NOTE(bill): Not needed in macros */
#ifndef cast
#define cast(Type) (Type)
#endif


/* NOTE(bill): Because a signed sizeof is more useful */
#ifndef gb_size_of
#define gb_size_of(x) (isize)(sizeof(x))
#endif

#ifndef gb_count_of
#define gb_count_of(x) ((gb_size_of(x)/gb_size_of(0[x])) / ((isize)(!(gb_size_of(x) % gb_size_of(0[x])))))
#endif

#ifndef gb_offset_of
#define gb_offset_of(Type, element) ((isize)&(((Type *)0)->element))
#endif

#if defined(__cplusplus)
#ifndef gb_align_of
	#if __cplusplus >= 201103L
		#define gb_align_of(Type) (isize)alignof(Type)
	#else
extern "C++" {
		/* NOTE(bill): Fucking Templates! */
		template <typename T> struct gbAlignment_Trick { char c; T member; };
		#define gb_align_of(Type) gb_offset_of(gbAlignment_Trick<Type>, member)
}
	#endif
#endif
#else
	#ifndef gb_align_of
	#define gb_align_of(Type) gb_offset_of(struct { char c; Type member; }, member)
	#endif
#endif

/* NOTE(bill): I do wish I had a type_of that was portable */
#ifndef gb_swap
#define gb_swap(Type, a, b) do { Type tmp = (a); (a) = (b); (b) = tmp; } while (0)
#endif

/* NOTE(bill): Because static means 3/4 different things in C/C++. Great design (!) */
#ifndef gb_global
#define gb_global        static /* Global variables */
#define gb_internal      static /* Internal linkage */
#define gb_local_persist static /* Local Persisting variables */
#endif


#ifndef gb_unused
#define gb_unused(x) ((void)(gb_size_of(x)))
#endif





/***************************************************************
 *
 * Defer statement
 * Akin to D's SCOPE_EXIT or
 * similar to Go's defer but scope-based
 *
 * NOTE: C++11 (and above) only!
 */
#if defined(__cplusplus) && __cplusplus >= 201103L
#if 1
extern "C++" {
#endif
namespace gb {

	/* NOTE(bill): Stupid fucking templates */
	template <typename T> struct RemoveReference       { typedef T Type; };
	template <typename T> struct RemoveReference<T &>  { typedef T Type; };
	template <typename T> struct RemoveReference<T &&> { typedef T Type; };

	/* NOTE(bill): "Move" semantics - invented because the C++ committee are idiots (as a collective not as indiviuals (well a least some aren't)) */
	template <typename T> inline T &&forward(typename RemoveReference<T>::Type &t)  { return static_cast<T &&>(t); }
	template <typename T> inline T &&forward(typename RemoveReference<T>::Type &&t) { return static_cast<T &&>(t); }
	template <typename T> inline T &&move   (T &&t)                                 { return static_cast<typename RemoveReference<T>::Type &&>(t); }
	template <typename F>
	struct privDefer {
		F f;
		privDefer(F &&f) : f(forward<F>(f)) {}
		~privDefer() { f(); }
	};
	template <typename F> privDefer<F> priv_defer_func(F &&f) { return privDefer<F>(forward<F>(f)); }

	#ifndef defer
	#define GB_DEFER_1(x, y) x##y
	#define GB_DEFER_2(x, y) GB_DEFER_1(x, y)
	#define GB_DEFER_3(x)    GB_DEFER_2(x, __COUNTER__)
	#define defer(code)      auto GB_DEFER_3(_defer_) = gb::priv_defer_func([&](){code;})
	#endif
} /* namespace gb */
#if 1
}
#endif

/* Example */
#if 0
	gbMutex m;
	gb_mutex_init(&m);
	{
		gb_mutex_lock(&m);
		defer (gb_mutex_unlock(&m));

		...
	}
#endif

#endif


/***************************************************************
 *
 * Macro Fun!
 *
 */

#ifndef GB_JOIN_MACROS
#define GB_JOIN_MACROS
	#define GB_JOIN2_IND(a, b) a##b

	#define GB_JOIN2(a, b)       GB_JOIN2_IND(a, b)
	#define GB_JOIN3(a, b, c)    GB_JOIN2(GB_JOIN2(a, b), c)
	#define GB_JOIN4(a, b, c, d) GB_JOIN2(GB_JOIN2(GB_JOIN2(a, b), c), d)
#endif


#ifndef GB_BIT
#define GB_BIT(x) (1<<(x))
#endif

#ifndef gb_min
#define gb_min(a, b) ((a) < (b) ? (a) : (b))
#endif

#ifndef gb_max
#define gb_max(a, b) ((a) > (b) ? (a) : (b))
#endif

#ifndef gb_clamp
#define gb_clamp(x, lower, upper) gb_min(gb_max((x), (lower)), (upper))
#endif

#ifndef gb_clamp01
#define gb_clamp01(x) gb_clamp((x), 0, 1)
#endif

#ifndef gb_is_between
#define gb_is_between(x, lower, upper) (((x) >= (lower)) && ((x) <= (upper)))
#endif


/* NOTE(bill): Some compilers support applying printf-style warnings to user functions. */
#if defined(__clang__) || defined(__GNUC__)
#define GB_PRINTF_ARGS(FMT) __attribute__((format(printf, FMT, (FMT+1))))
#else
#define GB_PRINTF_ARGS(FMT)
#endif

/***************************************************************
 *
 * Debug
 *
 */


#ifndef GB_DEBUG_TRAP
	#if defined(_MSC_VER)
	 	#if _MSC_VER < 1300
		#define GB_DEBUG_TRAP() __asm int 3; /* Trap to debugger! */
		#else
		#define GB_DEBUG_TRAP() __debugbreak()
		#endif
	#else
		#define GB_DEBUG_TRAP() __builtin_trap()
	#endif
#endif

#ifndef GB_ASSERT_MSG
#define GB_ASSERT_MSG(cond, msg) do { \
	if (!(cond)) { \
		gb_assert_handler(#cond, __FILE__, cast(i64)__LINE__, msg); \
		GB_DEBUG_TRAP(); \
	} \
} while (0)
#endif

#ifndef GB_ASSERT
#define GB_ASSERT(cond) GB_ASSERT_MSG(cond, NULL)
#endif

#ifndef GB_ASSERT_NOT_NULL
#define GB_ASSERT_NOT_NULL(ptr) GB_ASSERT_MSG((ptr) != NULL, #ptr " must not be NULL")
#endif

/* NOTE(bill): Things that shouldn't happen with a message! */
#ifndef GB_PANIC
#define GB_PANIC(msg) GB_ASSERT_MSG(0, msg)
#endif

GB_DEF void gb_assert_handler(char const *condition, char const *file, i32 line, char const *msg);



/***************************************************************
 *
 * Memory
 *
 */


GB_DEF isize gb_is_power_of_two(isize x);

GB_DEF void *      gb_align_forward(void *ptr, isize alignment);

GB_DEF void *      gb_pointer_add      (void *ptr, isize bytes);
GB_DEF void *      gb_pointer_sub      (void *ptr, isize bytes);
GB_DEF void const *gb_pointer_add_const(void const *ptr, isize bytes);
GB_DEF void const *gb_pointer_sub_const(void const *ptr, isize bytes);
GB_DEF isize       gb_pointer_diff     (void const *begin, void const *end);


GB_DEF void gb_zero_size(void *ptr, isize size);
/* TODO(bill): Should gb_zero_struct be renamed to gb_zero_elem(ent)? */
#ifndef     gb_zero_struct
#define     gb_zero_struct(t) gb_zero_size((t), gb_size_of(*(t))) /* NOTE(bill): Pass pointer of struct */
#define     gb_zero_array(a, count) gb_zero_size((a), gb_size_of(*(a))*count)
#endif

GB_DEF void *gb_memcopy   (void *gb_restrict dest, void const *gb_restrict source, isize size);
GB_DEF void *gb_memmove   (void *dest, void const *source, isize size);
GB_DEF void *gb_memset    (void *data, u8 byte_value, isize size);
GB_DEF i32   gb_memcompare(void const *s1, void const *s2, isize size);
GB_DEF void  gb_memswap   (void *i, void *j, isize size);


/* NOTE(bill): Very similar to doing `*cast(T *)(&u)` */
#ifndef GB_BIT_CAST
#define GB_BIT_CAST(dest, source) do { \
	GB_STATIC_ASSERT(gb_size_of(*(dest)) <= gb_size_of(source)); \
	gb_memcopy((dest), &(source), gb_size_of(*dest)); \
} while (0)
#endif




#ifndef gb_kilobytes
#define gb_kilobytes(x) (            (x) * (i64)(1024))
#define gb_megabytes(x) (gb_kilobytes(x) * (i64)(1024))
#define gb_gigabytes(x) (gb_megabytes(x) * (i64)(1024))
#define gb_terabytes(x) (gb_gigabytes(x) * (i64)(1024))
#endif




/* Atomics */
#if defined(_MSC_VER)
typedef struct gbAtomic32 { i32 value; } gbAtomic32;
typedef struct gbAtomic64 { i64 value; } gbAtomic64;
#else
typedef struct gbAtomic32 { i32 volatile value; } __attribute__ ((aligned(4))) gbAtomic32;
typedef struct gbAtomic64 { i64 volatile value; } __attribute__ ((aligned(8))) gbAtomic64;
#endif

GB_DEF i32  gb_atomic32_load            (gbAtomic32 const volatile *a);
GB_DEF void gb_atomic32_store           (gbAtomic32 volatile *a, i32 value);
GB_DEF i32  gb_atomic32_compare_exchange(gbAtomic32 volatile *a, i32 expected, i32 desired);
GB_DEF i32  gb_atomic32_exchanged       (gbAtomic32 volatile *a, i32 desired);
GB_DEF i32  gb_atomic32_fetch_add       (gbAtomic32 volatile *a, i32 operand);
GB_DEF i32  gb_atomic32_fetch_and       (gbAtomic32 volatile *a, i32 operand);
GB_DEF i32  gb_atomic32_fetch_or        (gbAtomic32 volatile *a, i32 operand);
GB_DEF void gb_atomic32_spin_lock       (gbAtomic32 volatile *a);
GB_DEF void gb_atomic32_spin_unlock     (gbAtomic32 volatile *a);


GB_DEF i64  gb_atomic64_load            (gbAtomic64 const volatile *a);
GB_DEF void gb_atomic64_store           (gbAtomic64 volatile *a, i64 value);
GB_DEF i64  gb_atomic64_compare_exchange(gbAtomic64 volatile *a, i64 expected, i64 desired);
GB_DEF i64  gb_atomic64_exchanged       (gbAtomic64 volatile *a, i64 desired);
GB_DEF i64  gb_atomic64_fetch_add       (gbAtomic64 volatile *a, i64 operand);
GB_DEF i64  gb_atomic64_fetch_and       (gbAtomic64 volatile *a, i64 operand);
GB_DEF i64  gb_atomic64_fetch_or        (gbAtomic64 volatile *a, i64 operand);
GB_DEF void gb_atomic64_spin_lock       (gbAtomic64 volatile *a);
GB_DEF void gb_atomic64_spin_unlock     (gbAtomic64 volatile *a);




typedef struct gbSemaphore {
#if defined(GB_SYSTEM_WINDOWS)
	void *win32_handle;
#elif defined(GB_SYSTEM_OSX)
	semaphore_t osx_handle;
#elif defined(GB_SYSTEM_UNIX)
	sem_t unix_handle;
#else
#error
#endif
} gbSemaphore;

GB_DEF void gb_semaphore_init   (gbSemaphore *s);
GB_DEF void gb_semaphore_destroy(gbSemaphore *s);
GB_DEF void gb_semaphore_post   (gbSemaphore *s, i32 count);
GB_DEF void gb_semaphore_wait   (gbSemaphore *s);


/* Mutex */
typedef struct gbMutex {
	gbSemaphore semaphore;
	gbAtomic32 counter;
	gbAtomic32 owner;
	i32 recursion;
} gbMutex;

GB_DEF void gb_mutex_init    (gbMutex *m);
GB_DEF void gb_mutex_destroy (gbMutex *m);
GB_DEF void gb_mutex_lock    (gbMutex *m);
GB_DEF b32  gb_mutex_try_lock(gbMutex *m);
GB_DEF void gb_mutex_unlock  (gbMutex *m);

/* NOTE(bill): If you wanted a Scoped Mutex in C++, why not use the defer() construct?
 * No need for a silly wrapper class and it's clear!
 */
#if 0
gbMutex m = {0};
gb_mutex_init(&m);
{
	gb_mutex_lock(&m);
	defer (gb_mutex_unlock(&m));

	/* Do whatever as the mutex is now scoped based! */
}
#endif

/* TODO(bill): Should I create a Condition Type? (gbCond vs gbCondition) */





#define GB_THREAD_PROC(name) void name(void *data)
typedef GB_THREAD_PROC(gbThreadProc);

typedef struct gbThread {
#if defined(GB_SYSTEM_WINDOWS)
	void *win32_handle;
#else
	pthread_t posix_handle;
#endif

	gbThreadProc *proc;
	void *data;

	gbSemaphore semaphore;
	isize       stack_size;
	b32         is_running;
} gbThread;

GB_DEF void gb_thread_init            (gbThread *t);
GB_DEF void gb_thread_destory         (gbThread *t);
GB_DEF void gb_thread_start           (gbThread *t, gbThreadProc *proc, void *data);
GB_DEF void gb_thread_start_with_stack(gbThread *t, gbThreadProc *proc, void *data, isize stack_size);
GB_DEF void gb_thread_join            (gbThread *t);
GB_DEF b32  gb_thread_is_running      (gbThread const *t);
GB_DEF u32  gb_thread_current_id      (void);
GB_DEF void gb_thread_set_name        (gbThread *t, char const *name);


/***************************************************************
 *
 * Virtual Memory
 *
 * Still incomplete and needs working on a lot as it's shit!
 */

/* TODO(bill): Track a lot more than just the pointer and size! */

typedef struct gbVirtualMemory {
	void *data;
	isize size;
} gbVirtualMemory;

GB_DEF gbVirtualMemory gb_virtual_memory(void *data, isize size);
GB_DEF gbVirtualMemory gb_vm_alloc(void *addr, isize size);
GB_DEF void            gb_vm_free (gbVirtualMemory vm);
GB_DEF gbVirtualMemory gb_vm_trim (gbVirtualMemory vm, isize lead_size, isize size);
GB_DEF b32             gb_vm_purge(gbVirtualMemory vm);



/***************************************************************
 *
 * Custom Allocation
 *
 */

typedef enum gbAllocationType {
	GB_ALLOCATION_ALLOC,
	GB_ALLOCATION_FREE,
	GB_ALLOCATION_FREE_ALL,
	GB_ALLOCATION_RESIZE
} gbAllocationType;

/* NOTE(bill): This is useful so you can define an allocator of the same type and parameters */
#define GB_ALLOCATOR_PROC(name)                         \
void *name(void *allocator_data, gbAllocationType type, \
           isize size, isize alignment,                 \
           void *old_memory, isize old_size,            \
           u64 options)
typedef GB_ALLOCATOR_PROC(gbAllocatorProc);

typedef struct gbAllocator {
	gbAllocatorProc *proc;
	void *data;
} gbAllocator;

#ifndef GB_DEFAULT_MEMORY_ALIGNMENT
#define GB_DEFAULT_MEMORY_ALIGNMENT 4
#endif

GB_DEF void *gb_alloc_align (gbAllocator a, isize size, isize alignment);
GB_DEF void *gb_alloc       (gbAllocator a, isize size);
GB_DEF void  gb_free        (gbAllocator a, void *ptr);
GB_DEF void  gb_free_all    (gbAllocator a);
GB_DEF void *gb_resize      (gbAllocator a, void *ptr, isize old_size, isize new_size);
GB_DEF void *gb_resize_align(gbAllocator a, void *ptr, isize old_size, isize new_size, isize alignment);
/* TODO(bill): For gb_resize, should the use need to pass the old_size or only the new_size? */

GB_DEF void *gb_alloc_copy      (gbAllocator a, void const *src, isize size);
GB_DEF void *gb_alloc_copy_align(gbAllocator a, void const *src, isize size, isize alignment);

GB_DEF char *gb_alloc_str(gbAllocator a, char const *str);


/* NOTE(bill): These are very useful and the type cast has saved me from numerous bugs */
#ifndef gb_alloc_struct
#define gb_alloc_struct(allocator, Type)       (Type *)gb_alloc(allocator, gb_size_of(Type))
#define gb_alloc_array(allocator, Type, count) (Type *)gb_alloc(allocator, gb_size_of(Type) * (count))
#endif

/* NOTE(bill): Use this if you don't need a "fancy" resize allocation */
GB_DEF void *gb_default_resize_align(gbAllocator a, void *ptr, isize old_size, isize new_size, isize alignment);



/* TODO(bill): Probably use a custom heap allocator system that doesn't depend on malloc/free
 * Base it off TCMalloc or something else? Or something entirely custom?
 */
GB_DEF gbAllocator gb_heap_allocator(void);
GB_DEF GB_ALLOCATOR_PROC(gb_heap_allocator_proc);

/* NOTE(bill): Yep, I use my own allocator system! */
#ifndef gb_malloc
#define gb_malloc(sz) gb_alloc(gb_heap_allocator(), sz)
#define gb_mfree(ptr) gb_free(gb_heap_allocator(), ptr)
#endif


/*
 * Arena Allocator
 */

typedef struct gbArena {
	gbAllocator backing;
	void *physical_start;
	isize total_size;
	isize total_allocated;
	u32 temp_count;
} gbArena;

GB_DEF void gb_arena_init_from_memory   (gbArena *arena, void *start, isize size);
GB_DEF void gb_arena_init_from_allocator(gbArena *arena, gbAllocator backing, isize size);
GB_DEF void gb_arena_init_sub           (gbArena *arena, gbArena *parent_arena, isize size);
GB_DEF void gb_arena_free               (gbArena *arena);

GB_DEF isize gb_arena_alignment_of  (gbArena *arena, isize alignment);
GB_DEF isize gb_arena_size_remaining(gbArena *arena, isize alignment);
GB_DEF void  gb_arena_check         (gbArena *arena);


/* Allocation Types: alloc, free_all, resize */
GB_DEF gbAllocator gb_arena_allocator(gbArena *arena);
GB_DEF GB_ALLOCATOR_PROC(gb_arena_allocator_proc);



typedef struct gbTempArenaMemory {
	gbArena *arena;
	isize original_count;
} gbTempArenaMemory;

GB_DEF gbTempArenaMemory gb_temp_arena_memory_begin(gbArena *arena);
GB_DEF void              gb_temp_arena_memory_end  (gbTempArenaMemory tmp_mem);







/*
 * Pool Allocator
 */


typedef struct gbPool {
	gbAllocator backing;

	void *physical_start;
	void *free_list;

	isize block_size;
	isize block_align;
	isize total_size;
} gbPool;

GB_DEF void gb_pool_init      (gbPool *pool, gbAllocator backing, isize num_blocks, isize block_size);
GB_DEF void gb_pool_init_align(gbPool *pool, gbAllocator backing, isize num_blocks, isize block_size, isize block_align);
GB_DEF void gb_pool_free      (gbPool *pool);

/* Allocation Types: alloc, free */
GB_DEF gbAllocator gb_pool_allocator(gbPool *pool);
GB_DEF GB_ALLOCATOR_PROC(gb_pool_allocator_proc);



/* NOTE(bill): Used for allocators to keep track of sizes */
typedef struct gbAllocationHeader {
	isize size;
} gbAllocationHeader;

GB_DEF gbAllocationHeader *gb_allocation_header     (void *data);
GB_DEF void                gb_allocation_header_fill(gbAllocationHeader *header, void *data, isize size);

/* TODO(bill): Find better way of doing this without #if #elif etc. */
#if defined(GB_ARCH_32_BIT)
#define GB_ISIZE_HIGH_BIT 0x80000000
#elif defined(GB_ARCH_64_BIT)
#define GB_ISIZE_HIGH_BIT 0x8000000000000000ll
#else
#error
#endif

/*
 * Free List Allocator
 */

/* IMPORTANT TODO(bill): Thoroughly test the free list allocator! */
/* NOTE(bill): This is a very shitty free list as it just picks the first free block not the best size
 * as I am just being lazy. Also, I will probably remove it later; it's only here because why not?!
 */
/* NOTE(bill): I may also complete remove this if I completely implement a fixed heap allocator */


typedef struct gbFreeListBlock {
	struct gbFreeListBlock *next;
	isize size;
} gbFreeListBlock;

typedef struct gbFreeList {
	void *           physical_start;
	isize            total_size;

	gbFreeListBlock *curr_block;

	isize            total_allocated;
	isize            allocation_count;
} gbFreeList;

GB_DEF void gb_free_list_init               (gbFreeList *fl, void *start, isize size);
GB_DEF void gb_free_list_init_from_allocator(gbFreeList *fl, gbAllocator backing, isize size);

/* Allocation Types: alloc, free, free_all, resize */
GB_DEF gbAllocator gb_free_list_allocator(gbFreeList *fl);
GB_DEF GB_ALLOCATOR_PROC(gb_free_list_allocator_proc);



/*
 * Scratch Memory Allocator - Ring Buffer Based Arena
 */

typedef struct gbScratchMemory {
	void *physical_start;
	isize total_size;
	void *alloc_point, *free_point;
} gbScratchMemory;

GB_DEF void gb_scratch_memory_init     (gbScratchMemory *s, void *start, isize size);
GB_DEF b32  gb_scratch_memory_is_in_use(gbScratchMemory *s, void *ptr);


/* Allocation Types: alloc, free, free_all, resize */
GB_DEF gbAllocator gb_scratch_allocator(gbScratchMemory *s);
GB_DEF GB_ALLOCATOR_PROC(gb_scratch_allocator_proc);

/* TODO(bill): Stack allocator */
/* TODO(bill): Fixed heap allocator */
/* TODO(bill): General heap allocator. Maybe a TCMalloc like clone? */


/***************************************************************
 *
 * Sort & Search
 *
 */

#define GB_COMPARE_PROC(name) int name(void const *a, void const *b)
typedef GB_COMPARE_PROC(gbCompareProc);

#define GB_COMPARE_PROC_PTR(def) GB_COMPARE_PROC((*def))

/* Producure pointers */
/* NOTE(bill): The offset parameter specifies the offset in the structure
 * e.g. gb_i32_cmp(gb_offset_of(Thing, value))
 * Use 0 if it's just the type instead.
 */

GB_DEF GB_COMPARE_PROC_PTR(gb_i16_cmp  (isize offset));
GB_DEF GB_COMPARE_PROC_PTR(gb_i32_cmp  (isize offset));
GB_DEF GB_COMPARE_PROC_PTR(gb_i64_cmp  (isize offset));
GB_DEF GB_COMPARE_PROC_PTR(gb_isize_cmp(isize offset));
GB_DEF GB_COMPARE_PROC_PTR(gb_str_cmp  (isize offset));
GB_DEF GB_COMPARE_PROC_PTR(gb_f32_cmp  (isize offset));
GB_DEF GB_COMPARE_PROC_PTR(gb_f64_cmp  (isize offset));
GB_DEF GB_COMPARE_PROC_PTR(gb_char_cmp (isize offset));

/* TODO(bill): Better sorting algorithms */
/* NOTE(bill): Uses quick sort for large arrays but insertion sort for small */
#define gb_sort_array(array, count, compare_proc) gb_sort(array, count, gb_size_of(*(array)), compare_proc)
GB_DEF void gb_sort(void *base, isize count, isize size, gbCompareProc compare_proc);

/* NOTE(bill): the count of temp == count of items */
GB_DEF void gb_radix_sort_u8 (u8  *gb_restrict items, u8  *gb_restrict temp, isize count);
GB_DEF void gb_radix_sort_u16(u16 *gb_restrict items, u16 *gb_restrict temp, isize count);
GB_DEF void gb_radix_sort_u32(u32 *gb_restrict items, u32 *gb_restrict temp, isize count);
GB_DEF void gb_radix_sort_u64(u64 *gb_restrict items, u64 *gb_restrict temp, isize count);



/* NOTE(bill): Returns index or -1 if not found */
#define gb_binary_search_array(array, count, key, compare_proc) gb_binary_search(array, count, gb_size_of(*(array)), key, compare_proc)
GB_DEF isize gb_binary_search(void const *base, isize count, isize size, void const *key, gbCompareProc compare_proc);



/***************************************************************
 *
 * Char Functions
 *
 */

GB_DEF char gb_char_to_lower       (char c);
GB_DEF char gb_char_to_upper       (char c);
GB_DEF b32  gb_char_is_space       (char c);
GB_DEF b32  gb_char_is_digit       (char c);
GB_DEF b32  gb_char_is_hex_digit   (char c);
GB_DEF b32  gb_char_is_alpha       (char c);
GB_DEF b32  gb_char_is_alphanumeric(char c);
GB_DEF i32  gb_digit_to_int        (char c);
GB_DEF i32  gb_hex_digit_to_int    (char c);

/* NOTE(bill): ASCII only */
GB_DEF void gb_str_to_lower(char *str);
GB_DEF void gb_str_to_upper(char *str);

GB_DEF isize gb_strlen (char const *str);
GB_DEF isize gb_strnlen(char const *str, isize max_len);
GB_DEF i32   gb_strcmp (char const *s1, char const *s2);
GB_DEF i32   gb_strncmp(char const *s1, char const *s2, isize len);
GB_DEF char *gb_strcpy (char *dest, char const *source);
GB_DEF char *gb_strncpy(char *dest, char const *source, isize len);
GB_DEF isize gb_strlcpy(char *dest, char const *source, isize len);
GB_DEF char *gb_strrev (char *str);



/* NOTE(bill): A less fucking crazy strtok! */
GB_DEF char const *gb_strtok(char *output, char const *src, char const *delimit);

GB_DEF b32 gb_str_has_prefix(char const *str, char const *prefix);
GB_DEF b32 gb_str_has_suffix(char const *str, char const *suffix);

GB_DEF char const *gb_char_first_occurence(char const *str, char c);
GB_DEF char const *gb_char_last_occurence (char const *str, char c);

GB_DEF void gb_str_concat(char *dest, isize dest_len,
                          char const *src_a, isize src_a_len,
                          char const *src_b, isize src_b_len);

GB_DEF i64   gb_str_to_i64(char const *str, char **end_ptr, i32 base); /* TODO(bill): Support more than just decimal and hexadecimal */
GB_DEF void  gb_i64_to_str(i64 value, char *string, i32 base);
GB_DEF void  gb_u64_to_str(u64 value, char *string, i32 base);


/***************************************************************
 *
 * UTF-8 Handling
 *
 *
 */

GB_DEF isize gb_utf8_strlen (char const *str);
GB_DEF isize gb_utf8_strnlen(char const *str, isize max_len);

/* Windows doesn't handle 8 bit filenames well ('cause Micro$hit) */
GB_DEF char16 *gb_utf8_to_ucs2(char16 *buffer, isize len, char const *str);
GB_DEF char *  gb_ucs2_to_utf8(char *buffer, isize len, char16 const *str);

/* NOTE(bill): Returns size of codepoint in bytes */
GB_DEF isize gb_utf8_decode    (char const *str, char32 *codepoint);
GB_DEF isize gb_utf8_decode_len(char const *str, isize str_len, char32 *codepoint);


/***************************************************************
 *
 * gbString - C Read-Only-Compatible
 *
 *

Reasoning:

	By default, strings in C are null terminated which means you have to count
	the number of character up to the null character to calculate the length.
	Many "better" C string libraries will create a struct for a string.
	i.e.

	    struct String {
	    	Allocator allocator;
	        size_t    length;
	        size_t    capacity;
	        char *    cstring;
	    };

	This library tries to augment normal C strings in a better way that is still
	compatible with C-style strings.

	+--------+-----------------------+-----------------+
	| Header | Binary C-style String | Null Terminator |
	+--------+-----------------------+-----------------+
	         |
	         +-> Pointer returned by functions

	Due to the meta-data being stored before the string pointer and every gb string
	having an implicit null terminator, gb strings are full compatible with c-style
	strings and read-only functions.

Advantages:

    * gb strings can be passed to C-style string functions without accessing a struct
      member of calling a function, i.e.

          printf("%s\n", gb_str);

      Many other libraries do either of these:

          printf("%s\n", string->cstr);
          printf("%s\n", get_cstring(string));

    * You can access each character just like a C-style string:

          printf("%c %c\n", str[0], str[13]);

    * gb strings are singularly allocated. The meta-data is next to the character
      array which is better for the cache.

Disadvantages:

    * In the C version of these functions, many return the new string. i.e.
          str = gb_string_appendc(str, "another string");
      This could be changed to gb_string_appendc(&str, "another string"); but I'm still not sure.

	* This is incompatible with "gb_string.h" strings
 */

#if 0
#include <stdio.h>
#include <stdlib.h>

#define GB_IMPLEMENTATION
#include "gb.h"

int main(int argc, char **argv)
{
	gbString str = gb_string_make("Hello");
	gbString other_str = gb_string_make_length(", ", 2);
	str = gb_string_append(str, other_str);
	str = gb_string_appendc(str, "world!");

	printf("%s\n", str); /* Hello, world! */

	printf("str length = %d\n", gb_string_length(str));

	str = gb_string_set(str, "Potato soup");
	printf("%s\n", str); /* Potato soup */

	str = gb_string_set(str, "Hello");
	other_str = gb_string_set(other_str, "Pizza");
	if (gb_strings_are_equal(str, other_str))
		printf("Not called\n");
	else
		printf("Called\n");

	str = gb_string_set(str, "Ab.;!...AHello World       ??");
	str = gb_string_trim(str, "Ab.;!. ?");
	printf("%s\n", str); /* "Hello World" */

	gb_string_free(str);
	gb_string_free(other_str);

	return 0;
}
#endif

typedef char *gbString;

/* NOTE(bill): If you only need a small string, just use a standard c string or change the size from isize to u16, etc. */
typedef struct gbStringHeader {
	gbAllocator allocator;
	isize length;
	isize capacity;
} gbStringHeader;

#define GB_STRING_HEADER(str) (cast(gbStringHeader *)(str) - 1)

GB_DEF gbString gb_string_make       (gbAllocator a, char const *str);
GB_DEF gbString gb_string_make_length(gbAllocator a, void const *str, isize num_bytes);
GB_DEF void     gb_string_free       (gbString str);

GB_DEF gbString gb_string_duplicate(gbAllocator a, gbString const str);

GB_DEF isize gb_string_length         (gbString const str);
GB_DEF isize gb_string_capacity       (gbString const str);
GB_DEF isize gb_string_available_space(gbString const str);

GB_DEF void gb_string_clear(gbString str);

GB_DEF gbString gb_string_append       (gbString str, gbString const other);
GB_DEF gbString gb_string_append_length(gbString str, void const *other, isize num_bytes);
GB_DEF gbString gb_string_appendc      (gbString str, char const *other);

GB_DEF gbString gb_string_set(gbString str, char const *cstr);

GB_DEF gbString gb_string_make_space_for(gbString str, isize add_len);
GB_DEF isize gb_string_allocation_size  (gbString const str);

GB_DEF b32 gb_string_are_equal(gbString const lhs, gbString const rhs);

GB_DEF gbString gb_string_trim      (gbString str, char const *cut_set);
GB_DEF gbString gb_string_trim_space(gbString str); /* Whitespace ` \t\r\n\v\f` */



/***************************************************************
 *
 * Fixed Capacity Buffer (POD Types)
 *
 *
	gbBuffer(Type) works like gbString or gbArray where the actual type is just a pointer to the first
	element.

 */



typedef struct gbBufferHeader {
	isize count;
	isize capacity;
} gbBufferHeader;

#define gbBuffer(Type) Type *

#define GB_BUFFER_HEADER(x)   (cast(gbBufferHeader *)(x) - 1)
#define gb_buffer_count(x)    (GB_BUFFER_HEADER(x)->count)
#define gb_buffer_capacity(x) (GB_BUFFER_HEADER(x)->capacity)

#define gb_buffer_init(x, allocator, cap) do { \
	void **nx = cast(void **)&(x); \
	gbBufferHeader *gb__bh = cast(gbBufferHeader *)gb_alloc((allocator), (cap)*gb_size_of(*(x))); \
	gb__bh->count = 0; \
	gb__bh->capacity = cap; \
	*nx = cast(void *)(gb__bh+1); \
} while (0)


#define gb_buffer_free(x, allocator) (gb_free(allocator, GB_BUFFER_HEADER(x)))

#define gb_buffer_append(x, item) do { (x)[gb_buffer_count(x)++] = (item); } while (0)

#define gb_buffer_appendv(x, items, item_count) do { \
	GB_ASSERT(gb_size_of(*(items)) == gb_size_of(*(x))); \
	GB_ASSERT(gb_buffer_count(x)+item_count <= gb_buffer_capacity(x)); \
	gb_memcopy(&(x)[gb_buffer_count(x)], (items), gb_size_of(*(x))*(item_count)); \
	gb_buffer_count(x) += (item_count); \
} while (0)

#define gb_buffer_pop(x)   do { GB_ASSERT(gb_buffer_count(x) > 0); gb_buffer_count(x)--; } while (0)
#define gb_buffer_clear(x) do { gb_buffer_count(x) = 0; } while (0)



/***************************************************************
 *
 * Dynamic Array (POD Types)
 *
	NOTE(bill): I know this is a macro hell but C is an old (and shit) language with no proper arrays
	Also why the fuck not?! It fucking works! And it has custom allocation, which is already better than C++!


	gbArray(Type) works like gbString or gbBuffer where the actual type is just a pointer to the first
	element.

 */


/* Available Procedures for gbArray(Type)
 *     gb_array_init
 *     gb_array_free
 *     gb_array_set_capacity
 *     gb_array_grow
 *     gb_array_append
 *     gb_array_appendv
 *     gb_array_pop
 *     gb_array_clear
 *     gb_array_resize
 *     gb_array_reserve
 */

#if 0 /* Example */
void foo(void)
{
	isize i;
	int test_values[] = {4, 2, 1, 7};
	gbAllocator a = gb_heap_allocator();
	gbArray(int) items;

	gb_array_init(items, a);

	gb_array_append(items, 1);
	gb_array_append(items, 4);
	gb_array_append(items, 9);
	gb_array_append(items, 16);

	items[1] = 3; /* Manually set value */
	                /* NOTE: No array bounds checking */

	for (i = 0; i < items.count; i++)
		gb_printf("%d\n", items[i]);
	/* 1
	 * 3
	 * 9
	 * 16
	 */

	gb_array_clear(items);

	gb_array_appendv(items, test_values, gb_count_of(test_values));
	for (i = 0; i < items.count; i++)
		gb_printf("%d\n", items[i]);
	/* 4
	 * 2
	 * 1
	 * 7
	 */

	gb_array_free(items);
}
#endif

typedef struct gbArrayHeader {
	gbAllocator allocator;
	isize count;
	isize capacity;
} gbArrayHeader;

/* NOTE(bill): This thing is magic! */
#define gbArray(Type) Type *

#ifndef GB_ARRAY_GROW_FORMULA
#define GB_ARRAY_GROW_FORMULA(x) (2*(x) + 8)
#endif

GB_STATIC_ASSERT(GB_ARRAY_GROW_FORMULA(0) > 0);

#define GB_ARRAY_HEADER(x)    (cast(gbArrayHeader *)(x) - 1)
#define gb_array_allocator(x) (GB_ARRAY_HEADER(x)->allocator)
#define gb_array_count(x)     (GB_ARRAY_HEADER(x)->count)
#define gb_array_capacity(x)  (GB_ARRAY_HEADER(x)->capacity)

/* TODO(bill): Have proper alignment! */
#define gb_array_init(x, allocator_) do { \
	void **gb__array_ = cast(void **)&(x); \
	gbArrayHeader *gb__ah = cast(gbArrayHeader *)gb_alloc(allocator_, gb_size_of(gbArrayHeader)+gb_size_of(*(x))*GB_ARRAY_GROW_FORMULA(0)); \
	gb__ah->allocator = allocator_; \
	gb__ah->count = gb__ah->capacity = 0; \
	*gb__array_ = cast(void *)(gb__ah+1); \
} while (0)

#define gb_array_free(x) do { \
	gbArrayHeader *gb__ah = GB_ARRAY_HEADER(x); \
	gb_free(gb__ah->allocator, gb__ah); \
} while (0)

#define gb_array_set_capacity(x, capacity) do { \
	if (x) { \
		void **gb__array_ = cast(void **)&(x); \
		*gb__array_ = gb__array_set_capacity((x), (capacity), gb_size_of(*(x))); \
	} \
} while (0)

/* NOTE(bill): Do not use the thing below directly, use the macro */
GB_DEF void *gb__array_set_capacity(void *array, isize capacity, isize element_size);


/* TODO(bill): Decide on a decent growing formula for gbArray */
#define gb_array_grow(x, min_capacity) do { \
	isize new_capacity = GB_ARRAY_GROW_FORMULA(gb_array_capacity(x)); \
	if (new_capacity < (min_capacity)) \
		new_capacity = (min_capacity); \
	gb_array_set_capacity(x, new_capacity); \
} while (0)


#define gb_array_append(x, item) do { \
	if (gb_array_capacity(x) < gb_array_count(x)+1) \
		gb_array_grow(x, 0); \
	(x)[gb_array_count(x)++] = (item); \
} while (0)

#define gb_array_appendv(x, items, item_count) do { \
	gbArrayHeader *gb__ah = GB_ARRAY_HEADER(x); \
	GB_ASSERT(gb_size_of((items)[0]) == gb_size_of((x)[0])); \
	if (gb__ah->capacity < gb__ah->count+(item_count)) \
		gb_array_grow(x, gb__ah->count+(item_count)); \
	gb_memcopy(&(x)[gb__ah->count], (items), gb_size_of((x)[0])*(item_count));\
	gb__ah->count += (item_count); \
} while (0)



#define gb_array_pop(x)   do { GB_ASSERT(GB_ARRAY_HEADER(x)->count > 0); GB_ARRAY_HEADER(x)->count--; } while (0)
#define gb_array_clear(x) do { GB_ARRAY_HEADER(x)->count = 0; } while (0)

#define gb_array_resize(x, new_count) do { \
	if (GB_ARRAY_HEADER(x)->capacity < (new_count)) \
		gb_array_grow(x, (new_count)); \
	GB_ARRAY_HEADER(x)->count = (new_count); \
} while (0)


#define gb_array_reserve(x, new_capacity) do { \
	if (GB_ARRAY_HEADER(x)->capacity < (new_capacity)) \
		gb_array_set_capacity(x, new_capacity); \
} while (0)





/***************************************************************
 *
 * Hashing Functions
 *
 */

GB_EXTERN u32 gb_adler32(void const *data, isize len);

GB_EXTERN u32 gb_crc32(void const *data, isize len);
GB_EXTERN u64 gb_crc64(void const *data, isize len);

GB_EXTERN u32 gb_fnv32 (void const *data, isize len);
GB_EXTERN u64 gb_fnv64 (void const *data, isize len);
GB_EXTERN u32 gb_fnv32a(void const *data, isize len);
GB_EXTERN u64 gb_fnv64a(void const *data, isize len);

/* NOTE(bill): Default seed of 0x9747b28c */
/* NOTE(bill): I prefer using murmur64 for most hashes */
GB_EXTERN u32 gb_murmur32(void const *data, isize len);
GB_EXTERN u64 gb_murmur64(void const *data, isize len);

GB_EXTERN u32 gb_murmur32_seed(void const *data, isize len, u32 seed);
GB_EXTERN u64 gb_murmur64_seed(void const *data, isize len, u64 seed);



/***************************************************************
 *
 * Hash Table - Still experimental!
 *
 */

/* NOTE(bill): Hash table for POD types with a u64 key
 * The hash table stores an void * for the data. This is because I don't want to macro hell to get this to work
 * With generic data types.
 * TODO(bill): Should the hash table store an isize or a void *? Which is more useful for the user?
 */

typedef struct gbHashTableEntry {
	u64   key;
	isize next;
	void *value;
} gbHashTableEntry;

typedef struct gbHashTable {
	gbArray(isize)            hashes;
	gbArray(gbHashTableEntry) entries;
} gbHashTable;


/* TODO(bill): I'm not very sure on the naming of these procedures and if they should be named better. */
GB_DEF void  gb_hash_table_init   (gbHashTable *h, gbAllocator a);
GB_DEF void  gb_hash_table_free   (gbHashTable *h);
GB_DEF void  gb_hash_table_clear  (gbHashTable *h);
GB_DEF b32   gb_hash_table_has    (gbHashTable const *h, u64 key);
GB_DEF void *gb_hash_table_get    (gbHashTable const *h, u64 key, void *default_value);
GB_DEF void  gb_hash_table_set    (gbHashTable *h, u64 key, void *value);
GB_DEF void  gb_hash_table_remove (gbHashTable *h, u64 key);
GB_DEF void  gb_hash_table_reserve(gbHashTable *h, isize capacity);

GB_DEF void  gb_multi_hash_table_get         (gbHashTable const *h, u64 key, void **values, isize count);
GB_DEF isize gb_multi_hash_table_count       (gbHashTable const *h, u64 key);
GB_DEF void  gb_multi_hash_table_insert      (gbHashTable *h, u64 key, void *value);
GB_DEF void  gb_multi_hash_table_remove_entry(gbHashTable *h, gbHashTableEntry const *e);
GB_DEF void  gb_multi_hash_table_remove_all  (gbHashTable *h, u64 key);

GB_DEF gbHashTableEntry const *gb_multi_hash_table_find_first_entry(gbHashTable const *h, u64 key);
GB_DEF gbHashTableEntry const *gb_multi_hash_table_find_next_entry (gbHashTable const *h, gbHashTableEntry const *e);





/***************************************************************
 *
 * File Handling
 *
 */

/*
typedef struct gbDirInfo {
	u8 *buf;
	isize buf_count;
	isize buf_pos;
} gbDirInfo;
*/

typedef u64 gbFileTime;

typedef struct gbFile {
	void *handle;
	char *name;
	/* gbDirInfo *dir_info; */
	gbFileTime last_write_time;
} gbFile;

typedef struct gbFileContents {
	void *data;
	isize size;
} gbFileContents;


typedef enum gbFileFlag {
	GB_FILE_READ       = GB_BIT(0),
	GB_FILE_WRITE      = GB_BIT(1),
	GB_FILE_READ_WRITE = GB_BIT(2),
	GB_FILE_APPEND     = GB_BIT(3),
	GB_FILE_CREATE     = GB_BIT(4),
	GB_FILE_TRUNCATE   = GB_BIT(5)
} gbFileFlag;

typedef u32 gbFileMode;
typedef enum gbFileModeType {
	GB_FILE_MODE_DENY   = 0,
	GB_FILE_MODE_READ   = GB_BIT(0),
	GB_FILE_MODE_WRITE  = GB_BIT(1),
	GB_FILE_MODE_DELETE = GB_BIT(2),
} gbFileModeType;

typedef enum gbSeekWhence {
	GB_SEEK_BEGIN   = 0,
	GB_SEEK_CURRENT = 1,
	GB_SEEK_END     = 2
} gbSeekWhence;

typedef enum gbFileError {
	GB_FILE_ERR_NONE,
	GB_FILE_ERR_INVALID,
	GB_FILE_ERR_EXISTS,
	GB_FILE_ERR_NOT_EXISTS,
	GB_FILE_ERR_PERMISSION,
	GB_FILE_ERR_TRUNCATION_FAILURE
} gbFileError;

typedef enum gbFileStandardType {
	GB_FILE_STANDARD_INPUT,
	GB_FILE_STANDARD_OUTPUT,
	GB_FILE_STANDARD_ERROR
} gbFileStandardType;

GB_DEF gbFile gb_file_get_std(gbFileStandardType std);


GB_DEF gbFileError gb_file_create     (gbFile *file, char const *filename, ...) GB_PRINTF_ARGS(2);
GB_DEF gbFileError gb_file_open       (gbFile *file, char const *filename, ...) GB_PRINTF_ARGS(2);
GB_DEF gbFileError gb_file_open_file  (gbFile *file, u32 flag, gbFileMode mode, char const *filename, ...) GB_PRINTF_ARGS(4);
GB_DEF gbFileError gb_file_close      (gbFile *file);
GB_DEF b32         gb_file_read       (gbFile *file, void *buffer, isize size);
GB_DEF b32         gb_file_write      (gbFile *file, void const *buffer, isize size);
GB_DEF b32         gb_file_read_at    (gbFile *file, void *buffer, isize size, i64 offset);
GB_DEF b32         gb_file_write_at   (gbFile *file, void const *buffer, isize size, i64 offset);
GB_DEF i64         gb_file_seek       (gbFile *file, i64 offset, gbSeekWhence whence);
GB_DEF i64         gb_file_tell       (gbFile *file);
GB_DEF i64         gb_file_size       (gbFile *file);
GB_DEF char const *gb_file_name       (gbFile *file);
GB_DEF gbFileError gb_file_truncate   (gbFile *file, i64 size);
GB_DEF b32         gb_file_has_changed(gbFile *file);



GB_DEF b32 gb_file_is_exist    (char const *name);
GB_DEF b32 gb_file_is_not_exist(char const *name);

GB_DEF gbFileTime gb_file_last_write_time(char const *filepath, ...) GB_PRINTF_ARGS(1);

GB_DEF b32 gb_file_copy  (char const *existing_filename, char const *new_filename, b32 fail_if_exists);
GB_DEF b32 gb_file_move  (char const *existing_filename, char const *new_filename);

GB_DEF gbFileContents gb_file_read_contents(gbAllocator a, b32 zero_terminate, char const *filepath, ...) GB_PRINTF_ARGS(3);


#ifndef GB_PATH_SEPARATOR
	#if defined(GB_SYSTEM_WINDOWS)
		#define GB_PATH_SEPARATOR '\\'
	#else
		#define GB_PATH_SEPARATOR '/'
	#endif
#endif

GB_DEF b32         gb_path_is_absolute(char const *path);
GB_DEF b32         gb_path_is_relative(char const *path);
GB_DEF b32         gb_path_is_root    (char const *path);
GB_DEF char const *gb_path_base_name  (char const *path);
GB_DEF char const *gb_path_extension  (char const *path);


/***************************************************************
 *
 * Printing
 *
 */


/* TODO(bill): Allow printf-ing to a gbFile!!! */

GB_DEF isize gb_printf        (char const *fmt, ...) GB_PRINTF_ARGS(1);
GB_DEF isize gb_printf_va     (char const *fmt, va_list va);
GB_DEF isize gb_printf_err    (char const *fmt, ...) GB_PRINTF_ARGS(1);
GB_DEF isize gb_printf_err_va (char const *fmt, va_list va);
GB_DEF isize gb_fprintf       (gbFile *f, char const *fmt, ...) GB_PRINTF_ARGS(2);
GB_DEF isize gb_fprintf_va    (gbFile *f, char const *fmt, va_list va);

GB_DEF char *gb_sprintf    (char const *fmt, ...) GB_PRINTF_ARGS(1); /* NOTE(bill): A locally persisting buffer is used internally */
GB_DEF char *gb_sprintf_va (char const *fmt, va_list va);            /* NOTE(bill): A locally persisting buffer is used internally */
GB_DEF isize gb_snprintf   (char *str, isize n, char const *fmt, ...) GB_PRINTF_ARGS(3);
GB_DEF isize gb_snprintf_va(char *str, isize n, char const *fmt, va_list va);

/* NOTE(bill): If you need an fprintf equivalent, you will need to but write to the file directly e.g.
 * char buf[...];
 * isize len = gb_snprintf(buf, gb_size_of(buf), "", ...);
 * isize offset = ...;
 * gb_file_write_at(&file, buf, len, offset);
 */



/***************************************************************
 *
 * DLL Handling
 *
 */

typedef void *gbDllHandle;
typedef void (*gbDllProc)(void);

GB_DEF gbDllHandle gb_dll_load        (char const *filepath, ...) GB_PRINTF_ARGS(1);
GB_DEF void        gb_dll_unload      (gbDllHandle dll);
GB_DEF gbDllProc   gb_dll_proc_address(gbDllHandle dll, char const *proc_name);


/***************************************************************
 *
 * Time
 *
 */

GB_DEF u64  gb_rdtsc       (void);
GB_DEF f64  gb_time_now    (void); /* NOTE(bill): This is only for relative time e.g. game loops */
GB_DEF u64  gb_utc_time_now(void); /* NOTE(bill): Number of microseconds since 1601-01-01 UTC */
GB_DEF void gb_sleep_ms    (u32 ms);



/***************************************************************
 *
 * Miscellany
 *
 */

typedef struct gbRandom {
	u64 seed[2];
} gbRandom;

GB_DEF void gb_random_init     (gbRandom *r);
GB_DEF u64  gb_random_next     (gbRandom *r);
GB_DEF i64  gb_random_range_i64(gbRandom *r, i64 lower_inc, i64 higher_inc);
GB_DEF f64  gb_random_range_f64(gbRandom *r, f64 lower_inc, f64 higher_inc);

GB_DEF void gb_exit     (u32 code);
GB_DEF void gb_yield    (void);
GB_DEF void gb_set_env  (char const *name, char const *value);
GB_DEF void gb_unset_env(char const *name);

GB_DEF u16 gb_endian_swap16(u16 i);
GB_DEF u32 gb_endian_swap32(u32 i);
GB_DEF u64 gb_endian_swap64(u64 i);



#if !defined(GB_NO_COLOUR_TYPE)


/***************************************************************
 *
 * Colour Type
 * It's quite useful
 * TODO(bill): Does this need to be in this library?
 *             Can I remove the anonymous struct extension?
 */

#if defined(_MSC_VER)
#pragma warning(push)
#pragma warning(disable:4201)
#endif

typedef union gbColour {
	u32 rgba; /* NOTE(bill): 0xaabbggrr */
	struct { u8 r, g, b, a; };
	u8 e[4];
} gbColour;
GB_STATIC_ASSERT(gb_size_of(gbColour) == gb_size_of(u32));

#if defined(_MSC_VER)
#pragma warning(pop)
#endif


GB_DEF gbColour gb_colour(f32 r, f32 g, f32 b, f32 a);

gb_global gbColour const GB_COLOUR_WHITE   = {0xffffffff};
gb_global gbColour const GB_COLOUR_GREY    = {0xff808080};
gb_global gbColour const GB_COLOUR_BLACK   = {0xff000000};

gb_global gbColour const GB_COLOUR_RED     = {0xff0000ff};
gb_global gbColour const GB_COLOUR_ORANGE  = {0xff0099ff};
gb_global gbColour const GB_COLOUR_YELLOW  = {0xff00ffff};
gb_global gbColour const GB_COLOUR_GREEN   = {0xff00ff00};
gb_global gbColour const GB_COLOUR_CYAN    = {0xffffff00};
gb_global gbColour const GB_COLOUR_BLUE    = {0xffff0000};
gb_global gbColour const GB_COLOUR_VIOLET  = {0xffff007f};
gb_global gbColour const GB_COLOUR_MAGENTA = {0xffff00ff};

#endif /* !defined(GB_NO_COLOUR_TYPE) */



/***************************************************************
 *
 * Platform Stuff
 *
 */

#if defined(GB_PLATFORM)

#if defined(GB_SYSTEM_WINDOWS)
#include <xinput.h>
#ifndef XUSER_MAX_COUNT
#define XUSER_MAX_COUNT 4
#endif
#endif

#ifndef GB_MAX_GAME_CONTROLLER_COUNT
#define GB_MAX_GAME_CONTROLLER_COUNT 4
#endif

typedef enum gbWindowType {
	GB_WINDOW_OPENGL   = 1,
	GB_WINDOW_SOFTWARE = 2,
} gbWindowType;

#if defined(_MSC_VER)
#pragma warning(push)
#pragma warning(disable:4201)
#endif

typedef struct gbWindow {
	void *handle;

	i32 x, y;
	i32 width, height;
	b32 is_closed;
	b32 has_focus;
	b32 is_minimized;
	b32 is_fullscreen;

#if defined(GB_SYSTEM_WINDOWS)
	WINDOWPLACEMENT win32_placement;
	HDC             win32_dc;
#endif

	gbWindowType type;
	union {
		struct {
#if defined(GB_SYSTEM_WINDOWS)
			HGLRC win32_context;
#endif
		} opengl;

		struct {
#if defined(GB_SYSTEM_WINDOWS)
			BITMAPINFO win32_bmi;
#endif
			void *memory;
			isize memory_size;
			i32 pitch;
			i32 bytes_per_pixel;
		} software;
	};
} gbWindow;

#if defined(_MSC_VER)
#pragma warning(pop)
#endif


typedef enum gbKeyType {
	GB_KEY_UNKNOWN = 0,  /* Unhandled key */

	GB_KEY_NUM0 = '0',
	GB_KEY_NUM1,
	GB_KEY_NUM2,
	GB_KEY_NUM3,
	GB_KEY_NUM4,
	GB_KEY_NUM5,
	GB_KEY_NUM6,
	GB_KEY_NUM7,
	GB_KEY_NUM8,
	GB_KEY_NUM9,

	GB_KEY_A = 'A',
	GB_KEY_B,
	GB_KEY_C,
	GB_KEY_D,
	GB_KEY_E,
	GB_KEY_F,
	GB_KEY_G,
	GB_KEY_H,
	GB_KEY_I,
	GB_KEY_J,
	GB_KEY_K,
	GB_KEY_L,
	GB_KEY_M,
	GB_KEY_N,
	GB_KEY_O,
	GB_KEY_P,
	GB_KEY_Q,
	GB_KEY_R,
	GB_KEY_S,
	GB_KEY_T,
	GB_KEY_U,
	GB_KEY_V,
	GB_KEY_W,
	GB_KEY_X,
	GB_KEY_Y,
	GB_KEY_Z,

	GB_KEY_LBRACKET  = '[',
	GB_KEY_RBRACKET  = ']',
	GB_KEY_SEMICOLON = ';',
	GB_KEY_COMMA     = ',',
	GB_KEY_PERIOD    = '.',
	GB_KEY_QUOTE     = '\'',
	GB_KEY_SLASH     = '/',
	GB_KEY_BACKSLASH = '\\',
	GB_KEY_GRAVE     = '`',
	GB_KEY_EQUALS    = '=',
	GB_KEY_MINUS     = '-',
	GB_KEY_SPACE     = ' ',

	GB_KEY__PAD = 128,   /* NOTE(bill): make sure ASCII is reserved */

	GB_KEY_ESCAPE,       /* The Escape key */
	GB_KEY_LCONTROL,     /* The left Control key */
	GB_KEY_LSHIFT,       /* The left Shift key */
	GB_KEY_LALT,         /* The left Alt key */
	GB_KEY_LSYSTEM,      /* The left OS specific key: window (Windows and Linux), apple (MacOS X), ... */
	GB_KEY_RCONTROL,     /* The right Control key */
	GB_KEY_RSHIFT,       /* The right Shift key */
	GB_KEY_RALT,         /* The right Alt key */
	GB_KEY_RSYSTEM,      /* The right OS specific key: window (Windows and Linux), apple (MacOS X), ... */
	GB_KEY_MENU,         /* The Menu key */
	GB_KEY_RETURN,       /* The Return key */
	GB_KEY_BACKSPACE,    /* The Backspace key */
	GB_KEY_TAB,          /* The Tabulation key */
	GB_KEY_PAGEUP,       /* The Page up key */
	GB_KEY_PAGEDOWN,     /* The Page down key */
	GB_KEY_END,          /* The End key */
	GB_KEY_HOME,         /* The Home key */
	GB_KEY_INSERT,       /* The Insert key */
	GB_KEY_DELETE,       /* The Delete key */
	GB_KEY_PLUS,         /* + */
	GB_KEY_SUBTRACT,     /* - */
	GB_KEY_MULTIPLY,     /* * */
	GB_KEY_DIVIDE,       /* / */
	GB_KEY_LEFT,         /* Left arrow */
	GB_KEY_RIGHT,        /* Right arrow */
	GB_KEY_UP,           /* Up arrow */
	GB_KEY_DOWN,         /* Down arrow */
	GB_KEY_NUMPAD0,      /* The numpad 0 key */
	GB_KEY_NUMPAD1,      /* The numpad 1 key */
	GB_KEY_NUMPAD2,      /* The numpad 2 key */
	GB_KEY_NUMPAD3,      /* The numpad 3 key */
	GB_KEY_NUMPAD4,      /* The numpad 4 key */
	GB_KEY_NUMPAD5,      /* The numpad 5 key */
	GB_KEY_NUMPAD6,      /* The numpad 6 key */
	GB_KEY_NUMPAD7,      /* The numpad 7 key */
	GB_KEY_NUMPAD8,      /* The numpad 8 key */
	GB_KEY_NUMPAD9,      /* The numpad 9 key */
	GB_KEY_F1,           /* The F1 key */
	GB_KEY_F2,           /* The F2 key */
	GB_KEY_F3,           /* The F3 key */
	GB_KEY_F4,           /* The F4 key */
	GB_KEY_F5,           /* The F5 key */
	GB_KEY_F6,           /* The F6 key */
	GB_KEY_F7,           /* The F7 key */
	GB_KEY_F8,           /* The F8 key */
	GB_KEY_F9,           /* The F8 key */
	GB_KEY_F10,          /* The F10 key */
	GB_KEY_F11,          /* The F11 key */
	GB_KEY_F12,          /* The F12 key */
	GB_KEY_F13,          /* The F13 key */
	GB_KEY_F14,          /* The F14 key */
	GB_KEY_F15,          /* The F15 key */
	GB_KEY_PAUSE,        /* The Pause key */

	GB_KEY_COUNT
} gbKeyType;

typedef u32 gbKeyState;
typedef enum gbKeyStateType {
	GB_KEY_STATE_DOWN     = GB_BIT(0),
	GB_KEY_STATE_PRESSED  = GB_BIT(1),
	GB_KEY_STATE_RELEASED = GB_BIT(2),
} gbKeyStateType;

typedef enum gbMouseButton {
	GB_MOUSE_BUTTON_LEFT,
	GB_MOUSE_BUTTON_MIDDLE,
	GB_MOUSE_BUTTON_RIGHT,
	GB_MOUSE_BUTTON_X1,
	GB_MOUSE_BUTTON_X2,

	GB_MOUSE_BUTTON_COUNT
} gbMouseButton;

typedef struct gbMouse {
	i32 x, y;
	i32 dx, dy;
	b8 buttons[GB_MOUSE_BUTTON_COUNT];
} gbMouse;


typedef enum gbControllerAxisType {
	GB_CONTROLLER_AXIS_LEFT_X,
	GB_CONTROLLER_AXIS_LEFT_Y,
	GB_CONTROLLER_AXIS_RIGHT_X,
	GB_CONTROLLER_AXIS_RIGHT_Y,
	GB_CONTROLLER_AXIS_LEFT_TRIGGER,
	GB_CONTROLLER_AXIS_RIGHT_TRIGGER,

	GB_CONTROLLER_AXIS_COUNT
} gbControllerAxisType;

typedef enum gbControllerButtonType {
	GB_CONTROLLER_BUTTON_UP,
	GB_CONTROLLER_BUTTON_DOWN,
	GB_CONTROLLER_BUTTON_LEFT,
	GB_CONTROLLER_BUTTON_RIGHT,
	GB_CONTROLLER_BUTTON_A,
	GB_CONTROLLER_BUTTON_B,
	GB_CONTROLLER_BUTTON_X,
	GB_CONTROLLER_BUTTON_Y,
	GB_CONTROLLER_BUTTON_LEFT_SHOULDER,
	GB_CONTROLLER_BUTTON_RIGHT_SHOULDER,
	GB_CONTROLLER_BUTTON_BACK,
	GB_CONTROLLER_BUTTON_START,

	GB_CONTROLLER_BUTTON_COUNT
} gbControllerButtonType;

typedef struct gbControllerButton {
	i32 half_transition_count;
	b32 ended_down;
} gbControllerButton;

typedef struct gbGameController {
	b16 is_connected;
	b16 is_analog;

	f32 axes[GB_CONTROLLER_AXIS_COUNT];
	gbControllerButton buttons[GB_CONTROLLER_BUTTON_COUNT];
} gbGameController;

#if defined(GB_SYSTEM_WINDOWS)
#define GB_XINPUT_GET_STATE(name) DWORD WINAPI name(DWORD dwUserIndex, XINPUT_STATE *pState)
typedef GB_XINPUT_GET_STATE(gbXInputGetStateProc);

#define GB_XINPUT_SET_STATE(name) DWORD WINAPI name(DWORD dwUserIndex, XINPUT_VIBRATION *pVibration)
typedef GB_XINPUT_SET_STATE(gbXInputSetStateProc);
#endif

typedef struct gbPlatform {
	gbWindow window;

	gbKeyState keys[GB_KEY_COUNT]; /* NOTE(bill): test with flags */
	struct {
		gbKeyState control;
		gbKeyState alt;
		gbKeyState shift;
	} key_modifiers;

	gbMouse mouse;

	gbGameController game_controllers[GB_MAX_GAME_CONTROLLER_COUNT];

	f64 curr_time;
	f64 dt_for_frame;

	b32 quit_requested;


#if defined(GB_SYSTEM_WINDOWS)
	struct {
		gbXInputGetStateProc *get_state;
		gbXInputSetStateProc *set_state;
	} xinput;
#endif
} gbPlatform;

GB_DEF void gb_platform_init   (gbPlatform *p);
GB_DEF void gb_platform_update (gbPlatform *p);
GB_DEF void gb_platform_display(gbPlatform *p);

GB_DEF void gb_platform_show_cursor       (gbPlatform *p, i32 show);
GB_DEF void gb_platform_set_mouse_position(gbPlatform *p, gbWindow *rel_win, i32 x, i32 y);

GB_DEF gbGameController *gb_platform_get_controller(gbPlatform *p, isize index);

/* NOTE(bill): Title is UTF-8 */
GB_DEF gbWindow *gb_window_init                (gbPlatform *p, char const *title, i32 w, i32 h, gbWindowType type, b32 is_fullscreen);
GB_DEF void      gb_window_set_position        (gbWindow *w, i32 x, i32 y);
GB_DEF void      gb_window_set_title           (gbWindow *w, char const *title, ...) GB_PRINTF_ARGS(2);
GB_DEF void      gb_window_toggle_fullscreen   (gbWindow *w);
GB_DEF void      gb_window_make_context_current(gbWindow *w);


#endif /* GB_PLATFORM */

#if defined(__cplusplus)
}
#endif

#endif /* GB_INCLUDE_GB_H */






/***************************************************************
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 * Implementation
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 ***************************************************************/
#if defined(GB_IMPLEMENTATION) && !defined(GB_IMPLEMENTATION_DONE)
#define GB_IMPLEMENTATION_DONE

#if defined(__cplusplus)
extern "C" {
#endif

#if 0
/* TODO(bill): Should I do this instead of windows.h or is it even really needed? */
#if defined(GB_SYSTEM_WINDOWS)
/*
 * Ginger Bill's Mini Windows.h
 * To avoid including windows.h
 *
 * NOTE(bill): These definitions aren't exactly the same so if you want to include windows.h,
 * make sure you do it before this!
 */

#include <process.h>

#ifndef _WINDOWS_  /* check windows.h guard */
#define INFINITE              0xffffffff
#define INVALID_HANDLE_VALUE  cast(void *)(-1)
#define EXCEPTION_EXECUTE_HANDLER 1


#if defined(GB_ARCH_64_BIT)
typedef u64 ULONG_PTR;
#else
typedef u32 ULONG_PTR;
#endif

typedef union _ULARGE_INTEGER {
	struct {
		DWORD LowPart;
		DWORD HighPart;
	};
	struct {
		DWORD LowPart;
		DWORD HighPart;
	} u;
	DWORD long QuadPart;
} ULARGE_INTEGER;

typedef union _LARGE_INTEGER {
	struct {
		DWORD LowPart;
		long  HighPart;
	};
	struct {
		DWORD LowPart;
		long  HighPart;
	} u;
	long long QuadPart;
} LARGE_INTEGER;

typedef enum _GET_FILEEX_INFO_LEVELS {
	GetFileExInfoStandard,
	GetFileExMaxInfoLevel
} GET_FILEEX_INFO_LEVELS;

typedef struct _FILETIME {
	DWORD dwLowDateTime;
	DWORD dwHighDateTime;
} FILETIME;

typedef struct _WIN32_FILE_ATTRIBUTE_DATA {
	DWORD    dwFileAttributes;
	FILETIME ftCreationTime;
	FILETIME ftLastAccessTime;
	FILETIME ftLastWriteTime;
	DWORD    nFileSizeHigh;
	DWORD    nFileSizeLow;
} WIN32_FILE_ATTRIBUTE_DATA;

typedef DWORD (__stdcall *gb__Win32ThreadProc)(void *arg);

GB_DLL_IMPORT b32    __stdcall CloseHandle(void *);
GB_DLL_IMPORT void   __stdcall Sleep(DWORD);
GB_DLL_IMPORT void * __stdcall CreateSemaphoreA(void *sec, long,long,char*);
GB_DLL_IMPORT DWORD  __stdcall WaitForSingleObject(void *, DWORD);
GB_DLL_IMPORT b32    __stdcall ReleaseSemaphore(void *, long, long *);
GB_DLL_IMPORT void * __stdcall CreateThread(void *, size_t, gb__Win32ThreadProc, void *, DWORD, DWORD *);
GB_DLL_IMPORT b32    __stdcall TerminateThread(void *, DWORD);
GB_DLL_IMPORT DWORD  __stdcall GetCurrentThreadId(void);
GB_DLL_IMPORT DWORD  __stdcall GetThreadId(void *);
GB_DLL_IMPORT void   __stdcall RaiseException(DWORD, DWORD, DWORD, ULONG_PTR const *);
GB_DLL_IMPORT void * __stdcall VirtualAlloc(void *base_address, size_t size, DWORD type, DWORD protect);
GB_DLL_IMPORT b32    __stdcall VirtualFree(void *base_address, size_t size, DWORD freetype);


GB_DLL_IMPORT b32    __stdcall GetFileAttributesExA(char const *filename, GET_FILEEX_INFO_LEVELS info_level_id, void *file_info);
GB_DLL_IMPORT b32    __stdcall CopyFileA(char const *existing_filename, char const *new_filename, b32 fail_if_exists);
GB_DLL_IMPORT b32    __stdcall MoveFileA(char const *existing_filename, char const *new_filename);

GB_DLL_IMPORT void *    __stdcall LoadLibraryA  (char const *filename);
GB_DLL_IMPORT void      __stdcall FreeLibrary   (void *dll);
GB_DLL_IMPORT gbDllProc __stdcall GetProcAddress(void *dll, char const *proc_name);


GB_DLL_IMPORT void __stdcall QueryPerformanceFrequency(LARGE_INTEGER *freq);
GB_DLL_IMPORT void __stdcall QueryPerformanceCounter  (LARGE_INTEGER *counter);

GB_DLL_IMPORT void __stdcall ExitProcess(unsigned int exit_code);

GB_DLL_IMPORT void __stdcall SetEnvironmentVariableA(char const *name, char const *value);

GB_DLL_IMPORT void __stdcall _getcwd(char *, i32);
GB_DLL_IMPORT i32  __stdcall _chdir (char const *path);

#endif /* _WINDOWS_ */

#endif
#endif





#if defined(__GCC__) || defined(__GNUC__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wattributes"
#endif


#if defined(_MSC_VER)
#pragma warning(push)
#pragma warning(disable:4127)
#endif

isize
gb_printf(char const *fmt, ...)
{
	isize res;
	va_list va;
	va_start(va, fmt);
	res = gb_printf_va(fmt, va);
	va_end(va);
	return res;
}


isize
gb_printf_err(char const *fmt, ...)
{
	isize res;
	va_list va;
	va_start(va, fmt);
	res = gb_printf_err_va(fmt, va);
	va_end(va);
	return res;
}

isize
gb_fprintf(struct gbFile *f, char const *fmt, ...)
{
	isize res;
	va_list va;
	va_start(va, fmt);
	res = gb_fprintf_va(f, fmt, va);
	va_end(va);
	return res;
}

char *
gb_sprintf(char const *fmt, ...)
{
	va_list va;
	char *str;
	va_start(va, fmt);
	str = gb_sprintf_va(fmt, va);
	va_end(va);
	return str;
}

isize
gb_snprintf(char *str, isize n, char const *fmt, ...)
{
	isize res;
	va_list va;
	va_start(va, fmt);
	res = gb_snprintf_va(str, n, fmt, va);
	va_end(va);
	return res;
}



gb_inline isize
gb_printf_va(char const *fmt, va_list va)
{
	gbFile f = gb_file_get_std(GB_FILE_STANDARD_OUTPUT);
	return gb_fprintf_va(&f, fmt, va);
}

gb_inline isize
gb_printf_err_va(char const *fmt, va_list va)
{
	gbFile f = gb_file_get_std(GB_FILE_STANDARD_ERROR);
	return gb_fprintf_va(&f, fmt, va);
}

gb_inline isize
gb_fprintf_va(struct gbFile *f, char const *fmt, va_list va)
{
	gb_local_persist char buf[4096];
	isize len = gb_snprintf_va(buf, gb_size_of(buf), fmt, va);
	gb_file_write(f, buf, len);
	return len;
}


gb_inline char *
gb_sprintf_va(char const *fmt, va_list va)
{
	gb_local_persist char buffer[1024];
	gb_snprintf_va(buffer, gb_size_of(buffer), fmt, va);
	return buffer;
}


enum {
	GB__FMT_MINUS     = GB_BIT(0),
	GB__FMT_PLUS      = GB_BIT(1),
	GB__FMT_ALT       = GB_BIT(2),
	GB__FMT_SPACE     = GB_BIT(3),
	GB__FMT_ZERO      = GB_BIT(4),

	GB__FMT_CHAR      = GB_BIT(5),
	GB__FMT_SHORT     = GB_BIT(6),
	GB__FMT_INT       = GB_BIT(7),
	GB__FMT_LONG      = GB_BIT(8),
	GB__FMT_LLONG     = GB_BIT(9),
	GB__FMT_SIZE      = GB_BIT(10),
	GB__FMT_INTPTR    = GB_BIT(11),

	GB__FMT_FLOAT     = GB_BIT(12),
	GB__FMT_DOUBLE    = GB_BIT(13),

	GB__FMT_UNSIGNED  = GB_BIT(14),
	GB__FMT_LOWER     = GB_BIT(15),
	GB__FMT_UPPER     = GB_BIT(16),


	GB__FMT_DONE      = GB_BIT(30)
};

typedef struct {
	i32 base;
	i32 flags;
	i32 width;
	i32 precision;
} gbFmtInfo;


gb_internal isize
gb__print_string(char *text, isize max_len, gbFmtInfo *info, char const *str)
{
	/* TODO(bill): Get precision and width to work correctly. How does it actually work?! */
	/* TODO(bill): This looks very buggy indeed. */
	isize res = 0;
	isize len;

	if (info && info->precision >= 0)
		len = gb_strnlen(str, info->precision);
	else
		len = gb_strlen(str);

	if (info && (info->width == 0 || info->flags & GB__FMT_MINUS)) {
		if (info->precision > 0)
			len = info->precision < len ? info->precision : len;

		res += gb_strlcpy(text, str, len);

		if (info->width > res) {
			isize padding = info->width - len;
			char pad = (info->flags & GB__FMT_ZERO) ? '0' : ' ';
			while (padding --> 0)
				*text++ = pad, res++;
		}
	} else {
		if (info && (info->width > res)) {
			isize padding = info->width - len;
			char pad = (info->flags & GB__FMT_ZERO) ? '0' : ' ';
			while (padding --> 0)
				*text++ = pad, res++;
		}

		res += gb_strlcpy(text, str, len);
	}


	if (info) {
		if (info->flags & GB__FMT_UPPER)
			gb_str_to_upper(text);
		else if (info->flags & GB__FMT_LOWER)
			gb_str_to_lower(text);
	}

	return res;
}

gb_internal isize
gb__print_char(char *text, isize max_len, gbFmtInfo *info, char arg)
{
	char str[2] = "";
	str[0] = arg;
	return gb__print_string(text, max_len, info, str);
}


gb_internal isize
gb__print_i64(char *text, isize max_len, gbFmtInfo *info, i64 value)
{
	char num[130];
	gb_i64_to_str(value, num, info ? info->base : 10);
	return gb__print_string(text, max_len, info, num);
}

gb_internal isize
gb__print_u64(char *text, isize max_len, gbFmtInfo *info, u64 value)
{
	char num[130];
	gb_u64_to_str(value, num, info ? info->base : 10);
	return gb__print_string(text, max_len, info, num);
}


gb_internal isize
gb__print_f64(char *text, isize max_len, gbFmtInfo *info, f64 arg)
{
	/* TODO(bill): Handle exponent notation */
	isize width, len, remaining = max_len;
	char *text_begin = text;

	if (arg) {
		u64 value;
		if (arg < 0) {
			if (remaining > 1)
				*text = '-', remaining--;
			text++;
			arg = -arg;
		} else if (info->flags & GB__FMT_MINUS) {
			if (remaining > 1)
				*text = '+', remaining--;
			text++;
		}

		value = cast(u64)arg;
		len = gb__print_u64(text, remaining, NULL, value);
		text += len;

		if (len >= remaining)
			remaining = gb_min(remaining, 1);
		else
			remaining -= len;
		arg -= value;

		if (info->precision < 0)
			info->precision = 6;

		if ((info->flags & GB__FMT_ALT) || info->precision > 0) {
			i64 mult = 10;
			if (remaining > 1)
				*text = '.', remaining--;
			text++;
			while (info->precision-- > 0) {
				value = cast(u64)(arg * mult);
				len = gb__print_u64(text, remaining, NULL, value);
				text += len;
				if (len >= remaining)
					remaining = gb_min(remaining, 1);
				else
					remaining -= len;
				arg -= cast(f64)value / mult;
				mult *= 10;
			}
		}
	} else {
		if (remaining > 1)
			*text = '0', remaining--;
		text++;
		if (info->flags & GB__FMT_ALT) {
			if (remaining > 1)
				*text = '.', remaining--;
			text++;
		}
	}

	width = info->width - (text - text_begin);
	if (width > 0) {
		char fill = (info->flags & GB__FMT_ZERO) ? '0' : ' ';
		char *end = text+remaining-1;
		len = (text - text_begin);

		for (len = (text - text_begin); len--; ) {
			if ((text_begin+len+width) < end)
				*(text_begin+len+width) = *(text_begin+len);
		}

		len = width;
		text += len;
		if (len >= remaining)
			remaining = gb_min(remaining, 1);
		else
			remaining -= len;

		while (len--) {
			if (text_begin+len < end)
				text_begin[len] = fill;
		}
	}

	return (text - text_begin);
}



gb_no_inline isize
gb_snprintf_va(char *text, isize max_len, char const *fmt, va_list va)
{
	char const *text_begin = text;
	isize remaining = max_len;

	while (*fmt) {
		gbFmtInfo info = {0};
		isize len = 0;
		info.precision = -1;

		while (*fmt && *fmt != '%' && remaining)
			*text++ = *fmt++;

		if (*fmt == '%') {
			do {
				switch (*fmt++) {
				case '-':
					info.flags |= GB__FMT_MINUS;
					fmt++;
					break;
				case '+':
					info.flags |= GB__FMT_PLUS;
					fmt++;
					break;
				case '#':
					info.flags |= GB__FMT_ALT;
					fmt++;
					break;
				case ' ':
					info.flags |= GB__FMT_SPACE;
					fmt++;
					break;
				case '0':
					info.flags |= GB__FMT_ZERO;
					fmt++;
					break;
				default:
					info.flags |= GB__FMT_DONE;
					break;
				}
			} while (!(info.flags & GB__FMT_DONE));
		}

		/* NOTE(bill): Optional Width */
		if (*fmt == '*') {
			int width = va_arg(va, int);
			if (width < 0) {
				info.flags |= GB__FMT_MINUS;
				info.width = -info.width;
			} else {
				info.width = -info.width;
			}
			fmt++;
		} else {
			info.width = cast(i32)gb_str_to_i64(fmt, cast(char **)&fmt, 10);
		}

		/* NOTE(bill): Optional Precision */
		if (*fmt == '.') {
				fmt++;
			if (*fmt == '*') {
				info.precision = va_arg(va, int);
				fmt++;
			} else {
				info.precision = cast(i32)gb_str_to_i64(fmt, cast(char **)&fmt, 10);
			}
			info.flags &= ~GB__FMT_ZERO;
		}

		switch (*fmt++) {
		case 'h':
			if (*fmt == 'h') {
				/* hh => char */
				info.flags |= GB__FMT_CHAR;
				fmt++;
			} else {
				/* h => short */
				info.flags |= GB__FMT_SHORT;
			}
			break;

		case 'l':
			if (*fmt == 'l') {
				/* ll => long long */
				info.flags |= GB__FMT_LLONG;
				fmt++;
			} else {
				/* l => long */
				info.flags |= GB__FMT_LONG;
			}
			break;

			break;

		case 'z': /* NOTE(bill): usize */
			info.flags |= GB__FMT_UNSIGNED;
			/* FALLTHROUGH */
		case 't': /* NOTE(bill): isize */
			info.flags |= GB__FMT_SIZE;
			break;

		default:
			fmt--;
			break;
		}


		switch (*fmt) {
		case 'u':
			info.flags |= GB__FMT_UNSIGNED;
			/* FALLTHROUGH */
		case 'd':
		case 'i':
			info.base = 10;
			break;

		case 'o':
			info.base = 8;
			break;

		case 'x':
			info.base = 16;
			info.flags |= (GB__FMT_UNSIGNED | GB__FMT_LOWER);
			break;

		case 'X':
			info.base = 16;
			info.flags |= (GB__FMT_UNSIGNED | GB__FMT_UPPER);
			break;

		case 'f':
		case 'F':
		case 'g':
		case 'G':
			len = gb__print_f64(text, remaining, &info, va_arg(va, f64));
			break;

		case 'a':
		case 'A':
			/* TODO(bill): */
			break;

		case 'c':
			len = gb__print_char(text, remaining, &info, cast(char)va_arg(va, int));
			break;

		case 's':
			len = gb__print_string(text, remaining, &info, va_arg(va, char *));
			break;

		case 'p':
			info.base = 16;
			info.flags |= (GB__FMT_LOWER|GB__FMT_UNSIGNED|GB__FMT_ALT|GB__FMT_INTPTR);
			break;

		default:
			fmt--;
			break;
		}

		fmt++;


		if (info.base != 0) {
			if (info.flags & GB__FMT_UNSIGNED) {
				u64 value = 0;
				if (info.flags & GB__FMT_CHAR)
					value = cast(u64)cast(u8)va_arg(va, int);
				else if (info.flags & GB__FMT_SHORT)
					value = cast(u64)cast(u16)va_arg(va, int);
				else if (info.flags & GB__FMT_LONG)
					value = cast(u64)va_arg(va, unsigned long);
				else if (info.flags & GB__FMT_LLONG)
					value = cast(u64)va_arg(va, unsigned long long);
				else if (info.flags & GB__FMT_SIZE)
					value = cast(u64)va_arg(va, usize);
				else if (info.flags & GB__FMT_INTPTR)
					value = cast(u64)va_arg(va, uintptr);
				else
					value = cast(u64)va_arg(va, int);

				len = gb__print_u64(text, remaining, &info, value);

			} else {
				i64 value = 0;
				if (info.flags & GB__FMT_CHAR)
					value = cast(i64)cast(i8)va_arg(va, int);
				else if (info.flags & GB__FMT_SHORT)
					value = cast(i64)cast(i16)va_arg(va, int);
				else if (info.flags & GB__FMT_LONG)
					value = cast(i64)va_arg(va, long);
				else if (info.flags & GB__FMT_LLONG)
					value = cast(i64)va_arg(va, long long);
				else if (info.flags & GB__FMT_SIZE)
					value = cast(i64)va_arg(va, isize);
				else if (info.flags & GB__FMT_INTPTR)
					value = cast(i64)va_arg(va, intptr);
				else
					value = cast(i64)va_arg(va, int);

				len = gb__print_i64(text, remaining, &info, value);
            }
		}


		text += len;
		if (len >= remaining)
			remaining = gb_min(remaining, 1);
		else
			remaining -= len;
	}

	*text++ = '\0';

	{
		isize res = (text - text_begin);
		return (res >= max_len || res < 0) ? -1 : res;
	}
}



void
gb_assert_handler(char const *condition, char const *file, i32 line, char const *msg)
{
	gb_printf_err("%s:%d: Assert Failure: ", file, line);
	if (condition)
		gb_printf_err( "`%s` ", condition);
	if (msg)
		gb_printf_err("%s", msg);
	gb_printf_err("\n");
}


gb_inline isize
gb_is_power_of_two(isize x)
{
	return (x != 0) && !(x & (x-1));
}



gb_inline void *
gb_align_forward(void *ptr, isize alignment)
{
	uintptr p;
	isize modulo;

	GB_ASSERT(gb_is_power_of_two(alignment));

	p = cast(uintptr)ptr;
	modulo = p & (alignment-1);
	if (modulo) p += (alignment - modulo);
	return cast(void *)p;
}



gb_inline void *      gb_pointer_add      (void *ptr, isize bytes)             { return cast(void *)(cast(u8 *)ptr + bytes); }
gb_inline void *      gb_pointer_sub      (void *ptr, isize bytes)             { return cast(void *)(cast(u8 *)ptr - bytes); }
gb_inline void const *gb_pointer_add_const(void const *ptr, isize bytes)       { return cast(void const *)(cast(u8 const *)ptr + bytes); }
gb_inline void const *gb_pointer_sub_const(void const *ptr, isize bytes)       { return cast(void const *)(cast(u8 const *)ptr - bytes); }
gb_inline isize       gb_pointer_diff     (void const *begin, void const *end) { return cast(isize)(cast(u8 const *)end - cast(u8 const *)begin); }

gb_inline void gb_zero_size(void *ptr, isize size) { gb_memset(ptr, 0, size); }

#if defined(_MSC_VER)
#pragma intrinsic(__movsb)
#endif

gb_inline void *
gb_memcopy(void *gb_restrict dest, void const *gb_restrict source, isize size)
{
#if defined(_MSC_VER)
	__movsb(cast(u8 *gb_restrict)dest, cast(u8 *gb_restrict)source, size);
#elif (defined(__i386__) || defined(__x86_64___))
	__asm__ __volatile__("rep movsb" : "+D"(cast(u8 *gb_restrict)dest), "+S"(cast(u8 *gb_restrict)source), "+c"(size) : : "memory");
#else
	/* TODO(bill): Heavily optimize */
	if ((cast(intptr)dest & 0x3) || (cast(intptr)source & 0x3)) {
		/* NOTE(bill): Do an unaligned byte copy */
		u8 *gb_restrict dp8 = cast(u8 *)dest;
		u8 *sp8 = cast(u8 *)source;

		while (size--)
			*dp8++ = *sp8++;

	} else {
		isize left = (size % 4);
		u32 *sp32;
		u32 *dp32;
		u8  *sp8;
		u8  *dp8;

		sp32 = cast(u32 *)source;
		dp32 = cast(u32 *)dest;
		size /= 4;
		while (size--)
			*dp32++ = *sp32++;

		sp8 = cast(u8 *)sp32;
		dp8 = cast(u8 *)dp32;
		switch (left) {
		case 3: *dp8++ = *sp8++;
		case 2: *dp8++ = *sp8++;
		case 1: *dp8++ = *sp8++;
		}
	}

	/* TODO(bill): More betterer memcpys!!!! */
#endif
	return dest;
}

gb_inline void *
gb_memmove(void *dest, void const *source, isize size)
{
	/* TODO(bill): Heavily optimize */
	u8 *dp8 = cast(u8 *)dest;
	u8 *sp8 = cast(u8 *)source;

	if (sp8 < dp8) {
		dp8 += size-1;
		sp8 += size-1;
		while (size--)
			*dp8-- = *sp8--;
	} else {
		gb_memcopy(dest, source, size);
	}

	return dest;
}

gb_inline void *
gb_memset(void *data, u8 c, isize size)
{
	/* TODO(bill): Heavily optimize */
	isize left;
	u32 *dp32;
	u8 *dp8 = cast(u8 *)data;
	u32 c32 = (c | (c << 8) | (c << 16) | (c << 24));

	/* The destination pointer needs to be aligned on a 4-byte boundary to
	* execute a 32-bit set. Set first bytes manually if needed until it is
	* aligned. */
	while (cast(intptr)dp8 & 0x3) {
		if (size--)
			*dp8++ = c;
		else
			return data;
	}

	dp32 = cast(u32 *)dp8;
	left = (size % 4);
	size /= 4;
	while (size--)
		*dp32++ = c32;

	dp8 = cast(u8 *)dp32;
	switch (left) {
	case 3: *dp8++ = c;
	case 2: *dp8++ = c;
	case 1: *dp8++ = c;
	}

	return data;
}

gb_inline i32
gb_memcompare(void const *s1, void const *s2, isize size)
{
	/* TODO(bill): Heavily optimize */

	u8 const *s1p8 = cast(u8 const *)s1;
	u8 const *s2p8 = cast(u8 const *)s2;
	while (size--) {
		if (*s1p8 != *s2p8)
			return (*s1p8 - *s2p8);
		s1p8++, s2p8++;
	}
	return 0;
}

void
gb_memswap(void *i, void *j, isize size)
{
	if (i == j) return;

	if (size == 4) {
		gb_swap(u32, *cast(u32 *)i, *cast(u32 *)j);
	} else if (size == 8) {
		gb_swap(u64, *cast(u64 *)i, *cast(u64 *)j);
	} else if (size < 8) {
		u8 *a = cast(u8 *)i;
		u8 *b = cast(u8 *)j;
		if (a != b) {
			while (size--) {
				gb_swap(u8, *a, *b);
				a++, b++;
			}
		}
	} else {
		char buffer[256];

		/* TODO(bill): Is the recursion ever a problem? */
		while (size > gb_size_of(buffer)) {
			gb_memswap(i, j, gb_size_of(buffer));
			i = gb_pointer_add(i, gb_size_of(buffer));
			j = gb_pointer_add(j, gb_size_of(buffer));
			size -= gb_size_of(buffer);
		}

		gb_memcopy(buffer, i,      size);
		gb_memcopy(i,      j,      size);
		gb_memcopy(j,      buffer, size);
	}
}



gb_inline void *gb_alloc_align (gbAllocator a, isize size, isize alignment)                                { return a.proc(a.data, GB_ALLOCATION_ALLOC, size, alignment, NULL, 0, 0); }
gb_inline void *gb_alloc       (gbAllocator a, isize size)                                                 { return gb_alloc_align(a, size, GB_DEFAULT_MEMORY_ALIGNMENT); }
gb_inline void  gb_free        (gbAllocator a, void *ptr)                                                  { a.proc(a.data, GB_ALLOCATION_FREE, 0, 0, ptr, 0, 0); }
gb_inline void  gb_free_all    (gbAllocator a)                                                             { a.proc(a.data, GB_ALLOCATION_FREE_ALL, 0, 0, NULL, 0, 0); }
gb_inline void *gb_resize      (gbAllocator a, void *ptr, isize old_size, isize new_size)                  { return gb_resize_align(a, ptr, old_size, new_size, GB_DEFAULT_MEMORY_ALIGNMENT); }
gb_inline void *gb_resize_align(gbAllocator a, void *ptr, isize old_size, isize new_size, isize alignment) { return a.proc(a.data, GB_ALLOCATION_RESIZE, new_size, alignment, ptr, old_size, 0); }

gb_inline void *gb_alloc_copy      (gbAllocator a, void const *src, isize size)                  { return gb_memcopy(gb_alloc(a, size), src, size); }
gb_inline void *gb_alloc_copy_align(gbAllocator a, void const *src, isize size, isize alignment) { return gb_memcopy(gb_alloc_align(a, size, alignment), src, size); }

gb_inline char *
gb_alloc_str(gbAllocator a, char const *str)
{
	char *result;
	isize len = gb_strlen(str);
	result = cast(char *)gb_alloc_copy(a, str, len+1);
	result[len] = '\0';
	return result;
}

gb_inline void *
gb_default_resize_align(gbAllocator a, void *old_memory, isize old_size, isize new_size, isize alignment)
{
	if (!old_memory) return gb_alloc_align(a, new_size, alignment);

	if (new_size < old_size)
		new_size = old_size;

	if (old_size == new_size) {
		return old_memory;
	} else {
		void *new_memory = gb_alloc_align(a, new_size, alignment);
		if (!new_memory) return NULL;
		gb_memmove(new_memory, old_memory, gb_min(new_size, old_size));
		gb_free(a, old_memory);
		return new_memory;
	}
}




/***************************************************************
 *
 * Concurrency
 *
 */
#if defined(_MSC_VER) && !defined(__clang__)
gb_inline i32  gb_atomic32_load (gbAtomic32 const volatile *a)      { return a->value;  }
gb_inline void gb_atomic32_store(gbAtomic32 volatile *a, i32 value) { a->value = value; }
gb_inline i32
gb_atomic32_compare_exchange(gbAtomic32 volatile *a, i32 expected, i32 desired)
{
	return _InterlockedCompareExchange(cast(long volatile *)a, desired, expected);
}
gb_inline i32
gb_atomic32_exchanged(gbAtomic32 volatile *a, i32 desired)
{
	return _InterlockedExchange(cast(long volatile *)a, desired);
}
gb_inline i32
gb_atomic32_fetch_add(gbAtomic32 volatile *a, i32 operand)
{
	return _InterlockedExchangeAdd(cast(long volatile *)a, operand);
}
gb_inline i32
gb_atomic32_fetch_and(gbAtomic32 volatile *a, i32 operand)
{
	return _InterlockedAnd(cast(long volatile *)a, operand);
}
gb_inline i32
gb_atomic32_fetch_or(gbAtomic32 volatile *a, i32 operand)
{
	return _InterlockedOr(cast(long volatile *)a, operand);
}


gb_inline i64
gb_atomic64_load(gbAtomic64 const volatile *a)
{
#if defined(GB_ARCH_64_BIT)
	return a->value;
#else
	/* NOTE(bill): The most compatible way to get an atomic 64-bit load on x86 is with cmpxchg8b */
	i64 result;
	__asm {
		mov esi, a;
		mov ebx, eax;
		mov ecx, edx;
		lock cmpxchg8b [esi];
		mov dword ptr result, eax;
		mov dword ptr result[4], edx;
	}
	return result;
#endif
}

gb_inline void
gb_atomic64_store(gbAtomic64 volatile *a, i64 value)
{
#if defined(GB_ARCH_64_BIT)
	a->value = value;
#else
	/* NOTE(bill): The most compatible way to get an atomic 64-bit store on x86 is with cmpxchg8b */
	__asm {
		mov esi, a;
		mov ebx, dword ptr value;
		mov ecx, dword ptr value[4];
	retry:
		cmpxchg8b [esi];
		jne retry;
	}
#endif
}

gb_inline i64
gb_atomic64_compare_exchange(gbAtomic64 volatile *a, i64 expected, i64 desired)
{
	return _InterlockedCompareExchange64(cast(i64 volatile *)a, desired, expected);
}

gb_inline i64
gb_atomic64_exchanged(gbAtomic64 volatile *a, i64 desired)
{
#if defined(GB_ARCH_64_BIT)
	return _InterlockedExchange64(cast(i64 volatile *)a, desired);
#else
	i64 expected = a->value;
	for (;;) {
		i64 original = _InterlockedCompareExchange64(cast(i64 volatile *)a, desired, expected);
		if (original == expected)
			return original;
		expected = original;
	}
#endif
}

gb_inline i64
gb_atomic64_fetch_add(gbAtomic64 volatile *a, i64 operand)
{
#if defined(GB_ARCH_64_BIT)
	return _InterlockedExchangeAdd64(cast(i64 volatile *)a, operand);
#else
	i64 expected = a->value;
	for (;;) {
		i64 original = _InterlockedCompareExchange64(cast(i64 volatile *)a, expected + operand, expected);
		if (original == expected)
			return original;
		expected = original;
	}
#endif
}

gb_inline i64
gb_atomic64_fetch_and(gbAtomic64 volatile *a, i64 operand)
{
#if defined(GB_ARCH_64_BIT)
	return _InterlockedAnd64(cast(i64 volatile *)a, operand);
#else
	i64 expected = a->value;
	for (;;) {
		i64 original = _InterlockedCompareExchange64(cast(i64 volatile *)a, expected & operand, expected);
		if (original == expected)
			return original;
		expected = original;
	}
#endif
}

gb_inline i64
gb_atomic64_fetch_or(gbAtomic64 volatile *a, i64 operand)
{
#if defined(GB_ARCH_64_BIT)
	return _InterlockedAnd64(cast(i64 volatile *)a, operand);
#else
	i64 expected = a->value;
	for (;;) {
		i64 original = _InterlockedCompareExchange64(cast(i64 volatile *)a, expected | operand, expected);
		if (original == expected)
			return original;
		expected = original;
	}
#endif
}



#else /* GCC */


gb_inline i32  gb_atomic32_load (gbAtomic32 const volatile *a)      { return a->value;  }
gb_inline void gb_atomic32_store(gbAtomic32 volatile *a, i32 value) { a->value = value; }

gb_inline i32
gb_atomic32_compare_exchange(gbAtomic32 volatile *a, i32 expected, i32 desired)
{
	i32 original;
	__asm__ volatile(
		"lock; cmpxchgl %2, %1"
		: "=a"(original), "+m"(a->value)
		: "q"(desired), "0"(expected)
	);
	return original;
}

gb_inline i32
gb_atomic32_exchanged(gbAtomic32 volatile *a, i32 desired)
{
	/* NOTE(bill): No lock prefix is necessary for xchgl */
	i32 original;
	__asm__ volatile(
		"xchgl %0, %1"
		: "=r"(original), "+m"(a->value)
		: "0"(desired)
	);
	return original;
}

gb_inline i32
gb_atomic32_fetch_add(gbAtomic32 volatile *a, i32 operand)
{
	i32 original;
	__asm__ volatile(
		"lock; xaddl %0, %1"
		: "=r"(original), "+m"(a->value)
		: "0"(operand)
	);
    return original;
}

gb_inline i32
gb_atomic32_fetch_and(gbAtomic32 volatile *a, i32 operand)
{
	i32 original;
	i32 tmp;
	__asm__ volatile(
		"1:     movl    %1, %0\n"
		"       movl    %0, %2\n"
		"       andl    %3, %2\n"
		"       lock; cmpxchgl %2, %1\n"
		"       jne     1b"
		: "=&a"(original), "+m"(a->value), "=&r"(tmp)
		: "r"(operand)
	);
	return original;
}

gb_inline i32
gb_atomic32_fetch_or(gbAtomic32 volatile *a, i32 operand)
{
	i32 original;
	i32 temp;
	__asm__ volatile(
		"1:     movl    %1, %0\n"
		"       movl    %0, %2\n"
		"       orl     %3, %2\n"
		"       lock; cmpxchgl %2, %1\n"
		"       jne     1b"
		: "=&a"(original), "+m"(a->value), "=&r"(temp)
		: "r"(operand)
	);
	return original;
}


gb_inline i64
gb_atomic64_load(gbAtomic64 const volatile *a)
{
#if defined(GB_ARCH_64_BIT)
	return a->value;
#else
	i64 original;
	__asm__ volatile(
		"movl %%ebx, %%eax\n"
		"movl %%ecx, %%edx\n"
		"lock; cmpxchg8b %1"
		: "=&A"(original)
		: "m"(a->value)
	);
	return original;
#endif
}

gb_inline void
gb_atomic64_store(gbAtomic64 volatile *a, i64 value)
{
#if defined(GB_ARCH_64_BIT)
	a->value = value;
#else
	i64 expected = a->value;
	__asm__ volatile(
		"1:    cmpxchg8b %0\n"
		"      jne 1b"
		: "=m"(a->value)
		: "b"((i32)value), "c"((i32)(value >> 32)), "A"(expected)
	);
#endif
}

gb_inline i64
gb_atomic64_compare_exchange(gbAtomic64 volatile *a, i64 expected, i64 desired)
{
#if defined(GB_ARCH_64_BIT)
	i64 original;
	__asm__ volatile(
		"lock; cmpxchgq %2, %1"
		: "=a"(original), "+m"(a->value)
		: "q"(desired), "0"(expected)
	);
	return original;
#else
	i64 original;
	__asm__ volatile(
		"lock; cmpxchg8b %1"
		: "=A"(original), "+m"(a->value)
		: "b"((i32)desired), "c"((i32)(desired >> 32)), "0"(expected)
	);
	return original;
#endif
}

gb_inline i64
gb_atomic64_exchanged(gbAtomic64 volatile *a, i64 desired)
{
#if defined(GB_ARCH_64_BIT)
	i64 original;
	__asm__ volatile(
		"xchgq %0, %1"
		: "=r"(original), "+m"(a->value)
		: "0"(desired)
	);
	return original;
#else
	i64 original = a->value;
	for (;;) {
		i64 previous = gb_atomic64_compare_exchange(a, original, desired);
		if (original == previous)
			return original;
		original = previous;
	}
#endif
}

gb_inline i64
gb_atomic64_fetch_add(gbAtomic64 volatile *a, i64 operand)
{
#if defined(GB_ARCH_64_BIT)
	i64 original;
	__asm__ volatile(
		"lock; xaddq %0, %1"
		: "=r"(original), "+m"(a->value)
		: "0"(operand)
	);
	return original;
#else
	for (;;) {
		i64 original = a->value;
		if (gb_atomic64_compare_exchange(a, original, original + operand) == original)
			return original;
	}
#endif
}

gb_inline i64
gb_atomic64_fetch_and(gbAtomic64 volatile *a, i64 operand)
{
#if defined(GB_ARCH_64_BIT)
	i64 original;
	i64 tmp;
	__asm__ volatile(
		"1:     movq    %1, %0\n"
		"       movq    %0, %2\n"
		"       andq    %3, %2\n"
		"       lock; cmpxchgq %2, %1\n"
		"       jne     1b"
		: "=&a"(original), "+m"(a->value), "=&r"(tmp)
		: "r"(operand)
	);
	return original;
#else
	for (;;) {
		i64 original = a->value;
		if (gb_atomic64_compare_exchange(a, original, original & operand) == original)
			return original;
	}
#endif
}

gb_inline i64
gb_atomic64_fetch_or(gbAtomic64 volatile *a, i64 operand)
{
#if defined(GB_ARCH_64_BIT)
	i64 original;
	i64 temp;
	__asm__ volatile(
		"1:     movq    %1, %0\n"
		"       movq    %0, %2\n"
		"       orq     %3, %2\n"
		"       lock; cmpxchgq %2, %1\n"
		"       jne     1b"
		: "=&a"(original), "+m"(a->value), "=&r"(temp)
		: "r"(operand)
	);
	return original;
#else
	for (;;) {
		i64 original = a->value;
		if (gb_atomic64_compare_exchange(a, original, original | operand) == original)
			return original;
	}
#endif
}
#endif

gb_inline void
gb_atomic32_spin_lock(gbAtomic32 volatile *a)
{
	a->value = 0;
	for (;;) {
		i32 expected = 0;
		if (gb_atomic32_compare_exchange(a, expected, 1))
			break;
	}
}
gb_inline void gb_atomic32_spin_unlock(gbAtomic32 volatile *a) { gb_atomic32_store(a, 0); }

gb_inline void
gb_atomic64_spin_lock(gbAtomic64 volatile *a)
{
	a->value = 0;
	for (;;) {
		i64 expected = 0;
		if (gb_atomic64_compare_exchange(a, expected, 1))
			break;
	}
}
gb_inline void gb_atomic64_spin_unlock(gbAtomic64 volatile *a) { gb_atomic64_store(a, 0); }




#if defined(GB_SYSTEM_WINDOWS)
	gb_inline void gb_semaphore_init   (gbSemaphore *s)            { s->win32_handle = CreateSemaphoreA(NULL, 0, MAXLONG, NULL); }
	gb_inline void gb_semaphore_destroy(gbSemaphore *s)            { CloseHandle(s->win32_handle); }
	gb_inline void gb_semaphore_post   (gbSemaphore *s, i32 count) { ReleaseSemaphore(s->win32_handle, count, NULL); }
	gb_inline void gb_semaphore_wait   (gbSemaphore *s)            { WaitForSingleObject(s->win32_handle, INFINITE); }

#elif defined(GB_SYSTEM_OSX)
	gb_inline void gb_semaphore_init   (gbSemaphore *s)            { semaphore_create(mach_task_self(), &s->osx_handle, SYNC_POLICY_FIFO, 0); }
	gb_inline void gb_semaphore_destroy(gbSemaphore *s)            { semaphore_destroy(mach_task_self(), &s->osx_handle); }
	gb_inline void gb_semaphore_post   (gbSemaphore *s, i32 count) { while (count --> 0) semaphore_signal(s->osx_handle); }
	gb_inline void gb_semaphore_wait   (gbSemaphore *s)            { semaphore_wait(s->osx_handle); }

#elif defined(GB_SYSTEM_UNIX)
	gb_inline void gb_semaphore_init   (gbSemaphore *s)            { sem_init(&s->unix_handle, 0, 0); }
	gb_inline void gb_semaphore_destroy(gbSemaphore *s)            { sem_destroy(&s->unix_handle); }
	gb_inline void gb_semaphore_post   (gbSemaphore *s, i32 count) { while (count --> 0) sem_post(&s->unix_handle); }
	gb_inline void gb_semaphore_wait   (gbSemaphore *s)            { int i; do { i = sem_wait(&s->unix_handle); } while (i == -1 && errno == EINTR); }

#else
#error
#endif

/* NOTE(bill): THIS IS FUCKING AWESOME THAT THIS "MUTEX" IS FAST AND RECURSIVE TOO! */
/* NOTE(bill): WHO THE FUCK NEEDS A NORMAL MUTEX NOW?!?!?!?! */
gb_inline void
gb_mutex_init(gbMutex *m)
{
	gb_atomic32_store(&m->counter, 0);
	gb_atomic32_store(&m->owner, gb_thread_current_id());
	gb_semaphore_init(&m->semaphore);
	m->recursion = 0;
}

gb_inline void gb_mutex_destroy(gbMutex *m) { gb_semaphore_destroy(&m->semaphore); }

gb_inline void
gb_mutex_lock(gbMutex *m)
{
	i32 thread_id = cast(i32)gb_thread_current_id();
	if (gb_atomic32_fetch_add(&m->counter, 1) > 0) {
		if (thread_id != gb_atomic32_load(&m->owner))
			gb_semaphore_wait(&m->semaphore);
	}

	gb_atomic32_store(&m->owner, thread_id);
	m->recursion++;
}

gb_inline b32
gb_mutex_try_lock(gbMutex *m)
{
	i32 thread_id = cast(i32)gb_thread_current_id();
	if (gb_atomic32_load(&m->owner) == thread_id) {
		gb_atomic32_fetch_add(&m->counter, 1);
	} else {
		i32 expected = 0;
		if (gb_atomic32_load(&m->counter) != 0)
			return false;
		if (!gb_atomic32_compare_exchange(&m->counter, expected, 1))
			return false;
		gb_atomic32_store(&m->owner, thread_id);
	}

	m->recursion++;
	return true;
}

gb_inline void
gb_mutex_unlock(gbMutex *m)
{
	i32 recursion;
	i32 thread_id = cast(i32)gb_thread_current_id();

	GB_ASSERT(thread_id == gb_atomic32_load(&m->owner));

	recursion = --m->recursion;
	if (recursion == 0)
		gb_atomic32_store(&m->owner, thread_id);

	if (gb_atomic32_fetch_add(&m->counter, -1) > 1) {
		if (recursion == 0)
			gb_semaphore_post(&m->semaphore, 1);
	}
}






void
gb_thread_init(gbThread *t)
{
	gb_zero_struct(t);
#if defined(GB_SYSTEM_WINDOWS)
	t->win32_handle = INVALID_HANDLE_VALUE;
#else
	t->posix_handle = 0;
#endif
	gb_semaphore_init(&t->semaphore);
}

void
gb_thread_destory(gbThread *t)
{
	if (t->is_running) gb_thread_join(t);
	gb_semaphore_destroy(&t->semaphore);
}


gb_inline void
gb__thread_run(gbThread *t)
{
	gb_semaphore_post(&t->semaphore, 1);
	t->proc(t->data);
}

#if defined(GB_SYSTEM_WINDOWS)
	gb_inline DWORD __stdcall gb__thread_proc(void *arg) { gb__thread_run(cast(gbThread *)arg); return 0; }
#else
	gb_inline void *          gb__thread_proc(void *arg) { gb__thread_run(cast(gbThread *)arg); return NULL; }
#endif

gb_inline void gb_thread_start(gbThread *t, gbThreadProc *proc, void *data) { gb_thread_start_with_stack(t, proc, data, 0); }

gb_inline void
gb_thread_start_with_stack(gbThread *t, gbThreadProc *proc, void *data, isize stack_size)
{
	GB_ASSERT(!t->is_running);
	GB_ASSERT(proc != NULL);
	t->proc = proc;
	t->data = data;
	t->stack_size = stack_size;

#if defined(GB_SYSTEM_WINDOWS)
	t->win32_handle = CreateThread(NULL, stack_size, gb__thread_proc, t, 0, NULL);
	GB_ASSERT_MSG(t->win32_handle != NULL, "CreateThread: GetLastError");
#else
	{
		pthread_attr_t attr;
		pthread_attr_init(&attr);
		pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
		if (stack_size != 0)
			pthread_attr_setstacksize(&attr, stack_size);
		pthread_create(&t->posix_handle, &attr, gb__thread_proc, t);
		pthread_attr_destroy(&attr);
	}
#endif

	t->is_running = true;
	gb_semaphore_wait(&t->semaphore);
}

gb_inline void
gb_thread_join(gbThread *t)
{
	if (!t->is_running) return;

#if defined(GB_SYSTEM_WINDOWS)
	WaitForSingleObject(t->win32_handle, INFINITE);
	CloseHandle(t->win32_handle);
	t->win32_handle = INVALID_HANDLE_VALUE;
#else
	pthread_join(t->posix_handle, NULL);
	t->posix_handle = 0;
#endif
	t->is_running = false;
}

gb_inline b32 gb_thread_is_running(gbThread const *t) { return t->is_running != 0; }

gb_inline u32
gb_thread_current_id(void)
{
	u32 thread_id;
#if defined(GB_SYSTEM_WINDOWS)
	thread_id = GetCurrentThreadId();
#elif defined(GB_SYSTEM_OSX) && defined(GB_ARCH_64_BIT)
	__asm__("mov %%gs:0x00,%0" : "=r"(thread_id));
#elif defined(GB_ARCH_32_BIT)
	__asm__("mov %%gs:0x08,%0" : "=r"(thread_id));
#elif defined(GB_ARCH_64_BIT)
	__asm__("mov %%gs:0x10,%0" : "=r"(thread_id));
#else
	#error Unsupported architecture for thread::current_id()
#endif

	return thread_id;
}



void
gb_thread_set_name(gbThread *t, char const *name)
{
#if defined(_MSC_VER)
	/* TODO(bill): Bloody Windows!!! */
	#pragma pack(push, 8)
		struct gbprivThreadName {
			DWORD      type;
			char const *name;
			DWORD      id;
			DWORD      flags;
		};
	#pragma pack(pop)
		struct gbprivThreadName tn;
		tn.type  = 0x1000;
		tn.name  = name;
		tn.id    = GetThreadId(t->win32_handle);
		tn.flags = 0;

		__try {
			RaiseException(0x406d1388, 0, gb_size_of(tn)/4, cast(ULONG_PTR *)&tn);
		} __except(EXCEPTION_EXECUTE_HANDLER) {
		}

#elif defined(GB_SYSTEM_WINDOWS) && !defined(_MSC_VER)
	/* TODO(bill): Set thread name for GCC/Clang on windows */
	return;
#elif defined(GB_SYSTEM_OSX)
	/* TODO(bill): Test if this works */
	pthread_setname_np(name);
#else
	/* TODO(bill): Test if this works */
	pthread_setname_np(t->posix_handle, name);
#endif
}





gb_inline gbAllocator
gb_heap_allocator(void)
{
	gbAllocator a;
	a.proc = gb_heap_allocator_proc;
	a.data = NULL;
	return a;
}

GB_ALLOCATOR_PROC(gb_heap_allocator_proc)
{
	gb_unused(allocator_data);
	gb_unused(options);
	gb_unused(old_size);
/* TODO(bill): Throughly test! */
	switch (type) {
#if defined(_MSC_VER)
	case GB_ALLOCATION_ALLOC:  return _aligned_malloc(size, alignment);
	case GB_ALLOCATION_FREE:   _aligned_free(old_memory); break;
	case GB_ALLOCATION_RESIZE: return _aligned_realloc(old_memory, size, alignment);
#else
	/* TODO(bill): *nix version that's decent */
	case GB_ALLOCATION_ALLOC: {
		isize total_size = size + alignment + gb_size_of(gbAllocationHeader);
		void *ptr = malloc(total_size);
		gbAllocationHeader *header = cast(gbAllocationHeader *)ptr;
		ptr = gb_align_forward(header+1, alignment);
		gb_allocation_header_fill(header, ptr, size);
		return ptr;
	} break;

	case GB_ALLOCATION_FREE: {
		free(gb_allocation_header(old_memory));
	} break;

	case GB_ALLOCATION_RESIZE: {
		gbAllocator a = gb_heap_allocator();
		return gb_default_resize_align(a, old_memory, old_size, size, alignment);
	} break;
#endif

	case GB_ALLOCATION_FREE_ALL:
		break;
	}

	return NULL; /* NOTE(bill): Default return value */
}


/***************************************************************
 *
 * Virtual Memory
 *
 */

gbVirtualMemory
gb_virtual_memory(void *data, isize size)
{
	gbVirtualMemory vm;
	vm.data = data;
	vm.size = size;
	return vm;
}


#if defined(GB_SYSTEM_WINDOWS)
gb_inline gbVirtualMemory
gb_vm_alloc(void *addr, isize size)
{
	gbVirtualMemory vm;
	GB_ASSERT(size > 0);
	vm.data = VirtualAlloc(addr, size,
	                       0x00001000 | 0x00002000, /* MEM_COMMIT | MEM_RESERVE */
	                       0x04 /* PAGE_READWRITE */);
	vm.size = size;
	return vm;
}

gb_inline void
gb_vm_free(gbVirtualMemory vm)
{
	VirtualFree(vm.data, vm.size > 0 ? vm.size : 0, 0x8000);
}

gb_inline gbVirtualMemory
gb_vm_trim(gbVirtualMemory vm, isize lead_size, isize size)
{
	gbVirtualMemory new_vm = {0};
	void *ptr;
	GB_ASSERT(vm.size >= lead_size + size);

	ptr = gb_pointer_add(vm.data, lead_size);

	gb_vm_free(vm);
	new_vm = gb_vm_alloc(ptr, size);
	if (new_vm.data == ptr)
		return new_vm;
	if (new_vm.data)
		gb_vm_free(new_vm);
	return new_vm;
}

gb_inline b32
gb_vm_purge(gbVirtualMemory vm)
{
	VirtualAlloc(vm.data, vm.size,
	             0x00080000, /* MEM_RESET */
	             0x04 /* PAGE_READWRITE */);
	/* NOTE(bill): Can this really fail? */
	return true;
}
#else

#ifndef MAP_ANONYMOUS
#define MAP_ANONYMOUS MAP_ANON
#endif

gb_inline gbVirtualMemory
gb_vm_alloc(void *addr, isize size)
{
	gbVirtualMemory vm;
	GB_ASSERT(size > 0);
	vm.data = mmap(addr, size,
	               PROT_READ | PROT_WRITE,
	               MAP_ANONYMOUS | MAP_PRIVATE,
	               -1, 0);
	vm.size = size;
	return vm;
}

gb_inline void
gb_vm_free(gbVirtualMemory vm)
{
	munmap(vm.data, vm.size);
}

gb_inline gbVirtualMemory
gb_vm_trim(gbVirtualMemory vm, isize lead_size, isize size)
{
	void *ptr;
	isize trail_size;
	GB_ASSERT(vm.size >= lead_size + size);

	ptr = gb_pointer_add(vm.data, lead_size);
	trail_size = vm.size - lead_size - size;

	if (lead_size != 0)
		gb_vm_free(gb_virtual_memory(vm.data, lead_size));
	if (trail_size != 0)
		gb_vm_free(gb_virtual_memory(ptr, trail_size));
	return gb_virtual_memory(ptr, size);

}

gb_inline b32
gb_vm_purge(gbVirtualMemory vm)
{
	int err = madvise(vm.data, vm.size, MADV_DONTNEED);
	return err != 0;
}
#endif




/***************************************************************
 *
 * Custom Allocation
 *
 */


/*
 * Arena Allocator
 */

gb_inline void
gb_arena_init_from_memory(gbArena *arena, void *start, isize size)
{
	arena->backing.proc    = NULL;
	arena->backing.data    = NULL;
	arena->physical_start  = start;
	arena->total_size      = size;
	arena->total_allocated = 0;
	arena->temp_count      = 0;
}

gb_inline void
gb_arena_init_from_allocator(gbArena *arena, gbAllocator backing, isize size)
{
	arena->backing         = backing;
	arena->physical_start  = gb_alloc(backing, size); /* NOTE(bill): Uses default alignment */
	arena->total_size      = size;
	arena->total_allocated = 0;
	arena->temp_count      = 0;
}

gb_inline void gb_arena_init_sub(gbArena *arena, gbArena *parent_arena, isize size) { gb_arena_init_from_allocator(arena, gb_arena_allocator(parent_arena), size); }


gb_inline void
gb_arena_free(gbArena *arena)
{
	if (arena->backing.proc) {
		gb_free(arena->backing, arena->physical_start);
		arena->physical_start = NULL;
	}
}


gb_inline isize
gb_arena_alignment_of(gbArena *arena, isize alignment)
{
	isize alignment_offset, result_pointer, mask;
	GB_ASSERT(gb_is_power_of_two(alignment));

	alignment_offset = 0;
	result_pointer = cast(isize)arena->physical_start + arena->total_allocated;
	mask = alignment - 1;
	if (result_pointer & mask)
		alignment_offset = alignment - (result_pointer & mask);

	return alignment_offset;
}

gb_inline isize
gb_arena_size_remaining(gbArena *arena, isize alignment)
{
	isize result = arena->total_size - (arena->total_allocated + gb_arena_alignment_of(arena, alignment));
	return result;
}

gb_inline void gb_arena_check(gbArena *arena) { GB_ASSERT(arena->temp_count == 0); }






gb_inline gbAllocator
gb_arena_allocator(gbArena *arena)
{
	gbAllocator allocator;
	allocator.proc = gb_arena_allocator_proc;
	allocator.data = arena;
	return allocator;
}


GB_ALLOCATOR_PROC(gb_arena_allocator_proc)
{
	gbArena *arena = cast(gbArena *)allocator_data;

	gb_unused(options);
	gb_unused(old_size);

	switch (type) {
	case GB_ALLOCATION_ALLOC: {
		void *ptr = NULL;
		void *end = gb_pointer_add(arena->physical_start, arena->total_allocated);
		isize total_size = size + alignment;

		/* NOTE(bill): Out of memory */
		if (arena->total_allocated + total_size > cast(isize)arena->total_size)
			return NULL;

		ptr = gb_align_forward(end, alignment);
		arena->total_allocated += total_size;
		return ptr;
	} break;

	case GB_ALLOCATION_FREE:
		/* NOTE(bill): Free all at once
		 * Use Temp_Arena_Memory if you want to free a block */
		break;

	case GB_ALLOCATION_FREE_ALL:
		arena->total_allocated = 0;
		break;

	case GB_ALLOCATION_RESIZE: {
		/* TODO(bill): Check if ptr is on top of stack and just extend */
		gbAllocator a = gb_arena_allocator(arena);
		return gb_default_resize_align(a, old_memory, old_size, size, alignment);
	} break;
	}

	return NULL; /* NOTE(bill): Default return value */
}


gb_inline gbTempArenaMemory
gb_temp_arena_memory_begin(gbArena *arena)
{
	gbTempArenaMemory tmp;
	tmp.arena = arena;
	tmp.original_count = arena->total_allocated;
	arena->temp_count++;
	return tmp;
}

gb_inline void
gb_temp_arena_memory_end(gbTempArenaMemory tmp)
{
	GB_ASSERT(tmp.arena->total_allocated >= tmp.original_count);
	GB_ASSERT(tmp.arena->temp_count > 0);
	tmp.arena->total_allocated = tmp.original_count;
	tmp.arena->temp_count--;
}




/*
 * Pool Allocator
 */


gb_inline void
gb_pool_init(gbPool *pool, gbAllocator backing, isize num_blocks, isize block_size)
{
	gb_pool_init_align(pool, backing, num_blocks, block_size, GB_DEFAULT_MEMORY_ALIGNMENT);
}

void
gb_pool_init_align(gbPool *pool, gbAllocator backing, isize num_blocks, isize block_size, isize block_align)
{
	isize actual_block_size, pool_size, block_index;
	void *data, *curr;
	uintptr *end;

	gb_zero_struct(pool);

	pool->backing = backing;
	pool->block_size = block_size;
	pool->block_align = block_align;

	actual_block_size = block_size + block_align;
	pool_size = num_blocks * actual_block_size;

	data = gb_alloc_align(backing, pool_size, block_align);

	/* NOTE(bill): Init intrusive freelist */
	curr = data;
	for (block_index = 0; block_index < num_blocks-1; block_index++) {
		uintptr *next = cast(uintptr *)curr;
		*next = cast(uintptr)curr + actual_block_size;
		curr = gb_pointer_add(curr, actual_block_size);
	}

	end  = cast(uintptr *)curr;
	*end = cast(uintptr)NULL;

	pool->physical_start = data;
	pool->free_list      = data;
}

gb_inline void
gb_pool_free(gbPool *pool)
{
	if (pool->backing.proc) {
		gb_free(pool->backing, pool->physical_start);
	}
}


gb_inline gbAllocator
gb_pool_allocator(gbPool *pool)
{
	gbAllocator allocator;
	allocator.proc = gb_pool_allocator_proc;
	allocator.data = pool;
	return allocator;
}

GB_ALLOCATOR_PROC(gb_pool_allocator_proc)
{
	gbPool *pool = cast(gbPool *)allocator_data;

	gb_unused(options);
	gb_unused(old_size);

	switch (type) {
	case GB_ALLOCATION_ALLOC: {
		uintptr next_free;
		void *ptr;
		GB_ASSERT(size      == pool->block_size);
		GB_ASSERT(alignment == pool->block_align);
		GB_ASSERT(pool->free_list != NULL);

		next_free = *cast(uintptr *)pool->free_list;
		ptr = pool->free_list;
		pool->free_list = cast(void *)next_free;
		pool->total_size += pool->block_size;
		return ptr;
	} break;

	case GB_ALLOCATION_FREE: {
		uintptr *next;
		if (old_memory == NULL) return NULL;

		next = cast(uintptr *)old_memory;
		*next = cast(uintptr)pool->free_list;
		pool->free_list = old_memory;
		pool->total_size -= pool->block_size;
	} break;

	case GB_ALLOCATION_FREE_ALL:
		/* TODO(bill): */
		break;

	case GB_ALLOCATION_RESIZE:
		/* NOTE(bill): Cannot resize */
		GB_ASSERT(false);
		break;
	}

	return NULL;
}





gb_inline gbAllocationHeader *
gb_allocation_header(void *data)
{
	isize *p = cast(isize *)data;
	while (p[-1] == cast(isize)(-1))
		p--;
	return cast(gbAllocationHeader *)p - 1;
}

gb_inline void gb_allocation_header_fill(gbAllocationHeader *header, void *data, isize size)
{
	isize *ptr;
	header->size = size;
	ptr = cast(isize *)(header + 1);
	while (cast(void *)ptr < data)
		*ptr++ = cast(isize)(-1);
}



/*
 * Free List Allocator
 */

gb_inline void
gb_free_list_init(gbFreeList *fl, void *start, isize size)
{
	GB_ASSERT(size > gb_size_of(gbFreeListBlock));

	fl->physical_start   = start;
	fl->total_size       = size;
	fl->curr_block       = cast(gbFreeListBlock *)start;
	fl->curr_block->size = size;
	fl->curr_block->next = NULL;
}


gb_inline void
gb_free_list_init_from_allocator(gbFreeList *fl, gbAllocator backing, isize size)
{
	void *start = gb_alloc(backing, size);
	gb_free_list_init(fl, start, size);
}



gb_inline gbAllocator
gb_free_list_allocator(gbFreeList *fl)
{
	gbAllocator a;
	a.proc = gb_free_list_allocator_proc;
	a.data = fl;
	return a;
}


GB_ALLOCATOR_PROC(gb_free_list_allocator_proc)
{
	gbFreeList *fl = cast(gbFreeList *)allocator_data;
	GB_ASSERT_NOT_NULL(fl);
	gb_unused(options);

	switch (type) {
	case GB_ALLOCATION_ALLOC: {
		gbFreeListBlock *prev_block = NULL;
		gbFreeListBlock *curr_block = fl->curr_block;

		while (curr_block) {
			void *ptr = NULL;
			isize total_size;
			gbAllocationHeader *header;

			total_size = size + alignment + gb_size_of(gbAllocationHeader);

			if (curr_block->size < total_size) {
				prev_block = curr_block;
				curr_block = curr_block->next;
				continue;
			}

			if (curr_block->size - total_size <= gb_size_of(gbAllocationHeader)) {
				total_size = curr_block->size;

				if (prev_block)
					prev_block->next = curr_block->next;
				else
					fl->curr_block = curr_block->next;
			} else {
				/* NOTE(bill): Create a new block for the remaining memory */
				gbFreeListBlock *next_block;
				next_block = cast(gbFreeListBlock *)gb_pointer_add(curr_block, total_size);

				GB_ASSERT(cast(void *)next_block < gb_pointer_add(fl->physical_start, fl->total_size));

				next_block->size = curr_block->size - total_size;
				next_block->next = curr_block->next;

				if (prev_block)
					prev_block->next = next_block;
				else
					fl->curr_block = next_block;
			}


			/* TODO(bill): Set Header Info */
			header = cast(gbAllocationHeader *)curr_block;
			ptr = gb_align_forward(header+1, alignment);
			gb_allocation_header_fill(header, ptr, size);

			fl->total_allocated += total_size;
			fl->allocation_count++;


			return ptr;
		}

		/* NOTE(bill): Ran out of free list memory! FUCK! */
		return NULL;
	} break;

	case GB_ALLOCATION_FREE: {
		gbAllocationHeader *header = gb_allocation_header(old_memory);
		isize block_size = header->size;
		uintptr block_start, block_end;
		gbFreeListBlock *prev_block = NULL;
		gbFreeListBlock *curr_block = fl->curr_block;

		block_start = cast(uintptr)header;
		block_end   = cast(uintptr)block_start + block_size;

		while (curr_block) {
			if (cast(uintptr)curr_block >= block_end)
				break;
			prev_block = curr_block;
			curr_block = curr_block->next;
		}

		if (prev_block == NULL) {
			prev_block = cast(gbFreeListBlock *)block_start;
			prev_block->size = block_size;
			prev_block->next = fl->curr_block;

			fl->curr_block = prev_block;
		} else if ((cast(uintptr)prev_block + prev_block->size) == block_start) {
			prev_block->size += block_size;
		} else {
			gbFreeListBlock *tmp = cast(gbFreeListBlock *)block_start;
			tmp->size = block_size;
			tmp->next = prev_block->next;
			prev_block->next = tmp;

			prev_block = tmp;
		}

		if (curr_block && (cast(uintptr)curr_block == block_end)) {
			prev_block->size += curr_block->size;
			prev_block->next = curr_block->next;
		}

		fl->allocation_count--;
		fl->total_allocated -= block_size;
	} break;

	case GB_ALLOCATION_FREE_ALL: {
		gb_free_list_init(fl, fl->physical_start, fl->total_size);
	} break;

	case GB_ALLOCATION_RESIZE:
		return gb_default_resize_align(gb_free_list_allocator(fl), old_memory, old_size, size, alignment);
	}

	return NULL;
}



void
gb_scratch_memory_init(gbScratchMemory *s, void *start, isize size)
{
	s->physical_start = start;
	s->total_size     = size;
	s->alloc_point    = start;
	s->free_point     = start;
}


b32
gb_scratch_memory_is_in_use(gbScratchMemory *s, void *ptr)
{
	if (s->free_point == s->alloc_point) return false;
	if (s->alloc_point > s->free_point)
		return ptr >= s->free_point && ptr < s->alloc_point;
	return ptr >= s->free_point || ptr < s->alloc_point;
}


gbAllocator
gb_scratch_allocator(gbScratchMemory *s)
{
	gbAllocator a;
	a.proc = gb_scratch_allocator_proc;
	a.data = s;
	return a;
}

GB_ALLOCATOR_PROC(gb_scratch_allocator_proc)
{
	gbScratchMemory *s = cast(gbScratchMemory *)allocator_data;
	GB_ASSERT_NOT_NULL(s);
	gb_unused(options);

	switch (type) {
	case GB_ALLOCATION_ALLOC: {
		void *ptr = s->alloc_point;
		gbAllocationHeader *header = cast(gbAllocationHeader *)ptr;
		void *data = gb_align_forward(header+1, alignment);
		void *end = gb_pointer_add(s->physical_start, s->total_size);

		GB_ASSERT(alignment % 4 == 0);
		size = ((size + 3)/4)*4;
		ptr = gb_pointer_add(ptr, size);

		/* NOTE(bill): Wrap around */
		if (ptr > end) {
			header->size = gb_pointer_diff(header, end) | GB_ISIZE_HIGH_BIT;
			ptr = s->physical_start;
			header = cast(gbAllocationHeader *)ptr;
			data = gb_align_forward(header+1, alignment);
			ptr = gb_pointer_add(ptr, size);
		}

		if (!gb_scratch_memory_is_in_use(s, ptr)) {
			gb_allocation_header_fill(header, ptr, gb_pointer_diff(header, ptr));
			s->alloc_point = cast(u8 *)ptr;
			return data;
		}
	} break;

	case GB_ALLOCATION_FREE: {
		if (old_memory) {
			void *end = gb_pointer_add(s->physical_start, s->total_size);
			if (old_memory < s->physical_start || old_memory >= end) {
				GB_ASSERT(false);
			} else {
				/* NOTE(bill): Mark as free */
				gbAllocationHeader *h = gb_allocation_header(old_memory);
				GB_ASSERT((h->size & GB_ISIZE_HIGH_BIT) == 0);
				h->size = h->size | GB_ISIZE_HIGH_BIT;

				while (s->free_point != s->alloc_point) {
					gbAllocationHeader *header = cast(gbAllocationHeader *)s->free_point;
					if ((header->size & GB_ISIZE_HIGH_BIT) == 0)
						break;

					s->free_point = gb_pointer_add(s->free_point, h->size & (~GB_ISIZE_HIGH_BIT));
					if (s->free_point == end)
						s->free_point = s->physical_start;
				}
			}
		}
	} break;

	case GB_ALLOCATION_FREE_ALL: {
		s->alloc_point = s->physical_start;
		s->free_point  = s->physical_start;
	} break;

	case GB_ALLOCATION_RESIZE:
		return gb_default_resize_align(gb_scratch_allocator(s), old_memory, old_size, size, alignment);
	}

	return NULL;
}






/***************************************************************
 *
 * Sorting
 *
 */

/* TODO(bill): Should I make all the macros local? */

#define GB__COMPARE_PROC(Type) \
gb_global isize gb__##Type##_cmp_offset; \
GB_COMPARE_PROC(gb__##Type##_cmp) \
{ \
	Type const p = *cast(Type const *)gb_pointer_add_const(a, gb__##Type##_cmp_offset); \
	Type const q = *cast(Type const *)gb_pointer_add_const(b, gb__##Type##_cmp_offset); \
	return p < q ? -1 : p > q; \
} \
GB_COMPARE_PROC_PTR(gb_##Type##_cmp(isize offset)) \
{ \
	gb__##Type##_cmp_offset = offset; \
	return &gb__##Type##_cmp; \
}


GB__COMPARE_PROC(i16);
GB__COMPARE_PROC(i32);
GB__COMPARE_PROC(i64);
GB__COMPARE_PROC(isize);
GB__COMPARE_PROC(f32);
GB__COMPARE_PROC(f64);
GB__COMPARE_PROC(char);

/* NOTE(bill): str_cmp is special as it requires a funny type and funny comparison*/
gb_global isize gb__str_cmp_offset;
GB_COMPARE_PROC(gb__str_cmp)
{
	char const *p = *cast(char const **)gb_pointer_add_const(a, gb__str_cmp_offset);
	char const *q = *cast(char const **)gb_pointer_add_const(b, gb__str_cmp_offset);
	return gb_strcmp(p, q);
}
GB_COMPARE_PROC_PTR(gb_str_cmp(isize offset))
{
	gb__str_cmp_offset = offset;
	return &gb__str_cmp;
}

#undef GB__COMPARE_PROC

/* TODO(bill): Make user definable? */
#define GB__SORT_STACK_SIZE   64
#define GB__SORT_INSERT_SORT_THRESHOLD  8

#define GB__SORT_PUSH(_base, _limit) do { \
	stack_ptr[0] = (_base); \
	stack_ptr[1] = (_limit); \
	stack_ptr += 2; \
} while (0)


#define GB__SORT_POP(_base, _limit) do { \
	stack_ptr -= 2; \
	(_base)  = stack_ptr[0]; \
	(_limit) = stack_ptr[1]; \
} while (0)



gb_inline void
gb_sort(void *base_, isize count, isize size, gbCompareProc cmp)
{
	u8 *i, *j;
	u8 *base = cast(u8 *)base_;
	u8 *limit = base + count*size;
	isize threshold = GB__SORT_INSERT_SORT_THRESHOLD * size;

	/* NOTE(bill): Prepare the stack */
	u8 *stack[GB__SORT_STACK_SIZE] = {0};
	u8 **stack_ptr = stack;

	for (;;) {
		if ((limit-base) > threshold) {
			/* NOTE(bill): Quick sort */
			i = base + size;
			j = limit - size;

			gb_memswap(((limit-base)/size/2) * size + base, base, size);
			if (cmp(i, j) > 0)    gb_memswap(i, j, size);
			if (cmp(base, j) > 0) gb_memswap(base, j, size);
			if (cmp(i, base) > 0) gb_memswap(i, base, size);

			for (;;) {
				do {
					i += size;
				} while(cmp(i, base) < 0);
				do {
					j -= size;
				} while (cmp(j, base) > 0);
				if (i > j)
					break;
				gb_memswap(i, j, size);
			}

			gb_memswap(base, j, size);

			if (j - base > limit - i) {
				GB__SORT_PUSH(base, j);
				base = i;
			} else {
				GB__SORT_PUSH(i, limit);
				limit = j;
			}
		} else {
			/* NOTE(bill): Insertion sort */
			for (j = base, i = j+size;
			     i < limit;
			     j = i, i += size) {
				for (; cmp(j, j+size) > 0; j -= size) {
					gb_memswap(j, j+size, size);
					if (j == base)
						break;
				}
			}

			if (stack_ptr == stack)
				break; /* NOTE(bill): Sorting is done! */
			GB__SORT_POP(base, limit);
		}
	}
}

#undef GB__SORT_PUSH
#undef GB__SORT_POP

void
gb_radix_sort_u8(u8 *gb_restrict items, u8 *gb_restrict temp, isize count)
{
	u8 *gb_restrict source = items;
	u8 *gb_restrict dest = temp;
	isize i;
	isize offsets[256] = {0};
	i64 total = 0;

	/* NOTE(bill): First pass - count how many of each key */
	for (i = 0; i < count; i++) {
		u8 radix_value = source[i];
		u8 radix_piece = radix_value & 0xff;
		offsets[radix_piece]++;
	}

	/* NOTE(bill): Change counts to offsets */
	for (i = 0; i < gb_count_of(offsets); i++) {
		isize skcount = offsets[i];
		offsets[i] = total;
		total += skcount;
	}

	/* NOTE(bill): Second pass - place elements into the right location */
	for (i = 0; i < count; i++) {
		u8 radix_value = source[i];
		u8 radix_piece = radix_value & 0xff;
		dest[offsets[radix_piece]++] = source[i];
	}

	gb_swap(u8 *gb_restrict, source, dest);
}

void
gb_radix_sort_u16(u16 *gb_restrict items, u16 *gb_restrict temp, isize count)
{
	u16 *gb_restrict source = items;
	u16 *gb_restrict dest   = temp;
	isize byte_index, i;
	for (byte_index = 0; byte_index < 16; byte_index += 8) {
		isize offsets[256] = {0};
		i64 total = 0;

		/* NOTE(bill): First pass - count how many of each key */
		for (i = 0; i < count; i++) {
			u16 radix_value = source[i];
			u16 radix_piece = (radix_value >> byte_index) & 0xff;
			offsets[radix_piece]++;
		}

		/* NOTE(bill): Change counts to offsets */
		for (i = 0; i < gb_count_of(offsets); i++) {
			isize skcount = offsets[i];
			offsets[i] = total;
			total += skcount;
		}

		/* NOTE(bill): Second pass - place elements into the right location */
		for (i = 0; i < count; i++) {
			u16 radix_value = source[i];
			u16 radix_piece = (radix_value >> byte_index) & 0xff;
			dest[offsets[radix_piece]++] = source[i];
		}

		gb_swap(u16 *gb_restrict, source, dest);
	}
}

void
gb_radix_sort_u32(u32 *gb_restrict items, u32 *gb_restrict temp, isize count)
{
	u32 *gb_restrict source = items;
	u32 *gb_restrict dest   = temp;
	isize byte_index, i;
	for (byte_index = 0; byte_index < 32; byte_index += 8) {
		isize offsets[256] = {0};
		i64 total = 0;

		/* NOTE(bill): First pass - count how many of each key */
		for (i = 0; i < count; i++) {
			u32 radix_value = source[i];
			u32 radix_piece = (radix_value >> byte_index) & 0xff;
			offsets[radix_piece]++;
		}

		/* NOTE(bill): Change counts to offsets */
		for (i = 0; i < gb_count_of(offsets); i++) {
			isize skcount = offsets[i];
			offsets[i] = total;
			total += skcount;
		}

		/* NOTE(bill): Second pass - place elements into the right location */
		for (i = 0; i < count; i++) {
			u32 radix_value = source[i];
			u32 radix_piece = (radix_value >> byte_index) & 0xff;
			dest[offsets[radix_piece]++] = source[i];
		}

		gb_swap(u32 *gb_restrict, source, dest);
	}
}

void
gb_radix_sort_u64(u64 *gb_restrict items, u64 *gb_restrict temp, isize count)
{
	u64 *gb_restrict source = items;
	u64 *gb_restrict dest   = temp;
	isize byte_index, i;
	for (byte_index = 0; byte_index < 64; byte_index += 8) {
		isize offsets[256] = {0};
		i64 total = 0;

		/* NOTE(bill): First pass - count how many of each key */
		for (i = 0; i < count; i++) {
			u64 radix_value = source[i];
			u64 radix_piece = (radix_value >> byte_index) & 0xff;
			offsets[radix_piece]++;
		}

		/* NOTE(bill): Change counts to offsets */
		for (i = 0; i < gb_count_of(offsets); i++) {
			u64 skcount = offsets[i];
			offsets[i] = total;
			total += skcount;
		}

		/* NOTE(bill): Second pass - place elements into the right location */
		for (i = 0; i < count; i++) {
			u64 radix_value = source[i];
			u64 radix_piece = (radix_value >> byte_index) & 0xff;
			dest[offsets[radix_piece]++] = source[i];
		}

		gb_swap(u64 *gb_restrict, source, dest);
	}
}




gb_inline isize
gb_binary_search(void const *base, isize count, isize size, void const *key, gbCompareProc compare_proc)
{
	isize start = 0;
	isize end = count;

	while (start < end) {
		isize mid = start + (end-start)/2;
		isize result = compare_proc(key, cast(u8 *)base + mid*size);
		if (result < 0)
			end = mid;
		else if (result > 0)
			start = mid+1;
		else
			return mid;
	}

	return -1;
}




/***************************************************************
 *
 * Char things
 *
 */




gb_inline char
gb_char_to_lower(char c)
{
	if (c >= 'A' && c <= 'Z')
		return 'a' + (c - 'A');
	return c;
}

gb_inline char
gb_char_to_upper(char c)
{
	if (c >= 'a' && c <= 'z')
		return 'A' + (c - 'a');
	return c;
}

gb_inline b32
gb_char_is_space(char c)
{
	if (c == ' '  ||
	    c == '\t' ||
	    c == '\n' ||
	    c == '\r' ||
	    c == '\f' ||
	    c == '\v')
	    return true;
	return false;
}

gb_inline b32
gb_char_is_digit(char c)
{
	if (c >= '0' && c <= '9')
		return true;
	return false;
}

gb_inline b32
gb_char_is_hex_digit(char c)
{
	if (gb_char_is_digit(c) ||
	    (c >= 'a' && c <= 'f') ||
	    (c >= 'A' && c <= 'F'))
	    return true;
	return false;
}

gb_inline b32
gb_char_is_alpha(char c)
{
	if ((c >= 'A' && c <= 'Z') ||
	    (c >= 'a' && c <= 'z'))
	    return true;
	return false;
}

gb_inline b32
gb_char_is_alphanumeric(char c)
{
	return gb_char_is_alpha(c) || gb_char_is_digit(c);
}

gb_inline i32
gb_digit_to_int(char c)
{
	return gb_char_is_digit(c) ? c - '0' : c - 'W';
}


gb_inline i32
gb_hex_digit_to_int(char c)
{
	if (gb_char_is_digit(c))
		return gb_digit_to_int(c);
	else if (gb_is_between(c, 'a', 'f'))
		return c - 'a' + 10;
	else if (gb_is_between(c, 'A', 'F'))
		return c - 'A' + 10;
	return 0;
}




gb_inline void
gb_str_to_lower(char *str)
{
	if (!str) return;
	while (*str) {
		*str = gb_char_to_lower(*str);
		str++;
	}
}

gb_inline void
gb_str_to_upper(char *str)
{
	if (!str) return;
	while (*str) {
		*str = gb_char_to_upper(*str);
		str++;
	}
}


gb_inline isize
gb_strlen(char const *str)
{
	isize result = 0;
	if (str) {
		char const *end = str;
		while (*end) end++;
		result = end - str;
	}
	return result;
}

gb_inline isize
gb_strnlen(char const *str, isize max_len)
{
	isize result = 0;
	if (str) {
		char const *end = str;
		while (*end && result < max_len) end++;
		result = end - str;
	}
	return result;
}


gb_inline isize
gb_utf8_strlen(char const *str)
{
	isize result = 0;
	for (; *str; str++) {
		if ((*str & 0xc0) != 0x80)
			result++;
	}
	return result;
}

gb_inline isize
gb_utf8_strnlen(char const *str, isize max_len)
{
	isize result = 0;
	for (; *str && result < max_len; str++) {
		if ((*str & 0xc0) != 0x80)
			result++;
	}
	return result;
}


gb_inline i32
gb_strcmp(char const *s1, char const *s2)
{
	while (*s1 && (*s1 == *s2)) {
		s1++, s2++;
	}
	return *(u8 *)s1 - *(u8 *)s2;
}

gb_inline char *
gb_strcpy(char *dest, char const *source)
{
	GB_ASSERT_NOT_NULL(dest);
	if (source) {
		char *str = dest;
		while (*source) *str++ = *source++;
	}
	return dest;
}


gb_inline char *
gb_strncpy(char *dest, char const *source, isize len)
{
	GB_ASSERT_NOT_NULL(dest);
	if (source) {
		char *str = dest;
		while (len > 0 && *source) {
			*str++ = *source++;
			len--;
		}
		while (len > 0) {
			*str++ = '\0';
			len--;
		}
	}
	return dest;
}

gb_inline isize
gb_strlcpy(char *dest, char const *source, isize len)
{
	isize result = 0;
	GB_ASSERT_NOT_NULL(dest);
	if (source) {
		char const *source_start = source;
		char *str = dest;
		while (len > 0 && *source) {
			*str++ = *source++;
			len--;
		}
		while (len > 0) {
			*str++ = '\0';
			len--;
		}

		result = source - source_start;
	}
	return result;
}

gb_inline char *
gb_strrev(char *str)
{
	isize len = gb_strlen(str);
	char *a = str + 0;
	char *b = str + len-1;
	len /= 2;
	while (len--) {
		gb_swap(char, *a, *b);
		a++, b--;
	}
	return str;
}




gb_inline i32
gb_strncmp(char const *s1, char const *s2, isize len)
{
	for (; len > 0;
	     s1++, s2++, len--) {
		if (*s1 != *s2)
			return ((s1 < s2) ? -1 : +1);
		else if (*s1 == '\0')
			return 0;
	}
	return 0;
}


gb_inline char const *
gb_strtok(char *output, char const *src, char const *delimit)
{
	while (*src && gb_char_first_occurence(delimit, *src) != NULL)
		*output++ = *src++;

	*output = 0;
	return *src ? src+1 : src;
}

gb_inline b32
gb_str_has_prefix(char const *str, char const *prefix)
{
	while (*prefix) {
		if (*str++ != *prefix++)
			return false;
	}
	return true;
}

gb_inline b32
gb_str_has_suffix(char const *str, char const *suffix)
{
	isize i = gb_strlen(str);
	isize j = gb_strlen(suffix);
	if (j <= i)
		return gb_strcmp(str+i-j, suffix) == 0;
	return false;
}




gb_inline char const *
gb_char_first_occurence(char const *s, char c)
{
	char ch = c;
	for (; *s != ch; s++) {
		if (*s == '\0')
			return NULL;
	}
	return s;
}


gb_inline char const *
gb_char_last_occurence(char const *s, char c)
{
	char const *result = NULL;
	do {
		if (*s == c)
			result = s;
	} while (*s++);

	return result;
}



gb_inline void
gb_str_concat(char *dest, isize dest_len,
              char const *src_a, isize src_a_len,
              char const *src_b, isize src_b_len)
{
	GB_ASSERT(dest_len >= src_a_len+src_b_len+1);
	if (dest) {
		gb_memcopy(dest, src_a, src_a_len);
		gb_memcopy(dest+src_a_len, src_b, src_b_len);
		dest[src_a_len+src_b_len] = '\0';
	}
}


gb_internal isize
gb__scan_i64(char const *text, i32 base, i64 *value)
{
	char const *text_begin = text;
	i64 result = 0;
	b32 negative = false;

	if (*text == '-') {
		negative = true;
		text++;
	}

	if (base == 16 && gb_strncmp(text, "0x", 2) == 0)
		text += 2;

	for (;;) {
		i64 v;
		if (gb_char_is_digit(*text))
			v = *text - '0';
		else if (base == 16 && gb_char_is_hex_digit(*text))
			v = gb_hex_digit_to_int(*text);
		else
			break;

		result *= base;
		result += v;
		text++;
	}

	if (value) {
		if (negative) result = -result;
		*value = result;
	}

	return (text - text_begin);
}


i64
gb_str_to_i64(char const *str, char **end_ptr, i32 base)
{
	isize len;
	i64 value;

	if (!base) {
		if ((gb_strlen(str) > 2) && (gb_strncmp(str, "0x", 2) == 0))
			base = 16;
		else
			base = 10;
	}

	len = gb__scan_i64(str, base, &value);
	if (end_ptr)
		*end_ptr = (char *)str + len;
	return value;
}


gb_global char const gb__num_to_char_table[] =
	"0123456789"
	"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
	"abcdefghijklmnopqrstuvwxyz"
	"_/";

gb_inline void
gb_i64_to_str(i64 value, char *string, i32 base)
{
	char *buf = string;
	b32 negative = false;
	if (value < 0) {
		negative = true;
		value = -value;
	}
	if (value) {
		while (value > 0) {
			*buf++ = gb__num_to_char_table[value % base];
			value /= base;
		}
	} else {
		*buf++ = '0';
	}
	if (negative)
		*buf++ = '-';
	*buf = '\0';
	gb_strrev(string);
}



gb_inline void
gb_u64_to_str(u64 value, char *string, i32 base)
{
	char *buf = string;

	if (value) {
		while (value > 0) {
			*buf++ = gb__num_to_char_table[value % base];
			value /= base;
		}
	} else {
		*buf++ = '0';
	}
	*buf = '\0';

	gb_strrev(string);
}






gb_inline void gb__set_string_length  (gbString str, isize len) { GB_STRING_HEADER(str)->length = len; }
gb_inline void gb__set_string_capacity(gbString str, isize cap) { GB_STRING_HEADER(str)->capacity = cap; }


gb_inline gbString
gb_string_make(gbAllocator a, char const *str)
{
	isize len = str ? gb_strlen(str) : 0;
	return gb_string_make_length(a, str, len);
}

gbString
gb_string_make_length(gbAllocator a, void const *init_str, isize num_bytes)
{
	isize header_size = gb_size_of(gbStringHeader);
	void *ptr = gb_alloc(a, header_size + num_bytes + 1);

	gbString str;
	gbStringHeader *header;

	if (!init_str) gb_zero_size(ptr, header_size + num_bytes + 1);
	if (ptr == NULL) return NULL;

	str = cast(char *)ptr + header_size;
	header = GB_STRING_HEADER(str);
	header->allocator = a;
	header->length    = num_bytes;
	header->capacity  = num_bytes;
	if (num_bytes && init_str)
		gb_memcopy(str, init_str, num_bytes);
	str[num_bytes] = '\0';

	return str;
}

gb_inline void
gb_string_free(gbString str)
{
	if (str) {
		gbStringHeader *header = GB_STRING_HEADER(str);
		gb_free(header->allocator, header);
	}
}


gb_inline gbString gb_string_duplicate(gbAllocator a, gbString const str) { return gb_string_make_length(a, str, gb_string_length(str)); }

gb_inline isize gb_string_length  (gbString const str) { return GB_STRING_HEADER(str)->length; }
gb_inline isize gb_string_capacity(gbString const str) { return GB_STRING_HEADER(str)->capacity; }

gb_inline isize
gb_string_available_space(gbString const str)
{
	gbStringHeader *h = GB_STRING_HEADER(str);
	if (h->capacity > h->length)
		return h->capacity - h->length;
	return 0;
}


gb_inline void gb_string_clear(gbString str) { gb__set_string_length(str, 0); str[0] = '\0'; }

gb_inline gbString gb_string_append(gbString str, gbString const other) { return gb_string_append_length(str, other, gb_string_length(other)); }

gbString
gb_string_append_length(gbString str, void const *other, isize other_len)
{
	isize curr_len = gb_string_length(str);

	str = gb_string_make_space_for(str, other_len);
	if (str == NULL)
		return NULL;

	gb_memcopy(str + curr_len, other, other_len);
	str[curr_len + other_len] = '\0';
	gb__set_string_length(str, curr_len + other_len);

	return str;
}

gb_inline gbString
gb_string_appendc(gbString str, char const *other)
{
	return gb_string_append_length(str, other, gb_strlen(other));
}


gbString
gb_string_set(gbString str, char const *cstr)
{
	isize len = gb_strlen(cstr);
	if (gb_string_capacity(str) < len) {
		str = gb_string_make_space_for(str, len - gb_string_length(str));
		if (str == NULL)
			return NULL;
	}

	gb_memcopy(str, cstr, len);
	str[len] = '\0';
	gb__set_string_length(str, len);

	return str;
}



gbString
gb_string_make_space_for(gbString str, isize add_len)
{
	isize available = gb_string_available_space(str);

	/* Return if there is enough space left */
	if (available >= add_len) {
		return str;
	} else {
		isize new_len, old_size, new_size;
		void *ptr, *new_ptr;

		new_len = gb_string_length(str) + add_len;
		ptr = GB_STRING_HEADER(str);
		old_size = gb_size_of(gbStringHeader) + gb_string_length(str) + 1;
		new_size = gb_size_of(gbStringHeader) + new_len + 1;

		new_ptr = gb_resize(GB_STRING_HEADER(str)->allocator, ptr, old_size, new_size);
		if (new_ptr == NULL) return NULL;

		str = cast(char *)(GB_STRING_HEADER(new_ptr) + 1);
		gb__set_string_capacity(str, new_len);

		return str;
	}
}

gb_inline isize
gb_string_allocation_size(gbString const str)
{
	isize cap = gb_string_capacity(str);
	return gb_size_of(gbStringHeader) + cap;
}


gb_inline b32
gb_string_are_equal(gbString const lhs, gbString const rhs)
{
	isize lhs_len, rhs_len, i;
	lhs_len = gb_string_length(lhs);
	rhs_len = gb_string_length(rhs);
	if (lhs_len != rhs_len)
		return false;

	for (i = 0; i < lhs_len; i++) {
		if (lhs[i] != rhs[i])
			return false;
	}

	return true;
}


gbString
gb_string_trim(gbString str, char const *cut_set)
{
	char *start, *end, *start_pos, *end_pos;
	isize len;

	start_pos = start = str;
	end_pos   = end   = str + gb_string_length(str) - 1;

	while (start_pos <= end && gb_char_first_occurence(cut_set, *start_pos))
		start_pos++;
	while (end_pos > start_pos && gb_char_first_occurence(cut_set, *end_pos))
		end_pos--;

	len = cast(isize)((start_pos > end_pos) ? 0 : ((end_pos - start_pos)+1));

	if (str != start_pos)
		gb_memmove(str, start_pos, len);
	str[len] = '\0';

	gb__set_string_length(str, len);

	return str;
}

gb_inline gbString gb_string_trim_space(gbString str) { return gb_string_trim(str, " \t\r\n\v\f"); }




/***************************************************************
 *
 * Windows UTF-8 Handling
 *
 */


char16 *
gb_utf8_to_ucs2(char16 *buffer, isize len, char const *s)
{
	u8 *str = cast(u8 *)s;
	char32 c;
	isize i = 0;
	len--;
	while (*str) {
		if (i >= len)
			return NULL;
		if (!(*str & 0x80)) {
			buffer[i++] = *str++;
		} else if ((*str & 0xe0) == 0xc0) {
			if (*str < 0xc2)
				return NULL;
			c = (*str++ & 0x1f) << 6;
			if ((*str & 0xc0) != 0x80)
				return NULL;
			buffer[i++] = cast(char16)(c + (*str++ & 0x3f));
		} else if ((*str & 0xf0) == 0xe0) {
			if (*str == 0xe0 &&
			    (str[1] < 0xa0 || str[1] > 0xbf))
				return NULL;
			if (*str == 0xed && str[1] > 0x9f) /* str[1] < 0x80 is checked below */
				return NULL;
			c = (*str++ & 0x0f) << 12;
			if ((*str & 0xc0) != 0x80)
				return NULL;
			c += (*str++ & 0x3f) << 6;
			if ((*str & 0xc0) != 0x80)
				return NULL;
			buffer[i++] = cast(char16)(c + (*str++ & 0x3f));
		} else if ((*str & 0xf8) == 0xf0) {
			if (*str > 0xf4)
				return NULL;
			if (*str == 0xf0 && (str[1] < 0x90 || str[1] > 0xbf))
				return NULL;
			if (*str == 0xf4 && str[1] > 0x8f) /* str[1] < 0x80 is checked below */
				return NULL;
			c = (*str++ & 0x07) << 18;
			if ((*str & 0xc0) != 0x80)
				return NULL;
			c += (*str++ & 0x3f) << 12;
			if ((*str & 0xc0) != 0x80)
				return NULL;
			c += (*str++ & 0x3f) << 6;
			if ((*str & 0xc0) != 0x80)
				return NULL;
			c += (*str++ & 0x3f);
			/* UTF-8 encodings of values used in surrogate pairs are invalid */
			if ((c & 0xfffff800) == 0xd800)
				return NULL;
			if (c >= 0x10000) {
				c -= 0x10000;
				if (i+2 > len)
					return NULL;
				buffer[i++] = 0xd800 | (0x3ff & (c>>10));
				buffer[i++] = 0xdc00 | (0x3ff & (c    ));
			}
		} else {
			return NULL;
		}
	}
	buffer[i] = 0;
	return buffer;
}

char *
gb_ucs2_to_utf8(char *buffer, isize len, char16 const *str)
{
	isize i = 0;
	len--;
	while (*str) {
		if (*str < 0x80) {
			if (i+1 > len)
				return NULL;
			buffer[i++] = (char) *str++;
		} else if (*str < 0x800) {
			if (i+2 > len)
				return NULL;
			buffer[i++] = cast(char)(0xc0 + (*str >> 6));
			buffer[i++] = cast(char)(0x80 + (*str & 0x3f));
			str += 1;
		} else if (*str >= 0xd800 && *str < 0xdc00) {
			char32 c;
			if (i+4 > len)
				return NULL;
			c = ((str[0] - 0xd800) << 10) + ((str[1]) - 0xdc00) + 0x10000;
			buffer[i++] = cast(char)(0xf0 +  (c >> 18));
			buffer[i++] = cast(char)(0x80 + ((c >> 12) & 0x3f));
			buffer[i++] = cast(char)(0x80 + ((c >>  6) & 0x3f));
			buffer[i++] = cast(char)(0x80 + ((c      ) & 0x3f));
			str += 2;
		} else if (*str >= 0xdc00 && *str < 0xe000) {
			return NULL;
		} else {
			if (i+3 > len)
				return NULL;
			buffer[i++] = 0xe0 +  (*str >> 12);
			buffer[i++] = 0x80 + ((*str >>  6) & 0x3f);
			buffer[i++] = 0x80 + ((*str      ) & 0x3f);
			str += 1;
		}
	}
	buffer[i] = 0;
	return buffer;
}


#define GB__UTF_SIZE 4
#define GB__UTF_INVALID 0xfffd

gb_global u8     const gb__utf_byte[GB__UTF_SIZE+1] = {0x80, 0, 0xc0, 0xe0, 0xf0};
gb_global u8     const gb__utf_mask[GB__UTF_SIZE+1] = {0xc0, 0x80, 0xe0, 0xf0, 0xf8};
gb_global char32 const gb__utf_min [GB__UTF_SIZE+1] = {0, 0, 0x80, 0x800, 0x10000};
gb_global char32 const gb__utf_max [GB__UTF_SIZE+1] = {0x10ffff, 0x7f, 0x7ff, 0xffff, 0x10ffff};

gb_internal isize
gb__utf_validate(char32 *c, isize i)
{
	GB_ASSERT_NOT_NULL(c);
	if (!c) return 0;
	if (!gb_is_between(*c, gb__utf_min[i], gb__utf_max[i]) ||
	     gb_is_between(*c, 0xd800, 0xdfff)) {
		*c = GB__UTF_INVALID;
	}
	i = 1;
	while (*c > gb__utf_max[i])
		i++;
	return i;
}

gb_internal char32
gb__utf_decode_byte(char c, isize *i)
{
	GB_ASSERT_NOT_NULL(i);
	if (!i) return 0;
	for (*i = 0; *i < gb_count_of(gb__utf_mask); (*i)++) {
		if ((cast(u8)c & gb__utf_mask[*i]) == gb__utf_byte[*i])
			return cast(u8)(c & ~gb__utf_mask[*i]);
	}
	return 0;
}

gb_inline isize gb_utf8_decode(char const *str, char32 *codepoint) { return gb_utf8_decode_len(str, gb_strlen(str), codepoint); }

isize
gb_utf8_decode_len(char const *s, isize str_len, char32 *c)
{
	isize i, j, len, type = 0;
	char32 cp;

	GB_ASSERT_NOT_NULL(s);
	GB_ASSERT_NOT_NULL(c);

	if (!s || !c) return 0;
	if (!str_len) return 0;
	*c = GB__UTF_INVALID;

	cp = gb__utf_decode_byte(s[0], &len);
	if (!gb_is_between(len, 1, GB__UTF_SIZE))
		return 1;

	for (i = 1, j = 1; i < str_len && j < len; i++, j++) {
		cp = (cp << 6) | gb__utf_decode_byte(s[i], &type);
		if (type != 0)
			return j;
	}
	if (j < len)
		return 0;
	*c = cp;
	gb__utf_validate(c, len);
	return len;
}




/***************************************************************
 *
 * Array
 *
 */


gb_no_inline void *
gb__array_set_capacity(void *array, isize capacity, isize element_size)
{
	gbArrayHeader *h = GB_ARRAY_HEADER(array);

	GB_ASSERT(element_size > 0);

	if (capacity == h->capacity)
		return array;

	if (capacity < h->count) {
		if (h->capacity < capacity) {
			isize new_capacity = GB_ARRAY_GROW_FORMULA(h->capacity);
			if (new_capacity < capacity)
				new_capacity = capacity;
			gb__array_set_capacity(array, new_capacity, element_size);
		}
		h->count = capacity;
	}

	{
		isize size = gb_size_of(gbArrayHeader) + element_size*capacity;
		gbArrayHeader *nh = cast(gbArrayHeader *)gb_alloc(h->allocator, size);
		gb_memmove(nh, h, gb_size_of(gbArrayHeader) + element_size*h->count);
		nh->allocator = h->allocator;
		nh->count     = h->count;
		nh->capacity  = capacity;
		gb_free(h->allocator, h);
		return nh+1;
	}
}


/***************************************************************
 *
 * Hashing functions
 *
 */

u32
gb_adler32(void const *data, isize len)
{
	u32 const MOD_ALDER = 65521;
	u32 a = 1, b = 0;
	isize i, block_len;
	u8 const *bytes = cast(u8 const *)data;

	block_len = len % 5552;

	while (len) {
		for (i = 0; i+7 < block_len; i += 8) {
			a += bytes[0], b += a;
			a += bytes[1], b += a;
			a += bytes[2], b += a;
			a += bytes[3], b += a;
			a += bytes[4], b += a;
			a += bytes[5], b += a;
			a += bytes[6], b += a;
			a += bytes[7], b += a;

			bytes += 8;
		}
		for (; i < block_len; i++)
			a += *bytes++, b += a;

		a %= MOD_ALDER, b %= MOD_ALDER;
		len -= block_len;
		block_len = 5552;
	}

	return (b << 16) | a;
}


gb_global u32 const GB__CRC32_TABLE[256] = {
	0x00000000, 0x77073096, 0xee0e612c, 0x990951ba,
	0x076dc419, 0x706af48f, 0xe963a535, 0x9e6495a3,
	0x0edb8832, 0x79dcb8a4, 0xe0d5e91e, 0x97d2d988,
	0x09b64c2b, 0x7eb17cbd, 0xe7b82d07, 0x90bf1d91,
	0x1db71064, 0x6ab020f2, 0xf3b97148, 0x84be41de,
	0x1adad47d, 0x6ddde4eb, 0xf4d4b551, 0x83d385c7,
	0x136c9856, 0x646ba8c0, 0xfd62f97a, 0x8a65c9ec,
	0x14015c4f, 0x63066cd9, 0xfa0f3d63, 0x8d080df5,
	0x3b6e20c8, 0x4c69105e, 0xd56041e4, 0xa2677172,
	0x3c03e4d1, 0x4b04d447, 0xd20d85fd, 0xa50ab56b,
	0x35b5a8fa, 0x42b2986c, 0xdbbbc9d6, 0xacbcf940,
	0x32d86ce3, 0x45df5c75, 0xdcd60dcf, 0xabd13d59,
	0x26d930ac, 0x51de003a, 0xc8d75180, 0xbfd06116,
	0x21b4f4b5, 0x56b3c423, 0xcfba9599, 0xb8bda50f,
	0x2802b89e, 0x5f058808, 0xc60cd9b2, 0xb10be924,
	0x2f6f7c87, 0x58684c11, 0xc1611dab, 0xb6662d3d,
	0x76dc4190, 0x01db7106, 0x98d220bc, 0xefd5102a,
	0x71b18589, 0x06b6b51f, 0x9fbfe4a5, 0xe8b8d433,
	0x7807c9a2, 0x0f00f934, 0x9609a88e, 0xe10e9818,
	0x7f6a0dbb, 0x086d3d2d, 0x91646c97, 0xe6635c01,
	0x6b6b51f4, 0x1c6c6162, 0x856530d8, 0xf262004e,
	0x6c0695ed, 0x1b01a57b, 0x8208f4c1, 0xf50fc457,
	0x65b0d9c6, 0x12b7e950, 0x8bbeb8ea, 0xfcb9887c,
	0x62dd1ddf, 0x15da2d49, 0x8cd37cf3, 0xfbd44c65,
	0x4db26158, 0x3ab551ce, 0xa3bc0074, 0xd4bb30e2,
	0x4adfa541, 0x3dd895d7, 0xa4d1c46d, 0xd3d6f4fb,
	0x4369e96a, 0x346ed9fc, 0xad678846, 0xda60b8d0,
	0x44042d73, 0x33031de5, 0xaa0a4c5f, 0xdd0d7cc9,
	0x5005713c, 0x270241aa, 0xbe0b1010, 0xc90c2086,
	0x5768b525, 0x206f85b3, 0xb966d409, 0xce61e49f,
	0x5edef90e, 0x29d9c998, 0xb0d09822, 0xc7d7a8b4,
	0x59b33d17, 0x2eb40d81, 0xb7bd5c3b, 0xc0ba6cad,
	0xedb88320, 0x9abfb3b6, 0x03b6e20c, 0x74b1d29a,
	0xead54739, 0x9dd277af, 0x04db2615, 0x73dc1683,
	0xe3630b12, 0x94643b84, 0x0d6d6a3e, 0x7a6a5aa8,
	0xe40ecf0b, 0x9309ff9d, 0x0a00ae27, 0x7d079eb1,
	0xf00f9344, 0x8708a3d2, 0x1e01f268, 0x6906c2fe,
	0xf762575d, 0x806567cb, 0x196c3671, 0x6e6b06e7,
	0xfed41b76, 0x89d32be0, 0x10da7a5a, 0x67dd4acc,
	0xf9b9df6f, 0x8ebeeff9, 0x17b7be43, 0x60b08ed5,
	0xd6d6a3e8, 0xa1d1937e, 0x38d8c2c4, 0x4fdff252,
	0xd1bb67f1, 0xa6bc5767, 0x3fb506dd, 0x48b2364b,
	0xd80d2bda, 0xaf0a1b4c, 0x36034af6, 0x41047a60,
	0xdf60efc3, 0xa867df55, 0x316e8eef, 0x4669be79,
	0xcb61b38c, 0xbc66831a, 0x256fd2a0, 0x5268e236,
	0xcc0c7795, 0xbb0b4703, 0x220216b9, 0x5505262f,
	0xc5ba3bbe, 0xb2bd0b28, 0x2bb45a92, 0x5cb36a04,
	0xc2d7ffa7, 0xb5d0cf31, 0x2cd99e8b, 0x5bdeae1d,
	0x9b64c2b0, 0xec63f226, 0x756aa39c, 0x026d930a,
	0x9c0906a9, 0xeb0e363f, 0x72076785, 0x05005713,
	0x95bf4a82, 0xe2b87a14, 0x7bb12bae, 0x0cb61b38,
	0x92d28e9b, 0xe5d5be0d, 0x7cdcefb7, 0x0bdbdf21,
	0x86d3d2d4, 0xf1d4e242, 0x68ddb3f8, 0x1fda836e,
	0x81be16cd, 0xf6b9265b, 0x6fb077e1, 0x18b74777,
	0x88085ae6, 0xff0f6a70, 0x66063bca, 0x11010b5c,
	0x8f659eff, 0xf862ae69, 0x616bffd3, 0x166ccf45,
	0xa00ae278, 0xd70dd2ee, 0x4e048354, 0x3903b3c2,
	0xa7672661, 0xd06016f7, 0x4969474d, 0x3e6e77db,
	0xaed16a4a, 0xd9d65adc, 0x40df0b66, 0x37d83bf0,
	0xa9bcae53, 0xdebb9ec5, 0x47b2cf7f, 0x30b5ffe9,
	0xbdbdf21c, 0xcabac28a, 0x53b39330, 0x24b4a3a6,
	0xbad03605, 0xcdd70693, 0x54de5729, 0x23d967bf,
	0xb3667a2e, 0xc4614ab8, 0x5d681b02, 0x2a6f2b94,
	0xb40bbe37, 0xc30c8ea1, 0x5a05df1b, 0x2d02ef8d,
};

gb_global u64 const GB__CRC64_TABLE[256] = {
	0x0000000000000000ull, 0x42f0e1eba9ea3693ull, 0x85e1c3d753d46d26ull, 0xc711223cfa3e5bb5ull,
	0x493366450e42ecdfull, 0x0bc387aea7a8da4cull, 0xccd2a5925d9681f9ull, 0x8e224479f47cb76aull,
	0x9266cc8a1c85d9beull, 0xd0962d61b56fef2dull, 0x17870f5d4f51b498ull, 0x5577eeb6e6bb820bull,
	0xdb55aacf12c73561ull, 0x99a54b24bb2d03f2ull, 0x5eb4691841135847ull, 0x1c4488f3e8f96ed4ull,
	0x663d78ff90e185efull, 0x24cd9914390bb37cull, 0xe3dcbb28c335e8c9ull, 0xa12c5ac36adfde5aull,
	0x2f0e1eba9ea36930ull, 0x6dfeff5137495fa3ull, 0xaaefdd6dcd770416ull, 0xe81f3c86649d3285ull,
	0xf45bb4758c645c51ull, 0xb6ab559e258e6ac2ull, 0x71ba77a2dfb03177ull, 0x334a9649765a07e4ull,
	0xbd68d2308226b08eull, 0xff9833db2bcc861dull, 0x388911e7d1f2dda8ull, 0x7a79f00c7818eb3bull,
	0xcc7af1ff21c30bdeull, 0x8e8a101488293d4dull, 0x499b3228721766f8ull, 0x0b6bd3c3dbfd506bull,
	0x854997ba2f81e701ull, 0xc7b97651866bd192ull, 0x00a8546d7c558a27ull, 0x4258b586d5bfbcb4ull,
	0x5e1c3d753d46d260ull, 0x1cecdc9e94ace4f3ull, 0xdbfdfea26e92bf46ull, 0x990d1f49c77889d5ull,
	0x172f5b3033043ebfull, 0x55dfbadb9aee082cull, 0x92ce98e760d05399ull, 0xd03e790cc93a650aull,
	0xaa478900b1228e31ull, 0xe8b768eb18c8b8a2ull, 0x2fa64ad7e2f6e317ull, 0x6d56ab3c4b1cd584ull,
	0xe374ef45bf6062eeull, 0xa1840eae168a547dull, 0x66952c92ecb40fc8ull, 0x2465cd79455e395bull,
	0x3821458aada7578full, 0x7ad1a461044d611cull, 0xbdc0865dfe733aa9ull, 0xff3067b657990c3aull,
	0x711223cfa3e5bb50ull, 0x33e2c2240a0f8dc3ull, 0xf4f3e018f031d676ull, 0xb60301f359dbe0e5ull,
	0xda050215ea6c212full, 0x98f5e3fe438617bcull, 0x5fe4c1c2b9b84c09ull, 0x1d14202910527a9aull,
	0x93366450e42ecdf0ull, 0xd1c685bb4dc4fb63ull, 0x16d7a787b7faa0d6ull, 0x5427466c1e109645ull,
	0x4863ce9ff6e9f891ull, 0x0a932f745f03ce02ull, 0xcd820d48a53d95b7ull, 0x8f72eca30cd7a324ull,
	0x0150a8daf8ab144eull, 0x43a04931514122ddull, 0x84b16b0dab7f7968ull, 0xc6418ae602954ffbull,
	0xbc387aea7a8da4c0ull, 0xfec89b01d3679253ull, 0x39d9b93d2959c9e6ull, 0x7b2958d680b3ff75ull,
	0xf50b1caf74cf481full, 0xb7fbfd44dd257e8cull, 0x70eadf78271b2539ull, 0x321a3e938ef113aaull,
	0x2e5eb66066087d7eull, 0x6cae578bcfe24bedull, 0xabbf75b735dc1058ull, 0xe94f945c9c3626cbull,
	0x676dd025684a91a1ull, 0x259d31cec1a0a732ull, 0xe28c13f23b9efc87ull, 0xa07cf2199274ca14ull,
	0x167ff3eacbaf2af1ull, 0x548f120162451c62ull, 0x939e303d987b47d7ull, 0xd16ed1d631917144ull,
	0x5f4c95afc5edc62eull, 0x1dbc74446c07f0bdull, 0xdaad56789639ab08ull, 0x985db7933fd39d9bull,
	0x84193f60d72af34full, 0xc6e9de8b7ec0c5dcull, 0x01f8fcb784fe9e69ull, 0x43081d5c2d14a8faull,
	0xcd2a5925d9681f90ull, 0x8fdab8ce70822903ull, 0x48cb9af28abc72b6ull, 0x0a3b7b1923564425ull,
	0x70428b155b4eaf1eull, 0x32b26afef2a4998dull, 0xf5a348c2089ac238ull, 0xb753a929a170f4abull,
	0x3971ed50550c43c1ull, 0x7b810cbbfce67552ull, 0xbc902e8706d82ee7ull, 0xfe60cf6caf321874ull,
	0xe224479f47cb76a0ull, 0xa0d4a674ee214033ull, 0x67c58448141f1b86ull, 0x253565a3bdf52d15ull,
	0xab1721da49899a7full, 0xe9e7c031e063acecull, 0x2ef6e20d1a5df759ull, 0x6c0603e6b3b7c1caull,
	0xf6fae5c07d3274cdull, 0xb40a042bd4d8425eull, 0x731b26172ee619ebull, 0x31ebc7fc870c2f78ull,
	0xbfc9838573709812ull, 0xfd39626eda9aae81ull, 0x3a28405220a4f534ull, 0x78d8a1b9894ec3a7ull,
	0x649c294a61b7ad73ull, 0x266cc8a1c85d9be0ull, 0xe17dea9d3263c055ull, 0xa38d0b769b89f6c6ull,
	0x2daf4f0f6ff541acull, 0x6f5faee4c61f773full, 0xa84e8cd83c212c8aull, 0xeabe6d3395cb1a19ull,
	0x90c79d3fedd3f122ull, 0xd2377cd44439c7b1ull, 0x15265ee8be079c04ull, 0x57d6bf0317edaa97ull,
	0xd9f4fb7ae3911dfdull, 0x9b041a914a7b2b6eull, 0x5c1538adb04570dbull, 0x1ee5d94619af4648ull,
	0x02a151b5f156289cull, 0x4051b05e58bc1e0full, 0x87409262a28245baull, 0xc5b073890b687329ull,
	0x4b9237f0ff14c443ull, 0x0962d61b56fef2d0ull, 0xce73f427acc0a965ull, 0x8c8315cc052a9ff6ull,
	0x3a80143f5cf17f13ull, 0x7870f5d4f51b4980ull, 0xbf61d7e80f251235ull, 0xfd913603a6cf24a6ull,
	0x73b3727a52b393ccull, 0x31439391fb59a55full, 0xf652b1ad0167feeaull, 0xb4a25046a88dc879ull,
	0xa8e6d8b54074a6adull, 0xea16395ee99e903eull, 0x2d071b6213a0cb8bull, 0x6ff7fa89ba4afd18ull,
	0xe1d5bef04e364a72ull, 0xa3255f1be7dc7ce1ull, 0x64347d271de22754ull, 0x26c49cccb40811c7ull,
	0x5cbd6cc0cc10fafcull, 0x1e4d8d2b65facc6full, 0xd95caf179fc497daull, 0x9bac4efc362ea149ull,
	0x158e0a85c2521623ull, 0x577eeb6e6bb820b0ull, 0x906fc95291867b05ull, 0xd29f28b9386c4d96ull,
	0xcedba04ad0952342ull, 0x8c2b41a1797f15d1ull, 0x4b3a639d83414e64ull, 0x09ca82762aab78f7ull,
	0x87e8c60fded7cf9dull, 0xc51827e4773df90eull, 0x020905d88d03a2bbull, 0x40f9e43324e99428ull,
	0x2cffe7d5975e55e2ull, 0x6e0f063e3eb46371ull, 0xa91e2402c48a38c4ull, 0xebeec5e96d600e57ull,
	0x65cc8190991cb93dull, 0x273c607b30f68faeull, 0xe02d4247cac8d41bull, 0xa2dda3ac6322e288ull,
	0xbe992b5f8bdb8c5cull, 0xfc69cab42231bacfull, 0x3b78e888d80fe17aull, 0x7988096371e5d7e9ull,
	0xf7aa4d1a85996083ull, 0xb55aacf12c735610ull, 0x724b8ecdd64d0da5ull, 0x30bb6f267fa73b36ull,
	0x4ac29f2a07bfd00dull, 0x08327ec1ae55e69eull, 0xcf235cfd546bbd2bull, 0x8dd3bd16fd818bb8ull,
	0x03f1f96f09fd3cd2ull, 0x41011884a0170a41ull, 0x86103ab85a2951f4ull, 0xc4e0db53f3c36767ull,
	0xd8a453a01b3a09b3ull, 0x9a54b24bb2d03f20ull, 0x5d45907748ee6495ull, 0x1fb5719ce1045206ull,
	0x919735e51578e56cull, 0xd367d40ebc92d3ffull, 0x1476f63246ac884aull, 0x568617d9ef46bed9ull,
	0xe085162ab69d5e3cull, 0xa275f7c11f7768afull, 0x6564d5fde549331aull, 0x279434164ca30589ull,
	0xa9b6706fb8dfb2e3ull, 0xeb46918411358470ull, 0x2c57b3b8eb0bdfc5ull, 0x6ea7525342e1e956ull,
	0x72e3daa0aa188782ull, 0x30133b4b03f2b111ull, 0xf7021977f9cceaa4ull, 0xb5f2f89c5026dc37ull,
	0x3bd0bce5a45a6b5dull, 0x79205d0e0db05dceull, 0xbe317f32f78e067bull, 0xfcc19ed95e6430e8ull,
	0x86b86ed5267cdbd3ull, 0xc4488f3e8f96ed40ull, 0x0359ad0275a8b6f5ull, 0x41a94ce9dc428066ull,
	0xcf8b0890283e370cull, 0x8d7be97b81d4019full, 0x4a6acb477bea5a2aull, 0x089a2aacd2006cb9ull,
	0x14dea25f3af9026dull, 0x562e43b4931334feull, 0x913f6188692d6f4bull, 0xd3cf8063c0c759d8ull,
	0x5dedc41a34bbeeb2ull, 0x1f1d25f19d51d821ull, 0xd80c07cd676f8394ull, 0x9afce626ce85b507ull,
};

u32
gb_crc32(void const *data, isize len)
{
	isize remaining;
	u32 result = ~(cast(u32)0);
	u8 const *c = cast(u8 const *)data;
	for (remaining = len; remaining--; c++)
		result = (result >> 8) ^ (GB__CRC32_TABLE[(result ^ *c) & 0xff]);
	return ~result;
}

u64
gb_crc64(void const *data, isize len)
{
	isize remaining;
	u64 result = ~(cast(u64)0);
	u8 const *c = cast(u8 const *)data;
	for (remaining = len; remaining--; c++)
		result = (result >> 8) ^ (GB__CRC64_TABLE[(result ^ *c) & 0xff]);
	return ~result;
}

u32
gb_fnv32(void const *data, isize len)
{
	isize i;
	u32 h = 0x811c9dc5;
	u8 const *c = cast(u8 const *)data;

	for (i = 0; i < len; i++)
		h = (h * 0x01000193) ^ c[i];

	return h;
}

u64
gb_fnv64(void const *data, isize len)
{
	isize i;
	u64 h = 0xcbf29ce484222325ull;
	u8 const *c = cast(u8 const *)data;

	for (i = 0; i < len; i++)
		h = (h * 0x100000001b3ll) ^ c[i];

	return h;
}

u32
gb_fnv32a(void const *data, isize len)
{
	isize i;
	u32 h = 0x811c9dc5;
	u8 const *c = cast(u8 const *)data;

	for (i = 0; i < len; i++)
		h = (h ^ c[i]) * 0x01000193;

	return h;
}

u64
gb_fnv64a(void const *data, isize len)
{
	isize i;
	u64 h = 0xcbf29ce484222325ull;
	u8 const *c = cast(u8 const *)data;

	for (i = 0; i < len; i++)
		h = (h ^ c[i]) * 0x100000001b3ll;

	return h;
}

gb_inline u32 gb_murmur32(void const *data, isize len) { return gb_murmur32_seed(data, len, 0x9747b28c); }
gb_inline u64 gb_murmur64(void const *data, isize len) { return gb_murmur64_seed(data, len, 0x9747b28c); }

u32
gb_murmur32_seed(void const *data, isize len, u32 seed)
{
	u32 const c1 = 0xcc9e2d51;
	u32 const c2 = 0x1b873593;
	u32 const r1 = 15;
	u32 const r2 = 13;
	u32 const m  = 5;
	u32 const n  = 0xe6546b64;

	isize i, nblocks = len / 4;
	u32 hash = seed, k1 = 0;
	u32 const *blocks = cast(u32 const*)data;
	u8 const *tail = cast(u8 const *)(data) + nblocks*4;

	for (i = 0; i < nblocks; i++) {
		u32 k = blocks[i];
		k *= c1;
		k = (k << r1) | (k >> (32 - r1));
		k *= c2;

		hash ^= k;
		hash = ((hash << r2) | (hash >> (32 - r2))) * m + n;
	}

	switch (len & 3) {
	case 3:
		k1 ^= tail[2] << 16;
	case 2:
		k1 ^= tail[1] << 8;
	case 1:
		k1 ^= tail[0];

		k1 *= c1;
		k1 = (k1 << r1) | (k1 >> (32 - r1));
		k1 *= c2;
		hash ^= k1;
	}

	hash ^= len;
	hash ^= (hash >> 16);
	hash *= 0x85ebca6b;
	hash ^= (hash >> 13);
	hash *= 0xc2b2ae35;
	hash ^= (hash >> 16);

	return hash;
}

u64
gb_murmur64_seed(void const *data_, isize len, u64 seed)
{
#if defined(GB_ARCH_64_BIT)
	u64 const m = 0xc6a4a7935bd1e995ULL;
	i32 const r = 47;

	u64 h = seed ^ (len * m);

	u64 const *data = cast(u64 const *)data_;
	u8  const *data2 = cast(u8 const *)data_;
	u64 const* end = data + (len / 8);

	while (data != end) {
		u64 k = *data++;

		k *= m;
		k ^= k >> r;
		k *= m;

		h ^= k;
		h *= m;
	}

	switch (len & 7) {
	case 7: h ^= cast(u64)(data2[6]) << 48;
	case 6: h ^= cast(u64)(data2[5]) << 40;
	case 5: h ^= cast(u64)(data2[4]) << 32;
	case 4: h ^= cast(u64)(data2[3]) << 24;
	case 3: h ^= cast(u64)(data2[2]) << 16;
	case 2: h ^= cast(u64)(data2[1]) << 8;
	case 1: h ^= cast(u64)(data2[0]);
		h *= m;
	};

	h ^= h >> r;
	h *= m;
	h ^= h >> r;

	return h;
#else
	u64 h;
	u32 const m = 0x5bd1e995;
	i32 const r = 24;

	u32 h1 = cast(u32)(seed) ^ cast(u32)(len);
	u32 h2 = cast(u32)(seed >> 32);

	u32 const *data = cast(u32 const *)data_;

	while (len >= 8) {
		u32 k1, k2;
		k1 = *data++;
		k1 *= m;
		k1 ^= k1 >> r;
		k1 *= m;
		h1 *= m;
		h1 ^= k1;
		len -= 4;

		k2 = *data++;
		k2 *= m;
		k2 ^= k2 >> r;
		k2 *= m;
		h2 *= m;
		h2 ^= k2;
		len -= 4;
	}

	if (len >= 4) {
		u32 k1 = *data++;
		k1 *= m;
		k1 ^= k1 >> r;
		k1 *= m;
		h1 *= m;
		h1 ^= k1;
		len -= 4;
	}

	switch (len) {
	case 3: h2 ^= (cast(u8 const *)data)[2] << 16;
	case 2: h2 ^= (cast(u8 const *)data)[1] <<  8;
	case 1: h2 ^= (cast(u8 const *)data)[0] <<  0;
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

	h = h1;
	h = (h << 32) | h2;

	return h;
#endif
}



/***************************************************************
 *
 * Hash Table
 *
 */


gb_inline void
gb_hash_table_init(gbHashTable *h, gbAllocator a)
{
	gb_array_init(h->hashes, a);
	gb_array_init(h->entries, a);
}

gb_inline void
gb_hash_table_free(gbHashTable *h)
{
	gb_array_free(&h->hashes);
	gb_array_free(&h->entries);
}

typedef struct gbprivFindResult {
	isize hash_index;
	isize data_prev;
	isize entry_index;
} gbprivFindResult;

gbprivFindResult const GB__INVALID_FIND_RESULT = {-1, -1, -1};

gbprivFindResult
gb__find_result_from_key(gbHashTable const *h, u64 key)
{
	gbprivFindResult fr = GB__INVALID_FIND_RESULT;

	if (gb_array_count(h->hashes) == 0)
		return fr;

	fr.hash_index = key % gb_array_count(h->hashes);
	fr.entry_index = h->hashes[fr.hash_index];
	while (fr.entry_index >= 0) {
		if (h->entries[fr.entry_index].key == key)
			return fr;
		fr.data_prev = fr.entry_index;
		fr.entry_index = h->entries[fr.entry_index].next;
	}

	return fr;
}

gb_inline b32
gb_hash_table_has(gbHashTable const *h, u64 key)
{
	return gb__find_result_from_key(h, key).entry_index >= 0;
}

gb_inline void *
gb_hash_table_get(gbHashTable const *h, u64 key, void *default_value)
{
	isize index = gb__find_result_from_key(h, key).entry_index;

	if (index < 0)
		return default_value;
	return h->entries[index].value;
}

gb_internal gb_inline isize
gb__hash_table_add_entry(gbHashTable *h, u64 key)
{
	isize i = gb_array_count(h->entries);
	gbHashTableEntry e = {0};
	e.key = key;
	e.next = -1;
	gb_array_append(h->entries, e);
	return i;
}

gb_internal gb_inline isize
gb__hash_table_is_full(gbHashTable *h)
{
	f64 const MAXIMUM_LOAD_COEFFICIENT = 0.75;
	return gb_array_count(h->entries) >= MAXIMUM_LOAD_COEFFICIENT * gb_array_count(h->hashes);
}

gb_internal gb_inline void gb__hash_table_grow(gbHashTable *h);

gb_inline void
gb_multi_hash_table_insert(gbHashTable *h, u64 key, void *value)
{
	gbprivFindResult fr;
	isize next;
	if (gb_array_count(h->hashes) == 0)
		gb__hash_table_grow(h);

	fr = gb__find_result_from_key(h, key);
	next = gb__hash_table_add_entry(h, key);

	if (fr.data_prev < 0)
		h->hashes[fr.hash_index] = next;
	else
		h->entries[fr.data_prev].next = next;

	h->entries[next].next = fr.entry_index;
	h->entries[next].value = value;

	if (gb__hash_table_is_full(h))
		gb__hash_table_grow(h);
}

gb_inline void
gb_multi_hash_table_get(gbHashTable const *h, u64 key, void **values, isize count)
{
	isize i = 0;
	gbHashTableEntry const *e = gb_multi_hash_table_find_first_entry(h, key);
	while (e && count --> 0) {
		values[i++] = e->value;
		e = gb_multi_hash_table_find_next_entry(h, e);
	}
}

gb_inline isize
gb_multi_hash_table_count(gbHashTable const *h, u64 key)
{
	isize count = 0;
	gbHashTableEntry const *e = gb_multi_hash_table_find_first_entry(h, key);
	while (e) {
		count++;
		e = gb_multi_hash_table_find_next_entry(h, e);
	}
	return count;
}

gbHashTableEntry const *
gb_multi_hash_table_find_first_entry(gbHashTable const *h, u64 key)
{
	isize index = gb__find_result_from_key(h, key).entry_index;
	if (index < 0) return NULL;
	return &h->entries[index];
}

gbHashTableEntry const *
gb_multi_hash_table_find_next_entry(gbHashTable const *h, gbHashTableEntry const *e)
{
	if (e) {
		isize index = e->next;
		while (index >= 0) {
			if (h->entries[index].key == e->key)
				return &h->entries[index];
			index = h->entries[index].next;
		}
	}
	return NULL;
}

void
gb__hash_table_erase_find_result(gbHashTable *h, gbprivFindResult fr)
{
	if (fr.data_prev < 0)
		h->hashes[fr.hash_index] = h->entries[fr.entry_index].next;
	else
		h->entries[fr.data_prev].next = h->entries[fr.entry_index].next;

	gb_array_pop(&h->entries);

	if (fr.entry_index != gb_array_count(h->entries)) {
		gbprivFindResult last;
		h->entries[fr.entry_index] = h->entries[gb_array_count(h->entries)];
		last = gb__find_result_from_key(h, h->entries[fr.entry_index].key);
		if (last.data_prev < 0)
			h->hashes[last.hash_index] = fr.entry_index;
		else
			h->entries[last.entry_index].next = fr.entry_index;
	}
}

gb_inline void
gb_multi_hash_table_remove_entry(gbHashTable *h, gbHashTableEntry const *e)
{
	gbprivFindResult fr = GB__INVALID_FIND_RESULT;

	if (gb_array_count(h->hashes) && e) {
		fr.hash_index  = e->key % gb_array_count(h->hashes);
		fr.entry_index = h->hashes[fr.hash_index];
		while (fr.entry_index >= 0) {
			if (&h->entries[fr.entry_index] == e)
				break;
			fr.data_prev = fr.entry_index;
			fr.entry_index = h->entries[fr.entry_index].next;
		}
	}

	if (fr.entry_index >= 0)
		gb__hash_table_erase_find_result(h, fr);
}

gb_inline void
gb_multi_hash_table_remove_all(gbHashTable *h, u64 key)
{
	while (gb_hash_table_has(h, key))
		gb_hash_table_remove(h, key);
}

void
gb__rehash(gbHashTable *h, isize new_capacity)
{
	gbHashTable nh, empty;
	isize i;

	gb_hash_table_init(&nh, gb_array_allocator(h->hashes));
	gb_array_resize(nh.hashes, new_capacity);
	gb_array_reserve(nh.entries, gb_array_count(h->entries));

	for (i = 0; i < new_capacity; i++)
		nh.hashes[i] = -1;

	for (i = 0; i < gb_array_count(h->entries); i++) {
		gbHashTableEntry *e = &h->entries[i];
		gb_multi_hash_table_insert(&nh, e->key, e->value);
	}

	gb_hash_table_init(&empty, gb_array_allocator(h->hashes));
	gb_hash_table_free(h);

	gb_memcopy(&nh, &h, gb_size_of(gbHashTable));
	gb_memcopy(&empty, &nh, gb_size_of(gbHashTable));
}

gb_internal gb_inline void
gb__hash_table_grow(gbHashTable *h)
{
	isize new_capacity = GB_ARRAY_GROW_FORMULA(gb_array_count(h->entries));
	gb__rehash(h, new_capacity);
}

isize
gb__find_or_make_entry(gbHashTable *h, u64 key)
{
	isize index;
	gbprivFindResult fr = gb__find_result_from_key(h, key);
	if (fr.entry_index >= 0)
		return fr.entry_index;
	index = gb__hash_table_add_entry(h, key);
	if (fr.data_prev < 0)
		h->hashes[fr.hash_index] = index;
	else
		h->entries[fr.data_prev].next = index;
	return index;
}

gb_inline void
gb_hash_table_set(gbHashTable *h, u64 key, void *value)
{
	isize i;
	if (gb_array_count(h->hashes) == 0)
		gb__hash_table_grow(h);
	i = gb__find_or_make_entry(h, key);
	h->entries[i].value = value;
	if (gb__hash_table_is_full(h))
		gb__hash_table_grow(h);
}

gb_inline void
gb_hash_table_remove(gbHashTable *h, u64 key)
{
	gbprivFindResult fr = gb__find_result_from_key(h, key);
	if (fr.entry_index >= 0)
		gb__hash_table_erase_find_result(h, fr);
}

gb_inline void
gb_hash_table_reserve(gbHashTable *h, isize capacity)
{
	gb__rehash(h, capacity);
}

gb_inline void
gb_hash_table_clear(gbHashTable *h)
{
	gb_array_clear(&h->hashes);
	gb_array_clear(&h->entries);
}




/***************************************************************
 *
 * File Handling
 *
 */

#if defined(GB_SYSTEM_WINDOWS)

gb_inline gbFile
gb_file_get_std(gbFileStandardType std)
{
	gbFile file = {0};
	switch (std) {
	case GB_FILE_STANDARD_INPUT:  file.handle = GetStdHandle(STD_INPUT_HANDLE);  break;
	case GB_FILE_STANDARD_OUTPUT: file.handle = GetStdHandle(STD_OUTPUT_HANDLE); break;
	case GB_FILE_STANDARD_ERROR:  file.handle = GetStdHandle(STD_ERROR_HANDLE);  break;
	default: GB_PANIC("Invalid standard file"); break;
	}
	return file;
}


gbFileError
gb_file_open_file_va(gbFile *file, u32 flag, gbFileMode mode, char const *filename, va_list va)
{
	char const *name = gb_sprintf_va(filename, va);
	gb_zero_struct(file);

	if (name) file->name = gb_alloc_str(gb_heap_allocator(), name);

	{
		DWORD  desired_access       = 0;
		DWORD  share_mode           = 0;
		DWORD  creation_disposition = 0;

		if (flag & GB_FILE_READ)       desired_access |= GENERIC_READ;
		if (flag & GB_FILE_WRITE)      desired_access |= GENERIC_WRITE;
		if (flag & GB_FILE_READ_WRITE) desired_access |= (GENERIC_READ|GENERIC_WRITE);
		if (flag & GB_FILE_APPEND)     desired_access |= (GENERIC_READ|GENERIC_WRITE);
		if (flag & GB_FILE_CREATE)     desired_access |= (GENERIC_READ|GENERIC_WRITE);

		if (mode & GB_FILE_MODE_READ)   share_mode |= FILE_SHARE_READ;
		if (mode & GB_FILE_MODE_WRITE)  share_mode |= FILE_SHARE_WRITE;
		if (mode & GB_FILE_MODE_DELETE) share_mode |= FILE_SHARE_DELETE;

		/* if (flag & GB_FILE_CREATE) creation_disposition |= */
		if (flag & GB_FILE_TRUNCATE) creation_disposition |= TRUNCATE_EXISTING;
		if (flag & GB_FILE_READ)     creation_disposition |= OPEN_EXISTING;
		if (flag & GB_FILE_WRITE)    creation_disposition |= OPEN_EXISTING;
		if (flag & GB_FILE_CREATE)   creation_disposition |= CREATE_ALWAYS;


		file->handle = CreateFileA(file->name,
		                           desired_access,
		                           share_mode,
		                           NULL,
		                           creation_disposition,
		                           0,
		                           NULL);
	}


	file->last_write_time = gb_file_last_write_time("%s", file->name);
	return GB_FILE_ERR_NONE;
}

gbFileError
gb_file_close(gbFile *file)
{
	if (!file)
		return GB_FILE_ERR_INVALID;

	if (file->name) gb_free(gb_heap_allocator(), file->name);

	if (file->handle == INVALID_HANDLE_VALUE)
		return GB_FILE_ERR_INVALID;

	CloseHandle(file->handle);

	return GB_FILE_ERR_NONE;
}

b32
gb_file_read(gbFile *f, void *buffer, isize size)
{
	DWORD bytes_read;
	if (ReadFile(f->handle, buffer, cast(DWORD)size, &bytes_read, NULL))
		return bytes_read == size;
	return false;
}

b32
gb_file_write(gbFile *f, void const *buffer, isize size)
{
	DWORD bytes_written;
	if (WriteFile(f->handle, buffer, cast(DWORD)size, &bytes_written, NULL))
		return bytes_written == size;
	return false;
}

gb_inline i64
gb_file_seek(gbFile *file, i64 offset_, gbSeekWhence whence)
{
	LARGE_INTEGER offset;
	LARGE_INTEGER new_offset;
	offset.QuadPart = offset_;
	SetFilePointerEx(file->handle, offset, &new_offset, whence);
	return cast(i64)new_offset.QuadPart;
}

gb_inline i64
gb_file_tell(gbFile *file)
{
	return gb_file_seek(file, 0, GB_SEEK_CURRENT);
}

gb_inline i64
gb_file_size(gbFile *file)
{
	LARGE_INTEGER size;
	GetFileSizeEx(file->handle, &size);
	return size.QuadPart;
}

gb_inline gbFileError
gb_file_truncate(gbFile *file, i64 size)
{
	gbFileError err = GB_FILE_ERR_NONE;
	i64 prev_offset = gb_file_tell(file);
	gb_file_seek(file, size, GB_SEEK_BEGIN);
	if (!SetEndOfFile(file))
		err = GB_FILE_ERR_TRUNCATION_FAILURE;
	gb_file_seek(file, prev_offset, GB_SEEK_BEGIN);
	return err;
}


b32
gb_file_is_exist(char const *name)
{
	WIN32_FIND_DATA data;
	HANDLE handle = FindFirstFile(name, &data);
	b32 found = handle != INVALID_HANDLE_VALUE;
	if (found) FindClose(handle);
	return found;
}

#else
gb_inline gbFile
gb_file_get_std(gbFileStandardType std)
{
	gbFile file = {0};
	switch (std) {
	case GB_FILE_STANDARD_INPUT:  file.handle = cast(void *)cast(intptr)0; break;
	case GB_FILE_STANDARD_OUTPUT: file.handle = cast(void *)cast(intptr)1; break;
	case GB_FILE_STANDARD_ERROR:  file.handle = cast(void *)cast(intptr)2; break;
	default: GB_PANIC("Invalid standard file"); break;
	}
	return file;
}

gbFileError
gb_file_open_file_va(gbFile *file, u32 flag, gbFileMode mode, char const *filename, va_list va)
{
	int fd = 0;
	int posix_flags = 0;
	mode_t posix_mode = {0};
	char const *name = gb_sprintf_va(filename, va);
	gb_zero_struct(file);

	if (name) file->name = gb_alloc_str(gb_heap_allocator(), name);

	{
		/* TODO(bill): THROUGHLY TEST!!! */
		if (flag & (GB_FILE_READ | GB_FILE_WRITE))
			posix_flags |= O_RDWR;
		else if (flag & GB_FILE_READ)
			posix_flags |= O_RDONLY;
		else if (flag & GB_FILE_WRITE)
			posix_flags |= O_WRONLY;

		if (flag & GB_FILE_APPEND)   posix_flags |= O_APPEND;
		if (flag & GB_FILE_CREATE)   posix_flags |= O_CREAT;
		if (flag & GB_FILE_TRUNCATE) posix_flags |= O_TRUNC;

		if (mode & GB_FILE_MODE_READ)   posix_mode |= S_IRUSR;
		if (mode & GB_FILE_MODE_WRITE)  posix_mode |= S_IWUSR;
		if (mode & GB_FILE_MODE_DELETE) {/* TODO(bill): GB_FILE_MODE_DELETE */}

		fd = open(file->name, posix_flags, posix_mode);
		if (fd < 0) {
			switch (errno) {
			case EACCES: return GB_FILE_ERR_PERMISSION;
			case EEXIST: return GB_FILE_ERR_EXISTS;
			case ENOENT: return GB_FILE_ERR_NOT_EXISTS;

			case EMFILE: /* FALLTHROUGH */
			case EINVAL:
			default:
				return GB_FILE_ERR_INVALID;
			}
		}
	}

	file->handle = cast(void *)cast(intptr)fd;
	file->last_write_time = gb_file_last_write_time("%s", file->name);
	return GB_FILE_ERR_NONE;
}

gbFileError
gb_file_close(gbFile *file)
{
	if (!file)
		return GB_FILE_ERR_INVALID;

	if (file->name) gb_free(gb_heap_allocator(), file->name);

	if (cast(intptr)file->handle < 0)
		return GB_FILE_ERR_INVALID;

	close(cast(int)cast(intptr)file->handle);

	return GB_FILE_ERR_NONE;
}

b32
gb_file_read(gbFile *f, void *buffer, isize size)
{
	isize bytes_read;
	bytes_read = read(cast(int)cast(intptr)f->handle, buffer, size);
	return bytes_read == size;
}

b32
gb_file_write(gbFile *f, void const *buffer, isize size)
{
	isize bytes_written;
	bytes_written = write(cast(int)cast(intptr)f->handle, buffer, size);
	return bytes_written == size;
}

gb_inline i64
gb_file_seek(gbFile *file, i64 offset, gbSeekWhence whence)
{
	return lseek64(cast(int)cast(intptr)file->handle, offset, whence);
}

gb_inline i64
gb_file_tell(gbFile *file)
{
	return gb_file_seek(file, 0, GB_SEEK_CURRENT);
}

gb_inline i64
gb_file_size(gbFile *file)
{
	i64 size = 0;
	i64 prev_offset = gb_file_tell(file);
	gb_file_seek(file, 0, GB_SEEK_END);
	size = gb_file_tell(file);
	gb_file_seek(file, prev_offset, GB_SEEK_BEGIN);
	return size;
}

gb_inline gbFileError
gb_file_truncate(gbFile *file, i64 size)
{
	gbFileError err = GB_FILE_ERR_NONE;
	int i = ftruncate(cast(int)cast(intptr)file->handle, size);
	if (i != 0) err = GB_FILE_ERR_TRUNCATION_FAILURE;
	return err;
}

b32
gb_file_is_exist(char const *name)
{
	return access(name, F_OK) != -1;
}
#endif


gb_inline b32 gb_file_is_not_exist(char const *name) { return !gb_file_is_exist(name); }

gbFileError
gb_file_create(gbFile *file, char const *filename, ...)
{
	gbFileError err;
	va_list va;
	va_start(va, filename);
	err = gb_file_open_file_va(file, GB_FILE_READ_WRITE|GB_FILE_CREATE, GB_FILE_MODE_READ|GB_FILE_MODE_WRITE, filename, va);
	va_end(va);
	return err;
}


gbFileError
gb_file_open(gbFile *file, char const *filename, ...)
{
	gbFileError err;
	va_list va;
	va_start(va, filename);
	err = gb_file_open_file_va(file, GB_FILE_READ, GB_FILE_MODE_READ, filename, va);
	va_end(va);
	return err;
}

gbFileError
gb_file_open_file(gbFile *file, u32 flag, gbFileMode perm, char const *filename, ...)
{
	gbFileError err;
	va_list va;
	va_start(va, filename);
	err = gb_file_open_file_va(file, flag, perm, filename, va);
	va_end(va);
	return err;
}

b32
gb_file_read_at(gbFile *f, void *buffer, isize size, i64 offset)
{
	int bytes_read;
	i64 prev_offset;

	prev_offset = gb_file_tell(f);
	gb_file_seek(f, offset, GB_SEEK_BEGIN);
	bytes_read = gb_file_read(f, buffer, size);
	gb_file_seek(f, prev_offset, GB_SEEK_BEGIN);

	return bytes_read == size;
}

b32
gb_file_write_at(gbFile *f, void const *buffer, isize size, i64 offset)
{
	int bytes_written;
	i64 prev_offset = 0;

	prev_offset = gb_file_tell(f);
	gb_file_seek(f, offset, GB_SEEK_BEGIN);
	bytes_written = gb_file_write(f, buffer, size);
	gb_file_seek(f, prev_offset, GB_SEEK_BEGIN);

	return bytes_written == size;
}

char const *gb_file_name(gbFile *file) { return file->name ? file->name : ""; }

gb_inline b32
gb_file_has_changed(gbFile *file)
{
	b32 result = false;
	gbFileTime last_write_time = gb_file_last_write_time("%s", file->name);
	if (file->last_write_time != last_write_time) {
		result = true;
		file->last_write_time = last_write_time;
	}
	return result;
}






#if defined(GB_SYSTEM_WINDOWS)
gbFileTime
gb_file_last_write_time(char const *filepath, ...)
{
	ULARGE_INTEGER li = {0};
	FILETIME last_write_time = {0};
	WIN32_FILE_ATTRIBUTE_DATA data = {0};

	va_list va;
	va_start(va, filepath);

	if (GetFileAttributesExA(gb_sprintf_va(filepath, va), GetFileExInfoStandard, &data))
		last_write_time = data.ftLastWriteTime;

	va_end(va);

	li.LowPart = last_write_time.dwLowDateTime;
	li.HighPart = last_write_time.dwHighDateTime;
	return cast(gbFileTime)li.QuadPart;
}


gb_inline b32
gb_file_copy(char const *existing_filename, char const *new_filename, b32 fail_if_exists)
{
	return CopyFileA(existing_filename, new_filename, fail_if_exists);
}

gb_inline b32
gb_file_move(char const *existing_filename, char const *new_filename)
{
	return MoveFileA(existing_filename, new_filename);
}



#else

gbFileTime
gb_file_last_write_time(char const *filepath, ...)
{
	time_t result = 0;

	struct stat file_stat;
	va_list va;
	va_start(va, filepath);
	if (stat(gb_sprintf_va(filepath, va), &file_stat)) {
		result = file_stat.st_mtime;
	}

	va_end(va);

	return cast(gbFileTime)result;
}


gb_inline b32
gb_file_copy(char const *existing_filename, char const *new_filename, b32 fail_if_exists)
{
	int fd_to, fd_from;
	u8 buf[4096];
	isize read_bytes;
	int saved_errno;

	fd_from = open(existing_filename, O_RDONLY);
	if (fd_from < 0) return false;
	
	fd_to = open(new_filename, O_RDONLY);
	if (fd_to >= 0 && fail_if_exists)
		goto err_cleanup;

	fd_to = open(new_filename, O_WRONLY | O_CREAT | O_EXCL, 0666);
	if (fd_to < 0)
		goto err_cleanup;

	while (read_bytes = read(fd_from, buf, gb_size_of(buf)), read_bytes > 0) {
		u8 *out = buf;
		isize written_bytes;

		do {
			written_bytes = write(fd_to, out, read_bytes);
			if (written_bytes >= 0) {
				read_bytes -= written_bytes;
				out        += written_bytes;
			} else if (errno != EINTR) {
				goto err_cleanup;
			}
		} while (read_bytes > 0);
	}

	if (read_bytes == 0) {
		if (close(fd_to) < 0) {
			fd_to = -1;
			goto err_cleanup;
		}
		close(fd_from);
		return true; /* NOTE(bill): IT WORKED! */
	}

err_cleanup:
	saved_errno = errno;

	close(fd_from);
	if (fd_to >= 0) close(fd_to);

	errno = saved_errno;
	return false;
}

gb_inline b32
gb_file_move(char const *existing_filename, char const *new_filename)
{
	/* TODO(bill): Is this TOO HACKY?! rename() would do fine really but it's in stdio */
	return system(gb_sprintf("mv %s %s", existing_filename, new_filename)) == 0;
}

#endif





gbFileContents
gb_file_read_contents(gbAllocator a, b32 zero_terminate, char const *filepath, ...)
{
	gbFileContents result = {0};
	gbFile file = {0};
	char *path;
	va_list va;
	va_start(va, filepath);
	path = gb_sprintf_va(filepath, va);
	va_end(va);

	if (gb_file_open(&file, "%s", path)) {
		i64 file_size = gb_file_size(&file);
		if (file_size > 0) {
			result.data = gb_alloc(a, zero_terminate ? file_size+1 : file_size);
			result.size = file_size;
			gb_file_read_at(&file, result.data, result.size, 0);
			if (zero_terminate) {
				u8 *str = cast(u8 *)result.data;
				str[file_size] = '\0';
			}
		}
		gb_file_close(&file);
	}

	return result;
}





gb_inline b32
gb_path_is_absolute(char const *path)
{
	b32 result = false;
	GB_ASSERT_NOT_NULL(path);
#if defined(GB_SYSTEM_WINDOWS)
	result == (gb_strlen(path) > 2) &&
	          gb_char_is_alpha(path[0]) &&
	          (path[1] == ':' && path[2] == GB_PATH_SEPARATOR);
#else
	result = (gb_strlen(path) > 0 && path[0] == GB_PATH_SEPARATOR);
#endif
	return result;
}

gb_inline b32 gb_path_is_relative(char const *path) { return !gb_path_is_absolute(path); }

gb_inline b32
gb_path_is_root(char const *path)
{
	b32 result = false;
	GB_ASSERT_NOT_NULL(path);
#if defined(GB_SYSTEM_WINDOWS)
	result = gb_path_is_absolute(path) && gb_strlen(path) == 3;
#else
	result = gb_path_is_absolute(path) && gb_strlen(path) == 1;
#endif
	return result;
}

gb_inline char const *
gb_path_base_name(char const *path)
{
	char const *ls;
	GB_ASSERT_NOT_NULL(path);
	ls = gb_char_last_occurence(path, '/');
	return (ls == NULL) ? path : ls+1;
}

gb_inline char const *
gb_path_extension(char const *path)
{
	char const *ld;
	GB_ASSERT_NOT_NULL(path);
	ld = gb_char_last_occurence(path, '.');
	return (ld == NULL) ? NULL : ld+1;
}



/***************************************************************
 *
 * DLL Handling
 *
 */

#if defined(GB_SYSTEM_WINDOWS)

gbDllHandle
gb_dll_load(char const *filepath, ...)
{
	gb_local_persist char buffer[512];
	va_list va;
	va_start(va, filepath);
	gb_snprintf_va(buffer, gb_size_of(buffer), filepath, va);
	va_end(va);
	return cast(gbDllHandle)LoadLibraryA(buffer);
}
gb_inline void      gb_dll_unload      (gbDllHandle dll)                        { FreeLibrary(cast(HMODULE)dll); }
gb_inline gbDllProc gb_dll_proc_address(gbDllHandle dll, char const *proc_name) { return cast(gbDllProc)GetProcAddress(cast(HMODULE)dll, proc_name); }

#else

gbDllHandle
gb_dll_load(char const *filepath, ...)
{
	gb_local_persist char buffer[512];
	va_list va;
	va_start(va, filepath);
	gb_snprintf_va(buffer, gb_size_of(buffer), filepath, va);
	va_end(va);
	/* TODO(bill): Should this be RTLD_LOCAL? */
	return cast(gbDllHandle)dlopen(buffer, RTLD_LAZY|RTLD_GLOBAL);
}

gb_inline void      gb_dll_unload      (gbDllHandle dll)                        { dlclose(dll); }
gb_inline gbDllProc gb_dll_proc_address(gbDllHandle dll, char const *proc_name) { return cast(gbDllProc)dlsym(dll, proc_name); }

#endif


/***************************************************************
 *
 * Time
 *
 */

#if defined(_MSC_VER) && !defined(__clang__)
	gb_inline u64 gb_rdtsc(void) { return __rdtsc(); }
#elif defined(__i386__)
	gb_inline u64
	gb_rdtsc(void)
	{
		u64 x;
		__asm__ volatile (".byte 0x0f, 0x31" : "=A" (x));
		return x;
	}
#elif defined(__x86_64__)
	gb_inline u64
	gb_rdtsc(void)
	{
		u32 hi, lo;
		__asm__ __volatile__ ("rdtsc" : "=a"(lo), "=d"(hi));
		return (cast(u64)lo) | ((cast(u64)hi)<<32);
	}
#elif defined(__powerpc__)
	gb_inline u64
	gb_rdtsc(void)
	{
		u64 result = 0;
		u32 upper, lower,tmp;
		__asm__ volatile(
			"0:                   \n"
			"\tmftbu   %0         \n"
			"\tmftb    %1         \n"
			"\tmftbu   %2         \n"
			"\tcmpw    %2,%0      \n"
			"\tbne     0b         \n"
			: "=r"(upper),"=r"(lower),"=r"(tmp)
		);
		result = upper;
		result = result<<32;
		result = result|lower;

		return result;
	}
#endif

#if defined(GB_SYSTEM_WINDOWS)

	gb_inline f64
	gb_time_now(void)
	{
		gb_local_persist LARGE_INTEGER win32_perf_count_freq = {0};
		f64 result;
		LARGE_INTEGER counter;
		if (!win32_perf_count_freq.QuadPart) {
			QueryPerformanceFrequency(&win32_perf_count_freq);
			GB_ASSERT(win32_perf_count_freq.QuadPart != 0);
		}

		QueryPerformanceCounter(&counter);

		result = counter.QuadPart / cast(f64)(win32_perf_count_freq.QuadPart);
		return result;
	}

	gb_inline u64
	gb_utc_time_now(void)
	{
		FILETIME ft;
		ULARGE_INTEGER li;

		GetSystemTimeAsFileTime(&ft);
		li.LowPart = ft.dwLowDateTime;
		li.HighPart = ft.dwHighDateTime;

		return li.QuadPart/10;
	}

	gb_inline void gb_sleep_ms(u32 ms) { Sleep(ms); }

#else

	gb_global f64 gb__timebase  = 0.0;
	gb_global u64 gb__timestart = 0;

	gb_inline f64
	gb_time_now(void)
	{
#if defined(GB_SYSTEM_OSX)
		f64 result;

		if (!gb__timestart) {
			mach_timebase_info_data_t tb = {0};
			mach_timebase_info(&tb);
			gb__timebase = tb.numer;
			gb__timebase /= tb.denom;
			gb__timestart = mach_absolute_time();
		}

		result = (mach_absolute_time() - gb__timestart) * gb__timebase;
		return result;
#else
		struct timespec t;
		f64 result;

		/* TODO(bill): THIS IS A HACK  */
		clock_gettime(1 /*CLOCK_MONOTONIC*/, &t);
		result = t.tv_sec + 1.0e-9 * t.tv_nsec;
		return result;
#endif
	}

	gb_inline u64
	gb_utc_time_now(void)
	{
		struct timespec t;
		/* TODO(bill): THIS IS A HACK */
		clock_gettime(0 /*CLOCK_REALTIME*/, &t);
		return cast(u64)t.tv_sec * 1000000ull + t.tv_nsec/1000 + 11644473600000000ull;
	}

	gb_inline void
	gb_sleep_ms(u32 ms)
	{
		struct timespec req = {cast(time_t)ms/1000, cast(long)((ms%1000)*1000000)};
		struct timespec rem = {0, 0};
		nanosleep(&req, &rem);
	}

#endif



/***************************************************************
 *
 * Miscellany
 *
 */

gb_internal gb_inline u64
gb__basic_hash(u64 x)
{
	/* NOTE(bill): Used in Murmur Hash */
	x ^= x >> 33;
    x *= 0xff51afd7ed558ccdull;
    x ^= x >> 33;
    x *= 0xc4ceb9fe1a85ec53ull;
    x ^= x >> 33;
    return x;
}


void
gb_random_init(gbRandom *r)
{
	u64 t;
	isize i;
	t = gb_utc_time_now();
	t = gb__basic_hash(t);
	r->seed[0] = gb__basic_hash(t|1);

	t = cast(u64)gb_time_now();
	t = gb__basic_hash(t) + (gb__basic_hash(gb_thread_current_id()) << 1);
	r->seed[1] = gb__basic_hash(t|1);

	for (i = 0; i < 10; i++) {
		cast(void)gb_random_next(r);
	}
}

u64
gb_random_next(gbRandom *r)
{
	u64 s1 = r->seed[0];
	u64 s0 = r->seed[1];
	r->seed[0] = s0;
	s1 ^= s1 << 23;
	r->seed[1] = (s1 ^ s0 ^ (s1>>17) ^ (s0>>26) + s0);
	return r->seed[1];
}

i64
gb_random_range_i64(gbRandom *r, i64 lower_inc, i64 higher_inc)
{
	u64 u = gb_random_next(r);
	i64 i = *cast(i64 *)&u;
	i64 diff = higher_inc-lower_inc+1;
	i %= diff;
	i += lower_inc;
	return i;
}

f64
gb_random_range_f64(gbRandom *r, f64 lower_inc, f64 higher_inc)
{
	u64 u = gb_random_next(r);
	f64 f = *cast(f64 *)&u;
	f64 diff = higher_inc-lower_inc+1.0f;
	f = fmod(f, diff); /* TODO(bill): Replace fmod, maybe... */
	f += lower_inc;
	return f;
}



#if defined(GB_SYSTEM_WINDOWS)
gb_inline void gb_exit(u32 code) { ExitProcess(code); }
#else
gb_inline void gb_exit(u32 code) { exit(code); }
#endif

gb_inline void
gb_yield(void)
{
#if defined(GB_SYSTEM_WINDOWS)
	Sleep(0);
#else
	sched_yield();
#endif
}

gb_inline void
gb_set_env(char const *name, char const *value)
{
#if defined(GB_SYSTEM_WINDOWS)
	SetEnvironmentVariableA(name, value);
#else
	setenv(name, value, 1);
#endif
}

gb_inline void
gb_unset_env(char const *name)
{
#if defined(GB_SYSTEM_WINDOWS)
	SetEnvironmentVariableA(name, NULL);
#else
	unsetenv(name);
#endif
}


gb_inline u16
gb_endian_swap16(u16 i)
{
	return (i>>8) | (i<<8);
}

gb_inline u32
gb_endian_swap32(u32 i)
{
	return (i>>24) |(i<<24) |
	       ((i&0x00ff0000)>>8)  | ((i&0x0000ff00)<<8);
}

gb_inline u64
gb_endian_swap64(u64 i)
{
	/* TODO(bill): Do I really need the cast here? */
	return (i>>56) | (i<<56) |
	       ((i&cast(u64)0x00ff000000000000)>>40) | ((i&cast(u64)0x000000000000ff00)<<40) |
	       ((i&cast(u64)0x0000ff0000000000)>>24) | ((i&cast(u64)0x0000000000ff0000)<<24) |
	       ((i&cast(u64)0x000000ff00000000)>>8)  | ((i&cast(u64)0x00000000ff000000)<<8);
}







/***************************************************************
 *
 * Colour Type
 * It's quite useful
 */

#if !defined(GB_NO_COLOUR_TYPE)
gb_inline gbColour
gb_colour(f32 r, f32 g, f32 b, f32 a)
{
	gbColour result;
	result.r = cast(u8)(gb_clamp01(r) * 255.0f);
	result.g = cast(u8)(gb_clamp01(g) * 255.0f);
	result.b = cast(u8)(gb_clamp01(b) * 255.0f);
	result.a = cast(u8)(gb_clamp01(a) * 255.0f);
	return result;
}
#endif


#if defined(GB_PLATFORM)

#if defined(GB_SYSTEM_WINDOWS)


GB_XINPUT_GET_STATE(gbXInputGetState_Stub)
{
	gb_unused(dwUserIndex); gb_unused(pState);
	return ERROR_DEVICE_NOT_CONNECTED;
}

GB_XINPUT_SET_STATE(gbXInputSetState_Stub)
{
	gb_unused(dwUserIndex); gb_unused(pVibration);
	return ERROR_DEVICE_NOT_CONNECTED;
}


gb_internal gb_inline void
gb__process_xinput_digital_button(DWORD xinput_button_state, DWORD button_bit,
                                  gbControllerButton *button)
{
	b32 ended_down = ((xinput_button_state & button_bit) == button_bit);
	button->half_transition_count = (button->ended_down != ended_down) ? 1 : 0;
	button->ended_down = ended_down;
}
 
gb_internal gb_inline f32
gb__process_xinput_stick_value(SHORT value, SHORT dead_zone_threshold)
{
	f32 result = 0;

	if (value < -dead_zone_threshold)
		result = cast(f32) (value + dead_zone_threshold) / (32768.0f - dead_zone_threshold);
	else if (value > dead_zone_threshold)
		result = cast(f32) (value - dead_zone_threshold) / (32767.0f - dead_zone_threshold);

	return result;
}

gb_internal gb_inline f32
gb__process_xinput_trigger_value(BYTE value)
{
	f32 result;
	result = cast(f32) (value / 255.0f);
	return result;
}

gb_internal void
gb__window_resize_dib_section(gbWindow *window, i32 width, i32 height)
{
	if ((window->width != width) ||
	    (window->height != height)) {
		BITMAPINFO bmi = {0};
		window->width  = width;
		window->height = height;

		window->software.bytes_per_pixel = 4;
		window->software.pitch = window->software.bytes_per_pixel * width;

		bmi.bmiHeader.biSize = gb_size_of(bmi.bmiHeader);
		bmi.bmiHeader.biWidth       = width;
		bmi.bmiHeader.biHeight      = -height; /* NOTE(bill): -ve is top-down, +ve is bottom-up */
		bmi.bmiHeader.biPlanes      = 1;
		bmi.bmiHeader.biBitCount    = 8*cast(WORD)window->software.bytes_per_pixel;
		bmi.bmiHeader.biCompression = BI_RGB;

		window->software.win32_bmi = bmi;

		if (window->software.memory)
			gb_vm_free(gb_virtual_memory(window->software.memory, window->software.memory_size));

		{
			isize memory_size = window->software.pitch * height;
			gbVirtualMemory vm = gb_vm_alloc(0, memory_size);
			window->software.memory      = vm.data;
			window->software.memory_size = vm.size;
		}
	}
}


void
gb_platform_init(gbPlatform *p)
{
	gb_zero_struct(p);

	{ /* Load XInput */
		gbDllHandle xinput_library = gb_dll_load("xinput1_4.dll");
		if (!xinput_library) xinput_library = gb_dll_load("xinput9_1_0.dll");
		if (!xinput_library) xinput_library = gb_dll_load("xinput1_3.dll");
		if (!xinput_library) {
			/* TODO(bill): Diagnostic */
			gb_printf_err("XInput could not be loaded. Controllers will not work!\n");
		} else {
			p->xinput.get_state = cast(gbXInputGetStateProc *) gb_dll_proc_address(xinput_library, "XInputGetState");
			if (!p->xinput.get_state) p->xinput.get_state = gbXInputGetState_Stub;

			p->xinput.set_state = cast(gbXInputSetStateProc *) gb_dll_proc_address(xinput_library, "XInputSetState");
			if (!p->xinput.set_state) p->xinput.set_state = gbXInputSetState_Stub;
		}
	}

	{ /* Init keys */
		isize i;
		for (i = 0; i < gb_count_of(p->keys); i++)
			p->keys[i] = 0;
	}

}


void
gb_platform_update(gbPlatform *p)
{
	isize i;

	{ /* NOTE(bill): Set window state */
		RECT window_rect;
		i32 x, y, w, h;

		GetClientRect(cast(HWND)p->window.handle, &window_rect);
		x = window_rect.left;
		y = window_rect.top;
		w = window_rect.right - window_rect.left;
		h = window_rect.bottom - window_rect.top;

		if ((p->window.width != w) || (p->window.height != h)) {
			if (p->window.type == GB_WINDOW_SOFTWARE)
				gb__window_resize_dib_section(&p->window, w, h);
		}


		p->window.x = x;
		p->window.y = y;
		p->window.width = w;
		p->window.height = h;

		p->window.has_focus = (GetFocus() == cast(HWND)p->window.handle);
		p->window.is_minimized = IsIconic(cast(HWND)p->window.handle) != 0;
	}

	{ /* NOTE(bill): Set mouse pos */
		POINT mouse_pos;

		GetCursorPos(&mouse_pos);
		ScreenToClient(cast(HWND)p->window.handle, &mouse_pos);

		p->mouse.dx = mouse_pos.x - p->mouse.x;
		p->mouse.dy = mouse_pos.y - p->mouse.y;
		p->mouse.x = mouse_pos.x;
		p->mouse.y = mouse_pos.y;
	}

	{ /* NOTE(bill): Set mouse buttons */
		DWORD win_button_id[GB_MOUSE_BUTTON_COUNT] = {
			VK_LBUTTON,
			VK_MBUTTON,
			VK_RBUTTON,
			VK_XBUTTON1,
			VK_XBUTTON2,
		};
		for (i = 0; i < GB_MOUSE_BUTTON_COUNT; i++)
			p->mouse.buttons[i] = GetAsyncKeyState(win_button_id[i]) < 0;
	}

	/* NOTE(bill): Set Key states */
	if (p->window.has_focus) {
		#define GB_KEY_STATE_SET(platform, test, state) \
			if (test) p->keys[platform] |=  state; \
			else      p->keys[platform] &= ~state
		#define GB__KEY_SET(platform, vk) do { \
			b32 was_down = (p->keys[platform] & GB_KEY_STATE_DOWN) != 0; \
			b32 is_down = GetAsyncKeyState(vk) < 0; \
			GB_KEY_STATE_SET(platform, is_down,               GB_KEY_STATE_DOWN); \
			GB_KEY_STATE_SET(platform, !was_down && is_down,  GB_KEY_STATE_PRESSED); \
			GB_KEY_STATE_SET(platform,  was_down && !is_down, GB_KEY_STATE_RELEASED); \
		} while (0)
		GB__KEY_SET(GB_KEY_A, 'A');
		GB__KEY_SET(GB_KEY_B, 'B');
		GB__KEY_SET(GB_KEY_C, 'C');
		GB__KEY_SET(GB_KEY_D, 'D');
		GB__KEY_SET(GB_KEY_E, 'E');
		GB__KEY_SET(GB_KEY_F, 'F');
		GB__KEY_SET(GB_KEY_G, 'G');
		GB__KEY_SET(GB_KEY_H, 'H');
		GB__KEY_SET(GB_KEY_I, 'I');
		GB__KEY_SET(GB_KEY_J, 'J');
		GB__KEY_SET(GB_KEY_K, 'K');
		GB__KEY_SET(GB_KEY_L, 'L');
		GB__KEY_SET(GB_KEY_M, 'M');
		GB__KEY_SET(GB_KEY_N, 'N');
		GB__KEY_SET(GB_KEY_O, 'O');
		GB__KEY_SET(GB_KEY_P, 'P');
		GB__KEY_SET(GB_KEY_Q, 'Q');
		GB__KEY_SET(GB_KEY_R, 'R');
		GB__KEY_SET(GB_KEY_S, 'S');
		GB__KEY_SET(GB_KEY_T, 'T');
		GB__KEY_SET(GB_KEY_U, 'U');
		GB__KEY_SET(GB_KEY_V, 'V');
		GB__KEY_SET(GB_KEY_W, 'W');
		GB__KEY_SET(GB_KEY_X, 'X');
		GB__KEY_SET(GB_KEY_Y, 'Y');
		GB__KEY_SET(GB_KEY_Z, 'Z');

		GB__KEY_SET(GB_KEY_NUM0, '0');
		GB__KEY_SET(GB_KEY_NUM1, '1');
		GB__KEY_SET(GB_KEY_NUM2, '2');
		GB__KEY_SET(GB_KEY_NUM3, '3');
		GB__KEY_SET(GB_KEY_NUM4, '4');
		GB__KEY_SET(GB_KEY_NUM5, '5');
		GB__KEY_SET(GB_KEY_NUM6, '6');
		GB__KEY_SET(GB_KEY_NUM7, '7');
		GB__KEY_SET(GB_KEY_NUM8, '8');
		GB__KEY_SET(GB_KEY_NUM9, '9');

		GB__KEY_SET(GB_KEY_ESCAPE, VK_ESCAPE);

		GB__KEY_SET(GB_KEY_LCONTROL, VK_LCONTROL);
		GB__KEY_SET(GB_KEY_LSHIFT,   VK_LSHIFT);
		GB__KEY_SET(GB_KEY_LALT,     VK_LMENU);
		GB__KEY_SET(GB_KEY_LSYSTEM,  VK_LWIN);
		GB__KEY_SET(GB_KEY_RCONTROL, VK_RCONTROL);
		GB__KEY_SET(GB_KEY_RSHIFT,   VK_RSHIFT);
		GB__KEY_SET(GB_KEY_RALT,     VK_RMENU);
		GB__KEY_SET(GB_KEY_RSYSTEM,  VK_RWIN);
		GB__KEY_SET(GB_KEY_MENU,     VK_MENU);

		GB__KEY_SET(GB_KEY_LBRACKET,  VK_OEM_4);
		GB__KEY_SET(GB_KEY_RBRACKET,  VK_OEM_6);
		GB__KEY_SET(GB_KEY_SEMICOLON, VK_OEM_1);
		GB__KEY_SET(GB_KEY_COMMA,     VK_OEM_COMMA);
		GB__KEY_SET(GB_KEY_PERIOD,    VK_OEM_PERIOD);
		GB__KEY_SET(GB_KEY_QUOTE,     VK_OEM_7);
		GB__KEY_SET(GB_KEY_SLASH,     VK_OEM_2);
		GB__KEY_SET(GB_KEY_BACKSLASH, VK_OEM_5);
		GB__KEY_SET(GB_KEY_GRAVE,     VK_OEM_3);
		GB__KEY_SET(GB_KEY_EQUALS,    VK_OEM_PLUS);
		GB__KEY_SET(GB_KEY_MINUS,     VK_OEM_MINUS);

		GB__KEY_SET(GB_KEY_SPACE,     VK_SPACE);
		GB__KEY_SET(GB_KEY_RETURN,    VK_RETURN);
		GB__KEY_SET(GB_KEY_BACKSPACE, VK_BACK);
		GB__KEY_SET(GB_KEY_TAB,       VK_TAB);

		GB__KEY_SET(GB_KEY_PAGEUP,   VK_PRIOR);
		GB__KEY_SET(GB_KEY_PAGEDOWN, VK_NEXT);
		GB__KEY_SET(GB_KEY_END,      VK_END);
		GB__KEY_SET(GB_KEY_HOME,     VK_HOME);
		GB__KEY_SET(GB_KEY_INSERT,   VK_INSERT);
		GB__KEY_SET(GB_KEY_DELETE,   VK_DELETE);

		GB__KEY_SET(GB_KEY_PLUS,     VK_ADD);
		GB__KEY_SET(GB_KEY_SUBTRACT, VK_SUBTRACT);
		GB__KEY_SET(GB_KEY_MULTIPLY, VK_MULTIPLY);
		GB__KEY_SET(GB_KEY_DIVIDE,   VK_DIVIDE);

		GB__KEY_SET(GB_KEY_LEFT,  VK_LEFT);
		GB__KEY_SET(GB_KEY_RIGHT, VK_RIGHT);
		GB__KEY_SET(GB_KEY_UP,    VK_UP);
		GB__KEY_SET(GB_KEY_DOWN,  VK_DOWN);

		GB__KEY_SET(GB_KEY_NUMPAD0, VK_NUMPAD0);
		GB__KEY_SET(GB_KEY_NUMPAD1, VK_NUMPAD1);
		GB__KEY_SET(GB_KEY_NUMPAD2, VK_NUMPAD2);
		GB__KEY_SET(GB_KEY_NUMPAD3, VK_NUMPAD3);
		GB__KEY_SET(GB_KEY_NUMPAD4, VK_NUMPAD4);
		GB__KEY_SET(GB_KEY_NUMPAD5, VK_NUMPAD5);
		GB__KEY_SET(GB_KEY_NUMPAD6, VK_NUMPAD6);
		GB__KEY_SET(GB_KEY_NUMPAD7, VK_NUMPAD7);
		GB__KEY_SET(GB_KEY_NUMPAD8, VK_NUMPAD8);
		GB__KEY_SET(GB_KEY_NUMPAD9, VK_NUMPAD9);

		GB__KEY_SET(GB_KEY_F1,  VK_F1);
		GB__KEY_SET(GB_KEY_F2,  VK_F2);
		GB__KEY_SET(GB_KEY_F3,  VK_F3);
		GB__KEY_SET(GB_KEY_F4,  VK_F4);
		GB__KEY_SET(GB_KEY_F5,  VK_F5);
		GB__KEY_SET(GB_KEY_F6,  VK_F6);
		GB__KEY_SET(GB_KEY_F7,  VK_F7);
		GB__KEY_SET(GB_KEY_F8,  VK_F8);
		GB__KEY_SET(GB_KEY_F9,  VK_F9);
		GB__KEY_SET(GB_KEY_F10, VK_F10);
		GB__KEY_SET(GB_KEY_F11, VK_F11);
		GB__KEY_SET(GB_KEY_F12, VK_F12);
		GB__KEY_SET(GB_KEY_F13, VK_F13);
		GB__KEY_SET(GB_KEY_F14, VK_F14);
		GB__KEY_SET(GB_KEY_F15, VK_F15);

		GB__KEY_SET(GB_KEY_PAUSE, VK_PAUSE);
		#undef GB__KEY_SET
		#undef GB_KEY_STATE_SET

		p->key_modifiers.control = p->keys[GB_KEY_LCONTROL] | p->keys[GB_KEY_RCONTROL];
		p->key_modifiers.alt     = p->keys[GB_KEY_LALT]     | p->keys[GB_KEY_RALT];
		p->key_modifiers.shift   = p->keys[GB_KEY_LSHIFT]   | p->keys[GB_KEY_RSHIFT];
	}

	{ /* NOTE(bill): Set Controller states */
		DWORD max_controller_count = XUSER_MAX_COUNT;
		if (max_controller_count > gb_count_of(p->game_controllers))
			max_controller_count = gb_count_of(p->game_controllers);

		for (i = 0;
		     i < max_controller_count;
		     i++) {
			gbGameController *controller = &p->game_controllers[i];

			XINPUT_STATE controller_state = {0};
			if (p->xinput.get_state(cast(DWORD)i, &controller_state) != ERROR_SUCCESS) {
				/* NOTE(bill): The controller is not available */
				controller->is_connected = false;
			} else {
				/* NOTE(bill): This controller is plugged in */
				/* TODO(bill): See if ControllerState.dwPacketNumber increments too rapidly */
				XINPUT_GAMEPAD *pad = &controller_state.Gamepad;

				controller->is_connected = true;

				/* TODO(bill): This is a square deadzone, check XInput to
				 * verify that the deadzone is "round" and show how to do
				 * round deadzone processing.
				 */
				controller->axes[GB_CONTROLLER_AXIS_LEFT_X]  = gb__process_xinput_stick_value(pad->sThumbLX, XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE);
				controller->axes[GB_CONTROLLER_AXIS_LEFT_Y]  = gb__process_xinput_stick_value(pad->sThumbLY, XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE);
				controller->axes[GB_CONTROLLER_AXIS_RIGHT_Y] = gb__process_xinput_stick_value(pad->sThumbRX, XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE);
				controller->axes[GB_CONTROLLER_AXIS_RIGHT_Y] = gb__process_xinput_stick_value(pad->sThumbRY, XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE);

				controller->axes[GB_CONTROLLER_AXIS_LEFT_TRIGGER]  = gb__process_xinput_trigger_value(pad->bLeftTrigger);
				controller->axes[GB_CONTROLLER_AXIS_RIGHT_TRIGGER] = gb__process_xinput_trigger_value(pad->bRightTrigger);


				if ((controller->axes[GB_CONTROLLER_AXIS_LEFT_X] != 0.0f) ||
					(controller->axes[GB_CONTROLLER_AXIS_LEFT_Y] != 0.0f)) {
					controller->is_analog = true;
				}

				/* NOTE(bill): I know, I just wanted macros */
			#define GB__PROCESS_PAD_BUTTON(stick_axis, sign, xinput_button) do { \
					if (pad->wButtons & xinput_button) { \
						controller->axes[stick_axis] = sign 1.0f; \
						controller->is_analog  = false; \
					} \
				} while (0)

				GB__PROCESS_PAD_BUTTON(GB_CONTROLLER_AXIS_LEFT_X, -, XINPUT_GAMEPAD_DPAD_LEFT);
				GB__PROCESS_PAD_BUTTON(GB_CONTROLLER_AXIS_LEFT_X, +, XINPUT_GAMEPAD_DPAD_RIGHT);
				GB__PROCESS_PAD_BUTTON(GB_CONTROLLER_AXIS_LEFT_Y, -, XINPUT_GAMEPAD_DPAD_DOWN);
				GB__PROCESS_PAD_BUTTON(GB_CONTROLLER_AXIS_LEFT_Y, +, XINPUT_GAMEPAD_DPAD_UP);
			#undef GB__PROCESS_PAD_BUTTON

			#define GB__PROCESS_DIGITAL_AXIS(stick_axis, sign, button_type) \
				gb__process_xinput_digital_button((controller->axes[stick_axis] < sign 0.5f) ? 1 : 0, 1, &controller->buttons[button_type])
				GB__PROCESS_DIGITAL_AXIS(GB_CONTROLLER_AXIS_LEFT_X, -, GB_CONTROLLER_BUTTON_LEFT);
				GB__PROCESS_DIGITAL_AXIS(GB_CONTROLLER_AXIS_LEFT_X, +, GB_CONTROLLER_BUTTON_RIGHT);
				GB__PROCESS_DIGITAL_AXIS(GB_CONTROLLER_AXIS_LEFT_Y, -, GB_CONTROLLER_BUTTON_DOWN);
				GB__PROCESS_DIGITAL_AXIS(GB_CONTROLLER_AXIS_LEFT_Y, +, GB_CONTROLLER_BUTTON_UP);
			#undef GB__PROCESS_DIGITAL_AXIS

			#define GB__PROCESS_DIGITAL_BUTTON(button_type, xinput_button) \
				gb__process_xinput_digital_button(pad->wButtons, xinput_button, &controller->buttons[button_type])
				GB__PROCESS_DIGITAL_BUTTON(GB_CONTROLLER_BUTTON_A, XINPUT_GAMEPAD_A);
				GB__PROCESS_DIGITAL_BUTTON(GB_CONTROLLER_BUTTON_B, XINPUT_GAMEPAD_B);
				GB__PROCESS_DIGITAL_BUTTON(GB_CONTROLLER_BUTTON_X, XINPUT_GAMEPAD_X);
				GB__PROCESS_DIGITAL_BUTTON(GB_CONTROLLER_BUTTON_Y, XINPUT_GAMEPAD_Y);
				GB__PROCESS_DIGITAL_BUTTON(GB_CONTROLLER_BUTTON_LEFT_SHOULDER, XINPUT_GAMEPAD_LEFT_SHOULDER);
				GB__PROCESS_DIGITAL_BUTTON(GB_CONTROLLER_BUTTON_RIGHT_SHOULDER, XINPUT_GAMEPAD_RIGHT_SHOULDER);
				GB__PROCESS_DIGITAL_BUTTON(GB_CONTROLLER_BUTTON_START, XINPUT_GAMEPAD_START);
				GB__PROCESS_DIGITAL_BUTTON(GB_CONTROLLER_BUTTON_BACK, XINPUT_GAMEPAD_BACK);
			#undef GB__PROCESS_DIGITAL_BUTTON
			}
		}
	}

	{ /* NOTE(bill): Process pending messages */
		MSG message;
		for (;;) {
			BOOL is_okay = PeekMessage(&message, 0, 0, 0, PM_REMOVE);
			if (!is_okay) break;

			switch (message.message) {
			case WM_QUIT:
				p->quit_requested = true;
				break;

			default:
				TranslateMessage(&message);
				DispatchMessageW(&message);
				break;
		}
		}
	}
}

void
gb_platform_display(gbPlatform *p)
{
	gbWindow *window;
	GB_ASSERT_NOT_NULL(p);

	window = &p->window;

	switch (window->type) {
	case GB_WINDOW_OPENGL: {
		SwapBuffers(window->win32_dc);
	} break;
	case GB_WINDOW_SOFTWARE: {
		StretchDIBits(window->win32_dc,
		              0, 0, window->width, window->height,
		              0, 0, window->width, window->height,
		              window->software.memory,
		              &window->software.win32_bmi,
		              DIB_RGB_COLORS, SRCCOPY);
	} break;
	default: GB_PANIC("Invalid window type"); break;
	}

	{
		f64 prev_time = p->curr_time;
		f64 curr_time = gb_time_now();
		p->dt_for_frame = curr_time - prev_time;
		p->curr_time = curr_time;
	}
}



void
gb_platform_show_cursor(gbPlatform *p, i32 show)
{
	gb_unused(p);
	ShowCursor(show);
}

void
gb_platform_set_mouse_position(gbPlatform *p, gbWindow *rel_win, i32 x, i32 y)
{
	POINT point;
	point.x = cast(LONG)x;
	point.y = cast(LONG)y;
	ClientToScreen(cast(HWND)rel_win->handle, &point);
	SetCursorPos(point.x, point.y);

	p->mouse.dx = point.x - p->mouse.x;
	p->mouse.dy = point.y - p->mouse.y;
	p->mouse.x  = point.x;
	p->mouse.y  = point.y;
}


gb_inline gbGameController *
gb_platform_get_controller(gbPlatform *p, isize index)
{
	if (index >= 0 && index < gb_count_of(p->game_controllers))
		return p->game_controllers + index;
	return NULL;
}

LRESULT CALLBACK
gb__win32_main_window_callback(HWND wnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	LRESULT result = 0;
	gbWindow *window = cast(gbWindow *)GetWindowLongPtr(wnd, GWLP_USERDATA);

	switch (msg) {
	case WM_CLOSE:
	case WM_DESTROY:
		window->is_closed = true;
		break;

	default:
		result = DefWindowProcW(wnd, msg, wparam, lparam);
		break;
	}

	return result;
}


/* TODO(bill): Make this return errors rathern than silly message boxes */
gbWindow *
gb_window_init(gbPlatform *p, char const *title, i32 w, i32 h, gbWindowType type, b32 is_fullscreen)
{
	gbWindow *window = NULL;
	WNDCLASSEXW wc = {gb_size_of(WNDCLASSEXW)};
	DWORD ex_style, style;
	RECT wr;
	char16 title_buffer[256] = {0}; /* TODO(bill): gb_local_persist this? */

	window = &p->window;
	gb_zero_struct(window);

	wc.style = CS_HREDRAW | CS_VREDRAW; /* | CS_OWNDC */
	wc.lpfnWndProc   = gb__win32_main_window_callback;
	wc.hIcon         = LoadIcon(NULL, IDI_WINLOGO);
	wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = cast(HBRUSH)GetStockObject(WHITE_BRUSH);
	wc.lpszMenuName  = NULL;
	wc.lpszClassName = L"gb-win32-wndclass"; /* TODO(bill): Is this enough? */
	wc.hInstance     = GetModuleHandle(NULL);
	wc.hIconSm       = LoadIcon(NULL, IDI_WINLOGO);

	if (RegisterClassExW(&wc) == 0) {
		MessageBoxW(NULL, L"Failed to register the window class", L"ERROR", MB_OK | MB_ICONEXCLAMATION);
		return NULL;
	}

	if (is_fullscreen) {
		DEVMODEW screen_settings = {gb_size_of(DEVMODEW)};
		screen_settings.dmPelsWidth	 = w;
		screen_settings.dmPelsHeight = h;
		screen_settings.dmBitsPerPel = 32;
		screen_settings.dmFields     = DM_BITSPERPEL|DM_PELSWIDTH|DM_PELSHEIGHT;

		if (ChangeDisplaySettingsW(&screen_settings, CDS_FULLSCREEN) != DISP_CHANGE_SUCCESSFUL) {
			if (MessageBoxW(NULL, L"The requested fullscreen mode is not supported by\n"
			                L"your video card. Use windowed mode instead?",
			                L"",
			                MB_YESNO|MB_ICONEXCLAMATION) == IDYES) {
				is_fullscreen = false;
			} else {
				MessageBoxW(NULL, L"Failed to create a window", L"ERROR", MB_OK|MB_ICONSTOP);
				return NULL;
			}
		}
	}

	ex_style = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;
	style = WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_VISIBLE | WS_OVERLAPPEDWINDOW;
	wr.left = 0;
	wr.top = 0;
	wr.right = w;
	wr.bottom = h;
	AdjustWindowRect(&wr, style, false);

	window->is_fullscreen = is_fullscreen;
	window->type = type;
	window->handle = CreateWindowExW(ex_style,
	                                 wc.lpszClassName,
	                                 cast(LPCWSTR)gb_utf8_to_ucs2(title_buffer, gb_size_of(title_buffer), title),
	                                 style,
	                                 CW_USEDEFAULT, CW_USEDEFAULT,
	                                 wr.right - wr.left, wr.bottom - wr.top,
	                                 0, 0,
	                                 GetModuleHandle(NULL),
	                                 cast(HWND)NULL);

	if (!window->handle) {
		MessageBoxW(NULL, L"Window creation failed", L"Error", MB_OK|MB_ICONEXCLAMATION);
		return NULL;
	}

	window->win32_dc = GetDC(cast(HWND)window->handle);

	switch (window->type) {
	case GB_WINDOW_OPENGL: {
		PIXELFORMATDESCRIPTOR pfd = {gb_size_of(PIXELFORMATDESCRIPTOR)};
		pfd.nVersion     = 1;
		pfd.dwFlags      = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL;
		pfd.iPixelType   = PFD_TYPE_RGBA;
		pfd.cColorBits   = 32;
		pfd.cAlphaBits   = 8;
		pfd.cDepthBits   = 24;
		pfd.cStencilBits = 8;
		pfd.iLayerType   = PFD_MAIN_PLANE;

		SetPixelFormat(window->win32_dc, ChoosePixelFormat(window->win32_dc, &pfd), NULL);

		window->opengl.win32_context = wglCreateContext(window->win32_dc);
		wglMakeCurrent(window->win32_dc, window->opengl.win32_context);
	} break;

	case GB_WINDOW_SOFTWARE: {
		gb__window_resize_dib_section(window, w, h);
	} break;

	default: GB_PANIC("Unknown window type"); break;
	}


	SetForegroundWindow(cast(HWND)window->handle);
	SetFocus(cast(HWND)window->handle);
	SetWindowLongPtr(cast(HWND)window->handle, GWLP_USERDATA, cast(LONG_PTR)window);
	return window;
}

void
gb_window_set_position(gbWindow *w, i32 x, i32 y)
{
	RECT rect;
	i32 width, height;

	GetClientRect(cast(HWND)w->handle, &rect);
	width  = rect.right - rect.left;
	height = rect.bottom - rect.top;
	MoveWindow(cast(HWND)w->handle, x, y, width, height, false);
}

void
gb_window_set_title(gbWindow *w, char const *title, ...)
{
	char16 buffer[256] = {0};
	char *str;
	LPCWSTR wstr;
	va_list va;
	va_start(va, title);
	str = gb_sprintf_va(title, va);
	va_end(va);

	wstr = cast(LPCWSTR)gb_utf8_to_ucs2(buffer, gb_size_of(buffer), str);
	if (wstr)
		SetWindowTextW(cast(HWND)w->handle, wstr);
}

void
gb_window_toggle_fullscreen(gbWindow *w)
{
	HWND handle = cast(HWND)w->handle;
	DWORD style = GetWindowLong(handle, GWL_STYLE);
	if (style & WS_OVERLAPPEDWINDOW) {
		MONITORINFO monitor_info = {gb_size_of(monitor_info)};
		if (GetWindowPlacement(handle, &w->win32_placement) &&
		    GetMonitorInfo(MonitorFromWindow(handle, 1), &monitor_info)) {
			SetWindowLong(handle, GWL_STYLE, style & ~WS_OVERLAPPEDWINDOW);
			SetWindowPos(handle, HWND_TOP,
			             monitor_info.rcMonitor.left, monitor_info.rcMonitor.top,
			             monitor_info.rcMonitor.right - monitor_info.rcMonitor.left,
			             monitor_info.rcMonitor.bottom - monitor_info.rcMonitor.top,
			             SWP_NOOWNERZORDER | SWP_FRAMECHANGED);

			w->is_fullscreen = true;
		}
	} else {
		SetWindowLong(handle, GWL_STYLE, style | WS_OVERLAPPEDWINDOW);
		SetWindowPlacement(handle, &w->win32_placement);
		SetWindowPos(handle, 0, 0, 0, 0, 0,
		             SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER |
		             SWP_NOOWNERZORDER | SWP_FRAMECHANGED);

		w->is_fullscreen = false;
	}
}

void
gb_window_make_context_current(gbWindow *w)
{
	if (w->type == GB_WINDOW_OPENGL) {
		wglMakeCurrent(w->win32_dc, w->opengl.win32_context);
	}
}



#endif

#endif /* GB_PLATFORM */






















#if defined(_MSC_VER)
#pragma warning(pop)
#endif

#if defined(__GCC__) || defined(__GNUC__)
#pragma GCC diagnostic pop
#endif


#if defined(__cplusplus)
}
#endif

#endif /* GB_IMPLEMENTATION */
