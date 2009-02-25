/***************************************************************************
                              mds.cc  -  MDS handler
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
#include <espace/vector.h>
#include <espace/quat.h>

#include "mds.h"

class MDSData : public Model
{
public:

  void boundBox(Vector3& mins, Vector3& maxs);
  int  tag(const char* name, Vector3& origin, Vector3 axis[3], int startIndex);
  void render(int frame, float backLerp, uint customShader, uint customSkin);

protected:

  friend class MDS;

  ~MDSData();

  class Frame
  {
  public:

    Frame()
      : bones(0)
    {
    }

    ~Frame()
    {
      if(bones)
        delete [] bones;
    }

    Vector3  min;
    Vector3  max;
    Vector3  origin;
    float    radius;
    Vector3  parentOffset;

    class Bone
    {
    public:

      Quat orientation;
      Quat position;
    };

    Bone* bones;
  };

  class Bone
  {
  public:

    char  name[64];
    int   parent;
    float torsoWeight;
    float parentDistance;
    uint  flags;
  };

  class Tag
  {
  public:

    char  name[64];
    float torsoWeight;
    uint  boneIndex;
  };

  class Surface
  {
  public:

    Surface()
      : vertexCount(0),
        boneRefCount(0)
    {
    }

    ~Surface()
    {
      if(vertexCount)
      {
        delete [] vertices;
        delete [] normals;
        delete [] textureCoords;
        delete [] collapseMap;
        delete [] indexes;
      }

      if(boneRefCount)
        delete [] boneRefs;
    }

    class Vertex
    {
    public:

      class Weight
      {
      public:

        uint    bone;
        float   weight;
        Vector3 position;
      };

      Weight* weights;
      uint    weightCount;

      uint  fixedParent;
      float fixedDist;
    };

    char     name[64];
    Shader*  shader;

    int      minLod;

    Vertex*  vertices;
    Vector3* normals;
    Vector2* textureCoords;
    uint*    collapseMap;
    uint     vertexCount;
    uint*    indexes;
    uint     triangleCount;
    uint*    boneRefs;
    uint     boneRefCount;

    Vector3* lerpVertices;
    Color*   colors;
  };

  class LerpBone
  {
  public:

    Vector3   offset;
    Quat      orientation;
    Matrix4x4 inverse;
    float     frame;
  };

  float     lodScale;
  float     lodBias;

  Frame*    frames;
  uint      frameCount;

  Bone*     bones;
  LerpBone* lerpBones;
  uint      boneCount;

  uint      torsoParent;

  Surface*  surfaces;
  uint      surfaceCount;

  Tag*      tags;
  uint      tagCount;

  void      updateBone(uint bone, float frame = 0);
};

uint32_t MDS::id()
{
  return 0x4D445358; // "MDSW"
}

bool MDS::canHandle(File& file)
{
  if(file.length() < 4)
    return false;

  char magic[4];

  file.seek(0);
  file.read(magic, 4);

  return !memcmp(magic, "MDSW", 4);
}

Model* MDS::read(File& file)
{
  file.seek(4 + 4 + 64); // Magic, version and model name

  MDSData* model = new MDSData;

  model->lodScale = file.getFloat();
  model->lodBias = file.getFloat();

  model->frameCount = file.getU32();
  model->boneCount = file.getU32();

  uint frameOffset = file.getU32();
  uint boneOffset = file.getU32();

  model->torsoParent = file.getU32();

  model->surfaceCount = file.getU32();
  uint surfaceOffset = file.getU32();

  model->tagCount = file.getU32();
  uint tagOffset = file.getU32();

  file.skip(4); // File size

  if(model->frameCount)
  {
    file.seek(frameOffset);

    model->frames = new MDSData::Frame[model->frameCount];

    for(uint i = 0; i < model->frameCount; ++i)
    {
      MDSData::Frame& frame = model->frames[i];

      frame.min = file.getVector3();
      frame.max = file.getVector3();
      frame.origin = file.getVector3();
      frame.radius = file.getFloat();
      frame.parentOffset = file.getVector3();

      if(model->boneCount)
      {
        frame.bones = new MDSData::Frame::Bone[model->boneCount];

        for(uint j = 0; j < model->boneCount; ++j)
        {
          MDSData::Frame::Bone& bone = frame.bones[j];

          float pitch = file.getS16() * M_PI / 32768.0;
          float yaw = file.getS16() * M_PI / 32768.0;
          float roll = file.getS16() * M_PI / 32768.0;
          file.skip(2); // Unused space

          float offsetPitch = file.getS16() * M_PI / 32768.0;
          float offsetYaw = file.getS16() * M_PI / 32768.0;

          bone.orientation.identity();
          bone.orientation.rotate(Vector3(0, 0, 1), -yaw);
          bone.orientation.rotate(Vector3(0, 1, 0), -pitch);
          bone.orientation.rotate(Vector3(1, 0, 0), -roll);

          bone.position.identity();
          bone.position.rotate(Vector3(0, 0, 1), -offsetYaw);
          bone.position.rotate(Vector3(0, 1, 0), -offsetPitch);
        }
      }
    }
  }

  if(model->boneCount)
  {
    file.seek(boneOffset);

    model->bones = new MDSData::Bone[model->boneCount];
    model->lerpBones = new MDSData::LerpBone[model->boneCount];

    for(uint i = 0; i < model->boneCount; ++i)
    {
      MDSData::Bone& bone = model->bones[i];

      file.read(bone.name, 64);

      bone.parent = file.getS32();
      bone.torsoWeight = file.getFloat();
      bone.parentDistance = file.getFloat();
      bone.flags = file.getU32();

      model->lerpBones[i].frame = -1;
    }
  }

  if(model->surfaceCount)
  {
    model->surfaces = new MDSData::Surface[model->surfaceCount];

    for(uint i = 0; i < model->surfaceCount; ++i)
    {
      file.seek(surfaceOffset);

      MDSData::Surface& surface = model->surfaces[i];

      file.skip(4); // == 0x00000008

      file.read(surface.name, 64);

      char shaderName[64];
      file.read(shaderName, 64);

      file.skip(4); // Space reserved in file for shader handle

      if(strlen(shaderName))
      {
        surface.shader = Shader::acquire(String(shaderName).replace('\\', '/'));

        if(!surface.shader)
          esWarning << "MDS: Failed to acquire shader \"" << shaderName << "\"." << std::endl;
      }
      else
      {
        surface.shader = 0;
      }

      surface.minLod = file.getS32();

      file.getS32(); // == file.offset() - surfaceOffset

      surface.vertexCount = file.getU32();
      uint vertexOffset = file.getU32() + surfaceOffset;

      surface.triangleCount = file.getU32();
      uint triangleOffset = file.getU32() + surfaceOffset;

      uint collapseMapOffset = file.getU32() + surfaceOffset;

      surface.boneRefCount = file.getU32();
      uint boneRefOffset = file.getU32() + surfaceOffset;

      surfaceOffset += file.getU32();

      if(surface.vertexCount)
      {
        file.seek(vertexOffset);

        surface.vertices = new MDSData::Surface::Vertex[surface.vertexCount];
        surface.normals = new Vector3[surface.vertexCount];
        surface.textureCoords = new Vector2[surface.vertexCount];
        surface.lerpVertices = new Vector3[surface.vertexCount];
        surface.colors = new Color[surface.vertexCount];

        for(uint j = 0; j < surface.vertexCount; ++j)
        {
          MDSData::Surface::Vertex& vertex = surface.vertices[j];

          surface.normals[j] = file.getVector3();
          surface.textureCoords[j] = file.getVector2();

          vertex.weightCount = file.getU32();
          vertex.fixedParent = file.getU32();
          vertex.fixedDist = file.getFloat();

          if(vertex.weightCount)
            vertex.weights = new MDSData::Surface::Vertex::Weight[vertex.weightCount];

          for(uint k = 0; k < vertex.weightCount; ++k)
          {
            MDSData::Surface::Vertex::Weight& weight = vertex.weights[k];

            weight.bone = file.getU32();
            weight.weight = file.getFloat();
            weight.position = file.getVector3();
          }
        }

        file.seek(collapseMapOffset);

        surface.collapseMap = new uint[surface.vertexCount];

        for(uint j = 0; j < surface.vertexCount; ++j)
          surface.collapseMap[j] = file.getU32();
      }

      if(surface.triangleCount)
      {
        file.seek(triangleOffset);

        surface.indexes = new uint[surface.triangleCount * 3];

        for(uint j = 0; j < surface.triangleCount * 3; j += 3)
        {
          surface.indexes[j + 2] = file.getU32();
          surface.indexes[j + 1] = file.getU32();
          surface.indexes[j] = file.getU32();
        }
      }

      if(surface.boneRefCount)
      {
        file.seek(boneRefOffset);

        surface.boneRefs = new uint[surface.boneRefCount];

        for(uint j = 0; j < surface.boneRefCount; ++j)
          surface.boneRefs[j] = file.getU32();
      }
    }
  }

  if(model->tagCount)
  {
    file.seek(tagOffset);

    model->tags = new MDSData::Tag[model->tagCount];

    for(uint i = 0; i < model->tagCount; ++i)
    {
      MDSData::Tag& tag = model->tags[i];

      file.read(tag.name, 64);

      tag.torsoWeight = file.getFloat();
      tag.boneIndex = file.getU32();
    }
  }

  return model;
}

MDSData::~MDSData()
{
  if(frameCount)
    delete [] frames;

  if(boneCount)
  {
    delete [] bones;
    delete [] lerpBones;
  }

  if(tagCount)
    delete [] tags;
}

void MDSData::boundBox(Vector3& mins, Vector3& maxs)
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

int MDSData::tag(const char* name, Vector3& origin, Vector3 axis[3], int startIndex)
{
  for(uint i = startIndex; i < tagCount; ++i)
  {
    if(!strcmp(tags[i].name, name))
    {
      updateBone(tags[i].boneIndex);
      updateBone(torsoParent);

      origin = lerpBones[tags[i].boneIndex].offset * (1 - tags[i].torsoWeight)
             + lerpBones[torsoParent].offset       * tags[i].torsoWeight;

      Quat orientation = lerpBones[tags[i].boneIndex].orientation
                       .slerp(lerpBones[torsoParent].orientation,
                              tags[i].torsoWeight);

      orientation.matrix(*(new(axis) Matrix3x3));

      return i;
    }
  }

  return -1;
}

void MDSData::render(int frameIndex, float backLerp,
                     uint _customShader, uint customSkin)
{
  frameIndex %= frameCount;

  if(backLerp == 1)
  {
    frameIndex = frameIndex ? (frameIndex - 1) : (frameCount - 1);
    backLerp = 0;
  }

  Skin* skin = customSkin ? Skin::skinForHandle(customSkin) : 0;

  Shader* customShader = skin          ? 0
                       : _customShader ? Shader::shaderForHandle(_customShader)
                                       : 0;

  for(uint i = 0; i < surfaceCount; ++i)
  {
    Surface& surface = surfaces[i];

    Shader* shader = skin         ? 0
                   : customShader ? customShader
                                  : surface.shader;

    if(!skin && !shader)
      continue;

    Renderer::setVertexArray(surface.lerpVertices);
    Renderer::setColorArray(surface.colors);
    Renderer::setTexCoordArray(0, surface.textureCoords);

    for(uint i = 0; i < surface.boneRefCount; ++i)
      updateBone(surface.boneRefs[i]);

    // Set all lerpVertices to (0, 0, 0)
    memset(surface.lerpVertices, 0, sizeof(Vector3) * surface.vertexCount);

    for(uint i = 0; i < surface.vertexCount; ++i)
    {
      Surface::Vertex& vertex = surface.vertices[i];

      for(uint j = 0; j < vertex.weightCount; ++j)
      {
        Surface::Vertex::Weight& weight = vertex.weights[j];

        surface.lerpVertices[i]
          += weight.weight * (weight.position * lerpBones[weight.bone].inverse);
      }
    }

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

void MDSData::updateBone(uint bone, float frame)
{
  LerpBone& lerpBone = lerpBones[bone];

  if(lerpBone.frame == frame)
    return;

  lerpBone.frame = frame;

  if(bone == 0)
  {
    lerpBone.orientation.identity();
    lerpBone.offset = Vector3(0, 0, 0);
    lerpBone.inverse.identity();

    return;
  }

  double _intFrame;
  float fraction = modf(frame, &_intFrame);

  int intFrame = static_cast<int>(_intFrame) % frameCount;

  Bone& baseBone = bones[bone];
  Frame& frame0 = frames[intFrame];
  Frame& frame1 = frames[(intFrame + 1) % frameCount];
  Frame::Bone& bone0 = frame0.bones[bone];
  Frame::Bone& bone1 = frame1.bones[bone];

  lerpBone.orientation = bone0.orientation
                              .slerp(bone1.orientation, fraction);

  lerpBone.offset = Vector3(baseBone.parentDistance, 0, 0)
                  * bone0.position.slerp(bone1.position, fraction).matrix();

  if(baseBone.parent == -1)
  {
    lerpBone.offset += frame0.parentOffset * (1 - fraction)
                     + frame1.parentOffset * fraction;
  }
  else
  {
    updateBone(baseBone.parent, frame);

    lerpBone.offset += lerpBones[baseBone.parent].offset;
  }

  Matrix3x3 tmp = lerpBones[bone].orientation.matrix();

  for(uint i = 0; i < 3; ++i)
  {
    for(uint j = 0; j < 3; ++j)
      lerpBone.inverse(i, j) = tmp(j, i);

    lerpBone.inverse(3, i) = lerpBone.offset(i);
    lerpBone.inverse(i, 3) = 0;
  }

  lerpBone.inverse(3, 3) = 1;
}

// vim: ts=2 sw=2 et
