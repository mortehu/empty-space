/***************************************************************************
                             cvar.cc  -  Console variable routines
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

#include <map>

#include <espace/cvar.h>
#include <espace/file.h>
#include <espace/output.h>
#include <espace/string.h>

namespace
{
  int nextHandle = 1;

  class CVarProxy
  {
  public:

    CVar cVar;
    int  flags;
    String name;
    CVar::Callback callback;

    CVarProxy()
      : flags(0),
        callback(0)
    {
      cVar.handle = nextHandle++;
      cVar.modificationCount = 0;
      cVar.value = 0;
      cVar.integer = 0;
      cVar.string[0] = 0;
    }
  };

  std::map<String, CVarProxy> names;
  std::map<int, CVarProxy*>   handles;

  String saveLocation = String::null;
}

void CVar::update()
{
  *this = handles[handle]->cVar;
}

void CVar::commit()
{
  handles[handle]->cVar = *this;

  if(handles[handle]->callback)
    handles[handle]->callback(handles[handle]->name);
}

void CVar::setCallback(Callback callback)
{
  handles[handle]->callback = callback;
}

CVar& CVar::operator=(int _integer)
{
  integer = _integer;
  strcpy(string, String::number(integer));
  value = integer;

  ++modificationCount;

  return *this;
}

CVar& CVar::operator=(const char* _string)
{
  strcpy(string, _string);
  value = String(string).toFloat();
  integer = static_cast<int>(value);

  ++modificationCount;

  return *this;
}

CVar& CVar::operator=(float _value)
{
  value = _value;
  strcpy(string, String::number(value));
  integer = static_cast<int>(value);

  ++modificationCount;

  return *this;
}

CVar CVar::acquire(const char* name, const char* defaultValue, int flags)
{
  CVar ret;

  std::map<String, CVarProxy>::iterator i = names.find(name);

  if(i != names.end())
  {
    i->second.flags |= flags;

    ret = i->second.cVar;

    return ret;
  }
  else // i == names.end()
  {
    CVarProxy& proxy = names[name];

    proxy.cVar = defaultValue;
    proxy.flags = flags;
    proxy.name = name;

    handles[proxy.cVar.handle] = &proxy;

    ret = proxy.cVar;
  }

  return ret;
}

String CVar::getString(const char* name)
{
  std::map<String, CVarProxy>::iterator i = names.find(name);

  if(i == names.end())
    return String::null;

  return i->second.cVar.string;
}

int CVar::getInt(const char* name)
{
  std::map<String, CVarProxy>::iterator i = names.find(name);

  if(i == names.end())
    return 0;

  return i->second.cVar.integer;
}

float CVar::getFloat(const char* name)
{
  std::map<String, CVarProxy>::iterator i = names.find(name);

  if(i == names.end())
    return 0;

  return i->second.cVar.value;
}

String CVar::getVars(int mask)
{
  String ret = "";

  std::map<String, CVarProxy>::iterator i;

  for(i = names.begin(); i != names.end(); ++i)
    if(i->second.flags & mask)
      ret += String(i->first) + "\\" + i->second.cVar.string + "\\";

  return ret;
}

void CVar::setSaveLocation(const char* fileName)
{
  saveLocation = fileName;
}

void CVar::save()
{
  if(saveLocation.isNull())
    return;

  File output(saveLocation, File::Write | File::Truncate);

  if(!output.isOpen())
  {
    esWarning << "Failed to save console variables to \"" << saveLocation
              << "\"." << std::endl;

    return;
  }

  std::map<String, CVarProxy>::iterator i;

  for(i = names.begin(); i != names.end(); ++i)
  {
    if(i->second.flags & CVar::Archive)
    {
      output.put(String("seta ") + i->first + " \""
                 + i->second.cVar.string + "\"\n");
    }
  }
}

// vim: ts=2 sw=2 et
