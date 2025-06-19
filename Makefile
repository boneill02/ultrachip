include mk/config/config.mk

C8_SRC = $(C8_SRCPREFIX)/chip8.c $(C8_SRCPREFIX)/debug.c \
		 $(C8_SRCPREFIX)/font.c $(C8_SRCPREFIX)/graphics.c \
		 $(C8_SRCPREFIX)/graphics_sdl.c $(C8_SRCPREFIX)/main.c \
		 $(UTILPREFIX)/decode.c $(UTILPREFIX)/exception.c $(UTILPREFIX)/util.c
C8_OBJ = $(patsubst %.c, %.o, $(C8_SRC))
C8_TARG = c8

C8AS_SRC = $(C8AS_SRCPREFIX)/main.c $(C8AS_SRCPREFIX)/parse.c \
           $(C8AS_SRCPREFIX)/symbol.c $(UTILPREFIX)/exception.c \
		   $(UTILPREFIX)/util.c
C8AS_OBJ = $(patsubst %.c, %.o, $(C8AS_SRC))
C8AS_TARG = c8as

C8DIS_SRC = $(C8DIS_SRCPREFIX)/dis.c $(C8DIS_SRCPREFIX)/main.c \
            $(UTILPREFIX)/decode.c $(UTILPREFIX)/exception.c \
			$(UTILPREFIX)/util.c
C8DIS_OBJ = $(patsubst %.c, %.o, $(C8DIS_SRC))
C8DIS_TARG = c8dis

all: $(C8_TARG) $(C8DIS_TARG) $(C8AS_TARG)

.c.o:
	$(CC) $(CFLAGS) -o $@ -c $<

$(C8_TARG): $(C8_OBJ)
	$(CC) -o $@ $(C8_OBJ) $(LDFLAGS) $(C8_LIBS)

$(C8AS_TARG): $(C8AS_OBJ)
	$(CC) -o $@ $(C8AS_OBJ) $(LDFLAGS)

$(C8DIS_TARG): $(C8DIS_OBJ)
	$(CC) -o $@ $(C8DIS_OBJ) $(LDFLAGS)

clean:
	rm -rf $(C8_TARG) $(C8_OBJ) $(C8DIS_TARG) $(C8DIS_OBJ) $(C8AS_TARG) \
	      $(C8AS_OBJ) build/

install: $(C8_TARG) $(C8DIS_TARG) $(C8AS_TARG)
	cp $(C8_TARG) $(C8DIS_TARG) $(C8AS_TARG) $(PREFIX)/bin
	chmod 755 $(DESTDIR)$(PREFIX)/bin/$(C8_TARG)
	chmod 755 $(DESTDIR)$(PREFIX)/bin/$(C8AS_TARG)
	chmod 755 $(DESTDIR)$(PREFIX)/bin/$(C8DIS_TARG)


test-util: $(UNITY_PATH)
	@make -f mk/test-util.mk

test-c8as: $(UNITY_PATH)
	@make -f mk/test-c8as.mk

test-c8: $(UNITY_PATH)
	@make -f mk/test-c8.mk

test: test-util test-c8as test-c8

uninstall:
	rm -f $(DESTDIR)$(PREFIX)/bin/$(C8_TARG) \
	      $(DESTDIR)$(PREFIX)/bin/$(C8DIS_TARG) \
		  $(DESTDIR)$(PREFIX)/bin/

.PHONY: all $(C8_TARG) $(C8DIS_TARG) $(C8AS_TARG) clean install uninstall
.PHONY: test-c8 test-c8as test-util
