extern "C"
{
#include <X11/Xutil.h>
}

#include <algorithm>
#include <cstring>
#include <glog/logging.h>
#include "window_manager.hpp"
#include "util.hpp"

bool WindowManager::wm_detected_;
std::mutex WindowManager::wm_detected_mutex_;

std::unique_ptr<WindowManager> WindowManager::Create(const std::string& disp_str)
{
  const char* display_str = disp_str.empty() ? nullptr : disp_str.c_str();
  Display* display = XOpenDisplay(display_str);
  if(display == nullptr)
  {
    LOG(ERROR) << "Failed to open X display" << XDisplayName(nullptr);
    return nullptr;
  }

  return std::unique_ptr<WindowManager>(new WindowManager(display));
}

WindowManager::WindowManager(Display* display)
    : display_(CHECK_NOTNULL(display)),
      root_(DefaultRootWindow(display_)),
      WM_PROTOCOLS(XInternAtom(display_, "WM_PROTOCOLS", false)),
      WM_DELETE_WINDOW(XInternAtom(display_, "WM_DELETE_WINDOW", false))

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
  if (!clients_.count(e.window))
    {
      LOG(INFO) << "Ignore unmap notify for non-client window" << e.window;
      return;
    }
  if (e.event == root_)
  {
    LOG(INFO) << "Ignore unmap notify for reparented window" << e.window;
    return;
  }
  Unframe(e.window);
}

void WindowManager::OnConfigureNotify(const XConfigureEvent& e) { }

void WindowManager::OnMapRequest(const XMapRequestEvent& e)
{ 
  Frame(e.window);
  XMapWindow(display_, e.window);
}

void WindowManager::OnConfigureRequest(const XConfigureRequestEvent& e)
{
  XWindowChanges wchanges;
  wchanges.x = e.x;
  wchanges.y = e.y;
  wchanges.width = e.width;
  wchanges.height = e.height;
  wchanges.border_width = e.border_width;
  wchanges.sibling = e.above;
  wchanges.stack_mode = e.detail;
  if (clients_.count(e.window))
  {
    const Window frame = clients_[e.window];
    XConfigureWindow(display_, frame, e.value_mask, &wchanges);
    LOG(INFO) << "resize " << frame << "to" << Size<int>(e.width, e.height);
  }

  XConfigureWindow(display_, e.window, e.value_mask, &wchanges);
  LOG(INFO) << "resize " << e.window << "to" << Size<int>(e.width, e.height);
}

void WindowManager::OnButtonPress(const XButtonEvent& e)
{
  CHECK(clients_.count(e.window));
  const Window frame = clients_[e.window];
 
  // save original cursor position 
  drag_start_pos_ = Position<int>(e.x_root, e.y_root);

  // save window info
  Window returned_root;
  int x, y;
  unsigned width, height, border_width, depth;
  CHECK(XGetGeometry(
        display_,
        frame,
        &returned_root,
        &x,
        &y,
        &width,
        &height,
        &border_width,
        &depth));

  drag_start_frame_pos_ = Position<int>(x, y);
  drag_start_frame_size_ = Size<int>(width, height);

  // raised click window
  XRaiseWindow(display_, frame);
}

void WindowManager::OnButtonRelease(const XButtonEvent& e) { }

void WindowManager::OnMotionNotify(const XMotionEvent& e)
{
  CHECK(clients_.count(e.window));
  const Window frame = clients_[e.window];
  const Position<int> drag_pos(e.x_root, e.y_root);
  const Vector2D<int> delta = drag_pos - drag_start_pos_;

  if (e.state & Button1Mask)
  {
    const Position<int> dest_frame_pos = drag_start_frame_pos_ + delta;
    XMoveWindow(display_, frame, dest_frame_pos.x, dest_frame_pos.y);
  }
  else if (e.state & Button3Mask)
  {
    // resize, make sure numbers aren't negative
    const Vector2D<int> size_delta(
        std::max(delta.x, -drag_start_frame_size_.width),
        std::max(delta.y, -drag_start_frame_size_.height));
    const Size<int> dest_frame_size = drag_start_frame_size_ + size_delta;

    XResizeWindow(display_, frame, dest_frame_size.width, dest_frame_size.height);
    XResizeWindow(display_, e.window, dest_frame_size.width, dest_frame_size.height);
  } 
}

