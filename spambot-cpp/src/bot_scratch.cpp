#include "TCPSocket.h"

void BotMain()
{
	BotInit();

	ThreadQueue *Messages = new ThreadQueue;
	ThreadQueue *Responses = new ThreadQueue;
	Mutex MessageMutex;
	Mutex ResponseMutex;
	
	auto MessageHandlerThread = CreateThread(MessageHandler,(void *)Messages);
	auto FollowerHandlerThread = CreateThread(FollowerHandler,(void *)Messages);
	auto UserInputThread = CreateThread(UserInputHandler,(void *)Messages);
	TCPSocket Socket("irc.twitch.tv","6667");

	//Send initial connection messages and check responses for errors
	
	while(true)
	{
		if(GetMessageWithTimeout())
		{
			PostMessage(MessageHandlerThread,1);
			PostMessage(FollowerHandlerThread,1);
			PostMessage(UserInputThread,1);
			GetMessage();
			GetMessage();
			GetMessage();
			return;
		}

		for(auto received = Socket.getMessage();
				received != "";
				received = Socket.getMessage())
		{
			lock(MessageMutex);
			Messages->enqueue(received);
			release(MessageMutex);
		}

		if(/*enough time has passed to rate-limit responses*/) //TODO: check if bot is mod in the channel???
		{
			lock(ResponseMutex);
			Socket.send(Responses->dequeue);
			release(ResponseMutex);
		}
	}
}

void MessageHandler(void *args)
{
	ThreadQueue *Messages = (ThreadQueue *)args;
	ThreadQueue *Responses = Messages+1;
	Mutex MessageMutex = OpenMutex("MessageMutexName");
	Mutex ResponseMutex = OpenMutex("ResponseMutexName");

	while(true)
	{
		//TODO: check for BotMain shutdown message
		while(Messages->size > 0)
		{
			lock(MessageMutex);
			auto message = Messages->dequeue();
			release(MessageMutex);
			
			auto response = ChooseResponse(message);

			lock(ResponseMutex);
			Responses->enqueue(response);
			release(ResponseMutex);
		}
	}
}

void FollowerHandler(void *args)
{
	ThreadQueue *Messages = (ThreadQueue *)args;
	Mutex MessageMutex = OpenMutex("MessageMutexName");

	auto LastNewFollowerTime = GetMostRecentFollowTime();

	//TODO: figure out message timings vs check timings
	while(true)
	{
		//TODO: check for BotMain shutdown message
		auto NewFollowers = GetNewFollowers(LastNewFollowerTime);
		LastNewFollowerTime = NewFollowers[0].GetField("updated_at");
		for(auto follower : NewFollowers)
		{
			lock(MessageMutex);
			Messages->enqueue("New follower message");
		}
		Sleep(/*1 minute*/); //TODO: determine ideal thread sleep time between follower checks
	}
}

void UserInputHandler(void *args)
{
	ThreadQueue *Messages = (ThreadQueue *)args;
	Mutex MessageMutex = OpenMutex("MessageMutexName");

	while(true)
	{
		//TODO: check for BotMain shutdown message
		auto UserMessage = GetUserInput();

		lock(MessageMutex);
		Messages->enqueue(UserMessage);
		release(MessageMutex);
	}
}
