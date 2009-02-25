/***************************************************************************
                       system_gnu.cc  -  GNU platform specific routines
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

#include <iostream>

#include <dlfcn.h>
#include <execinfo.h>
#include <signal.h>
#include <time.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/sysinfo.h>

#include <espace/system.h>

namespace
{
  const char* signalString(int signal);
}

void System::initializeGNU()
{
  signal(SIGHUP, exitHandler);
  signal(SIGINT, exitHandler);
  signal(SIGQUIT, crashHandler);
  signal(SIGILL, crashHandler);
  signal(SIGABRT, crashHandler);
  signal(SIGFPE, crashHandler);
  signal(SIGSEGV, crashHandler);
  signal(SIGPIPE, crashHandler);
  signal(SIGALRM, crashHandler);
  signal(SIGTERM, crashHandler);
  signal(SIGUSR1, crashHandler);
  signal(SIGUSR2, crashHandler);
}

double System::time()
{
  static struct timeval start;

  if(start.tv_sec == 0 && start.tv_usec == 0)
    gettimeofday(&start, NULL);

  struct timeval now;
  gettimeofday(&now, NULL);

  return (now.tv_sec - start.tv_sec) + (now.tv_usec - start.tv_usec) / 1000000.0;
}

uint System::memoryRemaining()
{
  return get_avphys_pages() * getpagesize();
}

void* System::dlopen(const char* fileName)
{
  return ::dlopen(fileName, RTLD_NOW);
}

void* System::dlsym(void* handle, const char* symbol)
{
  return ::dlsym(handle, symbol);
}

const char* System::dlerror()
{
  return ::dlerror();
}

void System::dlclose(void* handle)
{
  ::dlclose(handle);
}

void System::crashHandler(int signal)
{
  ::signal(signal, SIG_DFL);

  // esInfo is not safe, because it might try to allocate on the heap.
  std::cerr << "Abnormal termination: " << signalString(signal) << "." << std::endl;

  const int maxStackSize = 64;
  void* stack[maxStackSize];

  int stackSize = backtrace(stack, maxStackSize);

  char** stackSymbols = backtrace_symbols(stack, stackSize);

  std::cerr << "Stack backtrace:" << std::endl
            << std::endl;

  for(int i = 0; i < stackSize; ++i)
    std::cerr << "  " << i << ". " << stackSymbols[i] << std::endl;

  shutdownVideo();

  ::exit(EXIT_FAILURE);
}

void System::exitHandler(int signal)
{
  ::signal(signal, SIG_DFL);

  // esInfo is not safe, because it might try to allocate on the heap.
  std::cerr << "Normal termination: " << signalString(signal) << "."
            << std::endl;

  exit();
}

namespace
{
  const char* signalString(int signal)
  {
    switch(signal)
    {
    case SIGHUP:  return "Hangup detected on controlling terminal";
    case SIGINT:  return "Interrupt from keyboard";
    case SIGQUIT: return "Quit from keyboard";
    case SIGILL:  return "Illegal instruction";
    case SIGABRT: return "Abort signal from abort(3)";
    case SIGFPE:  return "Floating point exception";
    case SIGSEGV: return "Invalid memory reference";
    case SIGPIPE: return "Broken pipe: write to pipe with no readers";
    case SIGALRM: return "Timer signal from alarm(2)";
    case SIGTERM: return "Termination signal";
    case SIGUSR1: return "User-defined signal 1";
    case SIGUSR2: return "User-defined signal 2";
    default: return "Invalid signal";
    }
  }
}

// vim: ts=2 sw=2 et
