#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <xdo.h>
#include "notify.h"

static const char* COMMAND_NOTIFY = "notify-send";
static const char* APP_NAME = "TeamSpeak 3";

/**
 * Return 1 if the TeamSpeak window is focused, otherwise 0.
 */
int focused()
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
  if (focused() == 1) return;

  int extra = 15; // for use of spaces/formattign
  char notify[strlen(COMMAND_NOTIFY) + strlen(summary) + strlen(APP_NAME) + 
    strlen(body) + extra];

  snprintf(notify, sizeof notify, "%s \"%s\" -a \"%s\" \"%s\"",
      COMMAND_NOTIFY, summary, APP_NAME, body);
  printf("DEBUG: %s\n", notify);
  system(notify);
}

void notify_server_message(const char *server, const char *sender,
    const char *message)
{
  const char *separator = ": ";
  char body[strlen(sender) + strlen(separator) + strlen(message) + 1];
  strcpy(body, sender);
  strcat(body, separator);
  strcat(body, message);

  notify(server, body, LOW);
}

void notify_channel_message(const char *channel, const char *sender,
    const char *message)
{
  const char *separator = ": ";
  char body[strlen(sender) + strlen(separator) + strlen(message) + 1];
  strcpy(body, sender);
  strcat(body, separator);
  strcat(body, message);

  notify(channel, body, LOW);
}

void notify_private_message(const char *sender, const char *message)
{
  const char *separator = ": ";
  char body[strlen(sender) + strlen(separator) + strlen(message) + 1];
  strcpy(body, sender);
  strcat(body, separator);
  strcat(body, message);
  notify("PM:", body, CRITICAL);
}

void notify_join(const char *client, const char *mover)
{
  const char *suffix = " joined the channel.";
  char summary[strlen(client) + strlen(suffix) + 1];
  strcpy(summary, client);
  strcat(summary, suffix);

  if (mover == NULL)
    notify(summary, "", NORMAL);
  else
  {
    const char *bprefix = "Was moved by ";
    const char *bsuffix = ".";
    char body[strlen(bprefix) + strlen(mover) + strlen(bsuffix) + 1];
    strcpy(body, bprefix);
    strcat(body, mover);
    strcat(body, bsuffix);

    notify(summary, body, NORMAL);
  }
}

void notify_leave(const char *client, const char *mover)
{
  const char *suffix = " left the channel.";
  char summary[strlen(client) + strlen(suffix) + 1];
  strcpy(summary, client);
  strcat(summary, suffix);

  if (mover == NULL)
    notify(summary, "", NORMAL);
  else
  {
    const char *bprefix = "Was moved by ";
    const char *bsuffix = ".";
    char body[strlen(bprefix) + strlen(mover) + strlen(bsuffix) + 1];
    strcpy(body, bprefix);
    strcat(body, mover);
    strcat(body, bsuffix);

    notify(summary, body, NORMAL);
  }
}

void notify_move(const char *channel, const char *mover)
{
  const char *sprefix = "You left ";
  const char *ssuffix = ".";
  char summary[strlen(sprefix) + strlen(channel) + strlen(ssuffix) + 1];
  strcpy(summary, sprefix);
  strcat(summary, channel);
  strcat(summary, ssuffix);

  const char *bprefix = "You were moved by ";
  const char *bsuffix = "!";
  char body[strlen(bprefix) + strlen(mover) + strlen(bsuffix) + 1];
  strcpy(body, bprefix);
  strcat(body, mover);
  strcat(body, bsuffix);

  notify(summary, body, CRITICAL);
}

void notify_poke(const char *poker, const char *message)
{
  const char *suffix = " poked you!";
  char summary[strlen(poker) + strlen(suffix) + 1];
  strcpy(summary, poker);
  strcat(summary, suffix);
  notify(summary, message, CRITICAL);
}

void notify_channel_edited(const char *editer)
{
  const char *suffix = " edited your channel.";
  char summary[strlen(editer) + strlen(suffix) + 1];
  strcpy(summary, editer);
  strcat(summary, suffix);
  notify(summary, "", LOW);
}

