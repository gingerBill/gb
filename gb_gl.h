/* gb.h - v0.05  - OpenGL Helper Library - public domain
                 - no warranty implied; use at your own risk

	This is a single header file with a bunch of useful stuff
	for working with OpenGL

===========================================================================
	YOU MUST

		#define GBGL_IMPLEMENTATION

	in EXACTLY _one_ C or C++ file that includes this header, BEFORE the
	include like this:

		#define GBGL_IMPLEMENTATION
		#include "gb_gl.h"

	All other files should just #include "gb_gl.h" without #define

	Dependencies
		NOTE: You may need change the path

		This library REQUIRES "stb_image.h" for loading images from file
		This library REQUIRES "gb.h" at this moment in time.
		If you are using the font library (e.g. GBGL_NO_FONTS is _not_ defined):
			This library then REQUIRES "stb_truetype.h" for ttf handling
			This library then REQUIRES "stb_rect_pack.h"

			NOTE(bill): I may remove these dependencies for the font handling by
			embedding the needed types and procedures.

	Optional Dependencies
		"gb_math.h" as a lot of useful things in it over <math.h>.
		Why not have a look at it?

	Optional Defines
		GBGL_NO_FONTS       - Do no use font subsystem
		GBGL_NO_BASIC_STATE - Do no use basic state subsystem

	Steps for supporting dynamic reload:
		You _MUST_ defined you own malloc and free that use whatever
		permanent memory system you are using:

			#define gbgl_malloc
			#define gbgl_free


===========================================================================

Conventions used:
	gbglTypesAreLikeThis (None core types)
	gbgl_functions_and_variables_like_this
	Prefer // Comments
	Never use _t suffix for types (I think they are stupid...)


Version History:
	0.06  - Enum convention change
	0.05  - gbglColour
	0.04e - Change brace style because why not?
	0.04d - Use new gb.h file handling system
	0.04c - Use new gb.h file handling system
	0.04b - Work with the new gb.h
	0.04a - Better Documentation
	0.04  - Remove gb_math.h dependency
	0.03a - Better Rounded Rect
	0.03  - Basic State Rendering
	0.02  - Font Caching and Rendering
	0.01  - Initial Version

LICENSE
	This software is dual-licensed to the public domain and under the following
	license: you are granted a perpetual, irrevocable license to copy, modify,
	publish, and distribute this file as you see fit.

WARNING
	- This library is _highly_ experimental and features may not work as expected.
	- This also means that many functions are not documented.

CREDITS
	Written by Ginger Bill

*/

#ifndef GBGL_INCLUDE_GB_GL_H
#define GBGL_INCLUDE_GB_GL_H

#ifndef GB_IMPLEMENTATION
#include "gb.h"
#endif

#ifndef STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#endif


#if !defined(GBGL_NO_FONTS)
	#ifndef STB_RECT_PACK_IMPLEMENTATION
	#include "stb_rect_pack.h"
	#endif

	#ifndef STB_TRUETYPE_IMPLEMENTATION
	#include "stb_truetype.h"
	#endif
#endif


#if defined(GBGL_USE_GB_MATH)
	#ifndef GB_MATH_IMPLEMENTATION
	#include "gb_math.h"
	#endif

	#define gbgl_lerp(x, y, t) gb_lerp(x, y, t)
	#define gbgl_sin(x)        gb_sin(x)
	#define gbgl_cos(x)        gb_cos(x)
	#define gbgl_abs(x)        gb_abs(x)
	#define gbgl_min(x, y)     gb_min(x, y)
	#define gbgl_max(x, y)     gb_max(x, y)
	#define gbgl_round(x)      gb_round(x)

#else
	#if !defined(GBGL_USE_CUSTOM_MATH)
	#include <math.h>

	#define gbgl_lerp(x, y, t) ((x)*(1.0f-(t)) + (y)*(t))
	#define gbgl_sin(x)        sinf(x)
	#define gbgl_cos(x)        cosf(x)
	#define gbgl_abs(x)        ((x) >= 0 ? (x) : -(x))
	#define gbgl_min(x, y)     ((x) < (y) ? (x) : (y))
	#define gbgl_max(x, y)     ((x) > (y) ? (x) : (y))
	#define gbgl_round(x)      (((x) >= 0.0f) ? floorf((x) + 0.5f) : ceilf((x) - 0.5f))

	#endif
#endif

#define gbgl_clamp(x, lower, upper) gbgl_min(gbgl_max((x), (lower)), (upper))
#define gbgl_clamp01(x) gbgl_clamp(x, 0, 1)


#if defined(__cplusplus)
extern "C" {
#endif


#ifndef GBGL_DEF
#define GBGL_DEF extern
#endif

#ifndef gbgl_malloc
#define gbgl_malloc(sz) malloc(sz)
#endif

#ifndef gbgl_free
#define gbgl_free(ptr) free(ptr)
#endif

#ifndef GBGL_TAU
#define GBGL_TAU 6.28318530717958647692528676655900576f
#endif



////////////////////////////////////////////////////////////////
//
// Colour Type
// It's quite useful
// TODO(bill): Does this need to be in this library?
//             Can I remove the anonymous struct extension?
//

#if defined(_MSC_VER)
#pragma warning(push)
#pragma warning(disable:4201)
#endif

typedef union gbglColour {
	u32    rgba; // NOTE(bill): 0xaabbggrr in little endian
	struct { u8 r, g, b, a; };
	u8     e[4];
} gbglColour;
GB_STATIC_ASSERT(gb_size_of(gbglColour) == gb_size_of(u32));

#if defined(_MSC_VER)
#pragma warning(pop)
#endif


GB_DEF gbglColour gbgl_colour(f32 r, f32 g, f32 b, f32 a);

gb_global gbglColour const gbglColour_White   = {0xffffffff};
gb_global gbglColour const gbglColour_Grey    = {0xff808080};
gb_global gbglColour const gbglColour_Black   = {0xff000000};

gb_global gbglColour const gbglColour_Red     = {0xff0000ff};
gb_global gbglColour const gbglColour_Orange  = {0xff0099ff};
gb_global gbglColour const gbglColour_Yellow  = {0xff00ffff};
gb_global gbglColour const gbglColour_Green   = {0xff00ff00};
gb_global gbglColour const gbglColour_Cyan    = {0xffffff00};
gb_global gbglColour const gbglColour_Blue    = {0xffff0000};
gb_global gbglColour const gbglColour_Violet  = {0xffff007f};
gb_global gbglColour const gbglColour_Magenta = {0xffff00ff};


////////////////////////////////////////////////////////////////
//
// Generic Stuff
//
//


#ifndef GBGL_VERT_PTR_AA_GENERIC
#define GBGL_VERT_PTR_AA_GENERIC
// NOTE(bill) The "default" is just the f32 version
#define gbgl_vert_ptr_aa(index, element_count, Type, var_name) \
    gbgl_vert_ptr_aa_f32(index, element_count, Type, var_name)

#define gbgl_vert_ptr_aa_f32(index, element_count, Type, var_name) do {  \
	glVertexAttribPointer(index,                                         \
	                      element_count,                                 \
	                      GL_FLOAT,                                      \
	                      false,                                         \
	                      gb_size_of(Type),                              \
	                      (void const *)(gb_offset_of(Type, var_name))); \
	glEnableVertexAttribArray(index);                                    \
} while (0)

#define gbgl_vert_ptr_aa_u8(index, element_count, Type, var_name) do {   \
	glVertexAttribPointer(index,                                         \
	                      element_count,                                 \
	                      GL_UNSIGNED_BYTE,                              \
	                      false,                                         \
	                      gb_size_of(Type),                              \
	                      (void const *)(gb_offset_of(Type, var_name))); \
	glEnableVertexAttribArray(index);                                    \
} while (0)

#define gbgl_vert_ptr_aa_u8n(index, element_count, Type, var_name) do {  \
	glVertexAttribPointer(index,                                         \
	                      element_count,                                 \
	                      GL_UNSIGNED_BYTE,                              \
	                      true,                                          \
	                      gb_size_of(Type),                              \
	                      (void const *)(gb_offset_of(Type, var_name))); \
	glEnableVertexAttribArray(index);                                    \
} while (0)

#define gbgl_vert_ptr_aa_u32(index, element_count, Type, var_name) do {   \
	glVertexAttribIPointer(index,                                         \
	                       element_count,                                 \
	                       GL_UNSIGNED_INT,                               \
	                       gb_size_of(Type),                              \
	                       (void const *)(gb_offset_of(Type, var_name))); \
	glEnableVertexAttribArray(index);                                     \
} while (0)

#define gbgl_vert_ptr_aa_u16(index, element_count, Type, var_name) do {   \
	glVertexAttribIPointer(index,                                         \
	                       element_count,                                 \
	                       GL_UNSIGNED_SHORT,                             \
	                       gb_size_of(Type),                              \
	                       (void const *)(gb_offset_of(Type, var_name))); \
	glEnableVertexAttribArray(index);                                     \
} while (0)

#define gbgl_vert_ptr_aa_u16n(index, element_count, Type, var_name) do { \
	glVertexAttribPointer(index,                                         \
	                      element_count,                                 \
	                      GL_UNSIGNED_SHORT,                             \
	                      true,                                          \
	                      gb_size_of(Type),                              \
	                      (void const *)(gb_offset_of(Type, var_name))); \
	glEnableVertexAttribArray(index);                                    \
} while (0)

#endif


GBGL_DEF u32 gbgl_make_sampler(u32 min_filter, u32 max_filter, u32 s_wrap, u32 t_wrap);




////////////////////////////////////////////////////////////////
//
// Data Buffers
//
//


typedef enum gbglBufferDataType {
	gbglBufferData_u8 = GL_UNSIGNED_BYTE,
	gbglBufferData_i8 = GL_BYTE,

	gbglBufferData_u16 = GL_UNSIGNED_SHORT,
	gbglBufferData_i16 = GL_SHORT,
	gbglBufferData_f16 = GL_HALF_FLOAT,

	gbglBufferData_u32 = GL_UNSIGNED_INT,
	gbglBufferData_i32 = GL_INT,
	gbglBufferData_f32 = GL_FLOAT,

	gbglBufferData_f8, // NOTE(bill): This is not a "real" OpenGL type but it is needed for internal format enums
} gbglBufferDataType;


// NOTE(bill) index+1 = channels count
#if defined(GBGL_USE_SRGB_TEXTURE_FORMAT)
i32 const gbglTextureFormat[4] = { GL_RED, GL_RG, GL_SRGB8, GL_SRGB8_ALPHA8 };
#else
i32 const gbglTextureFormat[4] = { GL_RED, GL_RG, GL_RGB, GL_RGBA };
#endif

i32 const gbglInternalTextureFormat_8[4]  = { GL_R8,   GL_RG8,   GL_RGB8,	  GL_RGBA8   };
i32 const gbglInternalTextureFormat_16[4] = { GL_R16,  GL_RG16,  GL_RGB16,  GL_RGBA16  };
i32 const gbglInternalTextureFormat_32[4] = { GL_R32F, GL_RG32F, GL_RGB32F, GL_RGBA32F };

i32 const gbglInternalTextureFormat_u8[4] = { GL_R8UI, GL_RG8UI, GL_RGB8UI, GL_RGB8UI };
i32 const gbglInternalTextureFormat_i8[4] = { GL_R8I,  GL_RG8I,  GL_RGB8I,  GL_RGB8I  };
i32 const gbglInternalTextureFormat_f8[4] = { GL_R8,   GL_RG8,   GL_RGB8,   GL_RGB8   };

i32 const gbglInternalTextureFormat_u16[4] = { GL_R16UI, GL_RG16UI, GL_RGB16UI, GL_RGB16UI };
i32 const gbglInternalTextureFormat_i16[4] = { GL_R16I,  GL_RG16I,  GL_RGB16I,  GL_RGB16I  };
i32 const gbglInternalTextureFormat_f16[4] = { GL_R16F,  GL_RG16F,  GL_RGB16F,  GL_RGB16F  };

i32 const gbglInternalTextureFormat_u32[4] = { GL_R32UI, GL_RG32UI, GL_RGB32UI, GL_RGBA32UI };
i32 const gbglInternalTextureFormat_i32[4] = { GL_R32I,  GL_RG32I,  GL_RGB32I,  GL_RGBA32I  };
i32 const gbglInternalTextureFormat_f32[4] = { GL_R32F,  GL_RG32F,  GL_RGB32F,  GL_RGBA32F  };

