/***************************************************************************
                        bsp_trace.cc  -  BSP trace routines
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

#include <espace/collision.h>

#include "bspdata.h"

void BSPData::rayTrace(int nodeIndex, const Vector3& start, const Vector3& dir,
                       Trace& trace) const
{
  if(nodeIndex < 0)
  {
    const Leaf& leaf = leaves[-(nodeIndex + 1)];

    for(uint i = 0; i < leaf.leafBrushCount; ++i)
    {
      uint brushIndex = leafBrushes[leaf.leafBrush + i];

      if(brushMarks[brushIndex]++)
        continue;

      const Brush& brush = brushes[brushIndex];
      const Texture& texture = textures[brush.texture];

      bool startSolid = true;
      bool endSolid = true;
      float enter = -1;
      float leave = 1;
      const Plane* clipPlane = 0;

      uint j;

      for(j = 0; j < brush.brushSideCount; ++j)
      {
        const BrushSide& brushSide = brushSides[brush.brushSide + j];
        const Plane& plane = planes[brushSide.plane];

        float dist0, dist1, mid;

        int side = Collision::intersect(start, start + dir, plane,
                                        plane.distance, dist0, dist1, mid);

        if(side == -1)
          break;

        if(side == 1)
          continue;

        if(dist0 > 0)
          startSolid = false;

        if(dist1 > 0)
          endSolid = false;

        if(dist0 > dist1) // Entering hull
        {
          if(mid > enter)
          {
            enter = mid;
            clipPlane = &plane;
          }
        }
        else // dist0 <= dist1 // Leaving hull
        {
          if(mid < leave)
            leave = mid;
        }
      }

      if(j != brush.brushSideCount)
        continue;

      if(startSolid)
      {
        trace.startSolid = true;

        if(endSolid)
          trace.allSolid = true;
      }
      else if(enter < leave)
      {
        if(enter > -1 && enter < trace.fraction)
        {
          trace.fraction = (enter > 0) ? enter : 0;
          trace.end = start + trace.fraction * dir;
          trace.surfaceFlags = texture.flags;
          trace.contents = texture.content;
          trace.entityNum = Trace::World;
          trace.plane.normal = *clipPlane;
          trace.plane.distance = clipPlane->distance;
          trace.plane.type = ((*clipPlane)(0) == 1) ? CPlane::X
                           : ((*clipPlane)(1) == 1) ? CPlane::Y
                           : ((*clipPlane)(2) == 1) ? CPlane::Z
                                                    : CPlane::NonAxial;
          trace.plane.signBits = (((*clipPlane)(0) < 0) ? 0x01 : 0)
                               | (((*clipPlane)(1) < 0) ? 0x02 : 0)
                               | (((*clipPlane)(2) < 0) ? 0x04 : 0);
        }
      }
    }
  }
  else // node >= 0
  {
    const Node& node = nodes[nodeIndex];
    const Plane& plane = planes[node.plane];

    float dist0, dist1, mid;

    int side = Collision::intersect(start, start + dir, plane, plane.distance,
                                    dist0, dist1, mid);

    if(side == -1)
    {
      rayTrace(node.children[0], start, dir, trace);
    }
    else if(side == 1)
    {
      rayTrace(node.children[1], start, dir, trace);
    }
    else if(dist0 > dist1)
    {
      rayTrace(node.children[0], start, dir, trace);

      if(mid < trace.fraction)
        rayTrace(node.children[1], start, dir, trace);
    }
    else // dist0 <= dist1
    {
      rayTrace(node.children[1], start, dir, trace);

      if(mid < trace.fraction)
        rayTrace(node.children[0], start, dir, trace);
    }
  }
}

void BSPData::sphereTrace(int nodeIndex, const Vector3& start, const Vector3& dir,
                          float radius, Trace& trace) const
{
  if(nodeIndex < 0)
  {
    const Leaf& leaf = leaves[-(nodeIndex + 1)];

    for(uint i = 0; i < leaf.leafBrushCount; ++i)
    {
      uint brushIndex = leafBrushes[leaf.leafBrush + i];

      if(brushMarks[brushIndex]++)
        continue;

      const Brush& brush = brushes[brushIndex];
      const Texture& texture = textures[brush.texture];

      bool startSolid = true;
      bool endSolid = true;
      float enter = -1;
      float leave = 1;
      const Plane* clipPlane = 0;

      uint j;

      for(j = 0; j < brush.brushSideCount; ++j)
      {
        const BrushSide& brushSide = brushSides[brush.brushSide + j];
        const Plane& plane = planes[brushSide.plane];

        float dist0 = plane * start - (plane.distance + radius);
        float dist1 = plane * (start + dir) - (plane.distance + radius);

        int side = 0;

        // If we are on the front side of any plane of a convex hull, we must
        // be outside the hull as such
        if(dist0 > 0 && dist1 > 0)
          break;

        // Another plane in this hull might clip the ray, but this doesn't
        if(dist0 <= 0 && dist1 <= 0)
          continue;

        float fraction = (dist0) / (dist0 - dist1);

        if(dist0 > 0)
          startSolid = false;

        if(dist1 > 0)
          endSolid = false;

        if(dist0 > dist1) // Entering hull
        {
          if(fraction > enter)
          {
            enter = fraction;
            clipPlane = &plane;
          }
        }
        else // dist0 <= dist1 // Leaving hull
        {
          if(fraction < leave)
            leave = fraction;
        }
      }

      // We were on the front side of a plane
      if(j != brush.brushSideCount)
        continue;

      if(startSolid)
      {
        trace.startSolid = true;

        if(endSolid)
          trace.allSolid = true;
      }
      else if(enter < leave)
      {
        if(enter > -1 && enter < trace.fraction)
        {
          trace.fraction = (enter > 0) ? enter : 0;
          trace.end = start + trace.fraction * dir;
          trace.surfaceFlags = texture.flags;
          trace.contents = texture.content;
          trace.entityNum = Trace::World;
          trace.plane.normal = *clipPlane;
          trace.plane.distance = clipPlane->distance;
          trace.plane.type = ((*clipPlane)(0) == 1) ? CPlane::X
                           : ((*clipPlane)(1) == 1) ? CPlane::Y
                           : ((*clipPlane)(2) == 1) ? CPlane::Z
                                                    : CPlane::NonAxial;
          trace.plane.signBits = (((*clipPlane)(0) < 0) ? 0x01 : 0)
                               | (((*clipPlane)(1) < 0) ? 0x02 : 0)
                               | (((*clipPlane)(2) < 0) ? 0x04 : 0);
        }
      }
    }
  }
  else // node >= 0
  {
    const Node& node = nodes[nodeIndex];
    const Plane& plane = planes[node.plane];

    float dist0 = plane * start - plane.distance;
    float dist1 = plane * (start + dir) - plane.distance;

    if(dist0 > radius && dist1 > radius)
    {
      sphereTrace(node.children[0], start, dir, radius, trace);
    }
    else if(dist0 < -radius && dist1 < -radius)
    {
      sphereTrace(node.children[1], start, dir, radius, trace);
    }
    else if(dist0 > dist1)
    {
      sphereTrace(node.children[0], start, dir, radius, trace);

      if((dist0 - radius) / (dist0 - dist1) < trace.fraction)
        sphereTrace(node.children[1], start, dir, radius, trace);
    }
    else // dist0 <= dist1
    {
      sphereTrace(node.children[1], start, dir, radius, trace);

      if((dist0 + radius) / (dist0 - dist1) < trace.fraction)
        sphereTrace(node.children[0], start, dir, radius, trace);
    }
  }
}

void BSPData::boxTrace(int nodeIndex, const Vector3& start, const Vector3& dir,
                       const Vector3& min, const Vector3& max,
                       Trace& trace, float t1, float t2) const
{
  if(t1 >= trace.fraction)
    return;

  // XXX: Is this plausible?
  if(t1 < 0)
    t1 = 0;

  if(t2 < 0)
    t2 = 0;

  if(nodeIndex < 0)
  {
    const Leaf& leaf = leaves[-(nodeIndex + 1)];

    for(uint i = 0; i < leaf.leafBrushCount; ++i)
    {
      uint brushIndex = leafBrushes[leaf.leafBrush + i];

      if(brushMarks[brushIndex]++)
        continue;

      const Brush& brush = brushes[brushIndex];
      const Texture& texture = textures[brush.texture];

      bool startSolid = true;
      bool endSolid = true;
      float enter = -1;
      float leave = 1;
      const Plane* clipPlane = 0;

      uint j;

      for(j = 0; j < brush.brushSideCount; ++j)
      {
        const BrushSide& brushSide = brushSides[brush.brushSide + j];
        const Plane& plane = planes[brushSide.plane];

        float dist0, dist1, mid0, mid1;

        int side = Collision::intersect(start, start + dir, plane,
                                        plane.distance, min, max,
                                        dist0, dist1, mid0, mid1);

        if(dist0 > 0)
          startSolid = false;

        if(dist1 > 0)
          endSolid = false;

        if(side == -1)
          break;

        if(side == 1)
          continue;

        if(dist0 > dist1) // Entering hull
        {
          if(mid0 > enter)
          {
            enter = mid0;
            clipPlane = &plane;
          }
        }
        else // dist0 <= dist1 // Leaving hull
        {
          if(mid0 < leave)
            leave = mid0;
        }
      }

      if(j != brush.brushSideCount)
        continue;

      if(startSolid)
      {
        trace.startSolid = true;

        if(endSolid)
          trace.allSolid = true;
      }
      else if(enter < leave)
      {
        if(enter > -1 && enter < trace.fraction)
        {
          trace.fraction = (enter > 0) ? enter : 0;
          trace.end = start + trace.fraction * dir;
          trace.surfaceFlags = texture.flags;
          trace.contents = texture.content;
          trace.entityNum = Trace::World;
          trace.plane.normal = *clipPlane;
          trace.plane.distance = clipPlane->distance;
          trace.plane.type = ((*clipPlane)(0) == 1) ? CPlane::X
                           : ((*clipPlane)(1) == 1) ? CPlane::Y
                           : ((*clipPlane)(2) == 1) ? CPlane::Z
                                                    : CPlane::NonAxial;
          trace.plane.signBits = (((*clipPlane)(0) < 0) ? 0x01 : 0)
                               | (((*clipPlane)(1) < 0) ? 0x02 : 0)
                               | (((*clipPlane)(2) < 0) ? 0x04 : 0);
        }
      }
    }
  }
  else // node >= 0
  {
    const Node& node = nodes[nodeIndex];
    const Plane& plane = planes[node.plane];

    float dist0, dist1, mid0, mid1;

    int side = Collision::intersect(start, start + dir, plane, plane.distance,
                                    min, max, dist0, dist1, mid0, mid1);

    if(side == -1)
    {
      boxTrace(node.children[0], start, dir, min, max, trace, t1, t2);
    }
    else if(side == 1)
    {
      boxTrace(node.children[1], start, dir, min, max, trace, t1, t2);
    }
    else if(dist0 > dist1)
    {
      boxTrace(node.children[0], start, dir, min, max, trace, t1, mid0);
      boxTrace(node.children[1], start, dir, min, max, trace, mid1, t2);
    }
    else // dist0 <= dist1
    {
      boxTrace(node.children[1], start, dir, min, max, trace, t1, mid0);
      boxTrace(node.children[0], start, dir, min, max, trace, mid1, t2);
    }
  }
}

void BSPData::capsuleTrace(int nodeIndex, const Vector3& start,
                           const Vector3& dir,
                           const Vector3& min, const Vector3& max,
                           Trace& trace, float t1, float t2) const
{
  if(t1 >= trace.fraction)
    return;

  // XXX: Is this plausible?
  if(t1 < 0)
    t1 = 0;

  if(t2 < 0)
    t2 = 0;

  if(nodeIndex < 0)
  {
    const Leaf& leaf = leaves[-(nodeIndex + 1)];

    for(uint i = 0; i < leaf.leafBrushCount; ++i)
    {
      uint brushIndex = leafBrushes[leaf.leafBrush + i];

      if(brushMarks[brushIndex]++)
        continue;

      const Brush& brush = brushes[brushIndex];
      const Texture& texture = textures[brush.texture];

      bool startSolid = true;
      bool endSolid = true;
      float enter = -1;
      float leave = 1;
      const Plane* clipPlane = 0;

      uint j;

      for(j = 0; j < brush.brushSideCount; ++j)
      {
        const BrushSide& brushSide = brushSides[brush.brushSide + j];
        const Plane& plane = planes[brushSide.plane];

        float dist0, dist1, mid0, mid1;

        int side = Collision::capsuleIntersect(start, start + dir, plane,
                                               plane.distance, min, max,
                                               dist0, dist1, mid0, mid1);

        if(dist0 > 0)
          startSolid = false;

        if(dist1 > 0)
          endSolid = false;

        if(side == -1)
          break;

        if(side == 1)
          continue;

        if(dist0 > dist1) // Entering hull
        {
          if(mid0 > enter)
          {
            enter = mid0;
            clipPlane = &plane;
          }
        }
        else // dist0 <= dist1 // Leaving hull
        {
          if(mid0 < leave)
            leave = mid0;
        }
      }

      if(j != brush.brushSideCount)
        continue;

      if(startSolid)
      {
        trace.startSolid = true;

        if(endSolid)
          trace.allSolid = true;
      }
      else if(enter < leave)
      {
        if(enter > -1 && enter < trace.fraction)
        {
          trace.fraction = (enter > 0) ? enter : 0;
          trace.end = start + trace.fraction * dir;
          trace.surfaceFlags = texture.flags;
          trace.contents = texture.content;
          trace.entityNum = Trace::World;
          trace.plane.normal = *clipPlane;
          trace.plane.distance = clipPlane->distance;
          trace.plane.type = ((*clipPlane)(0) == 1) ? CPlane::X
                           : ((*clipPlane)(1) == 1) ? CPlane::Y
                           : ((*clipPlane)(2) == 1) ? CPlane::Z
                                                    : CPlane::NonAxial;
          trace.plane.signBits = (((*clipPlane)(0) < 0) ? 0x01 : 0)
                               | (((*clipPlane)(1) < 0) ? 0x02 : 0)
                               | (((*clipPlane)(2) < 0) ? 0x04 : 0);
        }
      }
    }
  }
  else // node >= 0
  {
    const Node& node = nodes[nodeIndex];
    const Plane& plane = planes[node.plane];

    float dist0, dist1, mid0, mid1;

    int side = Collision::capsuleIntersect(start, start + dir, plane,
                                           plane.distance, min, max,
                                           dist0, dist1, mid0, mid1);

    if(side == -1)
    {
      capsuleTrace(node.children[0], start, dir, min, max, trace, t1, t2);
    }
    else if(side == 1)
    {
      capsuleTrace(node.children[1], start, dir, min, max, trace, t1, t2);
    }
    else if(dist0 > dist1)
    {
      capsuleTrace(node.children[0], start, dir, min, max, trace, t1, mid0);
      capsuleTrace(node.children[1], start, dir, min, max, trace, mid1, t2);
    }
    else // dist0 <= dist1
    {
      capsuleTrace(node.children[1], start, dir, min, max, trace, t1, mid0);
      capsuleTrace(node.children[0], start, dir, min, max, trace, mid1, t2);
    }
  }
}

void BSPData::contents(int nodeIndex, const Vector3& min, const Vector3& max,
                       Trace& trace) const
{
  if(!trace.fraction)
    return;

  if(nodeIndex < 0)
  {
    const Leaf& leaf = leaves[-(nodeIndex + 1)];

    for(uint i = 0; i < leaf.leafBrushCount; ++i)
    {
      uint brushIndex = leafBrushes[leaf.leafBrush + i];

      if(brushMarks[brushIndex]++)
        continue;

      const Brush& brush = brushes[brushIndex];
      const Texture& texture = textures[brush.texture];

      uint j;

      for(j = 0; j < brush.brushSideCount; ++j)
      {
        const BrushSide& brushSide = brushSides[brush.brushSide + j];
        const Plane& plane = planes[brushSide.plane];

        if(Collision::back(plane, plane.distance, min, max))
          break;
      }

      if(j == brush.brushSideCount)
      {
        trace.startSolid = true;
        trace.allSolid = true;
        trace.fraction = 0;
        trace.surfaceFlags = texture.flags;
        trace.contents = texture.content;
        trace.entityNum = Trace::World;
      }
    }
  }
  else // node >= 0
  {
    const Node& node = nodes[nodeIndex];
    const Plane& plane = planes[node.plane];

    if(Collision::back(plane, plane.distance, min, max))
    {
      contents(node.children[0], min, max, trace);
    }
    else if(Collision::front(plane, plane.distance, min, max))
    {
      contents(node.children[1], min, max, trace);
    }
    else
    {
      contents(node.children[0], min, max, trace);
      contents(node.children[1], min, max, trace);
    }
  }
}

void BSPData::rayTrace(const Vector3& start, const Vector3& end,
                       Trace& trace, int contentMask) const
{
  memset(&trace, 0, sizeof(trace));

  trace.fraction = 1;
  trace.end = end;
  trace.entityNum = Trace::NoEntity;

  for(uint i = 0; i < brushes.size(); ++i)
    brushMarks[i] = !(textures[brushes[i].texture].content & contentMask);

  Vector3 dir = end - start;

  rayTrace(0, start, dir, trace);
}

void BSPData::sphereTrace(const Vector3& start, const Vector3& end,
                          float radius, Trace& trace, int contentMask) const
{
  memset(&trace, 0, sizeof(trace));

  trace.fraction = 1;
  trace.end = end;
  trace.entityNum = Trace::NoEntity;

  for(uint i = 0; i < brushes.size(); ++i)
    brushMarks[i] = !(textures[brushes[i].texture].content & contentMask);

  Vector3 dir = end - start;

  sphereTrace(0, start, dir, radius, trace);
}

void BSPData::boxTrace(const Vector3& start, const Vector3& end,
                       const Vector3& min, const Vector3& max,
                       Trace& trace, int contentMask) const
{
  memset(&trace, 0, sizeof(trace));

  trace.fraction = 1;
  trace.end = end;
  trace.entityNum = Trace::NoEntity;

  for(uint i = 0; i < brushes.size(); ++i)
    brushMarks[i] = !(textures[brushes[i].texture].content & contentMask);

  Vector3 dir = end - start;

  boxTrace(0, start, dir, min, max, trace);
}

void BSPData::capsuleTrace(const Vector3& start, const Vector3& end,
                           const Vector3& min, const Vector3& max,
                           Trace& trace, int contentMask) const
{
  memset(&trace, 0, sizeof(trace));

  trace.fraction = 1;
  trace.end = end;
  trace.entityNum = Trace::NoEntity;

  for(uint i = 0; i < brushes.size(); ++i)
    brushMarks[i] = !(textures[brushes[i].texture].content & contentMask);

  Vector3 dir = end - start;

  capsuleTrace(0, start, dir, min, max, trace);
}

void BSPData::contents(const Vector3& min, const Vector3& max,
                       Trace& trace, int contentMask) const
{
  memset(&trace, 0, sizeof(trace));

  trace.entityNum = Trace::NoEntity;

  for(uint i = 0; i < brushes.size(); ++i)
    brushMarks[i] = !(textures[brushes[i].texture].content & contentMask);

  contents(0, min, max, trace);
}

// vim: ts=2 sw=2 et
