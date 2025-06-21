include config.mk

C8 = c8
C8_SRC = $(C8_SRCDIR)/main.c $(C8_SRCDIR)/graphics_sdl.c
C8_OBJ = $(patsubst %.c, %.o, $(C8_SRC))

C8AS = c8as
C8AS_SRC = c8as.c
C8DIS_OBJ = $(patsubst $(C8DIS_SRCDIR)/%.c,$(OBJDIR)/%.o,$(LIBC8_SRC))

C8DIS = c8dis
C8DIS_SRC = c8dis.c
C8DIS_OBJ = $(patsubst $(C8DIS_SRCDIR)/%.c,$(OBJDIR)/%.o,$(LIBC8_SRC))

LIBC8 = libc8.a
LIBC8_SRC = $(LIBC8_SRCDIR)/decode.c $(LIBC8_SRCDIR)/encode.c \
			$(LIBC8_SRCDIR)/font.c $(LIBC8_SRCIR)/graphics.c \
			$(LIBC8_SRCDIR)/symbol.c $(LIBC8_SRCDIR)/util.c \
			$(LIBC8_SRCDIR)/internal/debug.c \
			$(LIBC8_SRCDIR)/internal/exception.c \
			$(LIBC8_SRCDIR)/internal/symbol.c \
			$(LIBC8_SRCDIR)/internal/util.c
LIBC8_OBJ = $(patsubst $(LIBC8_SRCDIR)/%.c,$(OBJDIR)/%.o,$(LIBC8_SRC))
LIBC8_INCLUDE = chip8.h decode.h defs.h encode.h graphics.h

all: $(C8) $(C8DIS) $(C8AS) $(LIBC8)

.c.o:
	$(CC) $(CFLAGS) -o $@ -c $<

$(C8): $(C8_OBJ) $(LIBC8)
	$(CC) -o $@ $(C8_OBJ) $(LDFLAGS) $(C8_LIBS)

$(C8AS): $(C8AS_OBJ) $(LIBC8)
	$(CC) -o $@ $(C8AS_OBJ) $(LDFLAGS)

$(C8DIS): $(C8DIS_OBJ) $(LIBC8)
	$(CC) -o $@ $(C8DIS_OBJ) $(LDFLAGS)

$(LIBC8_INCLUDE):
	cp -f $(LIBC8_INCLUDE) $(INCLUDE_PATH)

$(LIBC8): $(LIBC8_OBJ) $(LIBC8_INCLUDE)
	$(AR) $@ $(LIBC8_OBJ)

clean:
	rm -f $(OBJDIR)/* $(BINDIR)/* $(LIBDIR)/* $(INCLUDEDIR)/*

install: $(C8) $(C8DIS) $(C8AS) $(LIBC8)
	cp $(C8) $(C8DIS) $(C8AS_TARG) $(INSTALLDIR)/bin
	chmod 755 $(INSTALLDIR)/bin/$(C8)
	chmod 755 $(INSTALLDIR)/bin/$(C8AS)
	chmod 755 $(INSTALLDIR)/bin/$(C8DIS)

uninstall:
	rm -rf $(INSTALLDIR)/bin/$(C8) \
	      $(INSTALLDIR)/bin/$(C8DIS) \
		  $(INSTALLDIR)/bin/$(C8AS) \
		  $(INSTALLDIR)/include/c8 \
		  $(INSTALLDIR)/lib/$(LIBC8)

.PHONY: all clean install uninstall
.PHONY: $(C8_TARG) $(C8DIS_TARG) $(C8AS_TARG) $(LIBC8_TARG)
