CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -Iinclude
TARGET = pman
SRC = src/main.c src/utils.c src/languages.c src/tui.c src/config.c src/registry.c
OBJ = $(SRC:.c=.o)

.PHONY: all clean install test setup

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(OBJ) -o $(TARGET)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f src/*.o tests/*.o unit_tests $(TARGET)
	rm -rf test_run

install: all
	sudo cp $(TARGET) /usr/local/bin/

test: all
	@$(CC) $(CFLAGS) tests/unit_tests.c src/utils.c src/config.c src/registry.c src/languages.c -o unit_tests
	@./unit_tests
	@chmod +x tests/integration_tests.sh
	@./tests/integration_tests.sh

setup: all
	@chmod +x setup.sh
	@./setup.sh
