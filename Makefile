CFLAGS=-g -Wall
BINDIR=${DESTDIR}${PREFIX}/bin
INSTALL_BIN=install -m 0755
OBJ=spp.o
BIN=spp

all: ${BIN}

config.h: config.def.h
	cp config.def.h config.h

${BIN}: ${OBJ} config.h
	${CC} ${LDFLAGS} -o ${BIN} ${OBJ}

install:
	${INSTALL_BIN} spp ${BINDIR}

clean:
	-rm -f ${BIN} ${OBJ}
