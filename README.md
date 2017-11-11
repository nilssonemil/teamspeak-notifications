# teamspeak-notifications
### TeamSpeak 3 plugin for displaying notifications on events

## Features
[x] Send notification alerts on messages received.
[ ] Send notification alerts on users joining/leaving channel.
[ ] Send information that can be used to setup display of active/muted microphone.
[ ] Send notification on edited channel.
[ ] Show TeamSpeak 3 icon with notification. 
[ ] Show which channel message is sent to.
[ ] Show which server message is sent to.
[ ] Do not show notifications if TeamSpeak window is focused.

## Requirements
libnotify

## Installation
Compile the plugin using `make all`.
This will place the resulting *teamspeak-notifications.so* inside the 
`$HOME/.ts3client/plugins` directory.
You may have to reload TeamSpeak in order for it to be loaded.
