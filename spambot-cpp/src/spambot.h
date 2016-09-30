#pragma once
#include "StringQueue.h"

struct ThreadQueue
{
	StringQueue *queue;
	void *mutex;
};

void BotMain();
void MessageHandler(void *args);
void FollowerHandler(void *args);
void UserInputHandler(void *args);
void BotInit();