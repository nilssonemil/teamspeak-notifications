#include "notify.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

static const char* COMMAND_NOTIFY = "notify-send";
static const char* APP_NAME = "TeaamSpeak 3";

void notify(const char *summary, const char *body, int urgency_level)
{
	int extra = 15; // for use of spaces/formattign
	char notify[strlen(COMMAND_NOTIFY) + strlen(summary) + strlen(APP_NAME) + 
		strlen(body) + extra];

	snprintf(notify, sizeof notify, "%s \"%s\" -a \"%s\" \"%s\"",
			COMMAND_NOTIFY, summary, APP_NAME, body);
	system(notify);
}

void notify_server_message(const char *server, const char *sender,
		const char *message)
{
	const char *server_suffix = " MESSAGE";
	char summary[strlen(server) + strlen(server_suffix) + 1];
	strcpy(summary, server);
	strcat(summary, server_suffix);

	const char *separator = ": ";
	char body[strlen(sender) + strlen(separator) + strlen(message) + 1];
	strcpy(body, sender);
	strcat(body, separator);
	strcat(body, message);

	notify(summary, body, 0);
}

void notify_channel_message(const char *channel, const char *sender,
		const char *message)
{
	const char *channel_suffix = " MESSAGE";
	char summary[strlen(channel) + strlen(channel_suffix) + 1];
	strcpy(summary, channel);
	strcat(summary, channel_suffix);

	const char *separator = ": ";
	char body[strlen(sender) + strlen(separator) + strlen(message) + 1];
	strcpy(body, sender);
	strcat(body, separator);
	strcat(body, message);

	notify(summary, body, 0);
}

void notify_private_message(const char *sender, const char *message)
{
	const char *summary = "PRIVATE MESSAGE";
	const char *separator = ": ";
	char body[strlen(sender) + strlen(separator) + strlen(message) + 1];
	strcpy(body, sender);
	strcat(body, separator);
	strcat(body, message);

	notify(summary, body, 1);
}

