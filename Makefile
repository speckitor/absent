CC = gcc
CFLAGS = -march=native -O3 -ffast-math -flto -fno-exceptions -funroll-loops 

LIBS = -lxcb -lxcb-util -lxcb-icccm -lxcb-keysyms -lxkbcommon -lxcb-cursor 

BIN_DIR = /usr/bin
START_FILE = autostartabsent
SESSION_FILE = absent.desktop
SESSION_DIR = /usr/share/xsessions

all: copy install

copy:
	cp $(SESSION_FILE) $(SESSION_DIR)
	cp $(START_FILE) $(BIN_DIR)

install: absent.c
	$(CC) -o $(BIN_DIR)/absent absent.c $(CFLAGS) $(LIBS)

clean:
	rm -f $(BIN_DIR)/absent
	rm -f $(SESSION_DIR)/$(SESSION_FILE)
	rm -f $(BIN_DIR)/$(START_FILE)