gb_inline i32 gbgl_texture_format(gbglBufferDataType data_type, i32 channel_count) {
	GB_ASSERT(gb_is_between(channel_count, 1, 4));
	switch (data_type) {
	case gbglBufferData_u8:  return gbglInternalTextureFormat_u8[channel_count-1];
	case gbglBufferData_i8:  return gbglInternalTextureFormat_i8[channel_count-1];
	case gbglBufferData_f8:  return gbglInternalTextureFormat_f8[channel_count-1];
	case gbglBufferData_u16: return gbglInternalTextureFormat_u16[channel_count-1];
	case gbglBufferData_i16: return gbglInternalTextureFormat_i16[channel_count-1];
	case gbglBufferData_f16: return gbglInternalTextureFormat_f16[channel_count-1];
	case gbglBufferData_u32: return gbglInternalTextureFormat_u32[channel_count-1];
	case gbglBufferData_i32: return gbglInternalTextureFormat_i32[channel_count-1];
	case gbglBufferData_f32: return gbglInternalTextureFormat_f32[channel_count-1];
	}
	return gbglInternalTextureFormat_f32[4-1];
}

typedef struct gbglTBO {
	u32 buffer_obj_handle;
	u32 buffer_handle;
} gbglTBO;

// NOTE(bill): usage_hint == (GL_STATIC_DRAW, GL_STREAM_DRAW, GL_DYNAMIC_DRAW)
GBGL_DEF u32     gbgl_make_vbo(void const *data, isize size, i32 usage_hint);
GBGL_DEF u32     gbgl_make_ebo(void const *data, isize size, i32 usage_hint);
GBGL_DEF gbglTBO gbgl_make_tbo(gbglBufferDataType data_type, i32 channel_count, void const *data, isize size, i32 usage_hint);

GBGL_DEF void gbgl_vbo_copy(u32 vbo_handle, void *const data, isize size, isize offset);
GBGL_DEF void gbgl_ebo_copy(u32 ebo_handle, void *const data, isize size, isize offset);
GBGL_DEF void gbgl_tbo_copy(gbglTBO tbo,    void *const data, isize size, isize offset);



GBGL_DEF void gbgl_bind_vbo(u32 vbo_handle);
GBGL_DEF void gbgl_bind_ebo(u32 ebo_handle);
GBGL_DEF void gbgl_bind_tbo(gbglTBO tbo, i32 sampler_handle, i32 tex_unit);

// NOTE(bill): access = GL_WRITE_ONLY, etc.
GBGL_DEF void *gbgl_map_vbo(u32 vbo_handle, i32 access);
GBGL_DEF void *gbgl_map_ebo(u32 ebo_handle, i32 access);
GBGL_DEF void gbgl_unmap_vbo(void);
GBGL_DEF void gbgl_unmap_ebo(void);



////////////////////////////////////////////////////////////////
//
// Shader
//
//

typedef enum gbglShaderType {
	gbglShader_Vertex,
	gbglShader_Fragment,
	gbglShader_Geometry,

	gbglShader_Count,
} gbglShaderType;

i32 const gbglShaderMap[gbglShader_Count] = {
	GL_VERTEX_SHADER,   /* gbglShader_Vertex   */
	GL_FRAGMENT_SHADER, /* gbglShader_Fragment */
	GL_GEOMETRY_SHADER, /* gbglShader_Geometry */
};

typedef enum gbglShaderError {
	gbglShaderError_None,
	gbglShaderError_ShaderCompile,
	gbglShaderError_Linking,
	gbglShaderError_UnableToReadFile,

	gbglShaderError_Count,
} gbglShaderError;

#ifndef GBGL_MAX_UNIFORM_COUNT
#define GBGL_MAX_UNIFORM_COUNT 32
#endif

typedef struct gbglShader {
	u32 shaders[gbglShader_Count];
	u32 program;

	i32   uniform_locs[GBGL_MAX_UNIFORM_COUNT];
	char *uniform_names[GBGL_MAX_UNIFORM_COUNT];
	i32   uniform_count;

	u32   type_flags;

	gbFile files[gbglShader_Count];

	char base_name[64];
} gbglShader;



#ifndef GBGL_SHADER_FILE_EXTENSIONS_DEFINED
#define GBGL_SHADER_FILE_EXTENSIONS_DEFINED
gb_global char const *gbglShaderFileExtensions[gbglShader_Count] = {".vs", ".fs", ".gs"};
#endif


GBGL_DEF gbglShaderError gbgl_load_shader_from_file      (gbglShader *s, u32 type_bits, char const *filename);
GBGL_DEF gbglShaderError gbgl_load_shader_from_memory_vf (gbglShader *s, char const *vert_source, char const *frag_source);
GBGL_DEF gbglShaderError gbgl_load_shader_from_memory_vfg(gbglShader *s, char const *vert_source, char const *frag_source, char const *geom_source);

GBGL_DEF void gbgl_destroy_shader    (gbglShader *shader);
GBGL_DEF b32  gbgl_has_shader_changed(gbglShader *shader);
GBGL_DEF b32  gbgl_reload_shader     (gbglShader *shader); // TODO(bill): Return an error code?
GBGL_DEF void gbgl_use_shader        (gbglShader *shader);
GBGL_DEF b32  gbgl_is_shader_in_use  (gbglShader *shader);

GBGL_DEF i32 gbgl_get_uniform(gbglShader *shader, char const *name);

GBGL_DEF void gbgl_set_uniform_int       (gbglShader *s, char const *name, i32 i);
GBGL_DEF void gbgl_set_uniform_float     (gbglShader *s, char const *name, f32 f);
GBGL_DEF void gbgl_set_uniform_vec2      (gbglShader *s, char const *name, f32 const *v);
GBGL_DEF void gbgl_set_uniform_vec3      (gbglShader *s, char const *name, f32 const *v);
GBGL_DEF void gbgl_set_uniform_vec4      (gbglShader *s, char const *name, f32 const *v);
GBGL_DEF void gbgl_set_uniform_mat4      (gbglShader *s, char const *name, f32 const *m);
GBGL_DEF void gbgl_set_uniform_mat4_count(gbglShader *s, char const *name, f32 const *m, isize count);
GBGL_DEF void gbgl_set_uniform_colour    (gbglShader *s, char const *name, gbglColour col);


////////////////////////////////////////////////////////////////
//
// Texture
//
//

typedef enum gbglTextureType {
	gbglgTexture_2D,
	gbglgTexture_CubeMap,

	gbglgTexture_Count,
} gbglTextureType;

gb_global i32 const GBGL_TEXTURE_TYPE[gbglgTexture_Count] = {
	GL_TEXTURE_2D,       /* gbglgTexture_2D */
	GL_TEXTURE_CUBE_MAP, /* gbglgTexture_CubeMap */
};


typedef struct gbglTexture {
	i32 width, height, channel_count;
	gbglBufferDataType data_type;
	gbglTextureType type;
	u32 handle;
} gbglTexture;

GBGL_DEF b32  gbgl_load_texture2d_from_file  (gbglTexture *texture, b32 flip_vertically, char const *filename, ...);
GBGL_DEF b32  gbgl_load_texture2d_from_memory(gbglTexture *texture, void const *data, i32 width, i32 height, i32 channel_count);
GBGL_DEF b32  gbgl_init_texture2d_coloured   (gbglTexture *texture, gbglColour colour);
GBGL_DEF void gbgl_destroy_texture           (gbglTexture *texture);

GBGL_DEF void gbgl_bind_texture2d(gbglTexture const *texture, u32 position, u32 sampler);





////////////////////////////////////////////////////////////////
//
// Render Buffer
//
//

// TODO(bill): Record depth and stencil and numerous colour attachments

typedef struct gbglRenderBuffer {
	i32 width, height;
	i32 channel_count;
	u32 handle;

	gbglTexture colour_texture;
} gbglRenderBuffer;

#define GBGL_MAX_RENDER_COLOUR_BUFFERS 16
gb_global u32 const gbglColourBufferAttachments[GBGL_MAX_RENDER_COLOUR_BUFFERS] = {
	GL_COLOR_ATTACHMENT0,
	GL_COLOR_ATTACHMENT1,
	GL_COLOR_ATTACHMENT2,
	GL_COLOR_ATTACHMENT3,
	GL_COLOR_ATTACHMENT4,
	GL_COLOR_ATTACHMENT5,
	GL_COLOR_ATTACHMENT6,
	GL_COLOR_ATTACHMENT7,
	GL_COLOR_ATTACHMENT8,
	GL_COLOR_ATTACHMENT9,
	GL_COLOR_ATTACHMENT10,
	GL_COLOR_ATTACHMENT11,
	GL_COLOR_ATTACHMENT12,
	GL_COLOR_ATTACHMENT13,
	GL_COLOR_ATTACHMENT14,
	GL_COLOR_ATTACHMENT15,
};


GBGL_DEF b32  gbgl_init_render_buffer   (gbglRenderBuffer *rb, i32 width, i32 height, i32 channel_count);
GBGL_DEF void gbgl_destroy_render_buffer(gbglRenderBuffer *rb);
GBGL_DEF void gbgl_render_to_buffer(gbglRenderBuffer const *rb);
GBGL_DEF void gbgl_render_to_screen(i32 width, i32 height);


////////////////////////////////////////////////////////////////
//
// Font Stuff
//
//

#if !defined(GBGL_NO_FONTS)

typedef struct gbglGlyphMapKVPair {
	char32 codepoint;
	u16    index;
} gbglGlyphMapKVPair;

typedef struct gbglGlyphInfo {
	f32 s0, t0, s1, t1;
	i16 xoff, yoff;
	f32 xadv;
} gbglGlyphInfo;

typedef struct gbglKernPair {
	union {
		i32 packed;
		struct { u16 i0, i1; };
	};
	f32 kern;
} gbglKernPair;

typedef enum gbglJustifyType {
	gbglJustify_Left,
	gbglJustify_Centre,
	gbglJustify_Right,
} gbglJustifyType;

typedef enum gbglTextParamType {
	gbglTextParam_Invalid,
	gbglTextParam_MaxWidth,
	gbglTextParam_Justify,
	gbglTextParam_TextureFilter,

	gbglTextParam_Count,
} gbglTextParamType;

typedef struct gbglTextParam {
	gbglTextParamType type;
	union {
		f32 val_f32;
		i32 val_i32;
	};
} gbglTextParam;

typedef struct gbglFont {
	isize glyph_count;
	isize kern_pair_count;
	i32 bitmap_width, bitmap_height;
	f32 size;
	i32 ascent, descent, line_gap;
	char *ttf_filename;
	gbglTexture texture;

	gbglGlyphMapKVPair *glyph_map;
	gbglGlyphInfo *     glyphs;
	gbglKernPair *      kern_table;

	struct gbglFont *next; // NOTE(bill): Allow as linked list
} gbglFont;

typedef struct gbglFontCachedTTF {
	char *                    name;
	u8 *                      ttf;
	stbtt_fontinfo            finfo;
	struct gbglFontCachedTTF *next;
} gbglFontCachedTTF;

typedef struct gbglFontCache {
	isize font_char_list_count;
	char *font_char_list;

	isize   codepoint_count;
	char32 *codepoints;

	stbtt_pack_range *ranges;
	stbtt_packedchar *packed_char_data;
	stbrp_rect *      rect_cache;

	gbglFontCachedTTF *ttf_buffer;
	gbglFont *         fonts;
} gbglFontCache;


#if 0
GBGL_DEF void gbgl_destroy_font_cache(gbglFontCache *fc);
#endif

// NOTE(bill): gbgl_load_font_from_file will load from file if it is not found
GBGL_DEF gbglFont *gbgl_load_font_from_file     (gbglFontCache *fc, char const *ttf_filename, f32 font_size);
GBGL_DEF gbglFont *gbgl_get_font_only_from_cache(gbglFontCache *fc, char const *ttf_filename, f32 font_size);
GBGL_DEF gbglFont *gbgl_cache_font              (gbglFontCache *fc, char const *ttf_filename, f32 font_size);


GBGL_DEF b32            gbgl_get_packed_font_dim                (gbglFontCache *cache, gbglFontCachedTTF *ttf, i32 *width, i32 *height);
GBGL_DEF gbglGlyphInfo *gbgl_get_glyph_info                     (gbglFont *font, char32 codepoint, isize *out_index);
GBGL_DEF f32            gbgl_get_font_kerning_from_glyph_indices(gbglFont *font, isize left_index, isize right_index);
GBGL_DEF void           gbgl_get_string_dimensions              (gbglFont *font, char const *str, f32 *out_width, f32 *out_height);
GBGL_DEF f32            gbgl_get_sub_string_width               (gbglFont *font, char const *str, isize char_count);
GBGL_DEF i32            gbgl_get_wrapped_line_count             (gbglFont *font, char const *str, isize max_len, isize max_width);
GBGL_DEF f32            gbgl_get_string_width                   (gbglFont *font, char const *str, isize max_len);


