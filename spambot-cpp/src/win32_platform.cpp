#include <Winhttp.h>

// Note: Define the function macros before platform.h is included
#define PLATFORM_WRAPPER_NAME(function) Win32 ## function ## Wrapper

#define PLATFORM_THREAD_WRAPPER(function)\
DWORD WINAPI PLATFORM_WRAPPER_NAME(function) ## (LPVOID args)\
{\
	function(args);\
	return 1;\
}

#define PLATFORM_THREAD_WRAPPER_NO_ARGS(function)\
DWORD WINAPI PLATFORM_WRAPPER_NAME(function) ## (LPVOID args)\
{\
	function();\
	return 1;\
}

#include "platform.h"

// Win32 specific implementation of platform-independent function headers
void lock(void *mutex, unsigned long mstimeout = INFINITE)
{
	WaitForSingleObject((HANDLE) mutex, mstimeout);
}

void release(void *mutex)
{
	ReleaseMutex((HANDLE) mutex);
}

void *InitMutex()
{
	return CreateMutex(0, 0, nullptr);
}

void *spawn_thread(void *fptr, void *args)
{
	return CreateThread(0, 0, (LPTHREAD_START_ROUTINE) fptr, args, 0, 0);
}

wstring GetWStringInput()
{
	bool WaitingForInput = true;
	while (WaitingForInput)
	{
		// If the user has pressed enter in the TextInput window
		if (g_TextInput.updated)
		{
			// Get the text from the edit control and check that it's not an empty string
			GetWindowText(g_TextInput.handle, (LPWSTR)g_TextInput.buffer, (int)g_TextInput.bufferLen);
			if (g_TextInput.buffer[0] != '\0')
			{
				OutputDebugString((LPWSTR)g_TextInput.buffer);
				OutputDebugString(L"\n");

				// Clear the input field
				SetWindowText(g_TextInput.handle, L"");
				WaitingForInput = false;
			}
			g_TextInput.updated = false;
		}
	}
	return (LPWSTR)g_TextInput.buffer;
}

string GetStringInput()
{
	return StringToWString.to_bytes(GetWStringInput());
}

//TODO: add args/overloads?
void WriteToGUIIn(wchar_t *output)
{
	//write "oauth:" to the TextInput edit control and move the caret after it
	SetWindowText(g_TextInput.handle, output);
	SendMessage(g_TextInput.handle, EM_SETSEL, 0, MAKELONG(0xffff, 0xffff));
	SendMessage(g_TextInput.handle, EM_SETSEL, (unsigned int) -1, 0);
}

// Inline functions to wrap writing to the TextOutput
inline auto WritelnToGUIOut(const wstring &s)
{
	g_OutputStringBuffer.add(s + L"\r\n");

	// Write the output field
	SetWindowText(g_TextOutput.handle, g_OutputStringBuffer.to_serial_string().c_str());
	SendMessage(g_TextOutput.handle, WM_VSCROLL, SB_BOTTOM, NULL);
}

inline auto WritelnToGUIOut(wstring &&s)
{
	g_OutputStringBuffer.add(s + L"\r\n");

	// Write the output field
	SetWindowText(g_TextOutput.handle, g_OutputStringBuffer.to_serial_string().c_str());
	SendMessage(g_TextOutput.handle, WM_VSCROLL, SB_BOTTOM, NULL);
}

inline auto WriteToGUIOut(const wstring &s)
{
	g_OutputStringBuffer.add(s);

	// Write the output field
	SetWindowText(g_TextOutput.handle, g_OutputStringBuffer.to_serial_string().c_str());
	SendMessage(g_TextOutput.handle, WM_VSCROLL, SB_BOTTOM, NULL);
}

inline auto WriteToGUIOut(wstring &&s)
{
	g_OutputStringBuffer.add(s);

	// Write the output field
	SetWindowText(g_TextOutput.handle, g_OutputStringBuffer.to_serial_string().c_str());
	SendMessage(g_TextOutput.handle, WM_VSCROLL, SB_BOTTOM, NULL);
}

inline void WritelnToGUIOut(const string &s)
{
	// http://stackoverflow.com/questions/4804298/how-to-convert-wstring-into-string
	WritelnToGUIOut(StringToWString.from_bytes(s));
}

inline void WritelnToGUIOut(string &&s)
{
	WritelnToGUIOut(StringToWString.from_bytes(s));
}

inline void WriteToGUIOut(const string &s)
{
	WriteToGUIOut(StringToWString.from_bytes(s));
}

inline void WriteToGUIOut(string &&s)
{
	WriteToGUIOut(StringToWString.from_bytes(s));
}

inline void WriteToDebugOut(const string &s)
{
	WriteToGUIOut("[DEBUG] - " + s);
}

// TODO: replace that ^ with this v
#if 0
inline void WriteToDebugOut(char output[], bool error)
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
#endif

struct platform_http_vars
{
	HINTERNET WinHTTPSession, WinHTTPConnection;
};

platform_http_vars InitTwitchHTTPConnection()
{
	platform_http_vars Result = {};

	// TODO: Handle proxies?
	Result.WinHTTPSession = WinHttpOpen(0, WINHTTP_ACCESS_TYPE_NO_PROXY,
                                        WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS,
                                        WINHTTP_FLAG_ASYNC);
	Result.WinHTTPConnection = WinHttpConnect(Result.WinHTTPSession, L"api.twitch.tv",
	                                          INTERNET_DEFAULT_PORT, 0);

	return Result;
}

// TODO: Other requests beyond just follower requests
string MakeTwitchHTTPRequest(platform_http_vars TwitchAPIConnection, wstring Endpoint, wstring QueryParams, HTTPMethod Method = GET)
{
	string Response;
	LPCWCHAR AcceptTypes[] = { L"application/vnd.twitchtv.v3+json", 0 };
	const wchar_t *ClientIDHeader = L"Client-ID: 5fehntp2gxyxi4zb9lu81mob2qrqb5u";
	char buffer[Kilobytes(8)+1] = {};
	DWORD bytesToRead, bytesRead;

	HINTERNET WinHTTPRequest = WinHttpOpenRequest(TwitchAPIConnection.WinHTTPConnection, L"GET",
	                                              (Endpoint + QueryParams).c_str(),
	                                              0, WINHTTP_NO_REFERER,
	                                              AcceptTypes,
	                                              WINHTTP_FLAG_SECURE);

	WinHttpAddRequestHeaders(WinHTTPRequest, ClientIDHeader,
	                         (DWORD)(-1L), WINHTTP_ADDREQ_FLAG_ADD);

	WinHttpSendRequest(WinHTTPRequest, WINHTTP_NO_ADDITIONAL_HEADERS,
	                   0, 0, 0, 0, 0);

	WinHttpReceiveResponse(WinHTTPRequest, 0);

	WinHttpQueryDataAvailable(WinHTTPRequest, &bytesToRead);

	do
	{
		bytesRead = 0;
		WinHttpReadData(WinHTTPRequest, (LPVOID) buffer, bytesToRead, &bytesRead);
		buffer[bytesRead] = 0;
		Response += buffer;
		WinHttpQueryDataAvailable(WinHTTPRequest, &bytesToRead);
	} while(bytesToRead || bytesRead);	

	WinHttpCloseHandle(WinHTTPRequest);

	return Response;
}

void CloseTwitchHTTPConnection(platform_http_vars TwitchAPIConnection)
{
	WinHttpCloseHandle(TwitchAPIConnection.WinHTTPConnection);
	WinHttpCloseHandle(TwitchAPIConnection.WinHTTPSession);
}