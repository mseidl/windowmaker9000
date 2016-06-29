#ifndef WINDOW_MANAGER_H
#define WINDOW_MANAGER_H

extern "C"
{
#include <X11/Xlib.h>
}
#include <memory>
#include <mutex>

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

    // fames a top level window
    void Frame(Window win);

    // Unframes a client window
    void Unframe(Window win);

    // event handlers
    void OnCreateNotify(const XCreateWindowEvent& e);
    void OnDestroyNofity(const XDestroyWindowEvent& e);
    void OnReparentNotify(const XReparentEvent& e);
    void OnMapNotify(const XMapEvent& e);
    void OnUnmapNotify(const XUnmapEvent& e);
    void OnConfigureNotify(const XConfigureEvent& e);
    void OnMapRequest(const XMapRequestEvent& e);
    void OnConfigureRequest(const XConfigureRequestEvent& e);
    void OnButtonPress(const XButtonEvent& e);
    void OnButtonRelease(const XButtonEvent& e);
    void OnMotionNotify(const XMotionEvent& e);
    void OnKeyPress(const XKeyEvent& e);
    void OnKeyRelease(const XKeyEvent& e);

    
    // Xlib error handler
    static int OnXError(Display* display, XErrorEvent* e);

    // Xlib error handler used to whether another wm is running
    // It is set as the error handler right before selecting substructure
    // redirection mask on the root window, so it is invoked if and only if
    // another window manager is running
    static int OnWMDetected(Display* display, XErrorEvent* e);

     // Wheter an existing window manager has been detected, set by onWMDetected
    static bool wm_detected_;

    // Mutex for protecting wm_detected_
    static std::mutex wm_detected_mutex_;
 
    // Handles root window
    const Window root_;
};

#endif // WINDOW_MANAGER_H
