CC=gcc

LIBS = -lxcb -lxcb-util -lxcb-icccm -lxcb-keysyms -lxkbcommon -lxcb-cursor 

BINDIR = /usr/bin

all: install

install: absent.c
	$(CC) -o $(BINDIR)/absent absent.c $(LIBS)

clean:
	rm $(BINDIR)/absent
