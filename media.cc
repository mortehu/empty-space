/***************************************************************************
                            media.cc  -  Media stream handler
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

#include <espace/file.h>
#include <espace/media.h>
#include <espace/output.h>
#include <espace/plugins.h>
#include <espace/string.h>

Media* Media::acquire(const char* _name)
{
  if(!_name)
    return 0;

  String name = String(_name).replace('\\', '/');

  File* file = new File(name);

  if(!file->isOpen())
  {
    delete file;

    return 0;
  }

  PluginMap(Media)::iterator i;

  for(i = Plugin::media.begin(); i != Plugin::media.end(); ++i)
  {
    if(i->second->canHandle(*file))
    {
      Media* media = i->second->open(file);

      if(!media)
        continue;

      return media;
    }
  }

  delete file;

  esWarning << "Media: No plugin found for \"" << name << "\"." << std::endl;

  return 0;
}

void Media::unacquire(Media* media)
{
  delete media;
}

Media::~Media()
{
}

Image& Media::videoRead()
{
  return *static_cast<Image*>(0);
}

void Media::videoSkip(uint)
{
}

uint Media::videoFrameRate()
{
  return 0;
}

bool Media::videoEOF()
{
  return true;
}

void Media::videoReset()
{
}

uint Media::audioLength()
{
  return 0;
}

uint Media::audioRead(void*, uint)
{
  return 0;
}

uint Media::audioFormat()
{
  return 0;
}

uint Media::audioFrequency()
{
  return 0;
}

bool Media::audioEOF()
{
  return true;
}

void Media::audioReset()
{
}

// vim: ts=2 sw=2 et
