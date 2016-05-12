/* gb_regex.h - v0.01a - Regular Expressions Library - public domain
                       - no warranty implied; use at your own risk

	This is a single header file with a bunch of useful stuff
	to replace the C/C++ standard library

===========================================================================
	YOU MUST

		#define GB_REGEX_IMPLEMENTATION

	in EXACTLY _one_ C or C++ file that includes this header, BEFORE the
	include like this:

		#define GB_REGEX_IMPLEMENTATION
		#include "gb_regex.h"

	All other files should just #include "gb_regex.h" without #define
===========================================================================


Version History:
	0.01a - New \ codes and bug fixes
	0.01  - Initial Version

LICENSE
	This software is dual-licensed to the public domain and under the following
	license: you are granted a perpetual, irrevocable license to copy, modify,
	publish, and distribute this file as you see fit.

WARNING
	- This library is _highly_ experimental and features may not work as expected.
	- This also means that many functions are not documented.

NOTES
	Supported Matching:
		^
		$
		.
		()
		[]
		[^]
		+
		+?
		*
		*?
		?
		\XX
		\meta

		\s
		\S
		\d
		\D
		\a
		\l
		\u
		\w
		\W
		\x
		\p

		--Whitespace--
		\t
		\n
		\r
		\v
		\f

TODO
	{m,n}
	(?:)


CREDITS
	Written by Ginger Bill


*/

#ifndef GB_REGEX_INCLUDE_GB_REGEX_H
#define GB_REGEX_INCLUDE_GB_REGEX_H

#include <stddef.h>
#include <stdarg.h>
#include <string.h>

#if defined(__cplusplus)
extern "C" {
#endif

/* NOTE(bill): Redefine for DLL, etc. */
#ifndef GBRE_DEF
	#ifdef GBRE_STATIC
		#define GBRE_DEF static
	#else
		#define GBRE_DEF extern
	#endif
#endif

#if !defined(GBRE_NO_MALLOC)
#ifndef GBRE_MALLOC
#define GBRE_MALLOC(sz) malloc(sz)
#endif

#ifndef GBRE_REALLOC
#define GBRE_REALLOC(ptr, sz) realloc(ptr, sz)
#endif

#ifndef GBRE_FREE
#define GBRE_FREE(ptr) free(ptr)
#endif
#endif /* !defined(GBRE_NO_MALLOC) */

typedef ptrdiff_t isize; /* TODO(bill): Should this be replaced with int? */
typedef int       gbreBool;

#define GBRE_TRUE  (0 == 0)
#define GBRE_FALSE (0 != 0)


typedef struct gbreCapture {
	char *str;
	isize len;
} gbreCapture;

typedef struct gbRegex {
	isize capture_count;
	unsigned char *buf;
	isize          buf_len, buf_cap;
	gbreBool       used_malloc;
} gbRegex;

typedef enum gbreError {
	GBRE_ERROR_NONE,
	GBRE_ERROR_NO_MATCH,
	GBRE_ERROR_TOO_LONG,
	GBRE_ERROR_MISMATCHED_CAPTURES,
	GBRE_ERROR_MISMATCHED_BLOCKS,
	GBRE_ERROR_INVALID_QUANTIFIER,
	GBRE_ERROR_BRANCH_FAILURE,
	GBRE_ERROR_INTERNAL_FAILURE
} gbreError;


#if !defined(GBRE_NO_MALLOC)
GBRE_DEF gbreError gbre_compile            (gbRegex *re, char *pattern, isize pattern_len);
#endif
GBRE_DEF gbreError gbre_compile_from_buffer(gbRegex *re, char *pattern, isize pattern_len, void *buffer, isize buffer_len);
GBRE_DEF void      gbre_destroy            (gbRegex *re);
GBRE_DEF gbreBool  gbre_match              (gbRegex *re, char *str, isize str_len, gbreCapture *captures, isize max_capture_count);


#if defined(__cplusplus)
}
#endif


#endif /* GB_REGEX_INCLUDE_GB_REGEX_H */

/****************************************************************
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
 ***************************************************************/

