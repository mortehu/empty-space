/***************************************************************************
                             skin.cc  -  Skin routines
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

#include <espace/skin.h>
#include <espace/file.h>
#include <espace/opengl.h>
#include <espace/output.h>
#include <espace/renderer.h>
#include <espace/shader.h>
#include <espace/texture.h>

struct Skin::Internal
{
  std::map<String, Handle> textures;
};

namespace
{
  std::map<uint, Skin*> handles;
  uint                  nextHandle = 1;
}

Skin::Skin()
{
  m = new Internal;
}

Skin::~Skin()
{
  delete m;
}

uint Skin::acquire(const char* _name)
{
  String name = String(_name).replace('\\', '/');

  for(std::map<uint, Skin*>::iterator i = handles.begin(); i != handles.end(); ++i)
    if(i->second->name == name)
    {
      ++i->second->refCount;

      return i->first;
    }

  File file(name);

  if(!file.isOpen())
    return 0;

  Skin* skin = new Skin;

  skin->name = name;
  skin->refCount = 1;

  while(!file.eof())
  {
    String line = file.readLine();

    int comment = line.find("//");

    if(comment >= 0)
      line = line.left(comment);

    int comma = line.find(",");

    if(comma == -1)
      continue;

    String type = line.left(comma).stripWhiteSpace();
    String name = line.right(line.length() - comma - 1).stripWhiteSpace();

    if(type.isEmpty())
      continue;

    if(name[0] == '"' && name[name.length() - 1] == '"')
      name = name.mid(1, name.length() - 2);

    skin->m->textures[type].name = name;

    if(name.isEmpty())
    {
      skin->m->textures[type].handle = 0;
    }
    else if(!name.beginsWith("md3_"))
    {
      skin->m->textures[type].handle = Texture::acquire(name);
    }
  }

  handles[nextHandle++] = skin;

  return nextHandle - 1;
}

void Skin::unacquire(uint handle)
{
  std::map<uint, Skin*>::iterator i = handles.find(handle);

  if(i == handles.end())
    return;

  if(--i->second->refCount)
    return;

  std::map<String, Handle>::iterator j;

  for(j = i->second->m->textures.begin(); j != i->second->m->textures.end(); ++j)
  {
    if(j->second.handle)
      Texture::unacquire(j->second.handle);
  }

  delete i->second;

  handles.erase(i);
}

Skin* Skin::skinForHandle(uint handle)
{
  std::map<uint, Skin*>::iterator i = handles.find(handle);

  if(i == handles.end())
    return 0;

  return i->second;
}

void Skin::pushState(const char* surfaceName)
{
  std::map<String, Handle>::iterator i = m->textures.find(surfaceName);

  if(i == m->textures.end())
  {
    esWarning << "Can't find texture for surface \"" << surfaceName << "\"."
              << std::endl;

    return;
  }

  Renderer::setCullFace(Renderer::Face_Back);
  Renderer::setPolygonOffset(false);
  Renderer::setTexture(i->second.handle);
  Renderer::setColors(Renderer::Source_Array0);

  Renderer::setDepthMask(true);
}

const uint Skin::texture(const String& name) const
{
  std::map<String, Handle>::const_iterator i = m->textures.find(name);

  if(i == m->textures.end())
    return 0;

  return i->second.handle;
}

// vim: ts=2 sw=2 et
