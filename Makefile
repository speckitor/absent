CC=gcc

LIBS = -lxcb -lxcb-util -lxcb-keysyms -lxkbcommon -lxcb-cursor 

BINDIR = /usr/bin
SOURCE_FILE = absent.desktop
DESTINATION_DIR = /usr/share/xsessions/

all: copy install

copy:
	cp $(SOURCE_FILE) $(DESTINATION_DIR)

install: absent.c
	$(CC) -o $(BINDIR)/absent absent.c $(LIBS)

clean:
	rm -f $(BINDIR)/absent

