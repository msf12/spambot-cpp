#include "TCPSocket.h"
#include "yaml-cpp/yaml.h"
#include <fstream>
#include "spambot.h"
#include "spambot_util.cpp"
#include "spambot_message.cpp"

void BotMain()
{
	// auto testmsg = InterpretMessage("@badges=staff/1,bits/1;bits=69420;color=#FF23BA;display-name=;emotes=;"
	// 	"id=9428e2b3-615f-4728-9ed9-29995ebb2b52;mod=0;room-id=1337;sent-ts=1476320923316;subscriber=0;tmi-sent-ts=1476320936750;turbo=1;user-id=1337;user-type= "
	// 	":twitch_username!twitch_username@twitch_username.tmi.twitch.tv PRIVMSG #channel :cheer100");
	BotInit();

	ThreadQueue *Messages = (ThreadQueue *) malloc(sizeof(ThreadQueue)*2);
	ThreadQueue *Responses = Messages + 1;
	Messages->queue = new StringQueue;
	Responses->queue = new StringQueue;
	Messages->mutex = InitMutex();
	Responses->mutex = InitMutex();

	auto MessageHandlerThread  = spawn_thread((void *) PLATFORM_WRAPPER_NAME(MessageHandler),   (void *)Messages);
#if 0
	auto FollowerHandlerThread = spawn_thread((void *) PLATFORM_WRAPPER_NAME(FollowerHandler),  (void *)Messages);
#endif
	auto UserInputThread       = spawn_thread((void *) PLATFORM_WRAPPER_NAME(UserInputHandler), (void *)Messages);

	TCPSocket Socket("irc.twitch.tv", "6667");

	//Send initial connection messages
	// TODO: check responses for errors
	Socket.timeout(1000);

	WritelnToGUIOut("Sending password...");
	Socket.send("PASS " + PASS + "\r\n");
	Socket.receiveUntil("\r\n", 2);
	
	WritelnToGUIOut("Sending username...");
	Socket.send("NICK " + NICK + "\r\n");
	Socket.receiveUntil("\r\n", 2);

	WritelnToGUIOut("Requesting message tags");
	Socket.send("CAP REQ :twitch.tv/tags\r\n");
	Socket.receiveUntil("\r\n", 2);

	WritelnToGUIOut("Joining channel " + CHAN + "...");
	Socket.send("JOIN #" + CHAN + "\r\n");
	Socket.receiveUntil("\r\n",2);
	Socket.receiveUntil("\r\n",2);

	WritelnToGUIOut("Bot connected successfully!");

	Socket.timeout(1);
	if(SHOW_SALUTATIONS)
	{
		Socket.send(FormatTwitchPRIVMSG(STARTUP));
	}

	while(true)
	{
#if 0
		//TODO: implement thread message-queue wrappers
		if (GetMessageWithTimeout())
		{
			PostMessage(MessageHandlerThread, 1);
			PostMessage(FollowerHandlerThread, 1);
			PostMessage(UserInputThread, 1);
			GetMessage();
			GetMessage();
			GetMessage();
			free(Messages);
			//free(Responses);
			Socket.send(FormatTwitchPRIVMSG(SHUTDOWN));
			return;
		}
#endif

		for(auto received = Socket.receiveUntil("\r\n",2);
			received != "";
			received = Socket.receiveUntil("\r\n",2))
		{
			if(received.substr(0,4) == "PING")
			{
				Socket.send("PONG :tmi.twitch.tv\r\n");
			}
			else
			{
				lock(Messages->mutex);
				Messages->queue->enqueue(received);
				release(Messages->mutex);
			}
		}

		lock(Responses->mutex);
		if(Responses->queue->size)
		{
			//Socket.send(FormatTwitchPRIVMSG(Responses->queue->dequeue()));
			WritelnToGUIOut(Responses->queue->dequeue());
		}
		release(Responses->mutex);

#if 0
		if (/*enough time has passed to rate-limit responses*/) //TODO: check if bot is mod in the channel???
		{
			lock(Responses.mutex);
			Socket.send(Responses.queue->dequeue());
			release(Responses.mutex);
		}
#endif
	}
}

void MessageHandler(void *args)
{
	ThreadQueue *Messages = (ThreadQueue *)args;
	ThreadQueue *Responses = (ThreadQueue *)(Messages + 1);
	StringQueue *MessageQueue = Messages->queue;
	StringQueue *ResponseQueue = Responses->queue;

	while(true)
	{
		// TODO: check for BotMain shutdown message
		// TODO: use User input to activate commands
		while(MessageQueue->size > 0)
		{
			lock(Messages->mutex);
			auto raw_message = MessageQueue->dequeue();
			release(Messages->mutex);

			if (raw_message[0] == 'U')
			{
				lock(Responses->mutex);
				ResponseQueue->enqueue(raw_message.substr(1));
				release(Responses->mutex);
			}
			else if (raw_message[0] != '@')
			{
				// TODO: replace with WriteDebugOutput
				WritelnToGUIOut(("MessageHandler Error: expected @\nFound: " + raw_message).c_str());
			}
			else if(raw_message.find("\r\n") != (raw_message.length() - 2))
			{
				// TODO: replace with WriteDebugOutput
				WritelnToGUIOut(("MessageHandler Error: expected one message\nFound: " + raw_message).c_str());
				for(size_t index = 0, marker = 0; index < raw_message.length();)
				{
					// NOTE: marker is set to the index AFTER \r\n for easier substring math
					marker = raw_message.find("\r\n",index)+2;
					auto raw_submessage = raw_message.substr(index, marker - index);

					TwitchMessage message = {};
					message = InterpretMessage(raw_submessage);

					lock(Responses->mutex);
					ResponseQueue->enqueue(/*ChooseResponse*/(message.text));
					release(Responses->mutex);

					index = marker;
				}
			}
			else
			{
				TwitchMessage message = {};
				message = InterpretMessage(raw_message);

				auto response = message.display_name + ": " + message.text + "  |  (" +
					(message.mod ? "moderator:" : "-:") +
					(message.subscriber ? "subscriber:" : "-:") +
					(message.turbo ? "turbo" : "-") + ")";// = ChooseResponse(message);

				lock(Responses->mutex);
				ResponseQueue->enqueue(response);
				release(Responses->mutex);
			}
		}
	}
}

