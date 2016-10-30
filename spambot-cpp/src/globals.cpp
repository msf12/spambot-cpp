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

// Maintain a list of strings within MAX_BUFFER_CHARS
bool TextWindowBuffer::add(wstring str)
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
wstring TextWindowBuffer::to_serial_string()
{
	wstring SerialBuffer;
	for (auto s : Buffer)
	{
		SerialBuffer += s;
	}

	return SerialBuffer;
}

std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> StringToWString;
