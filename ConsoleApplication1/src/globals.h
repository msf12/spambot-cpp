#pragma once

#include <cstdio>
#include <string>
#include <iostream>
#include <list>

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
	bool add(wstring str)
	{
		while (CharCount + str.length() > MAX_BUFFER_CHARS)
		{
			CharCount -= Buffer.front().length();
			Buffer.pop_front();
		}
		CharCount += str.length();
		Buffer.push_back(str);
		return true;
	}

	// Convert the contents of Buffer to a single C-style wstring/const wchar_t * and return a pointer to it
	const wchar_t *to_buffer_string()
	{
		wstring SerialBuffer;
		for (auto s : Buffer)
		{
			SerialBuffer += s;
		}
		auto BufferPtr = new wchar_t[SerialBuffer.length() + 1];
		BufferPtr[SerialBuffer.length()] = '\0';
		SerialBuffer.copy(BufferPtr, SerialBuffer.length());

		return BufferPtr;
	}
};

extern string OWNER;
extern string NICK;
extern string PASS;
extern string CHAN;
extern bool SHOW_OPTIONS;
extern bool SHOW_SALUTATIONS;
extern string STARTUP;
extern string SHUTDOWN;

extern TextWindow g_TextInput;
extern TextWindow g_TextOutput;

extern TextWindowBuffer g_OutputStringBuffer;

void print_debug(char output[], bool error = false);