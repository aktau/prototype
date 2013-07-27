# Makefile for osx

SDL_PATH=./deps/sdl2

CCFLAGS = -D_THREAD_SAFE -std=c11
DEV_CFLAGS = $(CCFLAGS) -g -Wall
REL_CFLAGS = $(CCFLAGS) -Wall -O2

CC = /usr/local/bin/gcc-4.8
#CC = /usr/local/bin/clang
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
		ifeq ($(UNAME_S),Darwin)
		else
			CCFLAGS += -pthread
		endif
	else
		CCFLAGS += -D GCC
		CCFLAGS += -pthread
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

INCS = -I$(SDL_PATH)/include \
	-I/usr/include/malloc \
	-I/usr/X11R6/include

LIBS = ./build/libSDL2.a ./build/libSDL2main.a \
	-lm -liconv \
	-framework Cocoa -framework Carbon -framework OpenGL \
	-framework CoreAudio -framework AudioToolbox -framework AudioUnit \
	-framework ForceFeedback -framework IOKit

all:
	$(CC) -c src/main.c $(DEV_CFLAGS) $(INCS)
	$(CC) -o prototype *.o $(DEV_CFLAGS) $(LIBS)
	rm *.o
