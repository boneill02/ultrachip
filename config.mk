VERSION = $(shell git rev-parse --short HEAD)

# paths
PREFIX = /usr/local
MANPREFIX = $(PREFIX)/share/man
SRCPREFIX = $(PWD)/src
C8_SRCPREFIX = $(PWD)/src/c8
C8AS_SRCPREFIX = $(PWD)/src/c8as
C8DIS_SRCPREFIX = $(PWD)/src/c8dis
UTILPREFIX = $(PWD)/src/util

# flags
CPPFLAGS    = -D_DEFAULT_SOURCE -D_XOPEN_SOURCE=700 -D_POSIX_C_SOURCE=200809L -DVERSION=\"$(VERSION)\"
CFLAGS      = -std=gnu99 -pedantic -Werror -O2 -I$(SRCPREFIX) $(CPPFLAGS)
LDFLAGS     =
C8_LIBS     = -lSDL2

CC = gcc
