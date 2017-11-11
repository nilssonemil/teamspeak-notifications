#ifndef NOTIFY_H
#define NOTIFY_H

static const char* COMMAND_NOTIFY = "notify-send";
static const char* APP_NAME = "TeaamSpeak 3";

/**
 * Sends a notification to user using libnotify's notify-send.
 * Takes the arguments summary and body that will be passed to
 * notify-send as summary and body respectively.
 */
void send_notification(const char* summary, const char* body,
    int urgency_level);
