include config.mk

C8_SRC = ${SRCPREFIX}/chip8.c ${SRCPREFIX}/decode.c ${SRCPREFIX}/graphics.c
C8_OBJ = $(patsubst %.c, %.o, ${C8_SRC})
C8_TARG = c8

DIS_SRC = ${SRCPREFIX}/dis.c ${SRCPREFIX}/decode.c
DIS_OBJ = $(patsubst %.c, %.o, ${DIS_SRC})
DIS_TARG = c8dis

all: ${C8_TARG} ${DIS_TARG}

.c.o:
	${CC} ${CFLAGS} -o $@ -c $<

${C8_TARG}: ${C8_OBJ}
	${CC} -o $@ ${C8_OBJ} ${LDFLAGS}

${DIS_TARG}: ${DIS_OBJ}
	${CC} -o $@ ${DIS_OBJ} ${LDFLAGS}

clean:
	rm -f ${C8_TARG} ${C8_OBJ} ${DIS_TARG} ${DIS_OBJ}

install: ${C8_TARG} ${DIS_TARG}
	cp ${C8_TARG} ${DIS_TARG} $(PREFIX)/bin
	chmod 755 ${DESTDIR}${PREFIX}/bin/${C8_TARG}
	chmod 755 ${DESTDIR}${PREFIX}/bin/${DIS_TARG}

uninstall:
	rm -f ${PREFIX}/bin/${C8_TARG} ${PREFIX}/bin/${DIS_TARG}

.PHONY: all c8 c8dis clean run install uninstall