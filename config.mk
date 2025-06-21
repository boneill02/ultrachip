VERSION      = $(shell git rev-parse --short HEAD)

UNITY_PATH   = Unity

# source paths
SRCDIR         = src
TESTDIR        = test
C8_SRCDIR      = $(SRCDIR)/c8
C8AS_SRCDIR    = $(SRCDIR)/c8as
C8DIS_SRCDIR   = $(SRCDIR)/c8dis
LIBC8_SRCDIR   = $(SRCDIR)/libc8

# build paths
BUILDDIR       = build
OBJDIR         = $(BUILD_PATH)/obj
BINDIR         = $(BUILD_PATH)/bin
INCLUDEDIR     = $(BUILD_PATH)/include
LIBDIR         = $(BUILD_PATH)/lib

# install paths
INSTALLDIR     = /usr/local
MAN_INSTALLDIR = $(INSTALLPREFIX)/share/man
MAN_SRCDIR     = doc/man

# flags
INCLUDE_PATHS  = -I$(LIBC8_SRCPREFIX)
CPPFLAGS       = -D_DEFAULT_SOURCE -D_XOPEN_SOURCE=700 \
                 -D_POSIX_C_SOURCE=200809L -DVERSION=\"$(VERSION)\"
CFLAGS         = -std=gnu99 -pedantic -Werror -O2 -I$(SRCPREFIX) $(INCLUDES) $(CPPFLAGS)
LDFLAGS        = -L$(LIBDIR) -lc8
C8_LIBS        = -lSDL2

CC = gcc
AR = ar rcs
