CFLAGS=-g -Wall -MD
PREFIX?=/usr
BINDIR=${DESTDIR}${PREFIX}/bin
INSTALL_BIN=install -m 0755
OBJ=spp.o
BIN=spp

all: ${BIN}

config.h:
	cp config.def.h config.h

${BIN}: config.h ${OBJ}
	${CC} ${LDFLAGS} -o ${BIN} ${OBJ}
	#ln -s ${BIN} cpp
	#ln -s ${BIN} pod
	#ln -s ${BIN} sh

install:
	${INSTALL_BIN} spp ${BINDIR}

clean:
	-rm -f ${BIN} ${OBJ}

-include spp.d
