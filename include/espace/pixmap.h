#ifndef PIXMAP_H_
#define PIXMAP_H_

#ifndef SWIG
#include "image.h"
#include "types.h"
#endif

struct PixmapPrivate;

/**
 * Pixel map routines.
 *
 * OpenGL requires the width and height of textures to be in powers of two.
 * This class aids in displaying images not fitting this requirement, by
 * subdividing the images into smaller images.
 *
 * If the r_recttextures variable is set, and the GL_NV_texture_rectangle
 * extension is supported, textures with non-power-of-two dimensions will be
 * used.  This approach proved to be slower than subdivision on a
 * GeForce FX 5600, so r_recttextures is set to 0 by default.
 *
 * When software rendering is used, drawing pixmaps is significantly faster
 * than drawing textures.
 *
 * \author Morten Hustveit
 */
struct IMPORT Pixmap
{
  /**
   * Reads an image and creates a pixmap from it.
   *
   * Returns 0 on failure.
   */
  static IMPORT Pixmap* acquire(const char* fileName);

  /**
   * Constructs a new pixmap.
   *
   * \todo How about a const Image&?
   * \param image Image to read.
   */
  Pixmap(Image* image) IMPORT;

  /**
   * Destroys a pixmap.
   *
   * This function simply calls delete, but is provided to make the API
   * consistent.
   */
  static void unacquire(Pixmap* pixmap)
  {
    delete pixmap;
  }

  /**
   * Destroys a pixmap.
   */
  IMPORT ~Pixmap();

protected:

  friend class Renderer;

  PixmapPrivate* d;
};

#endif // !PIXMAP_H_

// vim: ts=2 sw=2 et
