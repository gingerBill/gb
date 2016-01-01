// gb.hpp - v0.32 - public domain C++11 helper library - no warranty implied; use at your own risk
// (Experimental) A C++11 helper library without STL geared towards game development

/*
LICENSE
	This software is in the public domain. Where that dedication is not
	recognized, you are granted a perpetual, irrevocable license to copy,
	distribute, and modify this file as you see fit.

WARNING
	- This library is _highly_ experimental and features may not work as expected.
	- This also means that many functions are not documented.
	- This library is not compatible with STL at all! (By design)

CONTENTS:
	- Common Macros
	- Assert
	- Types
	- Type Traits
	- C++11 Move Semantics
	- Defer
	- Casts
		- bit_cast
		- pseudo_cast
	- Memory
		- Mutex
		- Atomics
		- Semaphore
		- Thread
		- Allocator
		- Heap Allocator
		- Arena Allocator
		- Functions
	- String
	- Array
	- Hash Table
	- Hash Functions
*/

/*
Version History:
	0.33  - Explicit Everything! No ctor/dtor on Array<T> and Hash_Table<T>
	0.32  - Change const position convention
	0.31a - Minor fixes
	0.31  - Remove `_Allocator` suffix for allocator types
	0.30  - sort::quick
	0.29  - GB_ASSERT prints call stack
	0.28  - Pool Allocator
	0.27  - Dealloc to Free & More Hashing Functions
	0.26a - Heap_Allocator Fix
	0.26  - Better Allocation system
	0.25a - Array bug fix
	0.25  - Faster Heap_Allocator for Windows using HeapAlloc
	0.24b - Even More Hash_Table Bug Fixes
	0.24a - Hash_Table Bug Fixes
	0.24  - More documentation and bug fixes
	0.23  - Move Semantics for Array and Hash_Table
	0.22  - Code rearrangment into namespaces
	0.21d - Fix array::free
	0.21c - Fix Another Typo causing unresolved external symbol
	0.21b - Typo fixes
	0.21a - Better `static` keywords
	0.21  - Separate Math Library
	0.20a - #ifndef for many macros
	0.20  - Angle
	0.19  - Cache friendly Transform and String fixes
	0.18  - Hash_Table bug fixes
	0.17  - Death to OOP
	0.16  - All References are const convention
	0.15  - Namespaced Types
	0.14  - Casts and Quaternion Look At
	0.13a - Fix Todos
	0.13  - Basic Type Traits
	0.12  - Random
	0.11  - Complex
	0.10  - Atomics
	0.09  - Bug Fixes
	0.08  - Matrix(2,3)
	0.07  - Bug Fixes
	0.06  - Os spec ideas
	0.05  - Transform Type and Quaternion Functions
	0.04  - String
	0.03  - Hash Functions
	0.02  - Hash Table
	0.01  - Initial Version
*/


#ifndef GB_INCLUDE_GB_HPP
#define GB_INCLUDE_GB_HPP

#if !defined(__cplusplus) && __cplusplus >= 201103L
	#error This library is only for C++11 and above
#endif

// NOTE(bill): Because static means three different things in C/C++
//             Great Design(!)
#ifndef global_variable
#define global_variable  static
#define internal_linkage static
#define local_persist    static
#endif

/*
	Example for static defines

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

// `global_variable`  : variable is in the global scope
// `internal_linkage` : function is only visible for this linkage
// `local_persist`    : variable persists for the particular function scope


#if defined(_MSC_VER)
	#define _ALLOW_KEYWORD_MACROS

	#ifndef alignof // Needed for MSVC 2013 'cause Microsoft "loves" standards
	#define alignof(x) __alignof(x)
	#endif
#endif


////////////////////////////////
//                            //
// System OS                  //
//                            //
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


#if defined(_MSC_VER)
	// Microsoft Visual Studio
	#define GB_COMPILER_MSVC 1
#elif defined(__clang__)
	// Clang
	#define GB_COMPILER_CLANG 1
#elif defined(__GNUC__) || defined(__GNUG__) && !(defined(__clang__) || defined(__INTEL_COMPILER))
	// GNU GCC/G++ Compiler
	#define GB_COMPILER_GNU_GCC 1
#elif defined(__INTEL_COMPILER)
	// Intel C++ Compiler
	#define GB_COMPILER_INTEL 1
#endif

////////////////////////////////
//                            //
// Environment Bit Size       //
//                            //
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
	#define GB_IS_BIG_EDIAN    (!*(unsigned char*)&(unsigned short){1})
	#define GB_IS_LITTLE_EDIAN (!GB_IS_BIG_EDIAN)
#endif

#ifndef GB_IS_POWER_OF_TWO
#define GB_IS_POWER_OF_TWO(x) ((x) != 0) && !((x) & ((x) - 1))
#endif

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





////////////////////////////////
//                            //
// Includes                   //
//                            //
////////////////////////////////

#include <math.h>
#include <stdarg.h>
#if !defined(GB_NO_STDIO)
#include <stdio.h>
#endif

#if defined(GB_SYSTEM_WINDOWS)
	#define NOMINMAX            1
	#define VC_EXTRALEAN        1
	#define WIN32_EXTRA_LEAN    1
	#define WIN32_LEAN_AND_MEAN 1

	#include <windows.h> // TODO(bill): Should we include only the needed headers?
	#include <mmsystem.h> // Time functions

	#undef NOMINMAX
	#undef VC_EXTRALEAN
	#undef WIN32_EXTRA_LEAN
	#undef WIN32_LEAN_AND_MEAN

	#include <intrin.h>
#else
	#include <pthread.h>
	#include <sys/time.h>
#endif


#ifndef GB_UNUSED
#define GB_UNUSED(x) ((void)sizeof(x))
#endif

#if !defined(GB_ASSERT)
	#if !defined(NDEBUG)
		#define GB_ASSERT(x, ...) ((void)(gb__assert_handler((x), #x, __FILE__, __LINE__, ##__VA_ARGS__)))

		// Helper function used as a better alternative to assert which allows for
		// optional printf style error messages
		extern "C" void
		gb__assert_handler(bool condition, char const* condition_str,
		                   char const* filename, size_t line,
		                   char const* error_text = nullptr, ...);
	#else
		#define GB_ASSERT(x, ...) ((void)sizeof(x))
	#endif
#endif

////////////////////////////////
//                            //
// snprintf_msvc              //
//                            //
////////////////////////////////
#if !defined(GB_NO_STDIO) && defined(_MSC_VER)
	extern "C" inline int
	gb__vsnprintf_compatible(char* buffer, size_t size, char const* format, va_list args)
	{
		int result = -1;
		if (size > 0)
			result = _vsnprintf_s(buffer, size, _TRUNCATE, format, args);
		if (result == -1)
			return _vscprintf(format, args);

		return result;
	}

	extern "C" inline int
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
	#endif // GB_DO_NOT_USE_MSVC_SPRINTF_FIX
#endif


#if defined(GB_NO_GB_NAMESPACE)
	#define __GB_NAMESPACE_START
	#define __GB_NAMESPACE_END
	#define __GB_NAMESPACE_PREFIX
#else
	#ifndef __GB_NAMESPACE_PREFIX
	#define __GB_NAMESPACE_PREFIX gb
	#endif

	// NOTE(bill): __GB_NAMESPACE_PREFIX cannot be blank
	// This is why macros that not meant to be touched have `__` prefix
	// You can change them if you know what you are doing

	#define __GB_NAMESPACE_START namespace __GB_NAMESPACE_PREFIX {
	#define __GB_NAMESPACE_END   } // namespace __GB_NAMESPACE_PREFIX
#endif

#if !defined(GB_BASIC_WITHOUT_NAMESPACE)
__GB_NAMESPACE_START
#endif // GB_BASIC_WITHOUT_NAMESPACE

////////////////////////////////
//                            //
// Types                      //
//                            //
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
		// NOTE(bill): Of the platforms that I build for, these will be correct
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
		using bool8 = bool;
	#else
		using bool8 = s8;
	#endif
	using bool32 = s32;

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
////////////////////////////////
//                            //
// C++11 Types Traits         //
//                            //
////////////////////////////////

template <typename T> struct Add_Const_Def { using Type = const T; };
template <typename T> using  Add_Const = typename Add_Const_Def<T>::Type;

template <typename T> struct Add_Volatile_Def { using Type = volatile T; };
template <typename T> using  Add_Volatile = typename Add_Volatile_Def<T>::Type;

template <typename T> using  Add_Const_Volatile = Add_Const<Add_Volatile<T>>;

template <typename T> struct Add_Lvalue_Reference_Def                      { using Type = T&;                  };
template <typename T> struct Add_Lvalue_Reference_Def<T&>                  { using Type = T&;                  };
template <typename T> struct Add_Lvalue_Reference_Def<T&&>                 { using Type = T&;                  };
template <>           struct Add_Lvalue_Reference_Def<void>                { using Type = void;                };
template <>           struct Add_Lvalue_Reference_Def<void const>          { using Type = void const;          };
template <>           struct Add_Lvalue_Reference_Def<void volatile>       { using Type = void volatile;       };
template <>           struct Add_Lvalue_Reference_Def<void const volatile> { using Type = void const volatile; };
template <typename T> using  Add_Lvalue_Reference = typename Add_Lvalue_Reference_Def<T>::Type;

template <typename T> struct Add_Rvalue_Reference_Def                      { using Type = T&&;                 };
template <typename T> struct Add_Rvalue_Reference_Def<T&>                  { using Type = T&;                  };
template <typename T> struct Add_Rvalue_Reference_Def<T&&>                 { using Type = T&&;                 };
template <>           struct Add_Rvalue_Reference_Def<void>                { using Type = void;                };
template <>           struct Add_Rvalue_Reference_Def<void const>          { using Type = void const;          };
template <>           struct Add_Rvalue_Reference_Def<void volatile>       { using Type = void volatile;       };
template <>           struct Add_Rvalue_Reference_Def<void const volatile> { using Type = void const volatile; };
template <typename T> using  Add_Rvalue_Reference = typename Add_Rvalue_Reference_Def<T>::Type;


template <typename T> struct Remove_Pointer_Def                    { using Type = T; };
template <typename T> struct Remove_Pointer_Def<T*>                { using Type = T; };
template <typename T> struct Remove_Pointer_Def<T* const>          { using Type = T; };
template <typename T> struct Remove_Pointer_Def<T* volatile>       { using Type = T; };
template <typename T> struct Remove_Pointer_Def<T* const volatile> { using Type = T; };
template <typename T> using  Remove_Pointer = typename Remove_Pointer_Def<T>::Type;

template <typename T> struct Add_Pointer_Def { using Type = T*; };
template <typename T> using  Add_Pointer = typename Add_Pointer_Def<T>::Type;

template <typename T> struct Remove_Const_Def            { using Type = T; };
template <typename T> struct Remove_Const_Def<const T>   { using Type = T; };
template <typename T> using  Remove_Const = typename Remove_Const_Def<T>::Type;

template <typename T> struct Remove_Volatile_Def             { using Type = T; };
template <typename T> struct Remove_Volatile_Def<volatile T> { using Type = T; };
template <typename T> using  Remove_Volatile = typename Remove_Const_Def<T>::Type;

template <typename T> using  Remove_Const_Volatile = Remove_Const<Remove_Volatile<T>>;

template <typename T> struct Remove_Reference_Def      { using Type = T; };
template <typename T> struct Remove_Reference_Def<T&>  { using Type = T; };
template <typename T> struct Remove_Reference_Def<T&&> { using Type = T; };
template <typename T> using  Remove_Reference = typename Remove_Reference_Def<T>::Type;

template <typename T, T v> struct Integral_Constant { global_variable const T VALUE = v; using Value_Type = T; using Type = Integral_Constant; };

template <typename T, usize N = 0>      struct Extent          : Integral_Constant<usize, 0> {};
template <typename T>                   struct Extent<T[],  0> : Integral_Constant<usize, 0> {};
template <typename T, usize N>          struct Extent<T[],  N> : Integral_Constant<usize, Extent<T, N-1>::VALUE> {};
template <typename T, usize N>          struct Extent<T[N], 0> : Integral_Constant<usize, N> {};
template <typename T, usize I, usize N> struct Extent<T[I], N> : Integral_Constant<usize, Extent<T, N-1>::VALUE> {};

template <typename T>          struct Remove_Extent_Def       { using Type = T; };
template <typename T>          struct Remove_Extent_Def<T[]>  { using Type = T; };
template <typename T, usize N> struct Remove_Extent_Def<T[N]> { using Type = T; };

// TODO(bill): Do I "need" all of these template traits?


////////////////////////////////
//                            //
// C++11 Move Semantics       //
//                            //
////////////////////////////////

template <typename T>
inline T&&
forward_ownership(Remove_Reference<T>& t)
{
	return static_cast<T&&>(t);
}

template <typename T>
inline T&&
forward_ownership(Remove_Reference<T>&& t)
{
	return static_cast<T&&>(t);
}

template <typename T>
inline Remove_Reference<T>&&
move_ownership(T&& t)
{
	return static_cast<Remove_Reference<T>&&>(t);
}
__GB_NAMESPACE_END





////////////////////////////////
//                            //
// Defer                      //
//                            //
////////////////////////////////

#ifndef GB_DEFER
#define GB_DEFER
	__GB_NAMESPACE_START
	namespace impl
	{
	template <typename Func>
	struct Defer
	{
		Func f;

		Defer(Func&& f) : f{forward_ownership<Func>(f)} {}
		~Defer() { f(); };
	};

	template <typename Func>
	inline Defer<Func>
	defer_func(Func&& f) { return Defer<Func>(forward_ownership<Func>(f)); }
	} // namespace impl
	__GB_NAMESPACE_END

	// NOTE(bill): These macros are in the global namespace thus, defer can be treated without a __GB_NAMESPACE_PREFIX:: prefix
	#define GB_DEFER_1(x, y) x##y
	#define GB_DEFER_2(x, y) GB_DEFER_1(x, y)
	#define GB_DEFER_3(x)    GB_DEFER_2(GB_DEFER_2(GB_DEFER_2(x, __COUNTER__), _), __LINE__)
	#define defer(code) auto GB_DEFER_3(_defer_) = ::impl::defer_func([&](){code;})

	/* EXAMPLES

	// `defer (...)` will defer a statement till the end of scope

	FILE* file = fopen("test.txt", "rb");
	if (file == nullptr)
	{
		// Handle Error
	}
	defer (fclose(file)); // Will always be called at the end of scope

	//

	auto m = mutex::make();
	defer (mutex::destroy(&m)); // Mutex will be destroyed at the end of scope

	{
		mutex::lock(&m);
		defer (mutex::unlock(&m)); // Mutex will unlock at end of scope

		// Do whatever
	}

	// You can scope multiple statements together if needed with {...}
	defer ({
		func1();
		func2();
		func3();
	});

	*/
