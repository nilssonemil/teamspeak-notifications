#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "teamspeak/public_errors.h"
#include "teamspeak/public_errors_rare.h"
#include "teamspeak/public_definitions.h"
#include "teamspeak/public_rare_definitions.h"
#include "teamspeak/clientlib_publicdefinitions.h"
#include "ts3_functions.h"
#include "plugin.h"
#include "notify.h"

static struct TS3Functions ts3Functions;

#define _strcpy(dest, destSize, src) { strncpy(dest, src, destSize-1); \
  (dest)[destSize-1] = '\0'; }

#define PLUGIN_API_VERSION 22

#define PATH_BUFSIZE 512
#define COMMAND_BUFSIZE 128
#define INFODATA_BUFSIZE 128
#define SERVERINFO_BUFSIZE 256
#define CHANNELINFO_BUFSIZE 512
#define RETURNCODE_BUFSIZE 128

static char* pluginID = NULL;

/****** Local Function Prototypes ******/

/**
 * Return 1 if the client given is the same as the user client, otherwise 0.
 * Will return -1 if an error occured.
 */
int current_client(uint64 serverConnectionHandlerID, anyID other,
    anyID *client);

/**
 * Return 1 if the channel is the user's current channel, otherwise 0.
 * Will return -1 if an error occured.
 */
int current_channel(uint64 serverConnectionHandlerID, uint64 other,
    uint64 *current);

/**
 * Send notification of server message.
 */
void send_server_message_notification(uint64 serverConnectionHandlerID,
    const char *fromName, const char *message);

/**
 * Send notification of channel message.
 */
void send_channel_message_notification(uint64 serverConnectionHandlerID,
    anyID clientID, const char *fromName, const char *message);


/****** Required Functions ******/

/* Unique name identifying this plugin */
const char* ts3plugin_name() {
  return "TeamSpeak Notifications";
}

/* Plugin version */
const char* ts3plugin_version() {
  return "0.4.0";
}

/* Plugin API version. Must be the same as the clients API major version, else
 * the plugin fails to load. */
int ts3plugin_apiVersion() {
  return PLUGIN_API_VERSION;
}

/* Plugin author */
const char* ts3plugin_author() {
  return "Emil Nilsson <emil.nilsson@protonmail.com>";
}

/* Plugin description */
const char* ts3plugin_description() {
  return "This plugin sends notifications on TeamSpeak events via libnotify.";
}

/* Set TeamSpeak 3 callback functions */
void ts3plugin_setFunctionPointers(const struct TS3Functions funcs) {
  ts3Functions = funcs;
}

/**
 * Custom code called right after loading the plugin. Returns 0 on success, 1 on
 * failure. If the function returns 1 on failure, the plugin will be unloaded
 * again.
 */
int ts3plugin_init() {
  notify(ts3plugin_name(), "Hello, World!", 0);
  /**
   * Success -> 0
   * Failure -> 1
   * Failure -> -2 (wihotut failed to load warning)
   */
  return 0;
}

/* Custom code called right before the plugin is unloaded */
void ts3plugin_shutdown() {
  /**
   * NOTE:
   * If your plugin implements a settings dialog, it must be closed and deleted
   * here, else the TeamSpeak client will most likely crash (DLL removed but
   * dialog from DLL code still open).
   */

  /* Free pluginID if we registered it */
  if(pluginID) {
    free(pluginID);
    pluginID = NULL;
  }
}

/****** Optional Functions ******/

/*
 * Plugin requests to be always automatically loaded by the TeamSpeak 3 client 
 * unless the user manually disabled it in the plugin dialog.
 * This function is optional. If missing, no autoload is assumed.
 */
int ts3plugin_requestAutoload() {
  /* 1 = request autoloaded, 0 = do not request autoload */
  return 1;
}

/****** TeamSpeak Callbacks ******/

void ts3plugin_onConnectStatusChangeEvent(uint64 serverConnectionHandlerID,
    int newStatus, unsigned int errorNumber) {
  // TODO: Connection lost? Notify!
}

