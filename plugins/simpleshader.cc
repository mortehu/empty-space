/***************************************************************************
                     simpleshader.cc  -  Simple shader handler
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

#include <espace/color.h>
#include <espace/file.h>
#include <espace/opengl.h>
#include <espace/renderer.h>
#include <espace/shader.h>
#include <espace/texture.h>

#include "simpleshader.h"

class SimpleShaderData : public Shader
{
public:

  SimpleShaderData(uint image);

  void unacquire();

  uint passCount() const;
  void pushState(uint pass);
  void popState();

  bool isSky() const;
  uint sort() const;
  Renderer::Face cullFace() const;

protected:

  uint image;
};

uint32_t SimpleShader::id()
{
  return 0x534D504C; // 'SMPL'
}

bool SimpleShader::canHandle(File&)
{
  return false;
}

void SimpleShader::initialize()
{
}

Shader* SimpleShader::acquire(const char* _name, bool mipmaps)
{
  String name(_name);

  bool linear;

  if(name.beginsWith("simple:"))
  {
    linear = true;
  }
  else if(name.beginsWith("simple-nolinear:"))
  {
    linear = false;
  }
  else
    return 0;

  name = name.right(name.length() - name.find(":") - 1);

  uint image;

  if(name == "*white")
  {
    image = 0;
  }
  else
  {
    image = Texture::acquire(name,   (!mipmaps ? Texture::NoMipMaps : 0)
                                   | (!linear ? Texture::NoLinear : 0));

    if(!image)
      return 0;
  }

  SimpleShaderData* shader = new SimpleShaderData(image);

  return shader;
}

SimpleShaderData::SimpleShaderData(uint image)
  : image(image)
{
  refCount = 1;
}

void SimpleShaderData::unacquire()
{
  if(image)
    Texture::unacquire(image);
}

uint SimpleShaderData::passCount() const
{
  return 1;
}

void SimpleShaderData::pushState(uint /*pass*/)
{
  GL::color4ubv(st_entityColor.data());

  Renderer::setTexEnvMode(Renderer::EnvMode_Modulate);
  Renderer::setTexture(image);
  Renderer::setCullFace(Renderer::Face_Back);
  Renderer::setAlphaFunc(Renderer::Alpha_All);
  Renderer::setBlendFunc(Renderer::Factor_SrcAlpha,
                         Renderer::Factor_OneMinusSrcAlpha);
  Renderer::setPolygonOffset(false);
  Renderer::setDepthMask(true);
  Renderer::setTexCoords(Renderer::Source_Array0);
  Renderer::setColors(Renderer::Source_Constant);
}

void SimpleShaderData::popState()
{
}

bool SimpleShaderData::isSky() const
{
  return false;
}

uint SimpleShaderData::sort() const
{
  return 0;
}

Renderer::Face SimpleShaderData::cullFace() const
{
  return Renderer::Face_Back;
}

// vim: ts=2 sw=2 et
