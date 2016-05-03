/* gb.h - v0.07a - Ginger Bill's C Helper Library - public domain
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
===========================================================================

Conventions used:
	gbTypesAreLikeThis (None core types)
	gb_functions_and_variables_like_this
	Prefer C90 Comments
	Never use _t suffix for types (I think they are stupid...)


Version History:
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

#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>    /* TODO(bill): Remove and replace with OS Specific stuff */
#include <stdlib.h>
#include <string.h>   /* NOTE(bill): For memcpy, memmove, memcmp, etc. */
#include <sys/stat.h> /* NOTE(bill): File info */

#if defined(GB_SYSTEM_WINDOWS)
	#define NOMINMAX            1
	#define WIN32_LEAN_AND_MEAN 1
	#define WIN32_MEAN_AND_LEAN 1
	#define VC_EXTRALEAN        1
	#include <windows.h>
	#include <direct.h>
	#include <process.h>
	#include <malloc.h>
#else
	#include <dlfcn.h>
	#include <mach/mach_time.h>
	#include <pthread.h>
	#include <sys/stat.h>
	#include <time.h>
	#include <unistd.h>
#endif


#ifndef gb_malloc
#define gb_malloc(sz) malloc(sz)
#endif

#ifndef gb_mfree
#define gb_mfree(ptr) free(ptr)
#endif


/***************************************************************
 *
 * Base Types
 *
 */

#if defined(_MSC_VER)
	typedef unsigned __int8   u8;
	typedef   signed __int8   i8;
	typedef unsigned __int16 u16;
	typedef   signed __int16 i16;
	typedef unsigned __int32 u32;
	typedef   signed __int32 i32;
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

typedef uintptr_t uintptr;
typedef  intptr_t  intptr;

typedef float  f32;
typedef double f64;

GB_STATIC_ASSERT(sizeof(f32) == 4);
GB_STATIC_ASSERT(sizeof(f64) == 8);

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
		#ifndef false
		#define false 0
		#endif
		#ifndef true
		#define true 1
		#endif
	#else
		#include <stdbool.h>
	#endif
#endif

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
	#error Unknown architecture size
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


#if !defined(gb_inline)
	#if defined(_MSC_VER)
		#define gb_restrict __restrict
	#else
		#define gb_restrict restrict
	#endif
#endif

/* TODO(bill): Should force inline be a separate keyword and gb_inline be inline? */
#if !defined(gb_inline)
	#if defined(_MSC_VER)
		#define gb_inline __forceinline
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

/* NOTE(bill): I do which I had a type_of that was portable */
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
#if defined(__cplusplus)
extern "C++" {
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
}

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
	#define GB_JOIN3_IND(a, b, c) a##b##c

	#define GB_JOIN2(a, b)    GB_JOIN2_IND(a, b)
	#define GB_JOIN3(a, b, c) GB_JOIN3_IND(a, b, c)
#endif


#ifndef GB_BIT
#define GB_BIT(x) (1<<x)
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


/***************************************************************
 *
 * Debug
 *
 */


#ifndef GB_DEBUG_TRAP
	#if defined(_MSC_VER)
	 	#if _MSC_VER < 1300
		#define GB_DEBUG_TRAP() __asm int 3; // Trap to debugger!
		#else
		#define GB_DEBUG_TRAP() __debugbreak()
		#endif
	#else
		#define GB_DEBUG_TRAP() __builtin_trap()
	#endif
#endif

/* TODO(bill): This relies upon variadic macros which are not supported in MSVC 2003 and below, check for it if needed */
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

/* NOTE(bill): Things that shouldn't happen */
#ifndef GB_PANIC
#define GB_PANIC(msg) GB_ASSERT_MSG(0, msg)
#endif

GB_DEF void gb_assert_handler(char const *condition, char const *file, i32 line, char const *msg);





/***************************************************************
 *
 * Printing
 *
 */

/* NOTE(bill): Some compilers support applying printf-style warnings to user functions. */
#if defined(__clang__) || defined(__GNUC__)
#define GB_PRINTF_ARGS(FMT) __attribute__((format(printf, FMT, (FMT+1))))
#else
#define GB_PRINTF_ARGS(FMT)
#endif

/* TODO(bill): Should I completely rename these functions as they are a little weird to begin with? */

GB_DEF i32   gb_printf     (char const *fmt, ...) GB_PRINTF_ARGS(1);
GB_DEF i32   gb_printf_va  (char const *fmt, va_list va);
GB_DEF i32   gb_fprintf    (FILE *f, char const *fmt, ...) GB_PRINTF_ARGS(2);
GB_DEF i32   gb_fprintf_va (FILE *f, char const *fmt, va_list va);
GB_DEF char *gb_sprintf    (char const *fmt, ...) GB_PRINTF_ARGS(1); /* NOTE(bill): A locally persisting buffer is used internally */
GB_DEF char *gb_sprintf_va (char const *fmt, va_list va);            /* NOTE(bill): A locally persisting buffer is used internally */
GB_DEF i32   gb_snprintf   (char *str, isize n, char const *fmt, ...) GB_PRINTF_ARGS(3);
GB_DEF i32   gb_snprintf_va(char *str, isize n, char const *fmt, va_list va);

GB_DEF i32 gb_println (char const *str);
GB_DEF i32 gb_fprintln(FILE *f, char const *str);



/***************************************************************
 *
 * Memory
 *
 */

#ifndef gb_align_to
#define gb_align_to(value, alignment) (((value) + ((alignment)-1)) & ~((alignment) - 1))
#endif

#ifndef gb_is_power_of_two
#define gb_is_power_of_two(x) ((x) != 0) && !((x) & ((x)-1))
#endif

GB_DEF void *gb_align_forward(void *ptr, isize alignment);
GB_DEF void *gb_pointer_add  (void *ptr, isize bytes);
GB_DEF void *gb_pointer_sub  (void *ptr, isize bytes);

GB_DEF void gb_zero_size(void *ptr, isize size);

#ifndef gb_zero_struct
#define gb_zero_struct(t) gb_zero_size((t), gb_size_of(*(t))) /* NOTE(bill): Pass pointer of struct */
#define gb_zero_array(a, count) gb_zero_size((a), gb_size_of((a)[0])*count)
#endif

GB_DEF void *gb_memcopy(void *dest, void const *source, isize size);
GB_DEF void *gb_memmove(void *dest, void const *source, isize size);
GB_DEF void *gb_memset (void *data, u8 byte_value, isize size);


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

GB_DEF i32  gb_load_atomic32                   (gbAtomic32 const volatile *a);
GB_DEF void gb_store_atomic32                  (gbAtomic32 volatile *a, i32 value);
GB_DEF i32  gb_compare_exchange_strong_atomic32(gbAtomic32 volatile *a, i32 expected, i32 desired);
GB_DEF i32  gb_exchanged_atomic32              (gbAtomic32 volatile *a, i32 desired);
GB_DEF i32  gb_fetch_add_atomic32              (gbAtomic32 volatile *a, i32 operand);
GB_DEF i32  gb_fetch_and_atomic32              (gbAtomic32 volatile *a, i32 operand);
GB_DEF i32  gb_fetch_or_atomic32               (gbAtomic32 volatile *a, i32 operand);

GB_DEF i64  gb_load_atomic64                   (gbAtomic64 const volatile *a);
GB_DEF void gb_store_atomic64                  (gbAtomic64 volatile *a, i64 value);
GB_DEF i64  gb_compare_exchange_strong_atomic64(gbAtomic64 volatile *a, i64 expected, i64 desired);
GB_DEF i64  gb_exchanged_atomic64              (gbAtomic64 volatile *a, i64 desired);
GB_DEF i64  gb_fetch_add_atomic64              (gbAtomic64 volatile *a, i64 operand);
GB_DEF i64  gb_fetch_and_atomic64              (gbAtomic64 volatile *a, i64 operand);
GB_DEF i64  gb_fetch_or_atomic64               (gbAtomic64 volatile *a, i64 operand);


typedef struct gbMutex {
#if defined(GB_SYSTEM_WINDOWS)
	CRITICAL_SECTION win32_critical_section;
#else
	pthread_mutex_t posix_handle;
#endif
} gbMutex;

GB_DEF void gb_init_mutex    (gbMutex *m);
GB_DEF void gb_destroy_mutex (gbMutex *m);
GB_DEF void gb_lock_mutex    (gbMutex *m);
GB_DEF b32  gb_try_lock_mutex(gbMutex *m);
GB_DEF void gb_unlock_mutex  (gbMutex *m);

/* NOTE(bill): If you wanted a Scoped Mutex in C++, why not use the defer() construct?
 * No need for a silly wrapper class
 */
#if 0
gbMutex m = {0};
gb_init_mutex(&m);
{
	gb_lock_mutex(&m);
	defer (gb_unlock_mutex(&m));

	/* Do whatever as the mutex is now scoped based! */
}
#endif

/* TODO(bill): Should I create a Condition Type? */


typedef struct gbSemaphore {
#if defined(GB_SYSTEM_WINDOWS)
	void *win32_handle;
#else
	gbMutex        mutex;
	pthread_cond_t cond;
	i32            count;
#endif
} gbSemaphore;

GB_DEF void gb_init_semaphore   (gbSemaphore *s);
GB_DEF void gb_destroy_semaphore(gbSemaphore *s);
GB_DEF void gb_post_semaphore   (gbSemaphore *s, i32 count);
GB_DEF void gb_wait_semaphore   (gbSemaphore *s);




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

GB_DEF void gb_init_thread            (gbThread *t);
GB_DEF void gb_destory_thread         (gbThread *t);
GB_DEF void gb_start_thread           (gbThread *t, gbThreadProc *proc, void *data);
GB_DEF void gb_start_thread_with_stack(gbThread *t, gbThreadProc *proc, void *data, isize stack_size);
GB_DEF void gb_join_thread            (gbThread *t);
GB_DEF b32  gb_is_thread_running      (gbThread const *t);
GB_DEF u32  gb_current_thread_id      (void);
GB_DEF void gb_set_thread_name        (gbThread *t, char const *name);

/***************************************************************
 *
 * Virtual Memory
 *
 */

#if 0
typedef enum gbVmAllocationFlag {
	GB_VM_COMMIT     = GB_BIT(1),
	GB_VM_RESERVE    = GB_BIT(2),
	GB_VM_RESET      = GB_BIT(3),
	GB_VM_RESET_UNDO = GB_BIT(4),
} gbVmAllocationFlag;

typedef enum gbVmProtectionFlag {
	GB_VM_PAGE_NO_ACCESS     = GB_BIT(1),
	GB_VM_PAGE_GUARD         = GB_BIT(2),
	GB_VM_PAGE_NO_CACHE      = GB_BIT(3),
	GB_VM_PAGE_WRITE_COMBINE = GB_BIT(4),
} gbVmProtectionFlag;

typedef enum gbVmFreeType {
	GB_VM_RELEASE  = GB_BIT(1),
	GB_VM_DECOMMIT = GB_BIT(2),
} gbVmFreeType;

typedef struct gbVirtualMemory {
	void *memory;
	isize size;
	u32 allocation_flags;
	u32 protection_flags;
} gbVirtualMemory;

GB_DEF gbVirtualMemory gb_vm_alloc(void *base_address, isize size, u32 allocation_flags, u32 protection_flags);
GB_DEF void gb_vm_free(gbVirtualMemory *vm);
#endif


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

GB_DEF void *gb_alloc_copy      (gbAllocator a, void const *src, isize size);
GB_DEF void *gb_alloc_copy_align(gbAllocator a, void const *src, isize size, isize alignment);

GB_DEF char *gb_alloc_cstring(gbAllocator a, char const *str);


