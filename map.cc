/***************************************************************************
                              map.cc  -  Map plugin routines
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
#include <espace/map.h>
#include <espace/output.h>
#include <espace/plugins.h>

Map* Map::acquire(const char* _name)
{
  if(!_name)
    return 0;

  String name = String(_name).replace('\\', '/');

  File file(name);

  if(!file.isOpen())
    return 0;

  PluginMap(Map)::iterator i;

  for(i = Plugin::map.begin(); i != Plugin::map.end(); ++i)
  {
    if(i->second->canHandle(file))
      return i->second->read(file);
  }

  esWarning << "Map: No plugin found for \"" << name << "\"." << std::endl;

  return 0;
}

void Map::unacquire(Map* map)
{
  delete map;
}

Map::~Map()
{
}

String Entity::get(const String& key) const
{
  const_iterator i = find(key);

  if(i == end())
    return String::null;

  return i->second;
}

// vim: ts=2 sw=2 et
