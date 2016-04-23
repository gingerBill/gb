/* gb.h - v0.02a - Ginger Bill's C Helper Library - public domain
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
	Prefer // Comments
	Never use _t suffix for types (I think they are stupid...)


Version History:
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
#define GB_DLL_EXPORT __declspec(dllexport)
#endif

#ifndef GB_DLL_IMPORT
#define GB_DLL_IMPORT __declspec(dllimport)
#endif


// NOTE(bill): Redefine for DLL, etc.
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

// TODO(bill): Check if this works on clang
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
	// NOTE(bill): Token pasting madness!!
	#define GB_STATIC_ASSERT2(cond, line) GB_STATIC_ASSERT3(cond, static_assertion_at_line_##line)
	#define GB_STATIC_ASSERT1(cond, line) GB_STATIC_ASSERT2(cond, line)
	#define GB_STATIC_ASSERT(cond)        GB_STATIC_ASSERT1(cond, __LINE__)
#endif



////////////////////////////////////////////////////////////////
//
// Headers
//
//

#if defined(_WIN32) && !defined(__MINGW32__)
	#ifndef _CRT_SECURE_NO_WARNINGS
	#define _CRT_SECURE_NO_WARNINGS
	#endif
#endif

#include <stdarg.h>
#include <stddef.h>
#include <stdio.h> // TODO(bill): Remove and replace with OS Specific stuff
#include <string.h> // NOTE(bill): For memcpy, memmove, memcmp, etc.

#if defined(GB_SYSTEM_WINDOWS)
	#define NOMINMAX            1
	#define WIN32_LEAN_AND_MEAN 1
	#define VC_EXTRALEAN        1
	#include <windows.h>
#else
#error Add OS Specific headers
#endif


#if !defined(GB_NO_STDLIB)
#include <stdlib.h>
#endif

#ifndef gb_malloc
#define gb_malloc(sz) malloc(sz)
#endif

#ifndef gb_mfree
#define gb_mfree(ptr) free(ptr)
#endif


////////////////////////////////////////////////////////////////
//
// Base Types
//
//

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

typedef u16 char16;
typedef u32 char32;

// NOTE(bill): I think C99 and C++ `bool` is stupid for numerous reasons but there are too many
// to write in this small comment.
typedef i8  b8;
typedef i16 b16;
typedef i32 b32;

// NOTE(bill): Get true and false
#if !defined(__cplusplus)
	#if defined(_MSC_VER) && _MSC_VER <= 1800
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



#if !defined(__cplusplus) && defined(_MSC_VER) && _MSC_VER <= 1800
	#define inline __inline
#endif

#if !defined(gb_inline)
	#if defined(_MSC_VER)
		#define gb_restrict __restrict
	#else
		#define gb_restrict restrict
	#endif
#endif

// TODO(bill): Should force inline be a separate keyword and gb_inline be inline?
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

// NOTE(bill): Easy to grep
// NOTE(bill): Not needed in macros
#ifndef cast
#define cast(Type) (Type)
#endif


// NOTE(bill): Because a signed sizeof is more useful
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
		// NOTE(bill): Fucking Templates!
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

// NOTE(bill): I do which I had a type_of that was portable
#ifndef gb_swap
#define gb_swap(Type, a, b) do { Type tmp = (a); (a) = (b); (b) = tmp; } while (0)
#endif

// NOTE(bill): Because static means 3/4 different things in C/C++. Great design (!)
#ifndef gb_global
#define gb_global        static // Global variables
#define gb_internal      static // Internal linkage
#define gb_local_persist static // Local Persisting variables
#endif


#ifndef gb_unused
#define gb_unused(x) ((void)(gb_size_of(x)))
#endif






////////////////////////////////////////////////////////////////
//
// Defer statement
// Akin to D's SCOPE_EXIT or
// similar to Go's defer but scope-based
//
// NOTE: C++11 (and above) only!
//
#if defined(__cplusplus)
extern "C++" {
namespace gb {

	// NOTE(bill): Stupid fucking templates
	template <typename T> struct RemoveReference       { typedef T Type; };
	template <typename T> struct RemoveReference<T &>  { typedef T Type; };
	template <typename T> struct RemoveReference<T &&> { typedef T Type; };

	// NOTE(bill): "Move" semantics - invented because the C++ committee are idiots (as a collective not as indiviuals (well a least some aren't))
	template <typename T> inline T &&forward(typename RemoveReference<T>::Type &t)  { return static_cast<T &&>(t); }
	template <typename T> inline T &&forward(typename RemoveReference<T>::Type &&t) { return static_cast<T &&>(t); }
	template <typename T> inline T &&move(T &&t) { return static<typename RemoveReference<T>::Type &&>(t); }
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
	#define defer(code)      auto GB_DEFER_3(_defer_) = priv_defer_func([&](){code;})
	#endif
} // namespace gb
}

	// Example
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


////////////////////////////////
//
// Macro Fun!
//
//

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


////////////////////////////////
//
// Debug
//
//


#ifndef GB_DEBUG_TRAP
	#if defined(_MSC_VER)
		#define GB_DEBUG_TRAP() __debugbreak()
	#else
		#define GB_DEBUG_TRAP() __builtin_trap()
	#endif
#endif

// TODO(bill): This relies upon variadic macros which are not supported in MSVC 2003 and below, check for it if needed
#ifndef GB_ASSERT_MSG
#define GB_ASSERT_MSG(cond, msg, ...) do { \
	if (!(cond)) { \
		gb_assert_handler(#cond, __FILE__, cast(i64)__LINE__, msg, ##__VA_ARGS__); \
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

// NOTE(bill): Things that shouldn't happen
#ifndef GB_PANIC
#define GB_PANIC(msg, ...) GB_ASSERT_MSG(0, msg, ##__VA_ARGS__)
#endif

GB_DEF void gb_assert_handler(char const *condition, char const *file, i64 line, char const *msg, ...);





////////////////////////////////
//
// Printing
//
//

// Some compilers support applying printf-style warnings to user functions.
#if defined(__clang__) || defined(__GNUC__)
#define GB_PRINTF_ARGS(FMT) __attribute__((format(printf, FMT, (FMT+1))))
#else
#define GB_PRINTF_ARGS(FMT)
#endif

// TODO(bill): Should I completely rename these functions as they are a little weird to begin with?

GB_DEF i32   gb_printf(char const *fmt, ...) GB_PRINTF_ARGS(1);
GB_DEF i32   gb_printf_va(char const *fmt, va_list va);
GB_DEF i32   gb_fprintf(FILE *f, char const *fmt, ...) GB_PRINTF_ARGS(2);
GB_DEF i32   gb_fprintf_va(FILE *f, char const *fmt, va_list va);
GB_DEF char *gb_sprintf(char const *fmt, ...) GB_PRINTF_ARGS(1); // NOTE(bill): A locally persisting buffer is used internally
GB_DEF char *gb_sprintf_va(char const *fmt, va_list va);         // NOTE(bill): A locally persisting buffer is used internally
GB_DEF i32   gb_snprintf(char *str, isize n, char const *fmt, ...) GB_PRINTF_ARGS(3);
GB_DEF i32   gb_snprintf_va(char *str, isize n, char const *fmt, va_list va);

GB_DEF i32 gb_println(char const *str);
GB_DEF i32 gb_fprintln(FILE *f, char const *str);



////////////////////////////////////////////////////////////////
//
// Memory
//
//

#ifndef gb_align_to
#define gb_align_to(value, alignment) (((value) + ((alignment)-1)) & ~((alignment) - 1))
#endif

#ifndef gb_is_power_of_two
#define gb_is_power_of_two(x) ((x) != 0) && !((x) & ((x)-1))
#endif

GB_DEF void *gb_align_forward(void *ptr, isize alignment);
GB_DEF void *gb_pointer_add(void *ptr, isize bytes);
GB_DEF void *gb_pointer_sub(void *ptr, isize bytes);

GB_DEF void gb_zero_size(void *ptr, isize size);

#ifndef gb_zero_struct
#define gb_zero_struct(t) gb_zero_size((t), gb_size_of(*(t))) // NOTE(bill): Pass pointer of struct
#define gb_zero_array(a, count) gb_zero_size((a), gb_size_of((a)[0])*count)
#endif

GB_DEF void *gb_memcopy(void *dest, void const *source, isize size);
GB_DEF void *gb_memmove(void *dest, void const *source, isize size);
GB_DEF void *gb_memset(void *data, u8 byte_value, isize size);

// NOTE(bill): Very similar to doing `*cast(T *)(&u)`
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


// Atomics
typedef struct gbAtomic32 { i32 volatile value; } gbAtomic32;
typedef struct gbAtomic64 { i64 volatile value; } gbAtomic64;

GB_DEF i32  gb_atomic32_load(gbAtomic32 const volatile *a);
GB_DEF void gb_atomic32_store(gbAtomic32 volatile *a, i32 value);
GB_DEF i32  gb_atomic32_compare_exchange_strong(gbAtomic32 volatile *a, i32 expected, i32 desired);
GB_DEF i32  gb_atomic32_exchanged(gbAtomic32 volatile *a, i32 desired);
GB_DEF i32  gb_atomic32_fetch_add(gbAtomic32 volatile *a, i32 operand);
GB_DEF i32  gb_atomic32_fetch_and(gbAtomic32 volatile *a, i32 operand);
GB_DEF i32  gb_atomic32_fetch_or(gbAtomic32 volatile *a, i32 operand);

GB_DEF i64  gb_atomic64_load(gbAtomic64 const volatile *a);
GB_DEF void gb_atomic64_store(gbAtomic64 volatile *a, i64 value);
GB_DEF i64  gb_atomic64_compare_exchange_strong(gbAtomic64 volatile *a, i64 expected, i64 desired);
GB_DEF i64  gb_atomic64_exchanged(gbAtomic64 volatile *a, i64 desired);
GB_DEF i64  gb_atomic64_fetch_add(gbAtomic64 volatile *a, i64 operand);
GB_DEF i64  gb_atomic64_fetch_and(gbAtomic64 volatile *a, i64 operand);
GB_DEF i64  gb_atomic64_fetch_or(gbAtomic64 volatile *a, i64 operand);


typedef struct gbMutex {
#if defined(GB_SYSTEM_WINDOWS)
	HANDLE win32_handle;
#else
	pthread_mutex_t posix_handle;
#endif
} gbMutex;

GB_DEF void gb_mutex_init(gbMutex *m);
GB_DEF void gb_mutex_destroy(gbMutex *m);
GB_DEF void gb_mutex_lock(gbMutex *m);
GB_DEF b32  gb_mutex_try_lock(gbMutex *m);
GB_DEF void gb_mutex_unlock(gbMutex *m);

// NOTE(bill): If you wanted a Scoped Mutex in C++, why not use the defer() construct?
// No need for a silly wrapper class
#if 0
gbMutex m = {0};
gb_mutex_init(&m);
{
	gb_mutex_lock(&m);
	defer (gb_mutex_unlock(&m));

	// Do whatever
}
#endif



typedef struct gbSemaphore {
#if defined(GB_SYSTEM_WINDOWS)
	HANDLE win32_handle;
#else
	gbMutex        mutex;
	pthread_cond_t cond;
	i32            count;
#endif
} gbSemaphore;

GB_DEF void gb_semaphore_init(gbSemaphore *s);
GB_DEF void gb_semaphore_destroy(gbSemaphore *s);
GB_DEF void gb_semaphore_post(gbSemaphore *s, i32 count);
GB_DEF void gb_semaphore_wait(gbSemaphore *s);




#define GB_THREAD_PROC(name) void name(void *data)
;typedef GB_THREAD_PROC(gbThreadProc);

typedef struct gbThread {
#if defined(GB_SYSTEM_WINDOWS)
	HANDLE win32_handle;
#else
	pthread_t posix_handle;
#endif

	gbThreadProc *proc;
	void *data;

	gbSemaphore semaphore;
	isize stack_size;
	b32 is_running;
} gbThread;

GB_DEF void gb_thread_init(gbThread *t);
GB_DEF void gb_thread_destory(gbThread *t);
GB_DEF void gb_thread_start(gbThread *t, gbThreadProc *proc, void *data);
GB_DEF void gb_thread_start_with_stack(gbThread *t, gbThreadProc *proc, void *data, isize stack_size);
GB_DEF void gb_thread_join(gbThread *t);
GB_DEF b32 gb_thread_is_running(gbThread const *t);
GB_DEF u32 gb_thread_current_id(void);

////////////////////////////////////////////////////////////////
//
// Virtual Memory
//
//

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


////////////////////////////////////////////////////////////////
//
// Custom Allocation
//
//

typedef enum gbAllocationType {
	GB_ALLOCATION_ALLOC,
	GB_ALLOCATION_FREE,
	GB_ALLOCATION_FREE_ALL,
	GB_ALLOCATION_RESIZE,
} gbAllocationType;

// NOTE(bill): This is useful so you can define an allocator of the same type and parameters
#define GB_ALLOCATOR_PROC(name)                         \
void *name(void *allocator_data, gbAllocationType type, \
           isize size, isize alignment,                 \
           void *old_memory, isize old_size,            \
           u64 options)
;typedef GB_ALLOCATOR_PROC(gbAllocatorProc);

typedef struct gbAllocator {
	gbAllocatorProc *proc;
	void *data;
} gbAllocator;

#ifndef GB_DEFAULT_MEMORY_ALIGNMENT
#define GB_DEFAULT_MEMORY_ALIGNMENT 4
#endif

GB_DEF void *gb_alloc_align(gbAllocator a, isize size, isize alignment);
GB_DEF void *gb_alloc(gbAllocator a, isize size);
GB_DEF void  gb_free(gbAllocator a, void *ptr);
GB_DEF void  gb_free_all(gbAllocator a);
GB_DEF void *gb_resize(gbAllocator a, void *ptr, isize old_size, isize new_size);
GB_DEF void *gb_resize_align(gbAllocator a, void *ptr, isize old_size, isize new_size, isize alignment);

GB_DEF void *gb_alloc_copy(gbAllocator a, void const *src, isize size);
GB_DEF void *gb_alloc_copy_align(gbAllocator a, void const *src, isize size, isize alignment);

GB_DEF char *gb_alloc_cstring(gbAllocator a, char const *str);


// NOTE(bill): These are very useful and the type case has saved me from numerous bugs
#ifndef gb_alloc_struct
#define gb_alloc_struct(allocator, Type)       (Type *)gb_alloc_align(allocator, gb_size_of(Type))
#define gb_alloc_array(allocator, Type, count) (Type *)gb_alloc(allocator, gb_size_of(Type) * (count))
#endif





GB_DEF gbAllocator gb_heap_allocator(void);
GB_DEF GB_ALLOCATOR_PROC(gb_heap_allocator_proc);





typedef struct gbArena {
	gbAllocator backing;
	void *physical_start;
	isize total_size;
	isize total_allocated;
	u32 temp_count;
} gbArena;

GB_DEF void gb_arena_init_from_memory(gbArena *arena, void *start, isize size);
GB_DEF void gb_arena_init_from_allocator(gbArena *arena, gbAllocator backing, isize size);
GB_DEF void gb_arena_init_subarena(gbArena *arena, gbArena *parent_arena, isize size);
GB_DEF void gb_arena_free(gbArena *arena);

GB_DEF isize gb_arena_alignment_of(gbArena *arena, isize alignment);
GB_DEF isize gb_arena_size_remaining(gbArena *arena, isize alignment);
GB_DEF void  gb_arena_check(gbArena *arena);


GB_DEF gbAllocator gb_arena_allocator(gbArena *arena);
GB_DEF GB_ALLOCATOR_PROC(gb_arena_allocator_proc);



typedef struct gbTempArenaMemory {
	gbArena *arena;
	isize original_count;
} gbTempArenaMemory;

GB_DEF gbTempArenaMemory gb_temp_arena_memory_begin(gbArena *arena);
GB_DEF void gb_temp_arena_memory_end(gbTempArenaMemory tmp_mem);









typedef struct gbPool {
	gbAllocator backing;

	void *physical_start;
	void *free_list;

	isize block_size;
	isize block_align;
	isize total_size;
} gbPool;

GB_DEF void gb_pool_init(gbPool *pool, gbAllocator backing, isize num_blocks, isize block_size);
GB_DEF void gb_pool_init_align(gbPool *pool, gbAllocator backing, isize num_blocks, isize block_size, isize block_align);
GB_DEF void gb_pool_free(gbPool *pool);


GB_DEF gbAllocator gb_pool_allocator(gbPool *pool);
GB_DEF GB_ALLOCATOR_PROC(gb_pool_allocator_proc);







////////////////////////////////////////////////////////////////
//
// Char Functions
//
//

GB_DEF char gb_char_to_lower(char c);
GB_DEF char gb_char_to_upper(char c);
GB_DEF b32  gb_char_is_space(char c);
GB_DEF b32  gb_char_is_digit(char c);
GB_DEF b32  gb_char_is_hex_digit(char c);
GB_DEF b32  gb_char_is_alpha(char c);
GB_DEF b32  gb_char_is_alphanumeric(char c);


//

GB_DEF void gb_to_lower(char *str);
GB_DEF void gb_to_upper(char *str);

GB_DEF isize  gb_strlen(char const *str);
GB_DEF i32    gb_strcmp(char const *s1, char const *s2);
GB_DEF char * gb_strncpy(char *dest, char const *source, isize len);
GB_DEF i32    gb_strncmp(char const *s1, char const *s2, isize len);

GB_DEF char const *gb_char_first_occurence(char const *s1, char c);
GB_DEF char const *gb_char_last_occurence(char const *s1, char c);

GB_DEF void gb_cstr_concat(char *dest, isize dest_len,
                           char const *src_a, isize src_a_len,
                           char const *src_b, isize src_b_len);


////////////////////////////////////////////////////////////////
//
// Windows UTF-8 Handling
//
//
// Windows doesn't handle 8 bit filenames well ('cause Micro$hit)

GB_DEF char16 *gb_utf8_to_utf16(char16 *buffer, char *str, isize len);
GB_DEF char *  gb_utf16_to_utf8(char *buffer, char16 *str, isize len);


////////////////////////////////////////////////////////////////
//
// gbString - C Read-Only-Compatible
//
//


// Pascal like strings in C
typedef char *gbString;


// This is stored at the beginning of the string
// NOTE(bill): If you only need a small string, just use a standard c string or change the size
typedef struct gbStringHeader {
	gbAllocator allocator;
	isize length;
	isize capacity;
} gbStringHeader;

#define GB_STRING_HEADER(str) (cast(gbStringHeader *)(str) - 1)

GB_DEF gbString gb_string_make(gbAllocator a, char const *str);
GB_DEF gbString gb_string_make_length(gbAllocator a, void const *str, isize num_bytes);
GB_DEF void     gb_string_free(gbString str);

GB_DEF gbString gb_string_duplicate(gbAllocator a, gbString const str);

GB_DEF isize gb_string_length(gbString const str);
GB_DEF isize gb_string_capacity(gbString const str);
GB_DEF isize gb_string_available_space(gbString const str);

GB_DEF void gb_string_clear(gbString str);

GB_DEF gbString gb_string_append_string(gbString str, gbString const other);
GB_DEF gbString gb_string_append_string_length(gbString str, void const *other, isize num_bytes);
GB_DEF gbString gb_string_append_cstring(gbString str, char const *other);

GB_DEF gbString gb_string_set(gbString str, char const *cstr);

GB_DEF gbString gb_string_make_space_for(gbString str, isize add_len);
GB_DEF isize gb_string_allocation_size(gbString const str);

GB_DEF b32 gb_strings_are_equal(gbString const lhs, gbString const rhs);

GB_DEF gbString gb_string_trim(gbString str, char const *cut_set);
GB_DEF gbString gb_string_trim_space(gbString str); /* Whitespace ` \t\r\n\v\f` */