/* NOTE(bill): These are very useful and the type cast has saved me from numerous bugs */
#ifndef gb_alloc_struct
#define gb_alloc_struct(allocator, Type)       (Type *)gb_alloc_align(allocator, gb_size_of(Type))
#define gb_alloc_array(allocator, Type, count) (Type *)gb_alloc(allocator, gb_size_of(Type) * (count))
#endif

/* NOTE(bill): Use this if you need a fancy resize allocaiton */
GB_DEF void *gb_default_resize_align(gbAllocator a, void *ptr, isize old_size, isize new_size, isize alignment);





GB_DEF gbAllocator gb_heap_allocator(void);
GB_DEF GB_ALLOCATOR_PROC(gb_heap_allocator_proc);





typedef struct gbArena {
	gbAllocator backing;
	void *physical_start;
	isize total_size;
	isize total_allocated;
	u32 temp_count;
} gbArena;

GB_DEF void gb_init_arena_from_memory   (gbArena *arena, void *start, isize size);
GB_DEF void gb_init_arena_from_allocator(gbArena *arena, gbAllocator backing, isize size);
GB_DEF void gb_init_subarena            (gbArena *arena, gbArena *parent_arena, isize size);
GB_DEF void gb_free_arena               (gbArena *arena);

GB_DEF isize gb_arena_alignment_of  (gbArena *arena, isize alignment);
GB_DEF isize gb_arena_size_remaining(gbArena *arena, isize alignment);
GB_DEF void  gb_check_arena         (gbArena *arena);


GB_DEF gbAllocator gb_arena_allocator(gbArena *arena);
GB_DEF GB_ALLOCATOR_PROC(gb_arena_allocator_proc);



typedef struct gbTempArenaMemory {
	gbArena *arena;
	isize original_count;
} gbTempArenaMemory;

GB_DEF gbTempArenaMemory gb_begin_temp_arena_memory(gbArena *arena);
GB_DEF void              gb_end_temp_arena_memory  (gbTempArenaMemory tmp_mem);









typedef struct gbPool {
	gbAllocator backing;

	void *physical_start;
	void *free_list;

	isize block_size;
	isize block_align;
	isize total_size;
} gbPool;

GB_DEF void gb_init_pool      (gbPool *pool, gbAllocator backing, isize num_blocks, isize block_size);
GB_DEF void gb_init_pool_align(gbPool *pool, gbAllocator backing, isize num_blocks, isize block_size, isize block_align);
GB_DEF void gb_free_pool      (gbPool *pool);


GB_DEF gbAllocator gb_pool_allocator(gbPool *pool);
GB_DEF GB_ALLOCATOR_PROC(gb_pool_allocator_proc);



/***************************************************************
 *
 * Sort & Search
 *
 */

#define GB_COMPARE_PROC(name) int name(void const *a, void const *b)
typedef GB_COMPARE_PROC(gbCompareProc);


GB_DEF void gb_qsort(void *base, isize count, isize size, gbCompareProc compare_proc);

/* NOTE(bill): the count of temp == count of items */
GB_DEF void gb_radix_sort_u8 (u8  *gb_restrict items, u8  *gb_restrict temp, isize count);
GB_DEF void gb_radix_sort_u16(u16 *gb_restrict items, u16 *gb_restrict temp, isize count);
GB_DEF void gb_radix_sort_u32(u32 *gb_restrict items, u32 *gb_restrict temp, isize count);
GB_DEF void gb_radix_sort_u64(u64 *gb_restrict items, u64 *gb_restrict temp, isize count);




/* NOTE(bill): Returns index or -1 if not found */
GB_DEF isize gb_binary_search(void const *base, isize count, isize size, void const *key, gbCompareProc compare_proc);



/***************************************************************
 *
 * Char Functions
 *
 */

GB_DEF char gb_char_to_lower       (char c);
GB_DEF char gb_char_to_upper       (char c);
GB_DEF b32  gb_is_char_space       (char c);
GB_DEF b32  gb_is_char_digit       (char c);
GB_DEF b32  gb_is_char_hex_digit   (char c);
GB_DEF b32  gb_is_char_alpha       (char c);
GB_DEF b32  gb_is_char_alphanumeric(char c);
GB_DEF i32  gb_digit_to_int        (char c);
GB_DEF i32  gb_hex_digit_to_int    (char c);



GB_DEF void gb_to_lower(char *str);
GB_DEF void gb_to_upper(char *str);

GB_DEF isize gb_strlen (char const *str);
GB_DEF isize gb_strnlen(char const *str, isize max_len);
GB_DEF i32   gb_strcmp (char const *s1, char const *s2);
GB_DEF char *gb_strncpy(char *dest, char const *source, isize len);
GB_DEF i32   gb_strncmp(char const *s1, char const *s2, isize len);

GB_DEF char const *gb_strtok(char *output, char const *src, char const *delimit);

GB_DEF b32 gb_cstr_has_prefix(char const *str, char const *prefix);
GB_DEF b32 gb_cstr_has_suffix(char const *str, char const *suffix);



GB_DEF char const *gb_first_occurence_of_char(char const *s1, char c);
GB_DEF char const *gb_last_occurence_of_char (char const *s1, char c);

GB_DEF void gb_cstr_concat(char *dest, isize dest_len,
                           char const *src_a, isize src_a_len,
                           char const *src_b, isize src_b_len);


/***************************************************************
 *
 * UTF-8 Handling
 *
 *
 */

GB_DEF isize gb_utf8_strlen (char const *str);
GB_DEF isize gb_utf8_strnlen(char const *str, isize max_len);

/* Windows doesn't handle 8 bit filenames well ('cause Micro$hit) */
GB_DEF char16 *gb_utf8_to_utf16(char16 *buffer, char *str, isize len);
GB_DEF char *  gb_utf16_to_utf8(char *buffer, char16 *str, isize len);

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
          str = gb_append_cstring(str, "another string");
      This could be changed to gb_append_cstring(&str, "another string"); but I'm still not sure.

	* This is incompatible with "gb_string.h" strings
 */

#if 0
#include <stdio.h>
#include <stdlib.h>

#define GB_IMPLEMENTATION
#include "gb.h"

int main(int argc, char **argv)
{
	gbString str = gb_make_string("Hello");
	gbString other_str = gb_make_string_length(", ", 2);
	str = gb_append_string(str, other_str);
	str = gb_append_cstring(str, "world!");

	printf("%s\n", str); /* Hello, world! */

	printf("str length = %d\n", gb_string_length(str));

	str = gb_set_string(str, "Potato soup");
	printf("%s\n", str); /* Potato soup */

	str = gb_set_string(str, "Hello");
	other_str = gb_set_string(other_str, "Pizza");
	if (gb_strings_are_equal(str, other_str))
		printf("Not called\n");
	else
		printf("Called\n");

	str = gb_set_string(str, "Ab.;!...AHello World       ??");
	str = gb_trim_string(str, "Ab.;!. ?");
	printf("%s\n", str); /* "Hello World" */

	gb_free_string(str);
	gb_free_string(other_str);

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

GB_DEF gbString gb_make_string       (gbAllocator a, char const *str);
GB_DEF gbString gb_make_string_length(gbAllocator a, void const *str, isize num_bytes);
GB_DEF void     gb_free_string       (gbString str);

GB_DEF gbString gb_duplicate_string(gbAllocator a, gbString const str);

GB_DEF isize gb_string_length         (gbString const str);
GB_DEF isize gb_string_capacity       (gbString const str);
GB_DEF isize gb_string_available_space(gbString const str);

GB_DEF void gb_clear_string(gbString str);

GB_DEF gbString gb_append_string       (gbString str, gbString const other);
GB_DEF gbString gb_append_string_length(gbString str, void const *other, isize num_bytes);
GB_DEF gbString gb_append_cstring      (gbString str, char const *other);

GB_DEF gbString gb_set_string(gbString str, char const *cstr);

GB_DEF gbString gb_make_space_for_string(gbString str, isize add_len);
GB_DEF isize gb_string_allocation_size  (gbString const str);

GB_DEF b32 gb_are_strings_equal(gbString const lhs, gbString const rhs);

GB_DEF gbString gb_trim_string      (gbString str, char const *cut_set);
GB_DEF gbString gb_trim_space_string(gbString str); /* Whitespace ` \t\r\n\v\f` */



/***************************************************************
 *
 * Fixed Capacity Buffer (POD Types)
 *
 */

#ifndef GB_BUFFER_TYPE
#define GB_BUFFER_TYPE
#endif

#define gbBuffer(Type) struct { isize count, capacity; Type *e; }

typedef gbBuffer(u8) gbByteBuffer;

#define gb_init_buffer_from_allocator(x, allocator, cap) do {      \
	void **e = cast(void **)&((x)->e);                       \
	gb_zero_struct(x);                                             \
	(x)->capacity = (cap);                                         \
	*e = gb_alloc((allocator), (cap)*gb_size_of((x)->e[0])); \
} while (0)

#define gb_init_buffer_from_memory(x, memory, cap) do {            \
	void **e = cast(void **)&((x)->e);                       \
	gb_zero_struct(x);                                             \
	(x)->capacity = (cap);                                         \
	*e = memory;                                                \
} while (0)


#define gb_free_buffer(x, allocator) do { gb_free(allocator, (x)->e); } while (0)

#define gb_append_buffer(x, item)    do { (x)->e[(x)->count++] = item; } while (0)

#define gb_appendv_buffer(x, items, item_count) do {                                \
	GB_ASSERT(gb_size_of((items)[0]) == gb_size_of((x)->e[0]));                  \
	GB_ASSERT((x)->count+item_count <= (x)->capacity);                              \
	gb_memcopy((x)->e[a->count], (items), gb_size_of((x)->e[0])*(item_count)); \
	(x)->count += (item_count);                                                     \
} while (0)

#define gb_pop_buffer(x)   do { GB_ASSERT((x)->count > 0); (x)->count--; } while (0)
#define gb_clear_buffer(x) do { (x)->count = 0; } while (0)



/***************************************************************
 *
 * Dynamic Array (POD Types)
 *
 */

/* NOTE(bill): I know this is a macro hell but C is an old (and shit) language with no proper arrays
 * Also why the fuck not?! It fucking works! And it has custom allocation, which is already better than C++!
 */
/* NOTE(bill): Typedef every array or you get anonymous structures everywhere!
 * e.g. typedef gbArray(int) gbIntArray;
 */
#ifndef GB_ARRAY_TYPE
#define GB_ARRAY_TYPE

#define gbArray(Type) struct { gbAllocator allocator; isize count, capacity; Type *e; }

typedef gbArray(void) gbVoidArray; /* NOTE(bill): Useful for generic stuff */

/* Available Procedures for gbArray(Type)
 *     gb_init_array
 *     gb_free_array
 *     gb_set_array_capacity
 *     gb_grow_array
 *     gb_append_array
 *     gb_appendv_array
 *     gb_pop_array
 *     gb_clear_array
 *     gb_resize_array
 *     gb_reserve_array
 */

#if 0 /* Example */
void foo(void)
{
	isize i;
	int test_values[] = {4, 2, 1, 7};
	gbAllocator a = gb_heap_allocator();
	gbArray(int) items;

	gb_init_array(&items, a);

	gb_append_array(&items, 1);
	gb_append_array(&items, 4);
	gb_append_array(&items, 9);
	gb_append_array(&items, 16);

	items.e[1] = 3; /* Manually set value */
	                /* NOTE: No array bounds checking */

	for (i = 0; i < items.count; i++)
		gb_printf("%d\n", items.e[i]);
	/* 1
	 * 3
	 * 9
	 * 16
	 */

	gb_clear_array(&items);

	gb_appendv_array(&items, test_values, gb_count_of(test_values));
	for (i = 0; i < items.count; i++)
		gb_printf("%d\n", items.e[i]);
	/* 4
	 * 2
	 * 1
	 * 7
	 */

	gb_free_array(&items);
}
#endif

