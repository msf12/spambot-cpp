#include "stdafx.h"
#include "globals.h"

string OWNER;
string NICK;
string PASS;
string CHAN;
bool SHOW_OPTIONS;
bool SHOW_SALUTATIONS;
string STARTUP;
string SHUTDOWN;

TextWindow g_TextInput{ new char[4000], 4000 };
TextWindow g_TextOutput{ new char[MAX_BUFFER_CHARS], MAX_BUFFER_CHARS };

TextWindowBuffer g_OutputStringBuffer;

void print_debug(char output[], bool error)
{
	static HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	if (error)
	{
		SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_INTENSITY);
	}
	else
	{
		SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
	}
	printf("%s\n", output);
	SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
}
