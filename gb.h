/* gb.h - v0.04 - public domain C helper library - no warranty implied; use at your own risk */
/* (Experimental) A C helper library geared towards game development */

/*
 * LICENSE
 *  	This software is in the public domain. Where that dedication is not
 *  	recognized, you are granted a perpetual, irrevocable license to copy,
 *  	distribute, and modify this file as you see fit.
 *
 * WARNING
 * 	- This library is _highly_ experimental and features may not work as expected.
 * 	- This also means that many functions are not documented.
 *
 */

 /*
Version History:

	0.04a - Change conventions to be in keeping with `gb.hpp`
	0.04  - Allow for no <stdio.h>
	0.03  - Allocators can be passed to gb_alloc/free/etc. without cast using `typedef void* gb_Allocator_Ptr`
	0.02  - Implement all functions (from gb.hpp)
	0.01  - Initial Version (just prototypes)
*/


#ifndef GB_INCLUDE_GB_H
#define GB_INCLUDE_GB_H

#if defined(__cplusplus)
extern "C" {
#endif

/* NOTE(bill): Because static means three different things in C/C++
 *             Great Design(!)
 */
#ifndef global_variable
#define global_variable  static
#define internal_linkage static
#define local_persist    static
#endif

/* Example for static defines

	global_variable f32 const TAU = 6.283185f;
	global_variable void* g_memory;

	internal_linkage void
	some_function(...)
	{
		local_persist u32 count = 0;
		...
		count++;
		...
	}
*/


#if defined(_MSC_VER)
	#define _ALLOW_KEYWORD_MACROS

	#ifndef alignof /* Needed for MSVC 2013 'cause Microsoft "loves" standards */
	#define alignof(x) __alignof(x)
	#endif
#endif


/**********************************/
/*                                */
/* System OS                      */
/*                                */
/**********************************/

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


#if defined(_MSC_VER)
	/* Microsoft Visual Studio */
	#define GB_COMPILER_MSVC 1
#elif defined(__clang__)
	/* Clang */
	#define GB_COMPILER_CLANG 1
#elif defined(__GNUC__) || defined(__GNUG__) && !(defined(__clang__) || defined(__INTEL_COMPILER))
	/* GNU GCC/G++ Compiler */
	#define GB_COMPILER_GNU_GCC 1
#elif defined(__INTEL_COMPILER)
	/* Intel C++ Compiler */
	#define GB_COMPILER_INTEL 1
#endif

/* Environment Bit Size */
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
	#define GB_IS_BIG_EDIAN    (!*(unsigned char*)&(unsigned short){1})
	#define GB_IS_LITTLE_EDIAN (!GB_IS_BIG_EDIAN)
#endif

#define GB_IS_POWER_OF_TWO(x) ((x) != 0) && !((x) & ((x) - 1))


#if !defined(GB_HAS_NO_CONSTEXPR)
	#if defined(_GNUC_VER) && _GNUC_VER < 406  /* Less than gcc 4.06 */
		#define GB_HAS_NO_CONSTEXPR 1
	#elif defined(_MSC_VER) && _MSC_VER < 1900 /* Less than Visual Studio 2015/MSVC++ 14.0 */
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

#if !defined(GB_NO_STDIO)
#include <stdio.h>
#endif
#include <assert.h>

#if defined(GB_SYSTEM_WINDOWS)
	#define NOMINMAX            1
	#define VC_EXTRALEAN        1
	#define WIN32_EXTRA_LEAN    1
	#define WIN32_LEAN_AND_MEAN 1

	#include <windows.h> /* TODO(bill): Should we include only the needed headers? */
	#include <mmsystem.h> /* Time functions */

	#undef NOMINMAX
	#undef VC_EXTRALEAN
	#undef WIN32_EXTRA_LEAN
	#undef WIN32_LEAN_AND_MEAN

#else
	#include <pthread.h>
	#include <sys/time.h>
#endif

#ifndef true
#define true  (0==0)
#define false (0!=0)
#endif

#ifndef NULL
#define NULL ((void*)0)
#endif

#if defined(NDEBUG)
	#define GB_ASSERT(cond) ((void)(0))
#else
	/* TODO(bill): Do a better assert */
	#define GB_ASSERT(cond) assert(cond)
#endif

#define GB_STATIC_ASSERT(cond, msg) typedef char gb__static_assertion_##msg[(!!(cond))*2-1]
/* token pasting madness: */
#define GB_COMPILE_TIME_ASSERT3(cond, line) GB_STATIC_ASSERT(cond, static_assertion_at_line_##line)
#define GB_COMPILE_TIME_ASSERT2(cond, line) GB_COMPILE_TIME_ASSERT3(cond, line)
#define GB_COMPILE_TIME_ASSERT(cond)        GB_COMPILE_TIME_ASSERT2(cond, __LINE__)



#if !defined(GB_NO_STDIO) && defined(_MSC_VER)
	/* snprintf_msvc */
	int
	gb__vsnprintf_compatible(char* buffer, size_t size, char const* format, va_list args)
	{
		int result = -1;
		if (size > 0) result = _vsnprintf_s(buffer, size, _TRUNCATE, format, args);
		if (result == -1) return _vscprintf(format, args);
		return result;
	}

	int
	gb__snprintf_compatible(char* buffer, size_t size, char const* format, ...)
	{
		va_list args;
		va_start(args, format);
		int result = gb__vsnprintf_compatible(buffer, size, format, args);
		va_end(args);
		return result;
	}

	#if !defined(GB_DO_NOT_USE_MSVC_SPRINTF_FIX)
		#define snprintf  gb__snprintf_compatible
		#define vsnprintf gb__vsnprintf_compatible
	#endif /* GB_DO_NOT_USE_MSVC_SPRINTF_FIX */
#endif /* !defined(GB_NO_STDIO) */

#if defined(_MSC_VER)
	typedef unsigned __int8  u8;
	typedef   signed __int8  s8;
	typedef unsigned __int16 u16;
	typedef   signed __int16 s16;
	typedef unsigned __int32 u32;
	typedef   signed __int32 s32;
	typedef unsigned __int64 u64;
	typedef   signed __int64 s64;
#else
	#include <stdint.h>

	typedef uint8_t  u8;
	typedef  int8_t  s8;
	typedef uint16_t u16;
	typedef  int16_t s16;
	typedef uint32_t u32;
	typedef  int32_t s32;
	typedef uint64_t u64;
	typedef  int64_t s64;
#endif

GB_COMPILE_TIME_ASSERT(sizeof(s8)  == 1);
GB_COMPILE_TIME_ASSERT(sizeof(s16) == 2);
GB_COMPILE_TIME_ASSERT(sizeof(s32) == 4);
GB_COMPILE_TIME_ASSERT(sizeof(s64) == 8);

typedef float  f32;
typedef double f64;

typedef s32 bool32;

#if defined(GB_ARCH_32_BIT)
	typedef u32 usize;
	typedef s32 ssize;
#elif defined(GB_ARCH_64_BIT)
	typedef u64 usize;
	typedef s64 ssize;
#else
	#error Unknown architecture bit size
#endif

GB_COMPILE_TIME_ASSERT(sizeof(usize) == sizeof(size_t));

typedef uintptr_t uintptr;
typedef intptr_t  intptr;
typedef ptrdiff_t ptrdiff;

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


/**********************************/
/*                                */
/* Casts                          */
/*                                */
/**********************************/

/* NOTE(bill): Easier to grep/find for casts */
/* Still not as type safe as C++ static_cast, reinterpret_cast, const_cast */
#define cast(Type, src) ((Type)(src))

#if defined(GB_COMPILER_GNU_GCC)
	#define bit_cast(Type, src) ({ GB_ASSERT(sizeof(Type) <= sizeof(src)); Type dst; memcpy(&dst, &(src), sizeof(Type)); dst; })
#endif

#define pseudo_cast(Type, src) (*cast(Type*, &(src)))

#define GB_UNUSED(x) cast(void, sizeof(x))


/**********************************/
/*                                */
/* Memory                         */
/*                                */
/**********************************/

/* NOTE(bill): 0[x] is used to prevent C++ style arrays with operator overloading  */
#define GB_ARRAY_COUNT(x) ((sizeof(x)/sizeof(0[x])) / (cast(size_t, !(sizeof(x) % sizeof(0[x])))))

#define GB_KILOBYTES(x) (         (x) * 1024ll)
#define GB_MEGABYTES(x) (GB_KILOBYTES(x) * 1024ll)
#define GB_GIGABYTES(x) (GB_MEGABYTES(x) * 1024ll)
#define GB_TERABYTES(x) (GB_GIGABYTES(x) * 1024ll)


typedef struct gb_Mutex
{
#if defined(GB_SYSTEM_WINDOWS)
	HANDLE win32_mutex;
#else
	pthread_mutex_t posix_mutex;
#endif
} gb_Mutex;

gb_Mutex gb_mutex_make(void);
void     gb_mutex_destroy(gb_Mutex* mutex);
void     gb_mutex_lock(gb_Mutex* mutex);
bool32   gb_mutex_try_lock(gb_Mutex* mutex);
void     gb_mutex_unlock(gb_Mutex* mutex);





typedef struct gb_Atomic32 { u32 nonatomic; } gb_Atomic32;
typedef struct gb_Atomic64 { u64 nonatomic; } gb_Atomic64;

u32  gb_atomic32_load(gb_Atomic32 const volatile* a);
void gb_atomic32_store(gb_Atomic32 volatile* a, u32 value);
u32  gb_atomic32_compare_exchange_strong(gb_Atomic32 volatile* a, u32 expected, u32 desired);
u32  gb_atomic32_exchanged(gb_Atomic32 volatile* a, u32 desired);
u32  gb_atomic32_fetch_add(gb_Atomic32 volatile* a, s32 operand);
u32  gb_atomic32_fetch_and(gb_Atomic32 volatile* a, u32 operand);
u32  gb_atomic32_fetch_or(gb_Atomic32 volatile* a, u32 operand);

u64  gb_atomic64_load(gb_Atomic64 const volatile* a);
void gb_atomic64_store(gb_Atomic64 volatile* a, u64 value);
u64  gb_atomic64_compare_exchange_strong(gb_Atomic64 volatile* a, u64 expected, u64 desired);
u64  gb_atomic64_exchanged(gb_Atomic64 volatile* a, u64 desired);
u64  gb_atomic64_fetch_add(gb_Atomic64 volatile* a, s64 operand);
u64  gb_atomic64_fetch_and(gb_Atomic64 volatile* a, u64 operand);
u64  gb_atomic64_fetch_or(gb_Atomic64 volatile* a, u64 operand);





typedef struct gb_Semaphore
{
#if defined(GB_SYSTEM_WINDOWS)
	HANDLE win32_handle;
#else
	gb_Mutex       mutex;
	pthread_cond_t cond;
	s32            count;
#endif
} gb_Semaphore;

gb_Semaphore gb_semaphore_make(void);
void gb_semaphore_destroy(gb_Semaphore* s);
void gb_semaphore_post(gb_Semaphore* s);
void gb_semaphore_post_count(gb_Semaphore* s, u32 count);
void gb_semaphore_wait(gb_Semaphore* s);





typedef void(gb_Thread_Procedure)(void*);

typedef struct gb_Thread
{
#if defined(GB_SYSTEM_WINDOWS)
	HANDLE win32_handle;
#else
	pthread_t posix_handle;
#endif

	gb_Thread_Procedure* proc;
	void*                data;

	gb_Semaphore semaphore;
	usize        stack_size;
	bool32       is_running;
} gb_Thread;

gb_Thread gb_thread_make(void);
void      gb_thread_destroy(gb_Thread* t);
void      gb_thread_start(gb_Thread* t, gb_Thread_Procedure* proc, void* data);
void      gb_thread_start_with_stack(gb_Thread* t, gb_Thread_Procedure* proc, void* data, usize stack_size);
void      gb_thread_join(gb_Thread* t);
bool32    gb_thread_is_running(gb_Thread t);

u32 gb_thread_current_id(void);




/**********************************/
/*                                */
/* Allocators                     */
/*                                */
/**********************************/


/* Default aligment for memory allocations */
#ifndef GB_DEFAULT_ALIGNMENT
#define GB_DEFAULT_ALIGNMENT 8
#endif

/*
 * NOTE(bill): The cost of the function pointer lookup is minor compared to the actually allocation in most cases
 */
typedef struct gb_Allocator
{
	/* Allocates the specified amount of memory aligned to the specified alignment */
	void* (*alloc)(struct gb_Allocator* a, usize size, usize align);

	/* Frees an allocation made with alloc() */
	void (*free)(struct gb_Allocator* a, void* ptr);

	/* Returns the amount of usuable memory allocated at `ptr`.
	 * If the allocator does not support tracking of the allocation size,
	 * the function will return -1
	 */
	s64 (*allocated_size)(struct gb_Allocator* a, void const* ptr);

	/* Returns the total amount of memory allocated by this allocator
	 * If the allocator does not track memory, the function will return -1
	 */
	s64 (*total_allocated)(struct gb_Allocator* a);
} gb_Allocator;

typedef void* gb_Allocator_Ptr;

void*
gb_alloc_align(gb_Allocator_Ptr allocator, usize size, usize align)
{
	GB_ASSERT(allocator != NULL);
	gb_Allocator* a = allocator;
	return a->alloc(a, size, align);
}
void*
gb_alloc(gb_Allocator_Ptr allocator, usize size)
{
	GB_ASSERT(allocator != NULL);
	return gb_alloc_align(allocator, size, GB_DEFAULT_ALIGNMENT);
}

#define gb_alloc_struct(allocator, Type)       cast((Type)*, gb_alloc_align(allocator, sizeof(Type),         alignof(Type)))
#define gb_alloc_array(allocator, Type, count) cast((Type)*, gb_alloc_align(allocator, sizeof(Type)*(count), alignof(Type)))

void
gb_free(gb_Allocator_Ptr allocator, void* ptr)
{
	GB_ASSERT(allocator != NULL);
	gb_Allocator* a = allocator;
	if (ptr) a->free(a, ptr);
}

s64
gb_allocated_size(gb_Allocator_Ptr allocator, void const* ptr)
{
	GB_ASSERT(allocator != NULL);
	gb_Allocator* a = allocator;
	return a->allocated_size(a, ptr);
}

s64
gb_total_allocated(gb_Allocator_Ptr allocator)
{
	GB_ASSERT(allocator != NULL);
	gb_Allocator* a = allocator;
	return a->total_allocated(a);
}




typedef struct gb_Heap
{
	gb_Allocator base; /* NOTE(bill): Must be first into order to allow for polymorphism */

	gb_Mutex mutex;
	bool32   use_mutex;
	s64      total_allocated_count;
	s64      allocation_count;

#if defined(GB_SYSTEM_WINDOWS)
	HANDLE win32_heap_handle;
#endif
} gb_Heap;

gb_Heap gb_heap_make(bool32 use_mutex);
void    gb_heap_destroy(gb_Heap* heap);


typedef struct gb_Arena
{
	gb_Allocator base; /* NOTE(bill): Must be first into order to allow for polymorphism */

	gb_Allocator* backing;
	void*         physical_start;
	s64           total_size;
	s64           total_allocated_count;
	s64           temp_count;
} gb_Arena;


gb_Arena gb_arena_make_from_backing(gb_Allocator* backing, usize size);
gb_Arena gb_arena_make_from_pointer(void* start, usize size);
void     gb_arena_destroy(gb_Arena* arena);
void     gb_arena_clear(gb_Arena* arena);


typedef struct gb_Temporary_Arena_Memory
{
	gb_Arena* arena;
	s64       original_count;
} gb_Temporary_Arena_Memory;

gb_Temporary_Arena_Memory gb_make_temporary_arena_memory(gb_Arena* arena);
void gb_temporary_arena_memory_free(gb_Temporary_Arena_Memory t);


typedef struct gb_Pool
{
	gb_Allocator base; /* NOTE(bill): Must be first into order to allow for polymorphism */

	gb_Allocator* backing;

	void* physical_start;
	void* free_list;

	usize block_size;
	usize block_align;
	s64   total_size;
} gb_Pool;

gb_Pool gb_pool_make(gb_Allocator* backing, usize num_blocks, usize block_size);
gb_Pool gb_pool_make_align(gb_Allocator* backing, usize num_blocks, usize block_size, usize block_align);
void    gb_pool_destroy(gb_Pool* pool);




/**********************************/
/*                                */
/* Memory                         */
/*                                */
/**********************************/

void* gb_align_forward(void* ptr, usize align);

void   *gb_zero_size(void* ptr, usize bytes);
#define gb_zero_struct(element) (cast(void, gb_zero_size(&(element), sizeof(element))))
#define gb_zero_array(ptr, Type, count) cast(Type, gb_zero_size((ptr), sizeof(Type)*(count)))





/**********************************/
/*                                */
/* Array                          */
/*                                */
/**********************************/

/*
 * Array structure:
 *
 *
 * | Allocator * | usize count | usize capacity | char*  |
 *                                              |
 *                                               `-- Returned pointer
 */

typedef struct gb_Array_Header
{
	gb_Allocator* allocator;
	usize         count;
	usize         capacity;
} gb_Array_Header;

/* TODO(bill): Implement a c style array maybe like stb/stretchy_buffer.h but with a custom allocator */

#define gb_array_header(arr)           (cast(gb_Array_Header*, arr) - 1)

#define gb_array_make_count(allocator, Type, count) /* TODO(bill): */
#define gb_array_make(allocator, Type) (gb_array_make_count(allocator, Type, 0))
#define gb_array_free(arr)             (gb_free(gb_array_header(arr)->allocator, gb_array_header(arr)))

#define gb_array_allocator(arr) (gb_array_header(arr)->allocator)
#define gb_array_count(arr)     (gb_array_header(arr)->count)
#define gb_array_capacity(arr)  (gb_array_header(arr)->capacity)

#define gb_array_append(arr, item)               /* TODO(bill): */
#define gb_array_append_array(arr, items, count) /* TODO(bill): */

#define gb_array_pop(arr)  (GB_ASSERT(gb_array_header(arr)->count > 0), gb_array_header(arr)->count--)

#define gb_array_clear(arr) (gb_array_header(arr)->count = 0)
#define gb_array_resize(arr, count)          /* TODO(bill): */
#define gb_array_reserve(arr, capacity)      /* TODO(bill): */
#define gb_array_set_capacity(arr, capacity) /* TODO(bill): */
#define gb_array_grow(arr, min_capacity)     /* TODO(bill): */




/**********************************/
/*                                */
/* String - C Compatibile         */
/*                                */
/**********************************/


/* Pascal like strings in C */
typedef char* gb_String;

#ifndef GB_STRING_SIZE
#define GB_STRING_SIZE
/* define GB_STRING_SIZE to allow for a custom size e.g. u16, usize, int, etc. */
typedef u32 gb_String_Size;
#endif

typedef struct gb_String_Header
{
	gb_Allocator*  allocator;
	gb_String_Size length;
	gb_String_Size capacity;
} gb_String_Header;

#define GB_STRING_HEADER(str) (cast(gb_String_Header*, str) - 1)

gb_String gb_string_make(gb_Allocator* a, char const* str);
gb_String gb_string_make_length(gb_Allocator* a, void const* str, gb_String_Size num_bytes);
void   gb_string_free(gb_String str);

gb_String gb_string_duplicate(gb_Allocator* a, gb_String const str);

gb_String_Size gb_string_length(gb_String const str);
gb_String_Size gb_string_capacity(gb_String const str);
gb_String_Size gb_string_available_space(gb_String const str);

void gb_string_clear(gb_String str);

gb_String gb_string_append_string(gb_String str, gb_String const other);
gb_String gb_string_append_string_length(gb_String str, void const* other, gb_String_Size num_bytes);
gb_String gb_string_append_cstring(gb_String str, char const* other);

gb_String gb_string_set(gb_String str, char const* cstr);

gb_String gb_string_make_space_for(gb_String str, gb_String_Size add_len);
gb_String_Size gb_string_allocation_size(gb_String const str);

bool32 gb_strings_are_equal(gb_String const lhs, gb_String const rhs);

gb_String gb_string_trim(gb_String str, char const* cut_set);
gb_String gb_string_trim_space(gb_String str); /* Whitespace ` \t\r\n\v\f` */





/**********************************/
/*                                */
/* Hash                           */
/*                                */
/**********************************/


u32 gb_hash_adler32(void const* ket, u32 num_bytes);

u32 gb_hash_crc32(void const* key, u32 num_bytes);
u64 gb_hash_crc64(void const* key, usize num_bytes);

u32 gb_hash_fnv32(void const* key, usize num_bytes);
u64 gb_hash_fnv64(void const* key, usize num_bytes);
u32 gb_hash_fnv32a(void const* key, usize num_bytes);
u64 gb_hash_fnv64a(void const* key, usize num_bytes);

u32 gb_hash_murmur32(void const* key, u32 num_bytes, u32 seed);
u64 gb_hash_murmur64(void const* key, usize num_bytes, u64 seed);





/**********************************/
/*                                */
/* Time                           */
/*                                */
/**********************************/

/* TODO(bill): How should I this  */
typedef struct gb_Time { s64 microseconds; } gb_Time;

#define GB_TIME_ZERO (cast(gb_Time, {0}))

gb_Time gb_time_now(void);
void gb_time_sleep(gb_Time time);

gb_Time gb_time_seconds(f32 s);
gb_Time gb_time_milliseconsd(s32 ms);
gb_Time gb_time_microseconds(s64 us);

f32 gb_time_as_seconds(gb_Time t);
s32 gb_time_as_milliseconds(gb_Time t);
s64 gb_time_as_microseconds(gb_Time t);

/*
 * lhs <  rhs   -1
 * lhs == rhs    0
 * lhs >  rhs   +1
 */
s32 gb_time_cmp(gb_Time lhs, gb_Time rhs);

gb_Time gb_time_add(gb_Time lhs, gb_Time rhs);
gb_Time gb_time_sub(gb_Time lhs, gb_Time rhs);
gb_Time gb_time_mod(gb_Time lhs, gb_Time rhs);
gb_Time gb_time_mul(gb_Time t, f32 s);
gb_Time gb_time_div(gb_Time t, f32 s);
f32     gb_time_div_time(gb_Time num, gb_Time dom);


#if defined(__cplusplus)
}
#endif /* extern "C" */
#endif

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
// Woo hoo!
//
//
//
//
//
////////////////////////////////
//                            //
// Implemenation              //
//                            //
////////////////////////////////
#if defined(GB_IMPLEMENTATION)
#if defined(__cplusplus)
extern "C" {
#endif
/**********************************/
/*                                */
/* Memory                         */
/*                                */
/**********************************/
gb_Mutex
gb_mutex_make(void)
{
	gb_Mutex m = {0};
#if defined(GB_SYSTEM_WINDOWS)
	m.win32_mutex = CreateMutex(0, false, 0);
#else
	pthread_mutex_init(&m.posix_mutex, NULL);
#endif
	return m;
}

void
gb_mutex_destroy(gb_Mutex* m)
{
#if defined(GB_SYSTEM_WINDOWS)
	CloseHandle(m->win32_mutex);
#else
	pthread_mutex_destroy(&m->posix_mutex);
#endif
}

void
gb_mutex_lock(gb_Mutex* m)
{
#if defined(GB_SYSTEM_WINDOWS)
	WaitForSingleObject(m->win32_mutex, INFINITE);
#else
	pthread_mutex_lock(&m->posix_mutex);
#endif
}

bool32
gb_mutex_try_lock(gb_Mutex* m)
{
#if defined(GB_SYSTEM_WINDOWS)
	return WaitForSingleObject(m->win32_mutex, 0) == WAIT_OBJECT_0;
#else
	return pthread_mutex_trylock(&m->posix_mutex) == 0;
#endif
}

void
gb_mutex_unlock(gb_Mutex* m)
{
#if defined(GB_SYSTEM_WINDOWS)
	ReleaseMutex(m->win32_mutex);
#else
	pthread_mutex_unlock(&m->posix_mutex);
#endif
}




#if defined(_MSC_VER)
u32
gb_atomic32_load(gb_Atomic32 const volatile* object)
{
	return object->nonatomic;
}

void
gb_atomic32_store(gb_Atomic32 volatile* object, u32 value)
{
	object->nonatomic = value;
}

u32
gb_atomic32_compare_exchange_strong(gb_Atomic32 volatile* object, u32 expected, u32 desired)
{
	return _InterlockedCompareExchange(cast(long volatile*, object), desired, expected);
}

u32
gb_atomic32_exchanged(gb_Atomic32 volatile* object, u32 operand)
{
	return _InterlockedExchangeAdd(cast(long volatile*, object), operand);
}

u32
gb_atomic32_fetch_add(gb_Atomic32 volatile* object, s32 operand)
{
	return _InterlockedExchangeAdd(cast(long volatile*, object), operand);
}

u32
gb_atomic32_fetch_and(gb_Atomic32 volatile* object, u32 operand)
{
	return _InterlockedAnd(cast(long volatile*, object), operand);
}

u32
gb_atomic32_fetch_or(gb_Atomic32 volatile* object, u32 operand)
{
	return _InterlockedOr(cast(long volatile*, object), operand);
}


u64
gb_atomic64_load(gb_Atomic64 const volatile* object)
{
#if defined(GB_ARCH_64_BIT)
	return object->nonatomic;
#else
	/* NOTE(bill): The most compatible way to get an atomic 64-bit load on x86 is with cmpxchg8b */
	u64 result;
	__asm {
		mov esi, object;
		mov ebx, eax;
		mov ecx, edx;
		lock cmpxchg8b [esi];
		mov dword ptr result, eax;
		mov dword ptr result[4], edx;
	}
	return result;
#endif
}

void
gb_atomic64_store(gb_Atomic64 volatile* object, u64 value)
{
#if defined(GB_ARCH_64_BIT)
	object->nonatomic = value;
#else
	/* NOTE(bill): The most compatible way to get an atomic 64-bit store on x86 is with cmpxchg8b */
	__asm {
		mov esi, object;
		mov ebx, dword ptr value;
		mov ecx, dword ptr value[4];
	retry:
		cmpxchg8b [esi];
		jne retry;
	}
#endif
}

u64
gb_atomic64_compare_exchange_strong(gb_Atomic64 volatile* object, u64 expected, u64 desired)
{
	return _InterlockedCompareExchange64(cast(s64 volatile*, object), desired, expected);
}

u64
gb_atomic64_exchanged(gb_Atomic64 volatile* object, u64 desired)
{
#if defined(GB_ARCH_64_BIT)
	return _InterlockedExchange64(cast(s64 volatile*, object), desired);
#else
	u64 expected = object->nonatomic;
	while (true)
	{
		u64 original = _InterlockedCompareExchange64(cast(s64 volatile*, object), desired, expected);
		if (original == expected) return original;
		expected = original;
	}
#endif
}

u64
gb_atomic64_fetch_add(gb_Atomic64 volatile* object, s64 operand)
{
#if defined(GB_ARCH_64_BIT)
	return _InterlockedExchangeAdd64(cast(s64 volatile*, object), operand);
#else
	u64 expected = object->nonatomic;
	while (true)
	{
		u64 original = _InterlockedExchange64(cast(s64 volatile*, object), expected + operand, expected);
		if (original == expected) return original;
		expected = original;
	}
#endif
}

u64
gb_atomic64_fetch_and(gb_Atomic64 volatile* object, u64 operand)
{
#if defined(GB_ARCH_64_BIT)
	return _InterlockedAnd64(cast(s64 volatile*, object), operand);
#else
	u64 expected = object->nonatomic;
	while (true)
	{
		u64 original = _InterlockedCompareExchange64(cast(s64 volatile*, object), expected & operand, expected);
		if (original == expected)
			return original;
		expected = original;
	}
#endif
}
u64
gb_atomic64_fetch_or(gb_Atomic64 volatile* object, u64 operand)
{
#if defined(GB_ARCH_64_BIT)
	return _InterlockedAnd64(cast(s64 volatile*, object), operand);
#else
	u64 expected = object->nonatomic;
	while (true)
	{
		u64 original = _InterlockedCompareExchange64(cast(s64 volatile*, object), expected | operand, expected);
		if (original == expected)
			return original;
		expected = original;
	}
#endif
}
#else
#error TODO(bill): Implement atomics for this platform
#endif



gb_Semaphore
gb_semaphore_make(void)
{
	gb_Semaphore semaphore = {0};
#if defined(GB_SYSTEM_WINDOWS)
	semaphore.win32_handle = CreateSemaphore(NULL, 0, S32_MAX, NULL);
	GB_ASSERT(semaphore.win32_handle != NULL);

#else
	semaphore.count = 0;
	s32 result = pthread_cond_init(&semaphore.cond, NULL);
	GB_ASSERT(result == 0);

	semaphore.mutex = gb_mutex_make();
#endif

	return semaphore;
}

void
gb_semaphore_destroy(gb_Semaphore* s)
{
#if defined(GB_SYSTEM_WINDOWS)
	BOOL err = CloseHandle(s->win32_handle);
	GB_ASSERT(err != 0);
#else
	s32 result = pthread_cond_destroy(&s->cond);
	GB_ASSERT(result == 0);
	gb_mutex_destroy(&s->mutex);
#endif
}

void
gb_semaphore_post(gb_Semaphore* s)
{
	gb_semaphore_post_count(s, 1);
}

void
gb_semaphore_post_count(gb_Semaphore* s, u32 count)
{
#if defined(GB_SYSTEM_WINDOWS)
	BOOL err = ReleaseSemaphore(s->win32_handle, count, NULL);
	GB_ASSERT(err != 0);
#else
	gb_mutex_lock(&s->mutex);

	for (u32 i = 0; i < count; i++) {
		s32 result = pthread_cond_signal(&s->cond);
		GB_ASSERT(result == 0);
	}

	s->count += count;

	gb_mutex_unlock(&s->mutex);
#endif
}

void
gb_semaphore_wait(gb_Semaphore* s)
{
#if defined(GB_SYSTEM_WINDOWS)
	DWORD result = WaitForSingleObject(s->win32_handle, INFINITE);
	GB_ASSERT(result == WAIT_OBJECT_0);
#else
	gb_mutex_lock(&s->mutex);

	while (count <= 0) {
		s32 result = pthread_cond_wait(&s->cond, &s->mutex.posix_mutex);
		GB_ASSERT(result == 0);
	}

	count--;

	gb_mutex_unlock(&s->mutex);
#endif
}



gb_Thread
gb_thread_make(void)
{
	gb_Thread t = {0};
#if defined(GB_SYSTEM_WINDOWS)
	t.win32_handle = INVALID_HANDLE_VALUE;
#else
	t.posix_handle = 0;
#endif
	t.proc       = NULL;
	t.data       = NULL;
	t.stack_size = 0;
	t.is_running = false;
	t.semaphore  = gb_semaphore_make();

	return t;
}

void
gb_thread_destroy(gb_Thread* t)
{
	if (t->is_running) gb_thread_join(t);
	gb_semaphore_destroy(&t->semaphore);
}

internal_linkage void
gb__thread_run(gb_Thread* t)
{
	gb_semaphore_post(&t->semaphore);
	t->proc(t->data);
}

#if defined(GB_SYSTEM_WINDOWS)
internal_linkage DWORD WINAPI
gb__thread_proc(void* arg)
{
	gb__thread_run(cast(gb_Thread* , arg));
	return 0;
}

#else
internal_linkage void*
gb__thread_proc(void* arg)
{
	gb__thread_run(cast(gb_Thread* , arg));
	return NULL;
}
#endif

void
gb_thread_start(gb_Thread* t, gb_Thread_Procedure* proc, void* data)
{
	gb_thread_start_with_stack(t, proc, data, 0);
}

void
gb_thread_start_with_stack(gb_Thread* t, gb_Thread_Procedure* proc, void* data, usize stack_size)
{
	GB_ASSERT(!t->is_running);
	GB_ASSERT(proc != NULL);
	t->proc = proc;
	t->data = data;
	t->stack_size = stack_size;

#if defined(GB_SYSTEM_WINDOWS)
	t->win32_handle = CreateThread(NULL, stack_size, gb__thread_proc, t, 0, NULL);
	GB_ASSERT(t->win32_handle != NULL);

#else
	pthread_attr_t attr;
	s32 result = pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
	GB_ASSERT(result == 0);

	if (t->stack_size != 0)
	{
		result = pthread_attr_setstacksize(&attr, t->stack_size);
		GB_ASSERT(result == 0);
	}

	result = pthread_create(&t->posix_handle, &attr, gb__thread_proc, thread);
	GB_ASSERT(result == 0);

	/* NOTE(bill): Free attr memory */
	result = pthread_attr_destroy(&attr);
	GB_ASSERT(result == 0);

	/* NOTE(bill): So much boiler patch compared to windows.h (for once) */
#endif

	t->is_running = true;
	gb_semaphore_wait(&t->semaphore);
}

void
gb_thread_join(gb_Thread* t)
{
	if (!t->is_running) return;

#if defined(GB_SYSTEM_WINDOWS)
	WaitForSingleObject(t->win32_handle, INFINITE);
	CloseHandle(t->win32_handle);
	t->win32_handle = INVALID_HANDLE_VALUE;
#else
	int result = pthread_join(t->posix_handle, NULL);
	t->posix_handle = 0;
#endif

	t->is_running = false;
}

bool32
gb_thread_is_running(gb_Thread t)
{
	return t.is_running != 0;
}

u32
gb_thread_current_id(void)
{
	u32 thread_id;

#if defined(GB_SYSTEM_WINDOWS)
	u8* thread_local_storage = cast(u8*, __readgsqword(0x30));
	thread_id = *cast(u32 *, thread_local_storage + 0x48);

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




/**********************************/
/*                                */
/* Allocators                     */
/*                                */
/**********************************/

typedef struct gb__Heap_Header
{
	usize size;
} gb__Heap_Header;


internal_linkage void*
gb__heap_alloc(gb_Allocator* a, usize size, usize align)
{
	gb_Heap* heap = cast(gb_Heap*, a);

	if (heap->use_mutex) gb_mutex_lock(&heap->mutex);

	usize total = size + align - (size % align);

#if defined(GB_SYSTEM_WINDOWS)
	total += sizeof(gb__Heap_Header);

	void* data = HeapAlloc(heap->win32_heap_handle, 0, total);

	gb__Heap_Header* h = cast(gb__Heap_Header*, data);
	h->size = total;
	data = (h + 1);

#else
	/* TODO(bill): Find a better malloc alternative for this platform */
	void* data = malloc(total);
#endif

	heap->total_allocated_count += total;
	heap->allocation_count++;

	if (heap->use_mutex) gb_mutex_unlock(&heap->mutex);

	return data;
}



internal_linkage void
gb__heap_free(gb_Allocator* a, void* ptr)
{
	if (!ptr) return;

	gb_Heap* heap = cast(gb_Heap*, a);

	if (heap->use_mutex) gb_mutex_lock(&heap->mutex);

	heap->total_allocated_count -= gb_allocated_size(heap, ptr);
	heap->allocation_count--;

#if defined (GB_SYSTEM_WINDOWS)
	gb__Heap_Header* header = cast(gb__Heap_Header*, ptr) - 1;
	HeapFree(heap->win32_heap_handle, 0, header);
#else
	free(ptr);
#endif

	if (heap->use_mutex) gb_mutex_unlock(&heap->mutex);
}

internal_linkage s64
gb__heap_allocated_size(gb_Allocator* a, void const* ptr)
{
#if defined(GB_SYSTEM_WINDOWS)
	gb_Heap* heap = cast(gb_Heap*, a);

	if (heap->use_mutex) gb_mutex_lock(&heap->mutex);

	gb__Heap_Header const* h = cast(gb__Heap_Header const*, ptr) - 1;
	s64 result = h->size;

	if (heap->use_mutex) gb_mutex_unlock(&heap->mutex);

	return cast(s64, result);

#elif defined(GB_SYSTEM_OSX)
	return cast(s64, malloc_size(ptr));

#elif defined(GB_SYSTEM_LINUX)
	return cast(s64, malloc_usable_size(ptr));

#else
	#error Implement Heap::allocated_size
#endif
}

internal_linkage s64
gb__heap_total_allocated(gb_Allocator* a)
{
	gb_Heap* heap = cast(gb_Heap*, a);

	if (heap->use_mutex) gb_mutex_lock(&heap->mutex);

	s64 result = heap->total_allocated_count;

	if (heap->use_mutex) gb_mutex_unlock(&heap->mutex);

	return result;
}




gb_Heap
gb_heap_make(bool32 use_mutex)
{
	gb_Heap heap = {0};

	heap.use_mutex = use_mutex;
	if (use_mutex) heap.mutex = gb_mutex_make();

#if defined(GB_SYSTEM_WINDOWS)
	heap.win32_heap_handle = HeapCreate(0, 0, 0);
#endif

	heap.base.alloc           = gb__heap_alloc;
	heap.base.free            = gb__heap_free;
	heap.base.allocated_size  = gb__heap_allocated_size;
	heap.base.total_allocated = gb__heap_total_allocated;

	return heap;
}

void
gb_heap_destroy(gb_Heap* heap)
{
	if (heap->use_mutex) gb_mutex_destroy(&heap->mutex);

#if defined (GB_SYSTEM_WINDOWS)
	HeapDestroy(heap->win32_heap_handle);
#endif
}





internal_linkage void*
gb__arena_alloc(gb_Allocator* a, usize size, usize align)
{
	gb_Arena* arena = cast(gb_Arena*, a);

	s64 actual_size = size + align;

	if (arena->total_allocated_count + actual_size > arena->total_size)
	{
		GB_ASSERT(arena->total_allocated_count + actual_size <= arena->total_size);
		return NULL;
	}

	void* ptr = gb_align_forward(cast(u8*, arena->physical_start) + arena->total_allocated_count, align);

	arena->total_allocated_count += actual_size;

	return ptr;
}

internal_linkage void
gb__arena_free(gb_Allocator* a, void* ptr) /* NOTE(bill): Arenas free all at once */
{
	GB_UNUSED(a);
	GB_UNUSED(ptr);
}

internal_linkage s64
gb__arena_allocated_size(gb_Allocator* a, void const* ptr)
{
	GB_UNUSED(a);
	GB_UNUSED(ptr);
	return -1;
}

internal_linkage s64
gb__arena_total_allocated(gb_Allocator* a)
{
	return cast(gb_Arena*, a)->total_allocated_count;
}



gb_Arena
gb_arena_make_from_backing(gb_Allocator* backing, usize size)
{
	gb_Arena arena = {0};

	arena.backing = backing;
	arena.physical_start = NULL;
	arena.total_size = size;
	arena.temp_count = 0;
	arena.total_allocated_count = 0;

	arena.physical_start = gb_alloc(arena.backing, size);

	arena.base.alloc           = gb__arena_alloc;
	arena.base.free            = gb__arena_free;
	arena.base.allocated_size  = gb__arena_allocated_size;
	arena.base.total_allocated = gb__arena_total_allocated;

	return arena;
}

gb_Arena
gb_arena_make_from_pointer(void* start, usize size)
{
	gb_Arena arena = {0};

	arena.backing = NULL;
	arena.physical_start = start;
	arena.total_size = size;
	arena.temp_count = 0;
	arena.total_allocated_count = 0;

	arena.base.alloc           = gb__arena_alloc;
	arena.base.free            = gb__arena_free;
	arena.base.allocated_size  = gb__arena_allocated_size;
	arena.base.total_allocated = gb__arena_total_allocated;

	return arena;
}

void
gb_arena_destroy(gb_Arena* arena)
{
	if (arena->backing)
		gb_free(arena->backing, arena->physical_start);

	gb_arena_clear(arena);
}

void
gb_arena_clear(gb_Arena* arena)
{
	GB_ASSERT(arena->temp_count == 0);

	arena->total_allocated_count = 0;
}



gb_Temporary_Arena_Memory
gb_make_temporary_arena_memory(gb_Arena* arena)
{
	gb_Temporary_Arena_Memory tmp = {0};
	tmp.arena = arena;
	tmp.original_count = arena->total_allocated_count;
	arena->temp_count++;
	return tmp;
}

void
gb_temporary_arena_memory_free(gb_Temporary_Arena_Memory tmp)
{
	GB_ASSERT(gb_total_allocated(tmp.arena) >= tmp.original_count);
	tmp.arena->total_allocated_count = tmp.original_count;
	GB_ASSERT(tmp.arena->temp_count > 0);
	tmp.arena->temp_count--;
}








internal_linkage void*
gb__pool_alloc(gb_Allocator* a, usize size, usize align)
{
	gb_Pool* pool = cast(gb_Pool*, a);

	GB_ASSERT(size  == pool->block_size);
	GB_ASSERT(align == pool->block_align);
	GB_ASSERT(pool->free_list != NULL);

	uintptr next_free = *cast(uintptr*, pool->free_list);
	void* ptr = pool->free_list;
	pool->free_list = cast(void*, next_free);

	pool->total_size += pool->block_size;

	return ptr;
}

internal_linkage void
gb__pool_free(gb_Allocator* a, void* ptr)
{
	if (!ptr) return;

	gb_Pool* pool = cast(gb_Pool*, a);

	uintptr* next = cast(uintptr*, ptr);
	*next = cast(uintptr, pool->free_list);

	pool->free_list = ptr;

	pool->total_size -= pool->block_size;
}

internal_linkage s64
gb__pool_allocated_size(gb_Allocator* a, void const* ptr)
{
	GB_UNUSED(a);
	GB_UNUSED(ptr);
	return -1;
}

internal_linkage s64
gb__pool_total_allocated(gb_Allocator* a)
{
	gb_Pool* pool = cast(gb_Pool*, a);
	return pool->total_size;
}


gb_Pool
gb_pool_make(gb_Allocator* backing, usize num_blocks, usize block_size)
{
	return gb_pool_make_align(backing, num_blocks, block_size, GB_DEFAULT_ALIGNMENT);
}

gb_Pool
gb_pool_make_align(gb_Allocator* backing, usize num_blocks, usize block_size, usize block_align)
{
	gb_Pool pool = {0};

	pool.backing     = backing;
	pool.block_size  = block_size;
	pool.block_align = block_align;

	usize actual_block_size = block_size + block_align;
	usize pool_size = num_blocks * actual_block_size;

	u8* data = cast(u8*, gb_alloc_align(backing, pool_size, block_align));


	/* Init intrusive freelist */
	u8* curr = data;
	for (usize block_index = 0; block_index < num_blocks-1; block_index++)
	{
		uintptr* next = cast(uintptr*, curr);
		*next = cast(uintptr, curr) + actual_block_size;
		curr += actual_block_size;
	}

	uintptr* end = cast(uintptr*, curr);
	*end = cast(uintptr, NULL);

	pool.physical_start = data;
	pool.free_list      = data;

	/* Set functions pointers */
	pool.base.alloc           = gb__pool_alloc;
	pool.base.free            = gb__pool_free;
	pool.base.allocated_size  = gb__pool_allocated_size;
	pool.base.total_allocated = gb__pool_total_allocated;

	return pool;
}

void
gb_pool_destroy(gb_Pool* pool)
{
	gb_free(pool->backing, pool->physical_start);
}




/**********************************/
/*                                */
/* Memory                         */
/*                                */
/**********************************/

void*
gb_align_forward(void* ptr, usize align)
{
	GB_ASSERT(GB_IS_POWER_OF_TWO(align));

	uintptr p = cast(uintptr, ptr);
	usize modulo = p % align;
	if (modulo) p += (align - modulo);
	return cast(void*, p);
}

void* gb_zero_size(void* ptr, usize bytes) { return memset(ptr, 0, bytes); }




/**********************************/
/*                                */
/* String - C Compatibile         */
/*                                */
/**********************************/


internal_linkage void
gb__string_set_length(gb_String str, gb_String_Size len)
{
	GB_STRING_HEADER(str)->length = len;
}


internal_linkage void
gb__string_set_capacity(gb_String str, gb_String_Size cap)
{
	GB_STRING_HEADER(str)->capacity = cap;
}


gb_String
gb_string_make(gb_Allocator* a, char const* str)
{
	gb_String_Size len = cast(gb_String_Size, str ? strlen(str) : 0);
	return gb_string_make_length(a, str, len);
}

gb_String
gb_string_make_length(gb_Allocator* a, void const* init_str, gb_String_Size num_bytes)
{
	gb_String_Size header_size = sizeof(gb_String_Header);
	void* ptr = gb_alloc(a, header_size + num_bytes + 1);
	if (!init_str) gb_zero_size(ptr, header_size + num_bytes + 1);

	if (ptr == NULL) return NULL;

	gb_String str = (char*)ptr + header_size;
	gb_String_Header* header = GB_STRING_HEADER(str);
	header->allocator = a;
	header->length    = num_bytes;
	header->capacity  = num_bytes;
	if (num_bytes && init_str)
		memcpy(str, init_str, num_bytes);
	str[num_bytes] = '\0';

	return str;
}

void
gb_string_free(gb_String str)
{
	if (str == NULL) return;

	gb_String_Header* header = GB_STRING_HEADER(str);
	gb_free(header->allocator, header);
}


gb_String
gb_string_duplicate(gb_Allocator* a, gb_String const str)
{
	return gb_string_make_length(a, str, gb_string_length(str));
}


gb_String_Size
gb_string_length(gb_String const str)
{
	return GB_STRING_HEADER(str)->length;
}

gb_String_Size
gb_string_capacity(gb_String const str)
{
	return GB_STRING_HEADER(str)->capacity;
}

gb_String_Size
gb_string_available_space(gb_String const str)
{
	gb_String_Header* h = GB_STRING_HEADER(str);
	if (h->capacity > h->length)
		return h->capacity - h->length;
	return 0;
}


void
gb_string_clear(gb_String str)
{
	gb__string_set_length(str, 0);
	str[0] = '\0';
}


gb_String
gb_string_append_string(gb_String str, gb_String const other)
{
	return gb_string_append_string_length(str, other, gb_string_length(other));
}

gb_String
gb_string_append_string_length(gb_String str, void const* other, gb_String_Size other_len)
{
	gb_String_Size curr_len = gb_string_length(str);

	str = gb_string_make_space_for(str, other_len);
	if (str == NULL)
		return NULL;

	memcpy(str + curr_len, other, other_len);
	str[curr_len + other_len] = '\0';
	gb__string_set_length(str, curr_len + other_len);

	return str;
}

gb_String
gb_string_append_cstring(gb_String str, char const* other)
{
	return gb_string_append_string_length(str, other, cast(gb_String_Size, strlen(other)));
}


gb_String
gb_string_set(gb_String str, char const* cstr)
{
	gb_String_Size len = cast(gb_String_Size, strlen(cstr));
	if (gb_string_capacity(str) < len)
	{
		str = gb_string_make_space_for(str, len - gb_string_length(str));
		if (str == NULL)
			return NULL;
	}

	memcpy(str, cstr, len);
	str[len] = '\0';
	gb__string_set_length(str, len);

	return str;
}


internal_linkage void*
gb__string_realloc(gb_Allocator* a, void* ptr, gb_String_Size old_size, gb_String_Size new_size)
{
	if (!ptr) return gb_alloc(a, new_size);

	if (new_size < old_size)
		new_size = old_size;

	if (old_size == new_size)
		return ptr;

	void* new_ptr = gb_alloc(a, new_size);
	if (!new_ptr)
		return NULL;

	memcpy(new_ptr, ptr, old_size);

	gb_free(a, ptr);

	return new_ptr;
}



gb_String
gb_string_make_space_for(gb_String str, gb_String_Size add_len)
{
	gb_String_Size len = gb_string_length(str);
	gb_String_Size new_len = len + add_len;

	gb_String_Size available = gb_string_available_space(str);
	if (available >= add_len) /* Return if there is enough space left */
		return str;


	void* ptr = GB_STRING_HEADER(str);
	gb_String_Size old_size = sizeof(struct gb_String_Header) + gb_string_length(str) + 1;
	gb_String_Size new_size = sizeof(struct gb_String_Header) + new_len + 1;

	void* new_ptr = gb__string_realloc(GB_STRING_HEADER(str)->allocator, ptr, old_size, new_size);
	if (new_ptr == NULL)
		return NULL;
	str = cast(char*, GB_STRING_HEADER(new_ptr) + 1);

	gb__string_set_capacity(str, new_len);

	return str;
}

gb_String_Size
gb_string_allocation_size(gb_String const str)
{
	gb_String_Size cap = gb_string_capacity(str);
	return sizeof(gb_String_Header) + cap;
}


bool32
gb_strings_are_equal(gb_String const lhs, gb_String const rhs)
{
	gb_String_Size lhs_len = gb_string_length(lhs);
	gb_String_Size rhs_len = gb_string_length(rhs);
	if (lhs_len != rhs_len)
		return false;

	for (gb_String_Size i = 0; i < lhs_len; i++) {
		if (lhs[i] != rhs[i])
			return false;
	}

	return true;
}


gb_String
gb_string_trim(gb_String str, char const* cut_set)
{
	char* start;
	char* end;
	char* start_pos;
	char* end_pos;

	start_pos = start = str;
	end_pos   = end   = str + gb_string_length(str) - 1;

	while (start_pos <= end && strchr(cut_set, *start_pos))
		start_pos++;
	while (end_pos > start_pos && strchr(cut_set, *end_pos))
		end_pos--;

	gb_String_Size len = cast(gb_String_Size, (start_pos > end_pos) ? 0 : ((end_pos - start_pos)+1));

	if (str != start_pos)
		memmove(str, start_pos, len);
	str[len] = '\0';

	gb__string_set_length(str, len);

	return str;
}

gb_String gb_string_trim_space(gb_String str) { return gb_string_trim(str, " \t\r\n\v\f"); }



u32
gb_hash_adler32(void const* key, u32 num_bytes)
{
	const u32 MOD_ADLER = 65521;

	u32 a = 1;
	u32 b = 0;

	u8 const* bytes = cast(u8 const*, key);
	for (u32 i = 0; i < num_bytes; i++)
	{
		a = (a + bytes[i]) % MOD_ADLER;
		b = (b + a) % MOD_ADLER;
	}

	return (b << 16) | a;
}

global_variable const u32 GB_CRC32_TABLE[256] = {
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

global_variable const u64 GB_CRC64_TABLE[256] = {
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
gb_hash_crc32(void const* key, u32 num_bytes)
{
	u32 result = cast(u32, ~0);
	u8 const* c = cast(u8 const*, key);

	for (u32 remaining = num_bytes; remaining--; c++)
		result = (result >> 8) ^ (GB_CRC32_TABLE[(result ^ *c) & 0xff]);

	return ~result;
}

u64
gb_hash_crc64(void const* key, usize num_bytes)
{
	u64 result = cast(u64, ~0);
	u8 const* c = cast(u8 const*, key);
	for (usize remaining = num_bytes; remaining--; c++)
		result = (result >> 8) ^ (GB_CRC64_TABLE[(result ^ *c) & 0xff]);

	return ~result;
}


u32
gb_hash_fnv32(void const* key, usize num_bytes)
{
	u32 h = 0x811c9dc5;
	u8 const* buffer = cast(u8 const*, key);

	for (usize i = 0; i < num_bytes; i++)
		h = (h * 0x01000193) ^ buffer[i];

	return h;
}

u64
gb_hash_fnv64(void const* key, usize num_bytes)
{
	u64 h = 0xcbf29ce484222325ull;
	u8 const* buffer = cast(u8 const*, key);

	for (usize i = 0; i < num_bytes; i++)
		h = (h * 0x100000001b3ll) ^ buffer[i];

	return h;
}

u32
gb_hash_fnv32a(void const* key, usize num_bytes)
{
	u32 h = 0x811c9dc5;
	u8 const* buffer = cast(u8 const*, key);

	for (usize i = 0; i < num_bytes; i++)
		h = (h ^ buffer[i]) * 0x01000193;

	return h;
}

u64
gb_hash_fnv64a(void const* key, usize num_bytes)
{
	u64 h = 0xcbf29ce484222325ull;
	u8 const* buffer = cast(u8 const*, key);

	for (usize i = 0; i < num_bytes; i++)
		h = (h ^ buffer[i]) * 0x100000001b3ll;

	return h;
}


u32
gb_hash_murmur32(void const* key, u32 num_bytes, u32 seed)
{
	const u32 c1 = 0xcc9e2d51;
	const u32 c2 = 0x1b873593;
	const u32 r1 = 15;
	const u32 r2 = 13;
	const u32 m = 5;
	const u32 n = 0xe6546b64;

	u32 hash = seed;

	const usize nblocks = num_bytes / 4;
	const u32* blocks = cast(const u32*, key);
	for (usize i = 0; i < nblocks; i++)
	{
		u32 k = blocks[i];
		k *= c1;
		k = (k << r1) | (k >> (32 - r1));
		k *= c2;

		hash ^= k;
		hash = ((hash << r2) | (hash >> (32 - r2))) * m + n;
	}

	u8 const* tail = (cast(u8 const*, key)) + nblocks * 4;
	u32 k1 = 0;

	switch (num_bytes & 3)
	{
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

	hash ^= num_bytes;
	hash ^= (hash >> 16);
	hash *= 0x85ebca6b;
	hash ^= (hash >> 13);
	hash *= 0xc2b2ae35;
	hash ^= (hash >> 16);

	return hash;
}

#if defined(GB_ARCH_64_BIT)
	u64
	gb_hash_murmur64(void const* key, usize num_bytes, u64 seed)
	{
		const u64 m = 0xc6a4a7935bd1e995ULL;
		const s32 r = 47;

		u64 h = seed ^ (num_bytes * m);

		const u64* data = cast(const u64*, key);
		const u64* end = data + (num_bytes / 8);

		while (data != end)
		{
			u64 k = *data++;

			k *= m;
			k ^= k >> r;
			k *= m;

			h ^= k;
			h *= m;
		}

		u8 const* data2 = cast(u8 const*, data);

		switch (num_bytes & 7)
		{
		case 7: h ^= cast(u64, data2[6]) << 48;
		case 6: h ^= cast(u64, data2[5]) << 40;
		case 5: h ^= cast(u64, data2[4]) << 32;
		case 4: h ^= cast(u64, data2[3]) << 24;
		case 3: h ^= cast(u64, data2[2]) << 16;
		case 2: h ^= cast(u64, data2[1]) << 8;
		case 1: h ^= cast(u64, data2[0]);
			h *= m;
		};

		h ^= h >> r;
		h *= m;
		h ^= h >> r;

		return h;
	}
#elif GB_ARCH_32_BIT
	u64
	gb_hash_murmur64(void const* key, usize num_bytes, u64 seed)
	{
		const u32 m = 0x5bd1e995;
		const s32 r = 24;

		u32 h1 = cast(u32, seed) ^ cast(u32, num_bytes);
		u32 h2 = cast(u32, seed >> 32);

		const u32* data = cast(const u32*, key);

		while (num_bytes >= 8)
		{
			u32 k1 = *data++;
			k1 *= m;
			k1 ^= k1 >> r;
			k1 *= m;
			h1 *= m;
			h1 ^= k1;
			num_bytes -= 4;

			u32 k2 = *data++;
			k2 *= m;
			k2 ^= k2 >> r;
			k2 *= m;
			h2 *= m;
			h2 ^= k2;
			num_bytes -= 4;
		}

		if (num_bytes >= 4)
		{
			u32 k1 = *data++;
			k1 *= m;
			k1 ^= k1 >> r;
			k1 *= m;
			h1 *= m;
			h1 ^= k1;
			num_bytes -= 4;
		}

		switch (num_bytes)
		{
		case 3: h2 ^= cast(u8 const*, data)[2] << 16;
		case 2: h2 ^= cast(u8 const*, data)[1] <<  8;
		case 1: h2 ^= cast(u8 const*, data)[0] <<  0;
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

		u64 h = h1;

		h = (h << 32) | h2;

		return h;
	}
#else
	#error gb_hash_murmur64 function not supported on this architecture
#endif


/**********************************/
/*                                */
/* Hash                           */
/*                                */
/**********************************/


#if defined(GB_SYSTEM_WINDOWS)
	gb_Time
	gb_time_now()
	{
		/* NOTE(bill): std::chrono does not have a good enough precision in MSVC12
		 * and below. This may have been fixed in MSVC14 but unsure as of yet.
		 */

		/* Force the following code to run on first core */
		/* NOTE(bill, 2015): See
		 * http://msdn.microsoft.com/en-us/library/windows/desktop/ms644904(v=vs.85).aspx
		 */
		HANDLE currentThread   = GetCurrentThread();
		DWORD_PTR previousMask = SetThreadAffinityMask(currentThread, 1);

		/* Get the frequency of the performance counter */
		/* It is constant across the program's lifetime */
		local_persist LARGE_INTEGER s_frequency;
		QueryPerformanceFrequency(&s_frequency); /* TODO(bill): Is this fast enough? */

		/* Get the current time */
		LARGE_INTEGER t;
		QueryPerformanceCounter(&t);

		/* Restore the thread affinity */
		SetThreadAffinityMask(currentThread, previousMask);

		return gb_time_microseconds(1000000ll * t.QuadPart / s_frequency.QuadPart);
	}

	void
	gb_time_sleep(gb_Time t)
	{
		if (t.microseconds <= 0) return;

		/* Get the supported timer resolutions on this system */
		TIMECAPS tc;
		timeGetDevCaps(&tc, sizeof(TIMECAPS));
		/* Set the timer resolution to the minimum for the Sleep call */
		timeBeginPeriod(tc.wPeriodMin);

		/* Wait... */
		Sleep(gb_time_as_milliseconds(t));

		/* Reset the timer resolution back to the system default */
		timeBeginPeriod(tc.wPeriodMin);
	}

#else
	gb_Time
	gb_time_now()
	{
	#if defined(GB_SYSTEM_OSX)
		s64 t = cast(s64, mach_absolute_time());
		return gb_time_microseconds(t);
	#else
		struct timeval t;
		gettimeofday(&t, nullptr);

		return gb_time_microseconds((t.tv_sec * 1000000ll) + (t.tv_usec * 1ll));
	#endif
	}

	void
	gb_time_sleep(gb_Time t)
	{
		if (t.microseconds <= 0) return;

		struct timespec spec = {};
		spec.tv_sec = cast(s64, gb_time_as_seconds(t));
		spec.tv_nsec = 1000ll * (gb_time_as_microseconds(t) % 1000000ll);

		nanosleep(&spec, nullptr);
	}

#endif

gb_Time
gb_time_seconds(f32 s)
{
	gb_Time result;
	result.microseconds = cast(s64, s * 1000000ll);
	return result;
}

gb_Time
gb_time_milliseconsd(s32 ms)
{
	gb_Time result;
	result.microseconds = cast(s64, ms * 1000ll);
	return result;
}

gb_Time
gb_time_microseconds(s64 us)
{
	gb_Time result;
	result.microseconds = us;
	return result;
}

f32 gb_time_as_seconds(gb_Time t)      { return cast(f32, t.microseconds / 1000000.0f); }
s32 gb_time_as_milliseconds(gb_Time t) { return cast(s32, t.microseconds / 1000l); }
s64 gb_time_as_microseconds(gb_Time t) { return t.microseconds; }

/**********************************/
/*                                */
/* Time                           */
/*                                */
/**********************************/

/*
 * lhs <  rhs   -1
 * lhs == rhs    0
 * lhs >  rhs   +1
 */
s32
gb_time_cmp(gb_Time lhs, gb_Time rhs)
{
	if (lhs.microseconds < rhs.microseconds)
		return -1;
	if (lhs.microseconds == rhs.microseconds)
		return 0;
	return +1;
}

gb_Time
gb_time_add(gb_Time lhs, gb_Time rhs)
{
	lhs.microseconds += rhs.microseconds;
	return lhs;
}

gb_Time
gb_time_sub(gb_Time lhs, gb_Time rhs)
{
	lhs.microseconds -= rhs.microseconds;
	return lhs;
}

gb_Time
gb_time_mod(gb_Time lhs, gb_Time rhs)
{
	lhs.microseconds %= rhs.microseconds;
	return lhs;
}

gb_Time
gb_time_mul(gb_Time t, f32 s)
{
	gb_Time result = gb_time_seconds(gb_time_as_seconds(t) * s);
	return result;
}

gb_Time
gb_time_div(gb_Time t, f32 s)
{
	gb_Time result = gb_time_seconds(gb_time_as_seconds(t) / s);
	return result;
}

f32
gb_time_div_time(gb_Time num, gb_Time dom)
{
	f32 result = num.microseconds / cast(f32, dom.microseconds);
	return result;
}


#if defined(__cplusplus)
}
#endif /* extern "C" */

#endif /* GB_IMPLEMENTATION */


