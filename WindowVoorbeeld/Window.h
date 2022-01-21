#pragma once

// Additional includes
#include "Utility.h"



/**
@brief Start the message loop for every created window

This should be called in every application that uses windows, in order for them to function.
Make sure this is called after creating all the windows.
Windows created after this call will not be processed.

Example application entry point:

int main(int inArgC, char** inArgV)
{
	// Initialize the windows
	WindowA window_a = Window::sCreate<WindowA>({100, 100, 500, 500}, "WindowA");
	WindowB window_b = Window::sCreate<WindowB>({100, 100, 500, 500}, "WindowB");

	// ...
	
	gProcessMessageLoop();

	// ...

	return 0;
}
**/
extern void gProcessMessageLoop();



/**
@brief Kill all windows and shut down the application

WARNING:
Use with great care! This will close the entire application!

**/
extern void gQuitApplication();



/**
@brief Base Win32 window class
**/
struct WindowID;
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
	static Window*		sCreate(const IRect& inRect, const String& inName, void* inParent); ///< Create a window internally

	///@name Properties
	WindowID			mHandle;							///< Win32 window handle
};