void ts3plugin_onUpdateChannelEditedEvent(uint64 serverConnectionHandlerID,
    uint64 channelID, anyID invokerID, const char* invokerName,
    const char* invokerUniqueIdentifier) {
  ts3Functions.logMessage("onUpdateChannelEditedEvent", LogLevel_DEBUG,
      ts3plugin_name(), serverConnectionHandlerID);

  /**
   * Do not show notification if the client edited the channel itself or
   * if the channel edited is not the one the client resides in.
   */
  anyID myID;
  uint64 currentID;
  if (current_client(serverConnectionHandlerID, invokerID, &myID) != 1 &&
      (current_channel(serverConnectionHandlerID, channelID, &currentID) == 1))
    notify_channel_edited(invokerName);
}

void ts3plugin_onUpdateClientEvent(uint64 serverConnectionHandlerID,
    anyID clientID, anyID invokerID, const char* invokerName,
    const char* invokerUniqueIdentifier) {
  // TODO: Client updated? Notify!
}

void ts3plugin_onClientMoveEvent(uint64 serverConnectionHandlerID,
    anyID clientID, uint64 oldChannelID, uint64 newChannelID,
    int visibility, const char* moveMessage) {
  ts3Functions.logMessage("onClientMoveEvent", LogLevel_DEBUG,
      ts3plugin_name(), serverConnectionHandlerID);

  /**
   * Send notifications that client left/joined channel if the channel is the
   * client's current one and the client is not the moving client.
   */
  anyID myID;
  if (current_client(serverConnectionHandlerID, clientID, &myID) != 1)
  {
    char *clientname;
    if (ts3Functions.getClientVariableAsString(serverConnectionHandlerID,
          clientID, CLIENT_NICKNAME, &clientname) == ERROR_ok)
    {
      uint64 currentID;
      if (current_channel(serverConnectionHandlerID, oldChannelID,
            &currentID) == 1)
        notify_leave(clientname, NULL);
      if (current_channel(serverConnectionHandlerID, newChannelID,
            &currentID) == 1)
        notify_join(clientname, NULL);
    }
    else
      ts3Functions.logMessage("Could not get client nickname.",
          LogLevel_ERROR, ts3plugin_name(),
          serverConnectionHandlerID);
  }
}

void ts3plugin_onClientMoveTimeoutEvent(uint64 serverConnectionHandlerID,
    anyID clientID, uint64 oldChannelID, uint64 newChannelID,
    int visibility, const char* timeoutMessage) {
  // TODO: Client connection timeout, Notify!
}

void ts3plugin_onClientMoveMovedEvent(uint64 serverConnectionHandlerID,
    anyID clientID, uint64 oldChannelID, uint64 newChannelID,
    int visibility, anyID moverID, const char* moverName,
    const char* moverUniqueIdentifier, const char* moveMessage) {
  ts3Functions.logMessage("onClientMoveMovedEvent", LogLevel_DEBUG,
      ts3plugin_name(), serverConnectionHandlerID);

  /**
   * If the client itself was moved, show message to indicate that.
   */
  anyID myID;
  int res;
  if ((res = current_client(serverConnectionHandlerID, moverID, &myID)) == 1
      && clientID != myID)
  {
    char *channelname;
    if (ts3Functions.getChannelVariableAsString(serverConnectionHandlerID,
          newChannelID, CHANNEL_NAME, &channelname) == ERROR_ok)
      notify_move(channelname, moverName);
    else
      ts3Functions.logMessage("Could not get channel name.",
          LogLevel_ERROR, ts3plugin_name(), serverConnectionHandlerID);
  }
  else if (res == 0) // client is not current client
  {
    /**
     * Send message that another client was moved to/from your channel.
     */
    char *clientname;
    if (ts3Functions.getClientVariableAsString(serverConnectionHandlerID,
          clientID, CLIENT_NICKNAME, &clientname) == ERROR_ok)
    {
      uint64 currentID;
      if (current_channel(serverConnectionHandlerID, oldChannelID,
            &currentID) == 1)
        notify_leave(clientname, moverName);
      if (current_channel(serverConnectionHandlerID, newChannelID,
            &currentID) == 1)
        notify_join(clientname, moverName);
    }
    else
      ts3Functions.logMessage("Could not get client nickname.",
          LogLevel_ERROR, ts3plugin_name(),
          serverConnectionHandlerID);
  }
}

