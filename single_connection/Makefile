all: server client

client : client.o
	g++ -o client client.o

server : server.o
	g++ -o server server.o


.PHONY: clean
clean:
	rm -f *.o server client
