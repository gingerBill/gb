// gb.h - v0.02 - public domain C helper library - no warranty implied; use at your own risk
// (Experimental) A C helper library geared towards game development

/*
LICENSE
	This software is in the public domain. Where that dedication is not
	recognized, you are granted a perpetual, irrevocable license to copy,
	distribute, and modify this file as you see fit.

WARNING
	- This library is _highly_ experimental and features may not work as expected.
	- This also means that many functions are not documented.

CONTENTS
	- Common Macros
	- Assert
	- Types
	- Cast macro (easy grepping)
	- Memory
	- Custom Allocation
		- gb_Allocator
		- gb_Arena
		- gb_Pool
	- gb_String

TODO
	- Mutex
	- Atomics
	- Semaphore
	- Thread
	- OS Types and Functions (File/IO/OS/etc.)
*/

/*
Version History:
	0.02  - Minor fixes
	0.01  - Initial Version
*/


#ifndef GB_INCLUDE_GB_H
#define GB_INCLUDE_GB_H

#if defined(__cplusplus)
extern "C" {
#endif

#include <stdarg.h>
#include <stddef.h>

// NOTE(bill): Because static means three different things in C/C++
//             Great design(!)
#ifndef local_persist
#define global        static
#define internal      static
#define local_persist static
#endif

// NOTE(bill): If I put gb_inline, I _want_ it inlined
#ifndef gb_inline
	#if defined(_MSC_VER)
		#define gb_inline __forceinline
	#else
		#define gb_inline __attribute__ ((__always_inline__))
	#endif
#endif


#if !defined(GB_NO_STDIO)
#include <stdio.h>
#endif

#ifndef GB_ASSERT
#include <assert.h>
#define GB_ASSERT(cond) assert(cond)
#define GB_ASSERT_MSG(cond, msg) assert((cond) && (msg))
#endif

#define GB_STATIC_ASSERT3(cond, msg) typedef char gb__static_assertion_##msg[(!!(cond))*2-1]
// NOTE(bill): Token pasting madness
#define GB_STATIC_ASSERT2(cond, line) GB_STATIC_ASSERT3(cond, static_assertion_at_line_##line)
#define GB_STATIC_ASSERT1(cond, line) GB_STATIC_ASSERT2(cond, line)
#define GB_STATIC_ASSERT(cond)        GB_STATIC_ASSERT1(cond, __LINE__)


#if !defined(GB_NO_STDIO) && defined(_MSC_VER)
	// snprintf_msvc
	gb_inline int
	gb__vsnprintf_compatible(char* buffer, size_t size, char const *format, va_list args)
	{
		int result = -1;
		if (size > 0)
			result = _vsnprintf_s(buffer, size, _TRUNCATE, format, args);
		if (result == -1)
			return _vscprintf(format, args);
		return result;
	}

	gb_inline int
	gb__snprintf_compatible(char* buffer, size_t size, char const *format, ...)
	{
		int result = -1;
		va_list args;
		va_start(args, format);
		result = gb__vsnprintf_compatible(buffer, size, format, args);
		va_end(args);
		return result;
	}

	#if !defined(GB_DO_NOT_USE_MSVC_SPRINTF_FIX)
		#define snprintf  gb__snprintf_compatible
		#define vsnprintf gb__vsnprintf_compatible
	#endif /* GB_DO_NOT_USE_MSVC_SPRINTF_FIX */
#endif /* !defined(GB_NO_STDIO) */





#if defined(_MSC_VER)
	typedef  unsigned __int8  u8;
	typedef    signed __int8  s8;
	typedef unsigned __int16 u16;
	typedef   signed __int16 s16;
	typedef unsigned __int32 u32;
	typedef   signed __int32 s32;
	typedef unsigned __int64 u64;
	typedef   signed __int64 s64;
#else
	#include <stdint.h>
	typedef  uint8_t  u8;
	typedef   int8_t  s8;
	typedef uint16_t u16;
	typedef  int16_t s16;
	typedef uint32_t u32;
	typedef  int32_t s32;
	typedef uint64_t u64;
	typedef  int64_t s64;
#endif

GB_STATIC_ASSERT(sizeof(s8)  == 1);
GB_STATIC_ASSERT(sizeof(s16) == 2);
GB_STATIC_ASSERT(sizeof(s32) == 4);
GB_STATIC_ASSERT(sizeof(s64) == 8);

typedef size_t usize;

typedef uintptr_t uintptr;
typedef  intptr_t  intptr;

typedef float  f32;
typedef double f64;

#if defined(_MSC_VER) && _MSC_VER < 1900
	#ifndef false
	#define false 0
	#endif

	#ifndef true
	#define true 1
	#endif
#else
	#include <stdbool.h> // NOTE(bill): To get false/true
#endif

// Boolean Types
typedef s8  b8;
typedef s32 b32;


#ifndef U8_MIN
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
#endif




#ifndef COUNT_OF
#define COUNT_OF(x) (sizeof((x)) / sizeof(0[(x)]))
#endif

// NOTE(bill): Allows for easy grep of casts
// NOTE(bill): Still not as type safe as C++ static_cast, reinterpret_cast, and const_cast, but I don't need them
#ifndef cast
#define cast(x) (x)
#endif

#ifndef NULL
#define NULL ((void *)0)
#endif

#ifndef GB_UNUSED
#define GB_UNUSED(x) ((void)(sizeof(x)))
#endif

#ifndef gb_inline
	#if defined(_MSC_VER)
		#define gb_inline __forceinline
	#else
		#define gb_inline __attribute__ ((__always_inline__))
	#endif
#endif







////////////////////////////////
//                            //
// Memory                     //
//                            //
////////////////////////////////

#include <string.h> // For memcpy/memmove/memset/etc.

#ifndef GB_IS_POWER_OF_TWO
#define GB_IS_POWER_OF_TWO(x) ((x) != 0) && !((x) & ((x)-1))
#endif

void *gb_align_forward(void *ptr, usize align);

void gb_zero_size(void *ptr, usize size);
#define gb_zero_struct(t) gb_zero_size((t), sizeof(*(t))) // NOTE(bill): Pass pointer of struct
#define gb_zero_array(a, count) gb_zero_size((a), sizeof((a)[0])*count)


#if defined(GB_IMPLEMENTATION)
gb_inline void *
gb_align_forward(void *ptr, usize align)
{
	uintptr p;
	usize modulo;

	GB_ASSERT(GB_IS_POWER_OF_TWO(align));

	p = cast(uintptr)ptr;
	modulo = p % align;
	if (modulo) p += (align - modulo);
	return cast(void *)p;
}

gb_inline void gb_zero_size(void *ptr, usize size) { memset(ptr, 0, size); }

#endif // GB_IMPLEMENTATION








////////////////////////////////
//                            //
// Custom Allocation          //
//                            //
////////////////////////////////

typedef enum gb_Allocation_Type
{
	GB_ALLOCATION_TYPE_ALLOC,
	GB_ALLOCATION_TYPE_FREE,
	GB_ALLOCATION_TYPE_FREE_ALL,
	GB_ALLOCATION_TYPE_RESIZE,
} gb_Allocation_Type;


#ifndef GB_ALLOCATOR_PROCEDURE
#define GB_ALLOCATOR_PROCEDURE(name) void *name(void *allocator_data, gb_Allocation_Type type, usize size, usize alignment, void *old_memory, usize old_size, u32 options)
#endif
typedef GB_ALLOCATOR_PROCEDURE(gb_Allocator_Procedure);

typedef struct gb_Allocator
{
	gb_Allocator_Procedure *procedure;
	void *data;
} gb_Allocator;

#ifndef GB_DEFAULT_ALIGNMENT
#define GB_DEFAULT_ALIGNMENT 8
#endif

gb_inline void *gb_alloc_align(gb_Allocator a, usize size, usize alignment) { return a.procedure(a.data, GB_ALLOCATION_TYPE_ALLOC, size, alignment, NULL, 0, 0); }
gb_inline void *gb_alloc(gb_Allocator a, usize size)                        { return gb_alloc_align(a, size, GB_DEFAULT_ALIGNMENT); }
gb_inline void  gb_free(gb_Allocator a, void *ptr)                          { a.procedure(a.data, GB_ALLOCATION_TYPE_FREE, 0, 0, ptr, 0, 0); }
gb_inline void  gb_free_all(gb_Allocator a)                                 { a.procedure(a.data, GB_ALLOCATION_TYPE_FREE_ALL, 0, 0, NULL, 0, 0); }
gb_inline void *gb_resize(gb_Allocator a, void *ptr, usize new_size)        { return a.procedure(a.data, GB_ALLOCATION_TYPE_RESIZE, new_size, 0, ptr, 0, 0); }

gb_inline void *gb_alloc_copy(gb_Allocator a, void* src, usize size) { return memcpy(gb_alloc(a, size), src, size); }
gb_inline void *gb_alloc_align_copy(gb_Allocator a, void* src, usize size, usize alignment) { return memcpy(gb_alloc_align(a, size, alignment), src, size); }

#define gb_alloc_struct(allocator, Type)       (Type *)gb_alloc_align(allocator, sizeof(Type))
#define gb_alloc_array(allocator, Type, count) (Type *)gb_alloc(allocator, sizeof(Type) * (count))




typedef struct gb_Arena
{
	gb_Allocator backing;
	void *physical_start;
	usize total_size;
	usize total_allocated_count;
	usize prev_allocated_count;
	u32 temp_count;
} gb_Arena;

void gb_init_arena_from_memory(gb_Arena *arena, void *start, usize size);
void gb_init_arena_from_allocator(gb_Arena *arena, gb_Allocator backing, usize size);
void gb_free_arena(gb_Arena *arena);

gb_Allocator gb_make_arena_allocator(gb_Arena *arena);
GB_ALLOCATOR_PROCEDURE(gb_arena_allocator_procedure);



typedef struct gb_Temp_Arena_Memory
{
	gb_Arena *arena;
	usize original_count;
} gb_Temp_Arena_Memory;

gb_Temp_Arena_Memory gb_begin_temp_arena_memory(gb_Arena *arena);
void gb_end_temp_arena_memory(gb_Temp_Arena_Memory tmp_mem);



typedef struct gb_Pool
{
	gb_Allocator backing;

	void *physical_start;
	void *free_list;

	usize block_size;
	usize block_align;
	usize total_size;
} gb_Pool;

void gb_init_pool(gb_Pool *pool, gb_Allocator backing, usize num_blocks, usize block_size);
void gb_init_pool_align(gb_Pool *pool, gb_Allocator backing, usize num_blocks, usize block_size, usize block_align);
void gb_free_pool(gb_Pool *pool);


gb_Allocator gb_make_pool_allocator(gb_Pool *pool);
GB_ALLOCATOR_PROCEDURE(gb_pool_allocator_procedure);




#if defined(GB_IMPLEMENTATION)

gb_inline void
gb_init_arena_from_memory(gb_Arena *arena, void *start, usize size)
{
	arena->backing.procedure = NULL;
	arena->backing.data = NULL;
	arena->physical_start = start;
	arena->total_size = size;
	arena->total_allocated_count = 0;
	arena->temp_count = 0;
}

gb_inline void
gb_init_arena_from_allocator(gb_Arena *arena, gb_Allocator backing, usize size)
{
	arena->backing = backing;
	arena->physical_start = gb_alloc(backing, size);
	arena->total_size = size;
	arena->total_allocated_count = 0;
	arena->temp_count = 0;
}

gb_inline void
gb_free_arena(gb_Arena *arena)
{
	if (arena->backing.procedure) {
		gb_free(arena->backing, arena->physical_start);
		arena->physical_start = NULL;
	}
}




gb_inline gb_Allocator
gb_make_arena_allocator(gb_Arena *arena)
{
	gb_Allocator allocator;
	allocator.procedure = gb_arena_allocator_procedure;
	allocator.data = arena;
	return allocator;
}


GB_ALLOCATOR_PROCEDURE(gb_arena_allocator_procedure)
{
	gb_Arena *arena = cast(gb_Arena *)allocator_data;

	GB_UNUSED(options);
	GB_UNUSED(old_size);

	switch (type) {
	case GB_ALLOCATION_TYPE_ALLOC: {
		void *ptr;
		usize actual_size = size + alignment;

		// NOTE(bill): Out of memory
		if (arena->total_allocated_count + actual_size > cast(usize)arena->total_size)
			return NULL;

		ptr = gb_align_forward(cast(u8 *)arena->physical_start + arena->total_allocated_count, alignment);
		arena->prev_allocated_count = arena->total_allocated_count;
		arena->total_allocated_count += actual_size;
		return ptr;
	} break;

	case GB_ALLOCATION_TYPE_FREE: {
		// NOTE(bill): Free all at once
		// NOTE(bill): Use Temp_Arena_Memory if you want to free a block
	} break;

	case GB_ALLOCATION_TYPE_FREE_ALL:
		arena->total_allocated_count = 0;
		break;

	case GB_ALLOCATION_TYPE_RESIZE: {
		// TODO(bill): Check if ptr is at the top
		void *ptr = gb_alloc_align(gb_make_arena_allocator(arena), size, alignment);
		memcpy(ptr, old_memory, size);
		return ptr;
	} break;
	}

	return NULL; // NOTE(bill): Default return value
}


gb_inline gb_Temp_Arena_Memory
gb_begin_temp_arena_memory(gb_Arena *arena)
{
	gb_Temp_Arena_Memory tmp;
	tmp.arena = arena;
	tmp.original_count = arena->total_allocated_count;
	arena->temp_count++;
	return tmp;
}

gb_inline void
gb_end_temp_arena_memory(gb_Temp_Arena_Memory tmp)
{
	GB_ASSERT(tmp.arena->total_allocated_count >= tmp.original_count);
	GB_ASSERT(tmp.arena->temp_count > 0);
	tmp.arena->total_allocated_count = tmp.original_count;
	tmp.arena->temp_count--;
}




void
gb_init_pool(gb_Pool *pool, gb_Allocator backing, usize num_blocks, usize block_size)
{
	gb_init_pool_align(pool, backing, num_blocks, block_size, GB_DEFAULT_ALIGNMENT);
}

void
gb_init_pool_align(gb_Pool *pool, gb_Allocator backing, usize num_blocks, usize block_size, usize block_align)
{
	usize actual_block_size, pool_size, block_index;
	u8 *data, *curr;

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

	{
		uintptr *end = cast(uintptr*)curr;
		*end = cast(uintptr)NULL;
	}

	pool->physical_start = data;
	pool->free_list      = data;
}

void
gb_free_pool(gb_Pool *pool)
{
	if (pool->backing.procedure) {
		gb_free(pool->backing, pool->physical_start);
	}
}


gb_Allocator
gb_make_pool_allocator(gb_Pool *pool)
{
	gb_Allocator allocator;
	allocator.procedure = gb_pool_allocator_procedure;
	allocator.data = pool;
	return allocator;
}

GB_ALLOCATOR_PROCEDURE(gb_pool_allocator_procedure)
{
	gb_Pool *pool = cast(gb_Pool *)allocator_data;

	GB_UNUSED(options);
	GB_UNUSED(old_size);

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

	case GB_ALLOCATION_TYPE_FREE_ALL: {
		// TODO(bill):
	} break;

	case GB_ALLOCATION_TYPE_RESIZE: {
		// NOTE(bill): Cannot resize
	} break;
	}

	return NULL;
}











