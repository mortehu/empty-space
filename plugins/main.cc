/***************************************************************************
                             main.cc  -  Plugin export function
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

#include <espace/plugins.h>
#include "png.h"
#include "simpleshader.h"
#include "wav.h"

#include <iostream>
static Plugin** _plugins;

extern "C" void plugins(Plugin*** plugins, uint32_t* count)
{
  if(!_plugins)
  {
    _plugins = new Plugin*[3];
    _plugins[0] = new WAV;
    _plugins[1] = new SimpleShader;
    _plugins[2] = new PNG;
  }

  *plugins = _plugins;
  *count = 3;
}

// vim: ts=2 sw=2 et
