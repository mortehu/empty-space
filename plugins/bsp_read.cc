/***************************************************************************
                         bsp_read.cc  -  BSP reader
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

#include <ctype.h>

#include <espace/file.h>
#include <espace/image.h>
#include <espace/output.h>

#include "bspdata.h"

void BSPData::readLump(File& input, uint offset, uint length, Lump lump)
{
  esInfo << ".";

  input.seek(offset);

  switch(lump)
  {
  case Entities:

    {
      String entityData = String(length - 1);

      input.read(entityData, length - 1);

      uint offset = 0;

      for(;;)
      {
        while(offset < entityData.length() && isspace(entityData[offset]))
          ++offset;

        if(offset == entityData.length())
          break;

        if(entityData[offset] == '{')
        {
          ++offset;

          entities.push_back(Entity());

          continue;
        }

        if(entityData[offset] == '}')
        {
          ++offset;

          continue;
        }

        String key, data;

        ++offset; // Skip leading '"'

        while(entityData[offset] != '"')
          key += entityData[offset++];

        offset += 3; // Skip middle '" "'

        while(entityData[offset] != '"')
          data += entityData[offset++];

        ++offset; // Skip trailing "

        entities.back()[key] = data;
      }
    }

    break;

  case Textures:

    textures.reserve(length / 72);

    for(uint i = 0; i < length / 72; ++i)
    {
      Texture& t = *textures.insert(textures.end(), Texture());

      input.read(t.name, 64);

      // Convert \ to /
      char* c = t.name;
      while(0 != (c = strchr(c, '\\')))
        *c = '/';

      t.flags = input.getS32();
      t.content = input.getS32();
    }

    break;

  case Planes:

    planes.reserve(length / 16);

    for(uint i = 0; i < length / 16; ++i)
    {
      Plane& p = *planes.insert(planes.end(), Plane());

      p(0) = input.getFloat();
      p(1) = input.getFloat();
      p(2) = input.getFloat();
      p.distance = input.getFloat();
    }

    break;

  case Nodes:

    nodes.reserve(length / 36);

    for(uint i = 0; i < length / 36; ++i)
    {
      Node& n = *nodes.insert(nodes.end(), Node());

      n.plane = input.getS32();
      n.children[0] = input.getS32();
      n.children[1] = input.getS32();
      for(uint j = 0; j < 3; ++j)
        n.mins[j] = input.getS32();
      for(uint j = 0; j < 3; ++j)
        n.maxs[j] = input.getS32();
    }

    break;

  case Leaves:

    leaves.reserve(length / 48);
    rleaves.reserve(length / 48);

    for(uint i = 0; i < length / 48; ++i)
    {
      Leaf& l = *leaves.insert(leaves.end(), Leaf());
      RenderLeaf& rl = *rleaves.insert(rleaves.end(), RenderLeaf());

      rl.cluster = input.getS32();
      l.area = input.getS32();
      for(uint j = 0; j < 3; ++j)
        rl.mins[j] = input.getS32();
      for(uint j = 0; j < 3; ++j)
        rl.maxs[j] = input.getS32();
      l.leafFace = input.getS32();
      rl.faceCount = input.getU32();
      l.leafBrush = input.getS32();
      l.leafBrushCount = input.getS32();
    }

    break;

  case LeafFaces:

    leafFaces.resize(length / 4);

    for(uint i = 0; i < length / 4; ++i)
      leafFaces[i] = input.getU32();

    break;

  case LeafBrushes:

    leafBrushes.resize(length / 4);

    for(uint i = 0; i < length / 4; ++i)
      leafBrushes[i] = input.getS32();

    break;

  case Models:

    models.reserve(length / 40);

    for(uint i = 0; i < length / 40; ++i)
    {
      InlineModel& m = *models.insert(models.end(), InlineModel());

      m.min = input.getVector3();
      m.max = input.getVector3();
      m.face = input.getS32();
      m.faceCount = input.getS32();
      m.brush = input.getS32();
      m.brushCount = input.getS32();

      m.owner = this;
      //m.name = String("*") + i;
    }

    break;

  case Brushes:

    brushes.reserve(length / 12);

    for(uint i = 0; i < length / 12; ++i)
    {
      Brush& b = *brushes.insert(brushes.end(), Brush());

      b.brushSide = input.getS32();
      b.brushSideCount = input.getS32();
      b.texture = input.getS32();
    }

    break;

  case BrushSides:

    brushSides.reserve(length / 8);

    for(uint i = 0; i < length / 8; ++i)
    {
      BrushSide& b = *brushSides.insert(brushSides.end(), BrushSide());

      b.plane = input.getS32();
      b.texture = input.getS32();
    }

    break;

  case Vertices:

    vertices.reserve(length / 44);

    for(uint i = 0; i < length / 44; ++i)
    {
      Vertex& v = *vertices.insert(vertices.end(), Vertex());

      v(0) = input.getFloat();
      v(1) = input.getFloat();
      v(2) = input.getFloat();
      v.textureCoord = input.getVector2();
      v.lightmapCoord = input.getVector2();
      v.normal = input.getVector3();
      for(uint j = 0; j < 4; ++j)
        v.color(j) = input.getU8();
    }

    break;

  case MeshVertices:

    meshVertices.resize(length / 4);

    for(uint i = 0; i < length / 4; ++i)
      meshVertices[i] = input.getU32();

    break;

  case Effects:

    effects.reserve(length / 72);

    for(uint i = 0; i < length / 72; ++i)
    {
      Effect& e = *effects.insert(effects.end(), Effect());

      input.read(e.name, 64);
      e.brush = input.getS32();
      e.unknown = input.getS32();

      esDebug() << "Effect: \"" << e.name << "\"." << std::endl;
    }

    break;

  case Faces:

    faces.reserve(length / 104);
    rfaces.reserve(length / 104);

    for(uint i = 0; i < length / 104; ++i)
    {
      Face& f = *faces.insert(faces.end(), Face());
      RenderFace& rf = *rfaces.insert(rfaces.end(), RenderFace());

      rf.texture = input.getS32();
      f.effect = input.getS32();
      f.type = input.getU32();
      f.vertex = input.getS32();
      f.vertexCount = input.getU32();
      f.meshVertex = input.getS32();
      rf.meshVertexCount = input.getU32();
      rf.lightmap = input.getS32();
      f.lightmapStart[0] = input.getS32();
      f.lightmapStart[1] = input.getS32();
      f.lightmapSize[0] = input.getS32();
      f.lightmapSize[1] = input.getS32();
      f.origin = input.getVector3();
      f.lightmapUnits[0] = input.getVector3();
      f.lightmapUnits[1] = input.getVector3();
      rf.normal = input.getVector3();
      f.patchWidth = input.getS32();
      f.patchHeight = input.getS32();
    }

    break;

  case Lightmaps:

    lightmaps.reserve(length / (128 * 128 * 3));

    for(uint i = 0; i < length / (128 * 128 * 3); ++i)
    {
      Image* image = new Image(128, 128, Image::RGB);

      input.read(image->data(), 128 * 128 * 3);

      lightmaps.push_back(image);
    }

    break;

  case LightVolumes:

    lightVolumes.reserve(length / 8);

    for(uint i = 0; i < length / 8; ++i)
    {
      LightVolume& l = *lightVolumes.insert(lightVolumes.end(), LightVolume());

      for(uint j = 0; j < 3; ++j)
        l.ambient[j] = input.getU8();
      for(uint j = 0; j < 3; ++j)
        l.directional[j] = input.getU8();
      l.direction[0] = input.getU8();
      l.direction[1] = input.getU8();
    }

    break;

  case VisData:

    if(length)
    {
      visibility.vectorCount = input.getU32();
      visibility.vectorSize = input.getU32();
      visibility.data = new uint8_t[visibility.vectorCount * visibility.vectorSize];
      input.read(visibility.data, visibility.vectorCount * visibility.vectorSize);
    }
    else
    {
      int clusterCount = 0;

      for(uint i = 0; i < leaves.size(); ++i)
      {
        if(rleaves[i].cluster + 1 > clusterCount)
          clusterCount = rleaves[i].cluster + 1;
      }

      visibility.vectorCount = 0;
      visibility.vectorSize = 0;
      visibility.data = new uint8_t[(clusterCount + 7)/ 8];

      memset(visibility.data, 0xFF, (clusterCount + 7) / 8);
    }

    break;

  default:

    esWarning << "BSP: Unknown lump type " << static_cast<int>(lump)
              << "." << std::endl;
  }
}

// vim: ts=2 sw=2 et
