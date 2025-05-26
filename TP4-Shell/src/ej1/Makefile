# CC = gcc
# CFLAGS = -Wall -Wextra -std=c11

# TARGET = ring
# SRC = ring.c

# all: $(TARGET)

# $(TARGET): $(SRC)
# 	$(CC) $(CFLAGS) -o $@ $^

# clean:
# 	rm -f $(TARGET)

CC = gcc
CFLAGS = -Wall -Wextra -std=c11

TARGET = ring
SRC = ring.c
TEST_TARGET = ring_tests
TEST_SRC = ring_tests.c

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $@ $^

$(TEST_TARGET): $(TEST_SRC) $(TARGET)
	$(CC) $(CFLAGS) -o $@ $<

test: $(TEST_TARGET)
	@echo "Ejecutando tests..."
	@./$(TEST_TARGET)

clean:
	rm -f $(TARGET) $(TEST_TARGET)

help:
	@echo "Targets disponibles:"
	@echo "  all     - Compilar el programa (default)"
	@echo "  test    - Compilar y ejecutar tests"
	@echo "  clean   - Limpiar archivos generados"
	@echo "  help    - Mostrar esta ayuda"

.PHONY: all test clean help