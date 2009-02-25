/***************************************************************************
                              gif.cc  -  Interface to giflib
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

extern "C"
{
#include <gif_lib.h>
}

#include <espace/file.h>
#include <espace/image.h>

#include "gif.h"

static int gifRead(GifFileType* gifFile, GifByteType* data, int length)
{
  File& file = *reinterpret_cast<File*>(gifFile->UserData);

  file.read(data, length);

  return length;
}

uint32_t GIF::id()
{
  return 0x47494638; // 'GIF8'
}

bool GIF::canHandle(File& file)
{
  if(file.length() < 4)
    return false;

  char magic[4];

  file.seek(0);
  file.read(magic, 4);

  return !memcmp(magic, "GIF8", 4);
}

Image* GIF::read(File& file)
{
  file.seek(0);

  GifFileType* gifFile = DGifOpen(&file, gifRead);

  if(!gifFile)
    return 0;

  int transparency = -1;

  for(;;)
  {
    GifRecordType gifType;

    DGifGetRecordType(gifFile, &gifType);

    switch(gifType)
    {
    case IMAGE_DESC_RECORD_TYPE:

      {
        DGifGetImageDesc(gifFile);

        bool alpha = transparency != -1;

        Image* image = new Image(gifFile->SWidth, gifFile->SHeight,
                                 alpha ? Image::RGBA : Image::RGB);

        uint bpp = image->bytesPerPixel();

        uint8_t* line = new uint8_t[image->width()];

        uint width = image->width();
        uint height = image->height();
        uint8_t* data = image->data();

        uint i = 0;

        for(uint y = 0; y < height; ++y)
        {
          if(GIF_ERROR == DGifGetLine(gifFile, line, width))
          {
            delete [] line;
            delete image;

            DGifCloseFile(gifFile);

            return 0;
          }

          for(uint x = 0; x < width ; ++x, i += bpp)
          {
            uint color = line[x];

            data[i] =     gifFile->SColorMap->Colors[color].Red;
            data[i + 1] = gifFile->SColorMap->Colors[color].Green;
            data[i + 2] = gifFile->SColorMap->Colors[color].Blue;

            if(alpha)
            {
              data[i + 3] = (color == transparency) ? 0 : 255;
            }
          }
        }

        delete [] line;

        DGifCloseFile(gifFile);

        return image;
      }

    case EXTENSION_RECORD_TYPE:

      int extensionCode;
      GifByteType* extension;

      if(GIF_ERROR == DGifGetExtension(gifFile, &extensionCode, &extension))
      {
        DGifCloseFile(gifFile);

        return 0;
      }

      while(extension)
      {
        if(extensionCode == 0xF9)
        {
          if(extension[1] & 1)
            transparency = extension[4];
        }
        // Handle extensions here

        if(GIF_ERROR == DGifGetExtensionNext(gifFile, &extension))
        {
          DGifCloseFile(gifFile);

          return 0;
        }
      }

      break;

    case TERMINATE_RECORD_TYPE:

      DGifCloseFile(gifFile);

      return 0;

    default:

      DGifCloseFile(gifFile);

      return 0;
    }
  }

}

// vim: ts=2 sw=2 et
