/***************************************************************************
                     system_win32.cc  -  Win32 platform specific routines
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

#include <espace/cvar.h>
#include <espace/input.h>
#include <espace/network.h>
#include <espace/opengl.h>
#include <espace/output.h>
#include <espace/sound.h>
#include <espace/system.h>

#include <windows.h>
#include <iostream>
#include <imagehlp.h>
#include <stdint.h>

namespace
{
  HWND  window = 0;
  HDC   deviceContext = 0;
  HGLRC renderContext = 0;
  int   oldMouseParams[3];
  int   mouseParams[3] = { 0, 0, 1 };
  int   mouseState = 0;

  long PASCAL eventHandler(HWND window, UINT message, UINT wParam, LONG lParam);
  const char* lastError();
  int backtrace(void **array, int size);
  LONG CALLBACK exceptionHandler(EXCEPTION_POINTERS* exInfo);
  int keyNum(int charCode);
}

void System::initialize()
{
  SetUnhandledExceptionFilter(exceptionHandler);

  WNDCLASS windowClass;

  memset(&windowClass, 0, sizeof(windowClass));
  windowClass.lpfnWndProc = eventHandler;
  windowClass.hInstance = GetModuleHandle(0);
  windowClass.hCursor = LoadCursor(0, IDC_ARROW);
  windowClass.hbrBackground = (HBRUSH) GetStockObject(BLACK_BRUSH);
  windowClass.lpszClassName = "RenderWindowClass";
  windowClass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;

  if(!RegisterClass(&windowClass))
  {
    esError << "Failed to register window class." << std::endl;

    exit();
  }

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

  window = CreateWindow("RenderWindowClass", "RenderWindow",
                        WS_POPUP | WS_CLIPSIBLINGS | WS_CLIPCHILDREN, 0, 0,
                        width, height, 0, 0, GetModuleHandle(0), 0);

  if(!window)
  {
    esError << "Failed to create window." << std::endl;

    exit();
  }

  ShowWindow(window, SW_SHOWDEFAULT);
  UpdateWindow(window);

  deviceContext = GetDC(window);

  if(!deviceContext)
  {
    esError << "Failed to create device context." << std::endl;

    exit();
  }

  CVar fullscreen = CVar::acquire("r_fullscreen", "1", CVar::Archive);

  if(fullscreen.integer)
  {
    DEVMODE deviceMode;

    memset(&deviceMode, 0, sizeof(deviceMode));
    deviceMode.dmSize = sizeof(deviceMode);
    deviceMode.dmFields = DM_PELSWIDTH | DM_PELSHEIGHT;
    deviceMode.dmPelsWidth = GL::config.width;
    deviceMode.dmPelsHeight = GL::config.height;

    if(DISP_CHANGE_SUCCESSFUL != ChangeDisplaySettings(&deviceMode, CDS_FULLSCREEN))
      esWarning << "Change to fullscreen failed." << std::endl;
  }
  else
  {
    esInfo << "Running in windowed mode." << std::endl;
  }

  PIXELFORMATDESCRIPTOR pixelFormat;

  memset(&pixelFormat, 0, sizeof(pixelFormat));
  pixelFormat.nSize = sizeof(pixelFormat);
  pixelFormat.nVersion = 1;
  pixelFormat.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL
                      | PFD_DOUBLEBUFFER;
  pixelFormat.iPixelType = PFD_TYPE_RGBA;
  pixelFormat.cColorBits = 24;
  pixelFormat.cDepthBits = 24;
  pixelFormat.cAlphaBits = 0;
  pixelFormat.cStencilBits = 8;
  pixelFormat.iLayerType = PFD_MAIN_PLANE;

  int pixelFormatIndex = ChoosePixelFormat(deviceContext, &pixelFormat);

  if(!pixelFormatIndex)
  {
    esError << "Failed to choose pixel format: " << lastError() << std::endl;

    exit();
  }

  if(!SetPixelFormat(deviceContext, pixelFormatIndex, &pixelFormat))
  {
    esError << "Failed to set pixel format: " << lastError() << std::endl;

    exit();
  }

  renderContext = wglCreateContext(deviceContext);

  if(!renderContext)
  {
    esError << "Failed to create OpenGL render context: " << lastError()
            << std::endl;

    exit();
  }

  if(!wglMakeCurrent(deviceContext, renderContext))
  {
    esError << "Failed to make OpenGL render context current: " << lastError()
            << std::endl;

    exit();
  }

  SystemParametersInfo(SPI_GETMOUSE, 0, oldMouseParams, 0);
  SystemParametersInfo(SPI_SETMOUSE, 0, mouseParams, 0);

  SetCapture(window);
  SetCursor(0);
}

void System::updateScreen()
{
  SwapBuffers(deviceContext);
}

void System::idle()
{
  SwapBuffers(deviceContext);

  Sound::idle();
  Network::idle();

  // XXX: if(mouse)
  {
    int centerX = GL::config.width / 2;
    int centerY = GL::config.height / 2;

    POINT cursorPos;

    GetCursorPos(&cursorPos);

    int deltaX = cursorPos.x - centerX;
    int deltaY = cursorPos.y - centerY;

    if(deltaX != 0 || deltaY != 0)
    {
      Input::mouseMoved(deltaX, deltaY);

      SetCursorPos(centerX, centerY);
    }
  }

  MSG message;

  while(PeekMessage(&message, 0, 0, 0, PM_REMOVE))
  {
    if(message.message == WM_QUIT)
      ::exit(EXIT_SUCCESS);

    TranslateMessage(&message);
    DispatchMessage(&message);
  }
}

double System::time()
{
  static LARGE_INTEGER frequency;
  static LARGE_INTEGER start;

  if(!frequency.QuadPart)
  {
    if(!QueryPerformanceFrequency(&frequency))
    {
      frequency.QuadPart = 1000;
      start.QuadPart = GetTickCount();
    }
    else
    {
      QueryPerformanceCounter(&start);
    }
  }

  if(frequency.QuadPart == 1000)
    return (GetTickCount() - start.QuadPart) / 1000.0;

  LARGE_INTEGER time;

  QueryPerformanceCounter(&time);

  return static_cast<double>(time.QuadPart - start.QuadPart) / frequency.QuadPart;
}

uint System::memoryRemaining()
{
  MEMORYSTATUS memoryStatus;

  GlobalMemoryStatus(&memoryStatus);

  return memoryStatus.dwAvailPhys;
}

void* System::dlopen(const char* _fileName)
{
  char* fileName = strdup(_fileName);
  char* c;

  while(0 != (c = strchr(fileName, '/')))
    *c = '\\';

  void* ret = reinterpret_cast<void*>(LoadLibrary(fileName));

  esDebug(3) << "System::dlopen(\"" << fileName << "\") = " << ret << std::endl;

  free(fileName);

  return ret;
}

void* System::dlsym(void* handle, const char* symbol)
{
  return (void*) GetProcAddress(reinterpret_cast<HMODULE>(handle), symbol);
}

const char* System::dlerror()
{
  return lastError();
}

void System::dlclose(void* handle)
{
  esDebug(3) << "System::dlclose(" << handle << ")" << std::endl;

  FreeLibrary(reinterpret_cast<HMODULE>(handle));
}

void System::exit()
{
  CVar::save();

  Network::shutdown();

  SystemParametersInfo(SPI_SETMOUSE, 0, oldMouseParams, 0);

  wglDeleteContext(renderContext);

  DeleteDC(deviceContext);

  PostMessage(window, WM_CLOSE, 0, 0);

  // This will eventually terminate due to the WM_CLOSE message
  for(;;)
    System::idle();
}

namespace
{
  LONG CALLBACK exceptionHandler(EXCEPTION_POINTERS* exInfo)
  {
    std::cerr << "Fatal exception.  Stack backtrace follows." << std::endl;

    PIMAGEHLP_SYMBOL symbol;

    symbol = reinterpret_cast<PIMAGEHLP_SYMBOL>(GlobalAlloc(GMEM_FIXED, 16384));
    symbol->SizeOfStruct = 16384;
    symbol->MaxNameLength = 16384 - sizeof(PIMAGEHLP_SYMBOL);

    STACKFRAME frame;

    memset(&frame, 0, sizeof(frame));
    frame.AddrPC.Offset = exInfo->ContextRecord->Eip;
    frame.AddrStack.Offset = exInfo->ContextRecord->Esp;
    frame.AddrFrame.Offset = exInfo->ContextRecord->Ebp;
    frame.AddrPC.Mode = AddrModeFlat;
    frame.AddrStack.Mode = AddrModeFlat;
    frame.AddrFrame.Mode = AddrModeFlat;

    SymInitialize(GetCurrentProcess(), 0, TRUE);

    int i = 0;

    DWORD last = 0;

    while(StackWalk(IMAGE_FILE_MACHINE_I386, GetCurrentProcess(),
                    GetCurrentThread(), &frame, 0, 0,
                    SymFunctionTableAccess, SymGetModuleBase, 0))
    {
      if(!frame.AddrFrame.Offset)
        break;

      if(frame.AddrPC.Offset == last && i > 16)
      {
        std::cerr << "Corrupt stack." << std::endl;

        break;
      }

      last = frame.AddrPC.Offset;

      std::cerr << " " << i << ". "
                << reinterpret_cast<void*>(frame.AddrPC.Offset);

      DWORD disp;

      if(SymGetSymFromAddr(GetCurrentProcess(), frame.AddrPC.Offset, &disp,
                           symbol))
      {
        std::cerr << " " << symbol->Name << " + " << disp;
      }

      std::cerr << std::endl;

      ++i;
    }

    SymCleanup(GetCurrentProcess());

    GlobalFree(symbol);

    ::exit(EXIT_FAILURE);
  }

  long PASCAL eventHandler(HWND window, UINT message, UINT wParam, LONG lParam)
  {
    switch(message)
    {
    case WM_CHAR:

      Input::charReceived(wParam);

      break;

    case WM_KEYDOWN:

      if(lParam & 0x40000000)
        break;

      {
        int scanCode = (lParam >> 16) & 0xFF;

        if(scanCode == 41)
        {
          Input::keyPressed('`');

          break;
        }

        int key = keyNum(wParam);

        if(!key)
          break;

        Input::keyPressed(key);
      }

      break;

    case WM_KEYUP:

      {
        int key = keyNum(wParam);

        if(!key)
          break;

        Input::keyReleased(key);
      }

      break;

    case WM_LBUTTONDOWN:
    case WM_RBUTTONDOWN:
    case WM_MBUTTONDOWN:
    case WM_LBUTTONUP:
    case WM_RBUTTONUP:
    case WM_MBUTTONUP:
    case WM_MOUSEMOVE:

      {
        // The resolution of WM_MOUSEMOVE is not high enough to use for
        // controlling.  We poll in idle() instead.

        if((mouseState ^ wParam) & MK_LBUTTON)
        {
          if(wParam & MK_LBUTTON)
            Input::keyPressed(Input::K_MOUSE1);
          else
            Input::keyReleased(Input::K_MOUSE1);
        }

        if((mouseState ^ wParam) & MK_RBUTTON)
        {
          if(wParam & MK_RBUTTON)
            Input::keyPressed(Input::K_MOUSE2);
          else
            Input::keyReleased(Input::K_MOUSE2);
        }

        if((mouseState ^ wParam) & MK_MBUTTON)
        {
          if(wParam & MK_MBUTTON)
            Input::keyPressed(Input::K_MOUSE3);
          else
            Input::keyReleased(Input::K_MOUSE3);
        }

        mouseState = wParam;
      }

      break;

    case WM_CREATE:

      break;

    case WM_CLOSE:

      DestroyWindow(window);

      break;

    case WM_DESTROY:

      PostQuitMessage(0);

      break;

    default:

      return DefWindowProc(window, message, wParam, lParam);
    }

    return 0;
  }

  char errorBuffer[256];

  const char* lastError()
  {
    DWORD messageId = GetLastError();

    FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, 0, messageId, 0, errorBuffer, 256, 0);

    while(isspace(errorBuffer[strlen(errorBuffer) - 1]))
      errorBuffer[strlen(errorBuffer) - 1] = 0;

    return errorBuffer;
  }

  int keyNum(int charCode)
  {
    switch(charCode)
    {
    case VK_TAB: return Input::K_TAB;
    case VK_RETURN: return Input::K_ENTER;
    case VK_ESCAPE: return Input::K_ESCAPE;
    case VK_SPACE: return Input::K_SPACE;
    case VK_BACK: return Input::K_BACKSPACE;
    case VK_CAPITAL: return Input::K_CAPSLOCK;
    case VK_PAUSE: return Input::K_PAUSE;
    case VK_UP: return Input::K_UPARROW;
    case VK_DOWN: return Input::K_DOWNARROW;
    case VK_LEFT: return Input::K_LEFTARROW;
    case VK_RIGHT: return Input::K_RIGHTARROW;
    case VK_MENU: return Input::K_ALT;
    case VK_CONTROL: return Input::K_CTRL;
    case VK_SHIFT: return Input::K_SHIFT;
    case VK_INSERT: return Input::K_INS;
    case VK_DELETE: return Input::K_DEL;
    case VK_NEXT: return Input::K_PGDN;
    case VK_PRIOR: return Input::K_PGUP;
    case VK_END: return Input::K_END;
    case VK_F1: return Input::K_F1;
    case VK_F2: return Input::K_F2;
    case VK_F3: return Input::K_F3;
    case VK_F4: return Input::K_F4;
    case VK_F5: return Input::K_F5;
    case VK_F6: return Input::K_F6;
    case VK_F7: return Input::K_F7;
    case VK_F8: return Input::K_F8;
    case VK_F9: return Input::K_F9;
    case VK_F10: return Input::K_F10;
    case VK_F11: return Input::K_F11;
    case VK_F12: return Input::K_F12;
    case VK_F13: return Input::K_F13;
    case VK_F14: return Input::K_F14;
    case VK_F15: return Input::K_F15;
    case VK_NUMPAD7: return Input::K_KP_HOME;
    case VK_NUMPAD8: return Input::K_KP_UPARROW;
    case VK_NUMPAD9: return Input::K_KP_PGUP;
    case VK_NUMPAD4: return Input::K_KP_LEFTARROW;
    case VK_NUMPAD5: return Input::K_KP_5;
    case VK_NUMPAD6: return Input::K_KP_RIGHTARROW;
    case VK_NUMPAD1: return Input::K_KP_END;
    case VK_NUMPAD2: return Input::K_KP_DOWNARROW;
    case VK_NUMPAD3: return Input::K_KP_PGDN;
    case VK_NUMPAD0: return Input::K_KP_INS;
    case VK_DECIMAL: return Input::K_KP_DEL;
    case VK_DIVIDE: return Input::K_KP_SLASH;
    case VK_SUBTRACT: return Input::K_KP_MINUS;
    case VK_ADD: return Input::K_KP_PLUS;
    case VK_MULTIPLY: return Input::K_KP_STAR;
    default: return toupper(charCode);
    }
  }
}

// vim: ts=2 sw=2 et
