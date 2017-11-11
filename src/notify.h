#ifndef NOTIFY_H
#define NOTIFY_H

/**
 * Sends a notification to user using libnotify's notify-send.
 * Takes the arguments summary and body that will be passed to
 * notify-send as summary and body respectively.
 */
void notify(const char *summary, const char *body,
    int urgency_level);

void notify_server_message(const char *server, const char *sender,
    const char *message);

void notify_channel_message(const char *channel, const char *sender,
    const char *message);

void notify_private_message(const char *sender, const char *message);


#endif

