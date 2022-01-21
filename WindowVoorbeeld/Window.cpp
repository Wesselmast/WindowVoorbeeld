#include "Window.h"

// Win32 includes
#include <windows.h>

// Additional includes
#include "Input.h"



/**
@brief HashMap that tracks windows by window handles
**/
class Window;
HashMap<HWND, Window*> gWindows;



/**
@brief Special input key used for accessing setter functions on the input class

WARNING:
Please use this class structure carefully, as this directly modifies the key registry

**/
struct InputKey { static void sSetDown(KeyCode inKeyCode, bool inDown) { Input::sSetDown(inKeyCode, inDown); } };



/**
@brief Helper function for when a mouse button is pressed
**/
static bool sOnMouseDown(Window* inWindow, KeyCode inKeyCode)
{
	InputKey::sSetDown(inKeyCode, true);
	return inWindow->OnMouseDown();
}



/**
@brief Helper function for when a mouse button is released
**/
static bool sOnMouseUp(Window* inWindow, KeyCode inKeyCode)
{
	InputKey::sSetDown(inKeyCode, false);
	return inWindow->OnMouseUp();
}



/**
@brief General window procedure
**/
#define PROC_DEFAULT DefWindowProc(inHandle, inMsg, inWParam, inLParam)
LRESULT CALLBACK gWindowProc(HWND inHandle, UINT inMsg, WPARAM inWParam, LPARAM inLParam)
{
	Window* window = nullptr;

	// Find or create the window associated with inHandle
	auto iter = gWindows.find(inHandle);
	if (iter == gWindows.end())
	{
		if (inMsg == WM_CREATE)
		{
			// Get the window from the LParams
			window = (Window*)((LPCREATESTRUCT)inLParam)->lpCreateParams;

			// Add to gWindows to keep track of the pointer
			gWindows.insert({inHandle, window});
		}
		else
		{
			// Do not handle any window messages before WM_CREATE
			return PROC_DEFAULT;
		}
	}
	else
	{
		// Get the created window from gWindows
		window = iter->second;
	}

	// Handle callbacks based on the input message
	switch (inMsg)
	{
		// Generic events
		case WM_CREATE: window->OnCreate(); return PROC_DEFAULT;
		case WM_PAINT:  window->OnPaint();  return PROC_DEFAULT;
		case WM_CLOSE:  window->OnClose();  return PROC_DEFAULT;

		// Mouse Down
		case WM_LBUTTONDOWN: return sOnMouseDown(window, VK_LBUTTON) ? 0 : PROC_DEFAULT;
		case WM_MBUTTONDOWN: return sOnMouseDown(window, VK_MBUTTON) ? 0 : PROC_DEFAULT;
		case WM_RBUTTONDOWN: return sOnMouseDown(window, VK_RBUTTON) ? 0 : PROC_DEFAULT;

		// Mouse Up
		case WM_LBUTTONUP: return sOnMouseUp(window, VK_LBUTTON) ? 0 : PROC_DEFAULT;
		case WM_MBUTTONUP: return sOnMouseUp(window, VK_MBUTTON) ? 0 : PROC_DEFAULT;
		case WM_RBUTTONUP: return sOnMouseUp(window, VK_RBUTTON) ? 0 : PROC_DEFAULT;

		// Key Events
		case WM_KEYDOWN:
		{
			InputKey::sSetDown((KeyCode)inWParam, true);
			return window->OnKeyDown() ? 0 : PROC_DEFAULT;
		}
		case WM_KEYUP:
		{
			InputKey::sSetDown((KeyCode)inWParam, false);
			return window->OnKeyUp() ? 0 : PROC_DEFAULT;
		}

		// Destroy
		case WM_DESTROY:	
		{
			window->OnDestroy();	

			// Also remove the window from gWindows and free its memory
			gWindows.erase(gWindows.find(inHandle));
			delete window;

			return PROC_DEFAULT;
		}
	}

	// Default case
	return PROC_DEFAULT;
}



/**
@brief Create and allocate a window
**/
Window* Window::sCreate(const IRect& inRect, const String& inName, void* inParent)
{
	Window* window	= (Window*)inParent;

	// UTF-16 version of @a inName because windows expects this
	WString wname = WString::sFromUTF8(inName);

	// Initialize a window class and register it
	WNDCLASS window_class = {};
	window_class.style			= 0;
	window_class.lpfnWndProc	= gWindowProc;
	window_class.lpszClassName	= wname + WString(L"WindowClass");
	window_class.hInstance		= GetModuleHandle(0);
	window_class.hIcon			= LoadIcon(0, IDI_WINLOGO);
	window_class.hCursor		= LoadCursor(0, IDC_ARROW);
	RegisterClass(&window_class);

	// Create the HWND
	window->mHandle = CreateWindowEx(0, window_class.lpszClassName, wname, WS_OVERLAPPEDWINDOW | WS_VISIBLE,
									inRect.mX, inRect.mY, inRect.mW, inRect.mH, 0, 0, window_class.hInstance, window);
	return window;
}



/**
@brief Force the window to be shown
**/
void Window::Show()
{
	ShowWindow(mHandle, SW_NORMAL);
}



/**
@brief Activate the window
**/
void Window::Activate()
{
	SetActiveWindow(mHandle);
}



/**
@brief Show and activate the window
**/
void Window::ShowAndActivate()
{
	Show();
	Activate();
}



/**
@brief Start the message loop for every created window
**/
void gProcessMessageLoop()
{
	MSG message;
	while (true) 
	{
		// Peek for the next message. This message can come from any window and is not filtered based on type.
		while (PeekMessage(&message, 0, 0, 0, PM_REMOVE))
		{
			// If we received a quit message, delete the windows we have and break out
			if (message.message == WM_QUIT) 
				goto quit;

			TranslateMessage(&message);
			DispatchMessage(&message);
		}
	}

quit:
	// Delete all windows
	for (Pair<HWND, Window*> pair : gWindows)
		delete pair.second;
	gWindows.clear();
}



/**
@brief Kill all windows and shut down the application
**/
void gQuitApplication()
{
	PostQuitMessage(0);
}
