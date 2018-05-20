PLUGIN = teamspeak-notifications.so
CC = gcc
CFLAGS = -c -O2 -Wall -fPIC -DLINUX -I include
TS3PLUGINS = $(HOME)/.ts3client/plugins/

default: all clean

all: $(PLUGIN)

install: $(PLUGIN)
	@cp bin/$(PLUGIN) $(TS3PLUGINS)

$(PLUGIN): plugin.o notify.o config.o inih.o
	@$(CC) -o bin/$(PLUGIN) -shared build/plugin.o build/notify.o build/config.o build/inih.o /usr/lib/libxdo.so


plugin.o: src/plugin.c src/plugin.h src/notify.h src/config.h
	@$(CC) $(CFLAGS) src/plugin.c -o build/plugin.o

notify.o: src/notify.c src/notify.h
	@$(CC) $(CFLAGS) src/notify.c -o build/notify.o

config.o: src/config.c src/config.h include/inih-r42/ini.h
	@$(CC) $(CFLAGS) src/config.c -o build/config.o

inih.o: include/inih-r42/ini.c include/inih-r42/ini.h
	@$(CC) $(CFLAGS) include/inih-r42/ini.c -o build/inih.o


clean:
	@$(RM) bin/$(PLUGIN)
	@$(RM) build/*

