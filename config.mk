VERSION      = $(shell git rev-parse --short HEAD)

UNITY_PATH   = Unity

# SDL2 stuff (uncomment for SDL2 support in libc8)
#SDL2_PATH      = 
#SDL2_CFLAGS    = -DSDL2
#SDL2_LDFLAGS   = -lSDL2

# source paths
TOOLSDIR       = tools
LIBC8_SRCDIR   = libc8/c8

# build paths
BUILDDIR       = build
OBJDIR         = $(BUILDDIR)/obj
LIBOBJDIR      = $(BUILDDIR)/obj/libc8
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
CFLAGS         = -std=gnu99 -pedantic -Werror -O2 -Ilibc8 $(INCLUDES) $(CPPFLAGS) $(SDL2_CFLAGS)
LDFLAGS        = -L$(LIBDIR) -lc8 $(SDL2_LDFLAGS)
LIBC8_CFLAGS   = $(CFLAGS) -fPIC
LIBC8_LDFLAGS  = -fPIC -shared -Wl,--version-script=libc8/libc8.version $(SDL2_LDFLAGS)

CC = gcc
AR = ar rcs
