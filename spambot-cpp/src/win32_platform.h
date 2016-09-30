#pragma once

// Inline functions to wrap writing to the TextOutput
// TODO: rename to WriteToGUIOut functions
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
