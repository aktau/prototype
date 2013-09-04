CC := /usr/local/bin/gcc-4.8
# CC := /usr/local/bin/clang
CC_VERSION := $(shell $(CC) --version | head -1 | cut -f1 -d' ')

STD := -std=c11 -pedantic
SECURITY := -D_THREAD_SAFE -D_FORTIFY_SOURCE=2
WARN := -Wextra -Wcast-align -Wcast-qual \
	-Wpointer-arith -Waggregate-return -Wunreachable-code \
	-Wfloat-equal -Wformat=2 -Wredundant-decls \
	-Wundef -Wdisabled-optimization -Wshadow \
	-Wmissing-braces -Wstrict-aliasing=2 -Wstrict-overflow=5 \
	-Wconversion -Wno-unused-parameter \
	-Wno-missing-field-initializers -Wno-missing-braces
DEBUG := -g3 -DDEBUG
OPT := -O2 -march=native -mtune=native -ftree-vectorize -ffast-math

ifeq ($(OS),Windows_NT)
	# nothing special
else
	UNAME_S := $(shell uname -s)

	ifneq (,$(findstring clang,$(CC_VERSION)))
		# -pthread is not necessary when using Clang on Darwin
		ifneq ($(UNAME_S),Darwin)
			STD += -pthread
		endif
	else
		# at least on OS X 10.7.5, the apple linker does not understand AVX, and gcc uses it natively
		ifeq ($(UNAME_S),Darwin)
			# OPT += -mno-avx
		endif
	endif
endif

# clang doesn't perform link-time optimization without the gold linker, which as of yet
# is not the default, so for now I'm not bothering to make a wrapper script to make it
# use the gold linker. So don't perform LTO with clang for now. The slp vectorizer is cool though
ifneq (,$(findstring clang,$(CC_VERSION)))
	OPT += -fslp-vectorize
else
	OPT += -flto
endif

CFLAGS ?= $(STD) $(OPT)

matmul: matmul.c
	$(CC) $^ -o $@ -I. $(CFLAGS)

clean:
	-rm -f matmul

.PHONY: clean