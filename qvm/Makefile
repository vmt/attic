###############################################################################
# Makefile
###############################################################################

CPP=		g++ -Iinclude
CPPFLAGS=	`xml2-config --cflags` -DWITH_X
LDFLAGS+=	`xml2-config --libs` -Lsrc -lqvm -lkvm
MKDEP=		g++ -M -I.

###############################################################################
# Rules
###############################################################################
.cc.o:
	${CPP} ${CPPFLAGS} -c $< -o $@

all:	
	cd src   && make
	cd tasks && make
	${CPP} ${CPPFLAGS} main.cc -o nsrv ${LDFLAGS}
	${CPP} ${CPPFLAGS} server-main.cc -o vmsrv ${LDFLAGS}
	chmod 4755 nsrv
	chmod 4755 vmsrv
clean:
	rm -f ${OBJS} *.core
	cd src   && make clean
	cd tasks && make clean
