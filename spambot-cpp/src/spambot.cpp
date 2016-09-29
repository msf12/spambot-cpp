#include "stdafx.h"
#include "globals.h"
#include "spambot.h"
#include "TCPSocket.h"
#include "StringQueue.h"
#include "platform.h"
#include "yaml-cpp/yaml.h"
#include <fstream>

struct ThreadQueue
{
	StringQueue *queue;
	void *mutex;
};

void BotMain()
{
	BotInit();

	ThreadQueue Messages;
	ThreadQueue Responses;
	Messages.queue = new StringQueue;
	Responses.queue = new StringQueue;
	Messages.mutex = InitMutex();
	Responses.mutex = InitMutex();

	auto MessageHandlerThread  = spawn_thread((void *) PLATFORM_WRAPPER_NAME(MessageHandler),   (void *)&Messages);
	auto FollowerHandlerThread = spawn_thread((void *) PLATFORM_WRAPPER_NAME(FollowerHandler),  (void *)&Messages);
	auto UserInputThread       = spawn_thread((void *) PLATFORM_WRAPPER_NAME(UserInputHandler), (void *)&Messages);
	TCPSocket Socket("irc.twitch.tv", "6667");

	//Send initial connection messages and check responses for errors

	while(true)
	{
		//TODO: implement thread message-queue wrappers
#if 0
		if (GetMessageWithTimeout())
		{
			PostMessage(MessageHandlerThread, 1);
			PostMessage(FollowerHandlerThread, 1);
			PostMessage(UserInputThread, 1);
			GetMessage();
			GetMessage();
			GetMessage();
			return;
		}
#endif
		for (auto received = Socket.receive();
			received != "";
			received = Socket.receive())
		{
			lock(Messages.mutex);
			Messages.queue->enqueue(received);
			release(Messages.mutex);
		}

		if (/*enough time has passed to rate-limit responses*/) //TODO: check if bot is mod in the channel???
		{
			lock(Responses.mutex);
			Socket.send(Responses.queue->dequeue());
			release(Responses.mutex);
		}
	}
}

void MessageHandler(void *args)
{
	ThreadQueue *Messages = (ThreadQueue *)args;
	StringQueue *MessageQueue = Messages->queue;
	ThreadQueue *Responses = Messages + 1;
	StringQueue *ResponseQueue = Responses->queue;

	while (true)
	{
		//TODO: check for BotMain shutdown message
		while (MessageQueue->size > 0)
		{
			lock(Messages->mutex);
			auto message = MessageQueue->dequeue();
			release(Messages->mutex);

			auto response = "";// = ChooseResponse(message);

			lock(Responses->mutex);
			ResponseQueue->enqueue(response);
			release(Responses->mutex);
		}
	}
}

void FollowerHandler(void *args)
{
	ThreadQueue *Messages = (ThreadQueue *)args;

	auto LastNewFollowerTime = GetMostRecentFollowTime();

	//TODO: figure out message timings vs check timings
	while (true)
	{
		//TODO: check for BotMain shutdown message
		auto NewFollowers = GetNewFollowers(LastNewFollowerTime);
		LastNewFollowerTime = NewFollowers[0].GetField("updated_at");
		for (auto follower : NewFollowers)
		{
			lock(Messages->mutex);
			Messages->queue->enqueue("New follower message");
		}
		//Sleep(/*1 minute*/); //TODO: determine ideal thread sleep time between follower checks
	}
}

void UserInputHandler(void *args)
{
	ThreadQueue *Messages = (ThreadQueue *)args;

	while (true)
	{
		//TODO: check for BotMain shutdown message
		auto UserMessage = GetUserInput();

		lock(Messages->mutex);
		Messages->queue->enqueue(UserMessage);
		release(Messages->mutex);
	}
}

