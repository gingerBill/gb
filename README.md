# gb

gb single-file public domain libraries for C &amp; C++

library         | latest version | category | description
----------------|----------------|----------|-------------
**gb.h**        | 0.27           | misc     | Helper library (Standard library _improvement_)
**gb_math.h**   | 0.07e          | math     | Vector math library geared towards game development
**gb_gl.h**     | 0.05           | graphics | OpenGL Helper Library
**gb_string.h** | 0.95a          | strings  | A better string library (this is built into gb.h too with custom allocator support!)
**gb_ini.h**    | 0.93           | misc     | Simple ini file loader library


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