//////////////////////////////////////////////////////////////////
//
// Fixed Capacity Buffer (POD Types)
//
//

#ifndef GB_BUFFER_TYPE
#define GB_BUFFER_TYPE
#endif

#define gbBuffer(Type) struct { isize count, capacity; Type *data; }

typedef gbBuffer(u8) gbByteBuffer;

#define gb_buffer_init(x, allocator, cap) do {                     \
	void **data = cast(void **)&((x)->data);                       \
	gb_zero_struct(x);                                             \
	(x)->capacity = (cap);                                         \
	*data = gb_alloc((allocator), (cap)*gb_size_of((x)->data[0])); \
} while (0)

#define gb_buffer_free(x, allocator) do { gb_free(allocator, (x)->data); } while (0)

#define gb_buffer_append(x, item)    do { (x)->data[(x)->count++] = item; } while (0)

#define gb_buffer_appendv(x, items, item_count) do {                                \
	GB_ASSERT(gb_size_of((items)[0]) == gb_size_of((x)->data[0]));                  \
	GB_ASSERT((x)->count+item_count <= (x)->capacity);                              \
	gb_memcopy((x)->data[a->count], (items), gb_size_of((x)->data[0])*(item_count)); \
	(x)->count += (item_count);                                                     \
} while (0)

