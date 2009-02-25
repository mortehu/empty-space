/***************************************************************************
                              bsp.cc  -  BSP handler
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

#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <algorithm>

#include <espace/bezier.h>
#include <espace/collision.h>
#include <espace/color.h>
#include <espace/file.h>
#include <espace/image.h>
#include <espace/output.h>
#include <espace/renderer.h>
#include <espace/shader.h>
#include <espace/string.h>
#include <espace/texture.h>

#include "bsp.h"
#include "bspdata.h"

uint32_t BSP::id()
{
  return 0x49425350; // "IBSP"
}

bool BSP::canHandle(File& file)
{
  if(file.length() < 4)
    return false;

  uint8_t magic[4];

  file.seek(0);
  file.read(magic, 4);

  return !memcmp(magic, "IBSP", 4);
}

Map* BSP::read(File& file)
{
  BSPData* map = new BSPData;

  esInfo << "BSP: Loading " << file.length() << " bytes of data.";
  esInfo.flush();

  file.seek(8);

  struct
  {
    uint32_t offset;
    uint32_t length;
  } lumps[BSPData::LumpCount];

  for(int i = 0; i < BSPData::LumpCount; ++i)
  {
    lumps[i].offset = file.getU32();
    lumps[i].length = file.getU32();
  }

  map->readLump(file, lumps[0].offset,  lumps[0].length,  BSPData::Entities);
  map->readLump(file, lumps[1].offset,  lumps[1].length,  BSPData::Textures);
  map->readLump(file, lumps[2].offset,  lumps[2].length,  BSPData::Planes);
  map->readLump(file, lumps[3].offset,  lumps[3].length,  BSPData::Nodes);
  map->readLump(file, lumps[4].offset,  lumps[4].length,  BSPData::Leaves);
  map->readLump(file, lumps[5].offset,  lumps[5].length,  BSPData::LeafFaces);
  map->readLump(file, lumps[6].offset,  lumps[6].length,  BSPData::LeafBrushes);
  map->readLump(file, lumps[7].offset,  lumps[7].length,  BSPData::Models);
  map->readLump(file, lumps[8].offset,  lumps[8].length,  BSPData::Brushes);
  map->readLump(file, lumps[9].offset,  lumps[9].length,  BSPData::BrushSides);
  map->readLump(file, lumps[10].offset, lumps[10].length, BSPData::Vertices);
  map->readLump(file, lumps[11].offset, lumps[11].length, BSPData::MeshVertices);
  map->readLump(file, lumps[12].offset, lumps[12].length, BSPData::Effects);
  map->readLump(file, lumps[13].offset, lumps[13].length, BSPData::Faces);
  map->readLump(file, lumps[14].offset, lumps[14].length, BSPData::Lightmaps);
  map->readLump(file, lumps[15].offset, lumps[15].length, BSPData::LightVolumes);
  map->readLump(file, lumps[16].offset, lumps[16].length, BSPData::VisData);

  esInfo << std::endl;

  esInfo << "BSP: Map contains "
         << map->brushes.size() << " brushes, "
         << map->faces.size() << " faces, "
         << map->vertices.size() << " vertices and "
         << map->nodes.size() << " nodes." << std::endl;

  esInfo << "BSP: Uploading " << map->lightmaps.size() << " lightmaps... " << std::endl;

  map->lightmapHandles.resize(map->lightmaps.size());

  for(uint i = 0; i < map->lightmaps.size(); ++i)
  {
    Image* lightmap = map->lightmaps[i];

    lightmap->brighten(4.0);

    map->lightmapHandles[i] = Texture::acquire(lightmap);
  }

  esInfo << "BSP: Acquiring " << map->textures.size() << " shaders..." << std::endl;

  // *** Connect textures to the correct shader info

  for(uint i = 0; i < map->textures.size(); ++i)
  {
    BSPData::Texture& texture = map->textures[i];
    int shaderHandle = Shader::acquireHandle(texture.name);

    if(shaderHandle)
    {
      texture.handle = shaderHandle;
    }
    else
    {
      texture.handle = -Texture::acquire(String(texture.name) + ".jpg");

      if(!texture.handle)
      {
        texture.handle = -Texture::acquire(String(texture.name) + ".tga");

        if(!texture.handle)
          esWarning << "BSP: Failed to acquire texture \"" << String(texture.name) << "\"." << std::endl;
      }
    }
  }

  esInfo << "BSP: Tesselating Bezier patches... " << std::endl;

  uint patchCount = 0;
  uint patchVertexOffset = map->vertices.size();
  uint patchMeshVertexOffset = map->meshVertices.size();

  for(uint i = 0; i < map->faces.size(); ++i)
  {
    BSPData::Face& face = map->faces[i];
    BSPData::RenderFace& rface = map->rfaces[i];

    if(face.type != BSPData::Face::Patch)
      continue;

    face.meshVertex = patchMeshVertexOffset;
    rface.meshVertexCount = (face.patchWidth - 1) * (face.patchHeight - 1) * 2 * 3;

    patchVertexOffset += 0;
    patchMeshVertexOffset += rface.meshVertexCount;
    ++patchCount;
  }

  if(patchCount)
  {
    map->vertices.reserve(patchVertexOffset);
    map->meshVertices.reserve(patchMeshVertexOffset);

    for(uint i = 0; i < map->faces.size(); ++i)
    {
      BSPData::Face& face = map->faces[i];

      if(face.type != BSPData::Face::Patch)
        continue;

      for(uint y = 0; y < face.patchHeight - 1; ++y)
        for(uint x = 0; x < face.patchWidth - 1; ++x)
        {
          map->meshVertices.push_back(y * face.patchWidth + x);
          map->meshVertices.push_back((y + 1) * face.patchWidth + (x + 1));
          map->meshVertices.push_back(y * face.patchWidth + (x + 1));

          map->meshVertices.push_back(y * face.patchWidth + x);
          map->meshVertices.push_back((y + 1) * face.patchWidth + x);
          map->meshVertices.push_back((y + 1) * face.patchWidth + (x + 1));
        }
    }
  }

  esInfo << "Optimizing leaf structures..." << std::endl;

  for(uint i = 0; i < map->leaves.size(); ++i)
  {
    BSPData::Leaf& leaf = map->leaves[i];
    BSPData::RenderLeaf& rleaf = map->rleaves[i];

    rleaf.faces = &map->leafFaces[leaf.leafFace];
  }

  map->rnodes.resize(map->nodes.size());

  esInfo << "Optimizing node structures..." << std::endl;

  for(uint i = 0; i < map->nodes.size(); ++i)
  {
    BSPData::Node& node = map->nodes[i];
    BSPData::RenderNode& rnode = map->rnodes[i];

    rnode.plane = map->planes[node.plane];
    rnode.flags = (rnode.plane(0) >= 0.999) ? BSPData::RenderNode::PlaneX
                : (rnode.plane(1) >= 0.999) ? BSPData::RenderNode::PlaneY
                : (rnode.plane(2) >= 0.999) ? BSPData::RenderNode::PlaneZ
                : (rnode.plane(0) <= -0.999) ? BSPData::RenderNode::PlaneMX
                : (rnode.plane(1) <= -0.999) ? BSPData::RenderNode::PlaneMY
                : (rnode.plane(2) <= -0.999) ? BSPData::RenderNode::PlaneMZ
                : 0;

    if(node.children[0] < 0)
    {
      rnode.childLeaf0 = &map->rleaves[-(node.children[0] + 1)];
      rnode.flags |= BSPData::RenderNode::ChildLeaf0;
    }
    else
    {
      rnode.child0 = &map->rnodes[node.children[0]];
    }

    if(node.children[1] < 0)
    {
      rnode.childLeaf1 = &map->rleaves[-(node.children[1] + 1)];
      rnode.flags |= BSPData::RenderNode::ChildLeaf1;
    }
    else
    {
      rnode.child1 = &map->rnodes[node.children[1]];
    }

    for(uint i = 0; i < 3; ++i)
    {
      rnode.mins[i] = node.mins[i];
      rnode.maxs[i] = node.maxs[i];
    }
  }

  esInfo << "Optimizing face structures..." << std::endl;

  for(uint i = 0; i < map->faces.size(); ++i)
  {
    BSPData::Face& face = map->faces[i];
    BSPData::RenderFace& rface = map->rfaces[i];

    rface.flags = (rface.normal(0) >= 0.999) ? BSPData::RenderNode::PlaneX
                : (rface.normal(1) >= 0.999) ? BSPData::RenderNode::PlaneY
                : (rface.normal(2) >= 0.999) ? BSPData::RenderNode::PlaneZ
                : (rface.normal(0) <= -0.999) ? BSPData::RenderNode::PlaneMX
                : (rface.normal(1) <= -0.999) ? BSPData::RenderNode::PlaneMY
                : (rface.normal(2) <= -0.999) ? BSPData::RenderNode::PlaneMZ
                : 0;

    rface.center = Vector3(0, 0, 0);

    for(uint i = 0; i < face.vertexCount; ++i)
      rface.center += map->vertices[face.vertex + i];

    rface.center /= face.vertexCount;

    rface.meshVertices = new uint[rface.meshVertexCount];

    for(uint i = 0; i < rface.meshVertexCount; ++i)
      rface.meshVertices[i] = face.vertex + map->meshVertices[face.meshVertex + i];

    rface.texture = map->textures[rface.texture].handle;

    if(rface.texture > 0)
    {
      rface.shader = Shader::shaderForHandle(rface.texture);
      rface.flags |= BSPData::RenderFace::HasShader;
    }

    if(rface.lightmap >= 0)
      rface.lightmap = map->lightmapHandles[rface.lightmap];
  }

  esInfo << "Allocating memory for markers..." << std::endl;

  map->faceMarks = new bool[map->faces.size()];
  map->brushMarks = new bool[map->brushes.size()];

  // XXX: register internal models

  return map;
}

BSPData::BSPData()
{
}

BSPData::~BSPData()
{
  // XXX: unregister internal models

  delete [] faceMarks;
  delete [] brushMarks;

  for(uint i = 0; i < textures.size(); ++i)
  {
    if(textures[i].handle < 0)
    {
      ::Texture::unacquire(-textures[i].handle);
    }
    else
    {
      Shader::unacquire(Shader::shaderForHandle(textures[i].handle));
    }
  }

  // XXX: Unacquire shaders
}

uint BSPData::entityCount() const
{
  return entities.size();
}

Entity& BSPData::entity(uint index)
{
  return entities[index];
}

uint BSPData::modelCount() const
{
  return models.size();
}

Model* BSPData::model(uint index)
{
  return &models[index];
}

uint BSPData::findLeaf(const Vector3& pos) const
{
  signed int node = 0;

  while(node >= 0)
  {
    const Plane& plane = planes[nodes[node].plane];

    node = nodes[node].children[
        (plane * pos >= plane.distance) ? 0 : 1];
  }

  return static_cast<uint>(-(node + 1));
}

void BSPData::render()
{
  Matrix4x4 projection = Renderer::projectionMatrix();
  Matrix4x4 view = Renderer::viewMatrix();

  Matrix4x4 clip = view * projection;

  frustum[0](0) =       clip(0, 3) - clip(0, 0);
  frustum[0](1) =       clip(1, 3) - clip(1, 0);
  frustum[0](2) =       clip(2, 3) - clip(2, 0);
  frustum[0].distance = clip(3, 3) - clip(3, 0);
  frustum[1](0) =       clip(0, 3) + clip(0, 0);
  frustum[1](1) =       clip(1, 3) + clip(1, 0);
  frustum[1](2) =       clip(2, 3) + clip(2, 0);
  frustum[1].distance = clip(3, 3) + clip(3, 0);
  frustum[2](0) =       clip(0, 3) + clip(0, 1);
  frustum[2](1) =       clip(1, 3) + clip(1, 1);
  frustum[2](2) =       clip(2, 3) + clip(2, 1);
  frustum[2].distance = clip(3, 3) + clip(3, 1);
  frustum[3](0) =       clip(0, 3) - clip(0, 1);
  frustum[3](1) =       clip(1, 3) - clip(1, 1);
  frustum[3](2) =       clip(2, 3) - clip(2, 1);
  frustum[3].distance = clip(3, 3) - clip(3, 1);

  float m = frustum[0].magnitude();
  frustum[0] /= m;
  frustum[0].distance /= -m;
  m = frustum[1].magnitude();
  frustum[1] /= m;
  frustum[1].distance /= -m;
  m = frustum[2].magnitude();
  frustum[2] /= m;
  frustum[2].distance /= -m;
  m = frustum[3].magnitude();
  frustum[3] /= m;
  frustum[3].distance /= -m;

  Matrix4x4 inverse;

  inverse.identity();
  for(uint i = 0; i < 3; ++i)
    for(uint j = 0; j < 3; ++j)
      inverse(i, j) = view(j, i);

  inverse = view * inverse;

  cameraPosition = -Vector3(inverse(3, 0), inverse(3, 1), inverse(3, 2));
  cameraDirection = Vector3(view(0, 2), view(1, 2), view(2, 2));
  cameraDirection.normalize();
  cameraDistance = cameraDirection * cameraPosition;

  backFace = 0;

  for(uint i = 0; i < 4; ++i)
    if(cameraDirection * frustum[i] < backFace)
      backFace = cameraDirection * frustum[i];

  memset(faceMarks, 0, faces.size() * sizeof(bool));

  currentCluster = rleaves[findLeaf(cameraPosition)].cluster;

#define OFFSET(x,n) \
  (reinterpret_cast<char*>(x)+n)

  Renderer::setVertexArray(&vertices[0], sizeof(Vertex));
  Renderer::setColorArray(OFFSET(&vertices[0], sizeof(float) * 10),
                          sizeof(Vertex));
  Renderer::setTexCoordArray(0, OFFSET(&vertices[0], sizeof(float) * 3),
                             sizeof(Vertex));
  Renderer::setTexCoordArray(1, OFFSET(&vertices[0], sizeof(float) * 5),
                             sizeof(Vertex));
  Renderer::setNormalArray(OFFSET(&vertices[0], sizeof(float) * 7),
                           sizeof(Vertex));

  addNode(rnodes[0]);

  Renderer::flush();
}

void BSPData::addLeaf(const RenderLeaf& leaf)
{
  if(currentCluster != -1 && leaf.cluster != -1)
  {
    if(!testVisibility(currentCluster, leaf.cluster))
      return;
  }

  for(uint i = 0; i < leaf.faceCount; ++i)
  {
    uint faceIndex = leaf.faces[i];

    if(!faceMarks[faceIndex]++)
    {
      const RenderFace& face = rfaces[faceIndex];

      if(face.texture <= 0)
      {
        float angle = (face.flags & 0x03) ? cameraDirection((face.flags & 3) - 1)
                    : (face.flags & 0x0C) ? -cameraDirection(((face.flags >> 2) & 3) - 1)
                    : cameraDirection * face.normal;

        if(angle < backFace)
          continue;

        Renderer::addTriangles(face.meshVertexCount / 3, face.meshVertices,
                               -face.texture, face.lightmap);
      }
      else
      {
        if(face.shader->cullFace() != Renderer::Face_None)
        {
          float angle
            = (face.flags & 0x03) ? cameraDirection((face.flags & 3) - 1)
            : (face.flags & 0x0C) ? -cameraDirection(((face.flags >> 2) & 3) - 1)
            : cameraDirection * face.normal;

          if(face.shader->cullFace() == Renderer::Face_Back)
          {
            if(angle > -backFace)
              continue;
          }
          else // face.shader->cullFace() == Renderer::Face_Front
          {
            if(angle < backFace)
              continue;
          }
        }

        if(face.shader->sort() < Shader::Underwater)
        {
          Renderer::addTriangles(face.meshVertexCount / 3, face.meshVertices,
                                 face.shader, face.lightmap);
        }
        else
        {
          int sort = static_cast<int>(-(cameraPosition - face.center).square() * 10);

          Renderer::addTriangles(face.meshVertexCount / 3, face.meshVertices,
                                 face.shader, face.lightmap, sort);
        }
      }
    }
  }
}

void BSPData::addNode(const RenderNode& node)
{
  if(Collision::back(cameraDirection, cameraDistance, node.mins, node.maxs))
    return;

  for(uint i = 0; i < 4; ++i)
    if(Collision::front(frustum[i], frustum[i].distance,
                        node.mins, node.maxs))
      return;

  float distance = (node.flags & 0x03) ? cameraPosition((node.flags & 3) - 1)
                 : (node.flags & 0x0C) ? -cameraPosition(((node.flags >> 2) & 3) - 1)
                 : cameraPosition * node.plane;

  if(distance < node.plane.distance)
  {
    if(node.flags & RenderNode::ChildLeaf1)
      addLeaf(*node.childLeaf1);
    else
      addNode(*node.child1);

    if(node.flags & RenderNode::ChildLeaf0)
      addLeaf(*node.childLeaf0);
    else
      addNode(*node.child0);
  }
  else // distance <= node.plane.distance
  {
    if(node.flags & RenderNode::ChildLeaf0)
      addLeaf(*node.childLeaf0);
    else
      addNode(*node.child0);

    if(node.flags & RenderNode::ChildLeaf1)
      addLeaf(*node.childLeaf1);
    else
      addNode(*node.child1);
  }
}

bool BSPData::visible(const Vector3& from, const Vector3& to) const
{
  int cluster0 = rleaves[findLeaf(from)].cluster;

  if(cluster0 == -1)
    return true;

  int cluster1 = rleaves[findLeaf(to)].cluster;

  if(cluster1 == -1)
    return true;

  return testVisibility(cluster0, cluster1);
}

void BSPData::InlineModel::boundBox(Vector3& min, Vector3& max)
{
  min = this->min;
  max = this->max;
}

void BSPData::InlineModel::render(int frame, float backLerp,
                                  uint customShader, uint customSkin)
{
  // XXX
}

// vim: ts=2 sw=2 et
