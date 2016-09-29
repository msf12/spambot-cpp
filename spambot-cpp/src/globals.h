#pragma once

#include <string>
#include <iostream>
#include <list>
#include <codecvt>
#include <locale>

using namespace std;

#define MAX_BUFFER_CHARS 32767

struct TextWindow
{
	char *buffer;
	size_t bufferLen;
	bool updated;
	HWND handle;
};

struct TextWindowBuffer
{
	size_t CharCount = 0;
	list<wstring> Buffer;
	
	// Maintain a list of strings within MAX_BUFFER_CHARS
	bool add(wstring str);

	// Convert the contents of Buffer to a single C-style wstring/const wchar_t * and return a pointer to it
	wstring to_serial_string();
};

//TODO: clean these up with a struct? Do they need to be global?
extern string OWNER;
extern string NICK;
extern string PASS;
extern string CHAN;
extern bool SHOW_OPTIONS;
extern bool SHOW_SALUTATIONS;
extern string STARTUP;
extern string SHUTDOWN;

extern string CLIENT_ID;

//TODO: is there a way to make these local to the first thread?
extern TextWindow g_TextInput;
extern TextWindow g_TextOutput;

//TODO: comment why this exists
extern TextWindowBuffer g_OutputStringBuffer;

extern std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> StringToWString;

//TODO: implement colored text for notices and errors and rename/reimpliment this function
void print_debug(char output[], bool error = false);