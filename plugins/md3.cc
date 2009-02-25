/***************************************************************************
                              md3.cc  -  MD3 handler
                               -------------------
      copyright            : (C) 2003 by Morten Hustveit
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
#include <espace/model.h>
#include <espace/output.h>
#include <espace/renderer.h>
#include <espace/shader.h>
#include <espace/skin.h>

#include "md3.h"

class MD3Data : public Model
{
public:

  void boundBox(Vector3& mins, Vector3& maxs);
  int  tag(const char* name, Vector3& origin, Vector3 axis[3], int startIndex);
  void render(int frame, float backLerp, uint customShader, uint customSkin);

protected:

  friend class MD3;

  ~MD3Data();

  class Frame
  {
  public:

    char     name[16];
    Vector3  min;
    Vector3  max;
    Vector3  origin;
    float    radius;
  };

  class Tag
  {
  public:

    char    name[64];
    Vector3 origin;
    Vector3 axis[3];
  };

  class Surface
  {
  public:

    ~Surface()
    {
      delete [] textureCoords;
      delete [] indexes;
      delete [] frames;
      delete [] lerpVertices;
    }

    char      name[64];
    uint      flags;
    uint      frameCount;
    Shader*   shader;
    Vector2*  textureCoords;
    uint      vertexCount;
    uint*     indexes;
    uint      triangleCount;

    class Frame
    {
    public:

      ~Frame()
      {
        delete [] vertices;
        delete [] normals;
      }

      Vector3* vertices;
      Vector3* normals;
    };

    Frame*    frames;
    Vector3*  lerpVertices;
  };

  Frame*   frames;
  uint     frameCount;

  Tag*     tags;
  uint     tagCount;

  Surface* surfaces;
  uint     surfaceCount;
};

uint32_t MD3::id()
{
  return 0x49445033; // "IDP3"
}

bool MD3::canHandle(File& file)
{
  if(file.length() < 4)
    return false;

  char magic[4];

  file.seek(0);
  file.read(magic, 4);

  return !memcmp(magic, "IDP3", 4);
}

Model* MD3::read(File& file)
{
  file.seek(4 + 4 + 64 + 4); // Skip magic, version, model name and flags

  MD3Data* model = new MD3Data;

  model->frameCount = file.getU32();
  model->tagCount = file.getU32();
  model->surfaceCount = file.getU32();

  if(file.getU32())
    esDebug() << "MD3: Non-zero skin count" << std::endl;

  uint frameOffset = file.getU32();
  uint tagOffset = file.getU32();
  uint surfaceOffset = file.getU32();
  file.skip(4); // Skip file size

  if(model->frameCount)
  {
    file.seek(frameOffset);

    model->frames = new MD3Data::Frame[model->frameCount];

    for(uint i = 0; i < model->frameCount; ++i)
    {
      MD3Data::Frame& frame = model->frames[i];

      frame.min = file.getVector3();
      frame.max = file.getVector3();
      frame.origin = file.getVector3();
      frame.radius = file.getFloat();

      file.read(frame.name, 16);
    }
  }

  if(model->tagCount)
  {
    file.seek(tagOffset);

    for(uint j = 0; j < model->frameCount; ++j)
    {
      model->tags = new MD3Data::Tag[model->tagCount];

      for(uint i = 0; i < model->tagCount; ++i)
      {
        MD3Data::Tag& tag = model->tags[i];

        file.read(tag.name, 64);

        tag.origin = file.getVector3();
        tag.axis[0] = file.getVector3();
        tag.axis[1] = file.getVector3();
        tag.axis[2] = file.getVector3();
      }
    }
  }

  if(model->surfaceCount)
  {
    model->surfaces = new MD3Data::Surface[model->surfaceCount];

    for(uint i = 0; i < model->surfaceCount; ++i)
    {
      file.seek(surfaceOffset);

      MD3Data::Surface& surface = model->surfaces[i];

      file.getU32(); // Skip ID

      file.read(surface.name, 64);

      file.getU32(); // Skip flags

      surface.frameCount = file.getU32();
      uint shaderCount = file.getU32();
      surface.vertexCount = file.getU32();
      surface.triangleCount = file.getU32();

      surface.lerpVertices = new Vector3[surface.vertexCount];

      uint triangleOffset = surfaceOffset + file.getU32();
      uint shaderOffset = surfaceOffset + file.getU32();
      uint texCoordOffset = surfaceOffset + file.getU32();
      uint vertexOffset = surfaceOffset + file.getU32();

      surfaceOffset += file.getU32();

      file.seek(shaderOffset);

      surface.shader = 0;

      for(uint i = 0; i < shaderCount && !surface.shader; ++i)
      {
        char name[64];

        file.read(name, 64);

        String shaderName = String(name).replace('\\', '/');
        file.getU32(); // Skip useless integer

        surface.shader = Shader::acquire(shaderName);

        if(!surface.shader)
          surface.shader = Shader::acquire(String("simple:") + shaderName);
      }

      if(surface.shader)
        esWarning << "Failed to load shader for surface." << std::endl;

      file.seek(triangleOffset);

      surface.indexes = new uint[surface.triangleCount * 3];

      for(uint i = 0; i < surface.triangleCount * 3; i += 3)
      {
        surface.indexes[i + 2] = file.getU32();
        surface.indexes[i + 1] = file.getU32();
        surface.indexes[i] = file.getU32();
      }

      file.seek(texCoordOffset);

      surface.textureCoords = new Vector2[surface.vertexCount];

      for(uint i = 0; i < surface.vertexCount; ++i)
        surface.textureCoords[i] = file.getVector2();

      file.seek(vertexOffset);

      surface.frames = new MD3Data::Surface::Frame[model->frameCount];

      for(uint j = 0; j < model->frameCount; ++j)
      {
        MD3Data::Surface::Frame& frame = surface.frames[j];

        frame.vertices = new Vector3[surface.vertexCount];
        frame.normals = new Vector3[surface.vertexCount];

        for(uint i = 0; i < surface.vertexCount; ++i)
        {
          frame.vertices[i](0) = file.getS16() / 64.0;
          frame.vertices[i](1) = file.getS16() / 64.0;
          frame.vertices[i](2) = file.getS16() / 64.0;

          float lng = file.getU8() * 2 * M_PI / 255.0;
          float lat = file.getU8() * 2 * M_PI / 255.0;

          frame.normals[i](0) = cos(lat) * sin(lng);
          frame.normals[i](1) = sin(lat) * sin(lng);
          frame.normals[i](2) = cos(lng);
        }
      }
    }
  }

  return model;
}

MD3Data::~MD3Data()
{
  if(frameCount)
    delete [] frames;

  if(tagCount)
    delete [] tags;

  if(surfaceCount)
    delete [] surfaces;
}

void MD3Data::boundBox(Vector3& mins, Vector3& maxs)
{
  if(!frameCount)
  {
    maxs = mins = Vector3(0, 0, 0);

    return;
  }

  mins = frames[0].min;
  maxs = frames[0].max;

  for(uint i = 1; i < frameCount; ++i)
  {
    for(uint j = 0; j < 3; ++j)
    {
      if(frames[i].min(j) < mins(j))
        mins(j) = frames[i].min(j);

      if(frames[i].max(j) < maxs(j))
        maxs(j) = frames[i].max(j);
    }
  }
}

int MD3Data::tag(const char* name, Vector3& origin, Vector3 axis[3], int startIndex)
{
  for(uint i = startIndex; i < tagCount; ++i)
  {
    if(!strcmp(tags[i].name, name))
    {
      origin = tags[i].origin;
      axis[0] = tags[i].axis[0];
      axis[1] = tags[i].axis[1];
      axis[2] = tags[i].axis[2];

      return i;
    }
  }

  return -1;
}

void MD3Data::render(int frameIndex, float backLerp,
                     uint _customShader, uint customSkin)
{
  frameIndex %= frameCount;

  if(backLerp == 1)
  {
    frameIndex = frameIndex ? (frameIndex - 1) : (frameCount - 1);
    backLerp = 0;
  }

  Skin* skin = customSkin ? Skin::skinForHandle(customSkin)
                          : 0;

  Shader* customShader = skin          ? 0
                       : _customShader ? Shader::shaderForHandle(_customShader)
                                       : 0;

  for(uint i = 0; i < surfaceCount; ++i)
  {
    const Surface& surface = surfaces[i];
    const Surface::Frame& frame = surface.frames[frameIndex];

    Shader* shader = skin         ? 0
                   : customShader ? customShader
                                  : surface.shader;

    if(!skin && !shader)
      continue;

    if(backLerp > 0)
    {
      const Surface::Frame& lastFrame = surface.frames[frameIndex ? (frameIndex - 1) : (frameCount - 1)];

      for(uint i = 0; i < surface.vertexCount; ++i)
        surface.lerpVertices[i] = lastFrame.vertices[i] * backLerp
                                + frame.vertices[i] * (1 - backLerp);


      Renderer::setVertexArray(surface.lerpVertices);
    }
    else // backLerp == 0
    {
      Renderer::setVertexArray(frame.vertices);
    }

    Renderer::setTexCoordArray(0, surface.textureCoords);
    Renderer::setNormalArray(frame.normals);

    if(shader)
    {
      Renderer::drawTriangles(surface.triangleCount, surface.indexes,
                              shader);
    }
    else // skin
    {
      Renderer::drawTriangles(surface.triangleCount, surface.indexes,
                              skin, surface.name);
    }
  }
}

// vim: ts=2 sw=2 et
