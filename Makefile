# This file is part of prototype.
#
# (c) 2013 Nicolas Hillegeer <nicolas@hillegeer.com>
#
# For the full copyright and license information, please view the LICENSE
# file that was distributed with the source code.

# $@ The name of the target file (the one before the colon)
# $< The name of the first (or only) prerequisite file (the first one after the colon)
# $^ The names of all the prerequisite files (space separated)
# $* The stem (the bit which matches the % wildcard in the rule definition.

ENABLE_LUA ?= 1

# CC = /usr/local/bin/gcc-4.8
CC = /usr/local/bin/clang
CC_VERSION := $(shell $(CC) --version | head -1 | cut -f1 -d' ')

CFLAGS ?= -D_THREAD_SAFE \
	-D_FORTIFY_SOURCE=2 \
	-Wextra \
	-Wcast-align \
	-Wcast-qual \
	-Wpointer-arith \
	-Waggregate-return \
	-Wunreachable-code \
	-Wfloat-equal \
	-Wformat=2 \
	-Wredundant-decls \
	-Wundef \
	-Wdisabled-optimization \
	-Wshadow \
	-Wmissing-braces \
	-Wstrict-aliasing=2 \
	-Wstrict-overflow=5 \
	-Wconversion \
	-Wno-unused-parameter \
	-pedantic \
	-std=c11

# END OF CONFIGURABLE PART

DEPS_PATH := ./deps
SDL_PATH := $(DEPS_PATH)/sdl2
LUA_PATH := $(DEPS_PATH)/lua

ifeq ($(OS),Windows_NT)
	CFLAGS += -D WIN32
	ifeq ($(PROCESSOR_ARCHITECTURE),AMD64)
		CFLAGS += -D AMD64
	endif
	ifeq ($(PROCESSOR_ARCHITECTURE),x86)
		CFLAGS += -D IA32
	endif
else
	UNAME_S := $(shell uname -s)

	ifeq ($(UNAME_S),Linux)
		CFLAGS += -D LINUX
	endif
	ifeq ($(UNAME_S),Darwin)
		CFLAGS += -D OSX
	endif

	ifneq (,$(findstring clang,$(CC_VERSION)))
		CFLAGS += -D CLANG

		# -pthread is not necessary when using Clang on Darwin
		ifneq ($(UNAME_S),Darwin)
			CFLAGS += -pthread
		endif
	else
		CFLAGS += -D GCC
		CFLAGS += -pthread

		# at least on OS X 10.7.5, the apple linker does not understand AVX, and gcc uses it natively
		ifeq ($(UNAME_S),Darwin)
			CFLAGS += -mno-avx
		endif
	endif

	UNAME_P := $(shell uname -m)

	ifeq ($(UNAME_P),x86_64)
		CFLAGS += -D AMD64
	endif
	ifneq ($(filter %86,$(UNAME_P)),)
		CFLAGS += -D IA32
	endif
	ifneq ($(filter arm%,$(UNAME_P)),)
		CFLAGS += -D ARM
	endif
endif

# clang doesn't perform link-time optimization without the gold linker, which as of yet
# is not the default, so for now I'm not bothering to make a wrapper script to make it
# use the gold linker, and just disable LTO for clang

CFLAGS_DEBUG := -g3 \
		-O \
		-DDEBUG

CFLAGS_RELEASE := -g3 \
		-O2 \
		-march=native \
		-mtune=native \
		-ftree-vectorize \
		-DDEBUG

ifneq (,$(findstring clang,$(CC_VERSION)))
	# if clang, add vectorize
	CFLAGS_RELEASE += -fslp-vectorize
else
	#if gcc, add lto (clang can do it, but requires a special linker)
	CFLAGS_RELEASE += -flto
endif

INCS = -I$(SDL_PATH)/include \
	-I/usr/include/malloc \
	-I/usr/X11R6/include

LIBS = $(SDL_PATH)/build/.libs/libSDL2.a $(SDL_PATH)/build/libSDL2main.a \
	-lm -liconv \
	-framework Cocoa -framework Carbon -framework OpenGL \
	-framework CoreAudio -framework AudioToolbox -framework AudioUnit \
	-framework ForceFeedback -framework IOKit

EXECUTABLE:=prototype
SOURCE:=src/main.c \
	src/util.c \
	src/zmalloc.c

DEPENDENCY_TARGETS =

ifeq ($(ENABLE_LUA), 1)
	INCS += -I$(LUA_PATH)/src
	LIBS += $(LUA_PATH)/src/liblua.a
	DEPENDENCY_TARGETS += lua
	CFLAGS += -D HAVE_LUA
	SOURCE += src/scripting.c
endif

OBJECTS=$(patsubst src%.c,build%.o, $(SOURCE))

debug: CFLAGS += $(CFLAGS_DEBUG)
debug: $(EXECUTABLE)

release: CFLAGS += $(CFLAGS_RELEASE)
release: $(EXECUTABLE)

all: debug

$(EXECUTABLE): $(OBJECTS)
		echo $(ENABLE_LUA)
		-(cd $(DEPS_PATH) && $(MAKE) $(DEPENDENCY_TARGETS) CC=$(CC))
		install -d build
		$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

# zmalloc.c needs -fno-strict-aliasing unfortunately, so we have a special rule for it
build/zmalloc.o: src/zmalloc.c
		$(CC) -c $< -o $@ $(CFLAGS) -fno-strict-aliasing $(INCS)

build/%.o: src/%.c
		$(CC) -c $< -o $@ $(CFLAGS) $(INCS)

clean:
	rm -f build/*.o || true
	rm -f $(EXECUTABLE) || true

.PHONY: all debug release
