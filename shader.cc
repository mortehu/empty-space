/***************************************************************************
                           shader.cc  -  Shader routines
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
#include <math.h>

#include <espace/color.h>
#include <espace/file.h>
#include <espace/opengl.h>
#include <espace/output.h>
#include <espace/plugins.h>
#include <espace/renderer.h>
#include <espace/shader.h>
#include <espace/string.h>

namespace
{
  std::map<int, Shader*>   handles;
  std::map<String, int>    reverseHandles;
  std::map<String, String> remapping;
  int                      nextHandle = 1;
}

bool              Shader::st_tcMod = false;
Color             Shader::st_entityColor(255, 255, 255);

float             Shader::time = 0;

Shader* Shader::acquire(const char* _name, bool mipmaps)
{
  esDebug(3) << "Shader::acquire(\"" << _name << "\", " << mipmaps << ")" << std::endl;

  String name(_name);

  std::map<String, String>::iterator j = remapping.find(name);

  if(j != remapping.end())
    name = j->second;

  std::map<String, int>::iterator i = reverseHandles.find(name);

  if(i != reverseHandles.end())
  {
    if(i->second == 0)
      return 0;

    Shader* shader = handles[i->second];

    ++shader->refCount;

    return shader;
  }

  for(std::map<uint, ShaderPlugin*>::iterator i = Plugin::shader.begin();
      i != Plugin::shader.end(); ++i)
  {
    Shader* shader = i->second->acquire(name, mipmaps);

    if(shader)
    {
      handles[nextHandle++] = shader;
      reverseHandles[name] = nextHandle - 1;

      return shader;
    }
  }

  reverseHandles[name] = 0;

  return 0;
}

int Shader::acquireHandle(const char* _name, bool mipmaps)
{
  esDebug(3) << "Shader::acquireHandle(\"" << _name << "\", " << mipmaps << ")" << std::endl;

  String name(_name);

  std::map<String, String>::iterator j = remapping.find(name);

  if(j != remapping.end())
    name = j->second;

  std::map<String, int>::iterator i = reverseHandles.find(name);

  if(i != reverseHandles.end())
  {
    if(i->second == 0)
      return 0;

    Shader* shader = handles[i->second];

    ++shader->refCount;

    return i->second;
  }

  for(std::map<uint, ShaderPlugin*>::iterator i = Plugin::shader.begin();
      i != Plugin::shader.end(); ++i)
  {
    Shader* shader = i->second->acquire(name, mipmaps);

    if(shader)
    {
      handles[nextHandle++] = shader;
      reverseHandles[name] = nextHandle - 1;

      return nextHandle - 1;
    }
  }

  reverseHandles[name] = 0;

  return 0;
}

void Shader::unacquire(Shader* shader)
{
  if(--shader->refCount)
    return;

  for(std::map<int, Shader*>::iterator i = handles.begin();
      i != handles.end(); ++i)
  {
    if(i->second == shader)
    {
      handles.erase(i);

      for(std::map<String, int>::iterator j = reverseHandles.begin();
         j != reverseHandles.end(); ++j)
      {
        if(j->second == i->first)
          reverseHandles.erase(j);
      }

      break;
    }
  }

  shader->unacquire();
}

void Shader::unacquire(int handle)
{
  Shader* shader = shaderForHandle(handle);

  unacquire(shader);
}

Shader* Shader::shaderForHandle(int handle)
{
  std::map<int, Shader*>::iterator i = handles.find(handle);

  if(i == handles.end())
    return 0;

  return i->second;
}

void Shader::remap(const char* oldShader, const char* newShader)
{
  esDebug(3) << "Shader::remap(\"" << oldShader << "\", \"" << newShader
             << "\")" << std::endl;

  remapping[oldShader] = newShader;
}

float Shader::wave(WaveForm wave, float base, float amp, float phase,
                   float freq, float time)
{
  float value = base;
  float pos = phase + freq * time;
  float subpos = fmod(pos, 1);

  switch(wave)
  {
  case Sawtooth:

    value += amp * subpos;

    break;

  case InverseSawtooth:

    value += amp * (1 - subpos);

    break;

  case Sin:

    value += amp * sin(pos * 2 * M_PI);

    break;

  case Square:

    value += (subpos < 0.5) ? amp : -amp;

    break;

  case Triangle:

    value += amp * ((subpos < 0.5) ? (2 * subpos) : (2 - 2 * subpos));

    break;
  }

  return (value > 1) ? 1
       : (value < 0) ? 0
       :               value;
}

Shader::~Shader()
{
}

// vim: ts=2 sw=2 et
