/* gb.h - v0.01 - public domain C89/C99 helper library - no warranty implied; use at your own risk */
/* (Experimental) A C89/99 helper library geared towards game development */

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

#ifndef GB_INCLUDE_GB_H
#define GB_INCLUDE_GB_H

#if defined(__cplusplus)
extern "C" {
#endif /* extern "C" */

/* NOTE(bill): Because static means three different things in C/C++
 *             Great Design(!)
 */
#ifndef global_variable
#define global_variable  static
#define internal_linkage static
#define local_persist    static
#endif

/* Example for static defines

	global_variable const f32 TAU = 6.283185f;
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

/*
 * System OS
 */
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
	#define GB_IS_BIG_EDIAN    (!*(unsigned char *)&(unsigned short){1})
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

#include <math.h>
#include <stdio.h>
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

	#include <intrin.h>
#else
	#include <pthread.h>
	#include <sys/time.h>
#endif

#ifndef true
#define true  (0==0)
#define false (0!=0)
#endif

#ifndef NULL
#define NULL ((void *)0)
#endif

#if !defined(GB_NO_DEBUG)
	/* TODO(bill): Do a better assert */
	#define GB_ASSERT(cond) assert(cond)
#else
	#define GB_ASSERT(cond) ((void)(cond))
#endif

#define GB_STATIC_ASSERT(COND, MSG) typedef char gb__static_assertion_##MSG[(!!(COND))*2-1]
// token pasting madness:
#define GB_COMPILE_TIME_ASSERT3(cond, line) GB_STATIC_ASSERT(cond, static_assertion_at_line_##line)
#define GB_COMPILE_TIME_ASSERT2(cond, line) GB_COMPILE_TIME_ASSERT3(cond, line)
#define GB_COMPILE_TIME_ASSERT(cond)        GB_COMPILE_TIME_ASSERT2(cond, __LINE__)
/* snprintf_msvc */
#if defined(_MSC_VER)
	int gb__vsnprintf_compatible(char *buffer, size_t size, const char *format, va_list args)
	{
		int result = -1;
		if (size > 0) result = _vsnprintf_s(buffer, size, _TRUNCATE, format, args);
		if (result == -1) return _vscprintf(format, args);
		return result;
	}

	int gb__snprintf_compatible(char *buffer, size_t size, const char *format, ...)
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
#endif

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
	typedef uint8_t  u8;
	typedef  int8_t  s8;
	typedef uint16_t u16;
	typedef  int16_t s16;
	typedef uint32_t u32;
	typedef  int32_t s32;
	typedef uint64_t u64;
	typedef  int64_t s64;
#endif

GB_COMPILE_TIME_ASSERT(sizeof(s8)==1);
GB_COMPILE_TIME_ASSERT(sizeof(s16)==2);
GB_COMPILE_TIME_ASSERT(sizeof(s32)==4);
GB_COMPILE_TIME_ASSERT(sizeof(s64)==8);

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

/* Casts */
/* NOTE(bill): Easier to grep/find for casts */
#define cast(Type, var) ((Type)(var))

/*
 * Memory
 */

#define ARRAY_COUNT(x) ((sizeof(x)/sizeof(0[x])) / (cast(size_t, !(sizeof(x) % sizeof(0[x])))))

#define KILOBYTES(x) (         (x) * 1024ll)
#define MEGABYTES(x) (KILOBYTES(x) * 1024ll)
#define GIGABYTES(x) (MEGABYTES(x) * 1024ll)
#define TERABYTES(x) (GIGABYTES(x) * 1024ll)


typedef struct Mutex {
#if defined(GB_SYSTEM_WINDOWS)
	HANDLE win32_mutex;
#else
	pthread_mutex_t posix_mutex;
#endif
} Mutex;

Mutex  mutex_make(void);
void   mutex_destroy(Mutex *mutex);
void   mutex_lock(Mutex *mutex);
bool32 mutex_try_lock(Mutex *mutex);
void   mutex_unlock(Mutex *mutex);


typedef struct Atomic32 { u32 nonatomic; } Atomic32;
typedef struct Atomic64 { u64 nonatomic; } Atomic64;

u32  atomic32_load(const volatile Atomic32 *a);
void atomic32_store(volatile Atomic32 *a, u32 value);
u32 atomic32_compare_exchange_strong(volatile Atomic32 *a, u32 expected, u32 desired);
u32 atomic32_exchanged(volatile Atomic32 *a, u32 desired);
u32 atomic32_fetch_add(volatile Atomic32 *a, s32 desired);
u32 atomic32_fetch_and(volatile Atomic32 *a, u32 desired);
u32 atomic32_fetch_or(volatile Atomic32 *a, u32 desired);

u64  atomic64_load(const volatile Atomic64 *a);
void atomic64_store(volatile Atomic64 *a, u64 value);
u64 atomic64_compare_exchange_strong(volatile Atomic64 *a, u64 expected, u64 desired);
u64 atomic64_exchanged(volatile Atomic64 *a, u64 desired);
u64 atomic64_fetch_add(volatile Atomic64 *a, s64 desired);
u64 atomic64_fetch_and(volatile Atomic64 *a, u64 desired);
u64 atomic64_fetch_or(volatile Atomic64 *a, u64 desired);





typedef struct Semaphore {
#if defined(GB_SYSTEM_WINDOWS)
	HANDLE win32_handle;
#else
	Mutex          mutex;
	pthread_cond_t cond;
	s32            count;
#endif
} Semaphore;

Semaphore semaphore_make(void);
void semaphore_destroy(Semaphore *s);
void semaphore_post(Semaphore *s);
void semaphore_post_count(Semaphore *s, u32 count);
void semaphore_wait(Semaphore *s);





typedef void(Thread_Procedure)(void*);

typedef struct Thread {
#if defined(GB_SYSTEM_WINDOWS)
	HANDLE win32_handle;
#else
	pthread_t posix_handle;
#endif

	Thread_Procedure *functions;
	void             *data;

	Semaphore semaphore;
	usize     stack_size;
	bool32    is_running;
} Thread;

Thread thread_make(void);
void   thread_destroy(Thread* t);
void   thread_start(Thread* t, Thread_Procedure *func, void *data);
void   thread_start_with_stack(Thread* t, Thread_Procedure *func, void *data, usize stack_size);
void   thread_join(Thread* t);
bool32 thread_is_running(const Thread* t); /* NOTE(bill): Can this be just pass by value? */

u32 thread_current_id(void);





/* Default aligment for memory allocations */
#ifndef GB_DEFAULT_ALIGNMENT
#define GB_DEFAULT_ALIGNMENT 8
#endif

typedef struct Allocator {
	/* Allocates the specified amount of memory aligned to the specified alignment */
	void *(*alloc)(struct Allocator *a, usize size, usize align);

	/* Deallocates an allocation made with alloc() */
	void (*dealloc)(struct Allocator *a, void *ptr);

	/* Returns the amount of usuable memory allocated at `ptr`.
	 * If the allocator does not support tracking of the allocation size,
	 * the function will return -1
	 */
	s64 (*allocated_size)(struct Allocator *a, const void *ptr);

	/* Returns the total amount of memory allocated by this allocator */
	/* If the allocator does not track memory, the function will return -1 */
	s64 (*total_allocated)(struct Allocator *a);
} Allocator;

void *alloc_align(Allocator *a, usize size, usize align) { GB_ASSERT(a != NULL); return a->alloc(a, size, align); }
void *alloc(Allocator *a, usize size) { GB_ASSERT(a != NULL); return alloc_align(a, size, GB_DEFAULT_ALIGNMENT); }

#define alloc_struct(allocator, Type)       cast(Type *, alloc_align(allocator, sizeof(Type), alignof(Type)))
#define alloc_array(allocator, Type, count) cast(Type *, alloc_align(allocator, sizeof(Type)*(count), alignof(Type)))

void dealloc(Allocator *a, void *ptr) { GB_ASSERT(a != NULL); if (ptr) a->dealloc(a, ptr); }

s64 allocated_size(Allocator *a, const void *ptr) { GB_ASSERT(a != NULL); return a->allocated_size(a, ptr); }
s64 total_allocated(Allocator *a) { GB_ASSERT(a != NULL); return a->total_allocated(a); }




typedef struct Heap {
	Allocator base; /* NOTE(bill): Must be first into order to allow for polymorphism */

	Mutex  mutex;
	bool32 use_mutex;
	s64 total_allocated_count;
	s64 allocation_count;

#if defined(GB_SYSTEM_WINDOWS)
	HANDLE win32_heap_handle;
#endif
} Heap;

Heap heap_make(bool32 use_mutex);
void heap_destroy(Heap *heap);


typedef struct Arena {
	Allocator base; /* NOTE(bill): Must be first into order to allow for polymorphism */

	Allocator *backing;
	void      *physical_start;
	s64        total_size;
	s64        total_allocated_count;
	s64        temp_count;
} Arena;


Arena arena_make_from_backing(Allocator *backing, usize size);
Arena arena_make_from_pointer(void *start, usize size);
void  arena_destroy(Arena *arena);
void  arena_clear(Arena *arena);


typedef struct Temporary_Arena_Memory {
	Arena *arena;
	s64    original_count;
} Temporary_Arena_Memory;

Temporary_Arena_Memory make_temporary_arena_memory(Arena *arena);
void temporary_arena_memory_free(Temporary_Arena_Memory t);


typedef struct Pool {
	Allocator base; /* NOTE(bill): Must be first into order to allow for polymorphism */

	Allocator *backing;

	void *physical_start;
	void *free_list;

	usize block_size;
	usize block_align;
	s64   total_size;
} Pool;

Pool pool_make(Allocator *backing, usize num_blocks, usize block_size);
Pool pool_make_align(Allocator *backing, usize num_blocks, usize block_size, usize block_align);
void pool_destroy(Pool *pool);



/*
 * Memory
 */
void *align_forward(void *ptr, usize align);

#define zero_struct(element) ((void)zero_size(&(element), sizeof(element)))
#define zero_array(ptr, Type, count) cast(Type, zero_size((ptr), sizeof(Type)*(count)))
void   *zero_size(void *ptr, usize bytes);



/*
 * Array
 */
/*
 * Array structure:
 *
 *
 * | Allocator * | usize count | usize capacity | char * |
 *                                              |
 *                                               `-- Returned pointer
 */

typedef struct Array_Header {
	Allocator *allocator;
	usize      count;
	usize      capacity;
} Array_Header;

/* TODO(bill): Implement a c style array maybe like stb/stretchy_buffer.h but with a custom allocator */

#define array_header(arr)           (cast(Array_Header *, arr) - 1)

#define array_make_count(allocator, Type, count) /* TODO(bill): */
#define array_make(allocator, Type) (array_make_count(allocator, Type, 0))
#define array_free(arr)             (dealloc(array_header(arr)->allocator, array_header(arr)))

#define array_allocator(arr) (array_header(arr)->allocator)
#define array_count(arr)     (array_header(arr)->count)
#define array_capacity(arr)  (array_header(arr)->capacity)

#define array_append(arr, item)               /* TODO(bill): */
#define array_append_array(arr, items, count) /* TODO(bill): */

#define array_pop(arr)  (GB_ASSERT(array_header(arr)->count > 0), array_header(arr)->count--)

#define array_clear(arr) (array_header(arr)->count = 0)
#define array_resize(arr, count)          /* TODO(bill): */
#define array_reserve(arr, capacity)      /* TODO(bill): */
#define array_set_capacity(arr, capacity) /* TODO(bill): */
#define array_grow(arr, min_capacity)     /* TODO(bill): */


/*
 * String - c compatible strings
 */

typedef char * String;

typedef u32 String_Size;

typedef struct String_Header {
	Allocator  *allocator;
	String_Size length;
	String_Size capacity;
} String_Header;

#define GB_STRING_HEADER(str) (cast(String_Header *, str) - 1)

String string_make(Allocator *a, const char* str);
String string_make_length(Allocator *a, const void* str, String_Size num_bytes);
void   string_free(String str);

String string_duplicate(Allocator *a, const String str);

String_Size string_length(const String str);
String_Size string_capacity(const String str);
String_Size string_available_space(const String str);

void string_clear(String str);

String string_append_string(String *str, const String other);
String string_append_string_length(String *str, const void *other, String_Size num_bytes);
String string_append_cstring(String *str, const char *other);

String string_set(String str, const char *cstr);

String      string_make_space_for(String str, String_Size add_len);
String_Size string_allocation_size(const String str);

bool32 strings_are_equal(const String lhs, const String rhs);

String string_trim(String str, const char *cut_set);
String string_trim_space(String str); /* Whitespace ` \t\r\n\v\f` */



/*
 * Hash
 */

u32 hash_adler32(const void *ket, u32 num_bytes);

u32 hash_crc32(const void* key, u32 num_bytes);
u64 hash_crc64(const void* key, usize num_bytes);

u32 hash_fnv32(const void* key, usize num_bytes);
u64 hash_fnv64(const void* key, usize num_bytes);
u32 hash_fnv32a(const void* key, usize num_bytes);
u64 hash_fnv64a(const void* key, usize num_bytes);

u32 hash_murmur32(const void* key, u32 num_bytes, u32 seed);
u64 hash_murmur64(const void* key, usize num_bytes, u64 seed);

/*
 * Time
 */

/* TODO(bill): How should I this  */
typedef struct Time { s64 microseconds; } Time;

#define TIME_ZERO (cast(Time, {0}))

Time time_now(void);
void time_sleep(Time time);

Time time_seconds(f32 s);
Time time_milliseconsd(s32 ms);
Time time_microseconds(s64 us);

f32 time_as_seconds(Time t);
s32 time_as_milliseconds(Time t);
s64 time_as_microseconds(Time t);

#if defined(__cplusplus)
}
#endif /* extern "C" */

#if defined(GB_IMPLEMENTATION)


#endif /* GB_IMPLEMENTATION */

#endif
