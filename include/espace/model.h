#ifndef MODEL_H_
#define MODEL_H_

#ifndef SWIG
#include "string.h"
#include "types.h"
#include "vector.h"
#endif // !SWIG

/**
 * 3D model interface.
 * \author Morten Hustveit
 */
struct Model
{
  /**
   * Loads a 3D object and returns a handle for it.
   *
   * Reference counting is used to avoid duplicate copies.
   */
  static IMPORT uint acquire(const char* name);

  /**
   * Frees a model handle acquired with acquire().
   */
  static IMPORT void unacquire(uint);

  /**
   * Get the model corresponding to a given handle
   *
   * Returns NULL if the handle is invalid.
   */
  static IMPORT Model* modelForHandle(uint handle);

  /**
   * Returns the bounding box of the model.
   */
  virtual IMPORT void boundBox(Vector3& min, Vector3& max) = 0;

  /**
   * Renders the model.
   *
   * Specifying both a custom shader and a custom skin does not make sense.
   *
   * \param frame The frame to render.
   * \param backLerp How much to interpolate with the previous frame.
   * \param customShader Shader to use instead of the default shader or skin.
   * \param customSkin Skin to use instead of the default shader orskin.
   */
  virtual IMPORT void render(int frame, float backLerp = 0,
                      uint customShader = 0, uint customSkin = 0) = 0;

  /**
   * Returns the origin and axis of the specified tag.
   *
   * \todo Details?
   */
  virtual IMPORT int tag(const char* name, Vector3& origin, Vector3 axis[3], int startIndex);

protected:

  virtual IMPORT ~Model();
};

#endif // !MODEL_H_

// vim: ts=2 sw=2 et
