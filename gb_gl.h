/* gb.h - v0.01  - OpenGL Helper Library - public domain
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

	NOTE

	This library REQUIRES "stb_image.h" for loading images from file
		- You may need change the path
	This library REQUIRES "gb.h" at this moment in time.
	This library REQUIRES "gb_math.h" at this moment in time.

===========================================================================

Conventions used:
	gbglTypesAreLikeThis (None core types)
	gbgl_functions_and_variables_like_this
	Prefer // Comments
	Never use _t suffix for types (I think they are stupid...)


Version History:
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

#ifndef GB_MATH_IMPLEMENTATION
#include "gb_math.h"
#endif


#ifndef STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#endif


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


GBGL_DEF u32 gbgl_generate_sampler(u32 min_filter, u32 max_filter, u32 s_wrap, u32 t_wrap);




////////////////////////////////////////////////////////////////
//
// Data Buffers
//
//


typedef enum gbglBufferDataType {
	GBGL_BDT_U8 = GL_UNSIGNED_BYTE,
	GBGL_BDT_I8 = GL_BYTE,

	GBGL_BDT_U16 = GL_UNSIGNED_SHORT,
	GBGL_BDT_I16 = GL_SHORT,
	GBGL_BDT_F16 = GL_HALF_FLOAT,

	GBGL_BDT_U32 = GL_UNSIGNED_INT,
	GBGL_BDT_I32 = GL_INT,
	GBGL_BDT_F32 = GL_FLOAT,

	GBGL_BDT_F8,	// NOTE(bill): This is not a "real" OpenGL type but it is needed for internal format enums
} gbglBufferDataType;


// NOTE(bill) index+1 = channels count
#if defined(GBGL_USE_SRGB_TEXTURE_FORMAT)
i32 const GBGL_TEXTURE_FORMAT[4] = { GL_RED, GL_RG, GL_SRGB8, GL_SRGB8_ALPHA8 };
#else
i32 const GBGL_TEXTURE_FORMAT[4] = { GL_RED, GL_RG, GL_RGB, GL_RGBA };
#endif

i32 const GBGL_INTERNAL_TEXTURE_FORMAT_8[4]  = { GL_R8,   GL_RG8,   GL_RGB8,	  GL_RGBA8   };
i32 const GBGL_INTERNAL_TEXTURE_FORMAT_16[4] = { GL_R16,  GL_RG16,  GL_RGB16,  GL_RGBA16  };
i32 const GBGL_INTERNAL_TEXTURE_FORMAT_32[4] = { GL_R32F, GL_RG32F, GL_RGB32F, GL_RGBA32F };

i32 const GBGL_INTERNAL_TEXTURE_FORMAT_U8[4] = { GL_R8UI, GL_RG8UI, GL_RGB8UI, GL_RGB8UI };
i32 const GBGL_INTERNAL_TEXTURE_FORMAT_I8[4] = { GL_R8I,  GL_RG8I,  GL_RGB8I,  GL_RGB8I  };
i32 const GBGL_INTERNAL_TEXTURE_FORMAT_F8[4] = { GL_R8,   GL_RG8,   GL_RGB8,   GL_RGB8   };

i32 const GBGL_INTERNAL_TEXTURE_FORMAT_U16[4] = { GL_R16UI, GL_RG16UI, GL_RGB16UI, GL_RGB16UI };
i32 const GBGL_INTERNAL_TEXTURE_FORMAT_I16[4] = { GL_R16I,  GL_RG16I,  GL_RGB16I,  GL_RGB16I  };
i32 const GBGL_INTERNAL_TEXTURE_FORMAT_F16[4] = { GL_R16F,  GL_RG16F,  GL_RGB16F,  GL_RGB16F  };

i32 const GBGL_INTERNAL_TEXTURE_FORMAT_U32[4] = { GL_R32UI, GL_RG32UI, GL_RGB32UI, GL_RGBA32UI };
i32 const GBGL_INTERNAL_TEXTURE_FORMAT_I32[4] = { GL_R32I,  GL_RG32I,  GL_RGB32I,  GL_RGBA32I  };
i32 const GBGL_INTERNAL_TEXTURE_FORMAT_F32[4] = { GL_R32F,  GL_RG32F,  GL_RGB32F,  GL_RGBA32F  };

gb_inline i32
gbgl__get_texture_format(gbglBufferDataType data_type, i32 channel_count)
{
	GB_ASSERT(channel_count >= 1 && channel_count <= 4);
	switch (data_type) {
	case GBGL_BDT_U8:  return GBGL_INTERNAL_TEXTURE_FORMAT_U8[channel_count-1];
	case GBGL_BDT_I8:  return GBGL_INTERNAL_TEXTURE_FORMAT_I8[channel_count-1];
	case GBGL_BDT_F8:  return GBGL_INTERNAL_TEXTURE_FORMAT_F8[channel_count-1];
	case GBGL_BDT_U16: return GBGL_INTERNAL_TEXTURE_FORMAT_U16[channel_count-1];
	case GBGL_BDT_I16: return GBGL_INTERNAL_TEXTURE_FORMAT_I16[channel_count-1];
	case GBGL_BDT_F16: return GBGL_INTERNAL_TEXTURE_FORMAT_F16[channel_count-1];
	case GBGL_BDT_U32: return GBGL_INTERNAL_TEXTURE_FORMAT_U32[channel_count-1];
	case GBGL_BDT_I32: return GBGL_INTERNAL_TEXTURE_FORMAT_I32[channel_count-1];
	case GBGL_BDT_F32: return GBGL_INTERNAL_TEXTURE_FORMAT_F32[channel_count-1];
	}
	return GBGL_INTERNAL_TEXTURE_FORMAT_F32[4-1];
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
	GBGL_SHADER_TYPE_VERTEX,
	GBGL_SHADER_TYPE_FRAGMENT,
	GBGL_SHADER_TYPE_GEOMETRY,

	GBGL_SHADER_TYPE_COUNT,
} gbglShaderType;

i32 const GBGL_SHADER_TYPE[GBGL_SHADER_TYPE_COUNT] = {
	GL_VERTEX_SHADER,   /* GBGL_SHADER_TYPE_VERTEX   */
	GL_FRAGMENT_SHADER, /* GBGL_SHADER_TYPE_FRAGMENT */
	GL_GEOMETRY_SHADER, /* GBGL_SHADER_TYPE_GEOMETRY */
};

