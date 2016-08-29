// ConsoleApplication1.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "globals.h"
#include "yaml-cpp/yaml.h"
#include <iostream>
#include <fstream>
#include <sys/stat.h>
#include <errno.h>

using namespace std;
extern string OWNER;
extern string NICK;
extern string PASS;
extern string CHAN;
extern bool SHOW_OPTIONS;
extern bool SHOW_SALUTATIONS;
extern string STARTUP;
extern string SHUTDOWN;

bool botInit();

int main()
{

	botInit();
	/*
	 * TODO: the main loop of the program starts here
	 * it should accept user input of two varieties
	 * 1. plain text strings of arbitrary length to be sent to the chat via the bot
	 * 2. commands (denoted by starting with !) which are executed directly by the bot
	 */

	cout << "Bot terminated. ";
	system("pause");
	return 0;
}


bool botInit()
{
	cout << "BOT STARTING!" << endl;

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

	cout << "config.yaml contents - " << endl << config << endl;

	SHOW_OPTIONS = config["SHOW_OPTIONS"].as<bool>();
	SHOW_SALUTATIONS = config["SHOW_SALUTATIONS"].as<bool>();

	if(!SHOW_OPTIONS)
	{
		goto initComplete;
	}

	//string listing current settings and asking the user whether they should be edited
	string initmenu;
	string input;

	//label to return to if editing is canceled
init:
	initmenu = "This bot will be chatting as: " + config["NICK"].as<string>() + "\n"
			"This bot will be chatting in the channel: " + config["CHAN"].as<string>() + "\n"
			"The administrator for the bot is " + config["OWNER"].as<string>() + "\n"
			"Would you like to edit these settings? [Y/n]\n"
			"(To disable this message, change SHOW_OPTIONS to false in irc.conf)";
	
	cout << initmenu << endl;

	//check that the input is valid (either Y,y,N, or n)
	//this can definitely be simplified but it works the way it is
	while(getline(cin, input) &&
		(input.length() > 1 ||
		(input[0] != 'y' && input[0] != 'Y' && input[0] != 'n' && input[0] != 'N')))
	{
		print_debug("ERROR: Invalid input.");
		cout << initmenu << endl;
	}

	//if the input was Y or y show the field edit menu otherwise continue starting the bot
	if(input[0] == 'y' || input[0] == 'Y')
	{
		//string listing the current values of editable fields
		string fields;
		
		//create temp variables for the fields to propogate edits without loosing the original values
		auto tempCHAN = config["CHAN"].as<string>(), tempNICK = config["NICK"].as<string>(), tempPASS = config["PASS"].as<string>(), tempOWNER = config["OWNER"].as<string>();

	//label to return to after editing a field
	editfields:
		fields = "Choose a field to edit:\n"
			"1. Channel:     " + tempCHAN + "\n"
			"2. Username:    " + tempNICK + "\n"
			"3. Owner:       " + tempOWNER + "\n"
			"4. Save changes and return\n"
			"5. Cancel changes and return";

		cout << fields << endl;

		////loop until the input is a valid character ('1','2', or '3')
		while(getline(cin, input) &&
			(input.length() > 1 || input[0] < '1' || input[0] > '5'))
		{
			print_debug("ERROR: Invalid input.");
			cout << fields << endl;
		}

//TODO: validate field edits to insure they are valid

		//switch based on the character the user input
		switch(input[0])
		{
			//1. Channel
			case '1':
				cout << "What channel should be bot chat in?" << endl;
				//convert channel name to lower case and prepend # to fit the twitch irc message formatting (see: https://github.com/justintv/Twitch-API/blob/master/IRC.md)
				getline(cin,tempCHAN);
				cout << "Bot is now configured to join channel: " << tempCHAN << endl;
				configChanged = true;
				goto editfields;

			//2. Username
			case '2':
				cout << "What is the bot's new username?" << endl;
				//convert the username to lower case
				getline(cin, tempNICK);
				//logging into the twitch irc requires a valid oauth token starting with "oauth:" (see link above)
				cout << "What is the oath token for this username? (http://www.twitchapps.com/tmi/)" << endl;
				cout << "oauth:";
				getline(cin, tempPASS);
				tempPASS = "oauth:" + tempPASS;
				cout << "Bot is now configured to use the username: " << tempNICK << endl;
				configChanged = true;
				goto editfields;

			//3. Owner
			case '3':
				cout << "What is the username of the new administrator?" << endl;
				getline(cin, tempOWNER);
				cout << "Bot administrator is now: " << tempOWNER << endl;
				configChanged = true;
				goto editfields;

			//4. Save changes and return
			case '4':
				cout << "Saving configuration changes" << endl;
				config["OWNER"] = tempOWNER;
				config["NICK"] = tempNICK;
				config["PASS"] = tempPASS;
				config["CHAN"] = tempCHAN;
				config["SHOW_OPTIONS"] = SHOW_OPTIONS;
				goto init;

			//5. Cancel changes and return
			case '5':
				goto init;

			//seriously, if the code ever ends up here, I don't even know what could have gone wrong
			default:
				print_debug("ERROR: If you are seeing this something has gone very wrong",true);
				exit(1);
		}
	}

initComplete:
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
