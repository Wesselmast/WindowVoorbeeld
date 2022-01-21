// Additional includes
#include "Utility.h"
#include "Input.h"
#include "Window.h"



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
		if (Input::sIsDown(MOUSE_L | MOUSE_R))
			gLog("Party Time!\n");
		else
			gLog("Lame party");

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

		// OnDestroy quits the entire application!
		gQuitApplication();
	}
};



/**
@brief Entry Point
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

	// Start the window message loop. This will run forever until all windows are closed.
	gProcessMessageLoop();

	// Return the error code of the program
	return 0;
}