#endif





#if !defined(GB_CASTS_WITHOUT_NAMESPACE)
__GB_NAMESPACE_START
#endif // GB_CASTS_WITHOUT_NAMESPACE

#ifndef GB_SPECIAL_CASTS
#define GB_SPECIAL_CASTS
	// NOTE(bill): Very similar to doing `*(T*)(&u)`
	template <typename Dest, typename Source>
	inline Dest
	bit_cast(Source const& source)
	{
		static_assert(sizeof(Dest) <= sizeof(Source),
		              "bit_cast<Dest>(Source const&) - sizeof(Dest) <= sizeof(Source)");
		Dest dest;
		::memcpy(&dest, &source, sizeof(Dest));
		return dest;
	}

	// IMPORTANT NOTE(bill): Very similar to doing `*(T*)(&u)` but easier/clearer to write
	// however, it can be dangerous if sizeof(T) > sizeof(U) e.g. unintialized memory, undefined behavior
	// *(T*)(&u) ~~ pseudo_cast<T>(u)
	template <typename T, typename U>
	inline T
	pseudo_cast(U const& u)
	{
		return reinterpret_cast<T const&>(u);
	}

	/*
		EXAMPLES:

		// bit_cast

		u8 arr[4] = {0x78, 0x56, 0x34, 0x12};
		u32 var = bit_cast<u32>(arr); // Little edian => 0x12345678

		// pseudo_cast - except from gb_math.hpp
		Sphere
		calculate_min_bounding(void const* vertices, usize num_vertices, usize stride, usize offset, f32 step)
		{
			auto gen = random::make(0);

			u8 const* vertex = reinterpret_cast<u8 const*>(vertices);
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

	 */
#endif

// FORENOTE(bill): There used to be a magic_cast that was equivalent to
// a C-style cast but I removed it as I could not get it work as intented
// for everything using only C++ style casts (it needed to a c-style cast)

#if !defined(GB_CASTS_WITHOUT_NAMESPACE)
__GB_NAMESPACE_END
#endif // GB_CASTS_WITHOUT_NAMESPACE





__GB_NAMESPACE_START

////////////////////////////////
//                            //
// Memory                     //
//                            //
////////////////////////////////

template <typename T, usize N>
inline usize array_count(T const(& )[N]) { return N; }

inline s64 kilobytes(s64 x) { return          (x) * 1024ll; }
inline s64 megabytes(s64 x) { return kilobytes(x) * 1024ll; }
inline s64 gigabytes(s64 x) { return megabytes(x) * 1024ll; }
inline s64 terabytes(s64 x) { return gigabytes(x) * 1024ll; }




struct Mutex
{
#if defined(GB_SYSTEM_WINDOWS)
	HANDLE win32_mutex;
#else
	pthread_mutex_t posix_mutex;
#endif
};

namespace mutex
{
Mutex make();
void destroy(Mutex* mutex);
void lock(Mutex* mutex);
bool try_lock(Mutex* mutex);
void unlock(Mutex* mutex);
} // namespace mutex




// Atomic Types
struct Atomic32 { u32 nonatomic; };
struct Atomic64 { u64 nonatomic; };

namespace atomic
{
u32  load(Atomic32 const volatile* object);
void store(Atomic32 volatile* object, u32 value);
u32  compare_exchange_strong(Atomic32 volatile* object, u32 expected, u32 desired);
u32  exchanged(Atomic32 volatile* object, u32 desired);
u32  fetch_add(Atomic32 volatile* object, s32 operand);
u32  fetch_and(Atomic32 volatile* object, u32 operand);
u32  fetch_or(Atomic32 volatile* object, u32 operand);

u64  load(Atomic64 const volatile* object);
void store(Atomic64 volatile* object, u64 value);
u64  compare_exchange_strong(Atomic64 volatile* object, u64 expected, u64 desired);
u64  exchanged(Atomic64 volatile* object, u64 desired);
u64  fetch_add(Atomic64 volatile* object, s64 operand);
u64  fetch_and(Atomic64 volatile* object, u64 operand);
u64  fetch_or(Atomic64 volatile* object, u64 operand);
} // namespace atomic




struct Semaphore
{
#if defined(GB_SYSTEM_WINDOWS)
	HANDLE win32_handle;
#else
	Mutex          mutex;
	pthread_cond_t cond;
	s32            count;
#endif
};

namespace semaphore
{
Semaphore make();
void destroy(Semaphore* semaphore);
void post(Semaphore* semaphore, u32 count = 1);
void wait(Semaphore* semaphore);
} // namespace semaphore




// TODO(bill): Is this thread procedure definition good enough?
using Thread_Procedure = void(void*);

struct Thread
{
#if defined(GB_SYSTEM_WINDOWS)
	HANDLE win32_handle;
#else
	pthread_t posix_handle;
#endif

	Thread_Procedure* function;
	void*            data;

	Semaphore semaphore;
	usize     stack_size;
	bool32    is_running;
};

namespace thread
{
Thread make();
void destroy(Thread* t);
void start(Thread* t, Thread_Procedure* func, void* data = nullptr, usize stack_size = 0);
void join(Thread* t);
bool is_running(Thread const& t);
u32  current_id();
} // namespace thread




// Default alignment for memory allocations
#ifndef GB_DEFAULT_ALIGNMENT
#define GB_DEFAULT_ALIGNMENT 8
#endif

// Base class for memory allocators - Pretty much a vtable
struct Allocator
{
	// Allocates the specified amount of memory aligned to the specified alignment
	void* (*alloc)(Allocator* a, usize size, usize align);
	// Frees an allocation made with alloc()
	void (*free)(Allocator* a, void* ptr);
	// Returns the amount of usuable memory allocated at `ptr`.
	///
	// If the allocator does not support tracking of the allocation size,
	// the function will return -1
	s64 (*allocated_size)(Allocator* a, void const* ptr);
	// Returns the total amount of memory allocated by this allocator
	///
	// If the allocator does not track memory, the function will return -1
	s64 (*total_allocated)(Allocator* a);
};




struct Heap : Allocator
{
	struct Header
	{
		usize size;
	};

	Mutex  mutex;
	bool32 use_mutex;
	s64    total_allocated_count;
	s64    allocation_count;

#if defined(GB_SYSTEM_WINDOWS)
	HANDLE win32_heap_handle;
#endif
};

namespace heap
{
Heap make(bool use_mutex = true);
void destroy(Heap* heap);
} // namespace heap





struct Arena : Allocator
{
	Allocator* backing;
	void*      physical_start;
	s64        total_size;
	s64        total_allocated_count;
	s64        temp_count;
};

namespace arena
{
Arena make(Allocator* backing, usize size);
Arena make(void* start, usize size);
void destroy(Arena* arena);
void clear(Arena* arena);
} // namespace arena





struct Temporary_Arena_Memory
{
	Arena* arena;
	s64    original_count;
};

namespace temporary_arena_memory
{
Temporary_Arena_Memory make(Arena* arena);
void free(Temporary_Arena_Memory tmp);
} // namespace temporary_arena_memory




struct Pool : Allocator
{
	Allocator* backing;

	void* physical_start;
	void* free_list;

	usize block_size;
	usize block_align;
	s64   total_size;
};

namespace pool
{
Pool make(Allocator* backing, usize num_blocks, usize block_size,
          usize block_align = GB_DEFAULT_ALIGNMENT);
void destroy(Pool* pool);
} // namespace pool




