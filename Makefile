CC = g++
CFLAGS = -std=c++11

all: client server

server: server.cpp
	$(CC) $(CFLAGS) server.cpp -o server

client: client.cpp
	$(CC) $(CFLAGS) client.cpp -o client

clean:
	rm -f client server *.o *~