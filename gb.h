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
	gbTypes_Are_Like_This (None core types)
	gb_functions_and_variables_like_this
	Prefer // Comments
	Never use _t suffix for types
	Private "things" will have prefix `gbpriv` (e.g. Things the user shouldn't be touch if they don't know what they are doing)


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


#if defined(_WIN32) && !defined(__MINGW32__)
	#ifndef _CRT_SECURE_NO_WARNINGS
	#define _CRT_SECURE_NO_WARNINGS
	#endif
#endif

#include <assert.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h> // NOTE(bill): For memcpy, memmove, memcmp, etc.

#if !defined(GB_NO_STDLIB)
#include <stdlib.h>
#endif

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
		#define GB_DEF GB_EXTERN
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
typedef i32 b32; // NOTE(bill): Use this in structs if a boolean _is_ needed to be aligned well
typedef i8  b8;  // TODO(bill): Do I really want a 8-bit boolean ever?

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
	#define inline   __inline
	#define restrict __restrict
#endif


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
extern "C++" {
#ifndef gb_align_of
	#if __cplusplus >= 201103L
		#define gb_align_of(Type) (isize)alignof(Type)
	#else
		// NOTE(bill): Fucking Templates!
		template <typename T> struct gbAlignment_Trick { char c; T member; };
		#define gb_align_of(Type) gb_offset_of(gbAlignment_Trick<Type>, member)
	#endif
#endif
}
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
#if defined(__cplusplus)
extern "C++" {
	// NOTE(bill): Stupid fucking templates
	template <typename T> struct gbRemove_Reference       { typedef T Type; };
	template <typename T> struct gbRemove_Reference<T &>  { typedef T Type; };
	template <typename T> struct gbRemove_Reference<T &&> { typedef T Type; };

	// NOTE(bill): "Move" semantics - invented because the C++ committee are idiots (as a collective not as indiviuals (well a least some aren't))
	template <typename T> inline T &&gb_forward_ownership(typename gbRemove_Reference<T>::Type &t)  { return static_cast<T &&>(t); }
	template <typename T> inline T &&gb_forward_ownership(typename gbRemove_Reference<T>::Type &&t) { return static_cast<T &&>(t); }
	template <typename T> inline T &&gb_move_ownership(T &&t) { return static<typename gbRemove_Reference<T>::Type &&>(t); }

	template <typename F>
	struct gbprivDefer {
		F f;
		gbprivDefer(F &&f) : f(gb_forward_ownership<F>(f)) {}
		~gbprivDefer() { f(); }
	};
	template <typename F> gbprivDefer<F> gb_defer_func(F &&f) { return gbprivDefer<F>(gb_forward_ownership<F>(f)); }

	#ifndef defer
	#define GB_DEFER_1(x, y) x##y
	#define GB_DEFER_2(x, y) GB_DEFER_1(x, y)
	#define GB_DEFER_3(x)    GB_DEFER_2(x, __COUNTER__)
	#define defer(code)      auto GB_DEFER_3(_defer_) = gb_defer_func([&](){code;})
	#endif
}
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




#ifndef gb_min
#define gb_min(a, b) ((a) < (b) ? (a) : (b))
#endif

#ifndef gb_max
#define gb_max(a, b) ((a) > (b) ? (a) : (b))
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

GB_DEF int gb_printf(char const *fmt, ...) GB_PRINTF_ARGS(1);
GB_DEF int gb_printf_var(char const *fmt, va_list v);
GB_DEF int gb_fprintf(FILE *f, char const *fmt, ...) GB_PRINTF_ARGS(2);
GB_DEF int gb_fprintf_var(FILE *f, char const *fmt, va_list v);
GB_DEF int gb_sprintf(char const *fmt, ...) GB_PRINTF_ARGS(1);
GB_DEF int gb_sprintf_var(char const *fmt, va_list v);
GB_DEF int gb_snprintf(char *str, isize n, char const *fmt, ...) GB_PRINTF_ARGS(3);
GB_DEF int gb_snprintf_var(char *str, isize n, char const *fmt, va_list v);




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

GB_DEF void gb_zero_size(void *ptr, isize size);

#ifndef gb_zero_struct
#define gb_zero_struct(t) gb_zero_size((t), gb_size_of(*(t))) // NOTE(bill): Pass pointer of struct
#define gb_zero_array(a, count) gb_zero_size((a), gb_size_of((a)[0])*count)
#endif

