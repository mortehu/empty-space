#ifndef SYSTEM_H_
#define SYSTEM_H_

#ifndef SWIG
#include "types.h"
#endif

/**
 * Platform specific routines.
 * \author Morten Hustveit
 */
struct System
{
  /**
   * Terminates the application
   */
  static IMPORT void exit();

  /**
   * Swaps buffer and process platform specific events.
   */
  static IMPORT void idle();

  /**
   * Get time.
   * \return Seconds passed since a system specific time.
   */
  static IMPORT double time();

  /**
   * Check available memory.
   * \return Bytes of available physical memory.
   */
  static IMPORT uint memoryRemaining();

#ifndef SWIG
  /**
   * Open dynamic library.
   * \param fileName Name of dynamic library.
   * \return Dynamic libary handle.
   */
  static IMPORT void* dlopen(const char* fileName);
  /**
   * Get address of symbol in dynamic library.
   * \param handle Dynamic library handle.
   * \param symbol NULL-terminated symbol name.
   * \return NULL on failure, address on success.
   */
  static IMPORT void* dlsym(void* handle, const char* symbol);
  /**
   * Get human readable error message.
   * \return Pointer to the last error message.
   */
  static IMPORT const char* dlerror();
  /**
   * Close dynamic library.
   * \param handle Dynamic library handle.
   */
  static IMPORT void dlclose(void* handle);

protected:

  friend class Renderer;

  static void initialize();
  static void updateScreen();

  static void shutdownVideo();

#ifdef LINUX
  static void initializeLinux();
  static void idleLinux();
#endif
#ifdef GNU
  static void initializeGNU();
  static void exitHandler(int signal);
  static void crashHandler(int signal);
#endif
#endif // !SWIG
};

#endif // !SYSTEM_H_

// vim: ts=2 sw=2 et
