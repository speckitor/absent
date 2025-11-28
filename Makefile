CC=gcc
CFLAGS=-march=native -O3 -ffast-math -flto -fno-exceptions -funroll-loops -Wall -Wextra -I./include -I./thirdparty
LDFLAGS=-lxcb -lxcb-util -lxcb-icccm -lxcb-keysyms -lxkbcommon -lxcb-cursor -lxcb-randr

SRC=src
SRC_FILES=src/absent.c src/clients.c src/config.c src/desktops.c src/events.c src/keycallbacks.c src/keys.c src/layout.c src/logs.c src/monitors.c
BUILD=build
OBJS=$(patsubst src/%.c, $(BUILD)/%.o, $(SRC_FILES))

CFG_FILES=./config
BIN_DIR=/usr/local/bin
BIN=absent
DESKTOP=$(BIN).desktop
DESKTOP_DIR=/usr/share/xsessions
CFG=$(BIN).cfg
CFG_DIR=/etc/absent

all: $(BIN)

$(BUILD):
	mkdir -p build

$(BUILD)/%.o: $(SRC)/%.c
	$(CC) -o $@ -c $< $(CFLAGS)

$(BIN): $(BUILD) $(OBJS)
	$(CC) -o $(BUILD)/$(BIN) $(OBJS) $(LDFLAGS)

install: $(BIN)
	install -Dm755 $(BUILD)/$(BIN) $(BIN_DIR)/$(BIN)
	install -Dm644 $(CFG_FILES)/$(DESKTOP) $(DESKTOP_DIR)/$(DESKTOP)
	install -Dm644 $(CFG_FILES)/$(CFG) $(CFG_DIR)/$(CFG)

uninstall:
	rm -f $(BIN_DIR)/$(BIN)
	rm -f $(DESKTOP_DIR)/$(DESKTOP)
	rm -f $(CFG_DIR)/$(CFG)

clean:
	rm -rf ./$(BUILD)

.PHONY: all compile install uninstall clean
