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

  // Build final string
  const std::string properties_string = Join(properties, ", ",
                                         [] (const std::pair<std::string, std::string> &pair)
                                         {
                                           return pair.first + ": " + pair.second;
                                         });
  std::ostringstream out;
  out << X_EVENT_TYPE_NAMES[xev.type] << " { " << properties_string << " }";
  return out.str();
}

std::string XConfigureWindowValueMaskToString(unsigned long value_mask)
{
  std::vector<std::string> masks;
  if (value_mask & CWX)
  {
    masks.emplace_back("X");
  }
  if (value_mask & CWY)
  {
    masks.emplace_back("Y");
  }
  if (value_mask & CWWidth)
  {
    masks.emplace_back("Width");
  }
  if (value_mask & CWHeight)
  {
    masks.emplace_back("Height");
  }
  if (value_mask & CWBorderWidth)
  {
    masks.emplace_back("BorderWidth");
  }
  if (value_mask & CWSibling)
  {
    masks.emplace_back("Sibling");
  }
  if (value_mask & CWStackMode)
  {
    masks.emplace_back("StackMode");
  }

  return Join(masks, "|");
}

std::string XRequestCodeToString(unsigned char request_code)
{
  static const char* const X_REQUEST_CODE_NAMES[] = {
      "",
      "CreateWindow",
      "ChangeWindowAttributes",
      "GetWindowAttributes",
      "DestroyWindow",
      "DestroySubwindows",
      "ChangeSaveSet",
      "ReparentWindow",
      "MapWindow",
      "MapSubwindows",
      "UnmapWindow",
      "UnmapSubwindows",
      "ConfigureWindow",
      "CirculateWindow",
      "GetGeometry",
      "QueryTree",
      "InternAtom",
      "GetAtomName",
      "ChangeProperty",
      "DeleteProperty",
      "GetProperty",
      "ListProperties",
      "SetSelectionOwner",
      "GetSelectionOwner",
      "ConvertSelection",
      "SendEvent",
      "GrabPointer",
      "UngrabPointer",
      "GrabButton",
      "UngrabButton",
      "ChangeActivePointerGrab",
      "GrabKeyboard",
      "UngrabKeyboard",
      "GrabKey",
      "UngrabKey",
      "AllowEvents",
      "GrabServer",
      "UngrabServer",
      "QueryPointer",
      "GetMotionEvents",
      "TranslateCoords",
      "WarpPointer",
      "SetInputFocus",
      "GetInputFocus",
      "QueryKeymap",
      "OpenFont",
      "CloseFont",
      "QueryFont",
      "QueryTextExtents",
      "ListFonts",
      "ListFontsWithInfo",
      "SetFontPath",
      "GetFontPath",
      "CreatePixmap",
      "FreePixmap",
      "CreateGC",
      "ChangeGC",
      "CopyGC",
      "SetDashes",
      "SetClipRectangles",
      "FreeGC",
      "ClearArea",
      "CopyArea",
      "CopyPlane",
      "PolyPoint",
      "PolyLine",
      "PolySegment",
      "PolyRectangle",
      "PolyArc",
      "FillPoly",
      "PolyFillRectangle",
      "PolyFillArc",
      "PutImage",
      "GetImage",
      "PolyText8",
      "PolyText16",
      "ImageText8",
      "ImageText16",
      "CreateColormap",
      "FreeColormap",
      "CopyColormapAndFree",
      "InstallColormap",
      "UninstallColormap",
      "ListInstalledColormaps",
      "AllocColor",
      "AllocNamedColor",
      "AllocColorCells",
      "AllocColorPlanes",
      "FreeColors",
      "StoreColors",
      "StoreNamedColor",
      "QueryColors",
      "LookupColor",
      "CreateCursor",
      "CreateGlyphCursor",
      "FreeCursor",
      "RecolorCursor",
      "QueryBestSize",
      "QueryExtension",
      "ListExtensions",
      "ChangeKeyboardMapping",
      "GetKeyboardMapping",
      "ChangeKeyboardControl",
      "GetKeyboardControl",
      "Bell",
      "ChangePointerControl",
      "GetPointerControl",
      "SetScreenSaver",
      "GetScreenSaver",
      "ChangeHosts",
      "ListHosts",
      "SetAccessControl",
      "SetCloseDownMode",
      "KillClient",
      "RotateProperties",
      "ForceScreenSaver",
      "SetPointerMapping",
      "GetPointerMapping",
      "SetModifierMapping",
      "GetModifierMapping",
      "NoOperation",
  };

  return X_REQUEST_CODE_NAMES[request_code];
}