#endif // GB_IMPLEMENTATION


////////////////////////////////
//                            //
// gb_String - C Compatible   //
//                            //
////////////////////////////////


// Pascal like strings in C
typedef char *gb_String;

#ifndef GB_STRING_SIZE
#define GB_STRING_SIZE
typedef u32 gb_String_Size;
#endif


// This is stored at the beginning of the string
// NOTE(bill): It is (2*sizeof(gb_String_Size) + 2*sizeof(void*)) (default: 16B (32bit), 24B (64bit))
// NOTE(bill): If you only need a small string, just use a standard c string
typedef struct gb_String_Header
{
	gb_Allocator   allocator;
	gb_String_Size length;
	gb_String_Size capacity;
} gb_String_Header;

#define GB_STRING_HEADER(str) (cast(gb_String_Header *)str - 1)

gb_String gb_string_make(gb_Allocator a, char const *str);
gb_String gb_string_make_length(gb_Allocator a, void const *str, gb_String_Size num_bytes);
void      gb_string_free(gb_String str);

gb_String gb_string_duplicate(gb_Allocator a, gb_String const str);

gb_String_Size gb_string_length(gb_String const str);
gb_String_Size gb_string_capacity(gb_String const str);
gb_String_Size gb_string_available_space(gb_String const str);

