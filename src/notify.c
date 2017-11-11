#include "notify.h"
#include <stdlib.h>

void send_notification(const char* summary, const char* body, int urgency_level)
{
	// app constants for notify-send
	// TODO: move these to header file
	const char* command = "notify-send";
	const char* app = "Teamspeak 3";
	
	int extra = 15; // for use of spaces/formattign
	char notify[strlen(command) + strlen(summary) + strlen(app) + 
		strlen(body) + extra]

	snprintf(notify, sizeof notify, "%s \"%s\" -a \"%s\" \"%s\"", command,
			summary, app, body);
	system(notify);
}
