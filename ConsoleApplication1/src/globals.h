#pragma once
#include <cstdio>
#include <string>

using std::string;

string OWNER;
string NICK;
string PASS;
string CHAN;
bool SHOW_OPTIONS;
bool SHOW_SALUTATIONS;
string STARTUP;
string SHUTDOWN;

void print_debug(char output[], bool error = false)
{
	static HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	if(error)
	{
		SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_INTENSITY);
	}
	else
	{
		SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN	| FOREGROUND_INTENSITY);
	}
	printf("%s\n", output);
	SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
}
