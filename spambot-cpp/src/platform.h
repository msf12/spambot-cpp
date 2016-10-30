#pragma once
#include "spambot.h"

#define Kilobytes(N) (1024*N)

enum HTTPMethod
{
	// TODO: Do we need more request types?
	GET,
	POST
};

void lock(void *mutex, unsigned long timeout);
void release(void *mutex);
void *InitMutex();
void *spawn_thread(void *fptr, void *args);

PLATFORM_THREAD_WRAPPER_NO_ARGS(BotMain)
PLATFORM_THREAD_WRAPPER(MessageHandler)
PLATFORM_THREAD_WRAPPER(RestAPIHandler)
PLATFORM_THREAD_WRAPPER(UserInputHandler)

struct platform_http_vars;

platform_http_vars InitTwitchHTTPConnection();
string MakeTwitchHTTPRequest(platform_http_vars TwitchAPIConnection, wstring Endpoint, wstring QueryParams, HTTPMethod Method);
void CloseTwitchHTTPConnection(platform_http_vars TwitchAPIConnection);

wstring GetWStringInput();
string GetStringInput();

void WriteToGUIIn(wchar_t *output);