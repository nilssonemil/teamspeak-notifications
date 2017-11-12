# TeamSpeak Notifications
TeamSpeak 3 plugin for displaying notifications on Linux upon TeamSpeak events.

**Version:** 0.4.0

## Features
See [Feature List](FEATURES.md)

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