#define gb_init_array(x, allocator_) do { gb_zero_struct(x); (x)->allocator = allocator_; } while (0)

#define gb_free_array(x) do {           \
	if ((x)->allocator.proc) {          \
		gbAllocator a = (x)->allocator; \
		gb_free(a, (x)->e);          \
		gb_init_array((x), a);          \
	}                                   \
} while (0)

#define gb_set_array_capacity(array, capacity) gb__set_array_capacity((array), (capacity), gb_size_of((array)->e[0]))
/* NOTE(bill): Do not use the thing below directly, use the macro */
GB_DEF void gb__set_array_capacity(void *array, isize capacity, isize element_size);

#ifndef GB_ARRAY_GROW_FORMULA
#define GB_ARRAY_GROW_FORMULA(x) (2*(x) + 8)
#endif

/* TODO(bill): Decide on a decent growing formula for gbArray */
#define gb_grow_array(x, min_capacity) do {                \
	isize capacity = GB_ARRAY_GROW_FORMULA((x)->capacity); \
	if (capacity < (min_capacity))                         \
		capacity = (min_capacity);                         \
	gb_set_array_capacity(x, capacity);                    \
} while (0)


#define gb_append_array(x, item) do { \
	if ((x)->capacity < (x)->count+1) \
		gb_grow_array(x, 0);          \
	(x)->e[(x)->count++] = (item); \
} while (0)

#define gb_appendv_array(x, items, item_count) do {                                   \
	GB_ASSERT(gb_size_of((items)[0]) == gb_size_of((x)->e[0]));                    \
	if ((x)->capacity < (x)->count+(item_count))                                      \
		gb_grow_array(x, (x)->count+(item_count));                                    \
	gb_memcopy((x)->e[(x)->count], (items), gb_size_of((x)->e[0])*(item_count)); \
	(x)->count += (item_count);                                                       \
} while (0)



#define gb_pop_array(x)   do { GB_ASSERT((x)->count > 0); (x)->count--; } while (0)
#define gb_clear_array(x) do { (x)->count = 0; } while (0)

#define gb_resize_array(x, new_count) do { \
	if ((x)->capacity < (new_count))       \
		gb_grow_array(x, (new_count));     \
	(x)->count = (new_count);              \
} while (0)


#define gb_reserve_array(x, new_capacity) do {   \
	if ((x)->capacity < (new_capacity))         \
		gb_set_array_capacity(x, new_capacity); \
} while (0)


#endif /* GB_ARRAY_TYPE */





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
 * The hash table stores an isize which can be used to point to an array index for storing
 * the hash table's values
 * TODO(bill): Should the hash table store an isize or a void *? Which is more useful for the user?
 */

typedef struct gbHashTableEntry {
	u64   key;
	isize next;
	isize value;
} gbHashTableEntry;

typedef struct gbHashTable {
	gbArray(isize)            hashes;
	gbArray(gbHashTableEntry) entries;
} gbHashTable;


/* TODO(bill): I'm not very sure on the naming of these procedures and if they should be named better. */
GB_DEF void  gb_init_hash_table   (gbHashTable *h, gbAllocator a);
GB_DEF void  gb_destroy_hash_table(gbHashTable *h);
GB_DEF void  gb_clear_hash_table  (gbHashTable *h);
GB_DEF b32   gb_hash_table_has    (gbHashTable const *h, u64 key);
GB_DEF isize gb_hash_table_get    (gbHashTable const *h, u64 key, isize default_index);
GB_DEF void  gb_hash_table_set    (gbHashTable *h, u64 key, isize index);
GB_DEF void  gb_hash_table_remove (gbHashTable *h, u64 key);
GB_DEF void  gb_hash_table_reserve(gbHashTable *h, isize capacity);

GB_DEF void  gb_multi_hash_table_get         (gbHashTable const *h, u64 key, isize *indices, isize index_count);
GB_DEF isize gb_multi_hash_table_count       (gbHashTable const *h, u64 key);
GB_DEF void  gb_multi_hash_table_insert      (gbHashTable *h, u64 key, isize index);
GB_DEF void  gb_multi_hash_table_remove_entry(gbHashTable *h, gbHashTableEntry const *e);
GB_DEF void  gb_multi_hash_table_remove_all  (gbHashTable *h, u64 key);

GB_DEF gbHashTableEntry const *gb_find_first_hash_table_entry(gbHashTable const *h, u64 key);
GB_DEF gbHashTableEntry const *gb_find_next_hash_table_entry (gbHashTable const *h, gbHashTableEntry const *e);


/***************************************************************
 *
 * File Handling
 *
 */
typedef u64 gbFileTime;

typedef enum gbFileAccess {
	GB_FILE_ACCESS_READ  = 1,
	GB_FILE_ACCESS_WRITE = 2
} gbFileAccess;

typedef enum gbFileType {
	GB_FILE_TYPE_UNKNOWN,
	GB_FILE_TYPE_FILE,
	GB_FILE_TYPE_DIRECTORY,

	GB_FILE_TYPE_COUNT
} gbFileType;

typedef struct gbFile {
	void *handle; /* File to fread/fwrite */
	char *path;
	i64 size;
	b32 is_open;
	gbFileAccess access;
	gbFileType type;
	gbFileTime last_write_time;
} gbFile;

typedef struct gbFileContents {
	void *data;
	isize size;
} gbFileContents;


GB_DEF b32 gb_create_file     (gbFile *file, char const *filepath, ...) GB_PRINTF_ARGS(2); /* TODO(bill): Give file permissions */
GB_DEF b32 gb_open_file       (gbFile *file, char const *filepath, ...) GB_PRINTF_ARGS(2);
GB_DEF b32 gb_close_file      (gbFile *file);
GB_DEF b32 gb_file_read_at    (gbFile *file, void *buffer, isize size, i64 offset);
GB_DEF b32 gb_file_write_at   (gbFile *file, void const *buffer, isize size, i64 offset);
GB_DEF i64 gb_file_size       (gbFile *file);
GB_DEF b32 gb_has_file_changed(gbFile *file);

GB_DEF gbFileTime gb_file_last_write_time(char const *filepath, ...) GB_PRINTF_ARGS(1);


GB_DEF b32 gb_copy_file(char const *existing_filename, char const *new_filename, b32 fail_if_exists);
GB_DEF b32 gb_move_file(char const *existing_filename, char const *new_filename);


GB_DEF gbFileContents gb_read_entire_file_contents(gbAllocator a, b32 zero_terminate, char const *filepath, ...) GB_PRINTF_ARGS(3);


#ifndef GB_PATH_SEPARATOR
	#if defined(GB_SYSTEM_WINDOWS)
		#define GB_PATH_SEPARATOR '\\'
	#else
		#define GB_PATH_SEPARATOR '/'
	#endif
#endif

GB_DEF b32         gb_is_path_absolute(char const *path);
GB_DEF b32         gb_is_path_relative(char const *path);
GB_DEF b32         gb_is_path_root    (char const *path);
GB_DEF char const *gb_path_base_name  (char const *path);
GB_DEF char const *gb_path_extension  (char const *path);


GB_DEF void gb_exit(u32 code);


/***************************************************************
 *
 * DLL Handling
 *
 */

typedef void *gbDllHandle;
typedef void (*gbDllProc)(void);

GB_DEF gbDllHandle gb_load_dll        (char const *filepath, ...) GB_PRINTF_ARGS(1);
GB_DEF void        gb_unload_dll      (gbDllHandle dll);
GB_DEF gbDllProc   gb_dll_proc_address(gbDllHandle dll, char const *proc_name);


/***************************************************************
 *
 * Time
 *
 */

typedef struct gbDate {
	i32 year;
	i32 month;        /* 1 - Janurary, ... 12 - December */
	i32 day;          /* 1 - 31 */
	i32 day_of_week;  /* 0 - Sunday, ... 6 - Saturday */
	i32 hour;         /* 0 - 23 */
	i32 minute;       /* 0 - 59 */
	i32 second;       /* 0 - 60 (leap seconds) */
	i32 milliseconds; /* 0 - 999 */
} gbDate;


GB_DEF u64  gb_rdtsc   (void);
GB_DEF f64  gb_time_now(void); /* NOTE(bill): This is only for relative time e.g. game loops */
GB_DEF void gb_sleep_ms(u32 ms);

GB_DEF void gb_get_system_date(gbDate *date);
GB_DEF void gb_get_local_date (gbDate *date);


#if !defined(GB_NO_COLOUR_TYPE)


/***************************************************************
 *
 * Miscellany
 *
 */

GB_DEF void gb_yield(void);
GB_DEF void gb_set_env(char const *name, char const *value);
GB_DEF void gb_unset_env(char const *name);
GB_DEF i32  gb_chdir(char const *path);
GB_DEF void gb_get_working_cmd(char *buffer, isize len);

GB_DEF u16 gb_endian_swap16(u16 i);
GB_DEF u32 gb_endian_swap32(u32 i);
GB_DEF u64 gb_endian_swap64(u64 i);




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

#endif

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

#if defined(__GCC__) || defined(__GNUC__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wattributes"
#endif

i32
gb_printf(char const *fmt, ...)
{
	i32 res;
	va_list va;
	va_start(va, fmt);
	res = gb_fprintf_va(stdout, fmt, va);
	va_end(va);
	return res;
}


