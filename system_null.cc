/***************************************************************************
                      system_null.cc  -  NULL platform specific routines
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

#include <stdlib.h>

#include <espace/system.h>

void System::initialize()
{
}

void System::idle()
{
}

double System::time()
{
  return 0;
}

uint System::memoryRemaining()
{
  return 0;
}

void System::updateScreen()
{
}

void* System::dlopen(const char* fileName)
{
  return 0;
}

void* System::dlsym(void* handle, char* symbol)
{
  return 0;
}

const char* System::dlerror()
{
  return 0;
}

void System::dlclose(void* handle)
{
}

// vim: ts=2 sw=2 et