void WindowManager::OnKeyPress(const XKeyEvent& e)
{
  if ((e.state & Mod1Mask) && e.keycode == XKeysymToKeycode(display_, XK_F4))
  {
    Atom* supported_protocols;
    int num_supported_protocols;
    if (XGetWMProtocols(display_, e.window, &supported_protocols, &num_supported_protocols) &&
      (std::find(supported_protocols, supported_protocols + num_supported_protocols, WM_DELETE_WINDOW) !=
        supported_protocols + num_supported_protocols))
    {
      LOG(INFO) << "Deleting window: " << e.window;
      XEvent msg;
      memset(&msg, 0, sizeof(msg));
      msg.xclient.type = ClientMessage;
      msg.xclient.message_type = WM_PROTOCOLS;
      msg.xclient.window = e.window;
      msg.xclient.format = 32;
      msg.xclient.data.l[0] = WM_DELETE_WINDOW;

      CHECK(XSendEvent(display_, e.window, false, 0, &msg));
    }
    else
    {
      LOG(INFO) << "killing window: " << e.window;
      XKillClient(display_, e.window);
    }
  }
  else if ((e.state & Mod1Mask) && e.keycode == XKeysymToKeycode(display_, XK_Tab))
  {
    // we're alt-tabbing
   //find next window
    auto i = clients_.find(e.window);
    CHECK(i != clients_.end());
    ++i;
    if (i == clients_.end())
    {
      i = clients_.begin();
    }

    //raise and focus on
    XRaiseWindow(display_, i->second);
    XSetInputFocus(display_, i->first, RevertToPointerRoot, CurrentTime);
  }
}

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
  // set error handler
  XSetErrorHandler(&WindowManager::OnXError);
  
  // grab x to prevent windows from changing under us
  XGrabServer(display_);

  // reparent and query top level windows
  Window returned_root, returned_parent;
  Window* top_level_windows;
  unsigned int num_top_level_windows;
  CHECK(XQueryTree(display_, root_, &returned_root, &returned_parent, &top_level_windows, &num_top_level_windows));

  for (unsigned int i = 0; i < num_top_level_windows; ++i)
  {
    Frame(top_level_windows[i]);
  }
  
  XFree(top_level_windows);
  XUngrabServer(display_);
  // 2. Main event loop
  for (;;)
  {
    // Get next event
    XEvent xev;
    XNextEvent(display_, &xev); 
    LOG(INFO) << "Received event: " << ToString(xev.type);

    switch (xev.type)
    {
      case CreateNotify:
        OnCreateNotify(xev.xcreatewindow);
        break;
      case DestroyNotify:
        OnDestroyNotify(xev.xdestroywindow);
        break;
      case ReparentNotify:
        OnReparentNotify(xev.xreparent);
        break;
      case MapNotify:
        OnMapNotify(xev.xmap);
        break;
      case UnmapNotify:
        OnUnmapNotify(xev.xunmap);
        break;
      case ConfigureNotify:
        OnConfigureNotify(xev.xconfigure);
        break;
      case MapRequest:
        OnMapRequest(xev.xmaprequest);
        break;
      case ConfigureRequest:
        OnConfigureRequest(xev.xconfigurerequest);
        break;
      case ButtonPress:
        OnButtonPress(xev.xbutton);
        break;
      case ButtonRelease:
        OnButtonRelease(xev.xbutton);
        break;
      case MotionNotify:
        while (XCheckTypedWindowEvent(display_, xev.xmotion.window, MotionNotify, &xev)) {}
        OnMotionNotify(xev.xmotion);
        break;
      case KeyPress:
        OnKeyPress(xev.xkey);
        break;
      case KeyRelease:
        OnKeyRelease(xev.xkey);
        break;
      default:
        LOG(WARNING) << "Unhandled event";
    }
  }
}

void WindowManager::Frame(Window win) 
{
  const unsigned int BORDER_WIDTH = 3;
  const unsigned long BORDER_COLOR = 0xff0000;
  const unsigned long BG_COLOR = 0x0000ff;
  
  CHECK(!clients_.count(win));

  XWindowAttributes x_window_attrs;
  CHECK(XGetWindowAttributes(display_, win, &x_window_attrs));

  const Window frame = XCreateSimpleWindow(
      display_,
      root_,
      x_window_attrs.x,
      x_window_attrs.y,
      x_window_attrs.height,
      x_window_attrs.width,
      BORDER_WIDTH,
      BORDER_COLOR,
      BG_COLOR);

  // select events on frame
  XSelectInput(display_, frame, SubstructureRedirectMask | SubstructureNotifyMask);
  //save so restored in case of crash
  XAddToSaveSet(display_, win);
  
  XReparentWindow(display_, win, frame, 0, 0);

  XMapWindow(display_, frame);

  clients_[win] = frame;

  // grab window manage actions on client window
  // move windows with alt and left mouse
  XGrabButton(
      display_,
      Button1,
      Mod1Mask,
      win,
      false,
      ButtonPressMask | ButtonReleaseMask | ButtonMotionMask,
      GrabModeAsync,
      GrabModeAsync,
      None,
      None);

  // alt + right click for resize
  XGrabButton(
      display_,
      Button3,
      Mod1Mask,
      win,
      false,
      ButtonPressMask | ButtonPressMask | ButtonMotionMask,
      GrabModeAsync,
      GrabModeAsync,
      None,
      None);

  XGrabKey(
      display_,
      XKeysymToKeycode(display_, XK_F4),
      Mod1Mask,
      win,
      false,
      GrabModeAsync, 
      GrabModeAsync);
  // switch with alt+tab
  XGrabKey(
      display_,
      XKeysymToKeycode(display_, XK_Tab),
      Mod1Mask,
      win,
      false,
      GrabModeAsync,
      GrabModeAsync);
  LOG(INFO) << "framed window: " << win; 
}

void WindowManager::Unframe(Window win)
{
  CHECK(clients_.count(win));

  // reverse steps taken in frame
  const Window frame = clients_[win];
  XUnmapWindow(display_, frame);

  XReparentWindow(
      display_,
      win,
      root_,
      0,
      0);

  XRemoveFromSaveSet(display_, win);
  XDestroyWindow(display_, frame);

  clients_.erase(win);

  LOG(INFO) << "unframed window: " << win;
}

int WindowManager::OnXError(Display* display, XErrorEvent* e)
{
  const int MAX_ERR_LEN = 1024;
  char ERR_TXT[MAX_ERR_LEN];
  XGetErrorText(display, e->error_code, ERR_TXT, sizeof(ERR_TXT));
  LOG(ERROR) << "Received X error:\n"
             << "    Request: " << int(e->request_code)
             << " - " << XRequestCodeToString(e->request_code) << "\n"
             << "    Error code: " << int(e->error_code)
             << " - " << ERR_TXT << "\n"
             << "    Resource ID: " << e->resourceid;
    // The return value is ignored.
    //   return 0;
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