i32
gb_fprintf(FILE *f, char const *fmt, ...)
{
	i32 res;
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

i32
gb_snprintf(char *str, isize n, char const *fmt, ...)
{
	i32 res;
	va_list va;
	va_start(va, fmt);
	res = gb_snprintf_va(str, n, fmt, va);
	va_end(va);
	return res;
}


gb_inline i32 gb_printf_va(char const *fmt, va_list va) { return gb_fprintf_va(stdout, fmt, va); }
gb_inline i32 gb_fprintf_va(FILE *f, char const *fmt, va_list va) { return vfprintf(f, fmt, va); }

gb_inline char *
gb_sprintf_va(char const *fmt, va_list va)
{
	gb_local_persist char buffer[1024];
	gb_snprintf_va(buffer, gb_size_of(buffer), fmt, va);
	return buffer;
}

gb_inline i32
gb_snprintf_va(char *str, isize n, char const *fmt, va_list va)
{
	i32 res;
#if defined(_WIN32)
	res = _vsnprintf(str, n, fmt, va);
#else
	res = vsnprintf(str, n, fmt, va);
#endif
	if (n) str[n-1] = 0;
	/* NOTE(bill): Unix returns length output would require, Windows returns negative when truncated. */
	return (res >= n || res < 0) ? -1 : res;
}


gb_inline i32 gb_println(char const *str) { return gb_fprintln(stdout, str); }

gb_inline i32
gb_fprintln(FILE *f, char const *str)
{
	i32 res;
	res = gb_fprintf(f, "%s", str);
	gb_fprintf(f, "\n");
	res++;
	return res;
}





void
gb_assert_handler(char const *condition, char const *file, i32 line, char const *msg)
{
	gb_fprintf(stderr, "%s:%d: Assert Failure: ", file, line);
	if (condition)
		gb_fprintf(stderr, "`%s` ", condition);

	if (msg)
		gb_fprintf(stderr, "%s", msg);

	gb_fprintf(stderr, "\n");
}



gb_inline void *
gb_align_forward(void *ptr, isize align)
{
	uintptr p;
	isize modulo;

	GB_ASSERT(gb_is_power_of_two(align));

	p = cast(uintptr)ptr;
	modulo = p % align;
	if (modulo) p += (align - modulo);
	return cast(void *)p;
}

gb_inline void *gb_pointer_add(void *ptr, isize bytes) { return cast(void *)(cast(u8 *)ptr + bytes); }
gb_inline void *gb_pointer_sub(void *ptr, isize bytes) { return cast(void *)(cast(u8 *)ptr - bytes); }

gb_inline void gb_zero_size(void *ptr, isize size) { gb_memset(ptr, 0, size); }

gb_inline void *gb_memcopy(void *dest, void const *source, isize size) { return memcpy(dest, source, size);     }
gb_inline void *gb_memmove(void *dest, void const *source, isize size) { return memmove(dest, source, size);    }
gb_inline void *gb_memset (void *data, u8 byte_value, isize size)      { return memset(data, byte_value, size); }




gb_inline void *gb_alloc_align (gbAllocator a, isize size, isize alignment)                                { return a.proc(a.data, GB_ALLOCATION_ALLOC, size, alignment, NULL, 0, 0); }
gb_inline void *gb_alloc       (gbAllocator a, isize size)                                                 { return gb_alloc_align(a, size, GB_DEFAULT_MEMORY_ALIGNMENT); }
gb_inline void  gb_free        (gbAllocator a, void *ptr)                                                  { a.proc(a.data, GB_ALLOCATION_FREE, 0, 0, ptr, 0, 0); }
gb_inline void  gb_free_all    (gbAllocator a)                                                             { a.proc(a.data, GB_ALLOCATION_FREE_ALL, 0, 0, NULL, 0, 0); }
gb_inline void *gb_resize      (gbAllocator a, void *ptr, isize old_size, isize new_size)                  { return gb_resize_align(a, ptr, old_size, new_size, GB_DEFAULT_MEMORY_ALIGNMENT); }
gb_inline void *gb_resize_align(gbAllocator a, void *ptr, isize old_size, isize new_size, isize alignment) { return a.proc(a.data, GB_ALLOCATION_RESIZE, new_size, alignment, ptr, old_size, 0); }

gb_inline void *gb_alloc_copy      (gbAllocator a, void const *src, isize size)                  { return gb_memcopy(gb_alloc(a, size), src, size); }
gb_inline void *gb_alloc_copy_align(gbAllocator a, void const *src, isize size, isize alignment) { return gb_memcopy(gb_alloc_align(a, size, alignment), src, size); }

gb_inline char *
gb_alloc_cstring(gbAllocator a, char const *str)
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
  */
#if defined(_MSC_VER)
gb_inline i32
gb_load_atomic32(gbAtomic32 const volatile *a)
{
	return a->value;
}

gb_inline void
gb_store_atomic32(gbAtomic32 volatile *a, i32 value)
{
	a->value = value;
}

gb_inline i32
gb_compare_exchange_strong_atomic32(gbAtomic32 volatile *a, i32 expected, i32 desired)
{
	return _InterlockedCompareExchange(cast(long volatile *)a, desired, expected);
}

gb_inline i32
gb_exchanged_atomic32(gbAtomic32 volatile *a, i32 desired)
{
	return _InterlockedExchange(cast(long volatile *)a, desired);
}

gb_inline i32
gb_fetch_add_atomic32(gbAtomic32 volatile *a, i32 operand)
{
	return _InterlockedExchangeAdd(cast(long volatile *)a, operand);
}

gb_inline i32
gb_fetch_and_atomic32(gbAtomic32 volatile *a, i32 operand)
{
	return _InterlockedAnd(cast(long volatile *)a, operand);
}

gb_inline i32
gb_fetch_or_atomic32(gbAtomic32 volatile *a, i32 operand)
{
	return _InterlockedOr(cast(long volatile *)a, operand);
}


gb_inline i64
gb_load_atomic64(gbAtomic64 const volatile *a)
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
gb_store_atomic64(gbAtomic64 volatile *a, i64 value)
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
gb_compare_exchange_strong_atomic64(gbAtomic64 volatile *a, i64 expected, i64 desired)
{
	return _InterlockedCompareExchange64(cast(i64 volatile *)a, desired, expected);
}

gb_inline i64
gb_exchanged_atomic64(gbAtomic64 volatile *a, i64 desired)
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
gb_fetch_add_atomic64(gbAtomic64 volatile *a, i64 operand)
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
gb_fetch_and_atomic64(gbAtomic64 volatile *a, i64 operand)
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
gb_fetch_or_atomic64(gbAtomic64 volatile *a, i64 operand)
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


gb_inline i32
gb_load_atomic32(gbAtomic32 const volatile *a)
{
#if defined(GB_ARCH_64_BIT)
	return a->value;
#else

#endif
}

gb_inline void
gb_store_atomic32(gbAtomic32 volatile *a, i32 value)
{
	a->value = value;
}

gb_inline i32
gb_compare_exchange_strong_atomic32(gbAtomic32 volatile *a, i32 expected, i32 desired)
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
gb_exchanged_atomic32(gbAtomic32 volatile *a, i32 desired)
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
gb_fetch_add_atomic32(gbAtomic32 volatile *a, i32 operand)
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
gb_fetch_and_atomic32(gbAtomic32 volatile *a, i32 operand)
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
gb_fetch_or_atomic32(gbAtomic32 volatile *a, i32 operand)
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
gb_load_atomic64(gbAtomic64 const volatile *a)
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
gb_store_atomic64(gbAtomic64 volatile *a, i64 value)
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
gb_compare_exchange_strong_atomic64(gbAtomic64 volatile *a, i64 expected, i64 desired)
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
gb_exchanged_atomic64(gbAtomic64 volatile *a, i64 desired)
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
		i64 previous = gb_compare_exchange_strong_atomic64(a, original, desired);
		if (original == previous)
			return original;
		original = previous;
	}
#endif
}

gb_inline i64
gb_fetch_add_atomic64(gbAtomic64 volatile *a, i64 operand)
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
		if (gb_compare_exchange_strong_atomic64(a, original, original + operand) == original)
			return original;
	}
#endif
}

gb_inline i64
gb_fetch_and_atomic64(gbAtomic64 volatile *a, i64 operand)
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
		if (gb_compare_exchange_strong_atomic64(a, original, original & operand) == original)
			return original;
	}
#endif
}

gb_inline i64
gb_fetch_or_atomic64(gbAtomic64 volatile *a, i64 operand)
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
		if (gb_compare_exchange_strong_atomic64(a, original, original | operand) == original)
			return original;
	}
#endif
}
#endif



#if defined(GB_SYSTEM_WINDOWS)
volatile f32 gb__t1 = 1, gb__t2;

gb_internal void
gb__wait(i32 n)
{
	/* NOTE(bill): Taken from stb.h, Thank you Sean Barrett */
	f32 z = 0;
	i32 i;
	for (i = 0; i < n; i++)
		z += 1 / (gb__t1+i);
	gb__t2 = z;
}

gb_inline void
gb_init_mutex(gbMutex *m)
{
#if _WIN32_WINNT >= 0x0500
	InitializeCriticalSectionAndSpinCount(&m->win32_critical_section, 500);
#else
	InitializeCriticalSection(&m->win32_critical_section);
#endif
}
gb_inline void
gb_destroy_mutex(gbMutex *m)
{
	DeleteCriticalSection(&m->win32_critical_section);
}

gb_inline void
gb_lock_mutex(gbMutex *m)
{
	gb__wait(500);
	EnterCriticalSection(&m->win32_critical_section);

}

gb_inline b32
gb_try_lock_mutex(gbMutex *m)
{
	gb__wait(500);
	return cast(b32)TryEnterCriticalSection(&m->win32_critical_section);

}

gb_inline void
gb_unlock_mutex(gbMutex *m)
{
	LeaveCriticalSection(&m->win32_critical_section);
	gb__wait(500);
}


#else
gb_inline void
gb_init_mutex(gbMutex *m)
{
	pthread_mutexattr_t attr;
	pthread_mutexattr_init(&attr);
#if defined (PTHREAD_MUTEX_RECURSIVE) || defined(__FreeBSD__)
	pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
#else
	pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE_NP);
#endif
	pthread_mutex_init(&m->posix_handle, &attr);
}
gb_inline void
gb_destroy_mutex(gbMutex *m)
{
	pthread_mutex_destroy(&m->posix_handle);
}

gb_inline void
gb_lock_mutex(gbMutex *m)
{
	pthread_mutex_lock(&m->posix_handle);
}

gb_inline b32
gb_try_lock_mutex(gbMutex *m)
{
	return pthread_mutex_trylock(&m->posix_handle) == 0;
}

gb_inline void
gb_unlock_mutex(gbMutex *m)
{
	pthread_mutex_unlock(&m->posix_handle);
}
#endif


#if defined(GB_SYSTEM_WINDOWS)
gb_inline void
gb_init_semaphore(gbSemaphore *s)
{
	s->win32_handle = CreateSemaphoreA(NULL, 0, I32_MAX, NULL);
	GB_ASSERT_MSG(s->win32_handle != NULL, "CreateSemaphore: GetLastError");
}

gb_inline void
gb_destroy_semaphore(gbSemaphore *s)
{
	BOOL err = CloseHandle(s->win32_handle);
	GB_ASSERT_MSG(err != 0, "CloseHandle: GetLastError");
}

gb_inline void
gb_post_semaphore(gbSemaphore *s, i32 count)
{
	BOOL err = ReleaseSemaphore(s->win32_handle, count, NULL);
	GB_ASSERT_MSG(err != 0, "ReleaseSemaphore: GetLastError");
}

gb_inline void
gb_wait_semaphore(gbSemaphore *s)
{
	DWORD result = WaitForSingleObject(s->win32_handle, INFINITE);
	GB_ASSERT_MSG(result == WAIT_OBJECT_0, "WaitForSingleObject: GetLastError");
}

#else
gb_inline void
gb_init_semaphore(gbSemaphore *s)
{
	int err = pthread_cond_init(&s->cond, NULL);
	GB_ASSERT(err == 0);
	gb_init_mutex(&s->mutex);
}

gb_inline void
gb_destroy_semaphore(gbSemaphore *s)
{
	int err = pthread_cond_destroy(&s->cond);
	GB_ASSERT(err == 0);
	gb_destroy_mutex(&s->mutex);
}

gb_inline void
gb_post_semaphore(gbSemaphore *s, i32 count)
{
	i32 i;
	gb_lock_mutex(&s->mutex);
	for (i = 0; i < count; i++)
		pthread_cond_signal(&s->cond);
	s->count += count;
	gb_unlock_mutex(&s->mutex);
}

gb_inline void
gb_wait_semaphore(gbSemaphore *s)
{
	gb_lock_mutex(&s->mutex);
	while (s->count <= 0)
		pthread_cond_wait(&s->cond, &s->mutex.posix_handle);
	s->count--;
	gb_unlock_mutex(&s->mutex);
}

#endif




void
gb_init_thread(gbThread *t)
{
	gb_zero_struct(t);
#if defined(GB_SYSTEM_WINDOWS)
	t->win32_handle = INVALID_HANDLE_VALUE;
#else
	t->posix_handle = 0;
#endif
	gb_init_semaphore(&t->semaphore);
}

void
gb_destory_thread(gbThread *t)
{
	if (t->is_running) gb_join_thread(t);
	gb_destroy_semaphore(&t->semaphore);
}


gb_inline void
gb__run_thread(gbThread *t)
{
	gb_post_semaphore(&t->semaphore, 1);
	t->proc(t->data);
}

#if defined(GB_SYSTEM_WINDOWS)
	gb_inline DWORD WINAPI gb__thread_proc(void *arg) { gb__run_thread(cast(gbThread *)arg); return 0; }
#else
	gb_inline void *gb__thread_proc(void *arg) { gb__run_thread(cast(gbThread *)arg); return NULL; }
#endif