GB_DEF void *gb_memcpy(void *dest, void const *source, isize size);
GB_DEF void *gb_memmove(void *dest, void const *source, isize size);
GB_DEF void *gb_memset(void *data, u8 byte_value, isize size);



#ifndef gb_kilobytes
#define gb_kilobytes(x) (            (x) * (i64)(1024))
#define gb_megabytes(x) (gb_kilobytes(x) * (i64)(1024))
#define gb_gigabytes(x) (gb_megabytes(x) * (i64)(1024))
#define gb_terabytes(x) (gb_gigabytes(x) * (i64)(1024))
#endif


#if 0
// NOTE(bill): Is it possible in C (easily) to create a mutex where the zero value is
// an unlocked mutex be default and also does not require make/destroy ever?
// Mutex
typedef struct gbMutex { void *handle; } gbMutex;

GB_DEF gbMutex gb_mutex_make(void);
GB_DEF void gb_mutex_destroy(gbMutex *m);
GB_DEF void gb_mutex_lock(gbMutex *m);
GB_DEF void gb_mutex_try_lock(gbMutex *m);
GB_DEF void gb_mutex_unlock(gbMutex *m);
#endif



////////////////////////////////////////////////////////////////
//
// Custom Allocation
//
//

typedef enum gbAllocation_Type {
	GB_ALLOCATION_TYPE_ALLOC,
	GB_ALLOCATION_TYPE_FREE,
	GB_ALLOCATION_TYPE_FREE_ALL,
	GB_ALLOCATION_TYPE_RESIZE,
} gbAllocation_Type;

// NOTE(bill): This is useful so you can define an allocator of the same type and parameters
#define GB_ALLOCATOR_PROC(name)                          \
void *name(void *allocator_data, gbAllocation_Type type, \
           isize size, isize alignment,                  \
           void *old_memory, isize old_size,             \
           u64 options)
;typedef GB_ALLOCATOR_PROC(gbAllocator_Proc);

typedef struct gbAllocator {
	gbAllocator_Proc *proc;
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





#if !defined(GB_NO_STDLIB)

GB_DEF gbAllocator gb_malloc_allocator(void);
GB_DEF GB_ALLOCATOR_PROC(gb_malloc_allocator_proc);

#endif





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



typedef struct gbTemp_Arena_Memory {
	gbArena *arena;
	isize original_count;
} gbTemp_Arena_Memory;

GB_DEF gbTemp_Arena_Memory gb_temp_arena_memory_begin(gbArena *arena);
GB_DEF void gb_temp_arena_memory_end(gbTemp_Arena_Memory tmp_mem);









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

GB_DEF isize gb_strlen(char const *str);
GB_DEF char *gb_strncpy(char *dest, char const *source, isize len);
GB_DEF int gb_strncmp(char const *s1, char const *s2, isize len);


////////////////////////////////////////////////////////////////
//
// Windows UTF-8 Handling
//
//
// Windows doesn't handle 8 bit filenames well ('cause Micro$hit)

GB_DEF char16 *gb_from_utf8(char16 *buffer, char *str, isize len);
GB_DEF char   *gb_to_utf8(char *buffer, char16 *str, isize len);




////////////////////////////////////////////////////////////////
//
// gbString - C Read-Only-Compatible
//
//


// Pascal like strings in C
typedef char *gbString;


// This is stored at the beginning of the string
// NOTE(bill): If you only need a small string, just use a standard c string or change the size
typedef struct gbString_Header {
	gbAllocator allocator;
	isize length;
	isize capacity;
} gbString_Header;

#define GB_STRING_HEADER(str) ((gbString_Header *)str - 1)

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
// Dynamic Array (POD Types)
//
//

// NOTE(bill): I know this is a macro hell but C is an old (and shit) language with no proper arrays
// Also why the fuck not?! It fucking works! And it has custom allocation, which is already better than C++!

// NOTE(bill): Typedef every array or you get anonymous structures everywhere!
// e.g. typedef gbArray(int) gb_Int_Array;
#ifndef GB_ARRAY_TYPE
#define GB_ARRAY_TYPE

#define gbArray(Type) struct { \
	gbAllocator allocator;     \
	isize count;               \
	isize capacity;            \
	Type *data;                \
}