#define gb_buffer_pop(x)   do { GB_ASSERT((x)->count > 0); (x)->count--; } while (0)
#define gb_buffer_clear(x) do { (x)->count = 0; } while (0)



//////////////////////////////////////////////////////////////////
//
// Dynamic Array (POD Types)
//
//

// NOTE(bill): I know this is a macro hell but C is an old (and shit) language with no proper arrays
// Also why the fuck not?! It fucking works! And it has custom allocation, which is already better than C++!

// NOTE(bill): Typedef every array or you get anonymous structures everywhere!
// e.g. typedef gbArray(int) gb_Int_Array;
#ifndef GB_ARRAY_TYPE
#define GB_ARRAY_TYPE

#define gbArray(Type) struct { gbAllocator allocator; isize count, capacity; Type *data; }


typedef gbArray(void) gbVoidArray; // NOTE(bill): Useful for generic stuff

// Available Procedures for gbArray(Type)
//     gb_array_init
//     gb_array_free
//     gb_array_set_capacity
//     gb_array_grow
//     gb_array_append
//     gb_array_appendv
//     gb_array_pop
//     gb_array_clear
//     gb_array_resize
//     gb_array_reserve
//

#define gb_array_init(x, allocator_) do { gb_zero_struct(x); (x)->allocator = allocator_; } while (0)