namespace memory
{
void* align_forward(void* ptr, usize align);

void*       pointer_add(void*       ptr, usize bytes);
void*       pointer_sub(void*       ptr, usize bytes);
void const* pointer_add(void const* ptr, usize bytes);
void const* pointer_sub(void const* ptr, usize bytes);

template <typename T>
void fill(T* ptr, usize count, T const& value);

template <typename T>
void fill(T* ptr, usize count, T&& value);

void zero(void* ptr, usize bytes);
void copy(void const* src, usize bytes, void* dest);
void move(void const* src, usize bytes, void* dest);
bool equals(void const* a, void const* b, usize bytes);

template <typename T>
void zero_struct(T* ptr);

template <typename T>
void zero_array(T* ptr, usize count);

template <typename T>
void copy_array(T const* src_array, usize count, T* dest_array);

template <typename T>
void copy_struct(T const* src_array, T* dest_array);


template <typename T>
void swap(T* a, T* b);

template <typename T, usize N>
void swap(T (& a)[N], T (& b)[N]);
} // namespace memory




// Allocator Functions
void* alloc(Allocator* a, usize size, usize align = GB_DEFAULT_ALIGNMENT);
void  free(Allocator* a, void* ptr);
s64   allocated_size(Allocator* a, void const* ptr);
s64   total_allocated(Allocator* a);

template <typename T>
inline T* alloc_struct(Allocator* a) { return static_cast<T*>(alloc(a, sizeof(T), alignof(T))); }

template <typename T>
inline T* alloc_array(Allocator* a, usize count) { return static_cast<T*>(alloc(a, count * sizeof(T), alignof(T))); }





////////////////////////////////
//                            //
// String                     //
//                            //
// C compatible string        //
//                            //
////////////////////////////////

// A "better" string type that is compatible with C style read-only functions
using String = char*;

namespace string
{
using Size = u32;

struct Header
{
	Allocator* allocator;
	Size length;
	Size capacity;
};

inline Header* header(String str) { return reinterpret_cast<Header*>(str) - 1; }

String make(Allocator* a, char const* str = "");
String make(Allocator* a, void const* str, Size num_bytes);
void   free(String str);

String duplicate(Allocator* a, String const str);

Size length(String const str);
Size capacity(String const str);
Size available_space(String const str);

void clear(String str);

void append(String* str, char c);
void append(String* str, String const other);
void append_cstring(String* str, char const* other);
void append(String* str, void const* other, Size num_bytes);

void make_space_for(String* str, Size add_len);
usize allocation_size(String const str);

bool equals(String const lhs, String const rhs);
int compare(String const lhs, String const rhs); // NOTE(bill): three-way comparison

void trim(String* str, char const* cut_set);
void trim_space(String* str);
} // namespace string


// TODO(bill): string libraries



////////////////////////////////
//                            //
// Array                      //
//                            //
////////////////////////////////

#ifndef GB_ARRAY_BOUND_CHECKING
#define GB_ARRAY_BOUND_CHECKING 1
#endif

// Dynamic resizable array for POD types only
template <typename T>
struct Array
{
	using Type = T;

	Allocator* allocator;
	s64        count;
	s64        capacity;
	T*         data;

	T const& operator[](usize index) const;
	T&       operator[](usize index);
};

// NOTE(bill): There are not ctor/dtor for Array<T>.
// These are explicit functions e.g.
/*
auto old_array = array::make<T>(...);
auto new_array = array::copy(old_array);
array::free(&old_array);
array::free(&new_array);
*/
// This allows functions to be passed by value at a low cost

namespace array
{
// Helper functions to make, free, and copy an array
template <typename T> Array<T> make(Allocator* allocator, usize count = 0);
template <typename T> void     free(Array<T>* array);
// TODO(bill): Is passing by value okay here or is pass by const& ?
// (sizeof(Array<T>) = 16 + sizeof(void*)) (24 bytes on x86, 32 bytes on x64)
template <typename T> Array<T> copy(Array<T> array, Allocator* allocator = nullptr);

// Appends `item` to the end of the array
template <typename T> void append(Array<T>* a, T const& item);
template <typename T> void append(Array<T>* a, T&& item);
// Appends `items[count]` to the end of the array
template <typename T> void append(Array<T>* a, T const* items, usize count);
// Append the contents of another array of the same type
template <typename T> void append(Array<T>* a, Array<T> other);

// Pops the last item form the array. The array cannot be empty.
template <typename T> void pop(Array<T>* a);

// Removes all items from the array - does not free memory
template <typename T> void clear(Array<T>* a);
// Modify the size of a array - only reallocates when necessary
template <typename T> void resize(Array<T>* a, usize count);
// Makes sure that the array has at least the specified capacity - or the array the grows
template <typename T> void reserve(Array<T>* a, usize capacity);
// Reallocates the array to the specific capacity
template <typename T> void set_capacity(Array<T>* a, usize capacity);
// Grows the array to keep append() to be O(1)
template <typename T> void grow(Array<T>* a, usize min_capacity = 0);
} // namespace array

// Used to iterate over the array with a C++11 for loop
template <typename T> inline T*       begin(Array<T>& a)       { return a.data; }
template <typename T> inline T const* begin(Array<T> const& a) { return a.data; }
template <typename T> inline T*       begin(Array<T>&& a)      { return a.data; }
template <typename T> inline T*       end(Array<T>& a)         { return a.data + a.count; }
template <typename T> inline T const* end(Array<T> const& a)   { return a.data + a.count; }
template <typename T> inline T*       end(Array<T>&& a)        { return a.data + a.count; }





////////////////////////////////
//                            //
// Hash Table                 //
//                            //
////////////////////////////////

// Hash table for POD types only with a u64 key
template <typename T>
struct Hash_Table
{
	using Type = T;

	struct Entry
	{
		u64 key;
		s64 next;
		T   value;
	};

	Array<s64>   hashes;
	Array<Entry> entries;
};

namespace hash_table
{
// Helper function to make, free, and copy a hash table
template <typename T> Hash_Table<T> make(Allocator* a);
template <typename T> void          free(Hash_Table<T>* h);
template <typename T> Hash_Table<T> copy(Hash_Table<T> const& h, Allocator* a = nullptr);

// Return `true` if the specified key exist in the hash table
template <typename T> bool has(Hash_Table<T> const& h, u64 key);
// Returns the value stored at the key, or a `default_value` if the key is not found in the hash table
template <typename T> T const& get(Hash_Table<T> const& h, u64 key, T const& default_value);
// Sets the value for the key in the hash table
template <typename T> void set(Hash_Table<T>* h, u64 key, T const& value);
template <typename T> void set(Hash_Table<T>* h, u64 key, T&& value);
// Removes the key from the hash table if it exists
template <typename T> void remove(Hash_Table<T>* h, u64 key);
// Resizes the hash table's lookup table to the specified size
template <typename T> void reserve(Hash_Table<T>* h, usize capacity);
// Remove all elements from the hash table
template <typename T> void clear(Hash_Table<T>* h);
} // namespace hash_table

// Used to iterate over the array with a C++11 for loop - in random order
template <typename T> typename Hash_Table<T>::Entry const* begin(Hash_Table<T> const& h);
template <typename T> typename Hash_Table<T>::Entry const* end(Hash_Table<T> const& h);

namespace multi_hash_table
{
// Outputs all the items that with the specified key
template <typename T> void get(Hash_Table<T> const& h, u64 key, Array<T>& items);
// Returns the count of entries with the specified key
template <typename T> usize count(Hash_Table<T> const& h, u64 key);

// Finds the first entry with specified key in the hash table
template <typename T> typename Hash_Table<T>::Entry const* find_first(Hash_Table<T> const& h, u64 key);
// Finds the next entry with same key as `e`
template <typename T> typename Hash_Table<T>::Entry const* find_next(Hash_Table<T> const& h, typename Hash_Table<T>::Entry const* e);

// Inserts the `value` as an additional value for the specified key
template <typename T> void insert(Hash_Table<T>* h, u64 key, T const& value);
template <typename T> void insert(Hash_Table<T>* h, u64 key, T&& value);
// Removes a specified entry `e` from the hash table
template <typename T> void remove_entry(Hash_Table<T>* h, typename Hash_Table<T>::Entry const* e);
// Removes all entries with from the hash table with the specified key
template <typename T> void remove_all(Hash_Table<T>* h, u64 key);
} // namespace multi_hash_table





////////////////////////////////
//                            //
// Hash                       //
//                            //
////////////////////////////////

namespace hash
{
u32 adler32(void const* key, u32 num_bytes);

u32 crc32(void const* key, u32 num_bytes);
u64 crc64(void const* key, usize num_bytes);

u32 fnv32(void const* key, usize num_bytes);
u64 fnv64(void const* key, usize num_bytes);
u32 fnv32a(void const* key, usize num_bytes);
u64 fnv64a(void const* key, usize num_bytes);

u32 murmur32(void const* key, u32 num_bytes, u32 seed = 0x9747b28c);
u64 murmur64(void const* key, usize num_bytes, u64 seed = 0x9747b28c);
} // namespace hash



////////////////////////////////
//                            //
// Sort                       //
//                            //
////////////////////////////////

namespace sort
{
// Comparison_Function
// NOTE(bill): Similar to str(n)cmp
// a <  b --> -1
// a == b -->  0
// a >  b --> +1

// Quick Sort (Qsort)
template <typename T, typename Comparison_Function>
void quick(T* array, usize count, Comparison_Function compare);

// TODO(bill): Implement other sorting algorithms
} // namespace sort


////////////////////////////////
//                            //
// Time                       //
//                            //
////////////////////////////////

struct Time
{
	s64 microseconds;
};

extern Time const TIME_ZERO;

namespace time
{
Time now();
void sleep(Time time);

Time seconds(f32 s);
Time milliseconds(s32 ms);
Time microseconds(s64 us);

f32 as_seconds(Time t);
s32 as_milliseconds(Time t);
s64 as_microseconds(Time t);
} // namespace time

bool operator==(Time left, Time right);
bool operator!=(Time left, Time right);

bool operator<(Time left, Time right);
bool operator>(Time left, Time right);

bool operator<=(Time left, Time right);
bool operator>=(Time left, Time right);

Time operator+(Time right);
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
//                            //
// OS                         //
//                            //
////////////////////////////////

// TODO(bill): Should this be system:: vs os:: ?
namespace os
{
u64 rdtsc();
} // namespace os






////////////////////////////////
//                            //
// Template Implementations   //
//                            //
////////////////////////////////

////////////////////////////////
//                            //
// Array                      //
//                            //
////////////////////////////////

template <typename T>
inline T const&
Array<T>::operator[](usize index) const
{
#if GB_ARRAY_BOUND_CHECKING
	GB_ASSERT(index < static_cast<usize>(capacity), "Array out of bounds");
#endif
	return data[index];
}

template <typename T>
inline T&
Array<T>::operator[](usize index)
{
#if GB_ARRAY_BOUND_CHECKING
	GB_ASSERT(index < static_cast<usize>(capacity), "Array out of bounds");
#endif
	return data[index];
}


