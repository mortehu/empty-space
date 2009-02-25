#ifndef MAP_H_
#define MAP_H_

#ifndef SWIG
#include <map>

#include "collision.h"
#include "string.h"
#include "types.h"
#include "vector.h"
#include "model.h"
#endif

/**
 * Structure for holding map entity information.
 */
struct Entity
#ifndef SWIG
  : public std::map<String, String>
#endif
{
  IMPORT String get(const String& key) const;
};

/**
 * Map interface.
 *
 * \author Morten Hustveit
 */
struct Map
{
  /**
   * Reads a map file and returns a pointer to the object.
   */
  static IMPORT Map* acquire(const char* name);

  /**
   * Frees a map object previously allocated with acquire().
   */
  static IMPORT void unacquire(Map* map);

  /**
   * Renders the map from the viewpoint given in the OpenGL matrices.
   */
  virtual void render() = 0;

  /**
   * Returns the amount of entities in this map.
   *
   * \see entity()
   */
  virtual uint entityCount() const = 0;

  /**
   * Returns a map entity.
   *
   * \param index Index of the entity to return.
   *
   * \see entityCount()
   */
  virtual Entity& entity(uint index) = 0;

  /**
   * Returns the amount of models in this map.
   *
   * \see model()
   */
  virtual uint   modelCount() const = 0;

  /**
   * Returns a map model.
   *
   * \param index Index of the model to return.
   *
   * \see modelCount()
   */
  virtual Model* model(uint index) = 0;

  /**
   * Performs ray intersection testing.
   *
   * \param start Start of the ray to trace.
   * \param end   End of the ray to trace.
   * \param trace Where to place trace results.
   * \param contentMask Bits the surfaces' content value must match to be
   *                    considered for intersection testing.
   *
   * \todo Use a "Ray" data structure instead.
   */
  virtual void rayTrace(const Vector3& start, const Vector3& end,
                        Trace& trace, int contentMask) const = 0;

  /**
   * Performs sphere intersection testing.
   *
   * \param start Initial center point of the sphere.
   * \param end   Final center point of the sphere.
   * \param float Radius of the sphere.
   * \param trace Where to place trace results.
   * \param contentMask Bits the surfaces' content value must match to be
   *                    considered for intersection testing.
   *
   * \todo Use a "Sphere" data structure instead.
   */
  virtual void sphereTrace(const Vector3& start, const Vector3& end,
                           float radius, Trace& trace,
                           int contentMask) const = 0;

  /**
   * Performs axis aligned box intersection testing.
   *
   * \param start Initial center point of the box.
   * \param end   Final center point of the box.
   * \param min   Position of the corner of the box with minimum coordinates,
   *              relative to the center point.
   * \param max   Position of the corner of the box with maximum coordinates,
   *              relative to the center point.
   * \param trace Where to place trace results.
   * \param contentMask Bits the surfaces' content value must match to be
   *                    considered for intersection testing.
   *
   * \todo Use a "AABox" data structure instead.
   */
  virtual void boxTrace(const Vector3& start, const Vector3& end,
                        const Vector3& min, const Vector3& max,
                        Trace& trace, int contentMask) const = 0;

  /**
   * Performs axis aligned capsule intersection testing.
   *
   * \param start Initial center point of the capsule.
   * \param end   Final center point of the capsule.
   * \param min   Extension of the capsule in the negative direction of the
   *              axes, relative to the center point.
   * \param max   Extension of the capsule in the positive direction of the
   *              axes, relative to the center point.
   * \param trace Where to place trace results.
   * \param contentMask Bits the surfaces' content value must match to be
   *                    considered for intersection testing.
   *
   * \todo Use a "AABox" data structure instead.
   */
  virtual void capsuleTrace(const Vector3& start, const Vector3& end,
                            const Vector3& min, const Vector3& max,
                            Trace& trace, int contentMask) const = 0;

  /**
   * Returns the contents in a given axis aligned box.
   *
   * \param min   Minimum coordinate of box.
   * \param max   Maximum coordinate of box.
   * \param trace Where to place trace results.
   * \param contentMask Bits the surfaces' content value must match to be
   *                    considered for testing.
   */
  virtual void contents(const Vector3& min, const Vector3& max,
                        Trace& trace, int contentMask) const = 0;

  /**
   * Returns whether a point is potentially visible from another point.
   *
   * Unless the map contains surfaces that are transparent on one side, and
   * opaque on the other, visible(A, B) is identical to visible(B, A).
   *
   * \param from  The point from which to look.
   * \param to    The point to look at.
   */
  virtual bool visible(const Vector3& from, const Vector3& to) const = 0;

protected:

  virtual IMPORT ~Map();
};

#endif // !MAP_H_

// vim: ts=2 sw=2 et
