VERSION = $(shell git rev-parse --short HEAD)

# paths
PREFIX = /usr/local
MANPREFIX = $(PREFIX)/share/man
SRCPREFIX = src
TESTPREFIX = test
C8_SRCPREFIX = src/c8
C8AS_SRCPREFIX = src/c8as
C8DIS_SRCPREFIX = src/c8dis
UTILPREFIX = src/util
UNITY_PATH = Unity

# flags
CPPFLAGS    = -D_DEFAULT_SOURCE -D_XOPEN_SOURCE=700 -D_POSIX_C_SOURCE=200809L -DVERSION=\"$(VERSION)\"
CFLAGS      = -std=gnu99 -pedantic -Werror -O2 -I$(SRCPREFIX) $(CPPFLAGS) -g
LDFLAGS     = -g
C8_LIBS     = -lSDL2

CC = gcc