namespace array
{
template <typename T>
inline Array<T>
make(Allocator* allocator, usize count)
{
	Array<T> result = {};
	result.allocator = allocator;

	if (count > 0)
	{
		result.data = alloc_array<T>(allocator, count);
		if (result.data)
			result.count = result.capacity = count;
	}

	return result;
}

template <typename T>
inline void
free(Array<T>* a)
{
	if (a->allocator)
		free(a->allocator, a->data);
	a->count    = 0;
	a->capacity = 0;
	a->data     = nullptr;
}

template <typename T>
inline Array<T>
copy(Array<T> other, Allocator* allocator)
{
	Array<T> result = {};

	if (allocator)
		result.allocator = allocator;
	else
		result.allocator = other.allocator;

	auto new_count = other.count;

	array::resize(&result, new_count);
	memory::copy_array(other.data, new_count, data);

	return result;
}




template <typename T>
inline void
append(Array<T>* a, T const& item)
{
	if (a->capacity < a->count + 1)
		array::grow(a);
	a->data[a->count++] = item;
}

template <typename T>
inline void
append(Array<T>* a, T&& item)
{
	if (a->capacity < a->count + 1)
		array::grow(a);
	a->data[a->count++] = move_ownership(item);
}

template <typename T>
inline void
append(Array<T>* a, T const* items, usize count)
{
	if (a->capacity <= a->count + static_cast<s64>(count))
		array::grow(a, a->count + count);

	memory::copy_array(items, count, &a->data[a->count]);
	a->count += count;
}

template <typename T>
inline void
append(Array<T>* a, Array<T> other)
{
	array::append(a, other.data, other.count);
}


template <typename T>
inline void
pop(Array<T>* a)
{
	GB_ASSERT(a->count > 0);

	a->count--;
}

template <typename T>
inline void
clear(Array<T>* a)
{
	a->count = 0;
}

template <typename T>
inline void
resize(Array<T>* a, usize count)
{
	if (a->capacity < static_cast<s64>(count))
		array::grow(a, count);
	a->count = count;
}

template <typename T>
inline void
reserve(Array<T>* a, usize capacity)
{
	if (a->capacity < static_cast<s64>(capacity))
		array::set_capacity(a, capacity);
}

template <typename T>
inline void
set_capacity(Array<T>* a, usize capacity)
{
	if (static_cast<s64>(capacity) == a->capacity)
		return;

	if (static_cast<s64>(capacity) < a->count)
		array::resize(a, capacity);

	T* data = nullptr;
	if (capacity > 0)
	{
		data = alloc_array<T>(a->allocator, capacity);
		memory::copy_array(a->data, a->count, data);
	}
	free(a->allocator, a->data);
	a->data = data;
	a

	->capacity = capacity;
}

template <typename T>
inline void
grow(Array<T>* a, usize min_capacity)
{
	// TODO(bill): Decide on decent growing formula for Array
	usize capacity = 2 * a->capacity + 8;
	if (capacity < min_capacity)
		capacity = min_capacity;
	set_capacity(a, capacity);
}
} // namespace array







////////////////////////////////
//                            //
// Hash Table                 //
//                            //
////////////////////////////////

namespace hash_table
{
template <typename T>
inline Hash_Table<T>
make(Allocator* a)
{
	Hash_Table<T> result = {};

	result.hashes  = array::make<s64>(a);
	result.entries = array::make<typename Hash_Table<T>::Entry>(a);

	return result;
}

template <typename T>
inline void
free(Hash_Table<T>* h)
{
	if (h->hashes.allocator)
		array::free(&h->hashes);

	if (h->entries.allocator)
		array::free(&h->entries);
}

template <typename T>
inline Hash_Table<T>
copy(Hash_Table<T> const& other, Allocator* allocator)
{
	Allocator* a = other.hashes.allocator;
	if (allocator) a = allocator;

	Hash_Table<T> result = {};
	result.hashes  = array::copy(other.hashes,  a);
	result.entries = array::copy(other.entries, a);

	return result;
}


namespace impl
{
struct Find_Result
{
	s64 hash_index;
	s64 data_prev;
	s64 entry_index;
};

template <typename T> usize add_entry(Hash_Table<T>* h, u64 key);
template <typename T> void  erase(Hash_Table<T>* h, Find_Result const& fr);
template <typename T> Find_Result find_result_from_key(Hash_Table<T> const& h, u64 key);
template <typename T> Find_Result find_result_from_entry(Hash_Table<T> const& h, typename Hash_Table<T>::Entry const* e);
template <typename T> s64  make_entry(Hash_Table<T>* h, u64 key);
template <typename T> void find_and_erase_entry(Hash_Table<T>* h, u64 key);
template <typename T> s64  find_entry_or_fail(Hash_Table<T> const& h, u64 key);
template <typename T> s64  find_or_make_entry(Hash_Table<T>* h, u64 key);
template <typename T> void rehash(Hash_Table<T>* h, usize new_capacity);
template <typename T> void grow(Hash_Table<T>* h);
template <typename T> bool is_full(Hash_Table<T>* h);

template <typename T>
usize
add_entry(Hash_Table<T>* h, u64 key)
{
	typename Hash_Table<T>::Entry e = {};
	e.key  = key;
	e.next = -1;
	usize e_index = h->entries.count;
	array::append(&h->entries, e);

	return e_index;
}

template <typename T>
void
erase(Hash_Table<T>* h, Find_Result const& fr)
{
	if (fr.data_prev < 0)
		h->hashes[fr.hash_index] = h->entries[fr.entry_index].next;
	else
		h->entries[fr.data_prev].next = h->entries[fr.entry_index].next;

	array::pop(&h->entries); // Update array count

	if (fr.entry_index == h->entries.count)
		return;

	h->entries[fr.entry_index] = h->entries[h->entries.count];

	auto last = impl::find_result_from_key(*h, h->entries[fr.entry_index].key);

	if (last.data_prev < 0)
		h->hashes[last.hash_index] = fr.entry_index;
	else
		h->entries[last.entry_index].next = fr.entry_index;
}

template <typename T>
Find_Result
find_result_from_key(Hash_Table<T> const& h, u64 key)
{
	Find_Result fr = {};
	fr.hash_index  = -1;
	fr.data_prev   = -1;
	fr.entry_index = -1;

	if (h.hashes.count == 0)
		return fr;

	fr.hash_index = key % h.hashes.count;
	fr.entry_index = h.hashes[fr.hash_index];
	while (fr.entry_index >= 0)
	{
		if (h.entries[fr.entry_index].key == key)
			return fr;
		fr.data_prev  = fr.entry_index;
		fr.entry_index = h.entries[fr.entry_index].next;
	}

	return fr;
}


template <typename T>
Find_Result
find_result_from_entry(Hash_Table<T> const& h, typename Hash_Table<T>::Entry const* e)
{
	Find_Result fr = {};
	fr.hash_index  = -1;
	fr.data_prev   = -1;
	fr.entry_index = -1;

	if (h.hashes.count == 0 || !e)
		return fr;

	fr.hash_index = e->key % h.hashes.count;
	fr.entry_index = h.hashes[fr.hash_index];
	while (fr.entry_index >= 0)
	{
		if (&h.entries[fr.entry_index] == e)
			return fr;
		fr.data_prev  = fr.entry_index;
		fr.entry_index = h.entries[fr.entry_index].next;
	}

	return fr;
}

template <typename T>
s64
make_entry(Hash_Table<T>* h, u64 key)
{
	Find_Result const fr = impl::find_result_from_key(*h, key);
	s64 const index      = impl::add_entry(h, key);

	if (fr.data_prev < 0)
		h->hashes[fr.hash_index] = index;
	else
		h->entries[fr.data_prev].next = index;

	h->entries[index].next = fr.entry_index;

	return index;
}

template <typename T>
void
find_and_erase_entry(Hash_Table<T>* h, u64 key)
{
	Find_Result const fr = impl::find_result_from_key(*h, key);
	if (fr.entry_index >= 0)
		impl::erase(h, fr);
}

template <typename T>
s64
find_entry_or_fail(Hash_Table<T> const& h, u64 key)
{
	return impl::find_result_from_key(h, key).entry_index;
}

template <typename T>
s64
find_or_make_entry(Hash_Table<T>* h, u64 key)
{
	auto const fr = impl::find_result_from_key(*h, key);
	if (fr.entry_index >= 0)
		return fr.entry_index;

	s64 index = impl::add_entry(h, key);
	if (fr.data_prev < 0)
		h->hashes[fr.hash_index] = index;
	else
		h->entries[fr.data_prev].next = index;

	return index;
}

template <typename T>
void
rehash(Hash_Table<T>* h, usize new_capacity)
{
	auto nh = hash_table::make<T>(h->hashes.allocator);
	array::resize(&nh.hashes, new_capacity);
	array::reserve(&nh.entries, h->entries.count);

	for (usize i = 0; i < new_capacity; i++)
		nh.hashes[i] = -1;

	for (s64 i = 0; i < h->entries.count; i++)
	{
		auto const* e = &h->entries[i];
		multi_hash_table::insert(&nh, e->key, e->value);
	}

	Hash_Table<T> empty_ht{h->hashes.allocator};
	hash_table::free(h);

	memory::copy_struct(&nh,       h);
	memory::copy_struct(&empty_ht, &nh);
}

template <typename T>
inline void
grow(Hash_Table<T>* h)
{
	const usize new_capacity = 2 * h->entries.count + 8;
	impl::rehash(h, new_capacity);
}

template <typename T>
inline bool
is_full(Hash_Table<T>* h)
{
	// Make sure that there is enough space
	f64 const maximum_load_coefficient = 0.75;
	return h->entries.count >= maximum_load_coefficient * h->hashes.count;
}
} // namespace impl

template <typename T>
inline bool
has(Hash_Table<T> const& h, u64 key)
{
	return impl::find_entry_or_fail(h, key) >= 0;
}

template <typename T>
inline T const&
get(Hash_Table<T> const& h, u64 key, T const& default_value)
{
	s64 const index = impl::find_entry_or_fail(h, key);

	if (index < 0)
		return default_value;
	return h.entries[index].value;
}

template <typename T>
inline void
set(Hash_Table<T>* h, u64 key, T const& value)
{
	if (h->hashes.count == 0)
		impl::grow(h);

	s64 const index = impl::find_or_make_entry(h, key);
	h->entries[index].value = value;
	if (impl::is_full(h))
		impl::grow(h);
}

template <typename T>
inline void
set(Hash_Table<T>* h, u64 key, T&& value)
{
	if (h->hashes.count == 0)
		impl::grow(h);

	s64 const index = impl::find_or_make_entry(h, key);
	h->entries[index].value = move_ownership(value);
	if (impl::is_full(h))
		impl::grow(h);
}

template <typename T>
inline void
remove(Hash_Table<T>* h, u64 key)
{
	impl::find_and_erase_entry(h, key);
}

template <typename T>
inline void
reserve(Hash_Table<T>* h, usize capacity)
{
	impl::rehash(h, capacity);
}

template <typename T>
inline void
clear(Hash_Table<T>* h)
{
	array::clear(&h->hashes);
	array::clear(&h->entries);
}
} // namespace hash_table

template <typename T>
inline typename Hash_Table<T>::Entry const*
begin(Hash_Table<T> const& h)
{
	return begin(h.entries);
}

template <typename T>
inline typename Hash_Table<T>::Entry const*
end(Hash_Table<T> const& h)
{
	return end(h.entries);
}


