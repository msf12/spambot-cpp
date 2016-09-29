#pragma
#include <string>
#include "globals.h"

LRESULT CALLBACK MainWindowProc(
  _In_ HWND   hwnd,
  _In_ UINT   uMsg,
  _In_ WPARAM wParam,
  _In_ LPARAM lParam
);

BOOL CALLBACK EnumWindowProc(
	_In_ HWND   ChildWindow,
	_In_ LPARAM lParam
);

LRESULT CALLBACK InputWindowProc(
	_In_ HWND   Window,
	_In_ UINT   Message,
	_In_ WPARAM wParam,
	_In_ LPARAM lParam,
	UINT_PTR uIdSubclass,
	DWORD_PTR dwRefData
);

LRESULT CALLBACK OutputWindowProc(
	_In_ HWND   Window,
	_In_ UINT   Message,
	_In_ WPARAM wParam,
	_In_ LPARAM lParam,
	UINT_PTR uIdSubclass,
	DWORD_PTR dwRefData
);

// Inline functions to wrap writing to the TextOutput
// TODO: rename to WriteToGUIOut functions
inline auto writeln(const wstring &s)
{
	g_OutputStringBuffer.add(s + L"\r\n");

	// Write the output field
	SetWindowText(g_TextOutput.handle, g_OutputStringBuffer.to_serial_string().c_str());
	SendMessage(g_TextOutput.handle, WM_VSCROLL, SB_BOTTOM, NULL);
}

inline auto writeln(wstring &&s)
{
	g_OutputStringBuffer.add(s + L"\r\n");

	// Write the output field
	SetWindowText(g_TextOutput.handle, g_OutputStringBuffer.to_serial_string().c_str());
	SendMessage(g_TextOutput.handle, WM_VSCROLL, SB_BOTTOM, NULL);
}

inline auto write(const wstring &s)
{
	g_OutputStringBuffer.add(s);

	// Write the output field
	SetWindowText(g_TextOutput.handle, g_OutputStringBuffer.to_serial_string().c_str());
	SendMessage(g_TextOutput.handle, WM_VSCROLL, SB_BOTTOM, NULL);
}

inline auto write(wstring &&s)
{
	g_OutputStringBuffer.add(s);

	// Write the output field
	SetWindowText(g_TextOutput.handle, g_OutputStringBuffer.to_serial_string().c_str());
	SendMessage(g_TextOutput.handle, WM_VSCROLL, SB_BOTTOM, NULL);
}

inline void writeln(const string &s)
{
	// http://stackoverflow.com/questions/4804298/how-to-convert-wstring-into-string
	writeln(StringToWString.from_bytes(s));
}

inline void writeln(string &&s)
{
	writeln(StringToWString.from_bytes(s));
}

inline void write(const string &s)
{
	write(StringToWString.from_bytes(s));
}

inline void write(string &&s)
{
	write(StringToWString.from_bytes(s));
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
