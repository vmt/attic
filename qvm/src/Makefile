###############################################################################
# Makefile
###############################################################################

CPP=		g++ -I../include -L.
CPPFLAGS+=	-g -Wall -ansi -pedantic `xml2-config --cflags` -DWITH_X
LDFLAGS+=	`xml2-config --libs` -lkvm
MKDEP=		g++ -M -I.

###############################################################################
# Source/Object Files
###############################################################################

SRC=	bin.cc wrap.cc cpu-freebsd.cc packetio.cc \
	sock.cc nsrv.cc auth.cc queue.cc packet.cc \
	qvmtask.cc qvmstatus.cc server.cc

OBJS=	bin.o wrap.o cpu-freebsd.o packetio.o \
	sock.o nsrv.o auth.o queue.o packet.o \
	qvmtask.o qvmstatus.o server.o

###############################################################################
# Rules
###############################################################################

.cc.o:
	${CPP} ${CPPFLAGS} -c $< -o $@

all:	libqvm.a

libqvm.a: ${OBJS}
	ar r libqvm.a ${OBJS}

clean:
	rm -f ${OBJS} libqvm.a *.core
