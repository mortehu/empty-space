#ifndef IMAGE_H
#define IMAGE_H

#ifndef SWIG
#include <stdint.h>

#include "types.h"
#endif

/**
 * Image data container.
 *
 * \author Morten Hustveit
 */
struct Image
{
  enum PixelFormat
  {
    RGB = 1,
    RGBA = 2,
    Gray = 3,
    NV12 = 4, // See www.fourcc.org
    RGB16 = 5,
    RGBA16 = 6,
    Gray16 = 7
  };

  /**
   * Reads an image.
   *
   * Returns 0 on failure.
   */
  static IMPORT Image* acquire(const char* fileName);

  /**
   * Creates an empty image.
   */
  Image(uint width, uint height, uint format) IMPORT;

  /**
   * Destroys an image.
   *
   * This function simply calls delete, but is provided to make the API
   * consistent.
   */
  static void unacquire(Image* image)
  {
    delete image;
  }

  /**
   * Destroys an image.
   */
  IMPORT ~Image();

  /**
   * Scales the image data to the specified resolution.
   */
  IMPORT void resize(uint width, uint height);

  /**
   * Copies the contents of this image into another previously allocated image.
   *
   * If the images are of different resolutions or pixel formats, the data is
   * converted.
   */
  IMPORT void copy(const Image&);

  /**
   * Returns the pixel format.
   */
  IMPORT uint pixelFormat() const
  {
    return _pixelFormat;
  }

  /**
   * Returns the width of in pixels.
   */
  IMPORT uint width() const
  {
    return _width;
  }

  /**
   * Returns the height of in pixels.
   */
  IMPORT uint height() const
  {
    return _height;
  }

  /**
   * Returns the amount of bytes per pixel.
   */
  IMPORT uint bytesPerPixel() const;

  /**
   * Returns the data type used for storing color components.
   *
   * This is either GL::UNSIGNED_BYTE or GL::UNSIGNED_SHORT
   */
  IMPORT uint dataType() const;

  /**
   * Returns the amount of color components.
   *
   * This is usually 1, 3 or 4.
   */
  IMPORT uint componentCount() const;

  /**
   * Returns the size of this image in bytes.
   */
  IMPORT uint size() const;

  IMPORT void brighten(float factor);

#ifndef SWIG
  uint8_t* data()
  {
    return _data;
  }

  uint16_t* data16()
  {
    return reinterpret_cast<uint16_t*>(_data);
  }

  const uint8_t* data() const
  {
    return _data;
  }

  const uint16_t* data16() const
  {
    return reinterpret_cast<const uint16_t*>(_data);
  }
#endif // !SWIG

protected:

  uint8_t* _data;
  uint     _width;
  uint     _height;
  uint     _pixelFormat;
};

#endif // !IMAGE_H

// vim: ts=2 sw=2 et