gb_inline void gb_start_thread(gbThread *t, gbThreadProc *proc, void *data) { gb_start_thread_with_stack(t, proc, data, 0); }

gb_inline void
gb_start_thread_with_stack(gbThread *t, gbThreadProc *proc, void *data, isize stack_size)
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
	gb_wait_semaphore(&t->semaphore);
}

gb_inline void
gb_join_thread(gbThread *t)
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

gb_inline b32 gb_is_thread_running(gbThread const *t) { return t->is_running != 0; }

gb_inline u32
gb_current_thread_id(void)
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
gb_set_thread_name(gbThread *t, char const *name)
{
#if defined(_MSC_VER)
	/* TODO(bill): Bloody Windows!!! */
	#pragma pack(push, 8)
		struct gbprivThreadName {
			DWORD  type;
			LPCSTR name;
			DWORD  id;
			DWORD  flags;
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
/* TODO(bill): Throughly test! */
	switch (type) {
	case GB_ALLOCATION_ALLOC: {
#if defined(_MSC_VER)
		return _aligned_malloc(size, alignment);
#else
		return aligned_alloc(alignment, size);
#endif
	} break;

	case GB_ALLOCATION_FREE: {
#if defined(_MSC_VER)
		_aligned_free(old_memory);
#else
		free(old_memory);
#endif
	} break;

	case GB_ALLOCATION_FREE_ALL:
		break;

	case GB_ALLOCATION_RESIZE: {
#if defined(_MSC_VER)
		return _aligned_realloc(old_memory, size, alignment);
#else
		gbAllocator a = gb_heap_allocator();
		return gb_default_resize_align(a, old_memory, old_size, size, alignment);
#endif
	} break;
	}

	return NULL; /* NOTE(bill): Default return value */
}








gb_inline void
gb_init_arena_from_memory(gbArena *arena, void *start, isize size)
{
	arena->backing.proc    = NULL;
	arena->backing.data    = NULL;
	arena->physical_start  = start;
	arena->total_size      = size;
	arena->total_allocated = 0;
	arena->temp_count      = 0;
}

gb_inline void
gb_init_arena_from_allocator(gbArena *arena, gbAllocator backing, isize size)
{
	arena->backing         = backing;
	arena->physical_start  = gb_alloc(backing, size); /* NOTE(bill): Uses default alignment */
	arena->total_size      = size;
	arena->total_allocated = 0;
	arena->temp_count      = 0;
}

gb_inline void gb_init_subarena(gbArena *arena, gbArena *parent_arena, isize size) { gb_init_arena_from_allocator(arena, gb_arena_allocator(parent_arena), size); }


gb_inline void
gb_free_arena(gbArena *arena)
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

gb_inline void gb_check_arena(gbArena *arena) { GB_ASSERT(arena->temp_count == 0); }






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
		void *ptr;
		isize actual_size = size + alignment;

		/* NOTE(bill): Out of memory */
		if (arena->total_allocated + actual_size > cast(isize)arena->total_size)
			return NULL;

		ptr = gb_align_forward(gb_pointer_add(arena->physical_start, arena->total_allocated), alignment);
		arena->total_allocated += actual_size;
		return ptr;
	} break;

	case GB_ALLOCATION_FREE:
		/* NOTE(bill): Free all at once */
		/* NOTE(bill): Use Temp_Arena_Memory if you want to free a block */
		/* TODO(bill): Free it if it's on top of the stack */
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
gb_begin_temp_arena_memory(gbArena *arena)
{
	gbTempArenaMemory tmp;
	tmp.arena = arena;
	tmp.original_count = arena->total_allocated;
	arena->temp_count++;
	return tmp;
}

gb_inline void
gb_end_temp_arena_memory(gbTempArenaMemory tmp)
{
	GB_ASSERT(tmp.arena->total_allocated >= tmp.original_count);
	GB_ASSERT(tmp.arena->temp_count > 0);
	tmp.arena->total_allocated = tmp.original_count;
	tmp.arena->temp_count--;
}




gb_inline void
gb_init_pool(gbPool *pool, gbAllocator backing, isize num_blocks, isize block_size)
{
	gb_init_pool_align(pool, backing, num_blocks, block_size, GB_DEFAULT_MEMORY_ALIGNMENT);
}

void
gb_init_pool_align(gbPool *pool, gbAllocator backing, isize num_blocks, isize block_size, isize block_align)
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
gb_free_pool(gbPool *pool)
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


gb_inline void
gb_qsort(void *base, isize count, isize size, gbCompareProc compare_proc)
{
	qsort(base, count, size, compare_proc);
}

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
		u8 skcount = offsets[i];
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
			u16 skcount = offsets[i];
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
			u32 skcount = offsets[i];
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
gb_is_char_space(char c)
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
gb_is_char_digit(char c)
{
	if (c >= '0' && c <= '9')
		return true;
	return false;
}

gb_inline b32
gb_is_char_hex_digit(char c)
{
	if (gb_is_char_digit(c) ||
	    (c >= 'a' && c <= 'f') ||
	    (c >= 'A' && c <= 'F'))
	    return true;
	return false;
}

gb_inline b32
gb_is_char_alpha(char c)
{
	if ((c >= 'A' && c <= 'Z') ||
	    (c >= 'a' && c <= 'z'))
	    return true;
	return false;
}

gb_inline b32
gb_is_char_alphanumeric(char c)
{
	return gb_is_char_alpha(c) || gb_is_char_digit(c);
}

gb_inline i32
gb_digit_to_int(char c)
{
	return gb_is_char_digit(c) ? c - '0' : c - 'W';
}


gb_inline i32
gb_hex_digit_to_int(char c)
{
	if (gb_is_char_digit(c))
		return gb_digit_to_int(c);
	else if (gb_is_between(c, 'a', 'f'))
		return c - 'a' + 10;
	else if (gb_is_between(c, 'A', 'F'))
		return c - 'A' + 10;
	return 0;
}




gb_inline void
gb_to_lower(char *str)
{
	if (!str) return;
	while (*str) {
		*str = gb_char_to_lower(*str);
		str++;
	}
}

gb_inline void
gb_to_upper(char *str)
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

gb_inline i32
gb_strncmp(char const *s1, char const *s2, isize len)
{
	for(; len > 0; s1++, s2++, len--) {
		if (*s1 != *s2)
			return ((cast(uintptr)s1 < cast(uintptr)s2) ? -1 : +1);
		else if (*s1 == '\0')
			return 0;
	}
	return 0;
}


gb_inline char const *
gb_strtok(char *output, char const *src, char const *delimit)
{
	while (*src && gb_first_occurence_of_char(delimit, *src) != NULL) {
		*output++ = *src++;
	}

	*output = 0;
	return *src ? src+1 : src;
}

gb_inline b32
gb_cstr_has_prefix(char const *str, char const *prefix)
{
	while (*prefix) {
		if (*str++ != *prefix++)
			return false;
	}
	return true;
}

gb_inline b32
gb_cstr_has_suffix(char const *str, char const *suffix)
{
	isize i = gb_strlen(str);
	isize j = gb_strlen(suffix);
	if (j <= i)
		return gb_strcmp(str+i-j, suffix) == 0;
	return false;
}




gb_inline char const *
gb_first_occurence_of_char(char const *s, char c)
{
	char ch = c;
	for (; *s != ch; s++) {
		if (*s == '\0')
			return NULL;
	}
	return s;
}


gb_inline char const *
gb_last_occurence_of_char(char const *s, char c)
{
	char const *result = NULL;
	do {
		if (*s == c)
			result = s;
	} while (*s++);

	return result;
}