#define gb_array_free(x) do {           \
	if ((x)->allocator.proc) {          \
		gbAllocator a = (x)->allocator; \
		gb_free(a, (x)->data);          \
		gb_array_init((x), a);          \
	}                                   \
} while (0)

#define gb_array_set_capacity(array, capacity) gb__array_set_capacity((array), (capacity), gb_size_of((array)->data[0]))
// NOTE(bill): Do not use the thing below directly, use the macro
GB_DEF void gb__array_set_capacity(void *array, isize capacity, isize element_size);

#ifndef GB_ARRAY_GROW_FORMULA
#define GB_ARRAY_GROW_FORMULA(x) (2*(x) + 8)
#endif

// TODO(bill): Decide on a decent growing formula for gbArray
#define gb_array_grow(x, min_capacity) do {                \
	isize capacity = GB_ARRAY_GROW_FORMULA((x)->capacity); \
	if (capacity < (min_capacity))                         \
		capacity = (min_capacity);                         \
	gb_array_set_capacity(x, capacity);                    \
} while (0)


#define gb_array_append(x, item) do { \
	if ((x)->capacity < (x)->count+1) \
		gb_array_grow(x, 0);          \
	(x)->data[(x)->count++] = (item); \
} while (0)

#define gb_array_appendv(x, items, item_count) do {                                   \
	GB_ASSERT(gb_size_of((items)[0]) == gb_size_of((x)->data[0]));                    \
	if ((x)->capacity < (x)->count+(item_count))                                      \
		gb_array_grow(x, (x)->count+(item_count));                                    \
	gb_memcopy((x)->data[(x)->count], (items), gb_size_of((x)->data[0])*(item_count)); \
	(x)->count += (item_count);                                                       \
} while (0)



