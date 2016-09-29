#pragma once

void BotMain();
void MessageHandler(void *args);
void FollowerHandler(void *args);
void UserInputHandler(void *args);
void BotInit();

PLATFORM_THREAD_WRAPPER_NO_ARGS(BotMain)
PLATFORM_THREAD_WRAPPER(MessageHandler)
PLATFORM_THREAD_WRAPPER(FollowerHandler)
PLATFORM_THREAD_WRAPPER(UserInputHandler)

void lock(void *mutex, unsigned long mstimeout = INFINITE);
void release(void *mutex);
void *InitMutex();
void *spawn_thread(void *fptr, void *args);

void lock(void *mutex, unsigned long mstimeout = INFINITE)
{
	WaitForSingleObject(mutex, mstimeout);
}

void release(void *mutex)
{
	ReleaseMutex(mutex);
}

void *InitMutex()
{
	return CreateMutex(0, 0, nullptr);
}

void *spawn_thread(void *fptr, void *args)
{
	return CreateThread(0, 0, (DWORD(*)(void *)) fptr, args, 0, 0);
}