#include <winsock2.h>
#include <Windows.h>
#include <Commctrl.h>
#include "globals.cpp"
#include "win32_callbacks.cpp"
#include "win32_platform.cpp"
#include "spambot.cpp"
//#include <sys/stat.h>
//#include <stdlib.h>
//#include <crtdbg.h>
//#include <stdlib.h>
//#include <tchar.h>
//#include <string.h>
//#include <errno.h>

using namespace std;

int CALLBACK WinMain(
	_In_ HINSTANCE hInstance,
	_In_ HINSTANCE hPrevInstance,
	_In_ LPSTR     lpCmdLine,
	_In_ int       nCmdShow
)
{
	// Define and initialize WNDCLASSEX for the spambot window
	WNDCLASSEX WindowClass = {};
	WindowClass.cbSize = sizeof(WNDCLASSEX);
	WindowClass.style = CS_HREDRAW | CS_VREDRAW;
	WindowClass.lpfnWndProc = MainWindowProc;
	WindowClass.hInstance = hInstance;
	WindowClass.hIcon = LoadIcon(NULL,IDI_APPLICATION);
	WindowClass.hCursor = LoadCursor(NULL,IDC_ARROW);
	WindowClass.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
	WindowClass.lpszMenuName = NULL;
	WindowClass.lpszClassName = L"SpambotWindowClass";
	WindowClass.hIconSm = NULL;

	auto registerClassReturn = RegisterClassEx(&WindowClass);

	if(registerClassReturn == NULL)
	{
		//TODO: log errors
		*(char *)0;
		return 0;
	}

	// Create main window
	auto WindowHandle = CreateWindow(
		WindowClass.lpszClassName,
		L"SpambotMainWindow",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		NULL,
		NULL,
		hInstance,
		NULL
	);

	if(!WindowHandle)
	{
		//auto err = GetLastError();
	}

	ShowWindow(WindowHandle, nCmdShow);
	UpdateWindow(WindowHandle);

	//LPDWORD ThreadIDPtr{};
	CreateThread(0, 0, PLATFORM_WRAPPER_NAME(BotMain), 0, 0, 0);


    // Main message loop:
	MSG message;
	while(GetMessage(&message, NULL, 0, 0))
	{
		TranslateMessage(&message);
		DispatchMessage(&message);
	}

	OutputDebugString(g_OutputStringBuffer.to_serial_string().c_str());

	WriteToGUIOut(L"Bot terminated. ");
	//_CrtDumpMemoryLeaks();
	return 0;
}
