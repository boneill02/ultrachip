VERSION = $(shell git rev-parse --short HEAD)

# paths
PREFIX = /usr/local
MANPREFIX = $(PREFIX)/share/man

# flags
LIBS = -lSDL2 -lm
CPPFLAGS = -D_DEFAULT_SOURCE -D_XOPEN_SOURCE=700 -D_POSIX_C_SOURCE=200809L -DVERSION=\"$(VERSION)\"
CFLAGS   = -g -std=c99 -pedantic -Wall -Os $(CPPFLAGS)
LDFLAGS  = $(LIBS)

CC = cc
