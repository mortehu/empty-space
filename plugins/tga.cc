/***************************************************************************
                              tga.cc  -  TGA loader
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
#include <stdio.h>

#include <espace/file.h>
#include <espace/image.h>
#include <espace/output.h>

#include "tga.h"

static void putData(size_t offset, uint8_t* color, size_t bpp, size_t count, uint8_t* data)
{
  while(count--)
  {
    for(size_t i = 0; i < bpp; i++)
      *(data + offset++) = color[i];
  }
}

uint32_t TGA::id()
{
  return 0x544741; // "TGA"
}

bool TGA::canHandle(File& file)
{
  if(file.length() < 4)
    return false;

  uint32_t magic;

  file.seek(0);
  magic = file.getU32() & 0xFFF7FFFF;

  return magic == 0x00020000 || magic == 0x00030000;
}

Image* TGA::read(File& file)
{
  file.seek(0);

  bool rle;
  bool gray = false;
  uint8_t idLength = file.getU8();
  uint8_t colorMapType = file.getU8();

  if(colorMapType == 1)
  {
    esWarning << "TGA: Color maps are not supported." << std::endl;

    return 0;
  }

  uint8_t imageType = file.getU8();

  switch(imageType)
  {
  case 2:

    rle = false;

    break;

  case 3:

    gray = true;
    rle = false;

    break;

  case 10:

    rle = true;

    break;

  case 11:

    gray = true;
    rle = true;

    break;

  default:

    esWarning << "TGA: Unsupported image type \"" << static_cast<int>(imageType) << "\"." << std::endl;

    return 0;
  }

  file.skip(9); // Colormap data and X- and Y-origin

  uint width = file.getU16();
  uint height = file.getU16();

  uint pixelFormat = file.getU8();

  switch(pixelFormat)
  {
  case 8:

    // XXX: Doesn't work with paletted images
    pixelFormat = Image::Gray;

    break;

  case 24:

    pixelFormat = Image::RGB;

    break;

  case 32:

    pixelFormat = Image::RGBA;

    break;

  default:

    esWarning << "TGA: Unsupported pixel format \"" << static_cast<int>(pixelFormat) << "\"." << std::endl;

    return 0;
  }

  Image* image = new Image(width, height, pixelFormat);

  uint imageDescriptor = file.getU8();

  bool flip = !(imageDescriptor & 0x20); // Bottom-to-top pixel storage
  bool mirror = imageDescriptor & 0x10;  // Right-to-left pixel storage

  file.skip(idLength); // ID

  uint size = image->size();

  uint8_t* data = image->data();

  if(!rle)
  {
    file.read(data, size);
  }
  else // rle
  {
    uint     bpp = image->bytesPerPixel();
    size_t   offset = 0;
    uint8_t* color = new uint8_t[bpp];

    while(offset < size)
    {
      uint8_t buffer = file.getU8();

      uint8_t runLength = (buffer & 0x7F) + 1;

      if(buffer & 0x80)
      {
        file.read(color, bpp);

        putData(offset, color, bpp, runLength, data);

        offset += runLength * bpp;
      }
      else
      {
        while(runLength--)
        {
          file.read(color, bpp);

          putData(offset, color, bpp, 1, data);

          offset += bpp;
        }
      }
    }

    delete [] color;
  }

  if(flip)
  {
    uint bpp = image->bytesPerPixel();
    uint lineSize = width * bpp;
    uint8_t* tmp = new uint8_t[lineSize];

    for(uint y = 0; y < height / 2; ++y)
    {
      memcpy(tmp, &data[y * lineSize], lineSize);
      memcpy(&data[y * lineSize], &data[(height - y - 1) * lineSize], lineSize);
      memcpy(&data[(height - y - 1) * lineSize], tmp, lineSize);
    }

    delete [] tmp;

    if(image->pixelFormat() != Image::Gray)
    {
      for(uint i = 0; i < size; i += bpp)
      {
        uint tmp = data[i];
        data[i] = data[i + 2];
        data[i + 2] = tmp;
      }
    }
  }
  else // !flip -- need to swap red and blue
  {
    uint lineSize = width * image->bytesPerPixel();

    if(image->pixelFormat() == Image::RGBA) // BGRA
    {
      for(uint y = 0; y < height; ++y)
      {
        for(uint x = 0; x < width; ++x)
        {
          uint tmp = data[y * lineSize + x * 4];
          data[y * lineSize + x * 4] = data[y * lineSize + x * 4 + 2];
          data[y * lineSize + x * 4 + 2] = tmp;
        }
      }
    }
    else if(image->pixelFormat() == Image::RGB)
    {
      for(uint y = 0; y < height; ++y)
      {
        for(uint x = 0; x < width; ++x)
        {
          uint tmp = data[y * lineSize + x * 3];
          data[y * lineSize + x * 3] = data[y * lineSize + x * 4 + 2];
          data[y * lineSize + x * 3 + 2] = tmp;
        }
      }
    }
  }

  if(mirror)
  {
    uint lineSize = width * image->bytesPerPixel();

    for(uint y = 0; y < height; ++y)
    {
      for(uint i = 0; i < lineSize / 2; ++i)
      {
        uint tmp = data[y * lineSize + i];
        data[y * lineSize + i] = data[(y + 1) * lineSize - i - 1];
        data[(y + 1) * lineSize - i - 1] = tmp;
      }
    }
  }

  return image;
}

// vim: ts=2 sw=2 et