namespace multi_hash_table
{
template <typename T>
inline void
get(Hash_Table<T> const& h, u64 key, Array<T>& items)
{
	auto e = multi_hash_table::find_first(h, key);
	while (e)
	{
		array::append(items, e->value);
		e = multi_hash_table::find_next(h, e);
	}
}

template <typename T>
inline usize
count(Hash_Table<T> const& h, u64 key)
{
	usize count = 0;
	auto e = multi_hash_table::find_first(h, key);
	while (e)
	{
		count++;
		e = multi_hash_table::find_next(h, e);
	}

	return count;
}


template <typename T>
inline typename Hash_Table<T>::Entry const*
find_first(Hash_Table<T> const& h, u64 key)
{
	s64 const index = hash_table::impl::find_entry_or_fail(h, key);
	if (index < 0)
		return nullptr;
	return &h.entries[index];
}

template <typename T>
typename Hash_Table<T>::Entry const*
find_next(Hash_Table<T> const& h, typename Hash_Table<T>::Entry const* e)
{
	if (!e)
		return nullptr;

	auto index = e->next;
	while (index >= 0)
	{
		if (h.entries[index].key == e->key)
			return &h.entries[index];
		index = h.entries[index].next;
	}

	return nullptr;
}


template <typename T>
inline void
insert(Hash_Table<T>* h, u64 key, T const& value)
{
	if (h->hashes.count == 0)
		hash_table::impl::grow(h);

	auto next = hash_table::impl::make_entry(h, key);
	h->entries[next].value = value;

	if (hash_table::impl::is_full(h))
		hash_table::impl::grow(h);
}

template <typename T>
inline void
insert(Hash_Table<T>* h, u64 key, T&& value)
{
	if (h->hashes.count == 0)
		hash_table::impl::grow(h);

	auto next = hash_table::impl::make_entry(h, key);
	h->entries[next].value = move_ownership(value);

	if (hash_table::impl::is_full(h))
		hash_table::impl::grow(h);
}

template <typename T>
inline void
remove_entry(Hash_Table<T>* h, typename Hash_Table<T>::Entry const* e)
{
	auto const fr = hash_table::impl::find_result_from_entry(*h, e);
	if (fr.entry_index >= 0)
		hash_table::impl::erase(h, fr);
}

template <typename T>
inline void
remove_all(Hash_Table<T>* h, u64 key)
{
	while (hash_table::has(*h, key))
		hash_table::remove(h, key);
}
} // namespace multi_hash_table


namespace memory
{
template <typename T>
inline void
fill(T* ptr, usize count, T const& value)
{
	for (usize i = 0; i < count; i++)
		ptr[i] = value;
}

template <typename T>
inline void
fill(T* ptr, usize count, T&& value)
{
	for (usize i = 0; i < count; i++)
		ptr[i] = move_ownership(value);
}

template <typename T>
inline void
zero_struct(T* ptr)
{
	memory::zero(ptr, sizeof(T));
}

template <typename T>
inline void
zero_array(T* ptr, usize count)
{
	memory::zero(ptr, count * sizeof(T));
}

template <typename T>
inline void
copy_array(T const* src_array, usize count, T* dest_array)
{
	memory::copy(src_array, count * sizeof(T), dest_array);
}

template <typename T>
inline void
copy_struct(T const* src_array, T* dest_array)
{
	memory::copy(src_array, sizeof(T), dest_array);
}


template <typename T>
inline void
swap(T* a, T* b)
{
	T c = move_ownership(*a);
	*a  = move_ownership(*b);
	*b  = move_ownership(c);
}

template <typename T, usize N>
inline void
swap(T (& a)[N], T (& b)[N])
{
	for (usize i = 0; i < N; i++)
		math::swap(&a[i], &b[i]);
}
} // namespace memory



////////////////////////////////
//                            //
// Sort                       //
//                            //
////////////////////////////////

namespace sort
{
template <typename T, typename Comparison_Function>
void
quick(T* array, usize count, Comparison_Function compare)
{
	if (count < 2) return;

	T const& mid = array[count/2];

	s64 i = 0;
	s64 j = count-1;

	while (true)
	{
		while (compare(array[i], mid) < 0) i++;
		while (compare(mid, array[j]) < 0) j--;

		if (i >= j) break;

		memory::swap(&array[i], &array[j]);

		i++;
		j--;
	}

	sort::quick(array,   i,       compare);
	sort::quick(array+i, count-i, compare);
}
} // namespace sort



__GB_NAMESPACE_END

#endif // GB_INCLUDE_GB_HPP

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
// It's turtles all the way down!
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

#if defined(GB_SYSTEM_WINDOWS)

	#if !defined(GB_NO_STDIO)
		#include <dbghelp.h>
		#pragma comment(lib, "dbghelp.lib") // TODO(bill): Should this be pragma included or not?

		internal_linkage void
		gb__print_call_stack(FILE* out_stream)
		{
			SymInitialize(GetCurrentProcess(), nullptr, true);
			SymSetOptions(SYMOPT_LOAD_LINES | SYMOPT_UNDNAME);

			DWORD mtype = {};
			CONTEXT ctx = {};
			ctx.ContextFlags = CONTEXT_CONTROL;

			RtlCaptureContext(&ctx);

			STACKFRAME64 stack = {};

		#if defined(_M_IX86)
			mtype = IMAGE_FILE_MACHINE_I386;
			stack.AddrPC.Offset    = ctx.Eip;
			stack.AddrPC.Mode      = AddrModeFlat;
			stack.AddrFrame.Offset = ctx.Ebp;
			stack.AddrFrame.Mode   = AddrModeFlat;
			stack.AddrStack.Offset = ctx.Esp;
			stack.AddrStack.Mode   = AddrModeFlat;
		#elif defined(_M_X64)
			mtype = IMAGE_FILE_MACHINE_AMD64;
			stack.AddrPC.Offset    = ctx.Rip;
			stack.AddrPC.Mode      = AddrModeFlat;
			stack.AddrFrame.Offset = ctx.Rsp;
			stack.AddrFrame.Mode   = AddrModeFlat;
			stack.AddrStack.Offset = ctx.Rsp;
			stack.AddrStack.Mode   = AddrModeFlat;
		#else
			#error Unknown Windows Platform
		#endif

			DWORD ldsp = 0;
			IMAGEHLP_LINE64 line = {};
			line.SizeOfStruct = sizeof(IMAGEHLP_LINE64);

			char buf[sizeof(SYMBOL_INFO) + (MAX_SYM_NAME * sizeof(TCHAR))];

			SYMBOL_INFO* sym  = reinterpret_cast<SYMBOL_INFO*>(buf);
			sym->SizeOfStruct = sizeof(SYMBOL_INFO);
			sym->MaxNameLen   = MAX_SYM_NAME;

			UINT layer_count = 0;
			while (StackWalk64(mtype,
			                   GetCurrentProcess(), GetCurrentThread(),
			                   &stack, &ctx, nullptr,
			                   SymFunctionTableAccess64, SymGetModuleBase64, nullptr))
			{
				if (stack.AddrPC.Offset == 0)
					break;

				BOOL result = SymGetLineFromAddr64(GetCurrentProcess(), stack.AddrPC.Offset, &ldsp, &line);
				result = result && SymFromAddr(GetCurrentProcess(), stack.AddrPC.Offset, 0, sym);

				if (result)
				{
					fprintf(out_stream,
					        "\t[%u] `%s` (%s:%d)\n",
					        layer_count, sym->Name, line.FileName, line.LineNumber);
				}
				else
				{
					fprintf(out_stream,
					        "\t[%u] 0x%p\n",
					        layer_count, stack.AddrPC.Offset);
				}

				layer_count++;
			}

			SymCleanup(GetCurrentProcess());
		}
	#endif
#else
	#error gb__print_call_stack() not implemeneted
	// TODO(bill): Implemenet gb__print_call_stack()
#endif

// Helper function used as a better alternative to assert which allows for
// optional printf style error messages
inline void
gb__assert_handler(bool condition, char const* condition_str,
				   char const* filename, size_t line,
				   char const* error_text, ...)
{
	if (condition)
		return;

#if !defined(GB_NO_STDIO)
	FILE* out_stream = stderr;

	fprintf(out_stream, "ASSERT! %s(%lu): %s", filename, line, condition_str);
	if (error_text)
	{
		fprintf(out_stream, " - ");

		va_list args;
		va_start(args, error_text);
		vfprintf(out_stream, error_text, args);
		va_end(args);
	}
	fprintf(out_stream, "\n");

	fprintf(out_stream, "Stacktrack:\n");
	gb__print_call_stack(out_stream);
#endif

	// TODO(bill): Are these decent breaking functions???
#if defined(GB_COMPILER_MSVC)
	__debugbreak();
#elif defined(GB_COMPILER_GNU_GCC)
	__builtin_trap();
#else
	#error Implement aborting function
#endif
}



__GB_NAMESPACE_START

////////////////////////////////
//                            //
// Memory                     //
//                            //
////////////////////////////////

namespace mutex
{
inline Mutex
make()
{
	Mutex m = {};
#if defined(GB_SYSTEM_WINDOWS)
	m.win32_mutex = CreateMutex(0, false, 0);
#else
	pthread_mutex_init(&m.posix_mutex, nullptr);
#endif
	return m;
}

inline void
destroy(Mutex* m)
{
#if defined(GB_SYSTEM_WINDOWS)
	CloseHandle(m->win32_mutex);
#else
	pthread_mutex_destroy(&m->posix_mutex);
#endif
}


inline void
lock(Mutex* m)
{
#if defined(GB_SYSTEM_WINDOWS)
	WaitForSingleObject(m->win32_mutex, INFINITE);
#else
	pthread_mutex_lock(&m->posix_mutex);
#endif
}

inline bool
try_lock(Mutex* m)
{
#if defined(GB_SYSTEM_WINDOWS)
	return WaitForSingleObject(m->win32_mutex, 0) == WAIT_OBJECT_0;
#else
	return pthread_mutex_trylock(&m->posix_mutex) == 0;
#endif
}


inline void
unlock(Mutex* m)
{
#if defined(GB_SYSTEM_WINDOWS)
	ReleaseMutex(m->win32_mutex);
#else
	pthread_mutex_unlock(&m->posix_mutex);
#endif
}
} // namespace mutex





