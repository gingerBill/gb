# gb

gb single-file public domain libraries for C &amp; C++

library         | latest version | category | languages | description
----------------|----------------|----------|-----------|-------------
**gb.h**        | 0.05a          | misc     | C, C++    | A C helper library for C & C++
**gb_math.h**   | 0.05           | math     | C, C++    | A C/C++ vector math library geared towards game development
**gb_gl.h**     | 0.04a          | graphics | C, C++    | A C/C++ OpenGL Helper Library
**gb_string.h** | 0.94           | strings  | C, C++    | A better string library for C & C++ (this is built into gb.h too with custom allocator support!)
**gb_ini.h**    | 0.92           | misc     | C, C++    | A simple ini file loader library for C & C++


## FAQ

### What's the license?

These libraries are in the public domain. You can do anything you want with them. You have no legal obligation to do anything else, although I would appreciate attribution.

### If I wrap an gb library in a new library, does the new library have to be public domain?

No.

### Is this in the style of the [stb libraries](https://github.com/nothings/stb)?

Yes. I think these libraries are brilliant and use many of these on a daily basis.

### May I contribute?

Yes.

### What is the versioning system that you use?

I may change it in the future but at the moment it is like this this:

`1.23b`

* `1`  = major version
* `23` = minor version
* `b`  = patch
	- 1.23 => zero patches
	- 1.23a => patch 1
	- 1.23b => patch 2
	- etc.
