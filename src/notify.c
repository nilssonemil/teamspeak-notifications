#include "notify.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <xdo.h>

static const char* COMMAND_NOTIFY = "notify-send";
static const char* APP_NAME = "TeaamSpeak 3";

int teamspeak_focused()
{
	/**
	 * See man getpid.
	 */
	pid_t ts_pid = getpid();

	/*
	 * Create a new xdo_t instance for the display specified by the environment
	 * variable DISPLAY.
	 * http://www.semicomplete.com/files/xdotool/docs/html/xdo_8h.html#af5792fe8af4caafe90c44883d664eb1d
	 */
	const xdo_t *xdo = xdo_new(NULL);

	/**
	 * Get the window currently in focus, and fetch it's pid.
	 * http://www.semicomplete.com/files/xdotool/docs/html/xdo_8h.html#a531f14b0231f53eabd31e56801935512
	 */
	Window window;
	xdo_get_focused_window(xdo, &window);
	pid_t focus = (pid_t)xdo_get_pid_window(xdo, window);
	
	return focus == ts_pid;
}

void notify(const char *summary, const char *body, urgency_t urgency)
{
	// do not display notifcation if teamspeak is focused
	if (teamspeak_focused() == 1)
		return;

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

	notify(summary, body, LOW);
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

	notify(summary, body, LOW);
}

void notify_private_message(const char *sender, const char *message)
{
	const char *summary = "PRIVATE MESSAGE";
	const char *separator = ": ";
	char body[strlen(sender) + strlen(separator) + strlen(message) + 1];
	strcpy(body, sender);
	strcat(body, separator);
	strcat(body, message);

	notify(summary, body, NORMAL);
}