// Atomics
namespace atomic
{
#if defined(_MSC_VER)
inline u32
load(Atomic32 const volatile* object)
{
	return object->nonatomic;
}

inline void
store(Atomic32 volatile* object, u32 value)
{
	object->nonatomic = value;
}

inline u32
compare_exchange_strong(Atomic32 volatile* object, u32 expected, u32 desired)
{
	return _InterlockedCompareExchange(reinterpret_cast<long volatile*>(object), desired, expected);
}

inline u32
exchanged(Atomic32 volatile* object, u32 desired)
{
	return _InterlockedExchange(reinterpret_cast<long volatile*>(object), desired);
}

inline u32
fetch_add(Atomic32 volatile* object, s32 operand)
{
	return _InterlockedExchangeAdd(reinterpret_cast<long volatile*>(object), operand);
}

inline u32
fetch_and(Atomic32 volatile* object, u32 operand)
{
	return _InterlockedAnd(reinterpret_cast<long volatile*>(object), operand);
}

inline u32
fetch_or_32(Atomic32 volatile* object, u32 operand)
{
	return _InterlockedOr(reinterpret_cast<long volatile*>(object), operand);
}

inline u64
load(Atomic64 const volatile* object)
{
#if defined(GB_ARCH_64_BIT)
	return object->nonatomic;
#else
	// NOTE(bill): The most compatible way to get an atomic 64-bit load on x86 is with cmpxchg8b
	u64 result;
	__asm
	{
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

inline void
store(Atomic64 volatile* object, u64 value)
{
#if defined(GB_ARCH_64_BIT)
	object->nonatomic = value;
#else
	// NOTE(bill): The most compatible way to get an atomic 64-bit store on x86 is with cmpxchg8b
	__asm
	{
		mov esi, object;
		mov ebx, dword ptr value;
		mov ecx, dword ptr value[4];
	retry:
		cmpxchg8b [esi];
		jne retry;
	}
#endif
}

inline u64
compare_exchange_strong(Atomic64 volatile* object, u64 expected, u64 desired)
{
	return _InterlockedCompareExchange64(reinterpret_cast<s64 volatile*>(object), desired, expected);
}

inline u64
exchanged(Atomic64 volatile* object, u64 desired)
{
#if defined(GB_ARCH_64_BIT)
	return _InterlockedExchange64(reinterpret_cast<s64 volatile*>(object), desired);
#else
	u64 expected = object->nonatomic;
	while (true)
	{
		u64 original = _InterlockedCompareExchange64(reinterpret_cast<s64 volatile*>(object), desired, expected);
		if (original == expected)
			return original;
		expected = original;
	}
#endif
}

inline u64
fetch_add(Atomic64 volatile* object, s64 operand)
{
#if defined(GB_ARCH_64_BIT)
	return _InterlockedExchangeAdd64(reinterpret_cast<s64 volatile*>(object), operand);
#else
	u64 expected = object->nonatomic;
	while (true)
	{
		u64 original = _InterlockedExchange64(reinterpret_cast<s64 volatile*>(object), expected + operand, expected);
		if (original == expected)
			return original;
		expected = original;
	}
#endif
}

inline u64
fetch_and(Atomic64 volatile* object, u64 operand)
{
#if defined(GB_ARCH_64_BIT)
	return _InterlockedAnd64(reinterpret_cast<s64 volatile*>(object), operand);
#else
	u64 expected = object->nonatomic;
	while (true)
	{
		u64 original = _InterlockedCompareExchange64(reinterpret_cast<s64 volatile*>(object), expected & operand, expected);
		if (original == expected)
			return original;
		expected = original;
	}
#endif
}

inline u64
fetch_or(Atomic64 volatile* object, u64 operand)
{
#if defined(GB_ARCH_64_BIT)
	return _InterlockedAnd64(reinterpret_cast<s64 volatile*>(object), operand);
#else
	u64 expected = object->nonatomic;
	while (true)
	{
		u64 original = _InterlockedCompareExchange64(reinterpret_cast<s64 volatile*>(object), expected | operand, expected);
		if (original == expected)
			return original;
		expected = original;
	}
#endif
}

#else
#error TODO(bill): Implement atomics for this platform
#endif
} // namespace atomic




namespace semaphore
{
Semaphore
make()
{
	Semaphore semaphore = {};
#if defined(GB_SYSTEM_WINDOWS)
	semaphore.win32_handle = CreateSemaphore(nullptr, 0, GB_S32_MAX, nullptr);

	GB_ASSERT(semaphore.win32_handle != nullptr, "CreateSemaphore: GetLastError = %d", GetLastError());

#else
	semaphore.count = 0;
	s32 result = pthread_cond_init(&semaphore.cond, nullptr);
	GB_ASSERT(result == 0, "pthread_cond_init: errno = %d", result);

	semaphore.mutex = mutex::make();
#endif

	return semaphore;
}

void
destroy(Semaphore* semaphore)
{
#if defined(GB_SYSTEM_WINDOWS)
	BOOL err = CloseHandle(semaphore->win32_handle);
	GB_ASSERT(err != 0, "CloseHandle: GetLastError = %d", GetLastError());
#else
	s32 result = pthread_cond_destroy(&semaphore->cond);
	GB_ASSERT(result == 0, "pthread_cond_destroy: errno = %d", result);
	mutex::destroy(&semaphore->mutex);
#endif
}

void
post(Semaphore* semaphore, u32 count)
{
#if defined(GB_SYSTEM_WINDOWS)
	BOOL err = ReleaseSemaphore(semaphore->win32_handle, count, nullptr);
	GB_ASSERT(err != 0, "ReleaseSemaphore: GetLastError = %d", GetLastError());
#else
	mutex::lock(&semaphore->mutex);

	for (u32 i = 0; i < count; i++)
	{
		s32 result = pthread_cond_signal(&semaphore->cond);
		GB_ASSERT(result == 0, "pthread_cond_signal: errno = %d", result);
	}

	semaphore->count += count;

	mutex::unlock(&semaphore->mutex);
#endif
}

void
wait(Semaphore* semaphore)
{
#if defined(GB_SYSTEM_WINDOWS)
	DWORD result = WaitForSingleObject(semaphore->win32_handle, INFINITE);
	GB_ASSERT(result == WAIT_OBJECT_0, "WaitForSingleObject: GetLastError = %d", GetLastError());
#else
	mutex::lock(&semaphore->mutex);

	while (count <= 0)
	{
		s32 result = pthread_cond_wait(&semaphore->cond, &semaphore->mutex.posix_mutex);
		GB_ASSERT(result == 0, "pthread_cond_wait: errno = %d", result);
	}

	count--;

	mutex::unlock(&semaphore->mutex);
#endif
}
} // namespace semaphore

namespace thread
{
Thread
make()
{
	Thread t = {};
#if defined(GB_SYSTEM_WINDOWS)
	t.win32_handle = INVALID_HANDLE_VALUE;
#else
	t.posix_handle = 0;
#endif
	t.function   = nullptr;
	t.data       = nullptr;
	t.stack_size = 0;
	t.is_running = false;
	t.semaphore  = semaphore::make();

	return t;
}

void
destroy(Thread* t)
{
	if (t->is_running)
		thread::join(t);

	semaphore::destroy(&t->semaphore);
}

internal_linkage void
run(Thread* t)
{
	semaphore::post(&t->semaphore);
	t->function(t->data);
}

#if defined(GB_SYSTEM_WINDOWS)
internal_linkage DWORD WINAPI
thread_proc(void* arg)
{
	thread::run(static_cast<Thread*>(arg));
	return 0;
}

#else
internal_linkage void*
thread_proc(void* arg)
{
	thread::run(static_cast<Thread*>(arg));
	return nullptr;
}
#endif

void
start(Thread* t, Thread_Procedure* func, void* data, usize stack_size)
{
	GB_ASSERT(!t->is_running);
	GB_ASSERT(func != nullptr);
	t->function = func;
	t->data = data;
	t->stack_size = stack_size;

#if defined(GB_SYSTEM_WINDOWS)
	t->win32_handle = CreateThread(nullptr, stack_size, thread_proc, t, 0, nullptr);
	GB_ASSERT(t->win32_handle != nullptr,
	          "CreateThread: GetLastError = %d", GetLastError());

#else
	pthread_attr_t attr;
	s32 result = pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
	GB_ASSERT(result == 0, "pthread_attr_init: errno = %d", result);

	if (t->stack_size != 0)
	{
		result = pthread_attr_setstacksize(&attr, t->stack_size);
		GB_ASSERT(result == 0, "pthread_attr_setstacksize: errno = %d", result);
	}

	result = pthread_create(&t->posix_handle, &attr, thread_proc, thread);
	GB_ASSERT(result == 0, "pthread_create: errno = %d", result);

	// NOTE(bill): Free attr memory
	result = pthread_attr_destroy(&attr);
	GB_ASSERT(result == 0, "pthread_attr_destroy: errno = %d", result);

	// NOTE(bill): So much boiler patch compared to windows.h (for once)
#endif

	t->is_running = true;
	semaphore::wait(&t->semaphore);
}

void
join(Thread* t)
{
	if (!t->is_running) return;

#if defined(GB_SYSTEM_WINDOWS)
	WaitForSingleObject(t->win32_handle, INFINITE);
	CloseHandle(t->win32_handle);
	t->win32_handle = INVALID_HANDLE_VALUE;
#else
	int result = pthread_join(t->posix_handle, nullptr);
	t->posix_handle = 0;
#endif

	t->is_running = false;
}

inline bool
is_running(Thread const& thread)
{
	return thread.is_running != 0;
}

inline u32
current_id()
{
	u32 thread_id;

#if defined(GB_SYSTEM_WINDOWS)
	u8* thread_local_storage = reinterpret_cast<u8*>(__readgsqword(0x30));
	thread_id = *reinterpret_cast<u32*>(thread_local_storage + 0x48);

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


} // namespace thread


namespace heap
{
namespace functions
{
internal_linkage void*
alloc(Allocator* a, usize size, usize align)
{
	Heap* heap = reinterpret_cast<Heap*>(a);

	if (heap->use_mutex) mutex::lock(&heap->mutex);

	usize total = size + align - (size % align);

#if defined (GB_SYSTEM_WINDOWS)
	total += sizeof(Heap::Header);

	void* data = HeapAlloc(heap->win32_heap_handle, 0, total);

	Heap::Header* h = static_cast<Heap::Header*>(data);
	h->size = total;
	data = (h + 1);

#else
	// TODO(bill): Find a better malloc alternative for this platform
	void* data = malloc(total);
#endif

	heap->total_allocated_count += total;
	heap->allocation_count++;

	if (heap->use_mutex) mutex::unlock(&heap->mutex);

	return data;
}

internal_linkage void
free(Allocator* a, void* ptr)
{
	if (!ptr)
		return;

	Heap* heap = reinterpret_cast<Heap*>(a);

	if (heap->use_mutex) mutex::lock(&heap->mutex);

	heap->total_allocated_count -= allocated_size(heap, ptr);
	heap->allocation_count--;

#if defined (GB_SYSTEM_WINDOWS)
	auto* header = static_cast<Heap::Header*>(ptr) - 1;
	HeapFree(heap->win32_heap_handle, 0, header);
#else
	::free(ptr);
#endif

	if (heap->use_mutex) mutex::unlock(&heap->mutex);
}

inline s64
allocated_size(Allocator* a, void const* ptr)
{
#if defined(GB_SYSTEM_WINDOWS)
	auto* heap = reinterpret_cast<Heap*>(a);

	if (heap->use_mutex) mutex::lock(&heap->mutex);

	auto const* h = static_cast<Heap::Header const*>(ptr) - 1;
	s64 result = h->size;

	if (heap->use_mutex) mutex::unlock(&heap->mutex);

	return static_cast<s64>(result);

#elif defined(GB_SYSTEM_OSX)
	return static_cast<s64>(malloc_size(ptr));

#elif defined(GB_SYSTEM_LINUX)
	return static_cast<s64>(malloc_usable_size(ptr));

#else
	#error Implement Heap::allocated_size
#endif
}

inline s64
total_allocated(Allocator* a)
{
	auto* heap = reinterpret_cast<Heap*>(a);

	if (heap->use_mutex) mutex::lock(&heap->mutex);

	s64 result = heap->total_allocated_count;

	if (heap->use_mutex) mutex::unlock(&heap->mutex);

	return result;
}
} // namespace functions

Heap
make(bool use_mutex)
{
	Heap heap = {};

	heap.use_mutex = use_mutex;
	if (use_mutex) heap.mutex = mutex::make();

#if defined(GB_SYSTEM_WINDOWS)
	heap.win32_heap_handle = HeapCreate(0, 0, 0);
#endif

	heap.alloc           = functions::alloc;
	heap.free            = functions::free;
	heap.allocated_size  = functions::allocated_size;
	heap.total_allocated = functions::total_allocated;

	return heap;
}
void
destroy(Heap* heap)
{
	if (heap->use_mutex) mutex::destroy(&heap->mutex);

#if defined (GB_SYSTEM_WINDOWS)
	HeapDestroy(heap->win32_heap_handle);
#endif
}
} // namespace heap


namespace arena
{
namespace functions
{
internal_linkage void*
alloc(Allocator* a, usize size, usize align)
{
	Arena* arena = reinterpret_cast<Arena*>(a);

	s64 actual_size = size + align;

	if (arena->total_allocated_count + actual_size > arena->total_size)
	{
		GB_ASSERT(arena->total_allocated_count + actual_size <= arena->total_size,
		          "Arena has no more space for allocation");
		return nullptr;
	}

	void* ptr = memory::align_forward(memory::pointer_add(arena->physical_start, arena->total_allocated_count), align);

	arena->total_allocated_count += actual_size;

	return ptr;
}

inline void free(Allocator*, void*) {} // NOTE(bill): Arenas free all at once

inline s64 allocated_size(Allocator*, void const*) { return -1; }

inline s64
total_allocated(Allocator* a)
{
	return reinterpret_cast<Arena*>(a)->total_allocated_count;
}
} // namespace functions

Arena
make(Allocator* backing, usize size)
{
	Arena arena = {};

	arena.backing = backing;
	arena.physical_start = nullptr;
	arena.total_size = size;
	arena.temp_count = 0;
	arena.total_allocated_count = 0;

	arena.physical_start = alloc(arena.backing, size);

	arena.alloc           = functions::alloc;
	arena.free            = functions::free;
	arena.allocated_size  = functions::allocated_size;
	arena.total_allocated = functions::total_allocated;

	return arena;
}

Arena
make(void* start, usize size)
{
	Arena arena = {};

	arena.backing = nullptr;
	arena.physical_start = start;
	arena.total_size = size;
	arena.temp_count = 0;
	arena.total_allocated_count = 0;

	arena.alloc           = functions::alloc;
	arena.free            = functions::free;
	arena.allocated_size  = functions::allocated_size;
	arena.total_allocated = functions::total_allocated;

	return arena;
}

void
destroy(Arena* arena)
{
	if (arena->backing)
		free(arena->backing, arena->physical_start);

	clear(arena);
}

inline void
clear(Arena* arena)
{
	GB_ASSERT(arena->temp_count == 0,
			  "%ld Temporary_Arena_Memory have not be cleared", arena->temp_count);

	arena->total_allocated_count = 0;
}
} // namespace arena


namespace temporary_arena_memory
{
inline Temporary_Arena_Memory
make(Arena* arena)
{
	Temporary_Arena_Memory tmp = {};
	tmp.arena = arena;
	tmp.original_count = arena->total_allocated_count;
	arena->temp_count++;
	return tmp;
}

inline void
free(Temporary_Arena_Memory tmp)
{
	GB_ASSERT(total_allocated(tmp.arena) >= tmp.original_count);
	tmp.arena->total_allocated_count = tmp.original_count;
	GB_ASSERT(tmp.arena->temp_count > 0);
	tmp.arena->temp_count--;
}
} // namespace temporary_arena_memory




namespace pool
{
namespace functions
{
internal_linkage void*
alloc(Allocator* a, usize size, usize align)
{
	Pool* pool = reinterpret_cast<Pool*>(a);

	GB_ASSERT(size  == pool->block_size,  "Size must match block size");
	GB_ASSERT(align == pool->block_align, "Align must match block align");
	GB_ASSERT(pool->free_list != nullptr, "Pool out of memory");

	uintptr next_free = *reinterpret_cast<uintptr*>(pool->free_list);
	void* ptr = pool->free_list;
	pool->free_list = reinterpret_cast<void*>(next_free);

	pool->total_size += pool->block_size;

	return ptr;
}

internal_linkage void
free(Allocator* a, void* ptr)
{
	if (!ptr) return;

	Pool* pool = reinterpret_cast<Pool*>(a);

	uintptr* next = static_cast<uintptr*>(ptr);
	*next = reinterpret_cast<uintptr>(pool->free_list);

	pool->free_list = ptr;

	pool->total_size -= pool->block_size;
}

internal_linkage s64
allocated_size(Allocator*, void const*)
{
	return -1;
}

internal_linkage s64
total_allocated(Allocator* a)
{
	Pool* pool = reinterpret_cast<Pool*>(a);
	return pool->total_size;
}
} // namespace functions


Pool
make(Allocator* backing, usize num_blocks, usize block_size, usize block_align)
{
	Pool pool = {};

	pool.backing     = backing;
	pool.block_size  = block_size;
	pool.block_align = block_align;

	usize actual_block_size = block_size + block_align;
	usize pool_size = num_blocks * actual_block_size;

	u8* data = static_cast<u8*>(alloc(backing, pool_size, block_align));


	// Init intrusive freelist
	u8* curr = data;
	for (usize block_index = 0; block_index < num_blocks-1; block_index++)
	{
		uintptr* next = reinterpret_cast<uintptr*>(curr);
		*next = reinterpret_cast<uintptr>(curr) + actual_block_size;
		curr += actual_block_size;
	}

	uintptr* end = reinterpret_cast<uintptr*>(curr);
	*end = reinterpret_cast<uintptr>(nullptr);

	pool.physical_start = data;
	pool.free_list      = data;

	// Set functions pointers
	pool.alloc           = functions::alloc;
	pool.free            = functions::free;
	pool.allocated_size  = functions::allocated_size;
	pool.total_allocated = functions::total_allocated;

	return pool;
}

inline void
destroy(Pool* pool)
{
	free(pool->backing, pool->physical_start);
}
} // namespace pool






////////////////////////////////
//                            //
// Memory                     //
//                            //
////////////////////////////////

namespace memory
{
inline void*
align_forward(void* ptr, usize align)
{
	GB_ASSERT(GB_IS_POWER_OF_TWO(align),
	          "Alignment must be a power of two and not zero -- %llu", align);

	uintptr p = uintptr(ptr);
	const usize modulo = p % align;
	if (modulo) p += (align - modulo);
	return reinterpret_cast<void*>(p);
}

inline void*
pointer_add(void* ptr, usize bytes)
{
	return static_cast<void*>(static_cast<u8*>(ptr) + bytes);
}

inline void const*
pointer_add(void const* ptr, usize bytes)
{
	return static_cast<void const*>(static_cast<u8 const*>(ptr) + bytes);
}

inline void*
pointer_sub(void* ptr, usize bytes)
{
	return static_cast<void*>(static_cast<u8*>(ptr) - bytes);
}

inline void const*
pointer_sub(void const* ptr, usize bytes)
{
	return static_cast<void const*>(static_cast<u8 const*>(ptr) - bytes);
}


GB_FORCE_INLINE void
zero(void* ptr, usize bytes)
{
	memset(ptr, 0, bytes);
}

GB_FORCE_INLINE void
copy(void const* src, usize bytes, void* dest)
{
	memcpy(dest, src, bytes);
}

GB_FORCE_INLINE void
move(void const* src, usize bytes, void* dest)
{
	memmove(dest, src, bytes);
}

GB_FORCE_INLINE bool
equals(void const* a, void const* b, usize bytes)
{
	return (memcmp(a, b, bytes) == 0);
}
} // namespace memory

inline void*
alloc(Allocator* a, usize size, usize align)
{
	GB_ASSERT(a != nullptr);
	return a->alloc(a, size, align);
}

inline void
free(Allocator* a, void* ptr)
{
	GB_ASSERT(a != nullptr);
	if (ptr) a->free(a, ptr);
}

inline s64
allocated_size(Allocator* a, void const* ptr)
{
	GB_ASSERT(a != nullptr);
	return a->allocated_size(a, ptr);
}

inline s64
total_allocated(Allocator* a)
{
	GB_ASSERT(a != nullptr);
	return a->total_allocated(a);
}

////////////////////////////////
//                            //
// String                     //
//                            //
////////////////////////////////

namespace string
{
inline String
make(Allocator* a, char const* str)
{
	return string::make(a, str, (string::Size)strlen(str));
}

String
make(Allocator* a, void const* init_str, Size len)
{
	usize header_size = sizeof(string::Header);
	void* ptr = alloc(a, header_size + len + 1);
	if (!ptr) return nullptr;

	if (!init_str) memory::zero(ptr, header_size + len + 1);

	String str = static_cast<char*>(ptr) + header_size;

	string::Header* header = string::header(str);
	header->allocator = a;
	header->length    = len;
	header->capacity  = len;

	if (len && init_str)
		memory::copy(init_str, len, str);
	str[len] = '\0';

	return str;
}

inline void
free(String str)
{
	if (str == nullptr) return;

	string::Header* h = string::header(str);

	if (h->allocator) free(h->allocator, h);
}

inline String
duplicate(Allocator* a, String const str)
{
	return string::make(a, str, string::length(str));
}

inline Size
length(String const str)
{
	return string::header(str)->length;
}

inline Size
capacity(String const str)
{
	return string::header(str)->capacity;
}

inline Size
available_space(String const str)
{
	string::Header* h = string::header(str);
	if (h->capacity > h->length)
		return h->capacity - h->length;
	return 0;
}

inline void
clear(String str)
{
	string::header(str)->length = 0;
	str[0] = '\0';
}

void
append(String* str, char c)
{
	Size curr_len = string::length(*str);

	string::make_space_for(str, 1);
	if (str == nullptr) return;

	(*str)[curr_len]     = c;
	(*str)[curr_len + 1] = '\0';
	string::header(*str)->length = curr_len + 1;
}

inline void
append(String* str, String const other)
{
	string::append(str, other, string::length(other));
}

inline void
append_cstring(String* str, char const* other)
{
	string::append(str, other, (Size)strlen(other));
}

void
append(String* str, void const* other, Size other_len)
{
	Size curr_len = string::length(*str);

	string::make_space_for(str, other_len);
	if (str == nullptr)
		return;

	memory::copy(other, other_len, (*str) + curr_len);
	(*str)[curr_len + other_len] = '\0';
	string::header(*str)->length = curr_len + other_len;
}

namespace impl
{
// NOTE(bill): ptr _must_ be allocated with Allocator* a
internal_linkage inline void*
string_realloc(Allocator* a, void* ptr, usize old_size, usize new_size)
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

	memory::copy(ptr, old_size, new_ptr);

	free(a, ptr);

	return new_ptr;
}
} // namespace impl

void
make_space_for(String* str, Size add_len)
{
	Size len = string::length(*str);
	Size new_len = len + add_len;

	Size available = string::available_space(*str);
	if (available >= add_len) // Return if there is enough space left
		return;

	void* ptr = string::header(*str);
	usize old_size = sizeof(string::Header) + string::length(*str) + 1;
	usize new_size = sizeof(string::Header) + new_len + 1;

	Allocator* a = string::header(*str)->allocator;
	void* new_ptr = impl::string_realloc(a, ptr, old_size, new_size);
	if (new_ptr == nullptr)
		return;

	string::Header* header = static_cast<string::Header*>(new_ptr);
	header->allocator = a;
	header->length    = len;
	header->capacity  = new_len;

	*str = reinterpret_cast<String>(header + 1);
}

usize
allocation_size(String const str)
{
	Size cap = string::capacity(str);
	return sizeof(string::Header) + cap;
}

bool
equals(String const lhs, String const rhs)
{
	Size lhs_len = string::length(lhs);
	Size rhs_len = string::length(rhs);
	if (lhs_len != rhs_len)
		return false;

	for (Size i = 0; i < lhs_len; i++)
	{
		if (lhs[i] != rhs[i])
			return false;
	}

	return true;
}

int
compare(String const lhs, String const rhs) // NOTE(bill): three-way comparison
{
	// Treat as cstring
	char const* str1 = lhs;
	char const* str2 = rhs;
	int s1;
	int s2;
	do
	{
		s1 = *str1++;
		s2 = *str2++;
		if (s1 == 0)
			break;
	}
	while (s1 == s2);

	return (s1 < s2) ? -1 : (s1 > s2);
}

void
trim(String* str, char const* cut_set)
{
	char* start;
	char* end;
	char* start_pos;
	char* end_pos;

	start_pos = start = *str;
	end_pos   = end   = *str + string::length(*str) - 1;

	while (start_pos <= end && strchr(cut_set, *start_pos))
		start_pos++;
	while (end_pos > start_pos && strchr(cut_set, *end_pos))
		end_pos--;

	Size len = static_cast<Size>((start_pos > end_pos) ? 0 : ((end_pos - start_pos)+1));

	if (*str != start_pos)
		memory::move(start_pos, len, *str);
	(*str)[len] = '\0';

	string::header(*str)->length = len;
}
inline void
trim_space(String* str)
{
	trim(str, " \n\r\t\v\f");
}
} // namespace string









////////////////////////////////
//                            //
// Hash                       //
//                            //
////////////////////////////////

namespace hash
{
u32
adler32(void const* key, u32 num_bytes)
{
	const u32 MOD_ADLER = 65521;

	u32 a = 1;
	u32 b = 0;

	u8 const* bytes = static_cast<u8 const*>(key);
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
crc32(void const* key, u32 num_bytes)
{
	u32 result = static_cast<u32>(~0);
	u8 const* c = reinterpret_cast<u8 const*>(key);

	for (u32 remaining = num_bytes; remaining--; c++)
		result = (result >> 8) ^ (GB_CRC32_TABLE[(result ^ *c) & 0xff]);

	return ~result;
}

u64
crc64(void const* key, usize num_bytes)
{
	u64 result = static_cast<u64>(~0);
	u8 const* c = reinterpret_cast<u8 const*>(key);
	for (usize remaining = num_bytes; remaining--; c++)
		result = (result >> 8) ^ (GB_CRC64_TABLE[(result ^ *c) & 0xff]);

	return ~result;
}

inline u32
fnv32(void const* key, usize num_bytes)
{
	u32 h = 0x811c9dc5;
	u8 const* buffer = static_cast<u8 const*>(key);

	for (usize i = 0; i < num_bytes; i++)
	{
		h = (h * 0x01000193) ^ buffer[i];
	}

	return h;
}

inline u64
fnv64(void const* key, usize num_bytes)
{
	u64 h = 0xcbf29ce484222325ull;
	u8 const* buffer = static_cast<u8 const*>(key);

	for (usize i = 0; i < num_bytes; i++)
	{
		h = (h * 0x100000001b3ll) ^ buffer[i];
	}

	return h;
}

inline u32
fnv32a(void const* key, usize num_bytes)
{
	u32 h = 0x811c9dc5;
	u8 const* buffer = static_cast<u8 const*>(key);

	for (usize i = 0; i < num_bytes; i++)
	{
		h = (h ^ buffer[i]) * 0x01000193;
	}

	return h;
}

inline u64
fnv64a(void const* key, usize num_bytes)
{
	u64 h = 0xcbf29ce484222325ull;
	u8 const* buffer = static_cast<u8 const*>(key);

	for (usize i = 0; i < num_bytes; i++)
	{
		h = (h ^ buffer[i]) * 0x100000001b3ll;
	}

	return h;
}

u32
murmur32(void const* key, u32 num_bytes, u32 seed)
{
	const u32 c1 = 0xcc9e2d51;
	const u32 c2 = 0x1b873593;
	const u32 r1 = 15;
	const u32 r2 = 13;
	const u32 m = 5;
	const u32 n = 0xe6546b64;

	u32 hash = seed;

	const usize nblocks = num_bytes / 4;
	const u32* blocks = static_cast<const u32*>(key);
	for (usize i = 0; i < nblocks; i++) {
		u32 k = blocks[i];
		k *= c1;
		k = (k << r1) | (k >> (32 - r1));
		k *= c2;

		hash ^= k;
		hash = ((hash << r2) | (hash >> (32 - r2))) * m + n;
	}

	u8 const* tail = (static_cast<u8 const*>(key)) + nblocks * 4;
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

#if defined(GB_ARCH_64_BIT)
	u64
	murmur64(void const* key, usize num_bytes, u64 seed)
	{
		const u64 m = 0xc6a4a7935bd1e995ULL;
		const s32 r = 47;

		u64 h = seed ^ (num_bytes * m);

		const u64* data = static_cast<const u64*>(key);
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

		u8 const* data2 = reinterpret_cast<u8 const*>(data);

		switch (num_bytes & 7)
		{
		case 7: h ^= static_cast<u64>(data2[6]) << 48;
		case 6: h ^= static_cast<u64>(data2[5]) << 40;
		case 5: h ^= static_cast<u64>(data2[4]) << 32;
		case 4: h ^= static_cast<u64>(data2[3]) << 24;
		case 3: h ^= static_cast<u64>(data2[2]) << 16;
		case 2: h ^= static_cast<u64>(data2[1]) << 8;
		case 1: h ^= static_cast<u64>(data2[0]);
			h *= m;
		};

		h ^= h >> r;
		h *= m;
		h ^= h >> r;

		return h;
	}
#elif GB_ARCH_32_BIT
	u64
	murmur64(void const* key, usize num_bytes, u64 seed)
	{
		const u32 m = 0x5bd1e995;
		const s32 r = 24;

		u32 h1 = static_cast<u32>(seed) ^ static_cast<u32>(num_bytes);
		u32 h2 = static_cast<u32>(seed >> 32);

		const u32* data = static_cast<const u32*>(key);

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
		case 3: h2 ^= reinterpret_cast<u8 const*>(data)[2] << 16;
		case 2: h2 ^= reinterpret_cast<u8 const*>(data)[1] <<  8;
		case 1: h2 ^= reinterpret_cast<u8 const*>(data)[0] <<  0;
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
//                            //
// Time                       //
//                            //
////////////////////////////////

Time const TIME_ZERO = time::seconds(0);

namespace time
{
#if defined(GB_SYSTEM_WINDOWS)
	internal_linkage LARGE_INTEGER
	win32_get_frequency()
	{
		LARGE_INTEGER f;
		QueryPerformanceFrequency(&f);
		return f;
	}

	Time
	now()
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
		local_persist LARGE_INTEGER s_frequency = win32_get_frequency();

		// Get the current time
		LARGE_INTEGER t;
		QueryPerformanceCounter(&t);

		// Restore the thread affinity
		SetThreadAffinityMask(currentThread, previousMask);

		return time::microseconds(1000000ll * t.QuadPart / s_frequency.QuadPart);
	}

	void
	sleep(Time t)
	{
		if (t.microseconds <= 0)
			return;

		// Get the supported timer resolutions on this system
		TIMECAPS tc;
		timeGetDevCaps(&tc, sizeof(TIMECAPS));
		// Set the timer resolution to the minimum for the Sleep call
		timeBeginPeriod(tc.wPeriodMin);

		// Wait...
		::Sleep(time::as_milliseconds(t));

		// Reset the timer resolution back to the system default
		timeBeginPeriod(tc.wPeriodMin);
	}

#else
	Time
	now()
	{
	#if defined(GB_SYSTEM_OSX)
		s64 t = static_cast<s64>(mach_absolute_time());
		return microseconds(t);
	#else
		struct timeval t;
		gettimeofday(&t, nullptr);

		return microseconds((t.tv_sec * 1000000ll) + (t.tv_usec * 1ll));
	#endif
	}

	void
	sleep(Time t)
	{
		if (t.microseconds <= 0)
			return;

		struct timespec spec = {};
		spec.tv_sec = static_cast<s64>(as_seconds(t));
		spec.tv_nsec = 1000ll * (as_microseconds(t) % 1000000ll);

		nanosleep(&spec, nullptr);
	}

#endif

Time seconds(f32 s)              { return {static_cast<s64>(s * 1000000ll)}; }
Time milliseconds(s32 ms)        { return {static_cast<s64>(ms * 1000ll)}; }
Time microseconds(s64 us)        { return {us}; }
f32  as_seconds(Time t)      { return static_cast<f32>(t.microseconds / 1000000.0f); }
s32  as_milliseconds(Time t) { return static_cast<s32>(t.microseconds / 1000ll); }
s64  as_microseconds(Time t) { return t.microseconds; }
} // namespace time

bool operator==(Time left, Time right) { return left.microseconds == right.microseconds; }
bool operator!=(Time left, Time right) { return !operator==(left, right); }

bool operator<(Time left, Time right) { return left.microseconds < right.microseconds; }
bool operator>(Time left, Time right) { return left.microseconds > right.microseconds; }

bool operator<=(Time left, Time right) { return left.microseconds <= right.microseconds; }
bool operator>=(Time left, Time right) { return left.microseconds >= right.microseconds; }

Time operator+(Time right) { return {+right.microseconds}; }
Time operator-(Time right) { return {-right.microseconds}; }

Time operator+(Time left, Time right) { return {left.microseconds + right.microseconds}; }
Time operator-(Time left, Time right) { return {left.microseconds - right.microseconds}; }

Time& operator+=(Time& left, Time right) { return (left = left + right); }
Time& operator-=(Time& left, Time right) { return (left = left - right); }

Time operator*(Time left, f32 right) { return time::seconds(time::as_seconds(left) * right); }
Time operator*(Time left, s64 right) { return time::microseconds(time::as_microseconds(left) * right); }
Time operator*(f32 left, Time right) { return time::seconds(time::as_seconds(right) * left); }
Time operator*(s64 left, Time right) { return time::microseconds(time::as_microseconds(right) * left); }

Time& operator*=(Time& left, f32 right) { return (left = left * right); }
Time& operator*=(Time& left, s64 right) { return (left = left * right); }

Time operator/(Time left, f32 right) { return time::seconds(time::as_seconds(left) / right); }
Time operator/(Time left, s64 right) { return time::microseconds(time::as_microseconds(left) / right); }
f32 operator/(Time left, Time right) { return time::as_seconds(left) / time::as_seconds(right); }

Time& operator/=(Time& left, f32 right) { return (left = left / right); }
Time& operator/=(Time& left, s64 right) { return (left = left / right); }

Time operator%(Time left, Time right) { return time::microseconds(time::as_microseconds(left) % time::as_microseconds(right)); }
Time& operator%=(Time& left, Time right) { return (left = left % right); }

////////////////////////////////
//                            //
// OS                         //
//                            //
////////////////////////////////

namespace os
{
GB_FORCE_INLINE u64
rdtsc()
{
#if GB_SYSTEM_WINDOWS
	return ::__rdtsc();
#else
	// TODO(bill): Check that rdtsc() works
	return ::rdtsc();
#endif
}
} // namespace os

__GB_NAMESPACE_END

#endif // GB_IMPLEMENTATION
