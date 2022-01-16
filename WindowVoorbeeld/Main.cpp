// Win32 includes
#include <windows.h>

// Extra
#include "Utility.h"
#include "Input.h"



/**
@brief HashMap that tracks windows by window handles
**/
class Window;
HashMap<HWND, Window*> gWindows;



/**
@brief Base Win32 window class
**/
class Window
{
public:
	///@name Create function + awful STL type traits stuff, which ensures that T inherits from Window
	template<class T>
	static typename std::enable_if<std::is_base_of<Window, T>::value, T*>::type sCreate(const IRect& inRect, const String& inName) { return (T*)sCreate(inRect, inName, new T); }

	///@name Interaction
	void				Show();								///< Force the window to be shown
	void				Activate();							///< Activate the window
	void				ShowAndActivate();					///< Show and activate the window

	///@name Events 
	virtual void		OnCreate()							{ }	///< Occurs when the window is created
	virtual void		OnPaint()							{ }	///< Occurs every time the window requests a repaint
	virtual void		OnClose()							{ }	///< Occurs when the window is closed
	virtual void		OnDestroy()							{ }	///< Occurs when the window is finally destroyed

	///@name Events with overrides (return 'true' means the event is handled)
	virtual bool 		OnKeyDown()							{ return false; }	///< Occurs when a key is down
	virtual bool 		OnKeyUp()							{ return false; }	///< Occurs when a key is released
	virtual bool 		OnMouseDown()						{ return false; }	///< Occurs when a mouse button is down
	virtual bool 		OnMouseUp()							{ return false; }	///< Occurs when a mouse button is released

protected:
	///@name Constructor
						Window() = default;					///< Private default constructor as we want windows to be created with Window::sCreate

private:
	static Window*		sCreate(const IRect& inRect, const String& inName, void* inParent); ///< Create a window

	///@name Properties
	HWND				mHandle;							///< Win32 window handle
};



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
		case WM_CREATE:		window->OnCreate();		return PROC_DEFAULT;
		case WM_PAINT:		window->OnPaint();		return PROC_DEFAULT;
		case WM_CLOSE:		window->OnClose();		return PROC_DEFAULT;

		// Mouse Events
		case WM_LBUTTONDOWN: Input::sSetDown(VK_LBUTTON, true);
		case WM_MBUTTONDOWN: Input::sSetDown(VK_MBUTTON, true);
		case WM_RBUTTONDOWN: Input::sSetDown(VK_RBUTTON, true);
		{
			return window->OnMouseDown() ? 0 : PROC_DEFAULT;
		}

		// Mouse Events
		case WM_LBUTTONUP: Input::sSetDown(VK_LBUTTON, false);
		case WM_MBUTTONUP: Input::sSetDown(VK_MBUTTON, false);
		case WM_RBUTTONUP: Input::sSetDown(VK_RBUTTON, false);
		{
			return window->OnMouseUp() ? 0 : PROC_DEFAULT;
		}

		// SysKeydown should not do window stuff, but should do input stuff
//		case WM_SYSKEYDOWN:
//		{
//			return PROC_DEFAULT;
//		}

		// Key Events
		case WM_KEYDOWN:
		{
			Input::sSetDown(inWParam, true);
			return window->OnKeyDown() ? 0 : PROC_DEFAULT;
		}
		case WM_KEYUP:
		{
			Input::sSetDown(inWParam, false);
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
@brief A very basic message loop
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



/// Calling code...



/**
@brief Viewport window
**/
class ViewportWindow : public Window
{
public:
	virtual void OnCreate() override 
	{
		// Init D3D11
		gLog("[CREATE] \tInitialized D3D11\n");
	}

	virtual void OnPaint() override 
	{
		// Redraw
		gLog("[REDRAW] \t%d\n", mCounter++);
	}

	virtual void OnClose() override
	{
		// Close
		gLog("[CLOSE] \tClosed Window!\n");
	}

private:
	int	mCounter = 0;
};



/**
@brief Hello World window
**/
class HelloWindow : public Window 
{
	virtual void OnCreate() override
	{
		gLog("Hello, World!\n");
	}

	virtual bool OnMouseDown() override 
	{
		// Only respond on CTRL + Click
		if (Input::sIsDown(KEY_CTRL | MOUSE_L))
			gLog("Party Time!\n");

		return true;
	}
};



/**
@brief Main window
**/
class MainWindow : public Window
{
	virtual void OnDestroy() override 
	{
		gLog("Destroying main window destroys all!\n");

		// Quit destroys all
		PostQuitMessage(0);
	}
};



/**
@brief Custom entry point for e.g. a viewport application
**/
int main()
{
	// Create viewport window 
	ViewportWindow* viewport_window = Window::sCreate<ViewportWindow>({300, 200, 550, 600 }, "Viewport");

	// As an example, we also create another window called 'Hello, Window!'
	HelloWindow* hello_window = Window::sCreate<HelloWindow>({800, 310, 400, 400}, "Hello, Window!");

	// As another example, create a main window class that destroys all other windows when destroyed
	MainWindow* main_window = Window::sCreate<MainWindow>({50, 50, 1820, 980}, "Main Window");

	// Show the windows in this order: MainWindow > Viewport > Hello, Window!
	main_window->ShowAndActivate();
	viewport_window->ShowAndActivate();
	hello_window->ShowAndActivate();

	// Start the win32 message loop. This will run forever until all windows are closed.
	gProcessMessageLoop();

	// Return the error code of the program
	return 0;
}
