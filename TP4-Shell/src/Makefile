# Makefile para tests
CC = gcc
CFLAGS = -Wall -Wextra -g

all: test_ring test_shell

test_ring: test_ring.c
	$(CC) $(CFLAGS) -o test_ring test_ring.c

test_shell: test_shell.c
	$(CC) $(CFLAGS) -o test_shell test_shell.c

clean:
	rm -f test_ring test_shell

.PHONY: all clean