#define gb_array_pop(x) do { GB_ASSERT((x)->count > 0); (x)->count--; } while (0)
#define gb_array_clear(x) do { (x)->count = 0; } while (0)

#define gb_array_resize(x, count) do {    \
	if ((x)->capacity < (count))          \
		gb_array_grow(x, count);          \
	(x)->count = (count);                 \
} while (0)


#define gb_array_reserve(x, new_capacity) do {  \
	if ((x)->capacity < (new_capacity))         \
		gb_array_set_capacity(x, new_capacity); \
} while (0)


#endif /* GB_ARRAY_TYPE */








////////////////////////////////////////////////////////////////
//
//
// Hash_Table (POD Types)
//
//

#if 0
// TODO(bill): Hash Table and make it decent!!!

// NOTE(bill): All keys are u64
#ifndef GB_HASH_TABLE_TYPE
#define GB_HASH_TABLE_TYPE

typedef struct gbHashTableEntry {
	u64 key;
	isize value;
	isize next;
} gbHashTableEntry;

#define GB_HASH_TABLE_HEADER             \
	gbArray(isize) hashes;               \
	gbArray(gbHashTableEntry) entries  \

typedef struct gbHashTableHeader {
	GB_HASH_TABLE_HEADER;
} gbHashTableHeader;

#define gbHashTable(Type) struct { \
	GB_HASH_TABLE_HEADER;           \
	gbArray(Type) data;             \
}

typedef gbHashTable(void) gbHashTableVoid; // Generic Type

#define gb_hash_table_init(h, allocator) do { \
	gb_array_init((h)->hashes,  allocator);   \
	gb_array_init((h)->entries, allocator);   \
	gb_array_init((h)->data,    allocator);   \
} while (0)

#define gb_hash_table_free(h) do { \
	gb_free(&(h)->hashes);         \
	gb_free(&(h)->entries);        \
	gb_free(&(h)->data);           \
} while (0)

#define gb_hash_table_header(h) (cast(gbHashTableHeader *)(h))


// TODO(bill): Should the use have to pass the Hash Table and their array?




/* TODO(bill): Hash_Table Procs
	gb_hash_table_init(h, allocator)
	gb_hash_table_free(h)

	gb_hash_table_has(h, key) // Return false/true
	gb_hash_table_get(h, key) // Return entries index
	gb_hash_table_set(h, key, value)
	gb_hash_table_remove(h, key)
	gb_hash_table_reserve(h, capacity)
	gb_hash_table_clear(h)

*/



#endif

#endif /* GB_HASH_TABLE_TYPE */



////////////////////////////////////////////////////////////////
//
// File Handling
//
//
typedef u64 gbFileTime;

typedef enum gbFileType {
	GB_FILE_TYPE_READ  = 1,
	GB_FILE_TYPE_WRITE = 2,
} gbFileType;

typedef struct gbFile {
	void *handle; // File to fread/fwrite
	char *path;
	i64 size;
	b32 is_open;
	gbFileType type;
	gbFileTime last_write_time;
} gbFile;

typedef struct gbFileContents {
	void *data;
	isize size;
} gbFileContents;


GB_DEF b32 gb_file_create(gbFile *file, char const *filepath, ...); // TODO(bill): Give file permissions
GB_DEF b32 gb_file_open(gbFile *file, char const *filepath, ...);
GB_DEF b32 gb_file_close(gbFile *file);
GB_DEF b32 gb_file_read_at(gbFile *file, void *buffer, isize size, i64 offset);
GB_DEF b32 gb_file_write_at(gbFile *file, void const *buffer, isize size, i64 offset);
GB_DEF i64 gb_file_size(gbFile *file);
GB_DEF b32 gb_file_has_changed(gbFile *file);

GB_DEF gbFileTime gb_file_last_write_time(char const *filepath, ...);


GB_DEF b32 gb_file_copy(char const *existing_filename, char const *new_filename, b32 fail_if_exists);
GB_DEF b32 gb_file_move(char const *existing_filename, char const *new_filename);


GB_DEF gbFileContents gb_read_entire_file_contents(gbAllocator a, b32 zero_terminate, char const *filepath, ...);


#ifndef GB_PATH_SEPARATOR
	#if defined(GB_SYSTEM_WINDOWS)
		#define GB_PATH_SEPARATOR '\\'
	#else
		#define GB_PATH_SEPARATOR '/'
	#endif
#endif

GB_DEF b32 gb_path_is_absolute(char const *path);
GB_DEF b32 gb_path_is_relative(char const *path);
GB_DEF b32 gb_path_is_root(char const *path);
GB_DEF char const *gb_path_base_name(char const *path);
GB_DEF char const *gb_path_extension(char const *path);


GB_DEF void gb_exit(u32 code);


////////////////////////////////////////////////////////////////
//
// DLL Handling
//
//

typedef void *gbDllHandle;
typedef void (*gbDllProc)(void);

GB_DEF gbDllHandle gb_dll_load(char const *filepath, ...);
GB_DEF void        gb_dll_unload(gbDllHandle dll);
GB_DEF gbDllProc   gb_dll_proc_address(gbDllHandle dll, char const *proc_name);


////////////////////////////////////////////////////////////////
//
// Time
//
//

typedef struct gbDate {
	i32 year;
	i32 month;        // 1 - Janurary, ... 12 - December
	i32 day;          // 1 - 31
	i32 day_of_week;  // 0 - Sunday, ... 6 - Saturday
	i32 hour;         // 0 - 23
	i32 minute;       // 0 - 59
	i32 second;       // 0 - 60 (leap seconds)
	i32 milliseconds; // 0 - 999
} gbDate;


GB_DEF u64 gb_rdtsc(void);
GB_DEF f64 gb_time_now(void);

GB_DEF void gb_get_system_date(gbDate *date);
GB_DEF void gb_get_local_date(gbDate *date);



////////////////////////////////////////////////////////////////
//
// Colour Type
// It's quite useful
//

