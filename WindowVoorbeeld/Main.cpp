// Win32 includes
#include <windows.h>

// STL includes
#include <cassert>
#include <memory>
#include <type_traits>
#include <unordered_map>



/**
@brief Integer rectangle class
**/
struct IRect
{
	int mX = 0;		///< X position
	int mY = 0;		///< Y position
	int mW = 0;		///< Width
	int mH = 0;		///< Height
};



/**
@brief HashMap that tracks windows by window handles
**/
class Window;
std::unordered_map<HWND, Window*> gWindows;



/**
@brief Base Win32 window class
**/
class Window
{
public:
	///@name Create function + awful STL type traits stuff, which ensures that T inherits from Window
	template<class T>
	static typename std::enable_if<std::is_base_of<Window, T>::value, T*>::type sCreate(const IRect& inRect, LPCWSTR inName) { return (T*)sCreate(inRect, inName, new T); }

	///@name Interaction
	void				Show();								///< Force the window to be shown

	///@name Events (return if handled or not)
	virtual bool		OnCreate()		{ return false; }	///< Occurs when the window is created
	virtual bool		OnPaint()		{ return false; }	///< Occurs every time the window requests a repaint
	virtual bool		OnClose()		{ return false; }	///< Occurs when the window is closed
	virtual bool		OnDestroy()		{ return false; }	///< Occurs when the window is finally destroyed

protected:
	///@name Constructor
						Window() = default;					///< Private default constructor as we want windows to be created with Window::sCreate

private:
	
	static Window*		sCreate(const IRect& inRect, LPCWSTR inName, void* inParent); ///< Create a window

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
		// Events
		case WM_CREATE:		return window->OnCreate()	? 0 : PROC_DEFAULT;
		case WM_PAINT:		return window->OnPaint()	? 0 : PROC_DEFAULT;
		case WM_CLOSE:		return window->OnClose()	? 0 : PROC_DEFAULT;
		case WM_DESTROY:	return window->OnDestroy()	? 0 : PROC_DEFAULT;

		// Default
		default:			return PROC_DEFAULT;
	}
}



/**
@brief Create and allocate a window
**/
Window* Window::sCreate(const IRect& inRect, LPCWSTR inName, void* inParent)
{
	Window* window = (Window*)inParent;

	// Initialize a window class and register it
	WNDCLASS window_class = {};
	window_class.style = 0;
	window_class.lpfnWndProc = gWindowProc;
	window_class.hInstance = GetModuleHandle(0);
	window_class.lpszClassName = L"CoolNewFantasticWindowClass";
	window_class.hIcon = LoadIcon(0, IDI_WINLOGO);
	window_class.hCursor = LoadCursor(0, IDC_ARROW);
	RegisterClass(&window_class);

	// Create the HWND
	window->mHandle = CreateWindowEx(0, window_class.lpszClassName, inName, WS_OVERLAPPEDWINDOW | WS_VISIBLE,
									inRect.mX, inRect.mY, inRect.mW, inRect.mH, 0, 0, window_class.hInstance, window);
	return window;
}



/**
@brief Force the window to be shown
**/
void Window::Show()
{
	ShowWindow(mHandle, SW_SHOW);
}



/**
@brief Entry point 
**/
int gCustomEntryPoint();
int main()
{
	// Call custom entry point
	int result = gCustomEntryPoint();
	if (result != 0)
		return result;

	// Very basic message loop
	MSG message;
	while (true) 
	{
		while(PeekMessage(&message, 0, 0, 0, PM_REMOVE))
		{
			if(message.message == WM_QUIT) 
				goto quit;

			TranslateMessage(&message);
			DispatchMessage(&message);
		}
	}

quit:
	// Delete all windows
	for (std::pair<HWND, Window*> pair : gWindows)
		delete pair.second;
	gWindows.clear();

	return 0;
}



/// Jelle code...



/**
@brief Viewport window
**/
class ViewportWindow : public Window
{
public:
	virtual bool OnCreate() override 
	{
		// Init DX12
		printf("[CREATE] \tInitialized DX12\n");
		return false;
	}

	virtual bool OnPaint() override 
	{
		// Redraw
		printf("[REDRAW] \t%d\n", mCounter++);
		return false;
	}

	virtual bool OnClose() override
	{
		// Close
		printf("[CLOSE] \tClosed Window!\n");
		return false;
	}

private:
	int	mCounter = 0;
};



/**
@brief Hello World window
**/
class HelloWindow : public Window 
{
	virtual bool OnCreate() override
	{
		// Create
		printf("Hello, World!\n");
		return false;
	}
};



/**
@brief Custom entry point for e.g. a viewport application
**/
int gCustomEntryPoint()
{
	// Create viewport window and show it
	ViewportWindow* viewport_window = Window::sCreate<ViewportWindow>({ 50, 50, 550, 600 }, L"Viewport");
	viewport_window->Show();

	// As an example, we also create another window called 'Hello, Window!'
	HelloWindow* hello_window = Window::sCreate<HelloWindow>({100, 100, 400, 400}, L"Hello, Window!");
	hello_window->Show();

	return 0;
}