typedef gbArray(void) gbVoid_Array; // NOTE(bill): Used to generic stuff

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

#define gb_array_set_capacity(array, capacity) gbprivarray_set_capacity((array), (capacity), gb_size_of((array)->data[0]))
// NOTE(bill): Do not use directly the thing below, use the macro
GB_DEF void gbprivarray_set_capacity(void *array, i64 capacity, isize element_size);

// TODO(bill): Decide on a decent growing formula for gbArray
// Is 2*c+8 good enough
#define gb_array_grow(x, min_capacity) do { \
	i64 capacity = 2*(x)->capacity + 8;     \
	if (capacity < min_capacity)            \
		capacity = min_capacity;            \
	gb_array_set_capacity(x, capacity);     \
} while (0)


#define gb_array_append(x, item) do { \
	if ((x)->capacity < (x)->count+1) \
		gb_array_grow(x, 0);          \
	(x)->data[(x)->count++] = item;   \
} while (0)

#define gb_array_appendv(x, items, item_count) do {                             \
	GB_ASSERT(gb_size_of(items[0]) == gb_size_of((x)->data[0]));                \
	if ((x)->capacity < (x)->count+item_count)                                  \
		gb_array_grow(x, (x)->count+item_count);                                \
	gb_memcpy((x)->data[a->count], items, gb_size_of((x)->data[0])*item_count); \
	(x)->count += item_count;                                                   \
} while (0)



#define gb_array_pop(x) do { GB_ASSERT((x)->count > 0); (x)->count--; } while (0)
#define gb_array_clear(x) do { (x)->count = 0; } while (0)

#define gb_array_resize(x, count) do {    \
	if ((x)->capacity < count)            \
		gb_array_grow(x, count);          \
	(x)->count = count;                   \
} while (0)


#define gb_array_reserve(x, new_capacity) do {  \
	if ((x)->capacity < new_capacity)           \
		gb_array_set_capacity(x, new_capacity); \
} while (0)


#endif /* GB_ARRAY_TYPE */








////////////////////////////////////////////////////////////////
//
//
// Hash_Table (POD Types)
//
//


// TODO(bill): Hash Table and make it decent!!!

// NOTE(bill): All keys are u64
#ifndef GB_HASH_TABLE_TYPE
#define GB_HASH_TABLE_TYPE

#define gbHash_Table_Entry(Type) struct { \
	u64 key;                              \
	Type value;                           \
}

#define gbHash_Table(Type) struct {            \
	gbArray(isize) hashes;                     \
	gbArray(gbHash_Table_Entry(Type)) entries; \
}

#define gb_hash_table_init(h, allocator) do { \
	gb_array_init((h)->hashes, allocator);    \
	gb_array_init((h)->entries, allocator);   \
} while (0)

#define gb_hash_table_free(h) do { \
	gb_free(&(h)->hashes);         \
	gb_free(&(h)->entries);        \
} while (0)



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





#endif /* GB_HASH_TABLE_TYPE */



////////////////////////////////////////////////////////////////
//
// File Handling
//
//

typedef struct gbFile {
	FILE *handle; // File to fread/fwrite
} gbFile;

GB_DEF b32 gb_file_create(gbFile *file, char const *filepath); // TODO(bill): Give file permissions
GB_DEF b32 gb_file_open(gbFile *file, char const *filepath);
GB_DEF b32 gb_file_close(gbFile *file);
GB_DEF b32 gb_file_read_at(gbFile *file, void *buffer, isize size, i64 offset);
GB_DEF b32 gb_file_write_at(gbFile *file, void const *buffer, isize size, i64 offset);
GB_DEF i64 gb_file_size(gbFile *file);


typedef struct gbFile_Contents {
	void *data;
	isize size;
} gbFile_Contents;

GB_DEF gbFile_Contents gb_read_entire_file_contents(gbAllocator a, char const *filepath, b32 zero_terminate);


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
////////////////////////////////////////////////////////////////

#if defined(GB_IMPLEMENTATION)

