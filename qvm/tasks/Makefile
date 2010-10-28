###############################################################################
# Makefile
###############################################################################

CPP=		g++
CPPFLAGS=	-g -Wall -ansi -pedantic -I../include
LDFLAGS+=	-L../src -lqvm
MKDEP=		g++ -M -I.

###############################################################################
# Rules
###############################################################################

TASKS=	basic console master slave frac-master frac-slave

.cc.o:
	${CPP} ${CPPFLAGS} -c $< -o $@

all:	${TASKS}

basic:	basic.cc
	${CPP} -o basic ${CPPFLAGS} basic.cc ${LDFLAGS}
console: console.cc
	${CPP} -o console ${CPPFLAGS} console.cc ${LDFLAGS}
master: master.cc
	${CPP} -o master ${CPPFLAGS} master.cc ${LDFLAGS}
slave: slave.cc
	${CPP} -o slave ${CPPFLAGS} slave.cc ${LDFLAGS}

####### fractal ############
frac-master: frac.o frac-master.cc
	${CPP} -o frac-master ${CPPFLAGS} frac-master.cc frac.o ${LDFLAGS}
frac-slave: frac.o frac-slave.cc
	${CPP} -o frac-slave ${CPPFLAGS} frac-slave.cc frac.o ${LDFLAGS}
frac.o: frac.cc
	${CPP} -o frac.o ${CPPFLAGS} -c frac.cc
	
clean:
	rm -f ${OBJS} ${TASKS} *.core