#endif

////////////////////////////////////////////////////////////////
//
// Basic State
//
//

#if !defined(GBGL_NO_BASIC_STATE)

#ifndef GBGL_BS_MAX_VERTEX_COUNT
#define GBGL_BS_MAX_VERTEX_COUNT 32
#endif

#ifndef GBGL_BS_MAX_INDEX_COUNT
#define GBGL_BS_MAX_INDEX_COUNT 6
#endif


#if !defined(GBGL_NO_FONTS)

#ifndef GBGL_MAX_RENDER_STRING_LENGTH
#define GBGL_MAX_RENDER_STRING_LENGTH 4096
#endif

#ifndef gbglTextParam_Stack_size
#define gbglTextParam_Stack_size 128
#endif

#ifndef GBGL_FONT_CHAR_LIST
#define GBGL_FONT_CHAR_LIST \
	"ĀāăĄąĆćĈĉĊċČčĎďĐđĒēĔĕĖėĘęĚěĜĝĞğĠġĢģĤĥĨĩĪīĬĭĮįİıĲĳĴĵĶķĸĹĺĻļĽľŁł"\
	"ŃńŅņņŇňŉŊŋŌōōŎŏŐőŒœŕŖŗŘřŚśŜŝŞşŠšŢţŤťŨũŪūŬŭŮůŰűŲųŴŵŶŷŸŹźŻżŽža!ö"\
	"\"#$%%&\'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~"\
	"ŠšŒœŸÀÁÂÃÄÅÆÇÈÉÊËÌÍÎÏÐÑÒÓÔÕÖØÙÚÛÜÝÞßàáâãäåæçèéêëìíîïðñòóôõøùúûüýþÿ®™£"\
	" \t\r\n"
#endif

#ifndef GBGL_PT_TO_PX_SCALE
#define GBGL_PT_TO_PX_SCALE (96.0f / 72.0f)
#endif

#ifndef GBGL_TAB_CHARACTER_WIDTH
#define GBGL_TAB_CHARACTER_WIDTH 4
#endif

#endif

typedef struct gbglBasicVertex {
	f32 x, y;
	f32 u, v;
} gbglBasicVertex;

typedef struct gbglBasicState {
	gbglBasicVertex vertices[GBGL_BS_MAX_VERTEX_COUNT];
	u16 indices[GBGL_BS_MAX_INDEX_COUNT];

	u32 vao, vbo, ebo;
	u32 nearest_sampler;
	u32 linear_sampler;
	u32 mipmap_sampler;
	gbglShader ortho_tex_shader;
	gbglShader ortho_col_shader;

	f32 ortho_mat[16];
	i32 width, height;

#if !defined(GBGL_NO_FONTS)
	gbglFontCache   font_cache;
	gbglShader      font_shader;
	gbglBasicVertex font_vertices[GBGL_MAX_RENDER_STRING_LENGTH * 4];
	u16             font_indices[GBGL_MAX_RENDER_STRING_LENGTH * 6];
	u32             font_vao, font_vbo, font_ebo;
	char            font_text_buffer[GBGL_MAX_RENDER_STRING_LENGTH * 4]; // NOTE(bill): Maximum of 4 bytes per char for utf-8
	u32             font_samplers[2];

	gbglTextParam text_param_stack[gbglTextParam_Stack_size];
	isize         text_param_stack_count;
	gbglTextParam text_params[gbglTextParam_Count];
#endif
} gbglBasicState;

GBGL_DEF void gbgl_bs_init(gbglBasicState *bs, i32 window_width, i32 window_height);
GBGL_DEF void gbgl_bs_set_resolution(gbglBasicState *bs, i32 window_width, i32 window_height);
GBGL_DEF void gbgl_bs_begin(gbglBasicState *bs, i32 window_width, i32 window_height);
GBGL_DEF void gbgl_bs_end(gbglBasicState *bs);

GBGL_DEF void gbgl_bs_draw_textured_rect(gbglBasicState *bs, gbglTexture *tex, f32 x, f32 y, f32 w, f32 h, b32 v_up);
GBGL_DEF void gbgl_bs_draw_rect(gbglBasicState *bs, f32 x, f32 y, f32 w, f32 h, gbglColour col);
GBGL_DEF void gbgl_bs_draw_rect_outline(gbglBasicState *bs, f32 x, f32 y, f32 w, f32 h, gbglColour col, f32 thickness);
GBGL_DEF void gbgl_bs_draw_quad(gbglBasicState *bs,
                                f32 x0, f32 y0,
                                f32 x1, f32 y1,
                                f32 x2, f32 y2,
                                f32 x3, f32 y3,
                                gbglColour col);
GBGL_DEF void gbgl_bs_draw_quad_outline(gbglBasicState *bs,
                                        f32 x0, f32 y0,
                                        f32 x1, f32 y1,
                                        f32 x2, f32 y2,
                                        f32 x3, f32 y3,
                                        gbglColour col, f32 thickness);

GBGL_DEF void gbgl_bs_draw_line(gbglBasicState *bs, f32 x0, f32 y0, f32 x1, f32 y1, gbglColour col, f32 thickness);

GBGL_DEF void gbgl_bs_draw_elliptical_arc(gbglBasicState *bs, f32 x, f32 y, f32 radius_a, f32 radius_b, f32 min_angle, f32 max_angle, gbglColour col);
GBGL_DEF void gbgl_bs_draw_elliptical_arc_outline(gbglBasicState *bs, f32 x, f32 y, f32 radius_a, f32 radius_b,
                                                  f32 min_angle, f32 max_angle, gbglColour col, f32 thickness);

GBGL_DEF void gbgl_bs_draw_circle(gbglBasicState *bs, f32 x, f32 y, f32 radius, gbglColour col);
GBGL_DEF void gbgl_bs_draw_circle_outline(gbglBasicState *bs, f32 x, f32 y, f32 radius, gbglColour col, f32 thickness);


// Corners Flags:
// 1 - Bottom Left
// 2 - Bottom Right
// 4 - Top    Right
// 8 - Top    Left
// NOTE(bill): Apple, please don't sue me!
GBGL_DEF void gbgl_bs_draw_rounded_rect_corners(gbglBasicState *bs, f32 x, f32 y, f32 w, f32 h, f32 roundness, gbglColour col, u32 corners);
GBGL_DEF void gbgl_bs_draw_rounded_rect(gbglBasicState *bs, f32 x, f32 y, f32 w, f32 h, f32 roundness, gbglColour col);

GBGL_DEF void gbgl_bs_draw_rounded_rect_corners_outline(gbglBasicState *bs, f32 x, f32 y, f32 w, f32 h, f32 roundness, gbglColour col, f32 thickness, u32 corners);
GBGL_DEF void gbgl_bs_draw_rounded_rect_outline(gbglBasicState *bs, f32 x, f32 y, f32 w, f32 h, f32 roundness, gbglColour col, f32 thickness);


#if !defined(GBGL_NO_FONTS)
GBGL_DEF isize gbgl_bs_draw_substring(gbglBasicState *bs, gbglFont *font, f32 x, f32 y, gbglColour col, char const *str, isize len);
GBGL_DEF isize gbgl_bs_draw_string   (gbglBasicState *bs, gbglFont *font, f32 x, f32 y, gbglColour col, char const *fmt, ...);
GBGL_DEF isize gbgl_bs_draw_string_va(gbglBasicState *bs, gbglFont *font, f32 x, f32 y, gbglColour col, char const *fmt, va_list va);
#endif


#endif

#if defined(__cplusplus)
}
#endif

#endif

////////////////////////////////////////////////////////////////
//                                                            //
//                                                            //
//                                                            //
//                                                            //
//                                                            //
//                                                            //
//                                                            //
//                                                            //
//                                                            //
//                                                            //
//                                                            //
//                                                            //
//                                                            //
//                                                            //
//                                                            //
//                                                            //
//                                                            //
//                                                            //
//                                                            //
//                                                            //
//                                                            //
//                        Implementation                      //
//                                                            //
//                                                            //
//                                                            //
//                                                            //
//                                                            //
//                                                            //
//                                                            //
//                                                            //
//                                                            //
//                                                            //
//                                                            //
//                                                            //
//                                                            //
//                                                            //
//                                                            //
//                                                            //
//                                                            //
//                                                            //
//                                                            //
//                                                            //
//                                                            //
//                                                            //
//                                                            //
////////////////////////////////////////////////////////////////


#if defined(GBGL_IMPLEMENTATION)

gb_inline gbglColour gbgl_colour(f32 r, f32 g, f32 b, f32 a) {
	gbglColour result;
	result.r = cast(u8)(gbgl_clamp01(r) * 255.0f);
	result.g = cast(u8)(gbgl_clamp01(g) * 255.0f);
	result.b = cast(u8)(gbgl_clamp01(b) * 255.0f);
	result.a = cast(u8)(gbgl_clamp01(a) * 255.0f);
	return result;
}


u32 gbgl_make_sampler(u32 min_filter, u32 max_filter, u32 s_wrap, u32 t_wrap) {
	u32 samp;
	glGenSamplers(1, &samp);
	glSamplerParameteri(samp, GL_TEXTURE_MIN_FILTER, min_filter);
	glSamplerParameteri(samp, GL_TEXTURE_MAG_FILTER, max_filter);
	glSamplerParameteri(samp, GL_TEXTURE_WRAP_S,     s_wrap);
	glSamplerParameteri(samp, GL_TEXTURE_WRAP_T,     t_wrap);
	return samp;
}


////////////////////////////////////////////////////////////////
//
// Data Buffers
//
//

gb_inline u32 gbgl__make_buffer(isize size, void const *data, i32 target, i32 usage_hint) {
	u32 buffer_handle;
	glGenBuffers(1, &buffer_handle);
	glBindBuffer(target, buffer_handle);
	glBufferData(target, size, data, usage_hint);
	return buffer_handle;
}

gb_inline void gbgl__buffer_copy(u32 buffer_handle, i32 target, void const *data, isize size, isize offset) {
	glBindBuffer(target, buffer_handle);
	glBufferSubData(target, offset, size, data);
}

// NOTE(bill): usage_hint == (GL_STATIC_DRAW, GL_STREAM_DRAW, GL_DYNAMIC_DRAW)
gb_inline u32 gbgl_make_vbo(void const *data, isize size, i32 usage_hint) {
	return gbgl__make_buffer(size, data, GL_ARRAY_BUFFER, usage_hint);
}

gb_inline u32 gbgl_make_ebo(void const *data, isize size, i32 usage_hint) {
	return gbgl__make_buffer(size, data, GL_ELEMENT_ARRAY_BUFFER, usage_hint);
}

gb_inline gbglTBO gbgl_make_tbo(gbglBufferDataType data_type, i32 channel_count, void const *data, isize size, i32 usage_hint) {
	gbglTBO tbo;
	i32 internal_format;

	tbo.buffer_obj_handle = gbgl__make_buffer(size, data, GL_TEXTURE_BUFFER, usage_hint);

	glGenTextures(1, &tbo.buffer_handle);
	glBindTexture(GL_TEXTURE_BUFFER, tbo.buffer_handle);
	internal_format = gbgl_texture_format(data_type, channel_count);
	glTexBuffer(GL_TEXTURE_BUFFER, internal_format, tbo.buffer_obj_handle);
	return tbo;
}

gb_inline void gbgl_vbo_copy(u32 vbo_handle, void *const data, isize size, isize offset) {
	gbgl__buffer_copy(vbo_handle, GL_ARRAY_BUFFER, data, size, offset);
}

gb_inline void gbgl_ebo_copy(u32 ebo_handle, void *const data, isize size, isize offset) {
	gbgl__buffer_copy(ebo_handle, GL_ELEMENT_ARRAY_BUFFER, data, size, offset);
}

gb_inline void gbgl_tbo_copy(gbglTBO tbo, void *const data, isize size, isize offset) {
	gbgl__buffer_copy(tbo.buffer_obj_handle, GL_TEXTURE_BUFFER, data, size, offset);
}

gb_inline void gbgl_bind_vbo(u32 vbo_handle) { glBindBuffer(GL_ARRAY_BUFFER, vbo_handle); }
gb_inline void gbgl_bind_ebo(u32 ebo_handle) { glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_handle); }

gb_inline void gbgl_bind_tbo(gbglTBO tbo, i32 sampler_handle, i32 tex_unit) {
	glActiveTexture(GL_TEXTURE0 + tex_unit);
	glBindTexture(GL_TEXTURE_BUFFER, tbo.buffer_handle);
	glBindSampler(0, sampler_handle);
}

