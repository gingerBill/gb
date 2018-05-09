/*
	test_ini_handler  应该始终返回1，否则会因为解析到不存在的值，而导致解析中断

name=QQ
version=8.0.16954.0
[license]
name=Copyright (C) 1999-2015 Tencent. All Rights Reserved
[author]
name=Tencent

*/

#define GB_INI_CPP
#define GB_INI_IMPLEMENTATION
#include "gb_ini.h"

#include <stdio.h>
#include <stdlib.h>

struct Library {
	char const *name;
	int         version;
	char const *license;
	char const *author;
};

// The below macro expands to this:
static gbIniHRT test_ini_handler(void *data, char const *section, char const *name, char const *value)
//static GB_INI_HANDLER(test_ini_handler)
{
	Library* lib = (Library*)data;

#define TEST(s, n) (strcmp(section, s) == 0 && strcmp(name, n) == 0)

	if (TEST("", "name"))
		lib->name    = strdup(value);
	else if (TEST("", "version"))
		lib->version = atoi(value);
	else if (TEST("license", "name"))
		lib->license = strdup(value);
	else if (TEST("author", "name"))
		lib->author  = strdup(value);
	else
		return 1;

#undef TEST

	return 1;
}

int main(int argc, char** argv)
{
	Library lib = {};

	using namespace gb;

	Ini_Error err = ini_parse("test.ini", &test_ini_handler, &lib);
	if (err.type != INI_ERROR_NONE) 
	{
		if (err.line_num > 0)
			printf("Line (%d): ", err.line_num);
		printf("%s\n", ini_error_string(err));

		return 1;
	}

	printf("Name    : %s\n", lib.name);    // Name    : gb_init.h
	printf("Version : %d\n", lib.version); // Version : 90
	printf("License : %s\n", lib.license); // License : Public Domain
	printf("Author  : %s\n", lib.author);  // Author  : Ginger Bill

	return 0;
}