//TODO: Create WriteToDebugOut or some other print_debug wrapper?
void BotInit()
{
	WriteToGUIOut("BOT STARTING!");

	struct _stat buf;
	YAML::Node config;
	bool configChanged = false;

	if (_stat("config.yaml", &buf) != 0)
	{
		print_debug("config.yaml does not exist...generating with default configuration");

		config["OWNER"] = "default_owner";
		config["NICK"] = "default_account";
		config["PASS"] = "default_oauth";
		config["CHAN"] = "default_channel";
		config["SHOW_OPTIONS"] = true;
		config["SHOW_SALUTATIONS"] = false;
		config["STARTUP"] = "Bot starting";
		config["SHUTDOWN"] = "Bot shutting down";

		configChanged = true;
	}
	else
	{
		config = YAML::LoadFile("config.yaml");
	}

	SHOW_OPTIONS = config["SHOW_OPTIONS"].as<bool>();
	SHOW_SALUTATIONS = config["SHOW_SALUTATIONS"].as<bool>();

	if (SHOW_OPTIONS)
	{

		//string listing current settings and asking the user whether they should be edited
		string initmenu;
		string input;
		bool initLoop;

		//label to return to if editing is complete
		do
		{
			initLoop = false;
			initmenu = "This bot will be chatting as: " + config["NICK"].as<string>() + "\r\n"
				"This bot will be chatting in the channel: " + config["CHAN"].as<string>() + "\r\n"
				"The administrator for the bot is " + config["OWNER"].as<string>() + "\r\n"
				"Would you like to edit these settings? [Y/n]\r\n"
				"(To disable this message, change SHOW_OPTIONS to false in irc.conf)";

			WriteToGUIOut(initmenu);
			//TODO: migrate this to win32 platform layer

			//check that the input is valid (either Y,y,N, or n)
			//this can definitely be simplified but it works the way it is
			while ((input = GetStringInput()).length() > 1 ||
				(input[0] != 'y' && input[0] != 'Y' && input[0] != 'n' && input[0] != 'N'))
			{
				//TODO: fix print_debug
				//print_debug("ERROR: Invalid input.");
				WriteToGUIOut("\r\nERROR: Invalid input.\r\n");
				WriteToGUIOut(initmenu);
			}

			//if the input was Y or y show the field edit menu otherwise continue starting the bot
			if (input[0] == 'y' || input[0] == 'Y')
			{
				initLoop = true;
				//string listing the current values of editable fields
				string fields;

				//create temp variables for the fields to propogate edits without loosing the original values
				auto tempCHAN = config["CHAN"].as<string>(), tempNICK = config["NICK"].as<string>(), tempPASS = config["PASS"].as<string>(), tempOWNER = config["OWNER"].as<string>();

				//label to return to after editing a field
			editfields:
				fields = "Choose a field to edit:\r\n"
					"1. Channel:     " + tempCHAN + "\r\n"
					"2. Username:    " + tempNICK + "\r\n"
					"3. Owner:       " + tempOWNER + "\r\n"
					"4. Save changes and return\r\n"
					"5. Cancel changes and return";

				WriteToGUIOut(fields);

				//loop until the input is a valid character ('1','2', or '3')
				while ((input = GetStringInput()).length() > 1 ||
					input[0] < '1' || input[0] > '5')
				{
					print_debug("ERROR: Invalid input.");
					WriteToGUIOut(fields);
				}

				//TODO: validate field edits to ensure they are valid

				//switch based on the character the user input
				switch (input[0])
				{
					//1. Channel
				case '1':
					WriteToGUIOut("What channel should be bot chat in?");
					//convert channel name to lower case and prepend # to fit the twitch irc message formatting (see: https://github.com/justintv/Twitch-API/blob/master/IRC.md)
					tempCHAN = GetStringInput();
					WriteToGUIOut("Bot is now configured to join channel: " + tempCHAN);
					configChanged = true;
					goto editfields;

					//2. Username
				case '2':
					WriteToGUIOut("What is the bot's new username?");
					//convert the username to lower case
					tempNICK = GetStringInput();
					//logging into the twitch irc requires a valid oauth token starting with "oauth:" (see link above)
					WriteToGUIOut("What is the oath token for this username? (http://www.twitchapps.com/tmi/)");

					WriteToGUIIn(L"oauth:");

					tempPASS = GetStringInput();
					tempPASS = "oauth:" + tempPASS;
					WriteToGUIOut("Bot is now configured to use the username: " + tempNICK);
					configChanged = true;
					goto editfields;

					//3. Owner
				case '3':
					WriteToGUIOut("What is the username of the new administrator?");
					tempOWNER = GetStringInput();
					WriteToGUIOut("Bot administrator is now: " + tempOWNER);
					configChanged = true;
					goto editfields;

					//4. Save changes and return
				case '4':
					WriteToGUIOut("Saving configuration changes");
					config["OWNER"] = tempOWNER;
					config["NICK"] = tempNICK;
					config["PASS"] = tempPASS;
					config["CHAN"] = tempCHAN;
					config["SHOW_OPTIONS"] = SHOW_OPTIONS;
					break;

					//5. Cancel changes and return
				case '5':
					break;

					//seriously, if the code ever ends up here, I don't even know what could have gone wrong
				default:
					print_debug("ERROR: If you are seeing this something has gone very wrong", true);
					exit(1);
				}
			}
		} while (initLoop);

	}
	if (configChanged)
	{
		ofstream conffile("config.yaml");
		conffile << config;
		conffile.close();
	}

	OWNER = config["OWNER"].as<string>();
	NICK = config["NICK"].as<string>();
	PASS = config["PASS"].as<string>();
	CHAN = config["CHAN"].as<string>();
	STARTUP = config["STARTUP"].as<string>();
	SHUTDOWN = config["SHUTDOWN"].as<string>();

	WriteToGUIOut(L"\nBot initialized.");
}

//TODO: add args/overloads and move to platform layer
void WriteToGUIIn(wchar_t *output)
{
	//write "oauth:" to the TextInput edit control and move the caret after it
	SetWindowText(g_TextInput.handle, output);
	SendMessage(g_TextInput.handle, EM_SETSEL, 0, MAKELONG(0xffff, 0xffff));
	SendMessage(g_TextInput.handle, EM_SETSEL, -1, 0);
}
