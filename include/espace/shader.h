#ifndef SHADER_H_
#define SHADER_H_

#ifndef SWIG
#include "color.h"
#include "types.h"
#include "renderer.h"
#endif

/**
 * Shader routines.
 *
 * \author Morten Hustveit
 */
struct Shader
{
  /**
   * Acquire a handle for a shader.
   *
   * Returns NULL if the shader is not found.
   */
  static IMPORT Shader* acquire(const char* name, bool mipmaps = true);

  /**
   * Acquire an integer handle for a shader.
   *
   * Returns 0 if the shader is not found.
   */
  static IMPORT int acquireHandle(const char* name, bool mipmaps = true);

  /**
   * Unacquire a shader handle acquired by a call to acquire().
   */
  static IMPORT void unacquire(Shader*);

  /**
   * Unacquire an integer shader handle acquired by a call to acquireHandle().
   */
  static IMPORT void unacquire(int handle);

  /**
   * Converts an integer handle to a pointer handle.
   *
   * Returns NULL if the handle is invalid.
   */
  static IMPORT Shader* shaderForHandle(int handle);

  /**
   * Creates an alternate name for a shader.
   */
  static IMPORT void remap(const char* oldShader, const char* newShader);

#ifndef SWIG
  virtual IMPORT ~Shader();

  virtual IMPORT void unacquire() = 0;

  virtual IMPORT uint passCount() const = 0;
  virtual IMPORT void pushState(uint pass) = 0;
  virtual IMPORT void popState() = 0;

  virtual IMPORT bool isSky() const = 0;
  virtual IMPORT uint sort() const = 0;
  virtual IMPORT Renderer::Face cullFace() const = 0;

  uint refCount;

  enum WaveForm
  {
    Sawtooth,
    InverseSawtooth,
    Sin,
    Square,
    Triangle
  };

  enum VertexDeform
  {
    Wave,
    Normal,
    Bulge,
    Move,
    AutoSprite,
    AutoSprite2
  };

  enum Sort
  {
    Portal =     (1 << 24),
    Sky =        (2 << 24),
    Opaque =     (3 << 24),
    Banner =     (6 << 24),
    Underwater = (8 << 24),
    Additive =   (9 << 24),
    Nearest =    (16 << 24)
  };

  enum AlphaFunc
  {
    All,
    GT0,
    LT128,
    GE128
  };

  static IMPORT float time;

  static IMPORT float wave(WaveForm wave, float base, float amp, float phase, float freq, float time);

  // This have to go

  static IMPORT bool      st_withAlpha;
  static IMPORT bool      st_tcMod;
  static IMPORT Color     st_entityColor;
#endif // !SWIG
};

#endif // !SHADER_H_

// vim: ts=2 sw=2 et