#if defined(__cplusplus)
extern "C" {
#endif


int
gb_printf(char const *fmt, ...)
{
	int res;
	va_list v;
	va_start(v, fmt);
	res = gb_fprintf_var(stdout, fmt, v);
	va_end(v);
	return res;
}


int
gb_fprintf(FILE *f, char const *fmt, ...)
{
	int res;
	va_list v;
	va_start(v, fmt);
	res = gb_fprintf_var(stdout, fmt, v);
	va_end(v);
	return res;
}

int
gb_sprintf(char const *fmt, ...)
{
	gb_local_persist char buffer[1024];
	int res;
	va_list v;
	va_start(v, fmt);
	res = gb_snprintf_var(buffer, gb_size_of(buffer), fmt, v);
	va_end(v);
	return res;
}

int
gb_snprintf(char *str, isize n, char const *fmt, ...)
{
	int res;
	va_list v;
	va_start(v,fmt);
	res = gb_snprintf_var(str, n, fmt, v);
	va_end(v);
	return res;
}


gb_inline int gb_printf_var(char const *fmt, va_list v) { return gb_fprintf_var(stdout, fmt, v); }
gb_inline int gb_fprintf_var(FILE *f, char const *fmt, va_list v) { return vfprintf(f, fmt, v); }

gb_inline int
gb_sprintf_var(char const *fmt, va_list v)
{
	gb_local_persist char buffer[1024];
	int res;
	res = gb_snprintf_var(buffer, gb_size_of(buffer), fmt, v);
	return res;
}

gb_inline int
gb_snprintf_var(char *str, isize n, char const *fmt, va_list v)
{
	int res;
#if defined(_WIN32)
	res = _vsnprintf(str, n, fmt, v);
#else
	res = vsnprintf(str, n, fmt, v)
#endif
	if (n) str[n-1] = 0;
	// NOTE(bill): Unix returns length output would require, Windows returns negative when truncated.
	return (res >= n || res < 0) ? -1 : res;
}












void
gb_assert_handler(char const *condition, char const *file, i64 line, char const *msg, ...)
{
	gb_fprintf(stderr, "%s:%d: Assert Failure: ", file, cast(int)line);
	if (condition)
		gb_fprintf(stderr, "`%s` ", condition);

	if (msg) {
		va_list args;
		va_start(args, msg);
		gb_fprintf(stderr, msg, args);
		va_end(args);
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

gb_inline void gb_zero_size(void *ptr, isize size) { gb_memset(ptr, 0, size); }


gb_inline void *gb_memcpy(void *dest, void const *source, isize size)  { return memcpy(dest, source, size);     }
gb_inline void *gb_memmove(void *dest, void const *source, isize size) { return memmove(dest, source, size);    }
gb_inline void *gb_memset(void *data, u8 byte_value, isize size)       { return memset(data, byte_value, size); }





gb_inline void *gb_alloc_align(gbAllocator a, isize size, isize alignment) { return a.proc(a.data, GB_ALLOCATION_TYPE_ALLOC, size, alignment, NULL, 0, 0); }
gb_inline void *gb_alloc(gbAllocator a, isize size)                        { return gb_alloc_align(a, size, GB_DEFAULT_MEMORY_ALIGNMENT); }
gb_inline void  gb_free(gbAllocator a, void *ptr)                          { a.proc(a.data, GB_ALLOCATION_TYPE_FREE, 0, 0, ptr, 0, 0); }
gb_inline void  gb_free_all(gbAllocator a)                                 { a.proc(a.data, GB_ALLOCATION_TYPE_FREE_ALL, 0, 0, NULL, 0, 0); }
gb_inline void *gb_resize(gbAllocator a, void *ptr, isize old_size, isize new_size)        { return gb_resize_align(a, ptr, old_size, new_size, GB_DEFAULT_MEMORY_ALIGNMENT); }
gb_inline void *gb_resize_align(gbAllocator a, void *ptr, isize old_size, isize new_size, isize alignment) { return a.proc(a.data, GB_ALLOCATION_TYPE_RESIZE, new_size, alignment, ptr, old_size, 0); };

gb_inline void *gb_alloc_copy(gbAllocator a, void const *src, isize size) { return gb_memcpy(gb_alloc(a, size), src, size); }
gb_inline void *gb_alloc_copy_align(gbAllocator a, void const *src, isize size, isize alignment) { return gb_memcpy(gb_alloc_align(a, size, alignment), src, size); }

gb_inline char *
gb_alloc_cstring(gbAllocator a, char const *str)
{
	char *result;
	isize len = gb_strlen(str);
	result = cast(char *)gb_alloc_copy(a, str, len+1);
	result[len] = '\0';
	return result;
}










#if !defined(GB_NO_STDLIB)

gb_inline gbAllocator
gb_malloc_allocator(void)
{
	gbAllocator allocator;
	allocator.proc = gb_malloc_allocator_proc;
	allocator.data = NULL; // N/A
	return allocator;
}

GB_ALLOCATOR_PROC(gb_malloc_allocator_proc)
{
	gb_unused(options);

	switch (type) {
	case GB_ALLOCATION_TYPE_ALLOC: {
		#if defined(_MSC_VER)
			return _aligned_malloc(size, alignment);
		#else
			void *ptr = NULL;
			void *original_block; // Original block
			void **aligned_block; // Aligned block
			isize offset = (alignment-1) + sizeof(void *);
			original_block = cast(void *)malloc(size + offset);
			if (original_block) {
				uintptr t = (cast(uintptr)original_block + offset) & ~(alignment-1);
				aligned_block = cast(void **)t - 1;
				aligned_block[-1] = original_block;
				ptr = cast(void *)aligned_block;
			}
			return ptr;
		#endif
	} break;

	case GB_ALLOCATION_TYPE_FREE: {
		#if defined(_MSC_VER)
			_aligned_free(old_memory);
		#else
			free((cast(void **)old_memory)[-1]);
		#endif

	} break;

	case GB_ALLOCATION_TYPE_FREE_ALL:
		// N/A
		break;

	case GB_ALLOCATION_TYPE_RESIZE: {
		gbAllocator a = gb_malloc_allocator();
		if (!old_memory) return gb_alloc_align(a, size, alignment);

		if (size < old_size)
			size = old_size;

		if (old_size == size) {
			return old_memory;
		} else {
#if defined(_MSC_VER)
			return _aligned_realloc(old_memory, size, alignment);
#else
			void *new_memory = gb_alloc_align(a, size, alignment);
			if (!new_memory) return NULL;
			gb_memmove(new_memory, old_memory, gb_min(size, old_size));
			gb_free(a, old_memory);
			return new_memory;
#endif
		}
	} break;
	}

	return NULL; // NOTE(bill): Default return value
}

#endif




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
	case GB_ALLOCATION_TYPE_ALLOC: {
		void *ptr;
		isize actual_size = size + alignment;

		// NOTE(bill): Out of memory
		if (arena->total_allocated + actual_size > cast(isize)arena->total_size)
			return NULL;

		ptr = gb_align_forward(cast(u8 *)arena->physical_start + arena->total_allocated, alignment);
		arena->total_allocated += actual_size;
		return ptr;
	} break;

	case GB_ALLOCATION_TYPE_FREE:
		// NOTE(bill): Free all at once
		// NOTE(bill): Use Temp_Arena_Memory if you want to free a block
		// TODO(bill): Free it if it's on top of the stack
		break;

	case GB_ALLOCATION_TYPE_FREE_ALL:
		arena->total_allocated = 0;
		break;

	case GB_ALLOCATION_TYPE_RESIZE: {
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


gb_inline gbTemp_Arena_Memory
gb_temp_arena_memory_begin(gbArena *arena)
{
	gbTemp_Arena_Memory tmp;
	tmp.arena = arena;
	tmp.original_count = arena->total_allocated;
	arena->temp_count++;
	return tmp;
}

gb_inline void
gb_temp_arena_memory_end(gbTemp_Arena_Memory tmp)
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
	u8 *data, *curr;
	uintptr *end;

	gb_zero_struct(pool);

	pool->backing = backing;
	pool->block_size = block_size;
	pool->block_align = block_align;

	actual_block_size = block_size + block_align;
	pool_size = num_blocks * actual_block_size;

	data = cast(u8 *)gb_alloc_align(backing, pool_size, block_align);

	// Init intrusive freelist
	curr = data;
	for (block_index = 0; block_index < num_blocks-1; block_index++) {
		uintptr *next = cast(uintptr *)curr;
		*next = cast(uintptr)curr + actual_block_size;
		curr += actual_block_size;
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
	case GB_ALLOCATION_TYPE_ALLOC: {
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

	case GB_ALLOCATION_TYPE_FREE: {
		uintptr *next;
		if (old_memory == NULL) return NULL;

		next = cast(uintptr *)old_memory;
		*next = cast(uintptr)pool->free_list;
		pool->free_list = old_memory;
		pool->total_size -= pool->block_size;
	} break;

	case GB_ALLOCATION_TYPE_FREE_ALL:
		// TODO(bill):
		break;

	case GB_ALLOCATION_TYPE_RESIZE:
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














gb_inline void gbprivstring_set_length(gbString str, isize len) { GB_STRING_HEADER(str)->length = len; }
gb_inline void gbprivstring_set_capacity(gbString str, isize cap) { GB_STRING_HEADER(str)->capacity = cap; }


gb_inline gbString
gb_string_make(gbAllocator a, char const *str)
{
	isize len = str ? gb_strlen(str) : 0;
	return gb_string_make_length(a, str, len);
}

gbString
gb_string_make_length(gbAllocator a, void const *init_str, isize num_bytes)
{
	isize header_size = gb_size_of(gbString_Header);
	void *ptr = gb_alloc(a, header_size + num_bytes + 1);

	gbString str;
	gbString_Header *header;

	if (!init_str) gb_zero_size(ptr, header_size + num_bytes + 1);
	if (ptr == NULL) return NULL;

	str = cast(char *)ptr + header_size;
	header = GB_STRING_HEADER(str);
	header->allocator = a;
	header->length    = num_bytes;
	header->capacity  = num_bytes;
	if (num_bytes && init_str)
		gb_memcpy(str, init_str, num_bytes);
	str[num_bytes] = '\0';

	return str;
}

gb_inline void
gb_string_free(gbString str)
{
	if (str) {
		gbString_Header *header;
		header = GB_STRING_HEADER(str);
		gb_free(header->allocator, header);
	}
}


gb_inline gbString gb_string_duplicate(gbAllocator a, gbString const str) { return gb_string_make_length(a, str, gb_string_length(str)); }

gb_inline isize gb_string_length(gbString const str)   { return GB_STRING_HEADER(str)->length; }
gb_inline isize gb_string_capacity(gbString const str) { return GB_STRING_HEADER(str)->capacity; }

gb_inline isize
gb_string_available_space(gbString const str)
{
	gbString_Header *h = GB_STRING_HEADER(str);
	if (h->capacity > h->length)
		return h->capacity - h->length;
	return 0;
}


gb_inline void gb_string_clear(gbString str) { gbprivstring_set_length(str, 0); str[0] = '\0'; }

gb_inline gbString gb_string_append_string(gbString str, gbString const other) { return gb_string_append_string_length(str, other, gb_string_length(other)); }

gbString
gb_string_append_string_length(gbString str, void const *other, isize other_len)
{
	isize curr_len = gb_string_length(str);

	str = gb_string_make_space_for(str, other_len);
	if (str == NULL)
		return NULL;

	gb_memcpy(str + curr_len, other, other_len);
	str[curr_len + other_len] = '\0';
	gbprivstring_set_length(str, curr_len + other_len);

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

	gb_memcpy(str, cstr, len);
	str[len] = '\0';
	gbprivstring_set_length(str, len);

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
		isize old_size = gb_size_of(gbString_Header) + gb_string_length(str) + 1;
		isize new_size = gb_size_of(gbString_Header) + new_len + 1;

		void *new_ptr = gb_resize(GB_STRING_HEADER(str)->allocator, ptr, old_size, new_size);
		if (new_ptr == NULL) return NULL;

		str = cast(char *)(GB_STRING_HEADER(new_ptr) + 1);
		gbprivstring_set_capacity(str, new_len);

		return str;
	}
}

gb_inline isize
gb_string_allocation_size(gbString const str)
{
	isize cap = gb_string_capacity(str);
	return gb_size_of(gbString_Header) + cap;
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

	while (start_pos <= end && strchr(cut_set, *start_pos))
		start_pos++;
	while (end_pos > start_pos && strchr(cut_set, *end_pos))
		end_pos--;

	len = cast(isize)((start_pos > end_pos) ? 0 : ((end_pos - start_pos)+1));

	if (str != start_pos)
		gb_memmove(str, start_pos, len);
	str[len] = '\0';

	gbprivstring_set_length(str, len);

	return str;
}

gb_inline gbString gb_string_trim_space(gbString str) { return gb_string_trim(str, " \t\r\n\v\f"); }



gb_inline void
gb_to_lower(char *str)
{
	while (*str) {
		*str = gb_char_to_lower(*str);
		str++;
	}
}

gb_inline void
gb_to_upper(char *str)
{
	while (*str) {
		*str = gb_char_to_upper(*str);
		str++;
	}
}


gb_inline isize
gb_strlen(char const *str)
{
	char const *end;
	for (end = str; *end; end++) {
		//
	}
	return (end - str);
}

gb_inline char *
gb_strncpy(char *dest, char const *source, isize len)
{
	char *str = dest;
	while (len > 0 && *source) {
		*str++ = *source++;
		len--;
	}
	while (len > 0) {
		*str++ = '\0';
		len--;
	}
	return dest;
}

gb_inline int
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




////////////////////////////////////////////////////////////////
//
// Windows UTF-8 Handling
//
//


char16 *
gb_from_utf8(char16 *buffer, char *s, isize len)
{
	u8 *str = cast(u8 *)s;
	u32 c;
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
gb_to_utf8(char *buffer, char16 *str, isize len)
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
			u32 c;
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



////////////////////////////////
//                            //
// File Handling              //
//                            //
////////////////////////////////

gb_inline b32
gb_file_create(gbFile *file, char const *filepath)
{
	file->handle = fopen(filepath, "wb");
	return (file->handle != NULL); // TODO(bill): Handle fopen errors
}

gb_inline b32
gb_file_open(gbFile *file, char const *filepath)
{
	file->handle = fopen(filepath, "rb");
	return (file->handle != NULL); // TODO(bill): Handle fopen errors
}

gb_inline b32
gb_file_close(gbFile *file)
{
	b32 result = true;
	if (file && file->handle)
		result = fclose(file->handle) != 0; // TODO(bill): Handle fclose errors
	return result;
}

gb_inline b32
gb_file_read_at(gbFile *file, void *buffer, isize size, i64 offset)
{
	i64 prev_cursor_pos = ftell(file->handle);
	fseek(file->handle, offset, SEEK_SET);
	fread(buffer, 1, size, file->handle);
	fseek(file->handle, prev_cursor_pos, SEEK_SET);
	return true;
}

gb_inline b32
gb_file_write_at(gbFile *file, void const *buffer, isize size, i64 offset)
{
	isize written_size;

	i64 prev_cursor_pos = ftell(file->handle);
	fseek(file->handle, offset, SEEK_SET);

	written_size = fwrite(buffer, 1, size, file->handle);
	fseek(file->handle, prev_cursor_pos, SEEK_SET);
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

	fseek(file->handle, 0, SEEK_END);
	result_size = cast(i64)ftell(file->handle);
	fseek(file->handle, 0, SEEK_SET);
	return result_size;
}





gbFile_Contents
gb_read_entire_file_contents(gbAllocator a, char const *filepath, b32 zero_terminate)
{
	gbFile_Contents result = {0};
	gbFile file = {0};
	if (gb_file_open(&file, filepath)) {
		i64 file_size = gb_file_size(&file);
		if (file_size > 0) {
			result.data = gb_alloc(a, zero_terminate ? file_size+1 : file_size);
			result.size = file_size;
			gb_file_read_at(&file, result.data, result.size, 0);
			if (zero_terminate)
				(cast(u8 *)(result.data))[file_size] = '\0';
		}
		gb_file_close(&file);
	}

	return result;
}




gb_no_inline void
gbprivarray_set_capacity(void *array_, isize capacity, isize element_size)
{
	// NOTE(bill): I know this is unsafe so don't call this function directly
	gbVoid_Array *a = cast(gbVoid_Array *)array_;
	void *data = NULL;

	GB_ASSERT(element_size > 0);

	if (capacity == a->capacity)
		return;

	if (capacity < a->count) {
		if (a->capacity < capacity) {
			isize new_capacity = 2*a->capacity + 8;
			if (new_capacity < capacity)
				new_capacity = capacity;
			gbprivarray_set_capacity(a, new_capacity, element_size);
		}
		a->count = capacity;
	}

	if (capacity > 0) {
		data = gb_alloc(a->allocator, element_size*capacity);
		gb_memcpy(data, a->data, element_size*a->count);
	}
	gb_free(a->allocator, a->data);
	a->data = data;
	a->capacity = capacity;
}



#if defined(__cplusplus)
}
#endif
#endif /* GB_IMPLEMENTATION */