typedef enum gbglShaderError {
	GBGL_SHADER_ERROR_NONE,
	GBGL_SHADER_ERROR_SHADER_COMPILE,
	GBGL_SHADER_ERROR_LINKING,
	GBGL_SHADER_ERROR_UNABLE_TO_READ_FILE,

	GBGL_SHADER_ERROR_COUNT,
} gbglShaderError;

#ifndef GBGL_MAX_UNIFORM_COUNT
#define GBGL_MAX_UNIFORM_COUNT 32
#endif

typedef struct gbglShader {
	u32 shaders[GBGL_SHADER_TYPE_COUNT];
	u32 program;

	i32 uniform_locs[GBGL_MAX_UNIFORM_COUNT];
	char *uniform_names[GBGL_MAX_UNIFORM_COUNT];
	i32 uniform_count;

	u32 type_flags;

	gbFile files[GBGL_SHADER_TYPE_COUNT];

	char base_name[64];
} gbglShader;



#ifndef GBGL_SHADER_FILE_EXTENSIONS_DEFINED
#define GBGL_SHADER_FILE_EXTENSIONS_DEFINED
gb_global char const *GBGL_SHADER_FILE_EXTENSIONS[GBGL_SHADER_TYPE_COUNT] = {".vs", ".fs", ".gs"};
#endif


GBGL_DEF gbglShaderError gbgl_load_shader_from_file      (gbglShader *s, u32 type_bits, char const *filename, ...);
GBGL_DEF gbglShaderError gbgl_load_shader_vf_from_source (gbglShader *s, char const *vert_source, char const *frag_source);
GBGL_DEF gbglShaderError gbgl_load_shader_vfg_from_source(gbglShader *s, char const *vert_source, char const *frag_source, char const *geom_source);

GBGL_DEF void gbgl_destroy_shader(gbglShader *shader);
GBGL_DEF b32  gbgl_has_shader_changed(gbglShader *shader);
GBGL_DEF b32  gbgl_reload_shader(gbglShader *shader); // TODO(bill): Return an error code?
GBGL_DEF void gbgl_use_shader(gbglShader *shader);
GBGL_DEF b32  gbgl_is_shader_in_use(gbglShader *shader);

GBGL_DEF i32 gbgl_get_uniform(gbglShader *shader, char const *name);

#ifndef GBGL_UNIFORM_SET
#define GBGL_UNIFORM_SET

#define gbgl_set_uniform_int(loc, i)                 glUniform1i(loc, i)
#define gbgl_set_uniform_float(loc, f)               glUniform1f(loc, f)
#define gbgl_set_uniform_vec2(loc, v)                glUniform2fv(loc, 1, &v.x)
#define gbgl_set_uniform_vec3(loc, v)                glUniform3fv(loc, 1, &v.x)
#define gbgl_set_uniform_vec4(loc, v)                glUniform4fv(loc, 1, &v.x)
#define gbgl_set_uniform_mat4(loc, mat)              glUniformMatrix4fv(loc, 1, false, mat)
#define gbgl_set_uniform_mat4_count(loc, mat, count) glUniformMatrix4fv(loc, count, false, mat)

#endif

////////////////////////////////////////////////////////////////
//
// Texture
//
//

typedef enum gbglTextureType {
	GBGL_TEXTURE_TYPE_2D,
	GBGL_TEXTURE_TYPE_CUBE_MAP,

	GBGL_TEXTURE_TYPE_COUNT,
} gbglTextureType;

gb_global i32 const GBGL_TEXTURE_TYPE[GBGL_TEXTURE_TYPE_COUNT] = {
	GL_TEXTURE_2D,       /* GBGL_TEXTURE_TYPE_2D */
	GL_TEXTURE_CUBE_MAP, /* GBGL_TEXTURE_TYPE_CUBE_MAP */
};


typedef struct gbglTexture {
	i32 width, height;
	i32 channel_count;
	gbglBufferDataType data_type;
	gbglTextureType type;
	u32 handle;
} gbglTexture;

