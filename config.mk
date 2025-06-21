VERSION      = $(shell git rev-parse --short HEAD)

UNITY_PATH   = Unity

# source paths
C8_SRCDIR      = tools/c8
C8AS_SRCDIR    = tools
C8DIS_SRCDIR   = tools
LIBC8_SRCDIR   = libc8/c8

# build paths
BUILDDIR       = build
OBJDIR         = $(BUILDDIR)/obj
BINDIR         = $(BUILDDIR)/bin
INCLUDEDIR     = $(BUILDDIR)/include
LIBDIR         = $(BUILDDIR)/lib

# install paths
INSTALLDIR     = /usr/local
MAN_INSTALLDIR = $(INSTALLPREFIX)/share/man
MAN_SRCDIR     = doc/man

# flags
INCLUDE_PATHS  = -I$(LIBC8_SRCPREFIX)
CPPFLAGS       = -D_DEFAULT_SOURCE -D_XOPEN_SOURCE=700 \
                 -D_POSIX_C_SOURCE=200809L -DVERSION=\"$(VERSION)\"
CFLAGS         = -std=gnu99 -pedantic -Werror -O2 -Ilibc8 $(INCLUDES) $(CPPFLAGS)
LDFLAGS        = -L$(LIBDIR) -lc8
C8_LIBS        = -lSDL2

CC = gcc
AR = ar rcs
