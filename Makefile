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

all: $(C8) $(C8DIS) $(C8AS) $(LIBC8)

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
	mkdir -pv $(OBJDIR)

$(LIBOBJDIR):
	mkdir -pv $(LIBOBJDIR)/internal

$(INCLUDEDIR):
	mkdir -pv $(INCLUDEDIR)

$(BINDIR):
	mkdir -pv $(BINDIR)

$(LIBDIR):
	mkdir -pv $(LIBDIR)

clean:
	rm -rfv $(BUILDDIR)

install: $(C8) $(C8DIS) $(C8AS) $(LIBC8)
	cp -v $(C8) $(C8DIS) $(C8AS_TARG) $(INSTALLDIR)/bin
	chmod 755 $(INSTALLDIR)/bin/$(C8)
	chmod 755 $(INSTALLDIR)/bin/$(C8AS)
	chmod 755 $(INSTALLDIR)/bin/$(C8DIS)

uninstall:
	rm -rfv $(INSTALLDIR)/bin/$(C8) \
	      $(INSTALLDIR)/bin/$(C8DIS) \
		  $(INSTALLDIR)/bin/$(C8AS) \
		  $(INSTALLDIR)/include/c8 \
		  $(INSTALLDIR)/lib/$(LIBC8)

.PHONY: all clean install uninstall
.PHONY: $(C8) $(C8DIS) $(C8AS) $(LIBC8)
