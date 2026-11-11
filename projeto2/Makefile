CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -Wpedantic -O2

all: servidor cliente
servidor: servidor.c ptr.h
	$(CC) $(CFLAGS) -pthread servidor.c -o servidor
cliente: cliente.c ptr.h
	$(CC) $(CFLAGS) cliente.c -o cliente
test: all
	python3 teste.py
clean:
	rm -f servidor cliente
.PHONY: all test clean
