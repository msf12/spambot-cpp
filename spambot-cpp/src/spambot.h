#pragma once
#include "StringQueue.h"

struct ThreadQueue
{
	//TODO: Create message struct and modify queue to use it
	// messages should contain BOTH the text of the message
	// and an enum for which thread they came from (User,IRC,etc.)
	StringQueue *queue;
	void *mutex;
};

void BotMain();
void MessageHandler(void *args);
void RestAPIHandler(void *args);
void UserInputHandler(void *args);
void BotInit();