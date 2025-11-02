CC = gcc
CFLAGS = -march=native -Os -ffast-math -flto -fno-exceptions -funroll-loops -Wall -Wextra -I./include/
LIBS = -lxcb -lxcb-util -lxcb-icccm -lxcb-keysyms -lxkbcommon -lxcb-cursor -lxcb-randr -lconfig
SRC_FILES = ./src/*

CFG_FILES = ./config
BIN_DIR = /usr/local/bin
BINARY = absent
DESKTOP = $(BINARY).desktop
DESKTOP_DIR = /usr/share/xsessions
CFG = $(BINARY).cfg
CFG_DIR = /etc/absent

all: compile

compile: 
	$(CC) -o $(BINARY) $(SRC_FILES) $(CFLAGS) $(LIBS)

install: compile
	install -Dm755 $(BINARY) $(BIN_DIR)/$(BINARY)
	install -Dm644 $(CFG_FILES)/$(DESKTOP) $(DESKTOP_DIR)/$(DESKTOP)
	install -Dm644 $(CFG_FILES)/$(CFG) $(CFG_DIR)/$(CFG)

uninstall:
	rm -f $(BIN_DIR)/$(BINARY)
	rm -f $(DESKTOP_DIR)/$(DESKTOP)
	rm -f $(GLOBAL_CFG_DIR)/$(CFG)

clean:
	rm -f $(BINARY)

.PHONY: all compile install uninstall clean
