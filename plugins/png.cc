/***************************************************************************
                              png.cc  -  Interface to libpng
                               -------------------
      copyright            : (C) 2003 by Carl Henrik Holth Lunde
      email                : chlunde@ifi.uio.no
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <png.h>

#include <espace/file.h>
#include <espace/image.h>
#include <espace/output.h>

#include "png.h"

#ifndef png_jmpbuf
#  define png_jmpbuf(png) ((png)->jmpbuf)
#endif

static void fileRead(png_structp png, png_bytep data, png_size_t length)
{
  File* file = reinterpret_cast<File*>(png_get_io_ptr(png));

  file->read(data, length);
}

uint32_t PNG::id()
{
  return 0x89504e47; // ".PNG"
}

bool PNG::canHandle(File& file)
{
  file.seek(0);

  png_byte magic[4];
  file.read(magic, 4);

  return !png_sig_cmp(magic, static_cast<png_size_t>(0), 4);
}

Image* PNG::read(File& file)
{
  file.seek(0);

  png_structp png = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

  if(png == NULL)
    return 0;

  png_infop info = png_create_info_struct(png);

  if(info == NULL)
  {
    png_destroy_read_struct(&png, png_infopp_NULL, png_infopp_NULL);

    return 0;
  }

  if(setjmp(png_jmpbuf(png)))
  {
    png_destroy_read_struct(&png, &info, png_infopp_NULL);

    return 0;
  }

  png_set_read_fn(png, &file, fileRead);

  png_read_info(png, info);

  png_uint_32 width, height;
  int bitDepth, pixelFormat, interlaceType;

  png_get_IHDR(png, info, &width, &height, &bitDepth, &pixelFormat,
               &interlaceType, int_p_NULL, int_p_NULL);

  if(bitDepth != 8 && bitDepth != 16)
  {
    esWarning << "PNG: Unsupported bit depth " << bitDepth << std::endl;

    png_destroy_read_struct(&png, png_infopp_NULL, png_infopp_NULL);

    return 0;
  }

  uint pixelFormat2;

  switch(pixelFormat)
  {
  case PNG_COLOR_TYPE_GRAY:

    pixelFormat2 = (bitDepth == 8) ? Image::Gray : Image::Gray16;

    break;

  case PNG_COLOR_TYPE_RGB:

    pixelFormat2 = (bitDepth == 8) ? Image::RGB : Image::RGB16;

    break;

  case PNG_COLOR_TYPE_RGB_ALPHA:

    pixelFormat2 = (bitDepth == 8) ? Image::RGBA : Image::RGBA16;

    break;

  default:

    esWarning << "PNG: Unsupported pixel format." << std::endl;

    png_destroy_read_struct(&png, png_infopp_NULL, png_infopp_NULL);

    return 0;
  }

  Image* ret = new Image(width, height, pixelFormat2);

  png_bytep* rowPointers = new png_bytep[height];

  for(uint row = 0; row < height; ++row)
    rowPointers[row] = ret->data() + row * ret->width() * ret->bytesPerPixel();

  png_read_image(png, rowPointers);

  delete [] rowPointers;

  png_read_end(png, info);

  png_destroy_read_struct(&png, &info, png_infopp_NULL);

  return ret;
}

// vim: ts=2 sw=2 et
