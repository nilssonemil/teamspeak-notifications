# teamspeak-notifications
### TeamSpeak 3 plugin for displaying notifications on events

## Features
[x] Send notification alerts on messages received.
[ ] Send notification alters on users joining/leaving channel.
[ ] Send information that can be used to setup display of active/muted microphone.
[ ] Send notification on edited channel.

## Requirements
libnotify

## Installation
Compile the plugin using `make all` and place the resulting `teamspeak-notifications.so` from the directory `bin/` in your plugins directory (`$HOME/.ts3client/plugins`).
You may have to reload TeamSpeak in order for it to be loaded.
