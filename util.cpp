#include "util.hpp"
#include <sstream>
#include <algorithm>
#include <vector>

std::string ToString(const XEvent& xev)
{
  static const char*  const X_EVENT_TYPE_NAMES[] = 
  {
    "",
    "",
    "KeyPress",
    "KeyRelease",
    "ButtonPress",
    "ButtonRelease",
    "MotionNotify",
    "EnterNotify",
    "LeaveNotify",
    "FocusIn",
    "FocusOut",
    "KeymapNotify",
    "Expose",
    "GraphicsExpose",
    "NoExpose",
    "VisibilityNotify",
    "CreateNotify",
    "DestroyNotify",
    "UnmapNotify",
    "MapNotify",
    "MapRequest",
    "ReparentNotify",
    "ConfigureNotify",
    "ConfigureRequest",
    "GravityNotify",
    "ResizeRequest",
    "CirculateNotify",
    "CirculateRequest",
    "PropertyNotify",
    "SelectionClear",
    "SelectionRequest",
    "SelectionNotify",
    "ColormapNotify",
    "ClientMessage",
    "MappingNotify",
  };

  std::vector<std::pair<std::string, std::string>> properties;
  switch (xev.type)
  {
    case CreateNotify:
      properties.emplace_back("window", ToString(xev.xcreatewindow.window));
      properties.emplace_back("parent", ToString(xev.xcreatewindow.parent));
      properties.emplace_back("size", Size<int>(xev.xcreatewindow.width, xev.xcreatewindow.height).ToString());
      properties.emplace_back("position", Position<int>(xev.xcreatewindow.x, xev.xcreatewindow.y).ToString());
      properties.emplace_back("border_width", ToString(xev.xcreatewindow.border_width));
      break;
    case DestroyNotify:
      properties.emplace_back("window", ToString(xev.xdestroywindow.window));
      break;
    case MapNotify:
      properties.emplace_back("window", ToString(xev.xmap.window));
      properties.emplace_back("event", ToString(xev.xmap.event));
      properties.emplace_back("override_redirect", ToString(static_cast<bool>(xev.xmap.override_redirect)));
      break;
    case UnmapNotify:
      properties.emplace_back("window", ToString(xev.xunmap.window));
      properties.emplace_back("event", ToString(xev.xunmap.event));
      properties.emplace_back("from_configure", ToString(static_cast<bool>(xev.xunmap.from_configure)));
      break;
    case ConfigureNotify:
      properties.emplace_back("window", ToString(xev.xconfigure.window));
      properties.emplace_back("size", Size<int>(xev.xconfigure.width, xev.xconfigure.height).ToString());
      properties.emplace_back("position", Position<int>(xev.xconfigure.x, xev.xconfigure.y).ToString());
      properties.emplace_back("border_width", ToString(xev.xconfigure.border_width));
      properties.emplace_back("override_redirect", ToString(static_cast<bool>(xev.xconfigure.override_redirect)));
      break;
    case ReparentNotify:
      properties.emplace_back("window", ToString(xev.xreparent.window));
      properties.emplace_back("parent", ToString(xev.xreparent.parent));
      properties.emplace_back("position", Position<int>(xev.xreparent.x, xev.xreparent.y).ToString());
      properties.emplace_back("override_redirect", ToString(static_cast<bool>(xev.xreparent.override_redirect)));
      break;
    case MapRequest:
      properties.emplace_back("window", ToString(xev.xmaprequest.window));
      break;
    case ConfigureRequest:
      properties.emplace_back("window", ToString(xev.xconfigurerequest.window));
      properties.emplace_back("parent", ToString(xev.xconfigurerequest.parent));
      properties.emplace_back("position", Position<int>(xev.xconfigurerequest.x, xev.xconfigurerequest.y).ToString());
      properties.emplace_back("size", Size<int>(xev.xconfigurerequest.width, xev.xconfigurerequest.height).ToString());
      properties.emplace_back("border_width", ToString(xev.xconfigurerequest.border_width));
      break;
    case ButtonPress:
    case ButtonRelease:
      properties.emplace_back("window", ToString(xev.xbutton.window));
      properties.emplace_back("button", ToString(xev.xbutton.button));
      properties.emplace_back("position_root", Position<int>(xev.xbutton.x_root, xev.xbutton.y_root).ToString());
      break;
    case MotionNotify:
      properties.emplace_back("window", ToString(xev.xmotion.window));
      properties.emplace_back("position_root", Position<int>(xev.xmotion.x_root, xev.xmotion.y_root).ToString());
      properties.emplace_back("state", ToString(xev.xmotion.state));
      properties.emplace_back("time", ToString(xev.xmotion.time));
      break;
    case KeyPress:
    case KeyRelease:
      properties.emplace_back("window", ToString(xev.xkey.window));
      properties.emplace_back("state", ToString(xev.xkey.state));
      properties.emplace_back("keycode", ToString(xev.xkey.keycode));
      break;
    default:
      break;
  }
}