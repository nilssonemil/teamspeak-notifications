PLUGIN = teamspeak-notifications.so

all: 
	gcc -c -O2 -Wall -fPIC -DLINUX -I include src/plugin.c -o build/plugin.o
	gcc -o bin/$(PLUGIN) -shared build/plugin.o

clean:
	$(RM) $(PLUGIN)
	$(RM) build/*
