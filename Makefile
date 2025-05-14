include config.mk

SRC = ${SRCPREFIX}/chip8.c ${SRCPREFIX}/decode.c
OBJ = ${SRC:.c=.o}
TARG = c8

DIS_OBJ = ${SRCPREFIX}/dis.o ${SRCPREFIX}/decode.o
DIS_TARG = c8dis

all: ${TARG} c8dis

.c.o:
	${CC} -c ${CFLAGS} $<

${OBJ}: config.mk

${TARG}: ${OBJ}
	${CC} -o $@ ${OBJ} ${LDFLAGS}

${DIS_TARG}: ${DIS_OBJ}
	${CC} -o $@ decode.o dis.o ${LDFLAGS}

clean:
	rm -f ${TARG} ${OBJ} ${DIS_TARG} ${DIS_OBJ}

run: ${TARG}
	./${TARG}

install: ${TARG} ${DIS_TARG}
	cp ${TARG} ${DIS_TARG} $(PREFIX)/bin
	chmod 755 ${DESTDIR}${PREFIX}/bin/${TARG}
	chmod 755 ${DESTDIR}${PREFIX}/bin/${DIS_TARG}

uninstall:
	rm -f ${PREFIX}/bin/${TARG} ${PREFIX}/bin/${DIS_TARG}

.PHONY: all clean run install uninstall