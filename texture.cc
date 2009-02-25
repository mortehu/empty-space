/***************************************************************************
                          texture.cc  -  OpenGL texture routines
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

#include <espace/image.h>
#include <espace/opengl.h>
#include <espace/output.h>
#include <espace/renderer.h>
#include <espace/shader.h>
#include <espace/string.h>
#include <espace/texture.h>

namespace
{
  struct Handle
  {
    Handle(uint flags = 0)
      : flags(flags),
        refCount(1)
    {
    }

    uint   glHandle;
    uint   flags;
    uint   refCount;
  };

  std::map<String, Handle> handles;

  uint upload(Image* image, uint flags);
}

uint Texture::acquire(const char* _name, uint flags)
{
  esDebug(3) << "Texture::acquire(\"" << _name << "\", " << flags << ")" << std::endl;

  String name(_name);

  if(name == "$lightmap")
    return lightmap;

  std::map<String, Handle>::iterator i = handles.find(name);

  if(i != handles.end())
  {
    ++i->second.refCount;

    return i->second.glHandle;
  }

  Image* image = Image::acquire(name);

  if(!image)
    return 0;

  Handle handle(flags);

  handle.glHandle = upload(image, flags);

  handles[name] = handle;

  return handle.glHandle;
}

uint Texture::acquire(Image* image, uint flags)
{
  Handle handle(flags);

  handle.glHandle = upload(image, flags);

  handles[String::format("%p", image)] = handle;

  return handle.glHandle;
}

namespace
{
  uint upload(Image* image, uint flags)
  {
    uint width;
    uint height;

    if(!(flags & Texture::NVRect))
    {
      width = 1;
      height = 1;

      while(width < image->width() && width < GL::config.maxTextureSize)
        width <<= 1;

      while(height < image->height() && height < GL::config.maxTextureSize)
        height <<= 1;

      if(image->width() != width || image->height() != height)
        image->resize(width, height);
    }
    else
    {
      width = image->width();
      height = image->height();
    }

    uint glHandle;

    GL::genTextures(1, &glHandle);

    Renderer::setTexture(glHandle);

    if(!(flags & Texture::NoMipMaps))
    {
      if(!(flags & Texture::NoLinear))
      {
        GL::texParameteri(GL::TEXTURE_2D, GL::TEXTURE_MAG_FILTER,
                          GL::LINEAR);
        GL::texParameteri(GL::TEXTURE_2D, GL::TEXTURE_MIN_FILTER,
                          GL::LINEAR_MIPMAP_NEAREST);
      }
      else // flags & Texture::NoMipMaps
      {
        GL::texParameteri(GL::TEXTURE_2D, GL::TEXTURE_MAG_FILTER,
                          GL::NEAREST);
        GL::texParameteri(GL::TEXTURE_2D, GL::TEXTURE_MIN_FILTER,
                          GL::NEAREST_MIPMAP_NEAREST);
      }
    }
    else if(!(flags & Texture::NoLinear))
    {
      GL::texParameteri(GL::TEXTURE_2D, GL::TEXTURE_MAG_FILTER,
                        GL::LINEAR);
      GL::texParameteri(GL::TEXTURE_2D, GL::TEXTURE_MIN_FILTER,
                        GL::LINEAR);
    }
    else // (flags & Texture::NoLinear) && (flags & Texture::NoMipMaps)
    {
      GL::texParameteri(GL::TEXTURE_2D, GL::TEXTURE_MAG_FILTER,
                        GL::NEAREST);
      GL::texParameteri(GL::TEXTURE_2D, GL::TEXTURE_MIN_FILTER,
                        GL::NEAREST);
    }

    GL::texParameteri(GL::TEXTURE_2D, GL::TEXTURE_WRAP_S,
                      (flags & Texture::NoXRepeat) ? GL::CLAMP : GL::REPEAT);

    GL::texParameteri(GL::TEXTURE_2D, GL::TEXTURE_WRAP_T,
                      (flags & Texture::NoYRepeat) ? GL::CLAMP : GL::REPEAT);

    uint componentCount = image->componentCount();
    uint pixelFormat = image->pixelFormat();

    GL::texImage2D(
      (flags & Texture::NVRect) ? GL::TEXTURE_RECTANGLE_NV : GL::TEXTURE_2D,
      0,
      componentCount,
      width, height,
      0,
        (   pixelFormat == Image::RGB
         || pixelFormat == Image::RGB16)  ? GL::RGB
      : (   pixelFormat == Image::RGBA
         || pixelFormat == Image::RGBA16) ? GL::RGBA
      : (   pixelFormat == Image::Gray
         || pixelFormat == Image::Gray16) ? GL::LUMINANCE
      : GL::FALSE,
      image->dataType(),
      image->data());

    if(width > 1 || height > 1 && !(flags & Texture::NoMipMaps))
    {
      uint mipLevel = 1;

      if(image->dataType() == GL::UNSIGNED_BYTE)
      {
        uint8_t* data = new uint8_t[width * height * componentCount / 4];

        while(width > 1 || height > 1)
        {
          uint oldWidth = width;

          uint XScale = (width > 1) ? 2 : 1;
          uint YScale = (height > 1) ? 2 : 1;

          width /= XScale;
          height /= YScale;

          const uint8_t* refData = (mipLevel == 1) ? image->data() : data;

          uint nextX = (XScale - 1);
          uint nextY = (YScale - 1);

          for(uint y = 0, n = 0; y < height; ++y)
            for(uint x = 0; x < width; ++x)
              for(uint c = 0; c < componentCount; ++c, ++n)
                data[n] =
                  (refData[( y * YScale          * oldWidth +   x * XScale          ) * componentCount + c]
                 + refData[( y * YScale          * oldWidth + ((x * XScale) + nextX)) * componentCount + c]
                 + refData[((y * YScale + nextY) * oldWidth +   x * XScale          ) * componentCount + c]
                 + refData[((y * YScale + nextY) * oldWidth + ((x * XScale) + nextX)) * componentCount + c]) / 4;

          GL::texImage2D(
            GL::TEXTURE_2D,
            mipLevel,
            componentCount,
            width, height,
            0,
              (pixelFormat == Image::RGB)  ? GL::RGB
            : (pixelFormat == Image::RGBA) ? GL::RGBA
            : (pixelFormat == Image::Gray) ? GL::LUMINANCE
            : GL::FALSE,
            GL::UNSIGNED_BYTE,
            data);

          ++mipLevel;
        }

        delete [] data;
      }
      else if(image->dataType() == GL::UNSIGNED_SHORT)
      {
        uint16_t* data = new uint16_t[width * height * componentCount / 4];

        while(width > 1 || height > 1)
        {
          uint oldWidth = width;

          uint XScale = (width > 1) ? 2 : 1;
          uint YScale = (height > 1) ? 2 : 1;

          width /= XScale;
          height /= YScale;

          const uint16_t* refData = (mipLevel == 1) ? image->data16() : data;

          uint nextX = (XScale - 1);
          uint nextY = (YScale - 1);

          for(uint y = 0, n = 0; y < height; ++y)
            for(uint x = 0; x < width; ++x)
              for(uint c = 0; c < componentCount; ++c, ++n)
                data[n] =
                   (refData[( y * YScale          * oldWidth +   x * XScale          ) * componentCount + c]
                 +  refData[( y * YScale          * oldWidth + ((x * XScale) + nextX)) * componentCount + c]) / 2
                 + (refData[((y * YScale + nextY) * oldWidth +   x * XScale          ) * componentCount + c]
                 +  refData[((y * YScale + nextY) * oldWidth + ((x * XScale) + nextX)) * componentCount + c]) / 2;

          GL::texImage2D(
            GL::TEXTURE_2D,
            mipLevel,
            componentCount,
            width, height,
            0,
              (pixelFormat == Image::RGB16)  ? GL::RGB
            : (pixelFormat == Image::RGBA16) ? GL::RGBA
            : (pixelFormat == Image::Gray16) ? GL::LUMINANCE
            : GL::FALSE,
            GL::UNSIGNED_SHORT,
            data);

          ++mipLevel;
        }

        delete [] data;
      }
    }

    return glHandle;
  }
}

void Texture::unacquire(uint handle)
{
  if(handle == lightmap)
    return;

  for(std::map<String, Handle>::iterator i = handles.begin();
      i != handles.end(); ++i)
  {
    if(i->second.glHandle == handle)
    {
      if(--i->second.refCount)
        break;

      handles.erase(i);

      break;
    }
  }
}

// vim: ts=2 sw=2 et
