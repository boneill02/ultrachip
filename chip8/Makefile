include config.mk

SRC = chip8.c decode.c
OBJ = ${SRC:.c=.o}
DIS_TARG = dis
DIS_OBJ = dis.o decode.o
TARG = chip8

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
