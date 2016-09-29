#pragma once

//TODO: migrate this to win32 platform layer

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