GBGL_DEF b32 gbgl_load_texture2d_from_file(gbglTexture *texture, b32 flip_vertically, char const *filename, ...);
GBGL_DEF b32 gbgl_load_texture2d_from_memory(gbglTexture *texture, void const *data, i32 width, i32 height, i32 channel_count);
GBGL_DEF b32 gbgl_init_texture2d_coloured(gbglTexture *texture, gbColour colour);
GBGL_DEF void gbgl_destroy_texture(gbglTexture *texture);

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
gb_global u32 const GBGL_COLOUR_BUFFER_ATTACHMENTS[GBGL_MAX_RENDER_COLOUR_BUFFERS] = {
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


GBGL_DEF b32  gbgl_init_render_buffer(gbglRenderBuffer *rb, i32 width, i32 height, i32 channel_count);
GBGL_DEF void gbgl_destroy_render_buffer(gbglRenderBuffer *rb);
GBGL_DEF void gbgl_render_to_buffer(gbglRenderBuffer const *rb);
GBGL_DEF void gbgl_render_to_screen(i32 width, i32 height);

////////////////////////////////////////////////////////////////
//
// Basic State
//
//


typedef struct gbglBasicVertex {
	f32 x, y;
	f32 u, v;
} gbglBasicVertex;


#ifndef GBGL_BS_MAX_VERTEX_COUNT
#define GBGL_BS_MAX_VERTEX_COUNT 32
#endif


#ifndef GBGL_BS_MAX_INDEX_COUNT
#define GBGL_BS_MAX_INDEX_COUNT 6
#endif

typedef struct gbglBasicState {
	gbglBasicVertex vertices[GBGL_BS_MAX_VERTEX_COUNT];
	u16 indices[GBGL_BS_MAX_INDEX_COUNT];

	u32 vao, vbo, ebo;
	u32 nearest_sampler;
	u32 linear_sampler;
	gbglShader ortho_tex_shader;
	gbglShader ortho_col_shader;

	gbMat4 ortho_mat;
	i32 width, height;

} gbglBasicState;


GBGL_DEF void gbgl_bs_init(gbglBasicState *bs, i32 window_width, i32 window_height);
GBGL_DEF void gbgl_bs_set_resolution(gbglBasicState *bs, i32 window_width, i32 window_height);
GBGL_DEF void gbgl_bs_begin(gbglBasicState *bs, i32 window_width, i32 window_height);
GBGL_DEF void gbgl_bs_end(gbglBasicState *bs);

GBGL_DEF void gbgl_bs_draw_textured_rect(gbglBasicState *bs, gbglTexture *tex, gbVec2 pos, gbVec2 dim, b32 v_up);
GBGL_DEF void gbgl_bs_draw_rect(gbglBasicState *bs, gbVec2 pos, gbVec2 dim, gbColour col);
GBGL_DEF void gbgl_bs_draw_outlined_rect(gbglBasicState *bs, gbVec2 pos, gbVec2 dim, gbColour col, f32 thickness);
GBGL_DEF void gbgl_bs_draw_quad(gbglBasicState *bs,
                                   gbVec2 p0, gbVec2 p1, gbVec2 p2, gbVec2 p3,
                                   gbColour col);
GBGL_DEF void gbgl_bs_draw_outlined_quad(gbglBasicState *bs,
                                            gbVec2 p0, gbVec2 p1, gbVec2 p2, gbVec2 p3,
                                            gbColour col, f32 thickness);

GBGL_DEF void gbgl_bs_draw_line(gbglBasicState *bs, gbVec2 p0, gbVec2 p1, gbColour col, f32 thickness);
GBGL_DEF void gbgl_bs_draw_circle(gbglBasicState *bs, gbVec2 p, f32 radius, gbColour col);
GBGL_DEF void gbgl_bs_draw_outlined_circle(gbglBasicState *bs, gbVec2 p, f32 radius, gbColour col, f32 thickness);


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


u32
gbgl_generate_sampler(u32 min_filter, u32 max_filter, u32 s_wrap, u32 t_wrap)
{
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

gb_inline u32
gbgl__make_buffer(isize size, void const *data, i32 target, i32 usage_hint)
{
	u32 buffer_handle;
	glGenBuffers(1, &buffer_handle);
	glBindBuffer(target, buffer_handle);
	glBufferData(target, size, data, usage_hint);
	return buffer_handle;
}

gb_inline void
gbgl__buffer_copy(u32 buffer_handle, i32 target, void const *data, isize size, isize offset)
{
	glBindBuffer(target, buffer_handle);
	glBufferSubData(target, offset, size, data);
}

// NOTE(bill): usage_hint == (GL_STATIC_DRAW, GL_STREAM_DRAW, GL_DYNAMIC_DRAW)
gb_inline u32
gbgl_make_vbo(void const *data, isize size, i32 usage_hint)
{
	return gbgl__make_buffer(size, data, GL_ARRAY_BUFFER, usage_hint);
}

gb_inline u32
gbgl_make_ebo(void const *data, isize size, i32 usage_hint)
{
	return gbgl__make_buffer(size, data, GL_ELEMENT_ARRAY_BUFFER, usage_hint);
}

gb_inline gbglTBO
gbgl_make_tbo(gbglBufferDataType data_type, i32 channel_count, void const *data, isize size, i32 usage_hint)
{
	gbglTBO tbo;
	i32 internal_format;

	tbo.buffer_obj_handle = gbgl__make_buffer(size, data, GL_TEXTURE_BUFFER, usage_hint);

	glGenTextures(1, &tbo.buffer_handle);
	glBindTexture(GL_TEXTURE_BUFFER, tbo.buffer_handle);
	internal_format = gbgl__get_texture_format(data_type, channel_count);
	glTexBuffer(GL_TEXTURE_BUFFER, internal_format, tbo.buffer_obj_handle);
	return tbo;
}

gb_inline void
gbgl_vbo_copy(u32 vbo_handle, void *const data, isize size, isize offset)
{
	gbgl__buffer_copy(vbo_handle, GL_ARRAY_BUFFER, data, size, offset);
}

gb_inline void
gbgl_ebo_copy(u32 ebo_handle, void *const data, isize size, isize offset)
{
	gbgl__buffer_copy(ebo_handle, GL_ELEMENT_ARRAY_BUFFER, data, size, offset);
}

gb_inline void
gbgl_tbo_copy(gbglTBO tbo, void *const data, isize size, isize offset)
{
	gbgl__buffer_copy(tbo.buffer_obj_handle, GL_TEXTURE_BUFFER, data, size, offset);
}

gb_inline void gbgl_bind_vbo(u32 vbo_handle) { glBindBuffer(GL_ARRAY_BUFFER, vbo_handle); }
gb_inline void gbgl_bind_ebo(u32 ebo_handle) { glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_handle); }

gb_inline void
gbgl_bind_tbo(gbglTBO tbo, i32 sampler_handle, i32 tex_unit)
{
	glActiveTexture(GL_TEXTURE0 + tex_unit);
	glBindTexture(GL_TEXTURE_BUFFER, tbo.buffer_handle);
	glBindSampler(0, sampler_handle);
}

// NOTE(bill): access = GL_WRITE_ONLY, etc.
gb_inline void *
gbgl_map_vbo(u32 vbo_handle, i32 access)
{
	gbgl_bind_vbo(vbo_handle);
	return glMapBuffer(GL_ARRAY_BUFFER, access);
}

gb_inline void *
gbgl_map_ebo(u32 ebo_handle, i32 access)
{
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


gbglShaderError
gbgl__load_single_shader_from_file(gbglShader *shader, gbglShaderType type, char const *name)
{
	gbglShaderError err = GBGL_SHADER_ERROR_NONE;

	if (!gb_open_file(&shader->files[type], "%s%s", name, GBGL_SHADER_FILE_EXTENSIONS[type])) {
		err = GBGL_SHADER_ERROR_UNABLE_TO_READ_FILE;
	} else {
		gb_local_persist char info_log[4096];
		i64 file_size = gb_file_size(&shader->files[type]);
		char *file_source = cast(char *)gbgl_malloc(file_size+1);
		// TODO(bill): LOG MALLOC USAGE


		GB_ASSERT_NOT_NULL(file_source);
		if (file_source) {
			i32 params;

			gb_file_read_at(&shader->files[type], file_source, file_size, 0);
			file_source[file_size] = '\0';

			shader->shaders[type] = glCreateShader(GBGL_SHADER_TYPE[type]);
			glShaderSource(shader->shaders[type], 1, &file_source, NULL);
			glCompileShader(shader->shaders[type]);
			glGetShaderiv(shader->shaders[type], GL_COMPILE_STATUS, &params);
			if (!params) {
				gb_fprintf(stderr, "Shader Source:\n%s\n", file_source);
				glGetShaderInfoLog(shader->shaders[type], gb_size_of(info_log), NULL, info_log);
				gb_fprintf(stderr, "Shader compilation failed:\n %s\n", info_log);

				err = GBGL_SHADER_ERROR_SHADER_COMPILE;
			}

			gbgl_free(file_source);
		}
		gb_close_file(&shader->files[type]);
	}

	return err;
}

gbglShaderError
gbgl__load_single_shader_from_source(gbglShader *s, gbglShaderType type, char const *text)
{
	gbglShaderError err = GBGL_SHADER_ERROR_NONE;
	i32 status;

	s->shaders[type] = glCreateShader(GBGL_SHADER_TYPE[type]);
	glShaderSource(s->shaders[type], 1, &text, 0);
	glCompileShader(s->shaders[type]);

	glGetShaderiv(s->shaders[type], GL_COMPILE_STATUS, &status);
	if (!status) {
		gb_local_persist char log_info[4096];
		i32 total_len, log_len;

		gb_fprintf(stderr, "Unable to compile shader: %s\n", text);
		glGetShaderiv(s->shaders[type], GL_INFO_LOG_LENGTH, &status);
		total_len = status;

		glGetShaderInfoLog(s->shaders[type], 4095, &log_len, log_info);
		gb_fprintf(stderr, log_info);
		err = GBGL_SHADER_ERROR_SHADER_COMPILE;
	}

	return err;
}

gbglShaderError
gbgl__link_shader(gbglShader *shader)
{
	gbglShaderError err = GBGL_SHADER_ERROR_NONE;
	i32 i, status;
	shader->program = glCreateProgram();
	for (i = 0; i < GBGL_SHADER_TYPE_COUNT; i++) {
		if (shader->type_flags & GB_BIT(i))
			glAttachShader(shader->program, shader->shaders[i]);
	}

	glLinkProgram(shader->program);

	glGetProgramiv(shader->program, GL_LINK_STATUS, &status);
	if (!status) {
		gb_local_persist char log_info[4096];
		glGetProgramInfoLog(shader->program, gb_size_of(log_info), NULL, log_info);
		gb_fprintf(stderr, "Shader linking failed:\n %s \n", log_info);
		err = GBGL_SHADER_ERROR_LINKING;
	}

	for (i = 0; i < GBGL_SHADER_TYPE_COUNT; i++) {
		if (shader->type_flags & GB_BIT(i))
			glDetachShader(shader->program, shader->shaders[i]);
	}

	return err;
}



gbglShaderError
gbgl_load_shader_from_file(gbglShader *shader, u32 type_bits, char const *filename, ...)
{
	gbglShaderError err = GBGL_SHADER_ERROR_NONE;
	b32 loaded_shader[GBGL_SHADER_TYPE_COUNT] = {0};
	i32 i;

	gb_zero_struct(shader);
	shader->type_flags = type_bits;
	gb_strncpy(shader->base_name, filename, gb_size_of(shader->base_name));

	for (i = 0; i < GBGL_SHADER_TYPE_COUNT; i++) {
		if (type_bits & GB_BIT(i)) {
			err = gbgl__load_single_shader_from_file(shader, cast(gbglShaderType)i, filename);
			if (err != GBGL_SHADER_ERROR_NONE)
				return err;
			loaded_shader[i] = true;
		}
	}
	err = gbgl__link_shader(shader);

	return err;
}




gbglShaderError
gbgl_load_shader_vf_from_source(gbglShader *s, char const *vert_source, char const *frag_source)
{
	gbglShaderError err = GBGL_SHADER_ERROR_NONE;

	gb_zero_struct(s);
	s->type_flags = GB_BIT(GBGL_SHADER_TYPE_VERTEX) | GB_BIT(GBGL_SHADER_TYPE_FRAGMENT);

	err = gbgl__load_single_shader_from_source(s, GBGL_SHADER_TYPE_VERTEX, vert_source);
	if (err != GBGL_SHADER_ERROR_NONE) return err;
	err = gbgl__load_single_shader_from_source(s, GBGL_SHADER_TYPE_FRAGMENT, frag_source);
	if (err != GBGL_SHADER_ERROR_NONE) return err;

	err = gbgl__link_shader(s);

	return err;
}

gbglShaderError
gbgl_load_shader_vfg_from_source(gbglShader *s, char const *vert_source, char const *frag_source, char const *geom_source)
{
	gbglShaderError err = GBGL_SHADER_ERROR_NONE;

	gb_zero_struct(s);
	s->type_flags = GB_BIT(GBGL_SHADER_TYPE_VERTEX) | GB_BIT(GBGL_SHADER_TYPE_FRAGMENT) | GB_BIT(GBGL_SHADER_TYPE_GEOMETRY);

	err = gbgl__load_single_shader_from_source(s, GBGL_SHADER_TYPE_VERTEX, vert_source);
	if (err != GBGL_SHADER_ERROR_NONE) return err;
	err = gbgl__load_single_shader_from_source(s, GBGL_SHADER_TYPE_FRAGMENT, frag_source);
	if (err != GBGL_SHADER_ERROR_NONE) return err;
	err = gbgl__load_single_shader_from_source(s, GBGL_SHADER_TYPE_GEOMETRY, geom_source);
	if (err != GBGL_SHADER_ERROR_NONE) return err;

	err = gbgl__link_shader(s);

	return err;
}

gb_inline void
gbgl_destroy_shader(gbglShader *shader)
{
	i32 i;
	for (i = 0; i < GBGL_SHADER_TYPE_COUNT; i++) {
		if (shader->type_flags & GB_BIT(i)) {
			gb_close_file(&shader->files[i]);
			glDeleteShader(shader->shaders[i]);
		}
	}

	glDeleteProgram(shader->program);

	for (i = 0; i < shader->uniform_count; i++) {
		gbgl_free(shader->uniform_names[i]);
	}
}


gb_inline b32
gbgl_has_shader_changed(gbglShader *shader)
{
	i32 i;
	for (i = 0; i < GBGL_SHADER_TYPE_COUNT; i++) {
		if (shader->type_flags & GB_BIT(i)) {
			if (gb_has_file_changed(&shader->files[i])) {
				return true;
			}
		}
	}
	return false;
}


b32
gbgl_reload_shader(gbglShader *shader)
{
	i32 i;
	for (i = 0; i < GBGL_SHADER_TYPE_COUNT; i++) {
		if (shader->type_flags & GB_BIT(i)) {
			if (gbgl__load_single_shader_from_file(shader, cast(gbglShaderType)i, shader->base_name) != GBGL_SHADER_ERROR_NONE)
				return false;
		}
	}

	if (gbgl__link_shader(shader) != GBGL_SHADER_ERROR_NONE)
		return false;

	for (i = 0; i < shader->uniform_count; i++)
		shader->uniform_locs[i] = glGetUniformLocation(shader->program, shader->uniform_names[i]);


	return true;
}

gb_inline void gbgl_use_shader(gbglShader *s) { glUseProgram(s ? s->program : 0); }

gb_inline b32
gbgl_is_shader_in_use(gbglShader *s)
{
	if (s) {
		i32 curr = 0;
		glGetIntegerv(GL_CURRENT_PROGRAM, &curr);
		return (curr == cast(i32)s->program);
	}
	return false;
}


i32
gbgl_get_uniform(gbglShader *s, char const *name)
{
	i32 i, loc = -1;
	for (i = 0; i < s->uniform_count; i++) {
		if (gb_strcmp(s->uniform_names[i], name) == 0) {
			return s->uniform_locs[i];
		}
	}

	GB_ASSERT_MSG(s->uniform_count < GBGL_MAX_UNIFORM_COUNT,
	              "Uniform array for shader is full");

	loc = glGetUniformLocation(s->program, name);
	s->uniform_names[s->uniform_count] = gb_alloc_cstring(gb_heap_allocator(), name);
	s->uniform_locs[s->uniform_count] = loc;
	s->uniform_count++;

	return loc;
}


////////////////////////////////////////////////////////////////
//
// Render Buffer
//
//

#if 0
b32
gbgl_render_buffer_init(gbglRenderBuffer *in_out_rb)
{
	i32 i;
	if (in_out_rb->colour_buffer_count >= GBGL_MAX_RENDER_COLOUR_BUFFERS) {
		return false;
	}
	glGenFramebuffers(1, &in_out_rb->gl_frame_buffer_handle);
	glBindFramebuffer(GL_FRAMEBUFFER, in_out_rb->gl_frame_buffer_handle);

	glGenTextures(in_out_rb->colour_buffer_count, in_out_rb->handles);
	for (i = 0; i < in_out_rb->colour_buffer_count; i++) {
		i32 channel_count = in_out_rb->channel_count[i];
		glBindTexture(GL_TEXTURE_2D, in_out_rb->handles[i]);
		glTexImage2D(GL_TEXTURE_2D, 0,
		             GBGL_INTERNAL_TEXTURE_FORMAT_8[channel_count-1],
		             in_out_rb->width, in_out_rb->height, 0,
		             GBGL_TEXTURE_FORMAT[channel_count-1],
		             in_out_rb->data_type[0], 0);
		glBindTexture(GL_TEXTURE_2D, 0);
		glFramebufferTexture2D(GL_FRAMEBUFFER,
		                       GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D,
		                       in_out_rb->handles[0], 0);
	}
	glDrawBuffers(in_out_rb->colour_buffer_count, GBGL_COLOUR_BUFFER_ATTACHMENTS);
	//@TODO: not every valid permutation has been tested, it's likely that there's a format/internal format mismatch somewhere
	if (in_out_rb->has_depth || in_out_rb->has_stencil) {
		if (in_out_rb->has_depth && in_out_rb->has_stencil) {
			glGenRenderbuffers(1, &in_out_rb->gl_depth_stencil_buffer_handle);
			glBindRenderbuffer(GL_RENDERBUFFER, in_out_rb->gl_depth_stencil_buffer_handle);
			glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, in_out_rb->width, in_out_rb->height);
			glBindRenderbuffer(GL_RENDERBUFFER, 0);
			glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, in_out_rb->gl_depth_stencil_buffer_handle);
		} else if (in_out_rb->has_depth) {
			glGenRenderbuffers(1, &in_out_rb->gl_depth_stencil_buffer_handle);
			glBindRenderbuffer(GL_RENDERBUFFER, in_out_rb->gl_depth_stencil_buffer_handle);
			glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, in_out_rb->width, in_out_rb->height);
			glBindRenderbuffer(GL_RENDERBUFFER, 0);
			glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, in_out_rb->gl_depth_stencil_buffer_handle);
		} else if (in_out_rb->has_stencil) {
			GB_PANIC("A framebuffer cannot have a stencil without depth"); // NOTE(bill): no stencil w/o depth
		}
	}

	in_out_rb->gl_frame_buffer_status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

	if (in_out_rb->gl_frame_buffer_status != GL_FRAMEBUFFER_COMPLETE) {
		gb_fprintf(stderr, "Unable to create OpenGL Frame buffer. Frame buffer incomplete: %d\n", in_out_rb->gl_frame_buffer_status);
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	return in_out_rb->gl_frame_buffer_status == GL_FRAMEBUFFER_COMPLETE;
}
#endif