// NOTE(bill): access = GL_WRITE_ONLY, etc.
gb_inline void * gbgl_map_vbo(u32 vbo_handle, i32 access) {
	gbgl_bind_vbo(vbo_handle);
	return glMapBuffer(GL_ARRAY_BUFFER, access);
}

gb_inline void * gbgl_map_ebo(u32 ebo_handle, i32 access) {
	gbgl_bind_ebo(ebo_handle);
	return glMapBuffer(GL_ELEMENT_ARRAY_BUFFER, access);
}

gb_inline void gbgl_unmap_vbo(void) { glUnmapBuffer(GL_ARRAY_BUFFER); }
gb_inline void gbgl_unmap_ebo(void) { glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER); }



////////////////////////////////////////////////////////////////
//
// Shader
//
//


gbglShaderError gbgl__load_single_shader_from_file(gbglShader *shader, gbglShaderType type, char const *name) {
	gbglShaderError err = gbglShaderError_None;
	gbFileError ferr;
	gb_local_persist char filepath[1024];
	gb_snprintf(filepath, gb_count_of(filepath), "%s%s", name, gbglShaderFileExtensions[type]);
	ferr = gb_file_open(&shader->files[type], filepath);

	if (ferr != gbFileError_None) {
		err = gbglShaderError_UnableToReadFile;
	} else {
		gb_local_persist char info_log[4096];
		i64 file_size = gb_file_size(&shader->files[type]);
		char *file_source = cast(char *)gbgl_malloc(file_size+1);

		GB_ASSERT_NOT_NULL(file_source);
		if (file_source) {
			i32 params;

			gb_file_read_at(&shader->files[type], file_source, file_size, 0);
			file_source[file_size] = '\0';

			shader->shaders[type] = glCreateShader(gbglShaderMap[type]);
			glShaderSource(shader->shaders[type], 1, &file_source, NULL);
			glCompileShader(shader->shaders[type]);
			glGetShaderiv(shader->shaders[type], GL_COMPILE_STATUS, &params);
			if (!params) {
				gb_printf_err("Shader Source:\n%s\n", file_source);
				glGetShaderInfoLog(shader->shaders[type], gb_size_of(info_log), NULL, info_log);
				gb_printf_err("Shader compilation failed:\n %s\n", info_log);

				err = gbglShaderError_ShaderCompile;
			}

			gbgl_free(file_source);
		}
		gb_file_close(&shader->files[type]);
	}

	return err;
}

gbglShaderError gbgl__load_single_shader_from_memory(gbglShader *s, gbglShaderType type, char const *text) {
	gbglShaderError err = gbglShaderError_None;
	i32 status;

	s->shaders[type] = glCreateShader(gbglShaderMap[type]);
	glShaderSource(s->shaders[type], 1, &text, 0);
	glCompileShader(s->shaders[type]);

	glGetShaderiv(s->shaders[type], GL_COMPILE_STATUS, &status);
	if (!status) {
		gb_local_persist char log_info[4096];
		i32 total_len, log_len;

		gb_printf_err("Unable to compile shader: %s\n", text);
		glGetShaderiv(s->shaders[type], GL_INFO_LOG_LENGTH, &status);
		total_len = status;

		glGetShaderInfoLog(s->shaders[type], 4095, &log_len, log_info);
		gb_printf_err(log_info);
		err = gbglShaderError_ShaderCompile;
	}

	return err;
}

gbglShaderError gbgl__link_shader(gbglShader *shader) {
	gbglShaderError err = gbglShaderError_None;
	i32 i, status;
	shader->program = glCreateProgram();
	for (i = 0; i < gbglShader_Count; i++) {
		if (shader->type_flags & GB_BIT(i))
			glAttachShader(shader->program, shader->shaders[i]);
	}

	glLinkProgram(shader->program);

	glGetProgramiv(shader->program, GL_LINK_STATUS, &status);
	if (!status) {
		gb_local_persist char log_info[4096];
		glGetProgramInfoLog(shader->program, gb_size_of(log_info), NULL, log_info);
		gb_printf_err("Shader linking failed:\n %s \n", log_info);
		err = gbglShaderError_Linking;
	}

	for (i = 0; i < gbglShader_Count; i++) {
		if (shader->type_flags & GB_BIT(i))
			glDetachShader(shader->program, shader->shaders[i]);
	}

	return err;
}



gbglShaderError gbgl_load_shader_from_file(gbglShader *shader, u32 type_bits, char const *filename) {
	gbglShaderError err = gbglShaderError_None;
	b32 loaded_shader[gbglShader_Count] = {0};
	i32 i;

	gb_zero_item(shader);
	shader->type_flags = type_bits;
	gb_strncpy(shader->base_name, filename, gb_size_of(shader->base_name));

	for (i = 0; i < gbglShader_Count; i++) {
		if (type_bits & GB_BIT(i)) {
			err = gbgl__load_single_shader_from_file(shader, cast(gbglShaderType)i, filename);
			if (err != gbglShaderError_None)
				return err;
			loaded_shader[i] = true;
		}
	}
	err = gbgl__link_shader(shader);

	return err;
}




gbglShaderError gbgl_load_shader_from_memory_vf(gbglShader *s, char const *vert_source, char const *frag_source) {
	gbglShaderError err = gbglShaderError_None;

	gb_zero_item(s);
	s->type_flags = GB_BIT(gbglShader_Vertex) | GB_BIT(gbglShader_Fragment);

	err = gbgl__load_single_shader_from_memory(s, gbglShader_Vertex, vert_source);
	if (err != gbglShaderError_None) return err;
	err = gbgl__load_single_shader_from_memory(s, gbglShader_Fragment, frag_source);
	if (err != gbglShaderError_None) return err;

	err = gbgl__link_shader(s);

	return err;
}

gbglShaderError gbgl_load_shader_from_memory_vfg(gbglShader *s, char const *vert_source, char const *frag_source, char const *geom_source) {
	gbglShaderError err = gbglShaderError_None;

	gb_zero_item(s);
	s->type_flags = GB_BIT(gbglShader_Vertex) | GB_BIT(gbglShader_Fragment) | GB_BIT(gbglShader_Geometry);

	err = gbgl__load_single_shader_from_memory(s, gbglShader_Vertex, vert_source);
	if (err != gbglShaderError_None) return err;
	err = gbgl__load_single_shader_from_memory(s, gbglShader_Fragment, frag_source);
	if (err != gbglShaderError_None) return err;
	err = gbgl__load_single_shader_from_memory(s, gbglShader_Geometry, geom_source);
	if (err != gbglShaderError_None) return err;

	err = gbgl__link_shader(s);

	return err;
}

gb_inline void gbgl_destroy_shader(gbglShader *shader) {
	i32 i;
	for (i = 0; i < gbglShader_Count; i++) {
		if (shader->type_flags & GB_BIT(i)) {
			gb_file_close(&shader->files[i]);
			glDeleteShader(shader->shaders[i]);
		}
	}

	glDeleteProgram(shader->program);

	for (i = 0; i < shader->uniform_count; i++) {
		gbgl_free(shader->uniform_names[i]);
	}
}


gb_inline b32 gbgl_has_shader_changed(gbglShader *shader) {
	i32 i;
	for (i = 0; i < gbglShader_Count; i++) {
		if (shader->type_flags & GB_BIT(i)) {
			if (gb_file_has_changed(&shader->files[i])) {
				return true;
			}
		}
	}
	return false;
}


b32 gbgl_reload_shader(gbglShader *shader) {
	i32 i;
	for (i = 0; i < gbglShader_Count; i++) {
		if (shader->type_flags & GB_BIT(i)) {
			if (gbgl__load_single_shader_from_file(shader, cast(gbglShaderType)i, shader->base_name) != gbglShaderError_None)
				return false;
		}
	}

	if (gbgl__link_shader(shader) != gbglShaderError_None)
		return false;

	for (i = 0; i < shader->uniform_count; i++)
		shader->uniform_locs[i] = glGetUniformLocation(shader->program, shader->uniform_names[i]);


	return true;
}

gb_inline void gbgl_use_shader(gbglShader *s) { glUseProgram(s ? s->program : 0); }

gb_inline b32 gbgl_is_shader_in_use(gbglShader *s) {
	if (s) {
		i32 curr = 0;
		glGetIntegerv(GL_CURRENT_PROGRAM, &curr);
		return (curr == cast(i32)s->program);
	}
	return false;
}


i32 gbgl_get_uniform(gbglShader *s, char const *name) {
	i32 i, loc = -1;
	for (i = 0; i < s->uniform_count; i++) {
		if (gb_strcmp(s->uniform_names[i], name) == 0) {
			return s->uniform_locs[i];
		}
	}

	GB_ASSERT_MSG(s->uniform_count < GBGL_MAX_UNIFORM_COUNT,
	              "Uniform array for shader is full");

	loc = glGetUniformLocation(s->program, name);
	s->uniform_names[s->uniform_count] = gb_alloc_str(gb_heap_allocator(), name);
	s->uniform_locs[s->uniform_count] = loc;
	s->uniform_count++;

	return loc;
}



gb_inline void gbgl_set_uniform_int(gbglShader *s, char const *name, i32 i) {
	glUniform1i(gbgl_get_uniform(s, name), i);
}

gb_inline void gbgl_set_uniform_float(gbglShader *s, char const *name, f32 f) {
	glUniform1f(gbgl_get_uniform(s, name), f);
}

gb_inline void gbgl_set_uniform_vec2(gbglShader *s, char const *name, f32 const *v) {
	glUniform2fv(gbgl_get_uniform(s, name), 1, v);
}

gb_inline void gbgl_set_uniform_vec3(gbglShader *s, char const *name, f32 const *v) {
	glUniform3fv(gbgl_get_uniform(s, name), 1, v);
}

gb_inline void gbgl_set_uniform_vec4(gbglShader *s, char const *name, f32 const *v) {
	glUniform4fv(gbgl_get_uniform(s, name), 1, v);
}

gb_inline void gbgl_set_uniform_mat4(gbglShader *s, char const *name, f32 const *m) {
	gbgl_set_uniform_mat4_count(s, name, m, 1);
}

gb_inline void gbgl_set_uniform_mat4_count(gbglShader *s, char const *name, f32 const *m, isize count) {
	glUniformMatrix4fv(gbgl_get_uniform(s, name), count, false, m);
}


gb_inline void gbgl_set_uniform_colour(gbglShader *s, char const *name, gbglColour col) {
	f32 v[4];
	v[0] = col.r / 255.0f;
	v[1] = col.g / 255.0f;
	v[2] = col.b / 255.0f;
	v[3] = col.a / 255.0f;
	gbgl_set_uniform_vec4(s, name, v);
}



////////////////////////////////////////////////////////////////
//
// Render Buffer
//
//


b32 gbgl_init_render_buffer(gbglRenderBuffer *rb, i32 width, i32 height, i32 channel_count) {
	if ((rb->width == width) && (rb->height == height) && (rb->channel_count == channel_count)) return true;
	gbgl_destroy_render_buffer(rb);
	gb_zero_item(rb);

	rb->width = width;
	rb->height = height;

	glEnable(GL_FRAMEBUFFER_SRGB);

	glGenFramebuffers(1, &rb->handle);
	glBindFramebuffer(GL_FRAMEBUFFER, rb->handle);

	gbgl_load_texture2d_from_memory(&rb->colour_texture, NULL, width, height, channel_count);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, rb->colour_texture.handle, 0);

	glBindTexture(GL_TEXTURE_2D, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	{
		u32 draw_buffers[] = {GL_COLOR_ATTACHMENT0};
		glDrawBuffers(gb_count_of(draw_buffers), draw_buffers);
	}

	{
		u32 status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
		if (status != GL_FRAMEBUFFER_COMPLETE) {
			gb_printf_err("Framebuffer Status: 0x%x\n", status);
			return false;
		}
	}

	return true;
}

gb_inline void gbgl_destroy_render_buffer(gbglRenderBuffer *rb) {
	if (rb->handle)
		glDeleteFramebuffers(1, &rb->handle);

	gbgl_destroy_texture(&rb->colour_texture);
}


gb_inline void gbgl_render_to_buffer(gbglRenderBuffer const *rb) {
	GB_ASSERT_NOT_NULL(rb);
	glViewport(0, 0, rb->width, rb->height);
	glBindFramebuffer(GL_FRAMEBUFFER, rb->handle);
}

gb_inline void gbgl_render_to_screen(i32 width, i32 height) {
	glViewport(0, 0, width, height);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}


////////////////////////////////////////////////////////////////
//
// Texture
//
//