typedef union gbColour {
	u32 rgba; // NOTE(bill): 0xaabbggrr
	struct { u8 r, g, b, a; };
	u8 e[4];
} gbColour;
GB_STATIC_ASSERT(gb_size_of(gbColour) == gb_size_of(u32));

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



#if defined(__cplusplus)
}
#endif

#endif /* GB_INCLUDE_GB_H */






////////////////////////////////////////////////////////////////
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
// Implementation
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
////////////////////////////////////////////////////////////////

#if defined(GB_IMPLEMENTATION) && !defined(GB_IMPLEMENTATION_DONE)
#define GB_IMPLEMENTATION_DONE

#if defined(__cplusplus)
extern "C" {
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
	res = gb_fprintf_va(stdout, fmt, va);
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
	res = vsnprintf(str, n, fmt, va)
#endif
	if (n) str[n-1] = 0;
	// NOTE(bill): Unix returns length output would require, Windows returns negative when truncated.
	return (res >= n || res < 0) ? -1 : res;
}


gb_inline i32 gb_println(char const *str) { return gb_fprintln(stdout, str); }

gb_inline i32
gb_fprintln(FILE *f, char const *str)
{
	i32 res;
	res = gb_fprintf(f, str);
	gb_fprintf(f, "\n");
	res++;
	return res;
}





void
gb_assert_handler(char const *condition, char const *file, i64 line, char const *msg, ...)
{
	gb_fprintf(stderr, "%s:%lld: Assert Failure: ", file, cast(long long)line);
	if (condition)
		gb_fprintf(stderr, "`%s` ", condition);

	if (msg) {
		va_list va;
		va_start(va, msg);
		gb_fprintf(stderr, msg, va);
		va_end(va);
	}

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


gb_inline void *gb_memcopy(void *dest, void const *source, isize size)  { return memcpy(dest, source, size);     }
gb_inline void *gb_memmove(void *dest, void const *source, isize size) { return memmove(dest, source, size);    }
gb_inline void *gb_memset(void *data, u8 byte_value, isize size)       { return memset(data, byte_value, size); }





gb_inline void *gb_alloc_align(gbAllocator a, isize size, isize alignment) { return a.proc(a.data, GB_ALLOCATION_ALLOC, size, alignment, NULL, 0, 0); }
gb_inline void *gb_alloc(gbAllocator a, isize size)                        { return gb_alloc_align(a, size, GB_DEFAULT_MEMORY_ALIGNMENT); }
gb_inline void  gb_free(gbAllocator a, void *ptr)                          { a.proc(a.data, GB_ALLOCATION_FREE, 0, 0, ptr, 0, 0); }
gb_inline void  gb_free_all(gbAllocator a)                                 { a.proc(a.data, GB_ALLOCATION_FREE_ALL, 0, 0, NULL, 0, 0); }
gb_inline void *gb_resize(gbAllocator a, void *ptr, isize old_size, isize new_size)        { return gb_resize_align(a, ptr, old_size, new_size, GB_DEFAULT_MEMORY_ALIGNMENT); }
gb_inline void *gb_resize_align(gbAllocator a, void *ptr, isize old_size, isize new_size, isize alignment) { return a.proc(a.data, GB_ALLOCATION_RESIZE, new_size, alignment, ptr, old_size, 0); };

gb_inline void *gb_alloc_copy(gbAllocator a, void const *src, isize size) { return gb_memcopy(gb_alloc(a, size), src, size); }
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





////////////////////////////////////////////////////////////////
//
// Concurrency
//

#if defined(_MSC_VER)
gb_inline i32
gb_atomic32_load(gbAtomic32 const volatile *a)
{
	return a->value;
}
gb_inline void
gb_atomic32_store(gbAtomic32 volatile *a, i32 value)
{
	a->value = value;
}

gb_inline i32
gb_atomic32_compare_exchange_strong(gbAtomic32 volatile *a, i32 expected, i32 desired)
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
	// NOTE(bill): The most compatible way to get an atomic 64-bit load on x86 is with cmpxchg8b
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
	// NOTE(bill): The most compatible way to get an atomic 64-bit store on x86 is with cmpxchg8b
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
gb_atomic64_compare_exchange_strong(gbAtomic64 volatile *a, i64 expected, i64 desired)
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
		i64 original = _InterlockedExchange64(cast(i64 volatile *)a, expected + operand, expected);
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
	i64 expected = object->nonatomic;
	for (;;) {
		i64 original = _InterlockedCompareExchange64(cast(i64 volatile *)a, expected | operand, expected);
		if (original == expected)
			return original;
		expected = original;
	}
#endif
}

#else
#error Implement atomics for this platform
#endif





gb_inline void gb_mutex_init(gbMutex *m) { m->win32_handle = CreateMutex(0, false, 0); }
gb_inline void gb_mutex_destroy(gbMutex *m) { CloseHandle(m->win32_handle); }
gb_inline void gb_mutex_lock(gbMutex *m) { WaitForSingleObject(m->win32_handle, INFINITE); }
gb_inline b32  gb_mutex_try_lock(gbMutex *m) { return WaitForSingleObject(m->win32_handle, 0) == WAIT_OBJECT_0; }
gb_inline void gb_mutex_unlock(gbMutex *m) { ReleaseMutex(m->win32_handle); }




gb_inline void
gb_semaphore_init(gbSemaphore *s)
{
	s->win32_handle = CreateSemaphore(NULL, 0, I32_MAX, NULL);
	GB_ASSERT_MSG(s->win32_handle != NULL, "CreateSemaphore: GetLastError = %d", GetLastError());
}

gb_inline void
gb_semaphore_destroy(gbSemaphore *s)
{
	BOOL err = CloseHandle(s->win32_handle);
	GB_ASSERT_MSG(err != 0, "CloseHandle: GetLastError = %d", GetLastError());
}

gb_inline void
gb_semaphore_post(gbSemaphore *s, i32 count)
{
	BOOL err = ReleaseSemaphore(s->win32_handle, count, NULL);
	GB_ASSERT_MSG(err != 0, "ReleaseSemaphore: GetLastError = %d", GetLastError());
}

