/***************************************************************************
                       system_x11.cc  -  X11 platform specific routines
                               -------------------
      copyright            : (C) 2003 by Morten Hustveit
      email                : morten@debian.org
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <deque>
#include <map>

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/extensions/xf86vmode.h>
#include <X11/keysym.h>
#include <X11/Xatom.h>
#include <GL/glxtokens.h>

#include <espace/cvar.h>
#include <espace/input.h>
#include <espace/network.h>
#include <espace/opengl.h>
#include <espace/output.h>
#include <espace/sound.h>
#include <espace/system.h>

namespace
{
  Display*            display = 0;
  Window              window;
  XVisualInfo*        visual;
  bool                restore = false;
  XF86VidModeModeInfo oldMode;
  XF86VidModeGamma    oldGamma;

  std::map<String, CVar> vars;

  Bool waitForMapNotify(Display*, XEvent* event, char* arg);
  int  keyNum(KeySym keySym);
  void cvarChanged(const char* name);

  typedef struct __GLXcontextRec *GLXContext;
  typedef XID GLXDrawable;

  typedef Bool (*_glXQueryExtension)(Display *dpy, int *errorBase,
                                    int *eventBase);
  typedef XVisualInfo* (*_glXChooseVisual)(Display *dpy, int screen,
                                          int *attribList);
  typedef GLXContext (*_glXCreateContext)(Display *dpy, XVisualInfo *vis,
                                         GLXContext shareList, Bool direct);
  typedef Bool (*_glXMakeCurrent)(Display *dpy, GLXDrawable drawable,
                                 GLXContext ctx);
  typedef void (*_glXSwapBuffers)(Display *dpy, GLXDrawable drawable);

  _glXQueryExtension glXQueryExtension;
  _glXChooseVisual glXChooseVisual;
  _glXCreateContext glXCreateContext;
  _glXMakeCurrent glXMakeCurrent;
  _glXSwapBuffers glXSwapBuffers;
}

void System::initialize()
{
  esInfo << "Empty Space version " << PACKAGE_VERSION << "." << std::endl;

#ifdef LINUX
  initializeLinux();
#endif
#ifdef GNU
  initializeGNU();
#endif

  display = XOpenDisplay(0);

  if(!display)
  {
    const char* display = getenv("DISPLAY");

    esError << "Failed to open display " << (display ? display : "(null)") << std::endl;

    ::exit(EXIT_FAILURE);
  }

  GL::loadLibrary();

  glXQueryExtension = (_glXQueryExtension) System::dlsym(GL::library, "glXQueryExtension");
  glXChooseVisual = (_glXChooseVisual) System::dlsym(GL::library, "glXChooseVisual");
  glXCreateContext = (_glXCreateContext) System::dlsym(GL::library, "glXCreateContext");
  glXMakeCurrent = (_glXMakeCurrent) System::dlsym(GL::library, "glXMakeCurrent");
  glXSwapBuffers = (_glXSwapBuffers) System::dlsym(GL::library, "glXSwapBuffers");

  if(!glXQueryExtension(display, 0, 0))
  {
    esError << "No GLX extension present." << std::endl;

    ::exit(EXIT_FAILURE);
  }

  int attributes[] =
  {
    GLX_RGBA,
    GLX_DOUBLEBUFFER,
    GLX_DEPTH_SIZE, 1,
    None
  };

  visual = glXChooseVisual(display, DefaultScreen(display), attributes);

  if(!visual)
  {
    esError << "No appropriate GLX visual found." << std::endl;

    ::exit(EXIT_FAILURE);
  }

  XF86VidModeGetGamma(display, visual->screen, &oldGamma);

  vars["in_mouse"] = CVar::acquire("in_mouse", "1", CVar::Archive);
  vars["vid_gamma"] = CVar::acquire("vid_gamma", "1.3", CVar::Archive);

  for(std::map<String, CVar>::iterator i = vars.begin();
      i != vars.end(); ++i)
  {
    i->second.setCallback(cvarChanged);
  }

  cvarChanged("vid_gamma");

  CVar videoMode = CVar::acquire("r_mode", "3", CVar::Archive);

  uint width, height;

  if(videoMode.string[1] == 0)
  {
    switch(videoMode.integer)
    {
    case 0: width = 320; height = 240; break;
    case 1: width = 400; height = 300; break;
    case 2: width = 512; height = 384; break;
    case 3: width = 640; height = 480; break;
    case 4: width = 800; height = 600; break;
    case 5: width = 960; height = 720; break;
    case 6: width = 1024; height = 768; break;
    case 7: width = 1152; height = 864; break;
    case 8: width = 1280; height = 960; break;
    case 9: width = 1600; height = 1200; break;
    case 10: width = 2048; height = 1536; break;
    case 11: width = 856; height = 480; break;
    default: width = 640; height = 480;
    }
  }
  else
  {
    sscanf(videoMode.string, "%ux%u", &width, &height);

    if(width < 320 || width > 4096
    || height < 200 || height > 4096)
    {
      esWarning << "Invalid resolution \"" << videoMode.string << "\"."
                << std::endl;

      width = 640;
      height = 480;
    }
  }

  GL::config.width = width;
  GL::config.height = height;

  CVar fullscreen = CVar::acquire("r_fullscreen", "1", CVar::Archive);

  if(fullscreen.integer)
  {
    XF86VidModeModeInfo** modeInfo;
    int modeCount;

    XF86VidModeGetAllModeLines(display, visual->screen, &modeCount,
                               &modeInfo);

    oldMode = *modeInfo[0];
    restore = true;

    int smallestArea = INT_MAX;
    int smallestMode = 0;

    for(int i = 0; i < modeCount; ++i)
    {
      if(modeInfo[i]->hdisplay >= width
      && modeInfo[i]->vdisplay >= height)
      {
        if(modeInfo[i]->hdisplay * modeInfo[i]->vdisplay < smallestArea)
        {
          smallestArea = modeInfo[i]->hdisplay * modeInfo[i]->vdisplay;
          smallestMode = i;
        }
      }
    }

    XF86VidModeSwitchToMode(display, visual->screen, modeInfo[smallestMode]);
    XF86VidModeSetViewPort(display, visual->screen, 0, 0);

    XFree(modeInfo);
  }

  GLXContext glxContext = glXCreateContext(display, visual, 0, GL::TRUE);

  if(!glxContext)
  {
    esError << "Failed to create OpenGL context." << std::endl;

    ::exit(EXIT_FAILURE);
  }

  Colormap colorMap = XCreateColormap(display,
                                      RootWindow(display, visual->screen),
                                      visual->visual, AllocNone);

  Pixmap mask = XCreatePixmap(display, XRootWindow(display, 0), 1, 1, 1);

  XGCValues xgc;

  xgc.function = GXclear;

  GC gc = XCreateGC(display, mask, GCFunction, &xgc);

  XFillRectangle(display, mask, gc, 0, 0, 1, 1);

  XColor color;

  color.pixel = 0;
  color.red = 0;
  color.flags = 4;

  Cursor cursor = XCreatePixmapCursor(display, mask, mask, &color, &color,
                                      0, 0);

  XFreePixmap(display, mask);

  XFreeGC(display, gc);

  XSetWindowAttributes attr;

  attr.colormap = colorMap;
  attr.border_pixel = 0;
  attr.event_mask = StructureNotifyMask | KeyPressMask | KeyReleaseMask
                  | ButtonPressMask | ButtonReleaseMask | PointerMotionMask;
  attr.cursor = cursor;

  if(fullscreen.integer)
  {
    attr.override_redirect = True;

    window = XCreateWindow(display, RootWindow(display, visual->screen),
      0, 0, width, height,
      0, visual->depth, InputOutput, visual->visual,
      CWOverrideRedirect | CWCursor | CWColormap | CWEventMask, &attr);

    if(!window)
    {
      esError << "Failed to create a fullscreen X window." << std::endl;

      ::exit(EXIT_FAILURE);
    }

    XMapRaised(display, window);
    XWarpPointer(display, None, window, 0, 0, 0, 0, 0, 0);
    XGrabPointer(display, window, True, ButtonPressMask | ButtonReleaseMask | PointerMotionMask, GrabModeAsync, GrabModeAsync, window, None, CurrentTime);
    XGrabKeyboard(display, window, True, GrabModeAsync, GrabModeAsync,
                  CurrentTime);
    // XXX: Check return value of all of the above.
  }
  else // !fullscreen.integer
  {
    window = XCreateWindow(display, RootWindow(display, visual->screen),
      0, 0, width, height,
      0, visual->depth, InputOutput, visual->visual,
      CWBorderPixel | CWCursor | CWColormap | CWEventMask, &attr);

    if(!window)
    {
      esError << "Failed to create an X window." << std::endl;

      ::exit(EXIT_FAILURE);
    }

    XMapWindow(display, window);
  }

  const char* title = "pengupop";

  XStoreName(display, window, strdup(title));

  XEvent event;

  XIfEvent(display, &event, waitForMapNotify, reinterpret_cast<char*>(window));

  if(!glXMakeCurrent(display, window, glxContext))
  {
    esError << "Failed to activate GLX context." << std::endl;

    ::exit(EXIT_FAILURE);
  }
}

void System::updateScreen()
{
  glXSwapBuffers(display, window);
}

void System::idle()
{
  if(display)
    glXSwapBuffers(display, window);

  Sound::idle();
  Network::idle();

#ifdef LINUX
  idleLinux();
#endif

  if(!display)
    return;

  std::deque<int> keyEvents;

  while(XPending(display))
  {
    XEvent event;

    XNextEvent(display, &event);

    switch(event.type)
    {
    case KeyPress:

      {
        char letter;

        KeySym keySym = XLookupKeysym(&event.xkey, 0);
        XLookupString(&event.xkey, &letter, 1, 0, 0);

        if(letter)
          Input::charReceived(letter);

        int key = keyNum(keySym);

        if(!key)
          break;

        keyEvents.push_back(key);
      }

      break;

    case KeyRelease:

      {
        KeySym keySym = XLookupKeysym(&event.xkey, 0);

        int key = keyNum(keySym);

        if(!key)
          break;

        keyEvents.push_back(-key);
      }

      break;

    case MotionNotify:

      if(vars["in_mouse"].integer)
      {
        int centerX = GL::config.width / 2;
        int centerY = GL::config.height / 2;

        if(event.xmotion.x == centerX && event.xmotion.y == centerY)
          break;

        int deltaX = event.xmotion.x - centerX;
        int deltaY = event.xmotion.y - centerY;

        Input::mouseMoved(deltaX, deltaY);

        XWarpPointer(display, None, window, 0, 0, 0, 0, centerX, centerY);
      }

      break;

    case ButtonPress:

      if(vars["in_mouse"].integer)
      {
        int button = event.xbutton.button - 1;

        button = (button == 1) ? 2
               : (button == 2) ? 1
               : button;

        Input::keyPressed(Input::K_MOUSE1 + button);
      }

      break;

    case ButtonRelease:

      if(vars["in_mouse"].integer)
      {
        int button = event.xbutton.button - 1;

        button = (button == 1) ? 2
               : (button == 2) ? 1
               : button;

        Input::keyReleased(Input::K_MOUSE1 + button);
      }

      break;

    case ConfigureNotify:

      GL::config.width = event.xconfigure.width;
      GL::config.height = event.xconfigure.height;

      break;
    }
  }

  // This is the only reasonable way to skip repeat events; the alternative
  // is to disable them GLOBALLY, which will also result in keyboard repeat
  // staying disabled if the application crashes.
  for(std::deque<int>::iterator i = keyEvents.begin();
      i != keyEvents.end(); ++i)
  {
    if(*i < 0)
    {
      if((i + 1) == keyEvents.end())
      {
        Input::keyReleased(-*i);
      }
      else
      {
        if(*(i + 1) != -*i)
        {
          Input::keyReleased(-*i);
        }
        else
        {
          ++i;
        }
      }
    }
    else
    {
      Input::keyPressed(*i);
    }
  }
}

void System::exit()
{
  CVar::save();

  shutdownVideo();

  Network::shutdown();

  ::exit(EXIT_SUCCESS);
}

void System::shutdownVideo()
{
  if(!display)
    return;

  if(restore)
    XF86VidModeSwitchToMode(display, visual->screen, &oldMode);

  XF86VidModeSetGamma(display, visual->screen, &oldGamma);

  XFlush(display);
}

namespace
{
  Bool waitForMapNotify(Display*, XEvent* event, char* arg)
  {
    return (event->type == MapNotify)
        && (event->xmap.window == reinterpret_cast<Window>(arg));
  }

  int keyNum(KeySym keySym)
  {
    switch(keySym)
    {
    case XK_Tab: return Input::K_TAB;
    case XK_Escape: return Input::K_ESCAPE;
    case XK_Return: return Input::K_ENTER;
    case XK_KP_Enter: return Input::K_KP_ENTER;
    case XK_space: return Input::K_SPACE;
    case XK_BackSpace: return Input::K_BACKSPACE;
    case XK_Caps_Lock: return Input::K_CAPSLOCK;
    case XK_KP_Up: return Input::K_KP_UPARROW;
    case XK_KP_Down: return Input::K_KP_DOWNARROW;
    case XK_KP_Left: return Input::K_KP_LEFTARROW;
    case XK_KP_Right: return Input::K_KP_RIGHTARROW;
    case XK_Up: return Input::K_UPARROW;
    case XK_Down: return Input::K_DOWNARROW;
    case XK_Left: return Input::K_LEFTARROW;
    case XK_Right: return Input::K_RIGHTARROW;
    case XK_Alt_L: // XXX
    case XK_Alt_R: return Input::K_ALT;
    case XK_Control_L:
    case XK_Control_R: return Input::K_CTRL;
    case XK_Shift_L:
    case XK_Shift_R: return Input::K_SHIFT;
    case XK_KP_Insert: return Input::K_KP_INS;
    case XK_KP_Delete: return Input::K_KP_DEL;
    case XK_KP_Page_Up: return Input::K_KP_PGUP;
    case XK_KP_Page_Down: return Input::K_KP_PGDN;
    case XK_KP_Home: return Input::K_KP_HOME;
    case XK_KP_End: return Input::K_KP_END;
    case XK_Insert: return Input::K_INS;
    case XK_Delete: return Input::K_DEL;
    case XK_Page_Up: return Input::K_PGUP;
    case XK_Page_Down: return Input::K_PGDN;
    case XK_Home: return Input::K_HOME;
    case XK_End: return Input::K_END;
    case XK_F1: return Input::K_F1;
    case XK_F2: return Input::K_F2;
    case XK_F3: return Input::K_F3;
    case XK_F4: return Input::K_F4;
    case XK_F5: return Input::K_F5;
    case XK_F6: return Input::K_F6;
    case XK_F7: return Input::K_F7;
    case XK_F8: return Input::K_F8;
    case XK_F9: return Input::K_F9;
    case XK_F10: return Input::K_F10;
    case XK_F11: return Input::K_F11;
    case XK_F12: return Input::K_F12;
    case XK_F13: return Input::K_F13;
    case XK_F14: return Input::K_F14;
    case XK_F15: return Input::K_F15;
    case XK_Pause: return Input::K_PAUSE;
    case XK_KP_Multiply: return Input::K_KP_STAR;
    case XK_Pointer_Button1: return Input::K_MOUSE1;
    case XK_Pointer_Button2: return Input::K_MOUSE2;
    case XK_Pointer_Button3: return Input::K_MOUSE3;
    case XK_Pointer_Button4: return Input::K_MOUSE4;
    case XK_Pointer_Button5: return Input::K_MOUSE5;
    case XK_Pointer_Up: return Input::K_MWHEELUP;
    case XK_Pointer_Down: return Input::K_MWHEELDOWN;
    default: return toupper(keySym);
    }
  }

  void cvarChanged(const char* name)
  {
    vars[name].update();

    if(!strcmp(name, "vid_gamma"))
    {
      XF86VidModeGamma gamma;
      gamma.red = vars[name].value;
      gamma.green = vars[name].value;
      gamma.blue = vars[name].value;

      XF86VidModeSetGamma(display, visual->screen, &gamma);
    }
  }
}

// vim: ts=2 sw=2 et
