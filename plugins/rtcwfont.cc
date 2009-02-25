/***************************************************************************
                         rtcwfont.cc  -  RtCW font handler
                               -------------------
      copyright            : (C) 2004 by Morten Hustveit
      email                : morten@rashbox.org
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <espace/file.h>
#include <espace/font.h>
#include <espace/shader.h>

#include "rtcwfont.h"

uint32_t RtCWFont::id()
{
  return 0x52744357;
}

bool RtCWFont::canHandle(File& file)
{
  return file.length() == 20548;
}

Font* RtCWFont::read(File& file)
{
  Font* font = new Font;

  for(uint i = 0; i < 256; ++i)
  {
    Font::Glyph& glyph = font->glyphs[i];

    glyph.height = file.getU32();
    glyph.top = file.getU32();
    glyph.bottom = file.getU32();
    glyph.pitch = file.getU32();
    glyph.xSkip = file.getU32();
    glyph.imageWidth = file.getU32();
    glyph.imageHeight = file.getU32();
    glyph.s1 = file.getFloat();
    glyph.t1 = file.getFloat();
    glyph.s2 = file.getFloat();
    glyph.t2 = file.getFloat();
    glyph.shaderHandle = file.getU32();
    file.read(glyph.shaderName, sizeof(glyph.shaderName));
  }

  font->glyphScale = file.getFloat();
  file.read(font->name, sizeof(font->name));

  for(uint i = 0; i < 256; ++i)
    if(strlen(font->glyphs[i].shaderName))
      font->glyphs[i].shaderHandle
        = Shader::acquireHandle(String("simple:")
                                + font->glyphs[i].shaderName);

  return font;
}

