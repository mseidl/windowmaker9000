#include "window_manager.hpp"
#include <glog/logging.h>

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

void WindowManager::Run()
{
  // later
}
