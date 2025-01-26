CC=gcc

LIBS = -lxcb -lxcb-util -lxcb-icccm -lxcb-keysyms -lxkbcommon -lxcb-cursor 

BINDIR = /usr/bin
START_FILE = autostartabsent
SESSION_FILE = absent.desktop
DESTINATION_DIR = /usr/share/xsessions

all: copy install

copy:
	cp $(SESSION_FILE) $(DESTINATION_DIR)
	cp $(START_FILE) $(BINDIR)

install: absent.c
	$(CC) -o $(BINDIR)/absent absent.c $(LIBS)

clean:
	rm -f $(BINDIR)/absent
	rm -f $(DESTINATION_DIR)/$(SESSION_FILE)
	rm -f $(BINDIR)/$(START_FILE)
