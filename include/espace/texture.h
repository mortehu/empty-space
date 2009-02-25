#ifndef TEXTURE_H
#define TEXTURE_H

#ifndef SWIG
#include "types.h"
#endif

struct Image;

/**
 * Namespace for managing OpenGL texture handles.
 */
struct Texture
{
  static const uint lightmap = (uint) ~0UL;

  enum Flags
  {
    NoMipMaps =  0x00000001,
    NoLinear =   0x00000002,
    NoXRepeat =  0x00000004,
    NoYRepeat =  0x00000008,
    NoRepeat =   0x0000000C,
    NVRect =     0x00000010
  };

  /**
   * Loads an image and creates an OpenGL handle for it.
   *
   * If the image's width or height is not a power of 2, the image is
   * rescaled * to fit this requirement, unless NVRect is specified (only
   * supported on systems with the GL_NV_texture_rectangle extension).
   *
   * If this function is called several times with the same parameters, no
   * new data is allocated, but a reference counter is increased instead.  The data
   * is not freed until unacquire() is called an equal amount of times on the
   * returned handle.
   *
   * Returns OpenGL handle for the image on success, zero on failure.
   *
   * \param name The file name of the image.
   * \param flags Flags.
   */
  static IMPORT uint acquire(const char* name, uint flags = 0);

  /**
   * Creates an OpenGL handle for an already loaded image.
   *
   * If the image's width or height is not a power of 2, the image is rescaled
   * to fit this requirement.
   *
   * Returns OpenGL handle for the image on success, zero on failure.
   *
   * You can destroy the image after calling this function.
   */
  static IMPORT uint acquire(Image* image, uint flags = 0);

  /**
   * Unacquires a handle previous acquired with acquire().
   *
   * \see acquire()
   */
  static IMPORT void unacquire(uint handle);
};

#endif // !TEXTURE_H

// vim: ts=2 sw=2 et
