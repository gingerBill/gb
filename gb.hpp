// gb.hpp - v0.07 - public domain C++11 helper library - no warranty implied; use at your own risk
// (Experimental) A C++11 helper library without STL geared towards game development
//
// Version History:
//     0.07 - Bug Fixes
//     0.06 - Os spec ideas
//     0.05 - Transform Type and Quaternion Functions
//     0.04 - String
//     0.03 - Hash Functions
//     0.02 - Hash Table
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
// CONTENT
//
//     - Common Macros
//     - Assert
//     - Types
//     - C++11 Move Semantics
//     - Defer
//     - Memory
//         - Functions
//         - Allocator
//         - Heap_Allocator
//         - Arena_Allocator
//         - Temporary_Arena_Memory
//     - String
//     - Array
//     - Hash_Table
//     - Hash Functions
//    [- Os] (Not Yet Implemented)
//     - Math Types
//         - Vector(2,3,4)
//         - Quaternion
//         - Matrix4
//     - Math Operations
//     - Math Functions & Constants
//     - Math Type Functions
//
//
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
#if defined(_WIN32) || defined(_WIN64)
#define GB_SYSTEM_WINDOWS
#define NOMINMAX
#define VC_EXTRALEAN
#define WIN32_EXTRA_LEAN

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

#include <float.h>
#include <math.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#ifdef GB_SYSTEM_WINDOWS
#include <windows.h>
#else
#include <pthread.h>
#endif

#ifndef NDEBUG
#define GB_ASSERT(x, ...) ((void)(::gb__assert_handler((x), #x, __FILE__, __LINE__, ##__VA_ARGS__)))
#else
#define GB_ASSERT(x, ...) ((void)sizeof(x))
#endif

extern "C" inline void
gb__assert_handler(bool condition, const char* condition_str,
                   const char* filename, size_t line,
                   const char* error_text = nullptr, ...)
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

	abort();
}


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
	return static_cast<T&&>(t);
}

template <typename T>
inline T&&
forward(typename Remove_Reference<T>::Type&& t)
{
	return static_cast<T&&>(t);
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

// NOTE(bill): These macros are in the global namespace thus, defer can be treated without a gb:: prefix
#define GB_DEFER_1(x, y) x##y
#define GB_DEFER_2(x, y) GB_DEFER_1(x, y)
#define GB_DEFER_3(x)    GB_DEFER_2(GB_DEFER_2(GB_DEFER_2(x, __COUNTER__), _), __LINE__)
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

#ifndef GB_DEFAULT_ALIGNMENT
#define GB_DEFAULT_ALIGNMENT 4
#endif

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
	virtual s64 allocated_size(const void* ptr) = 0;
	virtual s64 total_allocated() = 0;

private:
	// Delete copying
	Allocator(const Allocator&) = delete;
	Allocator& operator=(const Allocator&) = delete;
};

inline void* alloc(Allocator& a, usize size, usize align = GB_DEFAULT_ALIGNMENT) { return a.alloc(size, align); }

inline void dealloc(Allocator& a, void* ptr) { return a.dealloc(ptr); }

template <typename T>
inline T* alloc_struct(Allocator& a) { return static_cast<T*>a.alloc(sizeof(T), alignof(T)); }


template <typename T>
inline T* alloc_array(Allocator& a, usize count) { return static_cast<T*>(alloc(a, count * sizeof(T), alignof(T))); }

template <typename T, usize count>
inline T* alloc_array(Allocator& a) { return static_cast<T*>(alloc(a, count * sizeof(T), alignof(T))); }

#define GB_HEAP_ALLOCATOR_HEADER_PAD_VALUE (usize)(-1)

struct Heap_Allocator : Allocator
{
	struct Header
	{
		s64 size;
	};

	Mutex mutex               = Mutex{};
	s64 total_allocated_count = 0;
	s64 allocation_count      = 0;

	Heap_Allocator() = default;

	virtual ~Heap_Allocator();

	virtual void* alloc(usize size, usize align = GB_DEFAULT_ALIGNMENT);
	virtual void  dealloc(void* ptr);
	virtual s64 allocated_size(const void* ptr);
	virtual s64 total_allocated();

	Header* get_header_ptr(const void* ptr);
};


struct Arena_Allocator : Allocator
{
	u8* base                  = nullptr;
	s64 base_size             = 0;
	s64 total_allocated_count = 0;
	s64 temp_count            = 0;

	Arena_Allocator() = default;
	explicit Arena_Allocator(void* base, usize base_size);

	virtual void* alloc(usize size, usize align = GB_DEFAULT_ALIGNMENT);
	virtual void  dealloc(void* ptr);
	virtual s64 allocated_size(const void* ptr);
	virtual s64 total_allocated();

	virtual usize get_alignment_offset(usize align = GB_DEFAULT_ALIGNMENT);
	virtual usize get_remaining_space(usize align = GB_DEFAULT_ALIGNMENT);
	void check();
};

struct Temporary_Arena_Memory
{
	Arena_Allocator* arena;
	s64              original_count;

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
/// String                   ///
////////////////////////////////
using String = char*;
using String_Size = u32;
struct String_Header
{
	Allocator* allocator;
	String_Size len;
	String_Size cap;
};

inline String_Header* string_header(String str) { return (String_Header*)str - 1; }

String make_string(Allocator& a, const char* str = "");
String make_string(Allocator& a, const void* str, String_Size len);
void   free_string(String& str);

String duplicate_string(Allocator& a, const String str);

String_Size string_length(const String str);
String_Size string_capacity(const String str);
String_Size string_available_space(const String str);

void clear_string(String str);

void append_string(String& str, const String other);
void append_cstring(String& str, const char* other);
void append_string(String& str, const void* other, String_Size len);

void string_make_space_for(String& str, String_Size add_len);
usize string_allocation_size(const String str);

bool strings_are_equal(const String lhs, const String rhs);

void trim_string(String& str, const char* cut_set);


////////////////////////////////
/// Array                    ///
////////////////////////////////

template <typename T>
struct Array
{
	Allocator* allocator;
	s64        count;
	s64        allocation;
	T*         data;