gb_inline void
gb_semaphore_wait(gbSemaphore *s)
{
	DWORD result = WaitForSingleObject(s->win32_handle, INFINITE);
	GB_ASSERT_MSG(result == WAIT_OBJECT_0, "WaitForSingleObject: GetLastError = %d", GetLastError());
}


void
gb_thread_init(gbThread *t)
{
	gb_zero_struct(t);
	t->win32_handle = INVALID_HANDLE_VALUE;
	gb_semaphore_init(&t->semaphore);
}

void
gb_thread_destory(gbThread *t)
{
	if (t->is_running) gb_thread_join(t);
	gb_semaphore_destroy(&t->semaphore);
}


gb_internal void
gb__thread_run(gbThread *t)
{
	gb_semaphore_post(&t->semaphore, 1);
	t->proc(t->data);
}

#if defined(GB_SYSTEM_WINDOWS)
	gb_internal DWORD WINAPI gb__thread_proc(void *arg) { gb__thread_run(cast(gbThread *)arg); return 0; }
#else
	gb_internal void *gb__thread_proc(void *arg) { gb__thread_run(cast(gbThread *)arg); return NULL; }
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

	t->win32_handle = CreateThread(NULL, stack_size, gb__thread_proc, t, 0, NULL);
	GB_ASSERT_MSG(t->win32_handle != NULL, "CreateThread: GetLastError = %d", GetLastError());

	t->is_running = true;
	gb_semaphore_wait(&t->semaphore);
}

gb_inline void
gb_thread_join(gbThread *t)
{
	if (!t->is_running) return;

	WaitForSingleObject(t->win32_handle, INFINITE);
	CloseHandle(t->win32_handle);
	t->win32_handle = INVALID_HANDLE_VALUE;
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
	asm("mov %%gs:0x00,%0" : "=r"(thread_id));
#elif defined(GB_ARCH_32_BIT)
	asm("mov %%gs:0x08,%0" : "=r"(thread_id));
#elif defined(GB_ARCH_64_BIT)
	asm("mov %%gs:0x10,%0" : "=r"(thread_id));
#else
	#error Unsupported architecture for thread::current_id()
#endif

	return thread_id;
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
	switch (type) {
	case GB_ALLOCATION_ALLOC: {
		void *ptr;
		isize actual_size = size + alignment;
		ptr = gb_align_forward(gb_malloc(actual_size), alignment);

		return ptr;
	} break;

	case GB_ALLOCATION_FREE: {
		gb_mfree(old_memory);
	} break;

	case GB_ALLOCATION_FREE_ALL:
		break;

	case GB_ALLOCATION_RESIZE: {
		// TODO(bill): Check if ptr is on top of stack and just extend
		gbAllocator a = gb_heap_allocator();
		if (!old_memory) return gb_alloc_align(a, size, alignment);

		if (size < old_size)
			size = old_size;

		if (old_size == size) {
			return old_memory;
		} else {
			void *new_memory = gb_alloc_align(a, size, alignment);
			if (!new_memory) return NULL;
			gb_memmove(new_memory, old_memory, gb_min(size, old_size));
			gb_free(a, old_memory);
			return new_memory;
		}
	} break;
	}

	return NULL; // NOTE(bill): Default return value
}








gb_inline void
gb_arena_init_from_memory(gbArena *arena, void *start, isize size)
{
	arena->backing.proc = NULL;
	arena->backing.data = NULL;
	arena->physical_start = start;
	arena->total_size = size;
	arena->total_allocated = 0;
	arena->temp_count = 0;
}

gb_inline void
gb_arena_init_from_allocator(gbArena *arena, gbAllocator backing, isize size)
{
	arena->backing = backing;
	arena->physical_start = gb_alloc(backing, size); // NOTE(bill): Uses default alignment
	arena->total_size = size;
	arena->total_allocated = 0;
	arena->temp_count = 0;
}

