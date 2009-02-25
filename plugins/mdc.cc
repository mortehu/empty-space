/***************************************************************************
                              mdc.cc  -  MDC handler
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

#include <espace/file.h>
#include <espace/model.h>
#include <espace/output.h>
#include <espace/renderer.h>
#include <espace/shader.h>
#include <espace/skin.h>

#include "mdc.h"

class MDCData : public Model
{
public:

  void boundBox(Vector3& mins, Vector3& maxs);
  int  tag(const char* name, Vector3& origin, Vector3 axis[3], int startIndex);
  void render(int frame, float backLerp, uint customShader, uint customSkin);

protected:

  friend class MDC;

  ~MDCData();

  class Frame
  {
  public:

    Vector3 min;
    Vector3 max;
    Vector3 origin;
    float   radius;
    char    name[16];
  };

  class Tag
  {
  public:

    Vector3 origin;
    Vector3 angle;
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

      Vector3*  vertices;
      Vector3*  normals;
    };

    Frame*    frames;
    Vector3*  lerpVertices;
  };

  typedef char TagName[64];

  Frame*   frames;
  uint     frameCount;
  TagName* tagNames;
  Tag*     tags;
  uint     tagCount;
  Surface* surfaces;
  uint     surfaceCount;
  uint     skinCount;
};

uint32_t MDC::id()
{
  return 0x49445043; // "IDPC"
}

bool MDC::canHandle(File& file)
{
  if(file.length() < 4)
    return false;

  char magic[4];

  file.seek(0);
  file.read(magic, 4);

  return !memcmp(magic, "IDPC", 4);
}

Model* MDC::read(File& file)
{
  file.seek(4 + 4 + 64); // Magic, version and model name

  MDCData* model = new MDCData;

  file.skip(4); // Flags

  model->frameCount = file.getU32();
  model->tagCount = file.getU32();
  model->surfaceCount = file.getU32();
  model->skinCount = file.getU32();

  uint frameOffset = file.getU32();
  uint tagNameOffset = file.getU32();
  uint tagFrameOffset = file.getU32();
  uint surfaceOffset = file.getU32();
  file.skip(4); // File size

  if(model->frameCount)
  {
    file.seek(frameOffset);

    model->frames = new MDCData::Frame[model->frameCount];

    for(uint i = 0 ; i < model->frameCount; ++i)
    {
      MDCData::Frame& frame = model->frames[i];

      frame.min = file.getVector3();
      frame.max = file.getVector3();
      frame.origin = file.getVector3();
      frame.radius = file.getFloat();

      file.read(frame.name, 16);
    }
  }

  if(model->tagCount)
  {
    file.seek(tagNameOffset);

    model->tagNames = new MDCData::TagName[model->tagCount];

    for(uint i = 0; i < model->tagCount; ++i)
      file.read(model->tagNames[i], 64);

    file.seek(tagFrameOffset);

    model->tags = new MDCData::Tag[model->tagCount];

    for(uint i = 0; i < model->tagCount; ++i)
    {
      MDCData::Tag& tag = model->tags[i];

      tag.origin(0) = file.getS16() / 64.0;
      tag.origin(1) = file.getS16() / 64.0;
      tag.origin(2) = file.getS16() / 64.0;

      tag.angle(0) = file.getS16() * 360.0 / 32767.0;
      tag.angle(1) = file.getS16() * 360.0 / 32767.0;
      tag.angle(2) = file.getS16() * 360.0 / 32767.0;
    }
  }

  if(model->surfaceCount)
  {
    model->surfaces = new MDCData::Surface[model->surfaceCount];

    for(uint i = 0; i < model->surfaceCount; ++i)
    {
      MDCData::Surface& surface = model->surfaces[i];

      file.seek(surfaceOffset);

      file.getU32(); // Skip ID

      file.read(surface.name, 64);

      file.getU32(); // Skip flags

      uint compFrameCount = file.getU32();
      uint baseFrameCount = file.getU32();
      surface.frameCount = compFrameCount + baseFrameCount;
      uint shaderCount = file.getU32();
      surface.vertexCount = file.getU32();
      surface.triangleCount = file.getU32();

      uint triangleOffset = surfaceOffset + file.getU32();
      uint shaderOffset = surfaceOffset + file.getU32();
      uint texCoordOffset = surfaceOffset + file.getU32();
      uint baseVertexOffset = surfaceOffset + file.getU32();
      uint compVertexOffset = surfaceOffset + file.getU32();
      uint baseFrameOffset = surfaceOffset + file.getU32();
      uint compFrameOffset = surfaceOffset + file.getU32();

      surfaceOffset += file.getU32();

      file.seek(triangleOffset);

      surface.indexes = new uint[surface.triangleCount * 3];

      for(uint i = 0; i < surface.triangleCount * 3; i += 3)
      {
        surface.indexes[i + 2] = file.getU32();
        surface.indexes[i + 1] = file.getU32();
        surface.indexes[i] = file.getU32();
      }

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

      file.seek(texCoordOffset);

      surface.textureCoords = new Vector2[surface.vertexCount];

      for(uint i = 0; i < surface.vertexCount; ++i)
        surface.textureCoords[i] = file.getVector2();

      surface.lerpVertices = new Vector3[surface.vertexCount];

      file.seek(baseVertexOffset);

      Vector3* baseVertices = new Vector3[surface.vertexCount * baseFrameCount];
      Vector3* baseNormals = new Vector3[surface.vertexCount * baseFrameCount];

      for(uint i = 0, j = 0; i < baseFrameCount; ++i)
      {
        for(uint k = 0; k < surface.vertexCount; ++k, ++j)
        {
          baseVertices[j](0) = file.getS16() / 64.0;
          baseVertices[j](1) = file.getS16() / 64.0;
          baseVertices[j](2) = file.getS16() / 64.0;

          float lng = file.getU8() * 2 * M_PI / 255.0;
          float lat = file.getU8() * 2 * M_PI / 255.0;

          baseNormals[j](0) = cos(lat) * sin(lng);
          baseNormals[j](1) = sin(lat) * sin(lng);
          baseNormals[j](2) = cos(lng);
        }
      }

      Vector3* compVertices = 0;
      Vector3* compNormals = 0;

      if(compFrameCount)
      {
        file.seek(compVertexOffset);

        compVertices = new Vector3[surface.vertexCount * compFrameCount];
        compNormals = new Vector3[surface.vertexCount * compFrameCount];

        for(uint i = 0, j = 0; i < compFrameCount; ++i)
        {
          for(uint k = 0; k < surface.vertexCount; ++k, ++j)
          {
            compVertices[j](0) = (static_cast<int>(file.getU8()) - 127) * 3.0 / 64.0;
            compVertices[j](1) = (static_cast<int>(file.getU8()) - 127) * 3.0 / 64.0;
            compVertices[j](2) = (static_cast<int>(file.getU8()) - 127) * 3.0 / 64.0;

            file.getU8(); // XXX: Don't know how to process delta normal

            compNormals[j] = Vector3(0, 0, 0);
          }
        }
      }

      surface.frames = new MDCData::Surface::Frame[model->frameCount];

      file.seek(baseFrameOffset);

      for(uint i = 0; i < model->frameCount; ++i)
      {
        MDCData::Surface::Frame& frame = surface.frames[i];

        frame.vertices = new Vector3[surface.vertexCount];
        frame.normals = new Vector3[surface.vertexCount];

        uint frameIndex = file.getU16();

        if(frameIndex >= baseFrameCount)
          continue;

        for(uint j = 0; j < surface.vertexCount; ++j)
        {
          frame.vertices[j] = baseVertices[frameIndex * surface.vertexCount + j];
          frame.normals[j] = baseNormals[frameIndex * surface.vertexCount + j];
        }
      }

      file.seek(compFrameOffset);

      for(uint i = 0; i < model->frameCount; ++i)
      {
        MDCData::Surface::Frame& frame = surface.frames[i];

        uint frameIndex = file.getU16();

        if(frameIndex >= compFrameCount)
          continue;

        for(uint j = 0; j < surface.vertexCount; ++j)
        {
          frame.vertices[j] += compVertices[frameIndex * surface.vertexCount + j];
          frame.normals[j] += compNormals[frameIndex * surface.vertexCount + j];
          frame.normals[j].normalize();
        }
      }
    }
  }

  return model;
}

MDCData::~MDCData()
{
  if(frameCount)
    delete [] frames;

  if(tagCount)
  {
    delete [] tagNames;
    delete [] tags;
  }

  if(surfaceCount)
    delete [] surfaces;
}

void MDCData::boundBox(Vector3& mins, Vector3& maxs)
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

int MDCData::tag(const char* name, Vector3& origin, Vector3 axis[3], int startIndex)
{
  for(uint i = startIndex; i < tagCount; ++i)
  {
    if(!strcmp(tagNames[i], name))
    {
      origin = tags[i].origin;

      // XXX: Derive from tags[i].axis
      axis[0] = Vector3(1, 0, 0);
      axis[1] = Vector3(0, 1, 0);
      axis[2] = Vector3(0, 0, 1);

      return i;
    }
  }

  return -1;
}

void MDCData::render(int frameIndex, float backLerp,
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

  for(uint j = 0; j < surfaceCount; ++j)
  {
    const Surface& surface = surfaces[j];
    const Surface::Frame& frame = surface.frames[frameIndex];

    Shader* shader = skin                ? 0
                   : customShader        ? customShader
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

    Renderer::setNormalArray(frame.normals);
    Renderer::setTexCoordArray(0, surface.textureCoords);

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
