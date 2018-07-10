# $Id: Makefile,v 1.20 2018-01-31 18:30:15-08 - - $

MKFILE      = Makefile
DEPFILE     = ${MKFILE}.dep
NOINCL      = ci clean spotless
NEEDINCL    = ${filter ${NOINCL}, ${MAKECMDGOALS}}
GMAKE       = ${MAKE} --no-print-directory

COMPILECPP  = g++ -std=gnu++17 -g -O0 -Wall -Wextra -Wold-style-cast
MAKEDEPCPP  = g++ -std=gnu++17 -MM

CPPHEADER   = listmap.h trace.h util.h xless.h xpair.h debug.h
TCCSOURCE   = listmap.tcc util.tcc
CPPSOURCE   = util.cpp main.cpp debug.cpp
ALLCPPSRC   = ${CPPHEADER} ${TCCSOURCE} ${CPPSOURCE}
OBJECTS     = ${CPPSOURCE:.cpp=.o}
EXECBIN     = keyvalue
OTHERS      = ${MKFILE} ${DEPFILE}
ALLSOURCES  = ${ALLCPPSRC} ${OTHERS}
LISTING     = Listing.ps

all : ${EXECBIN}

${EXECBIN} : ${OBJECTS}
	${COMPILECPP} -o $@ ${OBJECTS}

%.o : %.cpp
	${COMPILECPP} -c $<

ci : ${ALLSOURCES}
	cid + ${ALLSOURCES}
	- checksource ${ALLSOURCES}
	- cpplint.py.perl ${CPPSOURCE}

lis : ${ALLSOURCES}
	mkpspdf ${LISTING} ${ALLSOURCES}

clean :
	- rm ${OBJECTS} ${DEPFILE} core

spotless : clean
	- rm ${EXECBIN} ${LISTING} ${LISTING:.ps=.pdf}

dep : ${ALLCPPSRC}
	@ echo "# ${DEPFILE} created `LC_TIME=C date`" >${DEPFILE}
	${MAKEDEPCPP} ${CPPSOURCE} >>${DEPFILE}

${DEPFILE} :
	@ touch ${DEPFILE}
	${GMAKE} dep

again :
	${GMAKE} spotless dep ci all lis

ifeq (${NEEDINCL}, )
include ${DEPFILE}
endif

