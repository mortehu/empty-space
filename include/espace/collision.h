#ifndef COLLISION_H_
#define COLLISION_H_

#ifndef SWIG
#include <stdint.h>

#include "vector.h"
#endif

/**
 * Collision and intersection tests relating to squares.
 *
 * \author Morten Hustveit
 */
struct CSquare
{
  CSquare(const Vector2& min, const Vector2& max)
    : min(min),
      max(max)
  {
  }

  bool operator&(const Vector2& point)
  {
    return point(0) >= min(0) && point(0) <= max(0)
        && point(1) >= min(1) && point(1) <= max(1);
  }

  Vector2 min;
  Vector2 max;
};

/**
 * 3D plane information.
 *
 * Since most planes in game maps are axis-aligned, some CPU time can be saved
 * by storing alignment information in this structure, and eliminating the need
 * for multiplications in dot product calculations.
 * \author Morten Hustveit
 */
struct CPlane
{
  enum Type
  {
    X = 0,
    Y = 1,
    Z = 2,
    NonAxial = 3
  };

  Vector3 normal;
  float   distance;
  uint8_t type;
  uint8_t signBits;
  uint8_t pad[2];
};

struct CLine
{
  CLine(const Vector3& start, const Vector3& end)
    : start(start),
      end(end)
  {
  }

  bool operator&(const CPlane& plane)
  {
    // XXX: Use optimized plane information
    float dist0 = start * plane.normal - plane.distance;
    float dist1 = end * plane.normal - plane.distance;

    return (dist0 <= 0 && dist1 >= 0) || (dist0 > 0 && dist1 <= 0);
  }

  Vector3 operator()(float t)
  {
    return start * (1 - t) + end * t;
  }

  Vector3 start;
  Vector3 end;
};

/**
 * Trace results.
 * \author Morten Hustveit
 */
struct Trace
{
  enum EntityNum
  {
    World = 1022,
    NoEntity = 1023
  };

  bool    allSolid;
  bool    startSolid;
  float   fraction;     // 1.0 = no hits
  Vector3 end;
  CPlane  plane;
  int32_t surfaceFlags; // Surface hit
  int32_t contents;     // Contents on other side of surface hit
  int32_t entityNum;    // Owner of surface hit (also ENTITYNUM_NONE/_WORLD)
};

/**
 * Namespace for generic collision detection routines.
 *
 * \todo Move all these into C... classes.
 * \author Morten Hustveit
 */
struct Collision
{
  /**
   * Intersection between ray and plane.
   */
  static inline int intersect(const Vector3& start, const Vector3& end,
                              const Vector3& normal, float distance,
                              float& dist0, float& dist1, float& mid)
  {
    dist0 = normal * start - distance;
    dist1 = normal * end - distance;

    if(dist0 >= 0 && dist1 >= 0)
      return -1;

    if(dist0 < 0 && dist1 < 0)
      return 1;

    // XXX: Handle dist0 == dist1?

    mid = dist0 / (dist0 - dist1);

    return 0;
  }

  /**
   * Intersection between moving box and plane.
   */
  static inline int intersect(const Vector3& start, const Vector3& end,
                              const Vector3& normal, float distance,
                              const Vector3& min, const Vector3& max,
                              float& dist0, float& dist1,
                              float& mid0, float& mid1)
  {
    dist0 = normal * start - distance;
    dist1 = normal * end - distance;

    Vector3 corner0, corner1;

    for(uint i = 0; i < 3; ++i)
    {
      if(normal(i) < 0)
        corner0(i) = min(i), corner1(i) = max(i);
      else
        corner0(i) = max(i), corner1(i) = min(i);
    }

    float offset0 = corner0 * normal;
    float offset1 = corner1 * normal;

    if(dist0 >= offset0 && dist1 >= offset0)
      return -1;

    if(dist0 <= offset1 && dist1 <= offset1)
      return 1;

    // XXX: Handle dist0 == dist1?

    if(dist0 > dist1)
    {
      mid0 = (dist0 - offset0) / (dist0 - dist1);
      mid1 = (dist0 - offset1) / (dist0 - dist1);
    }
    else // dist0 <= dist1
    {
      mid0 = (dist0 - offset1) / (dist0 - dist1);
      mid1 = (dist0 - offset0) / (dist0 - dist1);
    }

    return 0;
  }

