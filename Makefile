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

SDL_PATH=./deps/sdl2

CCFLAGS = -D_THREAD_SAFE \
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

# CC = /usr/local/bin/gcc-4.8
CC = /usr/local/bin/clang
CC_VERSION := $(shell $(CC) --version | head -1 | cut -f1 -d' ')

ifeq ($(OS),Windows_NT)
	CCFLAGS += -D WIN32
	ifeq ($(PROCESSOR_ARCHITECTURE),AMD64)
		CCFLAGS += -D AMD64
	endif
	ifeq ($(PROCESSOR_ARCHITECTURE),x86)
		CCFLAGS += -D IA32
	endif
else
	UNAME_S := $(shell uname -s)

	ifeq ($(UNAME_S),Linux)
		CCFLAGS += -D LINUX
	endif
	ifeq ($(UNAME_S),Darwin)
		CCFLAGS += -D OSX
	endif

	ifneq (,$(findstring clang,$(CC_VERSION)))
		CCFLAGS += -D CLANG

		# -pthread is not necessary when using Clang on Darwin
		ifneq ($(UNAME_S),Darwin)
			CCFLAGS += -pthread
		endif
	else
		CCFLAGS += -D GCC
		CCFLAGS += -pthread

		# at least on OS X 10.7.5, the apple linker does not understand AVX, and gcc uses it natively
		ifeq ($(UNAME_S),Darwin)
			CCFLAGS += -mno-avx
		endif
	endif

	UNAME_P := $(shell uname -m)

	ifeq ($(UNAME_P),x86_64)
		CCFLAGS += -D AMD64
	endif
	ifneq ($(filter %86,$(UNAME_P)),)
		CCFLAGS += -D IA32
	endif
	ifneq ($(filter arm%,$(UNAME_P)),)
		CCFLAGS += -D ARM
	endif
endif

# clang doesn't perform link-time optimization without the gold linker, which as of yet
# is not the default, so for now I'm not bothering to make a wrapper script to make it
# use the gold linker, and just disable LTO for clang

CCFLAGS_DEBUG := -g3 \
		-O \
		-DDEBUG

CCFLAGS_RELEASE := -g3 \
		-O2 \
		-march=native \
		-mtune=native \
		-ftree-vectorize \
		-DDEBUG

ifneq (,$(findstring clang,$(CC_VERSION)))
	# if clang, add vectorize
	CCFLAGS_RELEASE += -fslp-vectorize
else
	#if gcc, add lto
	CCFLAGS_RELEASE += -flto
endif


INCS = -I$(SDL_PATH)/include \
	-I/usr/include/malloc \
	-I/usr/X11R6/include

LIBS = ./build/libSDL2.a ./build/libSDL2main.a \
	-lm -liconv \
	-framework Cocoa -framework Carbon -framework OpenGL \
	-framework CoreAudio -framework AudioToolbox -framework AudioUnit \
	-framework ForceFeedback -framework IOKit

EXECUTABLE:=prototype
SOURCE:=src/main.c
OBJECTS=$(patsubst src%.c,build%.o, $(SOURCE))
# DEV_CFLAGS = $(CCFLAGS) -g -Wall
# REL_CFLAGS = $(CCFLAGS) -Wall -O2

debug: CCFLAGS += $(CCFLAGS_DEBUG)
debug: $(EXECUTABLE)

release: CCFLAGS += $(CCFLAGS_RELEASE)
release: $(EXECUTABLE)

all: debug

$(EXECUTABLE): $(OBJECTS)
		install -d build
		$(CC) -o $@ $^ $(CCFLAGS) $(LIBS)

build/%.o: src/%.c
		$(CC) -c $< -o $@ $(CCFLAGS) $(INCS)

clean:
	rm -f build/*.o || true
	rm -f $(EXECUTABLE) || true

# all:
# 	$(CC) -c src/main.c $(DEV_CFLAGS) $(INCS)
# 	$(CC) -o prototype *.o $(DEV_CFLAGS) $(LIBS)
# 	rm *.o

.PHONY: all debug release
