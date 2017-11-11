#ifndef NOTIFY_H
#define NOTIFY_H

/**
 * Sends a notification to user using libnotify's notify-send.
 * Takes the arguments summary and body that will be passed to
 * notify-send as summary and body respectively.
 */
void send_notification(const char* summary, const char* body,
    int urgency_level);

#endif

