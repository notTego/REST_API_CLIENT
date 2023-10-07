CC=g++
CFLAGS=-I.

client: final_client.cpp requests.cpp helpers.cpp buffer.cpp
	$(CC) -o client final_client.cpp requests.cpp helpers.cpp buffer.cpp -Wall

run: client
	./client

clean:
	rm -f *.o client
