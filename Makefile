CC=g++
CFLAGS=-g
TARGET:test.exe
LIBS=-lpthread
OBJS=gluethread/glthread.o \
                  graph.o \
                  topology.o \
		  net.o	\
		  utils.o \
		  comm.o


test.exe:testapp.o ${OBJS} 
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
	
utils.o:utils.cpp
	${CC} ${CFLAGS} -c -I . utils.cpp -o utils.o

comm.o:comm.cpp
	${CC} ${CFLAGS} -c -I . comm.cpp -o comm.o

clean:
	rm *.o
	rm gluethread/glthread.o
	rm *exe
all:
	make
