CC = g++
CFLAGS = -Wall -O
CLIBS = 

all: myserver.o
	$(CC) -o myserver myserver.o $(CLIBS)

myserver.o: myserver.cpp
	$(CC) $(CFLAGS) -c myserver.cpp

clean:
	rm -rf *.o
	rm -rf myserver
