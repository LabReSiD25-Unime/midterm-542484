CC = gcc
CFLAGS = -Wall -pthread

all: server client

server: server.c operazioni.c common.h
	$(CC) $(CFLAGS) server.c operazioni.c -o server

client: client.c
	$(CC) $(CFLAGS) client.c -o client

clean:
	rm -f server client *.o