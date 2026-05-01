CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -Iinclude -O3 -flto -g
LDFLAGS = -flto
TARGET = pman
SRC = src/main.c src/utils.c src/languages.c src/tui.c src/config.c src/registry.c
OBJ = $(SRC:.c=.o)

PREFIX ?= /usr/local
BINDIR = $(DESTDIR)$(PREFIX)/bin

.PHONY: all clean install test setup uninstall release

all: $(TARGET)

release: CFLAGS += -march=native -fno-plt
release: LDFLAGS += -s
release: all

$(TARGET): $(OBJ)
	$(CC) $(OBJ) -o $(TARGET) $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f src/*.o tests/*.o unit_tests $(TARGET)
	rm -rf test_run

install: all
	mkdir -p $(BINDIR)
	cp $(TARGET) $(BINDIR)/
	mkdir -p $(HOME)/.config/pman/templates
	cp -r templates/* $(HOME)/.config/pman/templates/ 2>/dev/null || true

uninstall:
	rm -f $(BINDIR)/$(TARGET)
	rm -rf $(HOME)/.config/pman/templates
	@chmod +x uninstall.sh
	@./uninstall.sh

test: all
	@$(CC) $(CFLAGS) tests/unit_tests.c src/utils.c src/config.c src/registry.c src/languages.c -o unit_tests
	@./unit_tests
	@chmod +x tests/integration_tests.sh
	@./tests/integration_tests.sh

setup: all
	@chmod +x setup.sh
	@./setup.sh

