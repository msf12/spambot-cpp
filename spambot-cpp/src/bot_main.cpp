#include "stdafx.h"
#include "yaml-cpp/yaml.h"
#include "TCPSocket.h"
#include <cstdio>
#include <fstream>
#include <queue>

bool botInit();

DWORD WINAPI BotMain(LPVOID args)
{
	//botInit();
	
	TCPSocket Socket("irc.chat.twitch.tv","6667");

	Socket.send("PASS " + PASS + "\r\n");
	Socket.send("NICK " + NICK + "\r\n");
	Socket.send("CAP REQ :twitch.tv/tags\r\n");
	Socket.send("JOIN #" + CHAN + "\r\n");

	//ThreadQueue *MessagesReceived = new ThreadQueue();
	//ThreadQueue *Responses = new ThreadQueue();
	HANDLE MessageReceivedMutex = CreateMutex(0, false, L"MessageReceivedMutex");
	HANDLE ResponseMutex = CreateMutex(0, false, L"ResponseMutex");
	//CreateThread(0,0,MessageHandlerProc,0,messagesReceived);

	// Receive data until the server closes the connection
	for(auto received = Socket.receive(); received != ""; received = Socket.receive())
	{
		if(received == "PING")
		{
			Socket.send("PONG :tmi.twitch.tv\r\n");
		}
		else
		{
			WaitForSingleObject(MessageReceivedMutex,INFINITE);
			{
				//messagesReceived->enqueue(received);
			}
			writeln(received);
		}
	}

	//TODO: User input thread
	//TODO: Message receiver/sender thread
	//TODO: Message handler thread
	//TODO: Follower handler thread
	return 1;
}
#if 0
bool botInit()
{
	writeln("BOT STARTING!");

	struct _stat buf;
	YAML::Node config;
	bool configChanged = false;

	if(_stat("config.yaml",&buf) != 0)
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

			writeln(initmenu);

			//check that the input is valid (either Y,y,N, or n)
			//this can definitely be simplified but it works the way it is
			while ((input = getInput_s()).length() > 1 ||
				(input[0] != 'y' && input[0] != 'Y' && input[0] != 'n' && input[0] != 'N'))
			{
//TODO: fix print_debug
				//print_debug("ERROR: Invalid input.");
				writeln("\r\nERROR: Invalid input.\r\n");
				writeln(initmenu);
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

				writeln(fields);

				//loop until the input is a valid character ('1','2', or '3')
				while ((input = getInput_s()).length() > 1 ||
					input[0] < '1' || input[0] > '5')
				{
					print_debug("ERROR: Invalid input.");
					writeln(fields);
				}

				//TODO: validate field edits to ensure they are valid

						//switch based on the character the user input
				switch (input[0])
				{
					//1. Channel
				case '1':
					writeln("What channel should be bot chat in?");
					//convert channel name to lower case and prepend # to fit the twitch irc message formatting (see: https://github.com/justintv/Twitch-API/blob/master/IRC.md)
					tempCHAN = getInput_s();
					writeln("Bot is now configured to join channel: " + tempCHAN);
					configChanged = true;
					goto editfields;

					//2. Username
				case '2':
					writeln("What is the bot's new username?");
					//convert the username to lower case
					tempNICK = getInput_s();
					//logging into the twitch irc requires a valid oauth token starting with "oauth:" (see link above)
					writeln("What is the oath token for this username? (http://www.twitchapps.com/tmi/)");

					//write "oauth:" to the TextInput edit control and move the caret after it
					SetWindowText(g_TextInput.handle,L"oauth:");
					SendMessage(g_TextInput.handle, EM_SETSEL, 0, MAKELONG(0xffff, 0xffff));
					SendMessage(g_TextInput.handle, EM_SETSEL, -1, 0);

					tempPASS = getInput_s();
					tempPASS = "oauth:" + tempPASS;
					writeln("Bot is now configured to use the username: " + tempNICK);
					configChanged = true;
					goto editfields;

					//3. Owner
				case '3':
					writeln("What is the username of the new administrator?");
					tempOWNER = getInput_s();
					writeln("Bot administrator is now: " + tempOWNER);
					configChanged = true;
					goto editfields;

					//4. Save changes and return
				case '4':
					writeln("Saving configuration changes");
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

	writeln(L"\nBot initialized. Attempting to connect to server...");

	return true;
}
#endif