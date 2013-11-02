prototype
=========

A prototype game engine that will hopefully one day become the Warfare engine, just a hobby project. It tries
to make use of the latest and greatest technology, and it should compile out of the box (all dependencies in-tree).

The engine is written in C (the C11 standard, although I believe it's also valid C99, as of current writing). It aims to
be small and clean, with extensibility provided by Lua (through LuaJIT). It is also written with being cross-platform in mind.
Although development happens on OS X, no platform-specific functionality has been used, so it should be rather easy
to port as long as the platform in question has either Clang (LLVM) or GCC avaiable to compile. (MSVC doesn't understand
C11 unfortunately).

This is alpha software, it is not usable yet, I will post an announcement on my website (www.aktau.be) when I'm ready
to have the wider world play around with it.

For those of you following at home and reading the source code. All matrices, everywhere are stored in such a way
that columns are contiguous in memory (i.e.: column-major order). This is the native format for OpenGL and it makes
it a bit more efficient to do SIMD operations on matrices. This means that, when you see a matrix you can imagine
that is a `float[16]`, and that `(mat[0], mat[1], mat[2], mat[3])` is the first column.

dependencies
============

All dependencies are included in-tree and are as cross-platform (or
more) as the engine itself.

- [SDL2](http://www.libsdl.org/)
- [LuaJIT](http://luajit.org/)
- [threedee-simd](https://github.com/rikusalminen/threedee-simd) - I maintain a personal fork, where I'm adding some extra functions, fixes and compiler warning fixes until they are upstreamed.

building
========

For now, just run
```bash
$ make -j8
```

If you want a release build, do

```bash
$ make -j8 release
```

acknowledgements
================

- Sean Barrett, creator of stb_image.c and stb_truetype.c, loading images
 and rendering text have never been so pleasant (and easy to include
 in a project).
- Samuel Anjam, creator of the BlendELF engine, upon which much of my code
 and structure is based.
- Tor Andersson, creator of the [Mio](https://github.com/ccxvii/mio) engine, I learned
  quite a bit perusing the source when I found it. Its principles seem
  remarkably similar to the ones I have for this project.
- Mike Pall, creator of LuaJIT, amazing piece of engineering.
- Sam Lantinga, creator of SDL, cross-platform graphics for the masses
- Julien Pommier, creator of sse_mathfun.h, fast transcendental functions!
- Riku Salminen, creator of [threedee-simd](https://github.com/rikusalminen/threedee-simd), the fastest pure 3D math
 library I know of.

Anyone I might have forgotten, I'm in your debt. If you find that you deserve
to be in this list, please shoot me a mail and you'll be added asap!

License (BSD)
=============

All files that have not been explicitly marked with a license and are copyright
by Nicolas Hillegeer, fall under the BSD license, detailed below.

The projects in the "deps" subfolder have their own respective licenses, please
consult the directory tree for the relevant information. The files in "src/math"
fall under the zlib license and are by different authors.

When individual files explicitly mention a license or copyright in the file, then
those take precedence for that particular file.

Copyright (c) 2013, Nicolas Hillegeer and other authors mentioned in the AUTHORS
file and in individual files. All rights reserved.

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

  Redistributions of source code must retain the above copyright notice, this
  list of conditions and the following disclaimer.

  Redistributions in binary form must reproduce the above copyright notice, this
  list of conditions and the following disclaimer in the documentation and/or
  other materials provided with the distribution.

  Neither the name of the {organization} nor the names of its
  contributors may be used to endorse or promote products derived from
  this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