b32
gbgl_init_render_buffer(gbglRenderBuffer *rb, i32 width, i32 height, i32 channel_count)
{
	if ((rb->width == width) && (rb->height == height) && (rb->channel_count == channel_count)) return true;
	gbgl_destroy_render_buffer(rb);
	gb_zero_struct(rb);

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
			gb_fprintf(stderr, "Framebuffer Status: 0x%x\n", status);
			return false;
		}
	}

	return true;
}

gb_inline void
gbgl_destroy_render_buffer(gbglRenderBuffer *rb)
{
	if (rb->handle)
		glDeleteFramebuffers(1, &rb->handle);

	gbgl_destroy_texture(&rb->colour_texture);
}


gb_inline void
gbgl_render_to_buffer(gbglRenderBuffer const *rb)
{
	GB_ASSERT_NOT_NULL(rb);
	glViewport(0, 0, rb->width, rb->height);
	glBindFramebuffer(GL_FRAMEBUFFER, rb->handle);
}

gb_inline void
gbgl_render_to_screen(i32 width, i32 height)
{
	glViewport(0, 0, width, height);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}


////////////////////////////////////////////////////////////////
//
// Texture
//
//


b32
gbgl_load_texture2d_from_memory(gbglTexture *tex, void const *data, i32 width, i32 height, i32 channel_count)
{
	b32 result = true;

	gb_zero_struct(tex);

	tex->width = width;
	tex->height = height;
	tex->channel_count = channel_count;
	tex->data_type = GBGL_BDT_U8;
	tex->type = GBGL_TEXTURE_TYPE_2D;

	glGenTextures(1, &tex->handle);
	glBindTexture(GL_TEXTURE_2D, tex->handle);


	glTexImage2D(GL_TEXTURE_2D, 0,
	             GBGL_INTERNAL_TEXTURE_FORMAT_8[channel_count-1],
	             width, height, 0,
	             GBGL_TEXTURE_FORMAT[channel_count-1],
	             GL_UNSIGNED_BYTE, data);

	glBindTexture(GL_TEXTURE_2D, 0);
	glGenerateMipmap(GL_TEXTURE_2D);
	glFinish();


	return result;
}