b32 gbgl_load_texture2d_from_memory(gbglTexture *tex, void const *data, i32 width, i32 height, i32 channel_count) {
	b32 result = true;

	gb_zero_item(tex);

	tex->width = width;
	tex->height = height;
	tex->channel_count = channel_count;
	tex->data_type = gbglBufferData_u8;
	tex->type = gbglgTexture_2D;

	glGenTextures(1, &tex->handle);
	glBindTexture(GL_TEXTURE_2D, tex->handle);


	glTexImage2D(GL_TEXTURE_2D, 0,
	             gbglInternalTextureFormat_8[channel_count-1],
	             width, height, 0,
	             gbglTextureFormat[channel_count-1],
	             GL_UNSIGNED_BYTE, data);

	glGenerateMipmap(GL_TEXTURE_2D);

	glBindTexture(GL_TEXTURE_2D, 0);
	glFinish();


	return result;
}

b32 gbgl_load_texture2d_from_file(gbglTexture *texture, b32 flip_vertically, char const *filename, ...) {
	b32 result;
	u8 *data;
	int width, height, comp;
	char *path;

	va_list va;
	va_start(va, filename);
	path = gb_bprintf_va(filename, va);
	va_end(va);

	stbi_set_flip_vertically_on_load(flip_vertically);
	data = stbi_load(path, &width, &height, &comp, 0);
	if (data == NULL) {
		gb_printf_err("Failed to load image: %s\n", path);
		result = false;
	} else {
		result = gbgl_load_texture2d_from_memory(texture, data, width, height, comp);
		stbi_image_free(data);
	}
	return result;
}

gb_inline b32 gbgl_make_texture2d_coloured(gbglTexture *t, gbglColour colour) {
	return gbgl_load_texture2d_from_memory(t, &colour.rgba, 1, 1, 4);
}


gb_inline void gbgl_bind_texture2d(gbglTexture const *t, u32 position, u32 sampler) {
	if (t != NULL) {
		GB_ASSERT(t->type == gbglgTexture_2D);
	}

	if (position > 31) {
		position = 31;
		gb_printf_err("Textures can only bound to position [0 ... 31]\n");
		gb_printf_err("Will bind to position [31]\n");
	}

	glActiveTexture(GL_TEXTURE0 + position);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, t ? t->handle : 0);
	glBindSampler(position, sampler);
}

gb_inline void gbgl_destroy_texture(gbglTexture *t) {
	if (t->handle) {
		glDeleteTextures(1, &t->handle);
	}
}



////////////////////////////////////////////////////////////////
//
// Font
//
//
#if !defined(GBGL_NO_FONTS)
gb_inline GB_COMPARE_PROC(gbgl__kern_pair_compare) {
	gbglKernPair *kp0 = cast(gbglKernPair *)a;
	gbglKernPair *kp1 = cast(gbglKernPair *)b;
	return kp0->packed - kp1->packed;
}


gb_inline GB_COMPARE_PROC(gbgl__glyph_map_compare) {
	gbglGlyphMapKVPair g0 = *cast(gbglGlyphMapKVPair *)a;
	gbglGlyphMapKVPair g1 = *cast(gbglGlyphMapKVPair *)b;
	return g0.codepoint - g1.codepoint;
}




b32 gbgl_get_packed_font_dim(gbglFontCache *cache, gbglFontCachedTTF *ttf, i32 *width, i32 *height) {
	b32 result = true;
	stbtt_pack_context spc;
	b32 ext_w = true;
	isize sanity_counter = 0, i, j;
	*width = *height = (1<<5);
	for (j = 0; j < cache->codepoint_count; j++) {
		cache->rect_cache[j].x = cache->rect_cache[j].y = 0;
	}

	for (;;) {
		i32 res = stbtt_PackBegin(&spc, NULL, *width, *height, 0, 1, 0);
		GB_ASSERT(res == 1);
		if (res != 1) {
			return false;
		} else {
			stbrp_context *rp_ctx;
			b32 fit = true;

			gb_zero_array(cache->rect_cache, cache->codepoint_count);
			rp_ctx = cast(stbrp_context *)spc.pack_info;
			stbtt_PackFontRangesGatherRects(&spc, &ttf->finfo, cache->ranges, cache->codepoint_count, cache->rect_cache);
			gb_sort_array(cache->rect_cache, cache->codepoint_count, rect_height_compare);

			for (i = 0; i < cache->codepoint_count; i++) {
				stbrp__findresult fr = stbrp__skyline_pack_rectangle(rp_ctx, cache->rect_cache[i].w, cache->rect_cache[i].h);
				if (fr.prev_link) {
					cache->rect_cache[i].x = cast(stbrp_coord)fr.x;
					cache->rect_cache[i].y = cast(stbrp_coord)fr.y;
				} else {
					int res;
					if (ext_w) {
						ext_w = false;
						*width <<= 1;
					} else {
						ext_w = true;
						*height <<= 1;
					}
					fit = false;
					for (j = 0; j < cache->codepoint_count; j++) {
						cache->rect_cache[j].x = cache->rect_cache[j].y = 0;
					}
					stbtt_PackEnd(&spc);
					res = stbtt_PackBegin(&spc, NULL, *width, *height, 0, 1, 0);
					GB_ASSERT(res == 1);
					if (res != 1) {
						result = false;
						goto done;
					}
					break;
				}
			}
			if (fit) {
				result = true;
				goto done;
			}
			if (++sanity_counter > 32) {
				result = false;
				goto done;
			}
		}
	}

done:
	stbtt_PackEnd(&spc);

	return result;
}

#if 0
void gbgl_destroy_font_cache(gbglFontCache *fc) {
	gbglFontCachedTTF *curr_ttf = fc->ttf_buffer;
	gbglFontCachedTTF *next_ttf = NULL;

	gbglFont *curr_font = fc->fonts;
	gbglFont *next_font = NULL;

	gbgl_free(fc->font_char_list);
	gbgl_free(fc->codepoints);
	gbgl_free(fc->ranges);
	gbgl_free(fc->packed_char_data);
	gbgl_free(fc->rect_cache);

	// NOTE(bill): Free all linked listed ttfs
	while (curr_ttf) {
		gbgl_free(curr_ttf->name);
		gbgl_free(curr_ttf->ttf);

		next_ttf = curr_ttf->next;
		gbgl_free(curr_ttf);
		curr_ttf = next_ttf;
	}

	// NOTE(bill): Free all linked listed fonts
	while (curr_font) {
		gbgl_free(curr_font->ttf_filename);
		gbgl_destroy_texture(&curr_font->texture);
		gbgl_free(curr_font->glyph_map);
		gbgl_free(curr_font->glyphs);
		gbgl_free(curr_font->kern_table);

		next_font = curr_font->next;
		gbgl_free(curr_font);
		curr_font = next_font;
	}
}
#endif


gb_inline gbglFont * gbgl_load_font_from_file(gbglFontCache *fc, char const *ttf_filename, f32 font_size) {
	gbglFont *f = gbgl_get_font_only_from_cache(fc, ttf_filename, font_size);
	if (f) return f;
	return gbgl_cache_font(fc, ttf_filename, font_size);
}


gb_inline gbglFont * gbgl_get_font_only_from_cache(gbglFontCache *fc, char const *ttf_filename, f32 font_size) {
	gbglFont *f = fc->fonts;
	while (f) {
		if (f->size == font_size && gb_strcmp(ttf_filename, f->ttf_filename) == 0) {
			return f;
		}
		f = f->next;
	}
	return NULL;
}

gbglFont * gbgl_cache_font(gbglFontCache *fc, char const *ttf_filename, f32 font_size) {
	gbglFont *f = gbgl_get_font_only_from_cache(fc, ttf_filename, font_size);
	gbglFontCachedTTF *ttf = NULL;
	isize i;

	if (f) { // NOTE(bill): The font is already cached
		return f;
	}

	if (!fc->fonts) {
		fc->fonts = cast(gbglFont *)gbgl_malloc(gb_size_of(gbglFont));
		f = fc->fonts;
	} else {
		f = fc->fonts;
		while (f && f->next)
			f = f->next;
		f->next = cast(gbglFont *)gbgl_malloc(gb_size_of(gbglFont));
		f = f->next;
	}
	GB_ASSERT_NOT_NULL(f);
	if (!f) {
		gb_printf_err("Failed to cache font\n");
		return NULL;
	}

	gb_zero_item(f);

	// NOTE(bill): Make sure the character list file has been loaded
	if (!fc->font_char_list) {
		isize codepoint_count = 0, cpi = 0;
		fc->font_char_list = GBGL_FONT_CHAR_LIST;
		fc->font_char_list_count = gb_strlen(GBGL_FONT_CHAR_LIST);

		for (i = 0; i < fc->font_char_list_count; i++) {
			char32 c;
			isize utf8_len = gb_utf8_decode_len(fc->font_char_list + i, fc->font_char_list_count, &c);
			i += utf8_len-1;
			codepoint_count++;
		}

		fc->codepoint_count = codepoint_count;
		fc->ranges           = cast(stbtt_pack_range *)gbgl_malloc(gb_size_of(stbtt_pack_range) * codepoint_count);
		fc->codepoints       = cast(char32 *)          gbgl_malloc(gb_size_of(char32)           * codepoint_count);
		fc->packed_char_data = cast(stbtt_packedchar *)gbgl_malloc(gb_size_of(stbtt_packedchar) * codepoint_count);
		fc->rect_cache       = cast(stbrp_rect *)      gbgl_malloc(gb_size_of(stbrp_rect)       * codepoint_count);

		if (!fc->ranges || !fc->codepoints || !fc->packed_char_data) {
			gb_printf_err("Unable to get memory for fonts");
		}

		for (i = 0; i < fc->font_char_list_count; i++) {
			isize utf8_len = gb_utf8_decode_len(fc->font_char_list+i, fc->font_char_list_count, fc->codepoints+cpi);
			i += utf8_len-1;
			cpi++;
		}
		GB_ASSERT(cpi == fc->codepoint_count);
		for (i = 0; i < fc->codepoint_count; i++) {
			fc->ranges[i].first_unicode_codepoint_in_range = fc->codepoints[i];
			fc->ranges[i].array_of_unicode_codepoints = 0;
			fc->ranges[i].num_chars = 1;
			fc->ranges[i].chardata_for_range = fc->packed_char_data + i;
		}
	}

	{
		gbglFontCachedTTF **ttf_cache = &fc->ttf_buffer;

		while (*ttf_cache) {
			if (gb_strcmp((*ttf_cache)->name, ttf_filename) == 0)
				break;
			ttf_cache = &(*ttf_cache)->next;
		}
		if (!*ttf_cache) {
			isize name_len;
			gbFile file;


			*ttf_cache = cast(gbglFontCachedTTF *)gbgl_malloc(gb_size_of(gbglFontCachedTTF));
			GB_ASSERT_NOT_NULL(*ttf_cache);
			(*ttf_cache)->name = NULL;
			(*ttf_cache)->ttf  = NULL;
			gb_zero_item(&(*ttf_cache)->finfo);
			(*ttf_cache)->next = NULL;


			name_len = gb_strlen(ttf_filename);
			(*ttf_cache)->name = cast(char *)gbgl_malloc(name_len+1);
			gb_strncpy((*ttf_cache)->name, ttf_filename, name_len);
			(*ttf_cache)->name[name_len] = '\0';

			if (gb_file_open(&file, ttf_filename) == gbFileError_None) {
				i64 len = gb_file_size(&file);
				(*ttf_cache)->ttf = cast(u8 *)gbgl_malloc(len);
				GB_ASSERT_NOT_NULL((*ttf_cache)->ttf);

				gb_file_read_at(&file, (*ttf_cache)->ttf, len, 0);

				gb_file_close(&file);
			} else {
				GB_PANIC("Could not open ttf file");
			}

			stbtt_InitFont(&(*ttf_cache)->finfo, (*ttf_cache)->ttf, stbtt_GetFontOffsetForIndex((*ttf_cache)->ttf, 0));
		}
		ttf = *ttf_cache;
		GB_ASSERT_NOT_NULL(ttf);
	}

	// NOTE(bill): Set the range for the this look up
	for (i = 0; i < fc->codepoint_count; i++)
		fc->ranges[i].font_size = font_size;

	{ // NOTE(bill): Figure out smallest non-square power of 2 texture size
		i32 w, h;
		if (gbgl_get_packed_font_dim(fc, ttf, &w, &h)) {
			isize str_len, i, j;

			// NOTE(bill): Setup the font data
			f->glyph_count   = fc->codepoint_count;
			f->bitmap_width  = w;
			f->bitmap_height = h;
			f->size          = font_size;

			str_len = gb_strlen(ttf_filename);
			f->ttf_filename = cast(char *)gbgl_malloc(str_len+1);
			gb_strncpy(f->ttf_filename, ttf_filename, str_len);

			f->glyph_map = cast(gbglGlyphMapKVPair *)gbgl_malloc(gb_size_of(*f->glyph_map) * f->glyph_count);
			f->glyphs    = cast(gbglGlyphInfo *)     gbgl_malloc(gb_size_of(*f->glyphs)    * f->glyph_count);
			if (!f->glyph_map || !f->glyphs) {
				f = NULL;
				return f;
			} else {
				stbtt_pack_context spc;
				u8 *px;
				i32 res;
				f32 scale;

				px = cast(u8 *)gbgl_malloc(w * h);
				res = stbtt_PackBegin(&spc, px, w, h, 0, 1, NULL);
				GB_ASSERT(res == 1);
				res = stbtt_PackFontRanges(&spc, ttf->ttf, 0, fc->ranges, fc->codepoint_count);
				GB_ASSERT(res == 1);
				stbtt_PackEnd(&spc);

				gbgl_load_texture2d_from_memory(&f->texture, px, w, h, 1);

				gbgl_free(px);

				scale = stbtt_ScaleForPixelHeight(&ttf->finfo, font_size);
				stbtt_GetFontVMetrics(&ttf->finfo, &f->ascent, &f->descent, &f->line_gap);
				f->ascent   = cast(i32)(cast(f32)f->ascent   * scale);
				f->descent  = cast(i32)(cast(f32)f->descent  * scale);
				f->line_gap = cast(i32)(cast(f32)f->line_gap * scale);

				for (i = 0; i < f->glyph_count; i++) {
					gbglGlyphInfo *gi = f->glyphs + i;
					gi->s0 = cast(f32)fc->packed_char_data[i].x0;
					gi->t0 = cast(f32)fc->packed_char_data[i].y0;
					gi->s1 = cast(f32)fc->packed_char_data[i].x1;
					gi->t1 = cast(f32)fc->packed_char_data[i].y1;

					gi->xoff = cast(i16)fc->packed_char_data[i].xoff;
					gi->yoff = cast(i16)fc->packed_char_data[i].yoff;
					gi->xadv = fc->packed_char_data[i].xadvance;
				}

				for (i = 0; i < f->glyph_count; i++) {
					f->glyph_map[i].codepoint = fc->codepoints[i];
					f->glyph_map[i].index = i;
				}

				gb_sort_array(f->glyph_map, f->glyph_count, gbgl__glyph_map_compare);

				{ // Kerning Table
					isize kps_count = 0;
					for (i = 0; i < f->glyph_count; i++) {
						for (j = 0; j < f->glyph_count; j++) {
							i32 kern = stbtt_GetCodepointKernAdvance(&ttf->finfo, fc->codepoints[i], fc->codepoints[j]);
							if (kern != 0)
								kps_count++;
						}
					}
					f->kern_pair_count = kps_count;
					if (kps_count > 0) {
						int ikp = 0;
						f->kern_table = cast(gbglKernPair *)gbgl_malloc(gb_size_of(*f->kern_table) * kps_count);
						for (i = 0; i < f->glyph_count; i++) {
							for (j = 0; j < f->glyph_count; j++) {
								isize kern = stbtt_GetCodepointKernAdvance(&ttf->finfo, fc->codepoints[i], fc->codepoints[j]);
								if (kern != 0) {
									gbglKernPair *kp = f->kern_table + ikp++;
									kp->i0 = cast(u16)i;
									kp->i1 = cast(u16)j;
									kp->kern = cast(f32)kern * scale;
								}
							}
						}
						gb_sort_array(f->kern_table, f->kern_pair_count, gbgl__kern_pair_compare);
					}
				}
			}
		} else {
			GB_PANIC("Failure loading font");
			gb_zero_item(&f);
		}
	}
	return f;
}