	Array() = default;
	explicit Array(Allocator& a, usize count = 0);
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

////////////////////////////////
/// Hash Table               ///
////////////////////////////////

template <typename T>
struct Hash_Table
{
	struct Entry
	{
		u64 key;
		s64 next;
		T   value;
	};

	Array<s64>   hashes;
	Array<Entry> data;

	Hash_Table() = default;
	explicit Hash_Table(Allocator& a);
	~Hash_Table() = default;
};

template <typename T>
Hash_Table<T>::Hash_Table(Allocator& a)
{
	hashes = make_array<s64>(a);
	data = make_array<typename Hash_Table<T>::Entry>(a);
}

template <typename T>
inline Hash_Table<T>
make_hash_table(Allocator& a)
{
	Hash_Table<T> h = {};
	h.hashes = make_array<s64>(a);
	h.data   = make_array<typename Hash_Table<T>::Entry>(a);
	return h;
}

template <typename T> bool hash_table_has(const Hash_Table<T>& h, u64 key);

template <typename T> const T& hash_table_get(const Hash_Table<T>& h, u64 key, const T& default_value);
template <typename T> void hash_table_set(Hash_Table<T>& h, u64 key, const T& value);

template <typename T> void remove_from_hash_table(Hash_Table<T>& h, u64 key);
template <typename T> void reserve_hash_table(Hash_Table<T>& h, usize capacity);
template <typename T> void clear_hash_table(Hash_Table<T>& h);

// Iterators (in random order)
template <typename T> const typename Hash_Table<T>::Entry* begin(const Hash_Table<T>& h);
template <typename T> const typename Hash_Table<T>::Entry* end(const Hash_Table<T>& h);

// Mutli_Hash_Table
template <typename T> void get_multiple_from_hash_table(const Hash_Table<T>& h, u64 key, Array<T>& items);
template <typename T> usize multiple_count_from_hash_table(const Hash_Table<T>& h, u64 key);

template <typename T> const typename Hash_Table<T>::Entry* find_first_in_hash_table(const Hash_Table<T>& h, u64 key);
template <typename T> const typename Hash_Table<T>::Entry* find_next_in_hash_table(const Hash_Table<T>& h, const typename Hash_Table<T>::Entry* e);


template <typename T> void insert_into_hash_table(Hash_Table<T>& h, u64 key, const T& value);
template <typename T> void remove_entry_from_hash_table(Hash_Table<T>& h, const typename Hash_Table<T>::Entry* e);
template <typename T> void remove_all_from_hash_table(Hash_Table<T>& h, u64 key);

////////////////////////////////
/// Array                    ///
////////////////////////////////
template <typename T>
inline Array<T>::Array(Allocator& a, usize count_)
{
	allocator = &a;
	count = 0;
	allocation = 0;
	data = nullptr;
	if (count > 0)
	{
		data = alloc_array<T>(a, count_);
		if (data)
			count = allocation = count_;
	}
}



template <typename T>
inline Array<T>
make_array(Allocator& allocator, usize count)
{
	Array<T> array = {};
	array.allocator = &allocator;
	array.count = 0;
	array.allocation = 0;
	array.data = nullptr;
	if (count > 0)
	{
		array.data = alloc_array<T>(allocator, count);
		if (array.data)
			array.count = array.allocation = count;
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
	if (a.allocation < (s64)count)
		grow_array(a, count);
	a.count = count;
}

template <typename T>
inline void
reserve_array(Array<T>& a, usize allocation)
{
	if (a.allocation < (s64)allocation)
		set_array_allocation(a, allocation);
}

template <typename T>
inline void
set_array_allocation(Array<T>& a, usize allocation)
{
	if ((s64)allocation == a.allocation)
		return;

	if ((s64)allocation < a.count)
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
/// Hash Table               ///
////////////////////////////////
namespace impl
{
struct Find_Result
{
	s64 hash_index;
	s64 data_prev;
	s64 data_index;
};

template <typename T>
usize
add_hash_table_entry(Hash_Table<T>& h, u64 key)
{
	typename Hash_Table<T>::Entry e;
	e.key  = key;
	e.next = -1;
	usize e_index = h.data.count;
	append_array(h.data, e);

	return e_index;
}

template <typename T>
void
erase_from_hash_table(Hash_Table<T>& h, const Find_Result& fr)
{
	if (fr.data_prev < 0)
		h.hashes[fr.hash_index] = h.data[fr.data_index].next;
	else
		h.data[fr.data_prev].next = g.data[fr.data_index].next;

	pop_back_array(h.data); // updated array count

	if (fr.data_index == h.data.count)
		return;

	h.data[fr.data_index] = h.data[h.data.count];

	auto last = find_result_in_hash_table(h, h.data[fr.data_index].key);

	if (last.data_prev < 0)
		h.hashes[last.hash_index] = fr.data_index;
	else
		h.data[last.data_index].next = fr.data_index;
}

template <typename T>
Find_Result
find_result_in_hash_table(const Hash_Table<T>& h, u64 key)
{
	Find_Result fr;
	fr.hash_index = -1;
	fr.data_prev  = -1;
	fr.data_index = -1;

	if (h.hashes.count == 0)
		return fr;

	fr.hash_index = key % h.hashes.count;
	fr.data_index = h.hashes[fr.hash_index];
	while (fr.data_index >= 0)
	{
		if (h.data[fr.data_index].key == key)
			return fr;
		fr.data_prev  = fr.data_index;
		fr.data_index = h.data[fr.data_index].next;
	}

	return fr;
}


template <typename T>
Find_Result
find_result_in_hash_table(const Hash_Table<T>& h, const typename Hash_Table<T>::Entry* e)
{
	Find_Result fr;
	fr.hash_index = -1;
	fr.data_prev  = -1;
	fr.data_index = -1;

	if (h.hashes.count == 0 || !e)
		return fr;

	fr.hash_index = key % h.hashes.count;
	fr.data_index = h.hashes[fr.hash_index];
	while (fr.data_index >= 0)
	{
		if (&h.data[fr.data_index] == e)
			return fr;
		fr.data_prev  = fr.data_index;
		fr.data_index = h.data[fr.data_index].next;
	}

	return fr;
}

template <typename T>
s64 make_entry_in_hash_table(Hash_Table<T>& h, u64 key)
{
	const Find_Result fr = impl::find_result_in_hash_table(h, key);
	const s64 index    = impl::add_hash_table_entry(h, key);

	if (fr.data_prev < 0)
		h.hashes[fr.hash_index] = index;
	else
		h.data[fr.data_prev].next = index;

	h.data[index].next = fr.data_index;

	return index;
}

template <typename T>
void
find_and_erase_entry_from_hash_table(Hash_Table<T>& h, u64 key)
{
	const Find_Result fr = impl::find_result_in_hash_table(h, key);
	if (fr.data_index >= 0)
		erase_from_hash_table(h, fr);
}

template <typename T>
s64
find_entry_or_fail_in_hash_table(const Hash_Table<T>& h, u64 key)
{
	return find_result_in_hash_table(h, key).data_index;
}

template <typename T>
s64
find_or_make_entry_in_hash_table(Hash_Table<T>& h, u64 key)
{
	const auto fr = find_result_in_hash_table(h, key);
	if (fr.data_index >= 0)
		return fr.data_index;

	s64 index = add_hash_table_entry(h, key);
	if (fr.data_prev < 0)
		h.hashes[fr.hash_index] = index;
	else
		h.data[fr.data_prev].next = index;

	return index;
}

template <typename T>
void
rehash_hash_table(Hash_Table<T>& h, usize new_capacity)
{
	auto nh = make_hash_table<T>(*h.hashes.allocator);
	resize_array(nh.hashes, new_capacity);
	const usize old_count = h.data.count;
	reserve_array(nh.data, old_count);

	for (usize i = 0; i < new_capacity; i++)
		nh.hashes[i] = -1;

	for (usize i = 0; i < old_count; i++)
	{
		auto& e = h.data[i];
		insert_into_hash_table(nh, e.key, e.value);
	}

	auto empty = make_hash_table<T>(*h.hashes.allocator);
	h.~Hash_Table<T>();

	memcpy(&h,  &nh,    sizeof(Hash_Table<T>));
	memcpy(&nh, &empty, sizeof(Hash_Table<T>));
}

template <typename T>
void
grow_hash_table(Hash_Table<T>& h)
{
	const usize new_capacity = 2 * h.data.count + 2;
	rehash_hash_table(h, new_capacity);
}

template <typename T>
bool
is_hash_table_full(Hash_Table<T>& h)
{
	// Make sure that there is enough space
	const f32 maximum_load_coefficient = 0.75f;
	return h.data.count >= maximum_load_coefficient * h.hashes.count;
}
} // namespace impl

template <typename T>
inline bool
hash_table_has(const Hash_Table<T>& h, u64 key)
{
	return imple::find_entry_or_fail_in_hash_table(h, key) >= 0;
}

template <typename T>
inline const T&
hash_table_get(const Hash_Table<T>& h, u64 key, const T& default_value)
{
	const s64 index = impl::find_entry_or_fail_in_hash_table(h, key);

	if (index < 0)
		return default_value;
	return h.data[index].value;
}

template <typename T>
inline void
hash_table_set(Hash_Table<T>& h, u64 key, const T& value)
{
	if (h.hashes.count == 0)
		impl::grow_hash_table(h);

	const s64 index = impl::find_or_make_entry_in_hash_table(h, key);
	h.data[index].value = value;
	if (impl::is_hash_table_full(h))
		impl::grow_hash_table(h);
}

template <typename T>
inline void
remove_from_hash_table(Hash_Table<T>& h, u64 key)
{
	impl::find_and_erase_entry_from_hash_table(h, key);
}

template <typename T>
inline void
reserve_hash_table(Hash_Table<T>& h, usize capacity)
{
	impl:;rehash_hash_table(h, capacity);
}

template <typename T>
inline void
clear_hash_table(Hash_Table<T>& h)
{
	clear_array(h.hashes);
	clear_array(h.data);
}

template <typename T>
inline const typename Hash_Table<T>::Entry*
begin(const Hash_Table<T>& h)
{
	return begin(h.data);
}

template <typename T>
inline const typename Hash_Table<T>::Entry*
end(const Hash_Table<T>& h)
{
	return end(h.data);
}


// Mutli_Hash_Table
template <typename T>
inline void
get_multiple_from_hash_table(const Hash_Table<T>& h, u64 key, Array<T>& items)
{
	auto e = find_first_in_hash_table(h, key);
	while (e)
	{
		append_array(items, e->value);
		e = find_next_in_hash_table(h, e);
	}
}

template <typename T>
inline usize
multiple_count_from_hash_table(const Hash_Table<T>& h, u64 key)
{
	usize count = 0;
	auto e = find_first_in_hash_table(h, key);
	while (e)
	{
		count++
		e = find_next_in_hash_table(h, e);
	}

	return count;
}


template <typename T>
inline const typename Hash_Table<T>::Entry*
find_first_in_hash_table(const Hash_Table<T>& h, u64 key)
{
	const s64 index = impl::find_first_in_hash_table(h, key);
	if (index < 0)
		return nullptr;
	return &h.data[index];
}

template <typename T>
const typename Hash_Table<T>::Entry*
find_next_in_hash_table(const Hash_Table<T>& h, const typename Hash_Table<T>::Entry* e)
{
	if (!e)
		return nullptr;

	auto index = e->next;
	while (index >= 0)
	{
		if (h.data[index].ley == e->key)
			return &h.data[index];
		index = h.data[index].next;
	}

	return nullptr;
}


template <typename T>
inline void
insert_into_hash_table(Hash_Table<T>& h, u64 key, const T& value)
{
	if (h.hashes.count == 0)
		impl::grow_hash_table(h);

	auto next = impl::make_entry_in_hash_table(h, key);
	h.data[next].value = value;

	if (impl::is_hash_table_full(h))
		impl::grow_hash_table(h);
}

template <typename T>
inline void
remove_entry_from_hash_table(Hash_Table<T>& h, const typename Hash_Table<T>::Entry* e)
{
	const auto fr = impl:;find_result_in_hash_table(h, e);
	if (fr.data_index >= 0)
		impl::erase_from_hash_table(h, fr);
}

template <typename T>
inline void
remove_all_from_hash_table(Hash_Table<T>& h, u64 key)
{
	while (hash_table_has(h, key))
		remove(h, key);
}

////////////////////////////////
/// Hash                     ///
////////////////////////////////

namespace hash
{
u32 adler32(const void* key, u32 num_bytes);

u32 crc32(const void* key, u32 num_bytes);
u64 crc64(const void* key, usize num_bytes);

// TODO(bill): Complete hashing functions
// u32 fnv32(const void* key, usize num_bytes);
// u64 fnv64(const void* key, usize num_bytes);
// u32 fnv32a(const void* key, usize num_bytes);
// u64 fnv64a(const void* key, usize num_bytes);

u32 murmur32(const void* key, u32 num_bytes, u32 seed = 0x9747b28c);
u64 murmur64(const void* key, usize num_bytes, u64 seed = 0x9747b28c);
} // namespace hash

////////////////////////////////
/// Os                       ///
////////////////////////////////

namespace os
{
#if 0
// TODO(bill) NOTE(bill): How should I do error handling?
// Because C++ cannot return multiple variables (ignoring tuples),
// the Golang way is not possible
// e.g. auto file, err = os::open_file("whatever.ext");
// Also this is ugly :
// File* file; Error* err;
// tie(file, err) = os::open_file("whatever.ext");

// TODO(bill): Move to gb:: ? e.g. make error handling type?
struct Error
{
	const char* text;
};

struct File
{
	// TODO(bill): Implement File type
	// Os Specific Crap Here
};

const Error ERR_INVALID    = Error{"invalid argument"};
const Error ERR_PERMISSION = Error{"permission denied"};
const Error ERR_EXIST      = Error{"file already exists"};
const Error ERR_NOT_EXIST  = Error{"file already exists"};


// Os Functions

Error* chdir(const char* dir);
Error* chmod(const char* name, u32 mode);
Error* mkdir(const char* name, u32 perm);
Error* mkdir_all(const char* name, u32 perm);

Error* remove(const char* name);
Error* remove_all(const char* name);

Error* rename(const char* old_path, const char* new_path);

void clear_env();
void exit(int code);

int get_egid();
int get_euid();
int get_gid();
int get_page_size();
int get_ppid();
int get_uid();

Error* get_wd(const char* buffer, usize buffer_len);
Error* hostname(const char* buffer, usize buffer_len);

bool is_path_separator(char c);

Error* lchown(const char* name, int uid, int gid);

Error* link(const char* old_name, const char* new_name);
Error* read_link(const char* name, const char* buffer, usize buffer_len);
Error* symlink(const char* old_name, const char* new_name);

void temp_dir(const char* buffer, usize buffer_len);

Error* truncate(const char* name, s64 size);



// File functions

// TODO(bill): Create enums?
#define O_RDONLY 00
#define O_WRONLY 01
#define O_RDWR   02

File new_file(uintptr fd, const char* name);
Error* create_file(File& file, const char* name);
Error* open_file(File& file, const char* filename, int flag = O_RDONLY, u32 perm = 0);
Error* close_file(File& file);

bool is_file_open(File& file);

bool get_line(File& file, const char* buffer, usize buffer_len);

Error* read_file(File& file, const void* buffer, usize num_bytes);
Error* read_file_at(File& file, const void* buffer, usize num_bytes, s64 offset);

Error* write_to_file(File& file, const void* buffer, usize num_bytes);
Error* write_to_file_at(File& file, const void* buffer, usize num_bytes, s64 offset);

Error* seek_file(File& file, s64 offset, s64 whence);
Error* sync_file(File& file);
Error* truncate_file(File& file);

Error* chdir_of_file(File& file);
Error* chmod_of_file(File& file, u32 mode);
Error* chown_of_file(File& file, int uid, int gid);

void name_of_file(File& file, const char* buffer, usize buffer_len);
#endif
} // namespace os


////////////////////////////////
/// Time                     ///
////////////////////////////////

struct Time
{
	s64 microseconds;
};

Time time_now();
void time_sleep(Time time);

Time seconds(f32 s);
Time milliseconds(s32 ms);
Time microseconds(s64 us);
f32 time_as_seconds(Time t);
s32 time_as_milliseconds(Time t);
s64 time_as_microseconds(Time t);

bool operator==(Time left, Time right);
bool operator!=(Time left, Time right);

bool operator<(Time left, Time right);
bool operator>(Time left, Time right);

bool operator<=(Time left, Time right);
bool operator>=(Time left, Time right);

Time operator-(Time right);

Time operator+(Time left, Time right);
Time operator-(Time left, Time right);

Time& operator+=(Time& left, Time right);
Time& operator-=(Time& left, Time right);

Time operator*(Time left, f32 right);
Time operator*(Time left, s64 right);
Time operator*(f32 left, Time right);
Time operator*(s64 left, Time right);

Time& operator*=(Time& left, f32 right);
Time& operator*=(Time& left, s64 right);

Time operator/(Time left, f32 right);
Time operator/(Time left, s64 right);

Time& operator/=(Time& left, f32 right);
Time& operator/=(Time& left, s64 right);

f32 operator/(Time left, Time right);

Time  operator%(Time left, Time right);
Time& operator%=(Time& left, Time right);


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
		struct { Vector2 xy, zw; };
		Vector3 xyz;
		f32     data[4];
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

struct Euler_Angles
{
	// NOTE(bill): All angles in radians
	f32 pitch;
	f32 yaw;
	f32 roll;
};

struct Transform
{
	Vector3    position    = Vector3{0, 0, 0};
	Quaternion orientation = Quaternion{0, 0, 0, 1};
	Vector3    scale       = Vector3{0, 0, 0};
};

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

Vector3 operator*(const Quaternion& a, const Vector3& v); // Rotate v by a

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

// Transform Operators
// World = Parent * Local
Transform operator*(const Transform& ps, const Transform& ls);
Transform& operator*=(Transform& ps, const Transform& ls);
// Local = World / Parent
Transform operator/(const Transform& ws, const Transform& ps);
Transform& operator/=(Transform& ws, const Transform& ps);

//////////////////////////////////
/// Math Functions & Constants ///
//////////////////////////////////
extern const Vector2    VECTOR2_ZERO;
extern const Vector3    VECTOR3_ZERO;
extern const Vector4    VECTOR4_ZERO;
extern const Quaternion QUATERNION_IDENTITY;
extern const Matrix4    MATRIX4_IDENTITY;

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

s32 min(s32 a, s32 b);
s64 min(s64 a, s64 b);
f32 min(f32 a, f32 b);

s32 max(s32 a, s32 b);
s64 max(s64 a, s64 b);
f32 max(f32 a, f32 b);

s32 clamp(s32 x, s32 min, s32 max);
s64 clamp(s64 x, s64 min, s64 max);
f32 clamp(f32 x, f32 min, f32 max);

f32 lerp(f32 x, f32 y, f32 t);

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

// Spherical Linear Interpolation
Quaternion slerp(const Quaternion& x, const Quaternion& y, f32 t);

// Shoemake's Quaternion Curves
// Sqherical Cubic Interpolation
inline Quaternion squad(const Quaternion& p,
                        const Quaternion& a,
                        const Quaternion& b,
                        const Quaternion& q,
                        f32 t);

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

// Transform Functions
Vector3 transform_point(const Transform& transform, const Vector3& point);
Transform inverse(const Transform& t);
Matrix4 transform_to_matrix4(const Transform& t);

} // namespace math


#if 0
#ifdef GB_OPENGL_TOOLS

enum class Shader_Type
{
	VERTEX,
	FRAGMENT,
};

struct Shader_Program
{
#define GB_MAX_UNIFORM_COUNT 32
	u32 handle;
	b32 is_linked;
	Allocator* allocator;

	const char* base_directory;


	u32         uniform_count;
	const char* uniform_names[GB_MAX_UNIFORM_COUNT];
	s32         uniform_locations[GB_MAX_UNIFORM_COUNT];
};


Shader_Program make_shader_program(gb::Allocator& allocator);
void destroy_shader_program(Shader_Program* program);

b32 attach_shader_from_file(Shader_Program* program, Shader_Type type, const char* filename);
b32 attach_shader_from_memory(Shader_Program* program, Shader_Type type, const char* source, usize len);

void use_shader_program(const Shader_Program* program);
b32 is_shader_program_in_use(const Shader_Program* program);
void stop_using_shader_program(const Shader_Program* program);

b32 link_shader_program(Shader_Program* program);

void bind_attrib_location(Shader_Program* program, const char* name);

s32 get_uniform_location(Shader_Program* program, const char* name);

#endif // GB_OPENGL_TOOLS
#endif
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
	Header* h = (Header*)::malloc(total);
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


	Header* h = get_header_ptr(ptr);

	total_allocated_count -= h->size;
	allocation_count--;

	::free(h);
}

s64
Heap_Allocator::allocated_size(const void* ptr)
{
	lock_mutex(mutex);
	defer(unlock_mutex(mutex));

	return get_header_ptr(ptr)->size;
}

s64
Heap_Allocator::total_allocated()
{
	return total_allocated_count;
}

Heap_Allocator::Header*
Heap_Allocator::get_header_ptr(const void* ptr)
{
	const usize* data = (usize*)ptr;
	data--;

	while (*data == GB_HEAP_ALLOCATOR_HEADER_PAD_VALUE)
		data--;

	return (Heap_Allocator::Header*)data;
}

Arena_Allocator::Arena_Allocator(void* base, usize base_size)
: base((u8*)base)
, base_size((s64)base_size)
, temp_count(0)
, total_allocated_count(0)
{
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

void Arena_Allocator::dealloc(void*) {}

s64 Arena_Allocator::allocated_size(const void*)
{
	return -1;
}

s64 Arena_Allocator::total_allocated()
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
/// String                   ///
////////////////////////////////
String make_string(Allocator& a, const char* str)
{
	return make_string(a, str, (String_Size)strlen(str));
}

String make_string(Allocator& a, const void* init_str, String_Size len)
{
	usize header_size = sizeof(String_Header);
	void* ptr = alloc(a, header_size + len + 1);
	if (!init_str)
		memset(ptr, 0, header_size + len + 1);

	if (ptr == nullptr)
		return nullptr;

	String str = (char*)ptr + header_size;
	String_Header* header = string_header(str);
	header->allocator = &a;
	header->len = len;
	header->cap = len;
	if (len && init_str)
		memcpy(str, init_str, len);
	str[len] = '\0';

	return str;
}


void free_string(String& str)
{
	if (str == nullptr)
		return;
	String_Header* h = string_header(str);
	Allocator* a = h->allocator;
	if (a) dealloc(*a, h);
	str = nullptr;
}

String duplicate_string(Allocator& a, const String str)
{
	return make_string(a, str, string_length(str));
}

String_Size string_length(const String str)
{
	return string_header(str)->len;
}

String_Size string_capacity(const String str)
{
	return string_header(str)->cap;
}

String_Size string_available_space(const String str)
{
	String_Header* h = string_header(str);
	if (h->cap > h->len)
		return h->cap - h->len;
	return 0;
}

void clear_string(String str)
{
	string_header(str)->len = 0;
	str[0] = '\0';
}

void append_string(String& str, const String other)
{
	append_string(str, (const void*)other, string_length(other));
}

void append_cstring(String& str, const char* other)
{
	append_string(str, (const void*)other, (String_Size)strlen(other));
}

void append_string(String& str, const void* other, String_Size other_len)
{
	String_Size curr_len = string_length(str);

	string_make_space_for(str, other_len);
	if (str == nullptr)
		return;

	memcpy(str + curr_len, other, other_len);
	str[curr_len + other_len] = '\0';
	string_header(str)->len = curr_len + other_len;
}


namespace impl
{
// NOTE(bill): ptr _must_ be allocated with Allocator& a
internal inline void*
string_realloc(Allocator& a, void* ptr, usize old_size, usize new_size)
{
	if (!ptr)
		return alloc(a, new_size);

	if (new_size < old_size)
		new_size = old_size;

	if (old_size == new_size)
		return ptr;

	void* new_ptr = alloc(a, new_size);
	if (!new_ptr)
		return nullptr;

	memcpy(new_ptr, ptr, old_size);

	dealloc(a, ptr);

	return new_ptr;
}
} // namespace impl

void string_make_space_for(String& str, String_Size add_len)
{
	String_Size len = string_length(str);
	String_Size new_len = len + add_len;

	String_Size available = string_available_space(str);
	if (available >= add_len) // Return if there is enough space left
		return;

	void* ptr = (String_Header*)str - 1;
	usize old_size = sizeof(String_Header) + string_length(str) + 1;
	usize new_size = sizeof(String_Header) + new_len + 1;

	Allocator* a = string_header(str)->allocator;
	void* new_ptr = impl::string_realloc(*a, ptr, old_size, new_size);
	if (new_ptr == nullptr)
		return;
	str = (char*)new_ptr + sizeof(String_Header);

	string_header(str)->cap = new_len;
}

usize string_allocation_size(const String str)
{
	String_Size cap = string_capacity(str);
	return sizeof(String_Header) + cap;
}

bool strings_are_equal(const String lhs, const String rhs)
{
	String_Size lhs_len = string_length(lhs);
	String_Size rhs_len = string_length(rhs);
	if (lhs_len != rhs_len)
		return false;

	for (String_Size i = 0; i < lhs_len; i++)
	{
		if (lhs[i] != rhs[i])
			return false;
	}

	return true;
}

void trim_string(String& str, const char* cut_set)
{
	char* start;
	char* end;
	char* start_pos;
	char* end_pos;

	start_pos = start = str;
	end_pos   = end   = str + string_length(str) - 1;

	while (start_pos <= end && strchr(cut_set, *start_pos))
		start_pos++;
	while (end_pos > start_pos && strchr(cut_set, *end_pos))
		end_pos--;

	String_Size len = (String_Size)((start_pos > end_pos) ? 0 : ((end_pos - start_pos)+1));

	if (str != start_pos)
		memmove(str, start_pos, len);
	str[len] = '\0';

	string_header(str)->len = len;
}


////////////////////////////////
/// Hash                     ///
////////////////////////////////

namespace hash
{
u32 adler32(const void* key, u32 num_bytes)
{
	const u32 MOD_ADLER = 65521;

	u32 a = 1;
	u32 b = 0;

	const u8* bytes = (const u8*)key;
	for (u32 i = 0; i < num_bytes; i++)
	{
		a = (a + bytes[i]) % MOD_ADLER;
		b = (b + a) % MOD_ADLER;
	}

	return (b << 16) | a;
}

global const u32 GB_CRC32_TABLE[256] = {
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

global const u64 GB_CRC64_TABLE[256] = {
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


u32 crc32(const void* key, u32 num_bytes)
{
	u32 result = (u32)~0;
	u8* c = (u8*)key;
	for (u32 remaining = num_bytes; remaining--; c++)
		result = (result >> 8) ^ (GB_CRC32_TABLE[(result ^ *c) & 0xff]);

	return ~result;
}

u64 crc64(const void* key, usize num_bytes)
{
	u64 result = (u64)~0;
	u8* c = (u8*)key;
	for (usize remaining = num_bytes; remaining--; c++)
		result = (result >> 8) ^ (GB_CRC64_TABLE[(result ^ *c) & 0xff]);

	return ~result;
}

// u32 fnv32(const void* key, usize num_bytes)
// {

// }

// u64 fnv64(const void* key, usize num_bytes)
// {

// }

// u32 fnv32a(const void* key, usize num_bytes)
// {

// }

// u64 fnv64a(const void* key, usize num_bytes)
// {

// }

u32 murmur32(const void* key, u32 num_bytes, u32 seed)
{
	local_persist const u32 c1 = 0xcc9e2d51;
	local_persist const u32 c2 = 0x1b873593;
	local_persist const u32 r1 = 15;
	local_persist const u32 r2 = 13;
	local_persist const u32 m = 5;
	local_persist const u32 n = 0xe6546b64;

	u32 hash = seed;

	const usize nblocks = num_bytes / 4;
	const u32* blocks = (const u32*)key;
	for (usize i = 0; i < nblocks; i++) {
		u32 k = blocks[i];
		k *= c1;
		k = (k << r1) | (k >> (32 - r1));
		k *= c2;

		hash ^= k;
		hash = ((hash << r2) | (hash >> (32 - r2))) * m + n;
	}

	const u8* tail = ((const u8*)key) + nblocks * 4;
	u32 k1 = 0;

	switch (num_bytes & 3) {
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

#ifdef GB_ARCH_64_BIT
u64 murmur64(const void* key, usize num_bytes, u64 seed)
{
	local_persist const u64 m = 0xc6a4a7935bd1e995ULL;
	local_persist const s32 r = 47;

	u64 h = seed ^ (num_bytes * m);

	const u64* data = (const u64*)key;
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

	const u8* data2 = (const u8*)data;

	switch (num_bytes & 7)
	{
	case 7: h ^= u64{data2[6]} << 48;
	case 6: h ^= u64{data2[5]} << 40;
	case 5: h ^= u64{data2[4]} << 32;
	case 4: h ^= u64{data2[3]} << 24;
	case 3: h ^= u64{data2[2]} << 16;
	case 2: h ^= u64{data2[1]} << 8;
	case 1: h ^= u64{data2[0]};
		h *= m;
	};

	h ^= h >> r;
	h *= m;
	h ^= h >> r;

	return h;
}
#elif GB_ARCH_32_BIT
u64 murmur64(const void* key, usize num_bytes, u64 seed)
{
	local_persist const u32 m = 0x5bd1e995;
	local_persist const s32 r = 24;

	u32 h1 = u32(seed) ^ num_bytes;
	u32 h2 = u32(seed >> 32);

	const u32* data = (const u32*)key;

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
	case 3: h2 ^= ((u8*)data)[2] << 16;
	case 2: h2 ^= ((u8*)data)[1] << 8;
	case 1: h2 ^= ((u8*)data)[0];
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
#error murmur64 function not supported on this architecture
#endif
} // namespace hash



////////////////////////////////
/// Time                     ///
////////////////////////////////
#ifdef GB_SYSTEM_WINDOWS

internal LARGE_INTEGER
win32_get_frequency()
{
	LARGE_INTEGER f;
	QueryPerformanceFrequency(&f);
	return f;
}

Time time_now()
{
	// NOTE(bill): std::chrono does not have a good enough precision in MSVC12
	// and below. This may have been fixed in MSVC14 but unsure as of yet.

	// Force the following code to run on first core
	// NOTE(bill): See
	// http://msdn.microsoft.com/en-us/library/windows/desktop/ms644904(v=vs.85).aspx
	HANDLE currentThread   = GetCurrentThread();
	DWORD_PTR previousMask = SetThreadAffinityMask(currentThread, 1);

	// Get the frequency of the performance counter
	// It is constant across the program's lifetime
	internal LARGE_INTEGER s_frequency = win32_get_frequency();

	// Get the current time
	LARGE_INTEGER t;
	QueryPerformanceCounter(&t);

	// Restore the thread affinity
	SetThreadAffinityMask(currentThread, previousMask);

	return microseconds(1000000ll * t.QuadPart / s_frequency.QuadPart);
}

void time_sleep(Time t)
{
	if (t.microseconds <= 0)
		return;

	// Get the supported timer resolutions on this system
	TIMECAPS tc;
	timeGetDevCaps(&tc, sizeof(TIMECAPS));
	// Set the timer resolution to the minimum for the Sleep call
	timeBeginPeriod(tc.wPeriodMin);

	// Wait...
	::Sleep(time_as_milliseconds(t));

	// Reset the timer resolution back to the system default
	timeBeginPeriod(tc.wPeriodMin);
}

#else
Time time_now()
{
	struct timespec spec;
	clock_gettime(CLOCK_REALTIME, &spec);

	return milliseconds((spec.tv_sec * 1000000ll) + (spec.tv_nsec * 1000ll));
}

void time_sleep(Time t)
{
	if (t.microseconds <= 0)
		return;

	struct timespec spec = {};
	spec.tv_sec = static_cast<s64>(time_as_seconds(t));
	spec.tv_nsec = 1000ll * (time_as_microseconds(t) % 1000000ll);

	nanosleep(&spec, nullptr);
}

#endif

Time seconds(f32 s)              { return {(s64)(s * 1000000ll)}; }
Time milliseconds(s32 ms)        { return {(s64)(ms * 1000l)}; }
Time microseconds(s64 us)        { return {us}; }
f32 time_as_seconds(Time t)      { return (f32)(t.microseconds / 1000000.0f); }
s32 time_as_milliseconds(Time t) { return (s32)(t.microseconds / 1000l); }
s64 time_as_microseconds(Time t) { return t.microseconds; }

bool operator==(Time left, Time right)
{
	return left.microseconds == right.microseconds;
}

bool operator!=(Time left, Time right)
{
	return !operator==(left, right);
}


bool operator<(Time left, Time right)
{
	return left.microseconds < right.microseconds;
}

bool operator>(Time left, Time right)
{
	return left.microseconds > right.microseconds;
}

bool operator<=(Time left, Time right)
{
	return left.microseconds <= right.microseconds;
}

bool operator>=(Time left, Time right)
{
	return left.microseconds >= right.microseconds;
}

Time operator-(Time right)
{
	return {-right.microseconds};
}

Time operator+(Time left, Time right)
{
	return {left.microseconds + right.microseconds};
}

Time operator-(Time left, Time right)
{
	return {left.microseconds - right.microseconds};
}

Time& operator+=(Time& left, Time right)
{
	return (left = left + right);
}

Time& operator-=(Time& left, Time right)
{
	return (left = left - right);
}

Time operator*(Time left, f32 right)
{
	return seconds(time_as_seconds(left) * right);
}

Time operator*(Time left, s64 right)
{
	return microseconds(time_as_microseconds(left) * right);
}

Time operator*(f32 left, Time right)
{
	return seconds(time_as_seconds(right) * left);
}

Time operator*(s64 left, Time right)
{
	return microseconds(time_as_microseconds(right) * left);
}

Time& operator*=(Time& left, f32 right)
{
	return (left = left * right);
}

Time& operator*=(Time& left, s64 right)
{
	return (left = left * right);
}

Time operator/(Time left, f32 right)
{
	return seconds(time_as_seconds(left) / right);
}

Time operator/(Time left, s64 right)
{
	return microseconds(time_as_microseconds(left) / right);

}

Time& operator/=(Time& left, f32 right)
{
	return (left = left / right);
}

Time& operator/=(Time& left, s64 right)
{
	return (left = left / right);
}

f32 operator/(Time left, Time right)
{
	return time_as_seconds(left) / time_as_seconds(right);
}

Time operator%(Time left, Time right)
{
	return microseconds(time_as_microseconds(left) % time_as_microseconds(right));
}

Time& operator%=(Time& left, Time right)
{
	return (left = left % right);
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

Vector3 operator*(const Quaternion& a, const Vector3& v) // Rotate v by q
{
	// return (q * Quaternion{v.x, v.y, v.z, 0} * math::conjugate(q)).xyz; // More Expensive
	const Vector3 t = 2.0f * math::cross(a.xyz, v);
	return (v + a.w * t + math::cross(a.xyz, t));
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

// Transform Operators
// World = Parent * Local
Transform operator*(const Transform& ps, const Transform& ls)
{
	Transform ws;

	ws.position    = ps.position + ps.orientation * (ps.scale * ls.position);
	ws.orientation = ps.orientation * ls.orientation;
	ws.scale       = ps.scale * (ps.orientation * ls.scale);

	return ws;
}

Transform& operator*=(Transform& ps, const Transform& ls)
{
	return (ps = ps * ls);
}

// Local = World / Parent
Transform operator/(const Transform& ws, const Transform& ps)
{
	Transform ls;

	const Quaternion ps_conjugate = math::conjugate(ps.orientation);

	ls.position    = (ps_conjugate * (ws.position - ps.position)) / ps.scale;
	ls.orientation = ps_conjugate * ws.orientation;
	ls.scale       = ps_conjugate * (ws.scale / ps.scale);

	return ls;
}

Transform& operator/=(Transform& ws, const Transform& ps)
{
	return (ws = ws / ps);
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
inline f32 sqrt(f32 x)       { return ::sqrtf(x);        }
inline f32 pow(f32 x, f32 y) { return (f32)::powf(x, y); }
inline f32 cbrt(f32 x)       { return (f32)::cbrtf(x);   }

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

inline f32 asin(f32 x)         { return ::asinf(x);     }
inline f32 acos(f32 x)         { return ::acosf(x);     }
inline f32 atan(f32 x)         { return ::atanf(x);     }
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
inline f32 ceil(f32 x)       { return ::ceilf(x);    }
inline f32 floor(f32 x)      { return ::floorf(x);   }
inline f32 mod(f32 x, f32 y) { return ::fmodf(x, y); }
inline f32 truncate(f32 x)   { return ::truncf(x);   }
inline f32 round(f32 x)      { return ::roundf(x);   }

inline s32 sign(s32 x) { return x >= 0 ? +1 : -1; }
inline s64 sign(s64 x) { return x >= 0 ? +1 : -1; }
inline f32 sign(f32 x) { return x >= 0.0f ? +1.0f : -1.0f; }

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


inline s32 min(s32 a, s32 b) { return a < b ? a : b; }
inline s64 min(s64 a, s64 b) { return a < b ? a : b; }
inline f32 min(f32 a, f32 b) { return a < b ? a : b; }

inline s32 max(s32 a, s32 b) { return a > b ? a : b; }
inline s64 max(s64 a, s64 b) { return a > b ? a : b; }
inline f32 max(f32 a, f32 b) { return a > b ? a : b; }

inline s32 clamp(s32 x, s32 min, s32 max)
{
	if (x < min)
		return min;
	if (x > max)
		return max;
	return x;
}
inline s64 clamp(s64 x, s64 min, s64 max)
{
	if (x < min)
		return min;
	if (x > max)
		return max;
	return x;
}
inline f32 clamp(f32 x, f32 min, f32 max)
{
	if (x < min)
		return min;
	if (x > max)
		return max;
	return x;
}

inline f32 lerp(f32 x, f32 y, f32 t)
{
	return x + (y-x)*t;
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


// Spherical Linear Interpolation
Quaternion slerp(const Quaternion& x, const Quaternion& y, f32 t)
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

	f32 angle = math::acos(cos_theta);

	Quaternion result = math::sin(1.0f - (t * angle)) * x + math::sin(t * angle) * z;
	return result * (1.0f / math::sin(angle));
}

// Shoemake's Quaternion Curves
// Sqherical Cubic Interpolation
inline Quaternion squad(const Quaternion& p,
                        const Quaternion& a,
                        const Quaternion& b,
                        const Quaternion& q,
                        f32 t)
{
	return slerp(slerp(p, q, t), slerp(a, b, t), 2.0f * t * (1.0f - t));
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


Quaternion
look_at_quaternion(const Vector3& eye, const Vector3& center, const Vector3& up)
{
	const f32 similar = 0.001f;

	if (math::magnitude(center - eye) < similar)
		return QUATERNION_IDENTITY; // You cannot look at where you are!

	// TODO(bill): Implement using just quaternions
	return matrix4_to_quaternion(look_at_matrix4(eye, center, up));
}

// Transform Functions
Vector3 transform_point(const Transform& transform, const Vector3& point)
{
	return (math::conjugate(transform.orientation) * (transform.position - point)) / transform.scale;
}

Transform inverse(const Transform& t)
{
	const Quaternion inv_orientation = math::conjugate(t.orientation);

	Transform inv_transform;

	inv_transform.position    = (inv_orientation * -t.position) / t.scale;
	inv_transform.orientation = inv_orientation;
	inv_transform.scale       = inv_orientation * (Vector3{1, 1, 1} / t.scale);

	return inv_transform;
}

Matrix4 transform_to_matrix4(const Transform& t)
{
	return math::translate(t.position) *                //
	       math::quaternion_to_matrix4(t.orientation) * //
	       math::scale(t.scale);                        //
}


} // namespace math
} // namespace gb

#endif // GB_IMPLEMENTATION
