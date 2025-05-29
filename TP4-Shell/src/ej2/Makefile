# CC = gcc
# CFLAGS = -Wall -Wextra -std=c11

# TARGET = shell
# SRC = shell.c

# all: $(TARGET)

# $(TARGET): $(SRC)
# 	$(CC) $(CFLAGS) -o $@ $^

# clean:
# 	rm -f $(TARGET)

# Makefile para TP4 Shell

CC = gcc
CFLAGS = -Wall -Wextra -g
TARGET = shell
TESTS = test_simple test_pipes test_quotes

all: $(TARGET) $(TESTS)

$(TARGET): shell.c
	$(CC) $(CFLAGS) -o $@ $^

test_simple: test_simple.c
	$(CC) $(CFLAGS) -o $@ $^

test_pipes: test_pipes.c
	$(CC) $(CFLAGS) -o $@ $^

test_quotes: test_quotes.c
	$(CC) $(CFLAGS) -o $@ $^

test: all
	@echo "\n=== Running simple command test ==="
	./test_simple
	@echo "\n=== Running pipe command test ==="
	./test_pipes
	@echo "\n=== Running quoted arguments test (extra credit) ==="
	./test_quotes

clean:
	rm -f $(TARGET) $(TESTS) *.o

.PHONY: all test clean

# make file cambiado faltan los tests de deepseek