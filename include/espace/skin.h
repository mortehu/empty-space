#ifndef SKIN_H_
#define SKIN_H_

#ifndef SWIG
#include "string.h"
#include "types.h"
#endif // !SWIG

/**
 * Skin interface.
 *
 * \author Morten Hustveit
 */
class IMPORT Skin
{
public:

  /**
   * Acquire a handle for a skin.
   *
   * Returns 0 if the shader is not found.
   */
  static uint acquire(const char* name);

  /**
   * Unacquire a skin handle acquired by a call to acquire().
   */
  static void unacquire(uint);

  /**
   * Get a pointer to the skin object for a specified handle.
   */
  static Skin* skinForHandle(uint);

#ifndef SWIG
  void pushState(const char* surfaceName);

  String name;

  struct Handle
  {
    String name;
    uint   handle;
  };

  const uint texture(const String& name) const;

  uint refCount;

protected:

  Skin();
  ~Skin();

  struct Internal;

  Internal* m;
#endif // !SWIG
};

#endif // !SKIN_H_

// vim: ts=2 sw=2 et
