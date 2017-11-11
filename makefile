PLUGIN = teamspeak-notifications.so
CC = gcc
CFLAGS = -c -O2 -Wall -fPIC -DLINUX -I include
TS3PLUGINS = $(HOME)/.ts3client/plugins/

all: $(PLUGIN)
	cp bin/$(PLUGIN) $(TS3PLUGINS)

$(PLUGIN): plugin.o notify.o
	$(CC) -o bin/$(PLUGIN) -shared build/plugin.o build/notify.o


plugin.o: src/plugin.c src/plugin.h src/notify.h
	$(CC) $(CFLAGS) src/plugin.c -o build/plugin.o

notify.o: src/notify.c src/notify.h
	$(CC) $(CFLAGS) src/notify.c -o build/notify.o


clean:
	$(RM) bin/$(PLUGIN)
	$(RM) build/*