void gb_string_clear(gb_String str);

gb_String gb_string_append_string(gb_String str, gb_String const other);
gb_String gb_string_append_string_length(gb_String str, void const *other, gb_String_Size num_bytes);
gb_String gb_string_append_cstring(gb_String str, char const *other);

gb_String gb_string_set(gb_String str, char const *cstr);

gb_String gb_string_make_space_for(gb_String str, gb_String_Size add_len);
gb_String_Size gb_string_allocation_size(gb_String const str);

b32 gb_strings_are_equal(gb_String const lhs, gb_String const rhs);

gb_String gb_string_trim(gb_String str, char const *cut_set);
gb_String gb_string_trim_space(gb_String str); /* Whitespace ` \t\r\n\v\f` */


#if defined(GB_IMPLEMENTATION)

gb_inline void gb__string_set_length(gb_String str, gb_String_Size len) { GB_STRING_HEADER(str)->length = len; }
gb_inline void gb__string_set_capacity(gb_String str, gb_String_Size cap) { GB_STRING_HEADER(str)->capacity = cap; }


gb_inline gb_String
gb_string_make(gb_Allocator a, char const *str)
{
	gb_String_Size len = cast(gb_String_Size)(str ? strlen(str) : 0);
	return gb_string_make_length(a, str, len);
}

