/***************************************************************************
                            model.cc  -  MD3, MDC and MDS handler
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
#include <vector>

#include <espace/color.h>
#include <espace/file.h>
#include <espace/map.h>
#include <espace/model.h>
#include <espace/output.h>
#include <espace/plugins.h>
#include <espace/predicates.h>
#include <espace/string.h>

namespace
{
  struct ModelHandle
  {
    ModelHandle()
      : refCount(1)
    {
    }

    String name;
    uint   refCount;
    Model* model;
  };

  std::map<uint, ModelHandle> handles;
  uint                        nextHandle = 1;
}

uint Model::acquire(const char* _name)
{
  if(!_name)
    return 0;

  String name = String(_name).replace('\\', '/');

  for(std::map<uint, ModelHandle>::iterator i = handles.begin(); i != handles.end(); ++i)
  {
    if(i->second.name == name)
    {
      ++i->second.refCount;

      return i->first;
    }
  }

  File file(name);

  if(!file.isOpen())
  {
    String base = name.left(name.length() - 3);

    std::vector<String> files;

    File::find(StartsWith(base), files);

    for(std::vector<String>::iterator i = files.begin();
        i != files.end(); ++i)
    {
      if(*i == name)
        continue;

      file = File(*i);

      if(file.isOpen())
        break;
    }
  }

  if(!file.isOpen())
  {
    esWarning << "Model: Failed to open \"" << name << "\"." << std::endl;

    return 0;
  }

  std::map<uint, ModelPlugin*>::iterator i;

  for(i = Plugin::model.begin(); i != Plugin::model.end(); ++i)
  {
    if(i->second->canHandle(file))
    {
      ModelHandle handle;

      handle.model = i->second->read(file);

      if(!handle.model)
        continue;

      handle.name = name;

      handles[nextHandle++] = handle;

      return nextHandle - 1;
    }
  }

  esWarning << "Model: No plugin found for \"" << name << "\"." << std::endl;

  return 0;
}

void Model::unacquire(uint handle)
{
  std::map<uint, ModelHandle>::iterator i = handles.find(handle);

  if(i == handles.end())
    return;

  if(--i->second.refCount)
    return;

  delete i->second.model;

  handles.erase(i);
}

Model* Model::modelForHandle(uint handle)
{
  std::map<uint, ModelHandle>::iterator i = handles.find(handle);

  if(i == handles.end())
    return 0;

  return i->second.model;
}

Model::~Model()
{
}

int Model::tag(const char*, Vector3&, Vector3[3], int)
{
  return -1;
}

// vim: ts=2 sw=2 et