gb_inline void
gb_cstr_concat(char *dest, isize dest_len,
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










gb_inline void gb__set_string_length  (gbString str, isize len) { GB_STRING_HEADER(str)->length = len; }
gb_inline void gb__set_string_capacity(gbString str, isize cap) { GB_STRING_HEADER(str)->capacity = cap; }


gb_inline gbString
gb_make_string(gbAllocator a, char const *str)
{
	isize len = str ? gb_strlen(str) : 0;
	return gb_make_string_length(a, str, len);
}

gbString
gb_make_string_length(gbAllocator a, void const *init_str, isize num_bytes)
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
gb_free_string(gbString str)
{
	if (str) {
		gbStringHeader *header = GB_STRING_HEADER(str);
		gb_free(header->allocator, header);
	}
}


gb_inline gbString gb_string_duplicate(gbAllocator a, gbString const str) { return gb_make_string_length(a, str, gb_string_length(str)); }

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


gb_inline void gb_clear_string(gbString str) { gb__set_string_length(str, 0); str[0] = '\0'; }

gb_inline gbString gb_append_string(gbString str, gbString const other) { return gb_append_string_length(str, other, gb_string_length(other)); }

gbString
gb_append_string_length(gbString str, void const *other, isize other_len)
{
	isize curr_len = gb_string_length(str);

	str = gb_make_space_for_string(str, other_len);
	if (str == NULL)
		return NULL;

	gb_memcopy(str + curr_len, other, other_len);
	str[curr_len + other_len] = '\0';
	gb__set_string_length(str, curr_len + other_len);

	return str;
}

gb_inline gbString
gb_append_cstring(gbString str, char const *other)
{
	return gb_append_string_length(str, other, cast(isize)strlen(other));
}


gbString
gb_set_string(gbString str, char const *cstr)
{
	isize len = gb_strlen(cstr);
	if (gb_string_capacity(str) < len) {
		str = gb_make_space_for_string(str, len - gb_string_length(str));
		if (str == NULL)
			return NULL;
	}

	gb_memcopy(str, cstr, len);
	str[len] = '\0';
	gb__set_string_length(str, len);

	return str;
}



gbString
gb_make_space_for_string(gbString str, isize add_len)
{
	isize available = gb_string_available_space(str);

	/* Return if there is enough space left */
	if (available >= add_len) {
		return str;
	} else {
		isize new_len = gb_string_length(str) + add_len;
		void *ptr = GB_STRING_HEADER(str);
		isize old_size = gb_size_of(gbStringHeader) + gb_string_length(str) + 1;
		isize new_size = gb_size_of(gbStringHeader) + new_len + 1;

		void *new_ptr = gb_resize(GB_STRING_HEADER(str)->allocator, ptr, old_size, new_size);
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
gb_are_strings_equal(gbString const lhs, gbString const rhs)
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
gb_trim_string(gbString str, char const *cut_set)
{
	char *start, *end, *start_pos, *end_pos;
	isize len;

	start_pos = start = str;
	end_pos   = end   = str + gb_string_length(str) - 1;

	while (start_pos <= end && gb_first_occurence_of_char(cut_set, *start_pos))
		start_pos++;
	while (end_pos > start_pos && gb_first_occurence_of_char(cut_set, *end_pos))
		end_pos--;

	len = cast(isize)((start_pos > end_pos) ? 0 : ((end_pos - start_pos)+1));

	if (str != start_pos)
		gb_memmove(str, start_pos, len);
	str[len] = '\0';

	gb__set_string_length(str, len);

	return str;
}

gb_inline gbString gb_trim_space_string(gbString str) { return gb_trim_string(str, " \t\r\n\v\f"); }




/***************************************************************
 *
 * Windows UTF-8 Handling
 *
 */


char16 *
gb_utf8_to_utf16(char16 *buffer, char *s, isize len)
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
			buffer[i++] = c + (*str++ & 0x3f);
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
			buffer[i++] = c + (*str++ & 0x3f);
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
gb_utf16_to_utf8(char *buffer, char16 *str, isize len)
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
			buffer[i++] = 0xc0 + (*str >> 6);
			buffer[i++] = 0x80 + (*str & 0x3f);
			str += 1;
		} else if (*str >= 0xd800 && *str < 0xdc00) {
			char32 c;
			if (i+4 > len)
				return NULL;
			c = ((str[0] - 0xd800) << 10) + ((str[1]) - 0xdc00) + 0x10000;
			buffer[i++] = 0xf0 + (c >> 18);
			buffer[i++] = 0x80 + ((c >> 12) & 0x3f);
			buffer[i++] = 0x80 + ((c >>  6) & 0x3f);
			buffer[i++] = 0x80 + ((c      ) & 0x3f);
			str += 2;
		} else if (*str >= 0xdc00 && *str < 0xe000) {
			return NULL;
		} else {
			if (i+3 > len)
				return NULL;
			buffer[i++] = 0xe0 + (*str >> 12);
			buffer[i++] = 0x80 + ((*str >> 6) & 0x3f);
			buffer[i++] = 0x80 + ((*str     ) & 0x3f);
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
	char32 cd;

	GB_ASSERT_NOT_NULL(s);
	GB_ASSERT_NOT_NULL(c);

	if (!s || !c) return 0;
	if (!str_len) return 0;
	*c = GB__UTF_INVALID;

	cd = gb__utf_decode_byte(s[0], &len);
	if (!gb_is_between(len, 1, GB__UTF_SIZE))
		return 1;

	for (i = 1, j = 1; i < str_len && j < len; i++, j++) {
		cd = (cd << 6) | gb__utf_decode_byte(s[i], &type);
		if (type != 0)
			return j;
	}
	if (j < len)
		return 0;
	*c = cd;
	gb__utf_validate(c, len);
	return len;
}




/***************************************************************
 *
 * Array
 *
 */


gb_no_inline void
gb__set_array_capacity(void *array_, isize capacity, isize element_size)
{
	/* NOTE(bill): I know this is unsafe so don't call this function directly */
	gbVoidArray *a = cast(gbVoidArray *)array_;
	void *e = NULL;

	GB_ASSERT(element_size > 0);

	if (capacity == a->capacity)
		return;

	if (capacity < a->count) {
		if (a->capacity < capacity) {
			isize new_capacity = GB_ARRAY_GROW_FORMULA(a->capacity);
			if (new_capacity < capacity)
				new_capacity = capacity;
			gb__set_array_capacity(a, new_capacity, element_size);
		}
		a->count = capacity;
	}

	if (capacity > 0) {
		e = gb_alloc(a->allocator, element_size*capacity);
		gb_memcopy(e, a->e, element_size*a->count);
	}
	gb_free(a->allocator, a->e);
	a->e = e;
	a->capacity = capacity;
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
	isize i;
	u8 const *bytes = cast(u8 const *)data;
	for (i = 0; i < len; i++) {
		a = (a + bytes[i]) % MOD_ALDER;
		b = (b + a) % MOD_ALDER;
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
	0x0000000000000000ull, 0x42F0E1EBA9EA3693ull, 0x85E1C3D753D46D26ull, 0xC711223CFA3E5BB5ull,
	0x493366450E42ECDFull, 0x0BC387AEA7A8DA4Cull, 0xCCD2A5925D9681F9ull, 0x8E224479F47CB76Aull,
	0x9266CC8A1C85D9BEull, 0xD0962D61B56FEF2Dull, 0x17870F5D4F51B498ull, 0x5577EEB6E6BB820Bull,
	0xDB55AACF12C73561ull, 0x99A54B24BB2D03F2ull, 0x5EB4691841135847ull, 0x1C4488F3E8F96ED4ull,
	0x663D78FF90E185EFull, 0x24CD9914390BB37Cull, 0xE3DCBB28C335E8C9ull, 0xA12C5AC36ADFDE5Aull,
	0x2F0E1EBA9EA36930ull, 0x6DFEFF5137495FA3ull, 0xAAEFDD6DCD770416ull, 0xE81F3C86649D3285ull,
	0xF45BB4758C645C51ull, 0xB6AB559E258E6AC2ull, 0x71BA77A2DFB03177ull, 0x334A9649765A07E4ull,
	0xBD68D2308226B08Eull, 0xFF9833DB2BCC861Dull, 0x388911E7D1F2DDA8ull, 0x7A79F00C7818EB3Bull,
	0xCC7AF1FF21C30BDEull, 0x8E8A101488293D4Dull, 0x499B3228721766F8ull, 0x0B6BD3C3DBFD506Bull,
	0x854997BA2F81E701ull, 0xC7B97651866BD192ull, 0x00A8546D7C558A27ull, 0x4258B586D5BFBCB4ull,
	0x5E1C3D753D46D260ull, 0x1CECDC9E94ACE4F3ull, 0xDBFDFEA26E92BF46ull, 0x990D1F49C77889D5ull,
	0x172F5B3033043EBFull, 0x55DFBADB9AEE082Cull, 0x92CE98E760D05399ull, 0xD03E790CC93A650Aull,
	0xAA478900B1228E31ull, 0xE8B768EB18C8B8A2ull, 0x2FA64AD7E2F6E317ull, 0x6D56AB3C4B1CD584ull,
	0xE374EF45BF6062EEull, 0xA1840EAE168A547Dull, 0x66952C92ECB40FC8ull, 0x2465CD79455E395Bull,
	0x3821458AADA7578Full, 0x7AD1A461044D611Cull, 0xBDC0865DFE733AA9ull, 0xFF3067B657990C3Aull,
	0x711223CFA3E5BB50ull, 0x33E2C2240A0F8DC3ull, 0xF4F3E018F031D676ull, 0xB60301F359DBE0E5ull,
	0xDA050215EA6C212Full, 0x98F5E3FE438617BCull, 0x5FE4C1C2B9B84C09ull, 0x1D14202910527A9Aull,
	0x93366450E42ECDF0ull, 0xD1C685BB4DC4FB63ull, 0x16D7A787B7FAA0D6ull, 0x5427466C1E109645ull,
	0x4863CE9FF6E9F891ull, 0x0A932F745F03CE02ull, 0xCD820D48A53D95B7ull, 0x8F72ECA30CD7A324ull,
	0x0150A8DAF8AB144Eull, 0x43A04931514122DDull, 0x84B16B0DAB7F7968ull, 0xC6418AE602954FFBull,
	0xBC387AEA7A8DA4C0ull, 0xFEC89B01D3679253ull, 0x39D9B93D2959C9E6ull, 0x7B2958D680B3FF75ull,
	0xF50B1CAF74CF481Full, 0xB7FBFD44DD257E8Cull, 0x70EADF78271B2539ull, 0x321A3E938EF113AAull,
	0x2E5EB66066087D7Eull, 0x6CAE578BCFE24BEDull, 0xABBF75B735DC1058ull, 0xE94F945C9C3626CBull,
	0x676DD025684A91A1ull, 0x259D31CEC1A0A732ull, 0xE28C13F23B9EFC87ull, 0xA07CF2199274CA14ull,
	0x167FF3EACBAF2AF1ull, 0x548F120162451C62ull, 0x939E303D987B47D7ull, 0xD16ED1D631917144ull,
	0x5F4C95AFC5EDC62Eull, 0x1DBC74446C07F0BDull, 0xDAAD56789639AB08ull, 0x985DB7933FD39D9Bull,
	0x84193F60D72AF34Full, 0xC6E9DE8B7EC0C5DCull, 0x01F8FCB784FE9E69ull, 0x43081D5C2D14A8FAull,
	0xCD2A5925D9681F90ull, 0x8FDAB8CE70822903ull, 0x48CB9AF28ABC72B6ull, 0x0A3B7B1923564425ull,
	0x70428B155B4EAF1Eull, 0x32B26AFEF2A4998Dull, 0xF5A348C2089AC238ull, 0xB753A929A170F4ABull,
	0x3971ED50550C43C1ull, 0x7B810CBBFCE67552ull, 0xBC902E8706D82EE7ull, 0xFE60CF6CAF321874ull,
	0xE224479F47CB76A0ull, 0xA0D4A674EE214033ull, 0x67C58448141F1B86ull, 0x253565A3BDF52D15ull,
	0xAB1721DA49899A7Full, 0xE9E7C031E063ACECull, 0x2EF6E20D1A5DF759ull, 0x6C0603E6B3B7C1CAull,
	0xF6FAE5C07D3274CDull, 0xB40A042BD4D8425Eull, 0x731B26172EE619EBull, 0x31EBC7FC870C2F78ull,
	0xBFC9838573709812ull, 0xFD39626EDA9AAE81ull, 0x3A28405220A4F534ull, 0x78D8A1B9894EC3A7ull,
	0x649C294A61B7AD73ull, 0x266CC8A1C85D9BE0ull, 0xE17DEA9D3263C055ull, 0xA38D0B769B89F6C6ull,
	0x2DAF4F0F6FF541ACull, 0x6F5FAEE4C61F773Full, 0xA84E8CD83C212C8Aull, 0xEABE6D3395CB1A19ull,
	0x90C79D3FEDD3F122ull, 0xD2377CD44439C7B1ull, 0x15265EE8BE079C04ull, 0x57D6BF0317EDAA97ull,
	0xD9F4FB7AE3911DFDull, 0x9B041A914A7B2B6Eull, 0x5C1538ADB04570DBull, 0x1EE5D94619AF4648ull,
	0x02A151B5F156289Cull, 0x4051B05E58BC1E0Full, 0x87409262A28245BAull, 0xC5B073890B687329ull,
	0x4B9237F0FF14C443ull, 0x0962D61B56FEF2D0ull, 0xCE73F427ACC0A965ull, 0x8C8315CC052A9FF6ull,
	0x3A80143F5CF17F13ull, 0x7870F5D4F51B4980ull, 0xBF61D7E80F251235ull, 0xFD913603A6CF24A6ull,
	0x73B3727A52B393CCull, 0x31439391FB59A55Full, 0xF652B1AD0167FEEAull, 0xB4A25046A88DC879ull,
	0xA8E6D8B54074A6ADull, 0xEA16395EE99E903Eull, 0x2D071B6213A0CB8Bull, 0x6FF7FA89BA4AFD18ull,
	0xE1D5BEF04E364A72ull, 0xA3255F1BE7DC7CE1ull, 0x64347D271DE22754ull, 0x26C49CCCB40811C7ull,
	0x5CBD6CC0CC10FAFCull, 0x1E4D8D2B65FACC6Full, 0xD95CAF179FC497DAull, 0x9BAC4EFC362EA149ull,
	0x158E0A85C2521623ull, 0x577EEB6E6BB820B0ull, 0x906FC95291867B05ull, 0xD29F28B9386C4D96ull,
	0xCEDBA04AD0952342ull, 0x8C2B41A1797F15D1ull, 0x4B3A639D83414E64ull, 0x09CA82762AAB78F7ull,
	0x87E8C60FDED7CF9Dull, 0xC51827E4773DF90Eull, 0x020905D88D03A2BBull, 0x40F9E43324E99428ull,
	0x2CFFE7D5975E55E2ull, 0x6E0F063E3EB46371ull, 0xA91E2402C48A38C4ull, 0xEBEEC5E96D600E57ull,
	0x65CC8190991CB93Dull, 0x273C607B30F68FAEull, 0xE02D4247CAC8D41Bull, 0xA2DDA3AC6322E288ull,
	0xBE992B5F8BDB8C5Cull, 0xFC69CAB42231BACFull, 0x3B78E888D80FE17Aull, 0x7988096371E5D7E9ull,
	0xF7AA4D1A85996083ull, 0xB55AACF12C735610ull, 0x724B8ECDD64D0DA5ull, 0x30BB6F267FA73B36ull,
	0x4AC29F2A07BFD00Dull, 0x08327EC1AE55E69Eull, 0xCF235CFD546BBD2Bull, 0x8DD3BD16FD818BB8ull,
	0x03F1F96F09FD3CD2ull, 0x41011884A0170A41ull, 0x86103AB85A2951F4ull, 0xC4E0DB53F3C36767ull,
	0xD8A453A01B3A09B3ull, 0x9A54B24BB2D03F20ull, 0x5D45907748EE6495ull, 0x1FB5719CE1045206ull,
	0x919735E51578E56Cull, 0xD367D40EBC92D3FFull, 0x1476F63246AC884Aull, 0x568617D9EF46BED9ull,
	0xE085162AB69D5E3Cull, 0xA275F7C11F7768AFull, 0x6564D5FDE549331Aull, 0x279434164CA30589ull,
	0xA9B6706FB8DFB2E3ull, 0xEB46918411358470ull, 0x2C57B3B8EB0BDFC5ull, 0x6EA7525342E1E956ull,
	0x72E3DAA0AA188782ull, 0x30133B4B03F2B111ull, 0xF7021977F9CCEAA4ull, 0xB5F2F89C5026DC37ull,
	0x3BD0BCE5A45A6B5Dull, 0x79205D0E0DB05DCEull, 0xBE317F32F78E067Bull, 0xFCC19ED95E6430E8ull,
	0x86B86ED5267CDBD3ull, 0xC4488F3E8F96ED40ull, 0x0359AD0275A8B6F5ull, 0x41A94CE9DC428066ull,
	0xCF8B0890283E370Cull, 0x8D7BE97B81D4019Full, 0x4A6ACB477BEA5A2Aull, 0x089A2AACD2006CB9ull,
	0x14DEA25F3AF9026Dull, 0x562E43B4931334FEull, 0x913F6188692D6F4Bull, 0xD3CF8063C0C759D8ull,
	0x5DEDC41A34BBEEB2ull, 0x1F1D25F19D51D821ull, 0xD80C07CD676F8394ull, 0x9AFCE626CE85B507ull,
};

u32
gb_crc32(void const *data, isize len)
{
	isize remaining;
	u32 result = cast(u32)(~0);
	u8 const *c = cast(u8 const *)data;
	for (remaining = len; remaining--; c++)
		result = (result >> 8) ^ (GB__CRC32_TABLE[(result ^ *c) & 0xff]);
	return ~result;
}

u64
gb_crc64(void const *data, isize len)
{
	isize remaining;
	u64 result = cast(u64)(~0);
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
	u32 const m = 5;
	u32 const n = 0xe6546b64;

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
		u32 k1 = *data++;
		k1 *= m;
		k1 ^= k1 >> r;
		k1 *= m;
		h1 *= m;
		h1 ^= k1;
		len -= 4;

		u32 k2 = *data++;
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
gb_init_hash_table(gbHashTable *h, gbAllocator a)
{
	gb_init_array(&h->hashes, a);
	gb_init_array(&h->entries, a);
}

gb_inline void
gb_destroy_hash_table(gbHashTable *h)
{
	gb_free_array(&h->hashes);
	gb_free_array(&h->entries);
}

typedef struct gbprivFindResult {
	isize hash_index;
	isize data_prev;
	isize entry_index;
} gbprivFindResult;

gb_global gbprivFindResult const GB__INVALID_FIND_RESULT = {-1, -1, -1};

gb_internal gbprivFindResult
gb__find_result_from_key(gbHashTable const *h, u64 key)
{
	gbprivFindResult fr = GB__INVALID_FIND_RESULT;

	if (h->hashes.count == 0)
		return fr;

	fr.hash_index = key % h->hashes.count;
	fr.entry_index = h->hashes.e[fr.hash_index];
	while (fr.entry_index >= 0) {
		if (h->entries.e[fr.entry_index].key == key)
			return fr;
		fr.data_prev = fr.entry_index;
		fr.entry_index = h->entries.e[fr.entry_index].next;
	}

	return fr;
}

gb_internal gb_inline isize
gb__find_entry_of_fail(gbHashTable const *h, u64 key)
{
	return gb__find_result_from_key(h, key).entry_index;
}

gb_inline b32
gb_hash_table_has(gbHashTable const *h, u64 key)
{
	return gb__find_entry_of_fail(h, key) >= 0;
}

gb_inline isize
gb_hash_table_get(gbHashTable const *h, u64 key, isize default_index)
{
	isize index = gb__find_entry_of_fail(h, key);

	if (index < 0)
		return default_index;
	return h->entries.e[index].value;
}

gb_internal gb_inline isize
gb__add_hash_table_entry(gbHashTable *h, u64 key)
{
	isize i = h->entries.count;
	gbHashTableEntry e = {0};
	e.key = key;
	e.next = -1;
	gb_append_array(&h->entries, e);

	return i;
}

gb_internal isize
gb__make_hash_table_entry(gbHashTable *h, u64 key)
{
	gbprivFindResult fr = gb__find_result_from_key(h, key);
	isize index = gb__add_hash_table_entry(h, key);

	if (fr.data_prev < 0)
		h->hashes.e[fr.hash_index] = index;
	else
		h->entries.e[fr.data_prev].next = index;

	h->entries.e[index].next = fr.entry_index;

	return index;
}

gb_internal gb_inline isize
gb__is_hash_table_full(gbHashTable *h)
{
	f64 const MAXIMUM_LOAD_COEFFICIENT = 0.75;
	return h->entries.count >= MAXIMUM_LOAD_COEFFICIENT * h->hashes.count;
}

gb_internal gb_inline void gb__grow_hash_table(gbHashTable *h);

gb_inline void
gb_multi_hash_table_insert(gbHashTable *h, u64 key, isize index)
{
	isize next;
	if (h->hashes.count == 0)
		gb__grow_hash_table(h);

	next = gb__make_hash_table_entry(h, key);
	h->entries.e[next].value = index;

	if (gb__is_hash_table_full(h))
		gb__grow_hash_table(h);
}

gb_inline void
gb_multi_hash_table_get(gbHashTable const *h, u64 key, isize *indices, isize index_count)
{
	isize i = 0;
	gbHashTableEntry const *e = gb_find_first_hash_table_entry(h, key);
	while (e && index_count --> 0) {
		indices[i++] = e->value;
		e = gb_find_next_hash_table_entry(h, e);
	}
}

gb_inline isize
gb_multi_hash_table_count(gbHashTable const *h, u64 key)
{
	isize count = 0;
	gbHashTableEntry const *e = gb_find_first_hash_table_entry(h, key);
	while (e) {
		count++;
		e = gb_find_next_hash_table_entry(h, e);
	}
	return count;
}

gbHashTableEntry const *
gb_find_first_hash_table_entry(gbHashTable const *h, u64 key)
{
	isize index = gb__find_entry_of_fail(h, key);
	if (index < 0) return NULL;
	return &h->entries.e[index];
}

gbHashTableEntry const *
gb_find_next_hash_table_entry(gbHashTable const *h, gbHashTableEntry const *e)
{
	if (e) {
		isize index = e->next;
		while (index >= 0) {
			if (h->entries.e[index].key == e->key)
				return &h->entries.e[index];
			index = h->entries.e[index].next;
		}
	}
	return NULL;
}

gb_internal gbprivFindResult
gb__find_result_from_entry(gbHashTable *h, gbHashTableEntry const *e)
{
	gbprivFindResult fr = GB__INVALID_FIND_RESULT;

	if (h->hashes.count == 0 || !e)
		return fr;

	fr.hash_index  = e->key % h->hashes.count;
	fr.entry_index = h->hashes.e[fr.hash_index];
	while (fr.entry_index >= 0) {
		if (&h->entries.e[fr.entry_index] == e)
			return fr;
		fr.data_prev = fr.entry_index;
		fr.entry_index = h->entries.e[fr.entry_index].next;
	}

	return fr;
}

gb_internal void
gb__erase_hash_table_find_result(gbHashTable *h, gbprivFindResult fr)
{
	if (fr.data_prev < 0)
		h->hashes.e[fr.hash_index] = h->entries.e[fr.entry_index].next;
	else
		h->entries.e[fr.data_prev].next = h->entries.e[fr.entry_index].next;

	gb_pop_array(&h->entries);

	if (fr.entry_index != h->entries.count) {
		gbprivFindResult last;
		h->entries.e[fr.entry_index] = h->entries.e[h->entries.count];
		last = gb__find_result_from_key(h, h->entries.e[fr.entry_index].key);
		if (last.data_prev < 0)
			h->hashes.e[last.hash_index] = fr.entry_index;
		else
			h->entries.e[last.entry_index].next = fr.entry_index;
	}
}

gb_inline void
gb_multi_hash_table_remove_entry(gbHashTable *h, gbHashTableEntry const *e)
{
	gbprivFindResult fr = gb__find_result_from_entry(h, e);
	if (fr.entry_index >= 0)
		gb__erase_hash_table_find_result(h, fr);
}

gb_inline void
gb_multi_hash_table_remove_all(gbHashTable *h, u64 key)
{
	while (gb_hash_table_has(h, key))
		gb_hash_table_remove(h, key);
}

gb_internal void
gb__rehash(gbHashTable *h, isize new_capacity)
{
	gbHashTable nh;
	isize i;

	gb_init_hash_table(&nh, h->hashes.allocator);
	gb_resize_array(&nh.hashes, new_capacity);
	gb_reserve_array(&nh.entries, h->entries.count);

	for (i = 0; i < new_capacity; i++)
		nh.hashes.e[i] = -1;

	for (i = 0; i < h->entries.count; i++) {
		gbHashTableEntry *e = &h->entries.e[i];
		gb_multi_hash_table_insert(&nh, e->key, e->value);
	}

	{
		gbHashTable empty;
		gb_init_hash_table(&empty, h->hashes.allocator);
		gb_destroy_hash_table(h);

		gb_memcopy(&nh, &h, gb_size_of(gbHashTable));
		gb_memcopy(&empty, &nh, gb_size_of(gbHashTable));
	}

}

gb_internal gb_inline void
gb__grow_hash_table(gbHashTable *h)
{
	isize new_capacity = 2*h->entries.count + 8;
	gb__rehash(h, new_capacity);
}

gb_internal isize
gb__find_or_make_entry(gbHashTable *h, u64 key)
{
	isize index;
	gbprivFindResult fr = gb__find_result_from_key(h, key);
	if (fr.entry_index >= 0)
		return fr.entry_index;
	index = gb__add_hash_table_entry(h, key);
	if (fr.data_prev < 0)
		h->hashes.e[fr.hash_index] = index;
	else
		h->entries.e[fr.data_prev].next = index;
	return index;
}

gb_inline void
gb_hash_table_set(gbHashTable *h, u64 key, isize index)
{
	isize i;
	if (h->hashes.count == 0)
		gb__grow_hash_table(h);
	i = gb__find_or_make_entry(h, key);
	h->entries.e[i].value = index;
	if (gb__is_hash_table_full(h))
		gb__grow_hash_table(h);
}


gb_internal gb_inline void
gb__find_and_erase_entry(gbHashTable *h, u64 key)
{
	gbprivFindResult fr = gb__find_result_from_key(h, key);
	if (fr.entry_index >= 0)
		gb__erase_hash_table_find_result(h, fr);
}

gb_inline void
gb_hash_table_remove(gbHashTable *h, u64 key)
{
	gb__find_and_erase_entry(h, key);
}

gb_inline void
gb_hash_table_reserve(gbHashTable *h, isize capacity)
{
	gb__rehash(h, capacity);
}

gb_inline void
gb_clear_hash_table(gbHashTable *h)
{
	gb_clear_array(&h->hashes);
	gb_clear_array(&h->entries);
}




/***************************************************************
 *
 * File Handling
 *
 */

b32
gb_create_file(gbFile *file, char const *filepath, ...)
{
	va_list va;
	char *path;
	gb_zero_struct(file);

	va_start(va, filepath);
	path = gb_sprintf_va(filepath, va);
	va_end(va);

	file->handle = fopen(path, "wb");
	if (file->handle) {
		file->path = gb_alloc_cstring(gb_heap_allocator(), path);
		file->size = gb_file_size(file);
		file->is_open = true;
		file->access = GB_FILE_ACCESS_WRITE;
		file->last_write_time = gb_file_last_write_time(file->path);
		return true;
	}
	return false;
}



b32
gb_open_file(gbFile *file, char const *filepath, ...)
{
	va_list va;
	char *path;
	gb_zero_struct(file);

	va_start(va, filepath);
	path = gb_sprintf_va(filepath, va);
	va_end(va);

	file->handle = fopen(path, "rb");
	if (file->handle) {
		file->path = gb_alloc_cstring(gb_heap_allocator(), path);
		file->size = gb_file_size(file);
		file->is_open = true;
		file->access  = GB_FILE_ACCESS_READ;
		file->type    = GB_FILE_TYPE_UNKNOWN;
	#if defined(_MSC_VER)
		{
			struct _stat64 st;
			if (_stat64(path, &st) == 0) {
				if ((st.st_mode & _S_IFREG) != 0)
					file->type = GB_FILE_TYPE_FILE;
				else if ((st.st_mode & _S_IFDIR) != 0)
					file->type = GB_FILE_TYPE_DIRECTORY;
			}
		}
	#else
		{
			struct stat64 st;
			if (stat64(path, &st) == 0) {
				if ((st.st_mode & S_IFREG) != 0)
					file->type = GB_FILE_TYPE_FILE;
				else if ((st.st_mode & S_IFDIR) != 0)
					file->type = GB_FILE_TYPE_DIRECTORY;
			}
		}
	#endif
		file->last_write_time = gb_file_last_write_time(file->path);
		return true;
	}
	return false;
}

gb_inline b32
gb_close_file(gbFile *file)
{
	b32 result = true;
	if (file && file->handle)
		result = fclose(cast(FILE *)file->handle) != 0; /* TODO(bill): Handle fclose errors */

	if (file->path) gb_free(gb_heap_allocator(), file->path);
	file->is_open = false;

	return result;
}

gb_inline b32
gb_file_read_at(gbFile *file, void *buffer, isize size, i64 offset)
{
	i64 prev_cursor_pos;

	GB_ASSERT(file->access == GB_FILE_ACCESS_READ);

	prev_cursor_pos = ftell(cast(FILE *)file->handle);
	fseek(cast(FILE *)file->handle, offset, SEEK_SET);
	fread(buffer, 1, size, cast(FILE *)file->handle);
	fseek(cast(FILE *)file->handle, prev_cursor_pos, SEEK_SET);
	return true;
}

gb_inline b32
gb_file_write_at(gbFile *file, void const *buffer, isize size, i64 offset)
{
	isize written_size;
	i64 prev_cursor_pos;

	GB_ASSERT(file->access == GB_FILE_ACCESS_WRITE);

	prev_cursor_pos = ftell(cast(FILE *)file->handle);
	fseek(cast(FILE *)file->handle, offset, SEEK_SET);

	written_size = fwrite(buffer, 1, size, cast(FILE *)file->handle);
	fseek(cast(FILE *)file->handle, prev_cursor_pos, SEEK_SET);
	if (written_size != size) {
		GB_PANIC("Failed to write file data");
		return false;
	}

	return true;
}

gb_inline i64
gb_file_size(gbFile *file)
{
	i64 result_size;

	fseek(cast(FILE *)file->handle, 0, SEEK_END);
	result_size = cast(i64)ftell(cast(FILE *)file->handle);
	fseek(cast(FILE *)file->handle, 0, SEEK_SET);
	return result_size;
}

b32
gb_has_file_changed(gbFile *file)
{
	b32 result = false;
	gbFileTime last_write_time = gb_file_last_write_time(file->path);
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

	if (GetFileAttributesEx(gb_sprintf_va(filepath, va), GetFileExInfoStandard, &data))
		last_write_time = data.ftLastWriteTime;

	va_end(va);

	li.LowPart = last_write_time.dwLowDateTime;
	li.HighPart = last_write_time.dwHighDateTime;
	return cast(gbFileTime)li.QuadPart;
}

gb_inline b32
gb_copy_file(char const *existing_filename, char const *new_filename, b32 fail_if_exists)
{
	return CopyFile(existing_filename, new_filename, fail_if_exists);
}

gb_inline b32
gb_move_file(char const *existing_filename, char const *new_filename)
{
	return MoveFile(existing_filename, new_filename);
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
		result = file_stat.st_mtimespec.tv_sec;
	}

	va_end(va);

	return cast(gbFileTime)result;
}

gb_inline b32
gb_copy_file(char const *existing_filename, char const *new_filename, b32 fail_if_exists)
{
	GB_PANIC("TODO(bill): Implement");
	return false;
}

gb_inline b32
gb_move_file(char const *existing_filename, char const *new_filename)
{
	GB_PANIC("TODO(bill): Implement");
	return false;
}

#endif





gbFileContents
gb_read_entire_file_contents(gbAllocator a, b32 zero_terminate, char const *filepath, ...)
{
	gbFileContents result = {0};
	gbFile file = {0};
	char *path;
	va_list va;
	va_start(va, filepath);
	path = gb_sprintf_va(filepath, va);
	va_end(va);

	if (gb_open_file(&file, path)) {
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
		gb_close_file(&file);
	}

	return result;
}





gb_inline b32
gb_is_path_absolute(char const *path)
{
	b32 result = false;
	GB_ASSERT_NOT_NULL(path);
#if defined(GB_SYSTEM_WINDOWS)
	result == (gb_strlen(path) > 2) &&
	          gb_is_char_alpha(path[0]) &&
	          (path[1] == ':' && path[2] == GB_PATH_SEPARATOR);
#else
	result = (gb_strlen(path) > 0 && path[0] == GB_PATH_SEPARATOR);
#endif
	return result;
}

gb_inline b32 gb_is_path_relative(char const *path) { return !gb_is_path_absolute(path); }

gb_inline b32
gb_is_path_root(char const *path)
{
	b32 result = false;
	GB_ASSERT_NOT_NULL(path);
#if defined(GB_SYSTEM_WINDOWS)
	result = gb_is_path_absolute(path) && gb_strlen(path) == 3;
#else
	result = gb_is_path_absolute(path) && gb_strlen(path) == 1;
#endif
	return result;
}

gb_inline char const *
gb_path_base_name(char const *path)
{
	char const *ls;
	GB_ASSERT_NOT_NULL(path);
	ls = gb_last_occurence_of_char(path, '/');
	return (ls == NULL) ? path : ls+1;
}

gb_inline char const *
gb_path_extension(char const *path)
{
	char const *ld;
	GB_ASSERT_NOT_NULL(path);
	ld = gb_last_occurence_of_char(path, '.');
	return (ld == NULL) ? NULL : ld+1;
}

#if defined(GB_SYSTEM_WINDOWS)
gb_inline void gb_exit(u32 code) { ExitProcess(code); }
#else
gb_inline void gb_exit(u32 code) { exit(code); }
#endif


/***************************************************************
 *
 * DLL Handling
 *
 */

#if defined(GB_SYSTEM_WINDOWS)

gbDllHandle
gb_load_dll(char const *filepath, ...)
{
	gb_local_persist char buffer[512];
	va_list va;
	va_start(va, filepath);
	gb_snprintf_va(buffer, gb_size_of(buffer), filepath, va);
	va_end(va);
	return cast(gbDllHandle)LoadLibraryA(buffer);
}
gb_inline void      gb_unload_dll      (gbDllHandle dll)                        { FreeLibrary(cast(HMODULE)dll); }
gb_inline gbDllProc gb_dll_proc_address(gbDllHandle dll, char const *proc_name) { return cast(gbDllProc)GetProcAddress(cast(HMODULE)dll, proc_name); }

#else

gbDllHandle
gb_load_dll(char const *filepath, ...)
{
	gb_local_persist char buffer[512];
	va_list va;
	va_start(va, filepath);
	gb_snprintf_va(buffer, gb_size_of(buffer), filepath, va);
	va_end(va);
	/* TODO(bill): Should this be RTLD_LOCAL? */
	return cast(gbDllHandle)dlopen(buffer, RTLD_LAZY|RTLD_GLOBAL);
}

gb_inline void      gb_unload_dll      (gbDllHandle dll)                        { dlclose(dll); }
gb_inline gbDllProc gb_dll_proc_address(gbDllHandle dll, char const *proc_name) { return cast(gbDllProc)dlsym(dll, proc_name); }

#endif


/***************************************************************
 *
 * Time
 *
 */

#if defined(_MSC_VER)
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
			"0:                  \n"
			"\tmftbu   %0           \n"
			"\tmftb    %1           \n"
			"\tmftbu   %2           \n"
			"\tcmpw    %2,%0        \n"
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

	gb_inline void gb_sleep_ms(u32 ms) { Sleep(ms); }

	gb_inline void
	gb_get_system_date(gbDate *date)
	{
		SYSTEMTIME st = {0};
		GetSystemTime(&st);
		date->year         = st.wYear;
		date->month        = st.wMonth;
		date->day_of_week  = st.wDayOfWeek;
		date->day          = st.wDay;
		date->hour         = st.wHour;
		date->minute       = st.wMinute;
		date->second       = st.wSecond;
		date->milliseconds = st.wMilliseconds;
	}

	gb_inline void
	gb_get_local_date(gbDate *date)
	{
		SYSTEMTIME st = {0};
		GetLocalTime(&st);
		date->year         = st.wYear;
		date->month        = st.wMonth;
		date->day_of_week  = st.wDayOfWeek;
		date->day          = st.wDay;
		date->hour         = st.wHour;
		date->minute       = st.wMinute;
		date->second       = st.wSecond;
		date->milliseconds = st.wMilliseconds;
	}

#else

	gb_global f64 gb__timebase  = 0.0;
	gb_global u64 gb__timestart = 0;

	gb_inline f64
	gb_time_now(void)
	{
		struct timespec t;
		f64 result;

		if (gb__timestart) {
			mach_timebase_info_data_t tb = {0};
			mach_timebase_info(&tb);
			gb__timebase = tb.numer;
			gb__timebase /= tb.denom;
			gb__timestart = mach_absolute_time();
		}

		result = (mach_absolute_time() - gb__timestart) * gb__timebase;
		return result;
	}

	gb_inline void
	gb_sleep_ms(u32 ms)
	{
		timespec req = {cast(time_t)ms/1000, cast(long)((ms%1000)*1000000)};
		timespec rem = {0, 0};
		nanosleep(&req, &rem);
	}


	gb_inline void
	gb_get_system_date(gbDate *date)
	{
		GB_PANIC("TODO(bill): Implement");
	}

	gb_inline void
	gb_get_local_date(gbDate *date)
	{
		GB_PANIC("TODO(bill): Implement");
	}

#endif



/***************************************************************
 *
 * Miscellany
 *
 */

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

gb_inline i32
gb_chdir(char const *path)
{
#if defined(_MSC_VER)
	return _chdir(path);
#else
	return chdir(path);
#endif
}

gb_inline void
gb_get_working_cmd(char *buffer, isize len)
{
#if defined(_MSC_VER)
	_getcwd(buffer, cast(int)len);
#else
	getcwd(buffer, len);
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
	       ((i&0x00ff0000)>>8)  | ((i&0x00ff0000)<<8);
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

#if defined(__GCC__) || defined(__GNUC__)
#pragma GCC diagnostic pop
#endif


#if defined(__cplusplus)
}
#endif
#endif /* GB_IMPLEMENTATION */
