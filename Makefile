CC=g++
CFLAGS=-g
TARGET:test.exe CommandParser/libcli.a
LIBS=-lpthread -L ./CommandParser -lcli
OBJS=gluethread/glthread.o \
                  graph.o \
                  topology.o \
		  net.o	\
		  nwcli.o \
		  utils.o


test.exe:testapp.o ${OBJS} CommandParser/libcli.a
	${CC} ${CFLAGS} testapp.o ${OBJS} -o test.exe ${LIBS}

testapp.o:testapp.cpp
	${CC} ${CFLAGS} -c testapp.cpp -o testapp.o

gluethread/glthread.o:gluethread/glthread.c
	${CC} ${CFLAGS} -c -I gluethread gluethread/glthread.c -o gluethread/glthread.o

graph.o:graph.cpp
	${CC} ${CFLAGS} -c -I . graph.cpp -o graph.o

topology.o:topology.cpp
	${CC} ${CFLAGS} -c -I . topology.cpp -o topology.o

net.o:net.cpp
	${CC} ${CFLAGS} -c -I . net.cpp -o net.o

nwcli.o:nwcli.cpp
	${CC} ${CFLAGS} -c -I . nwcli.cpp  -o nwcli.o
	
utils.o:utils.cpp
	${CC} ${CFLAGS} -c -I . utils.cpp -o utils.o

CommandParser/libcli.a:
	(cd CommandParser; make)

clean:
	(cd CommandParser;  make clean)
	rm *.o
	rm gluethread/glthread.o
	rm *exe
all:
	make
	(cd CommandParser; make)
