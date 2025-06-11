CC = gcc
CFLAGS = -march=native -Os -ffast-math -flto -fno-exceptions -funroll-loops -Wall -Wextra
LIBS = -lxcb -lxcb-util -lxcb-icccm -lxcb-keysyms -lxkbcommon -lxcb-cursor -lxcb-randr 
SRC_FILES = ./src/*.c

BIN_DIR = /usr/local/bin
EXEC_FILE = absent
AUTOSTART_FILE = autostartabsent
SESSION_FILE = $(EXEC_FILE).desktop
SESSION_DIR = /usr/share/xsessions

all: compile

copy: clean
	mkdir -p $(SESSION_DIR)
	cp $(EXEC_FILE) $(BIN_DIR)
	cp $(AUTOSTART_FILE) $(BIN_DIR)
	cp $(SESSION_FILE) $(SESSION_DIR)

compile: 
	$(CC) -o $(EXEC_FILE) $(SRC_FILES) $(CFLAGS) $(LIBS)

clean:
	rm -f $(BIN_DIR)/$(EXEC_FILE)
	rm -f $(SESSION_DIR)/$(SESSION_FILE)
	rm -f $(BIN_DIR)/$(AUTOSTART_FILE)

.PHONY: all copy install clean
