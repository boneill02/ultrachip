include config.mk

SRC = ${SRCPREFIX}/chip8.c ${SRCPREFIX}/decode.c
OBJ = ${SRC:.c=.o}
TARG = chip8

DIS_OBJ = ${SRCPREFIX}/dis.o ${SRCPREFIX}/decode.o
DIS_TARG = dis

all: ${TARG} dis

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

.PHONY: all clean run