b32
gbgl_load_texture2d_from_file(gbglTexture *texture, b32 flip_vertically, char const *filename, ...)
{
	b32 result;
	u8 *data;
	int width, height, comp;
	char *path;

	va_list va;
	va_start(va, filename);
	path = gb_sprintf_va(filename, va);
	va_end(va);

	stbi_set_flip_vertically_on_load(flip_vertically);
	data = stbi_load(path, &width, &height, &comp, 0);
	if (data == NULL) {
		gb_fprintf(stderr, "Failed to load image: %s\n", path);
		result = false;
	} else {
		result = gbgl_load_texture2d_from_memory(texture, data, width, height, comp);
		stbi_image_free(data);
	}
	return result;
}

gb_inline b32
gbgl_make_texture2d_coloured(gbglTexture *t, gbColour colour)
{
	return gbgl_load_texture2d_from_memory(t, &colour.rgba, 1, 1, 4);
}


void
gbgl_bind_texture2d(gbglTexture const *t, u32 position, u32 sampler)
{
	GB_ASSERT(t->type == GBGL_TEXTURE_TYPE_2D);

	if (position > 31) {
		position = 31;
		gb_fprintf(stderr, "Textures can only bound to position [0 ... 31]\n");
		gb_fprintf(stderr, "Will bind to position [31]\n");
	}

	glActiveTexture(GL_TEXTURE0 + position);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, t ? t->handle : 0);
	glBindSampler(position, sampler);
}

