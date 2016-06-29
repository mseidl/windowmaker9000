extern "C"
{
#include <X11/Xutil.h>
}
#include "window_manager.hpp"
#include "util.hpp"
#include <glog/logging.h>

bool WindowManager::wm_detected_;
std::mutex WindowManager::wm_detected_mutex_;

std::unique_ptr<WindowManager> WindowManager::Create()
{
  Display* display = XOpenDisplay(nullptr);
  if(display == nullptr)
  {
    LOG(ERROR) << "Failed to open X display" << XDisplayName(nullptr);
    return nullptr;
  }

  return std::unique_ptr<WindowManager>(new WindowManager(display));
}

WindowManager::WindowManager(Display* display)
    : display_(CHECK_NOTNULL(display)),
      root_(DefaultRootWindow(display_))
{
}

WindowManager::~WindowManager()
{
  XCloseDisplay(display_);
}

void WindowManager::OnCreateNotify(const XCreateWindowEvent& e) { }

void WindowManager::OnDestroyNotify(const XDestroyWindowEvent& e) { }

void WindowManager::OnReparentNotify(const XReparentEvent& e) { }

void WindowManager::OnMapNotify(const XMapEvent& e) { }

void WindowManager::OnUnmapNotify(const XUnmapEvent& e)
{ 

}

void WindowManager::OnConfigureNotify(const XConfigureEvent& e) { }

void WindowManager::OnMapRequest(const XMapRequestEvent& e) { }

void WindowManager::OnConfigureRequest(const XConfigureRequestEvent& e) { }

void WindowManager::OnButtonPress(const XButtonEvent& e) { }

void WindowManager::OnButtonRelease(const XButtonEvent& e) { }

void WindowManager::OnMotionNotify(const XMotionEvent& e) { }

void WindowManager::OnKeyPress(const XKeyEvent& e) { }

void WindowManager::OnKeyRelease(const XKeyEvent& e) { }

void WindowManager::Run()
{
  // Select events on root window.  Use a special error handler
  // so we can exit gracefully if another wm is running
  std::lock_guard <std::mutex> lock(wm_detected_mutex_);

  wm_detected_ = false;
  XSetErrorHandler(&WindowManager::OnWMDetected);
  XSelectInput(display_, root_, SubstructureRedirectMask | SubstructureNotifyMask);
  XSync(display_, false);
  if(wm_detected_)
  {
    LOG(ERROR) << "Another window manager is running on display "
               << XDisplayString(display_);
    return;
  }
  XSetErrorHandler(&WindowManager::OnXError);
  
  // 2. Main event loop
  for (;;)
  {
    // Get next event
    XEvent xev;
    XNextEvent(display_, &xev); 
    LOG(INFO) << "Received event: " << ToString(xev);

    switch (xev.type)
    {
      case CreateNotify:
        OnCreateNotify(xev.xcreatewindow);
        break;
      case DestroyNotify:
        OnDestroyNotify(xev.xdestroywindow);
        break;
      default:
        LOG(WARNING) << "Unhandled event";
    }
  }
}

int WindowManager::OnXError(Display* display, XErrorEvent* e)
{
  //Todo
}

int WindowManager::OnWMDetected(Display* display, XErrorEvent* e)
{
  // in the case of another wm, the error code from XSelectInput
  // is BadAccess.  We don't expect this handler to receive any other 
  // errors.
  CHECK_EQ(static_cast<int>(e->error_code), BadAccess);
  wm_detected_ = true;
  return 0;
}
