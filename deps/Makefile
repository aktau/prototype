# This file is part of prototype, it compiles the dependencies
#
# (c) 2009 Salvatore Sanfillipo
# (c) 2013 Nicolas Hillegeer <nicolas@hillegeer.com>
#
# For the full copyright and license information, please view the LICENSE
# file that was distributed with the source code.

CCCOLOR="\033[34m"
LINKCOLOR="\033[34;1m"
SRCCOLOR="\033[33m"
BINCOLOR="\033[37;1m"
MAKECOLOR="\033[32;1m"
ENDCOLOR="\033[0m"

CC_VERSION := $(shell $(CC) --version | head -1 | cut -f1 -d' ')

SDL_CC ?= gcc

ifneq (,$(findstring clang,$(CC_VERSION)))
	SDL_CC := $(CC)
endif

default:
	@echo "Explicit target required"

.PHONY: default

# Prerequisites target
.make-prerequisites:
	@touch $@

# clean everything when CC is different
ifneq ($(shell sh -c '[ -f .make-cc ] && cat .make-cc || echo none'), $(CC))
.make-cc: distclean
	@echo "CC is different, should be $(CC), cleaned"
	-(echo "$(CC)" > .make-cc)
.make-prerequisites: .make-cc
endif

# clean everything when CFLAGS is different
ifneq ($(shell sh -c '[ -f .make-cflags ] && cat .make-cflags || echo none'), $(CFLAGS))
.make-cflags: distclean
	@echo "CFLAGS are different, should be $(CFLAGS), cleaned"
	-(echo "$(CFLAGS)" > .make-cflags)
.make-prerequisites: .make-cflags
endif

# clean everything when LDFLAGS is different
ifneq ($(shell sh -c '[ -f .make-ldflags ] && cat .make-ldflags || echo none'), $(LDFLAGS))
.make-ldflags: distclean
	@echo "LDFLAGS are different, should be $(LDFLAGS), cleaned"
	-(echo "$(LDFLAGS)" > .make-ldflags)
.make-prerequisites: .make-ldflags
endif

distclean:
	-(cd sdl2 && $(MAKE) clean && CC=$(SDL_CC) ./configure) || true
	-(cd lua && $(MAKE) clean) || true
	-(grep "" .make-*)

#-(rm -f .make-*)
#gsed -i 's|-fpascal-strings||' Makefile

.PHONY: distclean

sdl2: .make-prerequisites
	@printf '%b %b\n' $(MAKECOLOR)MAKE$(ENDCOLOR) $(BINCOLOR)$@$(ENDCOLOR)
	cd sdl2 && $(MAKE) CC="$(SDL_CC)"

LUA_OS ?= macosx

lua: .make-prerequisites
	@printf '%b %b\n' $(MAKECOLOR)MAKE$(ENDCOLOR) $(BINCOLOR)$@$(ENDCOLOR)
	cd lua/src && $(MAKE) CC="$(CC)" HOST_CC="$(CC)"

.PHONY: lua sdl2