void ts3plugin_onClientKickFromChannelEvent(uint64 serverConnectionHandlerID,
    anyID clientID, uint64 oldChannelID, uint64 newChannelID, int visibility,
    anyID kickerID, const char* kickerName,
    const char* kickerUniqueIdentifier, const char* kickMessage) {
  // TODO: Client kicked from channel, notify!
}

void ts3plugin_onClientKickFromServerEvent(uint64 serverConnectionHandlerID,
    anyID clientID, uint64 oldChannelID, uint64 newChannelID,
    int visibility, anyID kickerID, const char* kickerName,
    const char* kickerUniqueIdentifier, const char* kickMessage) {
  // TODO: Client kicked from server, notify!
}

void ts3plugin_onServerStopEvent(uint64 serverConnectionHandlerID,
    const char* shutdownMessage) {
  // TODO: Server shutdown, notify!
}

int ts3plugin_onTextMessageEvent(uint64 serverConnectionHandlerID,
    anyID targetMode, anyID toID, anyID fromID, const char* fromName,
    const char* fromUniqueIdentifier, const char* message, int ffIgnored) {
  ts3Functions.logMessage("onTextMessageEvent", LogLevel_DEBUG,
      ts3plugin_name(), serverConnectionHandlerID);

  /* Friend/Foe manager has ignored the message, so ignore here as well. */
  if (ffIgnored) return 0;

  /**
   * Send notification of server/channel/client message.
   * Do NOT send message if the sender is the client itself.
   */
  anyID myID;
  if (!current_client(serverConnectionHandlerID, fromID, &myID))
  {
    switch(targetMode)
    {
      case TextMessageTarget_SERVER:
        send_server_message_notification(serverConnectionHandlerID, fromName,
            message);
        return 0;

      case TextMessageTarget_CHANNEL:
        send_channel_message_notification(serverConnectionHandlerID, myID,
            fromName, message);
        return 0;
      
      case TextMessageTarget_CLIENT:;
        notify_private_message(fromName, message);
        return 0;
      
      default:
        ts3Functions.logMessage("Unexpected TextMessageTarget.",
            LogLevel_WARNING, ts3plugin_name(),
            serverConnectionHandlerID);
        return 0;
    }
  }

  return 0;
}

void ts3plugin_onTalkStatusChangeEvent(uint64 serverConnectionHandlerID,
    int status, int isReceivedWhisper, anyID clientID) {
  // NOTE: This could potentially be used to make an overlay for who's
  // currently talking, or display a lamp in status bar e.g.
}


void ts3plugin_onChannelDescriptionUpdateEvent(uint64 serverConnectionHandlerID,
    uint64 channelID) {
  // TODO: Channel description updated, notify
}

void ts3plugin_onChannelPasswordChangedEvent(uint64 serverConnectionHandlerID,
    uint64 channelID) {
  // TODO: Password changed, notify!
}

void ts3plugin_onClientBanFromServerEvent(uint64 serverConnectionHandlerID,
    anyID clientID, uint64 oldChannelID, uint64 newChannelID,
    int visibility, anyID kickerID, const char* kickerName,
    const char* kickerUniqueIdentifier, uint64 time,
    const char* kickMessage) {
  // TODO: Client banned from server, notify!
}

int ts3plugin_onClientPokeEvent(uint64 serverConnectionHandlerID,
    anyID fromClientID, const char* pokerName,
    const char* pokerUniqueIdentity, const char* message, int ffIgnored) {
  ts3Functions.logMessage("onClientPokeEvent", LogLevel_DEBUG,
      ts3plugin_name(), serverConnectionHandlerID);

  // Check if the Friend/Foe manager has already blocked this poke
  if(ffIgnored) return 0;

  /**
   * Only poke if the poker is not the client itself.
   */
  anyID myID;
  if (current_client(serverConnectionHandlerID, fromClientID, &myID) != 1)
    notify_poke(pokerName, message);

  return 0;
}

