/***************************************************************************
                            image.cc  -  Image routines
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

#include <string.h>

#include <espace/file.h>
#include <espace/image.h>
#include <espace/opengl.h>
#include <espace/predicates.h>
#include <espace/string.h>
#include <espace/output.h>
#include <espace/plugins.h>

Image* Image::acquire(const char* _fileName)
{
  String fileName(_fileName);

  File file(fileName);

  if(!file.isOpen())
  {
    String base = fileName.left(fileName.length() - 3);

    std::vector<String> files;

    File::find(StartsWith(base), files);

    for(std::vector<String>::iterator i = files.begin();
        i != files.end(); ++i)
    {
      if(*i == fileName)
        continue;

      file = File(*i);

      if(file.isOpen())
        break;
    }
  }

  if(!file.isOpen())
  {
    esWarning << "Image: Failed to open \"" << fileName << "\"." << std::endl;

    return 0;
  }

  Image* image = 0;

  for(PluginMap(Image)::iterator i = Plugin::image.begin();
      i != Plugin::image.end(); ++i)
  {
    if(i->second->canHandle(file))
    {
      image = i->second->read(file);

      if(image)
        return image;
    }
  }

  esWarning << "Image: No plugin found for \"" << fileName
            << "\"." << std::endl;

  return 0;
}

Image::Image(uint width, uint height, uint format)
  : _width(width),
    _height(height),
    _pixelFormat(format)
{
  _data = new uint8_t[size()];
}

Image::~Image()
{
  delete [] _data;
}

uint Image::bytesPerPixel() const
{
  switch(_pixelFormat)
  {
  case RGB: return 3;
  case RGBA: return 4;
  case Gray: return 1;
  case RGB16: return 6;
  case RGBA16: return 8;
  case Gray16: return 2;
  default: return 0;
  }
}

uint Image::dataType() const
{
  switch(_pixelFormat)
  {
  case RGB:
  case RGBA:
  case Gray: return GL::UNSIGNED_BYTE;
  case RGB16:
  case RGBA16:
  case Gray16: return GL::UNSIGNED_SHORT;
  default: return 0;
  }
}

uint Image::componentCount() const
{
  switch(_pixelFormat)
  {
  case Gray:
  case Gray16: return 1;
  case RGB:
  case RGB16: return 3;
  case RGBA:
  case RGBA16: return 4;
  default: return 0;
  }
}

void Image::brighten(float factor)
{
  uint bpp = bytesPerPixel();
  uint pixelCount = width() * height();
  uint8_t* c = data();
  uint32_t ffactor = static_cast<uint32_t>(factor * 0x100);

  switch(_pixelFormat)
  {
  case Gray:
  case NV12:

    for(uint i = 0; i < pixelCount; ++i, ++c)
    {
      uint32_t i = *c * ffactor;

      *c = (i <= 0xFFFF) ? (i >> 8) : 0xFF;
    }

    break;

  case Gray16:

    for(uint i = 0; i < pixelCount; ++i, ++c)
    {
      uint32_t i = *c * ffactor;

      *c = (i <= 0xFFFFFF) ? (i >> 8) : 0xFFFF;
    }

    break;

  case RGB:
  case RGBA:
  case RGB16:
  case RGBA16:

    static uint32_t scale[256];
    static uint32_t lastffactor;

    if(ffactor != lastffactor)
    {
      for(uint i = 0; i < 256; ++i)
      {
        int j = i * ffactor;

        scale[i] = (j < 0x10000) ? ffactor : (0xFFFF / i);
      }

      lastffactor = ffactor;
    }

    for(uint i = 0; i < pixelCount; ++i, c += bpp)
    {
      uint32_t f = (c[0] > c[1] && c[0] > c[2]) ? (scale[c[0]])
                 : (c[1] > c[2])                ? (scale[c[1]])
                 :                                (scale[c[2]]);

      c[0] = (c[0] * f) >> 8;
      c[1] = (c[1] * f) >> 8;
      c[2] = (c[2] * f) >> 8;
    }
  }
}

void Image::copy(const Image& image)
{
  if(image.width() != width()
  || image.height() != height())
  {
    esWarning << "Image: Resizing while copying unsupported." << std::endl;

    return;
  }

  if(image.pixelFormat() == pixelFormat())
  {
    memcpy(data(), image.data(), size());

    return;
  }

  switch(image.pixelFormat())
  {
  case Gray:

    switch(pixelFormat())
    {
    case NV12:

      memcpy(data(), image.data(), width() * height());
      memset(data() + width() * height(), 0, (height() / 2) * width());

    default:

      esWarning << "Image: Unsupported conversion." << std::endl;
    }

    break;

  case NV12:

    switch(pixelFormat())
    {
    case RGB:

      {
        uint8_t* RGB = data();
        const uint8_t* Y = image.data();
        uint width = this->width();
        uint height = this->height();
        const uint8_t* chroma = Y + width * height;

        for(uint y = 0; y < height; ++y)
        {
          const uint8_t* C = chroma + y / 2 * width;

          for(uint x = 0; x < width; x += 2, Y += 2, C += 2, RGB += 6)
          {
            int Cb = C[0] - 128;
            int Cr = C[1] - 128;

            int cRed   =  ((static_cast<int32_t>(0x10000 * 1.40200) * Cr) >> 16);
            int cGreen = -((static_cast<int32_t>(0x10000 * 0.34414) * Cb)
                         + (static_cast<int32_t>(0x10000 * 0.71414) * Cr) >> 16);
            int cBlue  =  ((static_cast<int32_t>(0x10000 * 1.77200) * Cb) >> 16);

            int r0 = Y[0] + cRed;
            int g0 = Y[0] + cGreen;
            int b0 = Y[0] + cBlue;
            int r1 = Y[1] + cRed;
            int g1 = Y[1] + cGreen;
            int b1 = Y[1] + cBlue;

            RGB[0] = (r0 >= 255) ? 255 : (r0 <= 0) ? 0 : r0;
            RGB[1] = (g0 >= 255) ? 255 : (g0 <= 0) ? 0 : g0;
            RGB[2] = (b0 >= 255) ? 255 : (b0 <= 0) ? 0 : b0;
            RGB[3] = (r1 >= 255) ? 255 : (r1 <= 0) ? 0 : r1;
            RGB[4] = (g1 >= 255) ? 255 : (g1 <= 0) ? 0 : g1;
            RGB[5] = (b1 >= 255) ? 255 : (b1 <= 0) ? 0 : b1;
          }
        }
      }

      break;

    default:

      esWarning << "Image: Unsupported conversion." << std::endl;
    }

    break;

  default:

    esWarning << "Image::Unsupported conversion." << std::endl;
  }
}

void Image::resize(uint width, uint height)
{
  uint oldWidth = _width;
  uint oldHeight = _height;
  uint8_t* oldData = _data;
  uint8_t* data = new uint8_t[width * height * bytesPerPixel()];

  uint stepX = (oldWidth * 0x10000) / width;
  uint stepY = (oldHeight * 0x10000) / height;

  uint bytesPerPixel = this->bytesPerPixel();

  // The following works poorly for downsampling

  for(uint y = 0, oldY = 0; y < height; ++y, oldY += stepY)
  {
    for(uint x = 0, oldX = 0; x < width; ++x, oldX += stepX)
    {
      for(uint n = 0; n < bytesPerPixel; ++n)
      {
        data[(y * width + x) * bytesPerPixel + n] =
          oldData[((oldY >> 16) * oldWidth + (oldX >> 16)) * bytesPerPixel + n];
      }
    }
  }

  _width = width;
  _height = height;

  delete [] oldData;

  _data = data;
}

uint Image::size() const
{
  if(_pixelFormat == NV12)
    return (_height + _height / 2) * _width;

  return bytesPerPixel() * _width * _height;
}

// vim: ts=2 sw=2 et
