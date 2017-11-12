#ifndef NOTIFY_H
#define NOTIFY_H

/**
 * Defines the urgency levels of notify-send.
 */
typedef enum {LOW, NORMAL, CRITICAL} urgency_t;

/**
 * Sends a notification to user using libnotify's notify-send.
 * Takes the arguments summary and body that will be passed to
 * notify-send as summary and body respectively.
 */
void notify(const char *summary, const char *body,
    urgency_t urgency);

void notify_server_message(const char *server, const char *sender,
    const char *message);

void notify_channel_message(const char *channel, const char *sender,
    const char *message);

void notify_private_message(const char *sender, const char *message);

void notify_join(const char *client);

void notify_leave(const char *client);

void notify_poke(const char *poker, const char *message);


#endif

