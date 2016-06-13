#ifndef WINDOW_MANAGER_H
#define WINDOW_MANAGER_H

extern "C"
{
#include <X11/Xlib.h>
}
#include <memory>

class WindowManager
{
  public:
    // Factory method for connecting to xserver and getting windowmanager instance
    static std::unique_ptr<WindowManager> Create();
    // Disconnects from xserver
    ~WindowManager();
    // Entry point to class
    void Run();

  private:
    // Invoked by Create()
    WindowManager(Display* display);
    
    // Underlying display struct
    Display* display_;
    // Handles root window
    const Window root_;
};

#endif // WINDOW_MANAGER_H