gb_inline GB_COMPARE_PROC(gbgl__font_glyph_map_search_proc) {
	gbglGlyphMapKVPair const *gm = cast(gbglGlyphMapKVPair const *)a;
	char32 ucp = *cast(char32 const *)b;
	return cast(i32)(cast(i64)gm->codepoint - cast(i64)ucp);
}

gb_inline gbglGlyphInfo * gbgl_get_glyph_info(gbglFont *font, char32 codepoint, isize *out_index) {
	isize index = gb_binary_search_array(font->glyph_map, font->glyph_count, &codepoint, gbgl__font_glyph_map_search_proc);
	if (index >= 0) {
		GB_ASSERT(codepoint == font->glyph_map[index].codepoint);
		if (out_index)
			*out_index = font->glyph_map[index].index;
		return font->glyphs + font->glyph_map[index].index;
	}
	return NULL;
}

gb_inline f32 gbgl_get_font_kerning_from_glyph_indices(gbglFont *font, isize left_index, isize right_index) {
	isize needle = (right_index << 16) | (left_index & 0xff);

	isize f = 0;
	isize l = font->kern_pair_count - 1;
	isize m = (f + l) >> 1;

	while (f <= l) {
		isize cmp = font->kern_table[m].packed - needle;
		if (cmp < 0)
			f = m + 1;
		else if (cmp > 0)
			l = m - 1;
		else
			return font->kern_table[m].kern;
		m = (f + l) >> 1;
	}
	return 0.0f;
}

void gbgl_get_string_dimensions(gbglFont *font, char const *str, f32 *out_width, f32 *out_height) {
	isize len, char_count, i;

	f32 w = 0.0f;
	f32 h = 0.0f;
	char const *ptr = str;

	len = gb_strlen(str);
	char_count = gb_utf8_strnlen(str, len);

	for (i = 0; i < char_count; i++) {
		char32 cp;
		isize byte_len, curr_index;
		gbglGlyphInfo *gi;

		byte_len = gb_utf8_decode_len(ptr, len-(ptr-str), &cp);
		ptr += byte_len;
		gi = gbgl_get_glyph_info(font, cp, &curr_index);
		if (gi) {
			f32 kern = 0;
			if (i < char_count-1) {
				isize next_index;
				char32 next_cp = 0;
				gbglGlyphInfo *ngi;
				gb_utf8_decode_len(ptr, len-(ptr-str), &next_cp);
				ngi = gbgl_get_glyph_info(font, next_cp, &next_index);
				if (ngi) kern = gbgl_get_font_kerning_from_glyph_indices(font, curr_index, next_index);
			}
			w += gi->xadv + kern;
		}
	}

	if (out_width)  *out_width  = w;
	if (out_height) *out_height = h;
}

f32 gbgl_get_sub_string_width(gbglFont *font, char const *str, isize char_count) {
	isize i, len;
	f32 w = 0;
	char const *ptr = str;
	len = gb_strlen(str);
	for (i = 0; i < char_count; i++) {
		if (*ptr == 0) {
			break;
		} else {
			char32 cp;
			isize byte_len, curr_index;
			f32 kern = 0;
			gbglGlyphInfo *gi;

			byte_len = gb_utf8_decode_len(ptr, len-(ptr-str), &cp);
			ptr += byte_len;
			if (ptr - str > char_count)
				break;

			gi = gbgl_get_glyph_info(font, cp, &curr_index);
			if (i < char_count-1) {
				isize next_index;
				char32 next_cp = 0;
				gb_utf8_decode_len(ptr, len-(ptr-str), &next_cp);
				gbgl_get_glyph_info(font, next_cp, &next_index);
				kern = gbgl_get_font_kerning_from_glyph_indices(font, curr_index, next_index);
			}
			w += gi->xadv + kern;
		}

	}
	return w;
}

i32 gbgl_get_wrapped_line_count(gbglFont *font, char const *str, isize max_len, isize max_width) {
	isize i, str_len, char_count, line_count = 1;
	f32 w = 0;
	char const *ptr = str;

	str_len = gb_strnlen(str, max_len);
	char_count = gb_utf8_strnlen(str, str_len);

	for (i = 0; i < char_count; i++) {
		char32 cp;
		isize byte_len, curr_index;
		gbglGlyphInfo *gi;
		f32 kern = 0;

		byte_len = gb_utf8_decode_len(ptr, str_len-(ptr-str), &cp);
		ptr += byte_len;
		// NOTE(bill): Check calculation here
		if (ptr-str >= max_len-6)
			break;

		gi = gbgl_get_glyph_info(font, cp, &curr_index);
		if (gi) {
			if (w + gi->xadv >= cast(f32)max_width) {
				line_count++;
				w = 0.0f;
			}
		}

		if (i < char_count-1) {
			char32 next_cp;
			isize next_index;
			gb_utf8_decode_len(ptr, str_len-(ptr-str), &next_cp);

			gbgl_get_glyph_info(font, next_cp, &next_index);
			kern = gbgl_get_font_kerning_from_glyph_indices(font, curr_index, next_index);
		}

		if (gi) {
			w += gi->xadv + kern;
		}
	}

	return line_count;
}

gb_inline f32 gbgl_get_string_width(gbglFont *font, char const *str, isize max_len) {
	isize len = gb_strnlen(str, max_len);
	isize char_count = gb_utf8_strnlen(str, len);
	return gbgl_get_sub_string_width(font, str, char_count);
}

#endif


////////////////////////////////////////////////////////////////
//
// Basic State
//
//

#if !defined(GBGL_NO_BASIC_STATE)


