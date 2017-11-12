# teamspeak-notifications
TeamSpeak 3 plugin for displaying notifications on Linux upon TeamSpeak events.

**Version:** 0.3.0

## Features
[x] Send notification alerts on messages received.
[x] Do not show notifications if TeamSpeak window is focused.
[x] Show which channel message is sent to.
[x] Show which server message is sent to.
[x] Send notifications on poke.
[x] Send notification alerts on users joining/leaving channel.
[x] Send notification on edited channel.
[x] Remove unnecessary code from plugin template.
[ ] Change styling of notifications depending on type.
[ ] Send information that can be used to setup display of active/muted
microphone.
[ ] Show TeamSpeak 3 icon with notification. 
[ ] Read config of which notification should be displayed from file.

## Requirements
In order for the plugin to work you will have to have following libaries
installed:

- libnotify
- libxdo

## Installation
Compile the plugin using `make install`.
This will place the resulting *teamspeak-notifications.so* inside the 
`$HOME/.ts3client/plugins` directory.
You may have to reload TeamSpeak in order for it to be loaded.