gb_String
gb_string_make_length(gb_Allocator a, void const *init_str, gb_String_Size num_bytes)
{
	gb_String_Size header_size = sizeof(gb_String_Header);
	void *ptr = gb_alloc(a, header_size + num_bytes + 1);

	if (ptr == NULL) {
		return NULL;
	} else {
		gb_String str = cast(char *)ptr + header_size;
		gb_String_Header *header = GB_STRING_HEADER(str);
		// Zero all data first
		if (!init_str) gb_zero_size(ptr, header_size + num_bytes + 1);
		header->allocator = a;
		header->length    = num_bytes;
		header->capacity  = num_bytes;

		if (num_bytes && init_str)
			memcpy(str, init_str, num_bytes);
		str[num_bytes] = '\0'; // Just in case

		return str;
	}
}

gb_inline void
gb_string_free(gb_String str)
{
	if (str) {
		gb_String_Header *header;
		header = GB_STRING_HEADER(str);
		gb_free(header->allocator, header);
	}
}


gb_inline gb_String gb_string_duplicate(gb_Allocator a, gb_String const str) { return gb_string_make_length(a, str, gb_string_length(str)); }

gb_inline gb_String_Size gb_string_length(gb_String const str)   { return GB_STRING_HEADER(str)->length; }
gb_inline gb_String_Size gb_string_capacity(gb_String const str) { return GB_STRING_HEADER(str)->capacity; }

