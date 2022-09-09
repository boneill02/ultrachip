include config.mk

SRC = chip8.c decode.c
OBJ = ${SRC:.c=.o}
TARG = chip8

all: ${TARG}

.c.o:
	${CC} -c ${CFLAGS} $<

${OBJ}: config.mk

${TARG}: ${OBJ}
	${CC} -o $@ ${OBJ} ${LDFLAGS}

clean:
	rm -f ${TARG} ${OBJ}

run: ${TARG}
	./${TARG}

.PHONY: all clean run
