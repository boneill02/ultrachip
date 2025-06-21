include config.mk

C8 = $(BINDIR)/c8
C8_SRC = $(C8_SRCDIR)/main.c $(C8_SRCDIR)/graphics_sdl.c
C8_OBJ = $(patsubst %.c, %.o, $(C8_SRC))

C8AS = $(BINDIR)/c8as
C8AS_SRC = tools/c8as.c
C8DIS_OBJ = $(patsubst $(C8DIS_SRCDIR)/%.c,$(OBJDIR)/%.o,$(LIBC8_SRC))

C8DIS = $(BINDIR)/c8dis
C8DIS_SRC = c8dis.c
C8DIS_OBJ = $(patsubst $(C8DIS_SRCDIR)/%.c,$(OBJDIR)/%.o,$(LIBC8_SRC))

LIBC8 = $(LIBDIR)/libc8.a
LIBC8_SRC = $(LIBC8_SRCDIR)/chip8.c $(LIBC8_SRCDIR)/decode.c \
            $(LIBC8_SRCDIR)/encode.c $(LIBC8_SRCDIR)/font.c \
			$(LIBC8_SRCDIR)/graphics.c \
			$(LIBC8_SRCDIR)/internal/debug.c \
			$(LIBC8_SRCDIR)/internal/exception.c \
			$(LIBC8_SRCDIR)/internal/symbol.c \
			$(LIBC8_SRCDIR)/internal/util.c
LIBC8_OBJ = $(patsubst $(LIBC8_SRCDIR)/%.c,$(OBJDIR)/%.o,$(LIBC8_SRC))
LIBC8_INCLUDE = $(LIBC8_SRCDIR)/chip8.h $(LIBC8_SRCDIR)/decode.h \
                $(LIBC8_SRCDIR)/defs.h $(LIBC8_SRCDIR)/encode.h \
				$(LIBC8_SRCDIR)/graphics.h

all: $(C8) $(C8DIS) $(C8AS) $(LIBC8)

$(OBJDIR)/%.o: $(LIBC8_SRCDIR)/%.c $(OBJDIR)
	$(CC) $(CFLAGS) -o $@ -c $<

$(C8): $(LIBC8) $(C8_OBJ) $(BINDIR)
	$(CC) -o $@ $(C8_OBJ) $(LDFLAGS) -lSDL2

$(C8AS): $(LIBC8) $(C8AS_OBJ) $(BINDIR)
	$(CC) -o $@ $(C8AS_OBJ) $(LDFLAGS)

$(C8DIS): $(LIBC8) $(C8DIS_OBJ) $(BINDIR)
	$(CC) -o $@ $(C8DIS_OBJ) $(LDFLAGS)

$(LIBC8): $(INCLUDEDIR) $(LIBDIR) $(LIBC8_OBJ)
	$(AR) $@ $(LIBC8_OBJ)
	cp $(LIBC8_INCLUDE) $(INCLUDEDIR)

$(OBJDIR):
	mkdir -p $(OBJDIR)/internal

$(INCLUDEDIR):
	mkdir -p $(INCLUDEDIR)

$(BINDIR):
	mkdir -p $(BINDIR)

$(LIBDIR):
	mkdir -p $(LIBDIR)

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
