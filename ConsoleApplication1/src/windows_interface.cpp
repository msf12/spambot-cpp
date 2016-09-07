#include "stdafx.h"
#include "windows_interface.h"

LRESULT CALLBACK MainWindowProc(
  _In_ HWND   Window,
  _In_ UINT   Message,
  _In_ WPARAM wParam,
  _In_ LPARAM lParam
)
{
	RECT ClientRect;
	GetClientRect(Window,&ClientRect);
	LRESULT Result = 0;
	switch(Message)
	{
		case WM_PAINT:
		{
			EnumChildWindows(Window, EnumWindowProc, (LPARAM)&ClientRect);
			break;	
		} break;
		case WM_CLOSE:
		{
			DestroyWindow(Window);
		} break;
		case WM_DESTROY:
		{
			PostQuitMessage(0);
		} break;
		case WM_CREATE:
		{
			g_TextOutput.handle = CreateWindow(
				L"EDIT", 
				NULL, 
				WS_CHILD | WS_VISIBLE | ES_LEFT | WS_VSCROLL | ES_MULTILINE | ES_READONLY,
				0, 0, 0, 0, 
				Window, 
				(HMENU) 100, 
				(HINSTANCE)GetWindowLong(Window, -6), // gets the hInstance of Window
				NULL
			);

			g_TextInput.handle = CreateWindow(
				L"EDIT",
				NULL,
				WS_CHILD | WS_VISIBLE | ES_LEFT | ES_AUTOHSCROLL,
				0, 0, 0, 0,
				Window,
				(HMENU)101,
				(HINSTANCE)GetWindowLong(Window, -6), // gets the hInstance of Window
				NULL
			);

			SendMessage(g_TextInput.handle, EM_LIMITTEXT, 4000, 0);

			SetWindowSubclass(g_TextInput.handle, EditWindowProc, 0, 0);
		} break;
		case WM_SETFOCUS:
		{
			SetFocus(g_TextInput.handle);
			return 0;
		} break;
		default:
		{
			Result = DefWindowProc(Window,Message,wParam,lParam);
		}	
	}

	return Result;
}

BOOL CALLBACK EnumWindowProc(
  _In_ HWND   ChildWindow,
  _In_ LPARAM lParam
)
{
	LPRECT ParentRect = (LPRECT) lParam;
	int ChildID = GetWindowLong(ChildWindow, GWL_ID);

	if(ChildID == 100)
	{
		MoveWindow(ChildWindow,
			0, 0,
			ParentRect->right - ParentRect->left,
			(ParentRect->bottom - 20) - ParentRect->top, true);
		ShowWindow(ChildWindow, SW_SHOW);
	}
	else
	{
		MoveWindow(ChildWindow,
			0, (ParentRect->bottom - 20) - ParentRect->top,
			ParentRect->right - ParentRect->left,
			ParentRect->bottom - 20, true);
		ShowWindow(ChildWindow, SW_SHOW);
	}
	return true;
}

LRESULT CALLBACK EditWindowProc(
  _In_ HWND   Window,
  _In_ UINT   Message,
  _In_ WPARAM wParam,
  _In_ LPARAM lParam,
  UINT_PTR uIdSubclass,
  DWORD_PTR dwRefData
)
{
	switch(Message)
	{
		case WM_KEYDOWN:
		{
			if(wParam == VK_RETURN)
			{
				g_TextInput.updated = true;
				OutputDebugString(L"Enter was pressed\n");
				return 0;
			}
		} break;
		case WM_CHAR:
		{
			if (wParam == VK_RETURN)
			{
				return 0;
			}
		}
		case WM_KILLFOCUS:
		{
			if((HWND)wParam == g_TextOutput.handle)
			{
				SetFocus(g_TextInput.handle);
			}
		}
		default:
		{
			return DefSubclassProc(Window, Message, wParam, lParam);	
		}
	}
	return 0;
}