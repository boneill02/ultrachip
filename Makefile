include config.mk

C8 = $(BINDIR)/c8
C8_SRC = $(TOOLSDIR)/c8.c
C8_OBJ = $(patsubst $(TOOLSDIR)/%.c, $(OBJDIR)/%.o, $(C8_SRC))

C8AS = $(BINDIR)/c8as
C8AS_SRC = $(TOOLSDIR)/c8as.c
C8AS_OBJ = $(patsubst $(TOOLSDIR)/%.c,$(OBJDIR)/%.o,$(C8AS_SRC))

C8DIS = $(BINDIR)/c8dis
C8DIS_SRC = $(TOOLSDIR)/c8dis.c
C8DIS_OBJ = $(patsubst $(TOOLSDIR)/%.c,$(OBJDIR)/%.o,$(C8DIS_SRC))

LIBC8 = $(LIBDIR)/libc8.a
LIBC8_SRC = $(LIBC8_SRCDIR)/chip8.c $(LIBC8_SRCDIR)/decode.c \
            $(LIBC8_SRCDIR)/encode.c $(LIBC8_SRCDIR)/font.c \
			$(LIBC8_SRCDIR)/graphics.c \
			$(LIBC8_SRCDIR)/internal/debug.c \
			$(LIBC8_SRCDIR)/internal/exception.c \
			$(LIBC8_SRCDIR)/internal/graphics_sdl.c \
			$(LIBC8_SRCDIR)/internal/symbol.c \
			$(LIBC8_SRCDIR)/internal/util.c
LIBC8_OBJ = $(patsubst $(LIBC8_SRCDIR)/%.c,$(LIBOBJDIR)/%.o,$(LIBC8_SRC))
LIBC8_INCLUDE = $(LIBC8_SRCDIR)/chip8.h $(LIBC8_SRCDIR)/decode.h \
                $(LIBC8_SRCDIR)/defs.h $(LIBC8_SRCDIR)/encode.h \
				$(LIBC8_SRCDIR)/graphics.h

all: c8 c8as c8dis libc8

c8: $(C8)
	@echo c8 built to $(LIBC8)

c8as: $(C8AS)
	@echo c8as built to $(LIBC8)
c8dis: $(C8DIS)
	@echo c8dis built to $(LIBC8)

libc8: $(LIBC8)
	@echo libc8 built to $(LIBC8)

$(LIBOBJDIR)/%.o: $(LIBC8_SRCDIR)/%.c $(LIBOBJDIR)
	$(CC) $(CFLAGS) -o $@ -c $<

$(OBJDIR)/%.o: $(TOOLSDIR)/%.c $(OBJDIR)
	$(CC) $(CFLAGS) -o $@ -c $<

$(C8): $(C8_OBJ) $(LIBC8) $(BINDIR)
	$(CC) -o $@ $< $(LDFLAGS)

$(C8AS): $(C8AS_OBJ) $(LIBC8) $(C8AS_OBJ) $(BINDIR)
	$(CC) -o $@ $< $(LDFLAGS)

$(C8DIS): $(C8DIS_OBJ) $(LIBC8) $(BINDIR)
	$(CC) -o $@ $< $(LDFLAGS)

$(LIBC8): $(INCLUDEDIR) $(LIBDIR) $(LIBC8_OBJ)
	$(AR) $@ $(LIBC8_OBJ) $(SDL2_PATH)
	cp $(LIBC8_INCLUDE) $(INCLUDEDIR)

print-%:
	@echo '$*=$($*)'

$(OBJDIR):
	mkdir -p $(OBJDIR)

$(LIBOBJDIR):
	mkdir -p $(LIBOBJDIR)/internal

$(INCLUDEDIR):
	mkdir -p $(INCLUDEDIR)

$(BINDIR):
	mkdir -p $(BINDIR)

$(LIBDIR):
	mkdir -p $(LIBDIR)

clean:
	rm -rf $(BUILDDIR)

install: $(C8) $(C8DIS) $(C8AS) $(LIBC8)
	cp $(C8) $(C8AS) $(C8DIS) $(INSTALLDIR)/bin
	chmod 755 $(INSTALLDIR)/bin/c8
	chmod 755 $(INSTALLDIR)/bin/c8as
	chmod 755 $(INSTALLDIR)/bin/c8dis
	cp $(LIBC8_INCLUDE) $(INSTALLDIR)/include
	cp $(LIBC8) $(INSTALLDIR)/lib

uninstall:
	rm -rf $(INSTALLDIR)/bin/c8 \
	       $(INSTALLDIR)/bin/c8as \
		   $(INSTALLDIR)/bin/c8dis \
		   $(INSTALLDIR)/include/c8 \
		   $(INSTALLDIR)/lib/libc8.a

.PHONY: all clean install uninstall
.PHONY: c8 c8as c8dis libc8