gb_inline gb_String_Size
gb_string_available_space(gb_String const str)
{
	gb_String_Header *h = GB_STRING_HEADER(str);
	if (h->capacity > h->length)
		return h->capacity - h->length;
	return 0;
}


gb_inline void gb_string_clear(gb_String str) { gb__string_set_length(str, 0); str[0] = '\0'; }

gb_inline gb_String gb_string_append_string(gb_String str, gb_String const other) { return gb_string_append_string_length(str, other, gb_string_length(other)); }

gb_String
gb_string_append_string_length(gb_String str, void const *other, gb_String_Size other_len)
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

gb_inline gb_String
gb_string_append_cstring(gb_String str, char const *other)
{
	return gb_string_append_string_length(str, other, cast(gb_String_Size)strlen(other));
}


gb_String
gb_string_set(gb_String str, char const *cstr)
{
	gb_String_Size len = cast(gb_String_Size)strlen(cstr);
	if (gb_string_capacity(str) < len) {
		str = gb_string_make_space_for(str, len - gb_string_length(str));
		if (str == NULL)
			return NULL;
	}

	memcpy(str, cstr, len);
	str[len] = '\0';
	gb__string_set_length(str, len);

	return str;
}


local_persist void *
gb__string_realloc(gb_Allocator a, void *ptr, gb_String_Size old_size, gb_String_Size new_size)
{
	if (!ptr) return gb_alloc(a, new_size);

	if (new_size < old_size)
		new_size = old_size;

	if (old_size == new_size) {
		return ptr;
	} else {
		// TODO(bill): Use gb_resize here??
		void *new_ptr = gb_alloc(a, new_size);
		if (!new_ptr)
			return NULL;

		memcpy(new_ptr, ptr, old_size);
		gb_free(a, ptr);
		return new_ptr;
	}
}



