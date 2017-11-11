#include "notify.h"
#include <stdlib.h>

static const char* COMMAND_NOTIFY = "notify-send";
static const char* APP_NAME = "TeaamSpeak 3";

void send_notification(const char* summary, const char* body, int urgency_level)
{
	int extra = 15; // for use of spaces/formattign
	char notify[strlen(command) + strlen(summary) + strlen(app) + 
		strlen(body) + extra]

	snprintf(notify, sizeof notify, "%s \"%s\" -a \"%s\" \"%s\"",
			COMMAND_NOTIFY, summary, APP_NAME, body);
	system(notify);
}