gb_inline void gb_arena_init_subarena(gbArena *arena, gbArena *parent_arena, isize size) { gb_arena_init_from_allocator(arena, gb_arena_allocator(parent_arena), size); }


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
		void *ptr;
		isize actual_size = size + alignment;

		// NOTE(bill): Out of memory
		if (arena->total_allocated + actual_size > cast(isize)arena->total_size)
			return NULL;

		ptr = gb_align_forward(gb_pointer_add(arena->physical_start, arena->total_allocated), alignment);
		arena->total_allocated += actual_size;
		return ptr;
	} break;

	case GB_ALLOCATION_FREE:
		// NOTE(bill): Free all at once
		// NOTE(bill): Use Temp_Arena_Memory if you want to free a block
		// TODO(bill): Free it if it's on top of the stack
		break;

	case GB_ALLOCATION_FREE_ALL:
		arena->total_allocated = 0;
		break;

	case GB_ALLOCATION_RESIZE: {
		// TODO(bill): Check if ptr is on top of stack and just extend
		gbAllocator a = gb_arena_allocator(arena);
		if (!old_memory) return gb_alloc_align(a, size, alignment);

		if (size < old_size)
			size = old_size;

		if (old_size == size) {
			return old_memory;
		} else {
			void *new_memory = gb_alloc_align(a, size, alignment);
			if (!new_memory) return NULL;
			gb_memmove(new_memory, old_memory, gb_min(size, old_size));
			gb_free(a, old_memory);
			return new_memory;
		}
	} break;
	}

	return NULL; // NOTE(bill): Default return value
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

	// NOTE(bill): Init intrusive freelist
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
		// TODO(bill):
		break;

	case GB_ALLOCATION_RESIZE:
		// NOTE(bill): Cannot resize
		break;
	}

	return NULL;
}







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
gb_char_first_occurence(char const *s, char c)
{
	char ch = c;
	for (; *s != ch; s++) {
		if (*s == '\0')
			return 0;
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










gb_inline void gb__string_set_length(gbString str, isize len) { GB_STRING_HEADER(str)->length = len; }
gb_inline void gb__string_set_capacity(gbString str, isize cap) { GB_STRING_HEADER(str)->capacity = cap; }


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

gb_inline isize gb_string_length(gbString const str)   { return GB_STRING_HEADER(str)->length; }
gb_inline isize gb_string_capacity(gbString const str) { return GB_STRING_HEADER(str)->capacity; }

gb_inline isize
gb_string_available_space(gbString const str)
{
	gbStringHeader *h = GB_STRING_HEADER(str);
	if (h->capacity > h->length)
		return h->capacity - h->length;
	return 0;
}


gb_inline void gb_string_clear(gbString str) { gb__string_set_length(str, 0); str[0] = '\0'; }

gb_inline gbString gb_string_append_string(gbString str, gbString const other) { return gb_string_append_string_length(str, other, gb_string_length(other)); }

gbString
gb_string_append_string_length(gbString str, void const *other, isize other_len)
{
	isize curr_len = gb_string_length(str);

	str = gb_string_make_space_for(str, other_len);
	if (str == NULL)
		return NULL;

	gb_memcopy(str + curr_len, other, other_len);
	str[curr_len + other_len] = '\0';
	gb__string_set_length(str, curr_len + other_len);

	return str;
}

gb_inline gbString
gb_string_append_cstring(gbString str, char const *other)
{
	return gb_string_append_string_length(str, other, cast(isize)strlen(other));
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
	gb__string_set_length(str, len);

	return str;
}



gbString
gb_string_make_space_for(gbString str, isize add_len)
{
	isize available = gb_string_available_space(str);

	// Return if there is enough space left
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
		gb__string_set_capacity(str, new_len);

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
gb_strings_are_equal(gbString const lhs, gbString const rhs)
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

	gb__string_set_length(str, len);

	return str;
}

gb_inline gbString gb_string_trim_space(gbString str) { return gb_string_trim(str, " \t\r\n\v\f"); }




////////////////////////////////////////////////////////////////
//
// Windows UTF-8 Handling
//
//


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
			if (*str == 0xed && str[1] > 0x9f) // str[1] < 0x80 is checked below
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
			if (*str == 0xf4 && str[1] > 0x8f) // str[1] < 0x80 is checked below
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
			// UTF-8 encodings of values used in surrogate pairs are invalid
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




gb_no_inline void
gb__array_set_capacity(void *array_, isize capacity, isize element_size)
{
	// NOTE(bill): I know this is unsafe so don't call this function directly
	gbVoidArray *a = cast(gbVoidArray *)array_;
	void *data = NULL;

	GB_ASSERT(element_size > 0);

	if (capacity == a->capacity)
		return;

	if (capacity < a->count) {
		if (a->capacity < capacity) {
			isize new_capacity = GB_ARRAY_GROW_FORMULA(a->capacity);
			if (new_capacity < capacity)
				new_capacity = capacity;
			gb__array_set_capacity(a, new_capacity, element_size);
		}
		a->count = capacity;
	}

	if (capacity > 0) {
		data = gb_alloc(a->allocator, element_size*capacity);
		gb_memcopy(data, a->data, element_size*a->count);
	}
	gb_free(a->allocator, a->data);
	a->data = data;
	a->capacity = capacity;
}






////////////////////////////////////////////////////////////////
//
// File Handling
//
//

b32
gb_file_create(gbFile *file, char const *filepath, ...)
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
		file->type = GB_FILE_TYPE_WRITE;
		file->last_write_time = gb_file_last_write_time(file->path);
		return true;
	}
	return false;
}



b32
gb_file_open(gbFile *file, char const *filepath, ...)
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
		file->type = GB_FILE_TYPE_READ;
		file->last_write_time = gb_file_last_write_time(file->path);
		return true;
	}
	return false;
}

gb_inline b32
gb_file_close(gbFile *file)
{
	b32 result = true;
	if (file && file->handle)
		result = fclose(cast(FILE *)file->handle) != 0; // TODO(bill): Handle fclose errors

	if (file->path) gb_free(gb_heap_allocator(), file->path);
	file->is_open = false;

	return result;
}

gb_inline b32
gb_file_read_at(gbFile *file, void *buffer, isize size, i64 offset)
{
	i64 prev_cursor_pos;

	GB_ASSERT(file->type == GB_FILE_TYPE_READ);

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

	GB_ASSERT(file->type == GB_FILE_TYPE_WRITE);

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
gb_file_has_changed(gbFile *file)
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
gb_file_copy(char const *existing_filename, char const *new_filename, b32 fail_if_exists)
{
	return CopyFile(existing_filename, new_filename, fail_if_exists);
}

gb_inline b32
gb_file_move(char const *existing_filename, char const *new_filename)
{
	return MoveFile(existing_filename, new_filename);
}



#else
#error
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

	if (gb_file_open(&file, path)) {
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

#if defined(GB_SYSTEM_WINDOWS)
gb_inline void gb_exit(u32 code) { ExitProcess(code); }
#else
#error
#endif


////////////////////////////////////////////////////////////////
//
// DLL Handling
//
//

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
gb_inline void gb_dll_unload(gbDllHandle dll) { FreeLibrary(cast(HMODULE)dll); }
gb_inline gbDllProc gb_dll_proc_address(gbDllHandle dll, char const *proc_name) { return cast(gbDllProc)GetProcAddress(cast(HMODULE)dll, proc_name); }

#else
#error
#endif


////////////////////////////////////////////////////////////////
//
// Time
//
//
#if defined(GB_SYSTEM_WINDOWS)

gb_inline u64 gb_rdtsc(void) { return __rdtsc(); }

gb_global LARGE_INTEGER gb__win32_perf_count_freq = {0};

gb_inline f64
gb_time_now(void)
{
	f64 result;
	LARGE_INTEGER counter;
	if (!gb__win32_perf_count_freq.QuadPart)
		QueryPerformanceFrequency(&gb__win32_perf_count_freq);
	GB_ASSERT(gb__win32_perf_count_freq.QuadPart != 0);

	QueryPerformanceCounter(&counter);

	result = counter.QuadPart / cast(f64)(gb__win32_perf_count_freq.QuadPart);
	return result;
}


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
#error
#endif



#if defined(__cplusplus)
}
#endif
#endif /* GB_IMPLEMENTATION */
