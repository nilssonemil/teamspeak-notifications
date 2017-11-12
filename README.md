# teamspeak-notifications
### TeamSpeak 3 plugin for displaying notifications on events

## Features
[x] Send notification alerts on messages received.
[x] Do not show notifications if TeamSpeak window is focused.
[x] Show which channel message is sent to.
[x] Show which server message is sent to.
[ ] Send notification alerts on users joining/leaving channel.
[ ] Send notification on edited channel.
[ ] Change styling of notifications depending on type.
[ ] Send information that can be used to setup display of active/muted microphone.
[ ] Show TeamSpeak 3 icon with notification. 
[ ] Remove unnecessary code from plugin template.

## Requirements
- libnotify
- libxdo

## Installation
Compile the plugin using `make all`.
This will place the resulting *teamspeak-notifications.so* inside the 
`$HOME/.ts3client/plugins` directory.
You may have to reload TeamSpeak in order for it to be loaded.
