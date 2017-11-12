/*
 * TeamSpeak 3 demo plugin
 *
 * Copyright (c) 2008-2017 TeamSpeak Systems GmbH
 */

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

#define _strcpy(dest, destSize, src) { strncpy(dest, src, destSize-1); (dest)[destSize-1] = '\0'; }

#define PLUGIN_API_VERSION 22

#define PATH_BUFSIZE 512
#define COMMAND_BUFSIZE 128
#define INFODATA_BUFSIZE 128
#define SERVERINFO_BUFSIZE 256
#define CHANNELINFO_BUFSIZE 512
#define RETURNCODE_BUFSIZE 128

static char* pluginID = NULL;

/*********************************** Required functions ************************************/
/*
 * If any of these required functions is not implemented, TS3 will refuse to load the plugin
 */

/* Unique name identifying this plugin */
const char* ts3plugin_name() {
	return "teamspeak-notifications";
}

/* Plugin version */
const char* ts3plugin_version() {
    return "0.2.0";
}

/* Plugin API version. Must be the same as the clients API major version, else the plugin fails to load. */
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

/*
 * Custom code called right after loading the plugin. Returns 0 on success, 1 on failure.
 * If the function returns 1 on failure, the plugin will be unloaded again.
 */
int ts3plugin_init() {
    return 0;
	/* 0 = success, 1 = failure, -2 = failure but client will not show a "failed to load" warning */
	/* -2 is a very special case and should only be used if a plugin displays a dialog (e.g. overlay) asking the user to disable
	 * the plugin again, avoiding the show another dialog by the client telling the user the plugin failed to load.
	 * For normal case, if a plugin really failed to load because of an error, the correct return value is 1. */
}

/* Custom code called right before the plugin is unloaded */
void ts3plugin_shutdown() {
	/*
	 * Note:
	 * If your plugin implements a settings dialog, it must be closed and deleted here, else the
	 * TeamSpeak client will most likely crash (DLL removed but dialog from DLL code still open).
	 */

	/* Free pluginID if we registered it */
	if(pluginID) {
		free(pluginID);
		pluginID = NULL;
	}
}

/**************************** Optional functions ******************************/
/*
 * Following functions are optional, if not needed you don't need to implement
 * them.
 */

/*
 * Plugin requests to be always automatically loaded by the TeamSpeak 3 client 
 * unless the user manually disabled it in the plugin dialog.
 * This function is optional. If missing, no autoload is assumed.
 */
int ts3plugin_requestAutoload() {
	/* 1 = request autoloaded, 0 = do not request autoload */
	return 1;
}

/************************** TeamSpeak callbacks ***************************/
/*
 * Following functions are optional, feel free to remove unused callbacks.
 * See the clientlib documentation for details on each function.
 */

/* Clientlib */

void ts3plugin_onConnectStatusChangeEvent(uint64 serverConnectionHandlerID,
		int newStatus, unsigned int errorNumber) {
	// TODO: Connection lost? Notify!
}

void ts3plugin_onUpdateChannelEvent(uint64 serverConnectionHandlerID,
		uint64 channelID) {
	// TODO: Notify channel edited, sync with below func
}

void ts3plugin_onUpdateChannelEditedEvent(uint64 serverConnectionHandlerID,
		uint64 channelID, anyID invokerID, const char* invokerName,
		const char* invokerUniqueIdentifier) {
	// TODO: Notify channel edited, sync with above func
}

void ts3plugin_onUpdateClientEvent(uint64 serverConnectionHandlerID,
		anyID clientID, anyID invokerID, const char* invokerName,
		const char* invokerUniqueIdentifier) {
	// TODO: Client updated? Notify!
}

void ts3plugin_onClientMoveEvent(uint64 serverConnectionHandlerID,
		anyID clientID, uint64 oldChannelID, uint64 newChannelID,
		int visibility, const char* moveMessage) {
	anyID myID;
	if ((ts3Functions.getClientID(serverConnectionHandlerID, &myID) == ERROR_ok)
			&& clientID == myID)
	{
		// ignore if the move is from yourself
		return;
	}

	uint64 currentID;
	if (ts3Functions.getChannelOfClient(serverConnectionHandlerID, myID,
				&currentID) != ERROR_ok)
		return;

	char *clientName;
	if (ts3Functions.getClientVariableAsString(serverConnectionHandlerID,
				clientID, CLIENT_NICKNAME, &clientName) != ERROR_ok)
		return;

	if (currentID == oldChannelID)
	{
		notify_leave(clientName);
	}
	else if (currentID == newChannelID)
	{
		notify_join(clientName);
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
	// TODO: Client was moved, notify!
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
	/* Friend/Foe manager has ignored the message, so ignore here as well. */
	if (ffIgnored) {
		// client will ignore message no matter what, rv does not matter
		return 0;
	}

	anyID myID;
    if ((ts3Functions.getClientID(serverConnectionHandlerID, &myID) == ERROR_ok)
			&& fromID == myID) {
		// do not send notifications on own messages
        return 0;
    }

	char *rv;
	if (targetMode == TextMessageTarget_SERVER &&
			ts3Functions.getServerVariableAsString(serverConnectionHandlerID,
				VIRTUALSERVER_NAME, &rv) == ERROR_ok)
	{
		notify_server_message(rv, fromName, message);
		ts3Functions.freeMemory(rv);
	}
	else if (targetMode == TextMessageTarget_CHANNEL)
	{
		uint64 channelID;
		if (ts3Functions.getChannelOfClient(serverConnectionHandlerID, myID,
					&channelID) == ERROR_ok)
		{
			unsigned int errcode;
			if ((errcode = ts3Functions.getChannelVariableAsString(
						serverConnectionHandlerID, channelID,
						CHANNEL_NAME, &rv)) == ERROR_ok)
			{
				notify_channel_message(rv, fromName, message);
				ts3Functions.freeMemory(rv);
			}
		}
	}
	else if (targetMode == TextMessageTarget_CLIENT)
	{
		notify_private_message(fromName, message);
	}

    return 0; /* 0 = handle normally, 1 = client will ignore the text message */
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

/* Clientlib rare */

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
    anyID myID;
	// Check if the Friend/Foe manager has already blocked this poke
	if(ffIgnored) {
		return 0;  /* Client will block anyways, doesn't matter what we return */
	}


    return 0;  /* 0 = handle normally, 1 = client will ignore the poke */
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

/* Client UI callbacks */
/* ---- */