gb_inline void
gbgl_destroy_texture(gbglTexture *t)
{
	if (t->handle) {
		glDeleteTextures(1, &t->handle);
	}
}




////////////////////////////////////////////////////////////////
//
// Basic State
//
//



void
gbgl_bs_init(gbglBasicState *bs, i32 window_width, i32 window_height)
{
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

	bs->nearest_sampler = gbgl_generate_sampler(GL_NEAREST, GL_NEAREST, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
	bs->linear_sampler  = gbgl_generate_sampler(GL_LINEAR,  GL_LINEAR,  GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);

	gbgl_load_shader_vf_from_source(&bs->ortho_tex_shader,
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
		"}\n");

	gbgl_load_shader_vf_from_source(&bs->ortho_col_shader,
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
		"}\n");
}

void
gbgl_bs_set_resolution(gbglBasicState *bs, i32 window_width, i32 window_height)
{
	bs->width  = window_width;
	bs->height = window_height;
	gb_mat4_ortho2d(&bs->ortho_mat, 0, bs->width, 0, bs->height);
}

void
gbgl_bs_begin(gbglBasicState *bs, i32 window_width, i32 window_height)
{
	glBindVertexArray(bs->vao);
	glDisable(GL_SCISSOR_TEST);
	glDisable(GL_CULL_FACE);
	gbgl_bs_set_resolution(bs, window_width, window_height);
}

void
gbgl_bs_end(gbglBasicState *bs)
{
	glBindVertexArray(0);
}




void
gbgl_bs_draw_textured_rect(gbglBasicState *bs, gbglTexture *tex, gbVec2 pos, gbVec2 dim, b32 v_up)
{
	bs->vertices[0].x = pos.x;
	bs->vertices[0].y = pos.y;
	bs->vertices[0].u = 0.0f;
	bs->vertices[0].v = v_up ? 0.0f : 1.0f;

	bs->vertices[1].x = pos.x + dim.x;
	bs->vertices[1].y = pos.y;
	bs->vertices[1].u = 1.0f;
	bs->vertices[1].v = v_up ? 0.0f : 1.0f;

	bs->vertices[2].x = pos.x + dim.x;
	bs->vertices[2].y = pos.y + dim.y;
	bs->vertices[2].u = 1.0f;
	bs->vertices[2].v = v_up ? 1.0f : 0.0f;

	bs->vertices[3].x = pos.x;
	bs->vertices[3].y = pos.y + dim.y;
	bs->vertices[3].u = 0.0f;
	bs->vertices[3].v = v_up ? 1.0f : 0.0f;

	gbgl_use_shader(&bs->ortho_tex_shader);
	gbgl_set_uniform_mat4(gbgl_get_uniform(&bs->ortho_tex_shader, "u_ortho_mat"), bs->ortho_mat.e);
	gbgl_bind_texture2d(tex, 0, bs->nearest_sampler);

	gbgl_vbo_copy(bs->vbo, bs->vertices, 4*gb_size_of(bs->vertices[0]), 0);

	gbgl_vert_ptr_aa(0, 2, gbglBasicVertex, x);
	gbgl_vert_ptr_aa(1, 2, gbglBasicVertex, u);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bs->ebo);

	glEnable(GL_BLEND);
	glBlendEquationi(0, GL_FUNC_ADD);
	glBlendFunci(0, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, NULL);
}