void gbgl_bs_init(gbglBasicState *bs, i32 window_width, i32 window_height) {
	isize i;

	gbgl_bs_set_resolution(bs, window_width, window_height);
	glGenVertexArrays(1, &bs->vao);
	glBindVertexArray(bs->vao);

	bs->vbo = gbgl_make_vbo(NULL, gb_size_of(gbglBasicVertex) * GBGL_BS_MAX_VERTEX_COUNT, GL_DYNAMIC_DRAW);

	for (i = 0; i < GBGL_BS_MAX_INDEX_COUNT / 6; i++) {
		bs->indices[i*6 + 0] = i*4 + 0;
		bs->indices[i*6 + 1] = i*4 + 1;
		bs->indices[i*6 + 2] = i*4 + 2;
		bs->indices[i*6 + 3] = i*4 + 2;
		bs->indices[i*6 + 4] = i*4 + 3;
		bs->indices[i*6 + 5] = i*4 + 0;
	}
	bs->ebo = gbgl_make_ebo(bs->indices, gb_size_of(u16) * GBGL_BS_MAX_INDEX_COUNT, GL_STATIC_DRAW);

	bs->nearest_sampler = gbgl_make_sampler(GL_NEAREST, GL_NEAREST, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
	bs->linear_sampler  = gbgl_make_sampler(GL_LINEAR,  GL_LINEAR,  GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
	bs->mipmap_sampler = gbgl_make_sampler(GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, GL_REPEAT, GL_REPEAT);

	gbgl_load_shader_from_memory_vf(&bs->ortho_tex_shader,
		"#version 410 core\n"
		"layout (location = 0) in vec4 a_position;\n"
		"layout (location = 1) in vec2 a_tex_coord;\n"
		"uniform mat4 u_ortho_mat;\n"
		"out vec2 v_tex_coord;\n"
		"void main(void) {\n"
		"	gl_Position = u_ortho_mat * a_position;\n"
		"	v_tex_coord = a_tex_coord;\n"
		"}\n",

		"#version 410 core\n"
		"precision mediump float;"
		"in vec2 v_tex_coord;\n"
		"layout (binding = 0) uniform sampler2D u_tex;\n"
		"out vec4 o_colour;\n"
		"void main(void) {\n"
		"	o_colour = texture2D(u_tex, v_tex_coord);\n"
		"}\n"
	);

	gbgl_load_shader_from_memory_vf(&bs->ortho_col_shader,
		"#version 410 core\n"
		"precision mediump float;"
		"layout (location = 0) in vec4 a_position;\n"
		"uniform mat4 u_ortho_mat;\n"
		"void main(void) {\n"
		"	gl_Position = u_ortho_mat * a_position;\n"
		"}\n",

		"#version 410 core\n"
		"uniform vec4 u_colour;\n"
		"out vec4 o_colour;\n"
		"void main(void) {\n"
		"	o_colour = u_colour;\n"
		"}\n"
	);


#if !defined(GBGL_NO_FONTS)
	gbgl_load_shader_from_memory_vf(&bs->font_shader,
		"#version 410 core\n"
		"layout (location = 0) in vec4 a_position;\n"
		"layout (location = 1) in vec2 a_tex_coord;\n"
		"uniform mat4 u_ortho_mat;\n"
		"out vec2 v_tex_coord;\n"
		"void main(void) {\n"
		"	gl_Position = u_ortho_mat * a_position;\n"
		"	v_tex_coord = a_tex_coord;\n"
		"}\n",

		"#version 410 core\n"
		"in vec2 v_tex_coord;\n"
		"uniform vec4 u_colour;\n"
		"layout (binding = 0) uniform sampler2D u_tex;\n"
		"out vec4 o_colour;\n"
		"void main(void) {\n"
		"	o_colour = u_colour * texture2D(u_tex, v_tex_coord).r;\n"
		"}\n"
	);

	glGenVertexArrays(1, &bs->font_vao);
	glBindVertexArray(bs->font_vao);

	bs->font_vbo = gbgl_make_vbo(NULL, gb_size_of(gbglBasicVertex) * GBGL_MAX_RENDER_STRING_LENGTH * 4, GL_DYNAMIC_DRAW);

	for (i = 0; i < GBGL_MAX_RENDER_STRING_LENGTH; i++) {
		bs->font_indices[i*6 + 0] = i*4 + 0;
		bs->font_indices[i*6 + 1] = i*4 + 1;
		bs->font_indices[i*6 + 2] = i*4 + 2;
		bs->font_indices[i*6 + 3] = i*4 + 2;
		bs->font_indices[i*6 + 4] = i*4 + 3;
		bs->font_indices[i*6 + 5] = i*4 + 0;
	}
	bs->font_ebo = gbgl_make_ebo(bs->font_indices, gb_size_of(u16) * GBGL_MAX_RENDER_STRING_LENGTH * 6, GL_STATIC_DRAW);

	bs->font_samplers[0] = gbgl_make_sampler(GL_NEAREST, GL_NEAREST, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
	bs->font_samplers[1] = gbgl_make_sampler(GL_LINEAR,  GL_LINEAR,  GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);

	bs->text_params[gbglTextParam_MaxWidth]     .val_i32 = 0;
	bs->text_params[gbglTextParam_Justify]       .val_i32 = gbglJustify_Left;
	bs->text_params[gbglTextParam_TextureFilter].val_i32 = 0;
#endif
}

gb_inline void gbgl_bs_set_resolution(gbglBasicState *bs, i32 window_width, i32 window_height) {
	f32 left   = 0.0f;
	f32 right  = cast(f32)window_width;
	f32 bottom = 0.0f;
	f32 top    = cast(f32)window_height;
	f32 znear  = 0.0f;
	f32 zfar   = 1.0f;

	bs->width  = window_width;
	bs->height = window_height;

	bs->ortho_mat[0] = 2.0f / (right - left);
	bs->ortho_mat[1] = 0.0f;
	bs->ortho_mat[2] = 0.0f;
	bs->ortho_mat[3] = 0.0f;

	bs->ortho_mat[4] = 0.0f;
	bs->ortho_mat[5] = 2.0f / (top - bottom);
	bs->ortho_mat[6] = 0.0f;
	bs->ortho_mat[7] = 0.0f;

	bs->ortho_mat[8]  =  0.0f;
	bs->ortho_mat[9]  =  0.0f;
	bs->ortho_mat[10] = -2.0f / (zfar - znear);
	bs->ortho_mat[11] =  0.0f;

	bs->ortho_mat[12] = -(right + left) / (right - left);
	bs->ortho_mat[13] = -(top + bottom) / (top - bottom);
	bs->ortho_mat[14] = -(zfar + znear) / (zfar - znear);
	bs->ortho_mat[15] = 1.0f;
}

gb_inline void gbgl_bs_begin(gbglBasicState *bs, i32 window_width, i32 window_height) {
	glBindVertexArray(bs->vao);
	glDisable(GL_SCISSOR_TEST);
	gbgl_bs_set_resolution(bs, window_width, window_height);
}

gb_inline void gbgl_bs_end(gbglBasicState *bs) {
	glBindVertexArray(0);
}




void gbgl_bs_draw_textured_rect(gbglBasicState *bs, gbglTexture *tex, f32 x, f32 y, f32 w, f32 h, b32 v_up) {
	bs->vertices[0].x = x;
	bs->vertices[0].y = y;
	bs->vertices[0].u = 0.0f;
	bs->vertices[0].v = v_up ? 0.0f : 1.0f;

	bs->vertices[1].x = x + w;
	bs->vertices[1].y = y;
	bs->vertices[1].u = 1.0f;
	bs->vertices[1].v = v_up ? 0.0f : 1.0f;

	bs->vertices[2].x = x + w;
	bs->vertices[2].y = y + h;
	bs->vertices[2].u = 1.0f;
	bs->vertices[2].v = v_up ? 1.0f : 0.0f;

	bs->vertices[3].x = x;
	bs->vertices[3].y = y + h;
	bs->vertices[3].u = 0.0f;
	bs->vertices[3].v = v_up ? 1.0f : 0.0f;

	gbgl_use_shader(&bs->ortho_tex_shader);
	gbgl_set_uniform_mat4(&bs->ortho_tex_shader, "u_ortho_mat", bs->ortho_mat);
	gbgl_bind_texture2d(tex, 0, bs->mipmap_sampler);

	gbgl_vbo_copy(bs->vbo, bs->vertices, 4*gb_size_of(bs->vertices[0]), 0);

	gbgl_vert_ptr_aa(0, 2, gbglBasicVertex, x);
	gbgl_vert_ptr_aa(1, 2, gbglBasicVertex, u);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bs->ebo);

	glEnable(GL_BLEND);
	glBlendEquationi(0, GL_FUNC_ADD);
	glBlendFunci(0, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, NULL);
}

gb_inline void gbgl_bs_draw_rect(gbglBasicState *bs, f32 x, f32 y, f32 w, f32 h, gbglColour col) {
	gbgl_bs_draw_quad(bs,
	                  x,   y,
	                  x+w, y,
	                  x+w, y+h,
	                  x,   y+h,
	                  col);
}

gb_inline void gbgl_bs_draw_rect_outline(gbglBasicState *bs, f32 x, f32 y, f32 w, f32 h, gbglColour col, f32 thickness) {
	gbgl_bs_draw_quad_outline(bs,
	                          x,   y,
	                          x+w, y,
	                          x+w, y+h,
	                          x,   y+h,
	                          col,
	                          thickness);
}


gb_internal void gbgl__bs_setup_ortho_colour_state(gbglBasicState *bs, isize vertex_count, gbglColour col) {

	gbgl_use_shader(&bs->ortho_col_shader);
	gbgl_set_uniform_mat4(&bs->ortho_col_shader, "u_ortho_mat", bs->ortho_mat);
	gbgl_set_uniform_colour(&bs->ortho_col_shader, "u_colour", col);

	gbgl_vbo_copy(bs->vbo, bs->vertices, vertex_count*gb_size_of(bs->vertices[0]), 0);
	gbgl_vert_ptr_aa(0, 2, gbglBasicVertex, x);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bs->ebo);

	glEnable(GL_BLEND);
	glBlendEquation(GL_FUNC_ADD);
	glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
}

gb_inline void gbgl_bs_draw_quad(gbglBasicState *bs,
                                 f32 x0, f32 y0,
                                 f32 x1, f32 y1,
                                 f32 x2, f32 y2,
                                 f32 x3, f32 y3,
                                 gbglColour col) {
	bs->vertices[0].x = x0;
	bs->vertices[0].y = y0;

	bs->vertices[1].x = x1;
	bs->vertices[1].y = y1;

	bs->vertices[2].x = x2;
	bs->vertices[2].y = y2;

	bs->vertices[3].x = x3;
	bs->vertices[3].y = y3;

	gbgl__bs_setup_ortho_colour_state(bs, 4, col);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, NULL);
}

gb_inline void gbgl_bs_draw_quad_outline(gbglBasicState *bs,
                                         f32 x0, f32 y0,
                                         f32 x1, f32 y1,
                                         f32 x2, f32 y2,
                                         f32 x3, f32 y3,
                                         gbglColour col, f32 thickness) {
	bs->vertices[0].x = x0;
	bs->vertices[0].y = y0;

	bs->vertices[1].x = x1;
	bs->vertices[1].y = y1;

	bs->vertices[2].x = x2;
	bs->vertices[2].y = y2;

	bs->vertices[3].x = x3;
	bs->vertices[3].y = y3;

	gbgl__bs_setup_ortho_colour_state(bs, 4, col);
	glLineWidth(thickness);
	glDrawArrays(GL_LINE_LOOP, 0, 4);
}

gb_inline void gbgl_bs_draw_line(gbglBasicState *bs, f32 x0, f32 y0, f32 x1, f32 y1, gbglColour col, f32 thickness) {
	bs->vertices[0].x = x0;
	bs->vertices[0].y = y0;

	bs->vertices[1].x = x1;
	bs->vertices[1].y = y1;

	gbgl__bs_setup_ortho_colour_state(bs, 2, col);
	glLineWidth(thickness);
	glDrawArrays(GL_LINES, 0, 2);
}

gb_inline void gbgl_bs_draw_elliptical_arc(gbglBasicState *bs, f32 x, f32 y, f32 radius_a, f32 radius_b,
                                           f32 min_angle, f32 max_angle, gbglColour col) {
	isize i;

	bs->vertices[0].x = x;
	bs->vertices[0].y = y;

	for (i = 0; i < 31; i++) {
		f32 t = cast(f32)i / 30.0f;
		f32 a = gbgl_lerp(min_angle, max_angle, t);
		f32 c = gbgl_cos(a);
		f32 s = gbgl_sin(a);
		bs->vertices[i+1].x = x + c*radius_a;
		bs->vertices[i+1].y = y + s*radius_b;
	}

	gbgl__bs_setup_ortho_colour_state(bs, 32, col);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 32);
}

gb_inline void gbgl_bs_draw_elliptical_arc_outline(gbglBasicState *bs, f32 x, f32 y, f32 radius_a, f32 radius_b,
                                                   f32 min_angle, f32 max_angle, gbglColour col, f32 thickness) {
	isize i;

	for (i = 0; i < 32; i++) {
		f32 t = cast(f32)i / 31.0f;
		f32 a = gbgl_lerp(min_angle, max_angle, t);
		f32 c = gbgl_cos(a);
		f32 s = gbgl_sin(a);
		bs->vertices[i+1].x = x + c*radius_a;
		bs->vertices[i+1].y = y + s*radius_b;
	}

	gbgl__bs_setup_ortho_colour_state(bs, 32, col);
	glLineWidth(thickness);
	glDrawArrays(GL_LINES, 0, 32);
}



gb_inline void gbgl_bs_draw_circle(gbglBasicState *bs, f32 x, f32 y, f32 radius, gbglColour col) {
	gbgl_bs_draw_elliptical_arc(bs, x, y, radius, radius, 0, GBGL_TAU, col);
}

gb_inline void gbgl_bs_draw_circle_outline(gbglBasicState *bs, f32 x, f32 y, f32 radius, gbglColour col, f32 thickness) {
	gbgl_bs_draw_elliptical_arc_outline(bs, x, y, radius, radius, 0, GBGL_TAU, col, thickness);
}

