// ConsoleApplication1.cpp : Defines the entry point for the console application.

#include "stdafx.h"
#include "windows_interface.h"
#include "globals.h"
#include "yaml-cpp/yaml.h"
#include <fstream>
#include <sys/stat.h>
#include <errno.h>

using namespace std;

bool botInit();

DWORD WINAPI BotMain(LPVOID args)
{
	botInit();
	//TODO: User input thread
	//TODO: Message receiver/sender thread
	//TODO: Message handler thread
	//TODO: Follower handler thread
	return 0;
}

int CALLBACK WinMain(
	_In_ HINSTANCE hInstance,
	_In_ HINSTANCE hPrevInstance,
	_In_ LPSTR     lpCmdLine,
	_In_ int       nCmdShow
)
{
	// Define and initialize WNDCLASSEX for the spambot window
	WNDCLASSEX WindowClass = {};
	WindowClass.cbSize = sizeof(WNDCLASSEX);
	WindowClass.style = CS_HREDRAW | CS_VREDRAW;
	WindowClass.lpfnWndProc = MainWindowProc;
	WindowClass.hInstance = hInstance;
	WindowClass.hIcon = LoadIcon(NULL,IDI_APPLICATION);
	WindowClass.hCursor = LoadCursor(NULL,IDC_ARROW);
	WindowClass.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
	WindowClass.lpszMenuName = NULL;
	WindowClass.lpszClassName = L"SpambotWindowClass";
	WindowClass.hIconSm = NULL;

	auto registerClassReturn = RegisterClassEx(&WindowClass);

	if(registerClassReturn == NULL)
	{
		//TODO: log errors
		return 0;
	}

	// Create main window
	auto WindowHandle = CreateWindow(
		WindowClass.lpszClassName,
		L"SpambotMainWindow",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		NULL,
		NULL,
		hInstance,
		NULL
	);

	if(!WindowHandle)
	{
		auto err = GetLastError();
	}

	ShowWindow(WindowHandle, nCmdShow);
	UpdateWindow(WindowHandle);

	LPDWORD ThreadIDPtr{};
	CreateThread(NULL, 0, BotMain, nullptr, 0, ThreadIDPtr);

    // Main message loop:
	MSG message;
	while(GetMessage(&message, NULL, 0, 0))
	{
		TranslateMessage(&message);
		DispatchMessage(&message);

		// If the user has pressed enter in the TextInput window
		//if(getInput())
		//{
			// Add the input to the Output buffer
		//	writeln((LPWSTR)g_TextInput.buffer);
		//}
	}

	OutputDebugString(g_OutputStringBuffer.to_buffer_string());

	//botInit();
	/*
	* TODO: the main loop of the program starts here
	* it should accept user input of two varieties
	* 1. plain text strings of arbitrary length to be sent to the chat via the bot
	* 2. commands (denoted by starting with !) which are executed directly by the bot
	*/


	write(L"Bot terminated. ");
	return 0;
}


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
			while ((input = getInput()).length() > 1 ||
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
				while ((input = getInput()).length() > 1 ||
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
					tempCHAN = getInput();
					writeln("Bot is now configured to join channel: " + tempCHAN);
					configChanged = true;
					goto editfields;

					//2. Username
				case '2':
					writeln("What is the bot's new username?");
					//convert the username to lower case
					tempNICK = getInput();
					//logging into the twitch irc requires a valid oauth token starting with "oauth:" (see link above)
					writeln("What is the oath token for this username? (http://www.twitchapps.com/tmi/)");
					write("oauth:");
					tempPASS = getInput();
					tempPASS = "oauth:" + tempPASS;
					writeln("Bot is now configured to use the username: " + tempNICK);
					configChanged = true;
					goto editfields;

					//3. Owner
				case '3':
					writeln("What is the username of the new administrator?");
					tempOWNER = getInput();
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

	return true;
}