void
gbgl_bs_draw_rect(gbglBasicState *bs, gbVec2 pos, gbVec2 dim, gbColour col)
{
	gbgl_bs_draw_quad(bs,
	                  gb_vec2(pos.x,       pos.y),
	                  gb_vec2(pos.x+dim.x, pos.y),
	                  gb_vec2(pos.x+dim.x, pos.y+dim.y),
	                  gb_vec2(pos.x,       pos.y+dim.y),
	                  col);
}

void
gbgl_bs_draw_outlined_rect(gbglBasicState *bs, gbVec2 pos, gbVec2 dim, gbColour col, f32 thickness)
{
	gbgl_bs_draw_outlined_quad(bs,
	                           gb_vec2(pos.x,       pos.y),
	                           gb_vec2(pos.x+dim.x, pos.y),
	                           gb_vec2(pos.x+dim.x, pos.y+dim.y),
	                           gb_vec2(pos.x,       pos.y+dim.y),
	                           col,
	                           thickness);
}


gb_internal void
gbgl__bs_setup_ortho_colour_state(gbglBasicState *bs, isize vertex_count, gbColour col)
{
	gbVec4 vcol = gb_vec4(col.r/255.0f, col.g/255.0f, col.b/255.0f, col.a/255.0f);

	gbgl_use_shader(&bs->ortho_col_shader);
	gbgl_set_uniform_mat4(gbgl_get_uniform(&bs->ortho_col_shader, "u_ortho_mat"), bs->ortho_mat.e);
	gbgl_set_uniform_vec4(gbgl_get_uniform(&bs->ortho_col_shader, "u_colour"), vcol);

	gbgl_vbo_copy(bs->vbo, bs->vertices, vertex_count*gb_size_of(bs->vertices[0]), 0);
	gbgl_vert_ptr_aa(0, 2, gbglBasicVertex, x);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bs->ebo);

	glDisable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glBlendEquation(GL_FUNC_ADD);
	glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
}

