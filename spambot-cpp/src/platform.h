#pragma once
#include "spambot.h"

void lock(void *mutex, unsigned long timeout);
void release(void *mutex);
void *InitMutex();
void *spawn_thread(void *fptr, void *args);

PLATFORM_THREAD_WRAPPER_NO_ARGS(BotMain)
PLATFORM_THREAD_WRAPPER(MessageHandler)
#if 0
PLATFORM_THREAD_WRAPPER(FollowerHandler)
#endif
PLATFORM_THREAD_WRAPPER(UserInputHandler)

wstring GetWStringInput();
string GetStringInput();

void WriteToGUIIn(wchar_t *output);