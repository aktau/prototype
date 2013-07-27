# Makefile for osx

# alternatives that I've tested: gcc (the apple gcc frontend for gcc 4.2, that is)
#CC=gcc-4.8
CC=/usr/local/bin/clang

SDL_PATH=./deps/sdl2

COMMON_CFLAGS = -D_THREAD_SAFE -std=c11
DEV_CFLAGS = $(COMMON_CFLAGS) -g -Wall
REL_CFLAGS = $(COMMON_CFLAGS) -Wall -O2

INCS = -I$(SDL_PATH)/include -I/usr/include/malloc -I/usr/X11R6/include

LIBS = ./build/libSDL2.a ./build/libSDL2main.a \
	-pthread \
	-lm -liconv \
	-framework Cocoa -framework Carbon -framework OpenGL \
	-framework CoreAudio -framework AudioToolbox -framework AudioUnit \
	-framework ForceFeedback -framework IOKit

all:
	$(CC) -c src/main.c $(DEV_CFLAGS) $(INCS)
	$(CC) -o prototype *.o $(DEV_CFLAGS) $(LIBS)
	rm *.o