void
gbgl_bs_draw_quad(gbglBasicState *bs,
                  gbVec2 p0, gbVec2 p1, gbVec2 p2, gbVec2 p3, gbColour col)
{
	bs->vertices[0].x = p0.x;
	bs->vertices[0].y = p0.y;

	bs->vertices[1].x = p1.x;
	bs->vertices[1].y = p1.y;

	bs->vertices[2].x = p2.x;
	bs->vertices[2].y = p2.y;

	bs->vertices[3].x = p3.x;
	bs->vertices[3].y = p3.y;

	gbgl__bs_setup_ortho_colour_state(bs, 4, col);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, NULL);
}

void
gbgl_bs_draw_outlined_quad(gbglBasicState *bs,
                           gbVec2 p0, gbVec2 p1, gbVec2 p2, gbVec2 p3, gbColour col, f32 thickness)
{
	bs->vertices[0].x = p0.x;
	bs->vertices[0].y = p0.y;

	bs->vertices[1].x = p1.x;
	bs->vertices[1].y = p1.y;

	bs->vertices[2].x = p2.x;
	bs->vertices[2].y = p2.y;

	bs->vertices[3].x = p3.x;
	bs->vertices[3].y = p3.y;

	gbgl__bs_setup_ortho_colour_state(bs, 4, col);
	glLineWidth(thickness);
	glDrawArrays(GL_LINE_LOOP, 0, 4);
}

void
gbgl_bs_draw_line(gbglBasicState *bs, gbVec2 p0, gbVec2 p1, gbColour col, f32 thickness)
{
	bs->vertices[0].x = p0.x;
	bs->vertices[0].y = p0.y;

	bs->vertices[1].x = p1.x;
	bs->vertices[1].y = p1.y;

	gbgl__bs_setup_ortho_colour_state(bs, 2, col);
	glLineWidth(thickness);
	glDrawArrays(GL_LINES, 0, 2);
}

void
gbgl_bs_draw_circle(gbglBasicState *bs, gbVec2 p, f32 radius, gbColour col)
{
	isize i;

	bs->vertices[0].x = p.x;
	bs->vertices[0].y = p.y;

	for (i = 0; i < 31; i++) {
		f32 t = cast(f32)i / 30.0f;
		f32 theta = t * GB_MATH_TAU;
		f32 c = gb_cos(theta);
		f32 s = gb_sin(theta);
		bs->vertices[i+1].x = p.x + c*radius;
		bs->vertices[i+1].y = p.y + s*radius;
	}

	gbgl__bs_setup_ortho_colour_state(bs, 32, col);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 32);
}

void
gbgl_bs_draw_outlined_circle(gbglBasicState *bs, gbVec2 p, f32 radius, gbColour col, f32 thickness)
{
	isize i;

	for (i = 0; i < 32; i++) {
		f32 t = cast(f32)i / 31.0f;
		f32 theta = t * GB_MATH_TAU;
		f32 c = gb_cos(theta);
		f32 s = gb_sin(theta);
		bs->vertices[i].x = p.x + c*radius;
		bs->vertices[i].y = p.y + s*radius;
	}

	gbgl__bs_setup_ortho_colour_state(bs, 32, col);
	glLineWidth(thickness);
	glDrawArrays(GL_LINE_LOOP, 0, 32);
}




#endif
