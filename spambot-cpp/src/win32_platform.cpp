#include "win32_platform.h"

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
	WaitForSingleObject(mutex, mstimeout);
}

void release(void *mutex)
{
	ReleaseMutex(mutex);
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