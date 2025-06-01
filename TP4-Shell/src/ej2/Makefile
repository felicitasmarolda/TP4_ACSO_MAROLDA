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

# Makefile para TP4 Shell
CC = gcc
CFLAGS = -Wall -Wextra -std=c11

TARGET = shell
SRC = shell.c

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -f $(TARGET)
	rm -rf test_files test_dir

# Tests automáticos
test: $(TARGET)
	# chmod +x test_shell.sh
	# ./test_shell.sh
	chmod +x tester.sh
	./tester.sh
	