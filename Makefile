CC = gcc
CFLAGS = -march=native -Os -ffast-math -flto -fno-exceptions -funroll-loops -Wall -Wextra
LIBS = -lxcb -lxcb-util -lxcb-icccm -lxcb-keysyms -lxkbcommon -lxcb-cursor -lxcb-randr 
SRC_FILES = ./src/*.c

BIN_DIR = /usr/bin
EXEC_FILE = absent
AUTOSTART_FILE = autostartabsent
SESSION_FILE = $(EXEC_FILE).desktop
SESSION_DIR = /usr/share/xsessions

all: copy install

copy:
	mkdir $(SESSION_DIR)
	cp $(SESSION_FILE) $(SESSION_DIR)
	cp $(AUTOSTART_FILE) $(BIN_DIR)

install: 
	$(CC) -o $(BIN_DIR)/$(EXEC_FILE) $(SRC_FILES) $(CFLAGS) $(LIBS)

clean:
	rm -f $(BIN_DIR)/$(EXEC_FILE)
	rm -f $(SESSION_DIR)/$(SESSION_FILE)
	rm -f $(BIN_DIR)/$(AUTOSTART_FILE)

.PHONY: all copy install clean