#if defined(GB_REGEX_IMPLEMENTATION) && !defined(GB_REGEX_IMPLEMENTATION_DONE)
#define GB_REGEX_IMPLEMENTATION_DONE


#if defined(__cplusplus)
extern "C" {
#endif

typedef enum gbreOp {
	GBRE_OP_BEGIN_CAPTURE,
	GBRE_OP_END_CAPTURE,

	GBRE_OP_BEGINNING_OF_LINE,
	GBRE_OP_END_OF_LINE,

	GBRE_OP_EXACT_MATCH,
	GBRE_OP_META_MATCH,

	GBRE_OP_ANY,
	GBRE_OP_ANY_OF,
	GBRE_OP_ANY_BUT,

	GBRE_OP_ZERO_OR_MORE,
	GBRE_OP_ONE_OR_MORE,
	GBRE_OP_ZERO_OR_MORE_SHORTEST,
	GBRE_OP_ONE_OR_MORE_SHORTEST,
	GBRE_OP_ZERO_OR_ONE,

	GBRE_OP_BRANCH_START,
	GBRE_OP_BRANCH_END
} gbreOp;

typedef enum gbreCode {
	GBRE_CODE_NULL              = 0x0000,
	GBRE_CODE_WHITESPACE        = 0x0100,
	GBRE_CODE_NOT_WHITESPACE    = 0x0200,
	GBRE_CODE_DIGIT             = 0x0300,
	GBRE_CODE_NOT_DIGIT         = 0x0400,
	GBRE_CODE_ALPHA             = 0x0500,
	GBRE_CODE_LOWER             = 0x0600,
	GBRE_CODE_UPPER             = 0x0700,
	GBRE_CODE_WORD              = 0x0800,
	GBRE_CODE_NOT_WORD          = 0x0900,

	GBRE_CODE_XDIGIT            = 0x0a00,
	GBRE_CODE_PRINTABLE         = 0x0b00,
} gbreCode;

typedef struct gbreContext {
	isize op, offset;
} gbreContext;

enum {
	GBRE__NO_MATCH = -1,
	GBRE__INTERNAL_FAILURE = -2
};

static char const GBRE__META_CHARS[]  = "^$()[].*+?|\\";
static char const GBRE__WHITESPACE[] = " \r\t\n\v\f";
#define GBRE__LITERAL(str) (str), sizeof(str)-1


static gbreContext gbre__exec_single(gbRegex *re, isize op, char *str, isize str_len, isize offset,
                                     gbreCapture *captures, isize max_capture_count);

static gbreContext gbre__exec(gbRegex *re, isize op, char *str, isize str_len, isize offset,
                              gbreCapture *captures, isize max_capture_count);

static gbreContext
gbre__context_no_match(isize next_op)
{
	gbreContext c;
	c.op = next_op;
	c.offset = GBRE__NO_MATCH;
	return c;
}


static gbreContext
gbre__context_internal_failure(isize next_op)
{
	gbreContext c;
	c.op = next_op;
	c.offset = GBRE__INTERNAL_FAILURE;
	return c;
}

static gbreBool
gbre__is_hex(char const *s)
{
	if ((s[0] < '0' || s[0] > '9') &&
	    (s[0] < 'a' || s[0] > 'f') &&
	    (s[0] < 'A' || s[0] > 'F')) {
		return GBRE_FALSE;
	}
	if ((s[1] < '0' || s[1] > '9') &&
	    (s[1] < 'a' || s[1] > 'f') &&
	    (s[1] < 'A' || s[1] > 'F')) {
		return GBRE_FALSE;
	}
	return GBRE_TRUE;
}

static unsigned char
gbre__hex_digit(char const *s)
{
	if (s[0] >= '0' && s[0] <= '9')
		return (unsigned char)(s[0] - '0');
	if (s[0] >= 'a' && s[0] <= 'f')
		return (unsigned char)(10 + s[0] - 'a');
	if (s[0] >= 'A' && s[0] <= 'F')
		return (unsigned char)(10 + s[0] - 'A');
	return 0;
}

static unsigned char
gbre__hex(char const *s)
{
	return ((gbre__hex_digit(s) << 4) & 0xf0) | (gbre__hex_digit(s+1) & 0x0f);
}

static isize
gbre__strfind(char const *str, isize len, char c, isize offset)
{
	if (offset < len) {
		void const *found = memchr(str+offset, c, len-offset);
		if (found)
			return (char const*)found-(char const*)str;
	}
	return -1;
}

static gbreBool
gbre__match_escape(char c, int code)
{
	switch (code) {
	case GBRE_CODE_NULL:           return c == 0;
	case GBRE_CODE_WHITESPACE:     return gbre__strfind(GBRE__LITERAL(GBRE__WHITESPACE), c, 0) >= 0;
	case GBRE_CODE_NOT_WHITESPACE: return gbre__strfind(GBRE__LITERAL(GBRE__WHITESPACE), c, 0) < 0;
	case GBRE_CODE_DIGIT:          return gbre__strfind(GBRE__LITERAL("0123456789"), c, 0) >= 0;
	case GBRE_CODE_NOT_DIGIT:      return gbre__strfind(GBRE__LITERAL("0123456789"), c, 0) < 0;
	case GBRE_CODE_ALPHA:          return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z');
	case GBRE_CODE_LOWER:          return (c >= 'a' && c <= 'z');
	case GBRE_CODE_UPPER:          return (c >= 'A' && c <= 'Z');

	/* TODO(bill): Make better */
	case GBRE_CODE_WORD:           return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || (c >= '0' && c <= '9') || c == '_';
	case GBRE_CODE_NOT_WORD:       return !((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || (c >= '0' && c <= '9') || c == '_');

	case GBRE_CODE_XDIGIT:         return gbre__strfind(GBRE__LITERAL("0123456789ABCDEFabcdef"), c, 0) >= 0;
	case GBRE_CODE_PRINTABLE:      return c >= 0x20 && c <= 0x7e;
	default: break;
	}
	return GBRE_FALSE;
}


static gbreContext
gbre__greedy_consume(gbRegex *re, isize op, char *str, isize str_len, isize offset,
                     gbreCapture *captures, isize max_capture_count)
{
	gbreContext c, best_c, next_c;

	c.op = op;
	c.offset = offset;

	best_c.op = GBRE__NO_MATCH;
	best_c.offset = offset;

	next_c;

	for (;;) {
		c = gbre__exec_single(re, op, str, str_len, c.offset, 0, 0);
		if (c.offset > str_len) break;
		if (c.op >= re->buf_len) return c;

		next_c = gbre__exec(re, c.op, str, str_len, c.offset, 0, 0);
		if (next_c.offset <= str_len) {
			if (captures)
				gbre__exec(re, c.op, str, str_len, c.offset, captures, max_capture_count);
			best_c = next_c;
			/* NOTE(bill): Break would be here for shortest consumption */
		}
	}

	if (best_c.op > re->buf_len)
		best_c.op = c.op;

	return best_c;
}


static gbreContext
gbre__non_greedy_consume(gbRegex *re, isize op, char *str, isize str_len, isize offset,
                          gbreCapture *captures, isize max_capture_count)
{
	gbreContext c, best_c, next_c;

	c.op = op;
	c.offset = offset;

	best_c.op = GBRE__NO_MATCH;
	best_c.offset = offset;

	next_c;

	for (;;) {
		c = gbre__exec_single(re, op, str, str_len, c.offset, 0, 0);
		if (c.offset > str_len) break;
		if (c.op >= re->buf_len) return c;

		next_c = gbre__exec_single(re, c.op, str, str_len, c.offset, 0, 0);
		if (next_c.offset <= str_len) {
			if (captures)
				gbre__exec(re, c.op, str, str_len, c.offset, captures, max_capture_count);
			best_c = next_c;
			break; /* NOTE(bill): Break early!!! */
		}
	}

	if (best_c.op > re->buf_len)
		best_c.op = c.op;

	return best_c;
}

static gbreContext
gbre__exec_single(gbRegex *re, isize op, char *str, isize str_len, isize offset,
                  gbreCapture *captures, isize max_capture_count)
{
	gbreContext context;
	isize buffer_len;
	isize matchlen;
	isize next_op;
	isize skip;

	switch (re->buf[op++]) {
	case GBRE_OP_BEGIN_CAPTURE: {
		unsigned char capture = re->buf[op++];
		if (captures && (capture < max_capture_count))
			captures[capture].str = str + offset;
	} break;

	case GBRE_OP_END_CAPTURE: {
		unsigned char capture = re->buf[op++];
		if (captures && (capture < max_capture_count))
			captures[capture].len = (char const *)captures[capture].str - (char const *)(str + offset);
	} break;

	case GBRE_OP_BEGINNING_OF_LINE: {
		if (offset != 0)
			return gbre__context_no_match(op);
	} break;

	case GBRE_OP_END_OF_LINE: {
		if (offset != str_len)
			return gbre__context_no_match(op);
	} break;

	case GBRE_OP_ANY_OF: {
		isize i;
		char cin = str[offset];
		buffer_len = re->buf[op++];

		if (offset >= str_len)
			return gbre__context_no_match(op + buffer_len);

		for (i = 0; i < buffer_len; i++) {
			char cmatch = (char)re->buf[op + i];
			if (!cmatch) {
				i++;
				if (gbre__match_escape(cin, re->buf[op+i] << 8))
					break;
			} else if (cin == cmatch) {
				break;
			}
		}

		if (i == buffer_len)
			return gbre__context_no_match(op + buffer_len);

		offset++;
		op += buffer_len;
	} break;

	case GBRE_OP_ANY_BUT: {
		isize i;
		char cin = str[offset];
		buffer_len = re->buf[op++];

		if (offset >= str_len)
			return gbre__context_no_match(op + buffer_len);

		for (i = 0; i < buffer_len; i++) {
			char cmatch = (char)re->buf[op + i];
			if (!cmatch) {
				i++;
				if (gbre__match_escape(cin, re->buf[op+i] << 8))
					return gbre__context_no_match(op + buffer_len);
			} else if (cin == cmatch) {
				return gbre__context_no_match(op + buffer_len);
			}
		}

		offset++;
		op += buffer_len;
	} break;

	case GBRE_OP_ANY: {
		if (offset < str_len) {
			offset++;
			break;
		}
		return gbre__context_no_match(op);
	} break;

	case GBRE_OP_EXACT_MATCH: {
		matchlen = re->buf[op++];
		if ((matchlen > (str_len-offset)) ||
		    strncmp(str+offset, (const char*)re->buf + op, matchlen) != 0)
			return gbre__context_no_match(op + matchlen);
		op += matchlen;
		offset += matchlen;
	} break;

	case GBRE_OP_META_MATCH: {
		char cin = (char)re->buf[op++];
		char cmatch = str[offset++];
		if (!cin) {
			if (gbre__match_escape(cmatch, re->buf[op++] << 8))
				break;
		} else if (cin == cmatch) {
			break;
		}
		return gbre__context_no_match(op);
	} break;

	case GBRE_OP_ZERO_OR_MORE: {
		context = gbre__greedy_consume(re, op, str, str_len, offset, captures, max_capture_count);
		offset = context.offset;
		op = context.op;
	} break;

	case GBRE_OP_ONE_OR_MORE: {
		context = gbre__exec_single(re, op, str, str_len, offset, captures, max_capture_count);
		if (context.offset > str_len)
			return context;
		context = gbre__greedy_consume(re, op, str, str_len, context.offset, captures, max_capture_count);
		offset = context.offset;
		op = context.op;
	} break;

	case GBRE_OP_ZERO_OR_MORE_SHORTEST: {
		context = gbre__non_greedy_consume(re, op, str, str_len, offset, captures, max_capture_count);
		offset = context.offset;
		op = context.op;
	} break;

	case GBRE_OP_ONE_OR_MORE_SHORTEST: {
		context = gbre__exec_single(re, op, str, str_len, offset, captures, max_capture_count);
		if (context.offset > str_len)
			return context;
		context = gbre__non_greedy_consume(re, op, str, str_len, context.offset, captures,
		                                 max_capture_count);
		offset = context.offset;
		op = context.op;
	} break;

	case GBRE_OP_ZERO_OR_ONE: {
		context = gbre__exec_single(re, op, str, str_len, offset, captures, max_capture_count);
		if (context.offset <= str_len) {
			gbreContext maybe_context = gbre__exec(re, context.op, str, str_len, context.offset,
			                                       captures, max_capture_count);
			if (maybe_context.offset <= str_len) {
				op = maybe_context.op;
				offset = maybe_context.offset;
				break;
			}
		}

		next_op = context.op;
		context = gbre__exec(re, next_op, str, str_len, offset, captures, max_capture_count);
		if (context.offset <= str_len) {
			op = context.op;
			offset = context.offset;
			break;
		}
		return gbre__context_no_match(next_op);
	} break;

	case GBRE_OP_BRANCH_START: {
		skip = re->buf[op++];
		context = gbre__exec(re, op, str, str_len, offset, captures, max_capture_count);
		if (context.offset <= str_len) {
			offset = context.offset;
			op = context.op;
		} else {
			context = gbre__exec(re, op + skip, str, str_len, offset, captures, max_capture_count);
			offset = context.offset;
			op = context.op;
		}
	} break;

	case GBRE_OP_BRANCH_END: {
		skip = re->buf[op++];
		op += skip;
	} break;

	default: {
		/* NOTE(bill): Not supported, FUCK!!! */
		return gbre__context_internal_failure(op);
	} break;
	}

	context.op = op;
	context.offset = offset;

	return context;
}

static gbreContext
gbre__exec(gbRegex *re, isize op, char *str, isize str_len, isize offset,
           gbreCapture *captures, isize max_capture_count)
{
	gbreContext c;
	c.op = op;
	c.offset = offset;
	while (c.op < re->buf_len) {
		c = gbre__exec_single(re, c.op, str, str_len, c.offset, captures, max_capture_count);
		if (c.offset > str_len)
			break;
	}

	return c;
}


static gbreError
gbre__emit(gbRegex *re, gbreBool allow_grow, isize op_count, ...)
{
	isize i;
	va_list va;

	if (re->buf_len + op_count > re->buf_cap) {
		if (!allow_grow) {
			return GBRE_ERROR_TOO_LONG;
		} else {
#if !defined(GBRE_NO_MALLOC)
			isize new_cap = (re->buf_cap * 2) + op_count;
			re->buf = (unsigned char *)GBRE_REALLOC(re->buf, new_cap);
			re->buf_cap = new_cap;
#else
#error GBRE_NO_MALLOC defined
#endif
		}
	}

	va_start(va, op_count);
	for (i = 0; i < op_count; i++) {
		int value = va_arg(va, int);
		if (value > 256)
			return GBRE_ERROR_TOO_LONG;
		re->buf[re->buf_len++] = (unsigned char)value;
	}
	va_end(va);

	return GBRE_ERROR_NONE;
}

static gbreError
gbre__emit_buffer(gbRegex *re, gbreBool allow_grow, isize op_count, unsigned char const *buffer)
{
	isize i;

	if (re->buf_len + op_count > re->buf_cap) {
		if (!allow_grow) {
			return GBRE_ERROR_TOO_LONG;
		} else {
#if !defined(GBRE_NO_MALLOC)
			isize new_cap = (re->buf_cap * 2) + op_count;
			re->buf = (unsigned char *)GBRE_REALLOC(re->buf, new_cap);
			re->buf_cap = new_cap;
#else
#error GBRE_NO_MALLOC defined
#endif
		}
	}

	for (i = 0; i < op_count; i++) {
		re->buf[re->buf_len++] = buffer[i];
	}

	return GBRE_ERROR_NONE;
}

static int
gbre__encode_espace(char code)
{
	switch (code) {
	default:   break; /* NOTE(bill): It's a normal character */

	case 't':  return '\t';
	case 'n':  return '\n';
	case 'r':  return '\r';
	case 'f':  return '\f';
	case 'v':  return '\v';

	case '0':  return GBRE_CODE_NULL;

	case 's':  return GBRE_CODE_WHITESPACE;
	case 'S':  return GBRE_CODE_NOT_WHITESPACE;

	case 'd':  return GBRE_CODE_DIGIT;
	case 'D':  return GBRE_CODE_NOT_DIGIT;

	case 'a':  return GBRE_CODE_ALPHA;
	case 'l':  return GBRE_CODE_LOWER;
	case 'u':  return GBRE_CODE_UPPER;

	case 'w':  return GBRE_CODE_WORD;
	case 'W':  return GBRE_CODE_NOT_WORD;

	case 'x':  return GBRE_CODE_XDIGIT;
	case 'p':  return GBRE_CODE_PRINTABLE;
	}
	return code;
}

static gbreError
gbre__parse_group(gbRegex *re, char *pattern, isize len, isize offset, gbreBool allow_grow, isize *new_offset)
{
	gbreError err = GBRE_ERROR_NONE;
	unsigned char buffer[256] = {0}; /* NOTE(bill): ascii is only 7/8 bits */
	isize buffer_len = 0, buffer_cap = sizeof(buffer);
	gbreBool closed = GBRE_FALSE;
	gbreOp op = GBRE_OP_ANY_OF;

	if (pattern[offset] == '^') {
		offset++;
		op = GBRE_OP_ANY_BUT;
	}

	while (!closed && err == GBRE_ERROR_NONE && (offset < len)) {
		if (pattern[offset] == ']') {
			err = gbre__emit(re, allow_grow, 2, (int)op, (int)buffer_len);
			if (err) break;

			err = gbre__emit_buffer(re, allow_grow, buffer_len, buffer);
			if (buffer && err) break;
			offset++;
			closed = GBRE_TRUE;
			break;
		}

		if (buffer_len >= buffer_cap)
			return GBRE_ERROR_TOO_LONG;

		if (pattern[offset] == '\\') {
			offset++;

			if ((offset+1 < len) && gbre__is_hex(pattern+offset)) {
				buffer[buffer_len++] = gbre__hex(pattern+offset);
				offset++;
			} else if (offset < len) {
				int code = gbre__encode_espace(pattern[offset]);
				if (!code || code > 0xff) {
					buffer[buffer_len++] = 0;
					if (buffer_len >= buffer_cap)
						return GBRE_ERROR_TOO_LONG;
					buffer[buffer_len++] = (code >> 8) & 0xff;
				} else {
					buffer[buffer_len++] = code & 0xff;
				}
			}
		} else {
			buffer[buffer_len++] = (unsigned char)pattern[offset];
		}
		offset++;
	}

	if (err) return err;
	if (!closed) return GBRE_ERROR_MISMATCHED_BLOCKS;
	if (new_offset) *new_offset = offset;
	return GBRE_ERROR_NONE;
}

static gbreError
gbre__compile_quantifier(gbRegex *re, gbreBool allow_grow, isize last_buf_len, unsigned char quantifier)
{
	gbreError err;
	isize move_size;
	if ((re->buf[last_buf_len] == GBRE_OP_EXACT_MATCH) &&
	    (re->buf[last_buf_len+1] > 1)) {
		unsigned char last_char = re->buf[re->buf_len-1];

		re->buf[last_buf_len+1]--;
		re->buf_len--;
		err = gbre__emit(re, allow_grow, 4, (int)quantifier, (int)GBRE_OP_EXACT_MATCH, 1, (int)last_char);
		if (err) return err;
		return GBRE_ERROR_NONE;
	}

	move_size = re->buf_len - last_buf_len + 1;

	err = gbre__emit(re, allow_grow, 1, 0);
	if (err) return err;

	memmove(re->buf+last_buf_len+1, re->buf+last_buf_len, move_size);
	re->buf[last_buf_len] = quantifier;

	return GBRE_ERROR_NONE;
}


/* NOTE(bill): Either returns error (-ve value) or offset (+ve value) */
static gbreError
gbre__parse(gbRegex *re, char *pattern, isize len, isize offset, gbreBool allow_grow, isize level, isize *new_offset)
{
	gbreError err = GBRE_ERROR_NONE;
	isize last_buf_len = re->buf_len;
	isize branch_begin = re->buf_len;
	isize branch_op = -1;

	while (offset < len) {
		switch (pattern[offset++]) {
		case '^': {
			err = gbre__emit(re, allow_grow, 1, GBRE_OP_BEGINNING_OF_LINE);
			if (err) return err;
		} break;

		case '$': {
			err = gbre__emit(re, allow_grow, 1, GBRE_OP_END_OF_LINE);
			if (err) return err;
		} break;

		case '(': {
			isize capture = re->capture_count++;
			last_buf_len = re->buf_len;
			err = gbre__emit(re, allow_grow, 2, GBRE_OP_BEGIN_CAPTURE, (int)capture);
			if (err) return err;

			err = gbre__parse(re, pattern, len, offset, allow_grow, level+1, &offset);

			if ((offset > len) || (pattern[offset-1] != ')'))
				return GBRE_ERROR_MISMATCHED_CAPTURES;

			err = gbre__emit(re, allow_grow, 2, GBRE_OP_END_CAPTURE, (int)capture);
			if (err) return err;
		} break;

		case ')': {
			if (branch_op != -1)
				re->buf[branch_op + 1] = (unsigned char)(re->buf_len - (branch_op+2));

			if (level == 0)
				return GBRE_ERROR_MISMATCHED_CAPTURES;
			if (new_offset) *new_offset = offset;
			return GBRE_ERROR_NONE;
		} break;

		case '[': {
			last_buf_len = re->buf_len;
			err = gbre__parse_group(re, pattern, len, offset, allow_grow, &offset);
			if (offset > len)
				return err;
		} break;

		/* NOTE(bill): Branching magic! */
		case '|': {
			if (branch_begin >= re->buf_len) {
				return GBRE_ERROR_BRANCH_FAILURE;
			} else {
				isize size = re->buf_len - branch_begin;
				err = gbre__emit(re, allow_grow, 4, 0, 0, GBRE_OP_BRANCH_END, 0);
				if (err) return err;

				memmove(re->buf + branch_begin + 2, re->buf + branch_begin, size);
				re->buf[branch_begin] = GBRE_OP_BRANCH_START;
				re->buf[branch_begin + 1] = (size+2) & 0xff;
				branch_op = re->buf_len - 2;
			}
		} break;

		case '.': {
			last_buf_len = re->buf_len;
			err = gbre__emit(re, allow_grow, 1, GBRE_OP_ANY);
			if (err) return err;
		} break;

		case '*':
		case '+':
		{
			unsigned char quantifier = GBRE_OP_ONE_OR_MORE;
			if (pattern[offset-1] == '*')
				quantifier = GBRE_OP_ZERO_OR_MORE;

			if (last_buf_len >= re->buf_len)
				return GBRE_ERROR_INVALID_QUANTIFIER;
			if ((re->buf[last_buf_len] < GBRE_OP_EXACT_MATCH) ||
			        (re->buf[last_buf_len] > GBRE_OP_ANY_BUT))
				return GBRE_ERROR_INVALID_QUANTIFIER;

			if ((offset < len) && (pattern[offset] == '?')) {
				quantifier = GBRE_OP_ONE_OR_MORE_SHORTEST;
				if (quantifier == GBRE_OP_ZERO_OR_MORE)
					quantifier = GBRE_OP_ZERO_OR_MORE_SHORTEST;
				offset++;
			}

			err = gbre__compile_quantifier(re, allow_grow, last_buf_len, quantifier);
			if (err) return err;
		} break;

		case '?': {
			if (last_buf_len >= re->buf_len)
				return GBRE_ERROR_INVALID_QUANTIFIER;
			if ((re->buf[last_buf_len] < GBRE_OP_EXACT_MATCH) ||
			        (re->buf[last_buf_len] > GBRE_OP_ANY_BUT))
				return GBRE_ERROR_INVALID_QUANTIFIER;

			err = gbre__compile_quantifier(re, allow_grow, last_buf_len,
			                               (unsigned char)GBRE_OP_ZERO_OR_ONE);
			if (err) return err;
		} break;

		case '\\': {
			last_buf_len = re->buf_len;
			if ((offset+1 < len) && gbre__is_hex(pattern+offset)) {
				unsigned char hex_value = gbre__hex(pattern+offset);
				offset += 2;
				err = gbre__emit(re, allow_grow, 2, GBRE_OP_META_MATCH, (int)hex_value);
				if (err) return err;
			} else if (offset < len) {
				int code = gbre__encode_espace(pattern[offset++]);
				if (!code || (code > 0xff)) {
					err = gbre__emit(re, allow_grow, 3, GBRE_OP_META_MATCH, 0, (int)((code >> 8) & 0xff));
					if (err) return err;
				} else {
					err = gbre__emit(re, allow_grow, 2, GBRE_OP_META_MATCH, (int)code);
					if (err) return err;
				}
			}
		} break;

		default: {
			/* NOTE(bill): Exact match */
			char const *match_start;
			isize size = 0;
			offset--;
			match_start = pattern+offset;
			while ((offset < len) &&
			        (gbre__strfind(GBRE__LITERAL(GBRE__META_CHARS), pattern[offset], 0) < 0)) {
				size++, offset++;
			}

			last_buf_len = re->buf_len;
			err = gbre__emit(re, allow_grow, 2, GBRE_OP_EXACT_MATCH, (int)size);
			if (err) return err;
			err = gbre__emit_buffer(re, allow_grow, size, (unsigned char const *)match_start);
			if (err) return err;
		} break;
		}
	}

	if (new_offset) *new_offset = offset;
	return GBRE_ERROR_NONE;
}

gbreError
gbre_compile_from_buffer(gbRegex *re, char *pattern, isize pattern_len, void *buffer, isize buffer_len)
{
	gbreError err;
	re->capture_count = 0;
	re->buf           = (unsigned char *)buffer;
	re->buf_len       = 0;
	re->buf_cap       = buffer_len;
	re->used_malloc   = GBRE_FALSE;

	err = gbre__parse(re, pattern, pattern_len, 0, GBRE_FALSE, 0, 0);
	return err;
}

#if !defined(GBRE_NO_MALLOC)
gbreError
gbre_compile(gbRegex *re, char *pattern, isize len)
{
	gbreError err = GBRE_ERROR_NONE;
	isize cap = len+128;
	isize offset = 0;

	re->capture_count = 0;
	re->buf           = (unsigned char *)GBRE_MALLOC(cap);
	re->buf_len       = 0;
	re->buf_cap       = cap;
	re->used_malloc   = GBRE_TRUE;


	err = gbre__parse(re, pattern, len, 0, GBRE_TRUE, 0, &offset);
	if (offset != len)
		GBRE_FREE(re->buf);
	return err;
}
#endif

void gbre_destroy(gbRegex *re)
{
	(void)sizeof(re);

#if !defined(GBRE_NO_MALLOC)
	if (re->used_malloc && re->buf) {
		GBRE_FREE(re->buf);
		re->buf = NULL;
	}
#endif
}

gbreBool
gbre_match(gbRegex *re, char *str, isize len, gbreCapture *captures, isize max_capture_count)
{
	if (re && re->buf_len > 0) {
		if (re->buf[0] == GBRE_OP_BEGINNING_OF_LINE) {
			gbreContext c = gbre__exec(re, 0, str, len, 0, captures, max_capture_count);
			if (c.offset >= 0 && c.offset <= len)   return GBRE_TRUE;
			if (c.offset == GBRE__INTERNAL_FAILURE) return GBRE_FALSE;
		} else {
			isize i;
			for (i = 0; i < len; i++) {
				gbreContext c = gbre__exec(re, 0, str, len, i, captures, max_capture_count);
				if (c.offset >= 0 && c.offset <= len)   return GBRE_TRUE;
				if (c.offset == GBRE__INTERNAL_FAILURE) return GBRE_FALSE;
			}
		}
		return GBRE_FALSE;
	}
	return GBRE_TRUE;
}


#if defined(__cplusplus)
}
#endif

#endif /* GB_REGEX_IMPLEMENTATION */
