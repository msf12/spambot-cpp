#pragma once

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