void ts3plugin_onFileListEvent(uint64 serverConnectionHandlerID,
    uint64 channelID, const char* path, const char* name, uint64 size,
    uint64 datetime, int type, uint64 incompletesize,
    const char* returnCode) {
  // TODO: find out what this does ...
}

void ts3plugin_onClientChannelGroupChangedEvent(
    uint64 serverConnectionHandlerID, uint64 channelGroupID,
    uint64 channelID, anyID clientID, anyID invokerClientID,
    const char* invokerName, const char* invokerUniqueIdentity) {
  // TODO: Channel group changed? Notify!
}

void ts3plugin_onServerGroupClientAddedEvent(uint64 serverConnectionHandlerID, 
    anyID clientID, const char* clientName,
    const char* clientUniqueIdentity, uint64 serverGroupID,
    anyID invokerClientID, const char* invokerName,
    const char* invokerUniqueIdentity) {
  // TODO: Added to server group? Notify!
}

void ts3plugin_onServerGroupClientDeletedEvent(uint64 serverConnectionHandlerID,
    anyID clientID, const char* clientName,
    const char* clientUniqueIdentity, uint64 serverGroupID,
    anyID invokerClientID, const char* invokerName,
    const char* invokerUniqueIdentity) {
  // TODO: Removed from server group? Notify!
}

void ts3plugin_onClientChatClosedEvent(uint64 serverConnectionHandlerID,
    anyID clientID, const char* clientUniqueIdentity) {
  // TODO: Client closed chat, notify?
}

void ts3plugin_onClientChatComposingEvent(uint64 serverConnectionHandlerID,
    anyID clientID, const char* clientUniqueIdentity) {
  // NOTE: What does this mean? Client writing message? Could show
  // symbol in status bar?
}

/****** Local Functions ******/

int current_client(uint64 serverConnectionHandlerID, anyID other,
    anyID *client)
{
  if (ts3Functions.getClientID(serverConnectionHandlerID, client) == ERROR_ok)
    return other == *client;

  return -1;
}

int current_channel(uint64 serverConnectionHandlerID, uint64 other,
    uint64 *current)
{
  anyID myID;
  if (ts3Functions.getClientID(serverConnectionHandlerID, &myID) != ERROR_ok)
  {
    ts3Functions.logMessage("Could not get client ID.", LogLevel_ERROR,
        ts3plugin_name(), serverConnectionHandlerID);
    return  -1;
  }
  if (ts3Functions.getChannelOfClient(serverConnectionHandlerID, myID,
        current) != ERROR_ok)
  {
    ts3Functions.logMessage("Could not get client's channel's ID.",
        LogLevel_DEBUG, ts3plugin_name(), serverConnectionHandlerID);
    return -1;
  }
  return other == *current;
}

void send_server_message_notification(uint64 serverConnectionHandlerID,
    const char *fromName, const char *message)
{
  char *servername;
  if (ts3Functions.getServerVariableAsString(serverConnectionHandlerID,
        VIRTUALSERVER_NAME, &servername) == ERROR_ok)
  {
    notify_server_message(servername, fromName, message);
    ts3Functions.freeMemory(servername);
  }
  else
    ts3Functions.logMessage("Could not get server name.", LogLevel_ERROR,
        ts3plugin_name(), serverConnectionHandlerID);
}

void send_channel_message_notification(uint64 serverConnectionHandlerID,
    anyID clientID, const char *fromName, const char *message)
{
  uint64 channelID;
  if (ts3Functions.getChannelOfClient(serverConnectionHandlerID, clientID,
        &channelID) == ERROR_ok)
  {
    char *channelname;
    if (ts3Functions.getChannelVariableAsString(
          serverConnectionHandlerID, channelID, CHANNEL_NAME, &channelname)
        == ERROR_ok)
    {
      notify_channel_message(channelname, fromName, message);
      ts3Functions.freeMemory(channelname);
    }
    else
      ts3Functions.logMessage("Could not get channel name.", LogLevel_ERROR,
          ts3plugin_name(), serverConnectionHandlerID);
    }
  else
    ts3Functions.logMessage("Could not get current channel's ID.",
        LogLevel_ERROR, ts3plugin_name(), serverConnectionHandlerID);
}

