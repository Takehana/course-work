
GCC        = g++ -g -O0 -Wall -Wextra -std=gnu++14
MKDEP      = g++ -MM -std=gnu++14
VALGRIND   = valgrind --leak-check=full --show-reachable=yes


MKFILE     = Makefile
DEPFILE    = Makefile.dep
SOURCES    = cppstrtok.cpp string_set.cpp main.cpp
HEADERS    = cppstrtok.h string_set.h
OBJECTS    = ${SOURCES:.cpp=.o}
EXECBIN    = oc
SRCFILES   = ${HEADERS} ${SOURCES} ${MKFILE}
CHECKINS   = ${SRCFILES}
LISTING    = Listing.ps

all : ${EXECBIN}

${EXECBIN} : ${OBJECTS}
	${GCC} -o${EXECBIN} ${OBJECTS}

%.o : %.cpp
	${GCC} -c $<

ci :
	cid + ${CHECKINS}
	checksource ${CHECKINS}

clean :
	- rm ${OBJECTS}

spotless : clean
	- rm ${EXECBIN} *.str *.out *.err ${DEPFILE}

${DEPFILE} :
	${MKDEP} ${SOURCES} >${DEPFILE}

dep :
	- rm ${DEPFILE}
	${MAKE} --no-print-directory ${DEPFILE}

include Makefile.dep

again :
	${MAKE} spotless dep all