void gbgl_bs_draw_rounded_rect_corners(gbglBasicState *bs, f32 x, f32 y, f32 w, f32 h, f32 roundness, gbglColour col, u32 corners) {
	if ((2.0f*roundness > gbgl_abs(w)) ||
	    (2.0f*roundness > gbgl_abs(h))) {
		roundness = 0.5f*gbgl_min(gbgl_abs(w), gbgl_abs(h));
	}

	if (roundness == 0 || corners == 0) {
		gbgl_bs_draw_rect(bs, x, y, w, h, col);
	} else {
		isize i, vc = 0;

		bs->vertices[vc].x = x + 0.5f*w;
		bs->vertices[vc].y = y + 0.5f*h;
		vc++;

		if (corners & 1) {
			for (i = 0; i < 6; i++) {
				f32 t = cast(f32)i / 5.0f;
				f32 a = gbgl_lerp(0.5f*GBGL_TAU, 0.75f*GBGL_TAU, t);
				f32 c = gbgl_cos(a);
				f32 s = gbgl_sin(a);
				bs->vertices[vc].x = x + roundness + c*roundness;
				bs->vertices[vc].y = y + roundness + s*roundness;
				vc++;
			}
		} else {
			bs->vertices[vc].x = x;
			bs->vertices[vc].y = y;
			vc++;
		}

		if (corners & 2) {
			for (i = 0; i < 6; i++) {
				f32 t = cast(f32)i / 5.0f;
				f32 a = gbgl_lerp(0.75f*GBGL_TAU, 1.00f*GBGL_TAU, t);
				f32 c = gbgl_cos(a);
				f32 s = gbgl_sin(a);
				bs->vertices[vc].x = x + w - roundness + c*roundness;
				bs->vertices[vc].y = y + roundness + s*roundness;
				vc++;
			}
		} else {
			bs->vertices[vc].x = x + w;
			bs->vertices[vc].y = y;
			vc++;
		}


		if (corners & 4) {
			for (i = 0; i < 6; i++) {
				f32 t = cast(f32)i / 5.0f;
				f32 a = gbgl_lerp(0.00f*GBGL_TAU, 0.25f*GBGL_TAU, t);
				f32 c = gbgl_cos(a);
				f32 s = gbgl_sin(a);
				bs->vertices[vc].x = x + w - roundness + c*roundness;
				bs->vertices[vc].y = y + h - roundness + s*roundness;
				vc++;
			}
		} else {
			bs->vertices[vc].x = x + w;
			bs->vertices[vc].y = y + h;
			vc++;
		}

		if (corners & 8) {
			for (i = 0; i < 6; i++) {
				f32 t = cast(f32)i / 5.0f;
				f32 a = gbgl_lerp(0.25f*GBGL_TAU, 0.50f*GBGL_TAU, t);
				f32 c = gbgl_cos(a);
				f32 s = gbgl_sin(a);
				bs->vertices[vc].x = x + roundness + c*roundness;
				bs->vertices[vc].y = y + h - roundness + s*roundness;
				vc++;
			}
		} else {
			bs->vertices[vc].x = x;
			bs->vertices[vc].y = y + h;
			vc++;
		}

		if (corners & 1) {
			bs->vertices[vc].x = x;
			bs->vertices[vc].y = y + roundness;
		} else {
			bs->vertices[vc].x = x;
			bs->vertices[vc].y = y;
		}
		vc++;

		gbgl__bs_setup_ortho_colour_state(bs, vc, col);
		glDrawArrays(GL_TRIANGLE_FAN, 0, vc);
	}
}

gb_inline void gbgl_bs_draw_rounded_rect(gbglBasicState *bs, f32 x, f32 y, f32 w, f32 h, f32 roundness, gbglColour col) {
	gbgl_bs_draw_rounded_rect_corners(bs, x, y, w, h, roundness, col, 1|2|4|8);
}


void gbgl_bs_draw_rounded_rect_corners_outline(gbglBasicState *bs, f32 x, f32 y, f32 w, f32 h, f32 roundness, gbglColour col, f32 thickness, u32 corners) {
	if ((2.0f*roundness > gbgl_abs(w)) ||
	    (2.0f*roundness > gbgl_abs(h))) {
		roundness = 0.5f*gbgl_min(gbgl_abs(w), gbgl_abs(h));
	}

	if (roundness == 0 || corners == 0) {
		gbgl_bs_draw_rect_outline(bs, x, y, w, h, col, thickness);
	} else {
		isize i, vc = 0;

		if (corners & 1) {
			for (i = 0; i < 6; i++) {
				f32 t = cast(f32)i / 5.0f;
				f32 a = gbgl_lerp(0.5f*GBGL_TAU, 0.75f*GBGL_TAU, t);
				f32 c = gbgl_cos(a);
				f32 s = gbgl_sin(a);
				bs->vertices[vc].x = x + roundness + c*roundness;
				bs->vertices[vc].y = y + roundness + s*roundness;
				vc++;
			}
		} else {
			bs->vertices[vc].x = x;
			bs->vertices[vc].y = y;
			vc++;
		}

		if (corners & 2) {
			for (i = 0; i < 6; i++) {
				f32 t = cast(f32)i / 5.0f;
				f32 a = gbgl_lerp(0.75f*GBGL_TAU, 1.00f*GBGL_TAU, t);
				f32 c = gbgl_cos(a);
				f32 s = gbgl_sin(a);
				bs->vertices[vc].x = x + w - roundness + c*roundness;
				bs->vertices[vc].y = y + roundness + s*roundness;
				vc++;
			}
		} else {
			bs->vertices[vc].x = x + w;
			bs->vertices[vc].y = y;
			vc++;
		}


		if (corners & 4) {
			for (i = 0; i < 6; i++) {
				f32 t = cast(f32)i / 5.0f;
				f32 a = gbgl_lerp(0.00f*GBGL_TAU, 0.25f*GBGL_TAU, t);
				f32 c = gbgl_cos(a);
				f32 s = gbgl_sin(a);
				bs->vertices[vc].x = x + w - roundness + c*roundness;
				bs->vertices[vc].y = y + h - roundness + s*roundness;
				vc++;
			}
		} else {
			bs->vertices[vc].x = x + w;
			bs->vertices[vc].y = y + h;
			vc++;
		}

		if (corners & 8) {
			for (i = 0; i < 6; i++) {
				f32 t = cast(f32)i / 5.0f;
				f32 a = gbgl_lerp(0.25f*GBGL_TAU, 0.50f*GBGL_TAU, t);
				f32 c = gbgl_cos(a);
				f32 s = gbgl_sin(a);
				bs->vertices[vc].x = x + roundness + c*roundness;
				bs->vertices[vc].y = y + h - roundness + s*roundness;
				vc++;
			}
		} else {
			bs->vertices[vc].x = x;
			bs->vertices[vc].y = y + h;
			vc++;
		}

		gbgl__bs_setup_ortho_colour_state(bs, vc, col);
		glLineWidth(thickness);
		glDrawArrays(GL_LINE_LOOP, 0, vc);
	}
}

gb_inline void gbgl_bs_draw_rounded_rect_outline(gbglBasicState *bs, f32 x, f32 y, f32 w, f32 h, f32 roundness, gbglColour col, f32 thickness) {
	gbgl_bs_draw_rounded_rect_corners_outline(bs, x, y, w, h, roundness, col, thickness, 1|2|4|8);
}




#if !defined(GBGL_NO_FONTS)


isize gbgl_bs_draw_substring(gbglBasicState *bs, gbglFont *font, f32 x, f32 y, gbglColour col, char const *str, isize len) {
	isize char_count = gb_utf8_strnlen(str, len);
	isize line_count = 0;
	if (char_count > 0) {
		char const *ptr = str;

		f32 sf = 1.0f / cast(f32)font->bitmap_width;
		f32 tf = 1.0f / cast(f32)font->bitmap_height;

		f32 ox, oy;
		f32 px, py;

		isize glyph_count = 0, i;
		f32 font_height = font->size;
		i32 max_width = bs->text_params[gbglTextParam_MaxWidth].val_i32;

		gbglJustifyType justify = cast(gbglJustifyType)bs->text_params[gbglTextParam_Justify].val_i32;
		if (justify == gbglJustify_Centre) {
			f32 width;
			gbgl_get_string_dimensions(font, ptr, &width, NULL);
			x = gbgl_round(x - width*0.5f);
		} else if (justify == gbglJustify_Right) {
			f32 width;
			gbgl_get_string_dimensions(font, ptr, &width, NULL);
			x = gbgl_round(x - width);
		}

		line_count = 1;

		ox = x;
		oy = y;
		px = ox;
		py = oy;

		for (i = 0; i < char_count; i++) {
			char32 cp;
			isize byte_len, curr_index, draw_this_glyph_count = 1, j;
			gbglGlyphInfo *gi;

			byte_len = gb_utf8_decode_len(ptr, len-(ptr-str), &cp);
			ptr += byte_len;
			if (ptr - str > len)
				break;

			if (cp == '\t') {
				draw_this_glyph_count = GBGL_TAB_CHARACTER_WIDTH;
				cp = ' '; // TODO(bill): Set tab to be space
			}


			gi = gbgl_get_glyph_info(font, cp, &curr_index);
			if (!gi) {
				gi = gbgl_get_glyph_info(font, ' ', &curr_index);
			}

			if (gi) {
				for (j = 0; j < draw_this_glyph_count; j++) {
					f32 s0, t0, s1, t1;
					f32 x0, y0, x1, y1;
					f32 kern = 0.0f;


					if (cp == '\r' || cp == '\n' ||
					    (max_width > 0 && px - ox + gi->xadv >= cast(f32)max_width)) {
						px = ox;

						py -= font_height;
						line_count += 2;

						if (cp == '\r' || cp == '\n') {
							if (cp == '\r' && ptr[1] == '\n')
								ptr++;
							continue;
						}
					}

					s0 = cast(f32)gi->s0 * sf;
					t0 = cast(f32)gi->t0 * tf;
					s1 = cast(f32)gi->s1 * sf;
					t1 = cast(f32)gi->t1 * tf;

					x0 = px + gi->xoff;
					y0 = py - gi->yoff;
					x1 = x0 + (gi->s1 - gi->s0);
					y1 = y0 + (gi->t0 - gi->t1);

					bs->font_vertices[glyph_count*4 + 0].x = x0;
					bs->font_vertices[glyph_count*4 + 0].y = y0;
					bs->font_vertices[glyph_count*4 + 0].u = s0;
					bs->font_vertices[glyph_count*4 + 0].v = t0;

					bs->font_vertices[glyph_count*4 + 1].x = x1;
					bs->font_vertices[glyph_count*4 + 1].y = y0;
					bs->font_vertices[glyph_count*4 + 1].u = s1;
					bs->font_vertices[glyph_count*4 + 1].v = t0;

					bs->font_vertices[glyph_count*4 + 2].x = x1;
					bs->font_vertices[glyph_count*4 + 2].y = y1;
					bs->font_vertices[glyph_count*4 + 2].u = s1;
					bs->font_vertices[glyph_count*4 + 2].v = t1;

					bs->font_vertices[glyph_count*4 + 3].x = x0;
					bs->font_vertices[glyph_count*4 + 3].y = y1;
					bs->font_vertices[glyph_count*4 + 3].u = s0;
					bs->font_vertices[glyph_count*4 + 3].v = t1;

					glyph_count++;

					if (i < char_count-1) {
						isize next_index;
						char32 next_cp = 0;
						gbglGlyphInfo *ngi;

						gb_utf8_decode_len(ptr, len-(ptr-str), &next_cp);
						ngi = gbgl_get_glyph_info(font, next_cp, &next_index);
						if (ngi) {
							kern = gbgl_get_font_kerning_from_glyph_indices(font, curr_index, next_index);
						}
					}

					px += gi->xadv + kern;
				}
			}
		}


		if (glyph_count > 0) {
			isize sampler_index = 0;

			gbgl_use_shader(&bs->font_shader);
			gbgl_set_uniform_mat4(&bs->font_shader, "u_ortho_mat", bs->ortho_mat);
			gbgl_set_uniform_colour(&bs->font_shader, "u_colour", col);
			GB_ASSERT(bs->text_params[gbglTextParam_TextureFilter].val_i32 < gb_count_of(bs->font_samplers));
			if (bs->text_params[gbglTextParam_TextureFilter].val_i32 < gb_count_of(bs->font_samplers))
				sampler_index = bs->text_params[gbglTextParam_TextureFilter].val_i32;

			gbgl_bind_texture2d(&font->texture, 0, bs->font_samplers[sampler_index]);
			gbgl_vbo_copy(bs->font_vbo, bs->font_vertices, gb_size_of(bs->font_vertices[0]) * glyph_count * 4, 0);

			gbgl_vert_ptr_aa(0, 2, gbglBasicVertex, x);
			gbgl_vert_ptr_aa(1, 2, gbglBasicVertex, u);

			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bs->font_ebo);

			glEnable(GL_BLEND);
			glBlendEquation(GL_FUNC_ADD);
			glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
			glDrawElements(GL_TRIANGLES, glyph_count*6, GL_UNSIGNED_SHORT, NULL);

		}
	}
	return line_count;
}

isize gbgl_bs_draw_string(gbglBasicState *bs, gbglFont *font, f32 x, f32 y, gbglColour col, char const *fmt, ...) {
	isize len;
	va_list va;
	va_start(va, fmt);
	len = gbgl_bs_draw_string_va(bs, font, x, y, col, fmt, va);
	va_end(va);
	return len;
}

gb_inline isize gbgl_bs_draw_string_va(gbglBasicState *bs, gbglFont *font, f32 x, f32 y, gbglColour col, char const *fmt, va_list va) {
	isize len = gb_snprintf_va(bs->font_text_buffer, gb_size_of(bs->font_text_buffer),
	                           fmt, va);
	isize char_count = gb_utf8_strnlen(bs->font_text_buffer, len);
	if (char_count <= 0)
		return 0;
	return gbgl_bs_draw_substring(bs, font, x, y, col, bs->font_text_buffer, len);
}

#endif // !defined(GBGL_NO_FONTS)
#endif // !defined(GBGL_NO_BASIC_STATE)

#endif
