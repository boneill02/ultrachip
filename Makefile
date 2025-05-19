include config.mk

C8_SRC = ${C8_SRCPREFIX}/chip8.c ${C8_SRCPREFIX}/debug.c \
         ${C8_SRCPREFIX}/graphics.c ${C8_SRCPREFIX}/main.c \
		 ${UTILPREFIX}/decode.c ${UTILPREFIX}/util.c
C8_OBJ = $(patsubst %.c, %.o, ${C8_SRC})
C8_TARG = c8

C8DIS_SRC = ${C8DIS_SRCPREFIX}/dis.c ${C8DIS_SRCPREFIX}/main.c \
            ${UTILPREFIX}/decode.c
C8DIS_OBJ = $(patsubst %.c, %.o, ${C8DIS_SRC})
C8DIS_TARG = c8dis

all: ${C8_TARG} ${C8DIS_TARG}

.c.o:
	${CC} ${CFLAGS} -o $@ -c $<

${C8_TARG}: ${C8_OBJ}
	${CC} -o $@ ${C8_OBJ} ${LDFLAGS}

${C8DIS_TARG}: ${C8DIS_OBJ}
	${CC} -o $@ ${C8DIS_OBJ} ${LDFLAGS}

clean:
	rm -f ${C8_TARG} ${C8_OBJ} ${C8DIS_TARG} ${C8DIS_OBJ}

install: ${C8_TARG} ${C8DIS_TARG}
	cp ${C8_TARG} ${C8DIS_TARG} $(PREFIX)/bin
	chmod 755 ${DESTDIR}${PREFIX}/bin/${C8_TARG}
	chmod 755 ${DESTDIR}${PREFIX}/bin/${C8DIS_TARG}

uninstall:
	rm -f ${PREFIX}/bin/${C8_TARG} ${PREFIX}/bin/${C8DIS_TARG}

.PHONY: all ${C8_TARG} ${C8DIS_TARG} clean run install uninstall