gb_String
gb_string_make_space_for(gb_String str, gb_String_Size add_len)
{
	gb_String_Size available = gb_string_available_space(str);

	// Return if there is enough space left
	if (available >= add_len) {
		return str;
	} else {
		gb_String_Size new_len = gb_string_length(str) + add_len;
		void *ptr = GB_STRING_HEADER(str);
		gb_String_Size old_size = sizeof(struct gb_String_Header) + gb_string_length(str) + 1;
		gb_String_Size new_size = sizeof(struct gb_String_Header) + new_len + 1;

		void *new_ptr = gb__string_realloc(GB_STRING_HEADER(str)->allocator, ptr, old_size, new_size);
		if (new_ptr == NULL) return NULL;

		str = cast(char *)(GB_STRING_HEADER(new_ptr) + 1);
		gb__string_set_capacity(str, new_len);

		return str;
	}
}

gb_inline gb_String_Size
gb_string_allocation_size(gb_String const str)
{
	gb_String_Size result = gb_string_capacity(str) + cast(gb_String_Size)sizeof(gb_String_Header);
	return result;
}


gb_inline b32
gb_strings_are_equal(gb_String const lhs, gb_String const rhs)
{
	gb_String_Size lhs_len, rhs_len, i;
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


gb_String
gb_string_trim(gb_String str, char const *cut_set)
{
	char *start, *end, *start_pos, *end_pos;
	gb_String_Size len;

	start_pos = start = str;
	end_pos   = end   = str + gb_string_length(str) - 1;

	while (start_pos <= end && strchr(cut_set, *start_pos))
		start_pos++;
	while (end_pos > start_pos && strchr(cut_set, *end_pos))
		end_pos--;

	len = cast(gb_String_Size)((start_pos > end_pos) ? 0 : ((end_pos - start_pos)+1));

	if (str != start_pos)
		memmove(str, start_pos, len);
	str[len] = '\0';

	gb__string_set_length(str, len);

	return str;
}

gb_inline gb_String gb_string_trim_space(gb_String str) { return gb_string_trim(str, " \t\r\n\v\f"); }


#endif



////////////////////////////////
//                            //
// Unfinished code            //
//                            //
////////////////////////////////



#if defined(__cplusplus)
}
#endif

#endif // GB_INCLUDE_GB_H