// TODO: WinHTTP follower notifications?
#if 0
void FollowerHandler(void *args)
{
	ThreadQueue *Messages = (ThreadQueue *)args;

	auto LastNewFollowerTime = GetMostRecentFollowTime();

	//TODO: figure out message timings vs check timings
	while(true)
	{
		//TODO: check for BotMain shutdown message
		auto NewFollowers = GetNewFollowers(LastNewFollowerTime);
		LastNewFollowerTime = NewFollowers[0].GetField("updated_at");
		for(auto follower : NewFollowers)
		{
			lock(Messages->mutex);
			Messages->queue->enqueue("New follower message");
			release(Messages->mutex)
		}
		//Sleep(/*1 minute*/); //TODO: determine ideal thread sleep time between follower checks
	}
}
#endif

void UserInputHandler(void *args)
{
	ThreadQueue *Messages = (ThreadQueue *)args;

	while(true)
	{
		//TODO: check for BotMain shutdown message
		auto UserMessage = GetStringInput();

		// TODO: This is a hack and should be evaluated to see if it really is
		// the best way to do things
		if(UserMessage.length())
		{
			lock(Messages->mutex);
			Messages->queue->enqueue("U" + UserMessage);
			release(Messages->mutex);
		}
	}
}

//TODO: Create WriteToDebugOut or some other print_debug wrapper?
void BotInit()
{
	WritelnToGUIOut("BOT STARTING!");

	struct _stat buf;
	YAML::Node config;
	bool configChanged = false;

	if(_stat("config.yaml", &buf) != 0)
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

	if(SHOW_OPTIONS)
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

			WritelnToGUIOut(initmenu);

			//check that the input is valid (either Y,y,N, or n)
			//this can definitely be simplified but it works the way it is
			while((input = GetStringInput()).length() > 1 ||
				(input[0] != 'y' && input[0] != 'Y' && input[0] != 'n' && input[0] != 'N'))
			{
				//TODO: fix print_debug
				//print_debug("ERROR: Invalid input.");
				WritelnToGUIOut("\r\nERROR: Invalid input.\r\n");
				WritelnToGUIOut(initmenu);
			}

			//if the input was Y or y show the field edit menu otherwise continue starting the bot
			if(input[0] == 'y' || input[0] == 'Y')
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

				WritelnToGUIOut(fields);

				//loop until the input is a valid character ('1','2', or '3')
				while((input = GetStringInput()).length() > 1 ||
					input[0] < '1' || input[0] > '5')
				{
					print_debug("ERROR: Invalid input.");
					WritelnToGUIOut(fields);
				}

				//TODO: validate field edits to ensure they are valid

				//switch based on the character the user input
				switch(input[0])
				{
					//1. Channel
				case '1':
					WritelnToGUIOut("What channel should be bot chat in?");
					//convert channel name to lower case and prepend # to fit the twitch irc message formatting (see: https://github.com/justintv/Twitch-API/blob/master/IRC.md)
					tempCHAN = GetStringInput();
					WritelnToGUIOut("Bot is now configured to join channel: " + tempCHAN);
					configChanged = true;
					goto editfields;

					//2. Username
				case '2':
					WritelnToGUIOut("What is the bot's new username?");
					//convert the username to lower case
					tempNICK = GetStringInput();
					//logging into the twitch irc requires a valid oauth token starting with "oauth:" (see link above)
					WritelnToGUIOut("What is the oath token for this username? (http://www.twitchapps.com/tmi/)");

					WriteToGUIIn(L"oauth:");

					tempPASS = GetStringInput();
					tempPASS = "oauth:" + tempPASS;
					WritelnToGUIOut("Bot is now configured to use the username: " + tempNICK);
					configChanged = true;
					goto editfields;

					//3. Owner
				case '3':
					WritelnToGUIOut("What is the username of the new administrator?");
					tempOWNER = GetStringInput();
					WritelnToGUIOut("Bot administrator is now: " + tempOWNER);
					configChanged = true;
					goto editfields;

					//4. Save changes and return
				case '4':
					WritelnToGUIOut("Saving configuration changes");
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
		} while(initLoop);

	}
	if(configChanged)
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

	WritelnToGUIOut(L"\r\nBot initialized.\r\n");
}
