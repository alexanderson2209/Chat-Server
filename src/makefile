OBJS = ClientInformation.cpp main.cpp Server.cpp
CC = g++
CFLAGS = -pthread -std=c++11
FILENAME = Server

all: $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(FILENAME)

MemTest: Server
	valgrind --tool=memcheck --leak-check=yes --leak-check=full ./Server
?:
	@echo "==============================================================="
	@echo "=help:                                                        ="
	@echo "=make all - (default) Makes Server                            ="
	@echo "=make MemTest - Opens Server and ValGrind for memory testing. ="
	@echo "==============================================================="