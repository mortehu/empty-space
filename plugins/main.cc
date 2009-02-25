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
#include "bsp.h"
#include "gif.h"
#include "jpeg.h"
#include "tga.h"
#include "md3.h"
#include "mdc.h"
#include "mds.h"
#include "png.h"
#include "q3shader.h"
#include "roq.h"
#include "rtcwfont.h"
#include "simpleshader.h"
#include "vorbis.h"
#include "wav.h"
#include "zip.h"

static Plugin** _plugins;

extern "C" void plugins(Plugin*** plugins, uint32_t* count)
{
  if(!_plugins)
  {
    _plugins = new Plugin*[15];
    _plugins[0] = new JPEG;
    _plugins[1] = new TGA;
    _plugins[2] = new ZIP;
    _plugins[3] = new MD3;
    _plugins[4] = new MDC;
    _plugins[5] = new MDS;
    _plugins[6] = new WAV;
    _plugins[7] = new RoQ;
    _plugins[8] = new BSP;
    _plugins[9] = new SimpleShader;
    _plugins[10] = new Q3Shader;
    _plugins[11] = new GIF;
    _plugins[12] = new Vorbis;
    _plugins[13] = new PNG;
    _plugins[14] = new RtCWFont;
  }

  *plugins = _plugins;
  *count = 15;
}

// vim: ts=2 sw=2 et