  /**
   * Intersection between moving capsule and plane.
   */
  static inline int capsuleIntersect(const Vector3& start, const Vector3& end,
                                     const Vector3& normal, float distance,
                                     const Vector3& min, const Vector3& max,
                                     float& dist0, float& dist1,
                                     float& mid0, float& mid1)
  {
    dist0 = normal * start - distance;
    dist1 = normal * end - distance;

    Vector3 corner0, corner1;

    for(uint i = 0; i < 3; ++i)
    {
      if(normal(i) < 0)
        corner0(i) = min(i) * -normal(i), corner1(i) = max(i) * -normal(i);
      else
        corner0(i) = max(i) * normal(i), corner1(i) = min(i) * normal(i);
    }

    float offset0 = corner0 * normal;
    float offset1 = corner1 * normal;

    if(dist0 >= offset0 && dist1 >= offset0)
      return -1;

    if(dist0 <= offset1 && dist1 <= offset1)
      return 1;

    // XXX: Handle dist0 == dist1?

    if(dist0 > dist1)
    {
      mid0 = (dist0 - offset0) / (dist0 - dist1);
      mid1 = (dist0 - offset1) / (dist0 - dist1);
    }
    else // dist0 <= dist1
    {
      mid0 = (dist0 - offset1) / (dist0 - dist1);
      mid1 = (dist0 - offset0) / (dist0 - dist1);
    }

    return 0;
  }

  /**
   * Is box in front of plane?
   */
  static inline bool front(const Vector3& normal, float distance,
                           const int* mins, const int* maxs)
  {
    Vector3 nearest;

    for(uint i = 0; i < 3; ++i)
      nearest(i) = (normal(i) < 0) ? mins[i] : maxs[i];

    return nearest * normal < distance;
  }

  /**
   * Is box in front of plane?
   */
  static inline bool front(const Vector3& normal, float distance,
                           const Vector3& min, const Vector3& max)
  {
    Vector3 nearest;

    for(uint i = 0; i < 3; ++i)
      nearest(i) = (normal(i) < 0) ? min(i) : max(i);

    return nearest * normal < distance;
  }

  /**
   * Is box in behind plane?
   */
  static inline bool back(const Vector3& normal, float distance,
                          const int* mins, const int* maxs)
  {
    Vector3 nearest;

    for(uint i = 0; i < 3; ++i)
      nearest(i) = (normal(i) > 0) ? mins[i] : maxs[i];

    return nearest * normal > distance;
  }

  /**
   * Is box in behind plane?
   */
  static inline bool back(const Vector3& normal, float distance,
                          const Vector3& min, const Vector3& max)
  {
    Vector3 nearest;

    for(uint i = 0; i < 3; ++i)
      nearest(i) = (normal(i) > 0) ? min(i) : max(i);

    return nearest * normal > distance;
  }

  /**
   * Intersection between line segment and sphere.
   */
  static inline bool intersect(const Vector3& start, const Vector3& end,
                               const Vector3& center, float radius)
  {
    // XXX: Extremely inefficient

    float radius2 = radius * radius;

    if((start - center).square() < radius2
    || (end - center).square() < radius2)
      return true;

    // Is point closest to 'center' between 'start' and 'end'?
    float u = ((center - start) * (end - start)) / (end - start).square();

    if(u < 0 || u > 1)
      return false;

    Vector3 intersection = start + u * (end - start);

    return radius > (intersection - center).magnitude();
  }

  /**
   * Intersection between two moving spheres.
   */
  static inline float intersect(const Vector3& center0, float radius0,
                                const Vector3& center1, float radius1,
                                const Vector3& dir0, const Vector3& dir1)
  {
    // XXX: Extremely inefficient

    Vector3 diff = center1 - center0;
    float diffsq = diff.square();

    float radius = radius0 + radius1;
    float radiussq = radius * radius;

    // Initially intersecting
    if(diffsq < radiussq)
      return 0;

    Vector3 dir = dir1 - dir0; // Relative direction
    float dirsq = dir.square();

    // No relative motion
    if(dirsq < 0)
      return 1;

    Vector3 normal = diff;
    normal.normalize();

    float speed = -(normal * dir);

    // Spheres are not moving towards each other
    if(speed <= 0)
      return 1;

    float t = (sqrt(diffsq) - sqrt(radiussq)) / speed;

    t = (t < 0) ? 0
      : (t > 1) ? 1
      : t;

    return t;
  }
};

#endif // !COLLISION_H_

// vim: ts=2 sw=2 et
