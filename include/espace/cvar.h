#ifndef CVAR_H_
#define CVAR_H_

#ifndef SWIG
#include "string.h"
#endif

/**
 * Console variable interface.
 *
 * \author Morten Hustveit
 */
struct CVar
{
  typedef void (*Callback)(const char* name);

  enum Flag
  {
    Archive = 1,       /**< Save to vars.rc */
    UserInfo = 2,      /**< Send to server on connect */
    ServerInfo = 4,    /**< Send in response to frontend requests */
    SystemInfo = 8,    /**< Duplicate on all clients */
    Init = 16,         /**< Change from command line only */
    Latch = 32,        /**< XXX: Odd behavior */
    ROM = 64,          /**< Cannot be changed by user */
    UserCreated = 128, /**< Created by a "set" command */
    Temp = 256,        /**< Not archived */
    Cheat = 512,       /**< Read only if cheats are disabled */
    NoRestart = 1024   /**< Don't clear on restart */
  };

  int   handle;
  int   modificationCount;
  float value;
  int   integer;
  char  string[256];

  /**
   * Reread the variable's value from the engine's database.
   */
  IMPORT void update();

  /**
   * Commit any change in the variable's value to the engine's database.
   */
  IMPORT void commit();

  /**
   * Sets a function to be called whenever the variable's value changes.
   */
  IMPORT void setCallback(Callback callback);

#ifndef SWIG
  /**
   * Update the value of the variable.
   *
   * You need to commit the changes to make them persistent.
   *
   * \see commit()
   */
  IMPORT CVar& operator=(int integer);

  /**
   * Update the value of the variable.
   *
   * You need to commit the changes to make them persistent.
   *
   * \see commit()
   */
  IMPORT CVar& operator=(const char* string);

  /**
   * Update the value of the variable.
   *
   * You need to commit the changes to make them persistent.
   *
   * \see commit()
   */
  IMPORT CVar& operator=(float value);
#endif

  /**
   * Get a handle for the specified console variable.
   *
   * \param name The name of the variable to get a handle for.
   * \param defaultValue The default value of the variable, set if the
   *                     variable did not exist prior to this call.
   * \param flags Flags that should be associated with the variable.
   */
  static IMPORT CVar acquire(const char* name, const char* defaultValue = "",
                               int flags = 0);

  /**
   * Get the string value of the specified variable.
   */
  static IMPORT String getString(const char* name);

  /**
   * Get the integer value of the specified variable.
   */
  static IMPORT int getInt(const char* name);

  /**
   * Get the floating point value of the spcified variable.
   */
  static IMPORT float getFloat(const char* name);

  /**
   * Get a string containing a selection of variables.
   *
   * The list of values is encoded by concatening "name\value\" for each
   * variable having flags matching at least 1 bit in the provided mask.
   *
   * \param mask The mask to match variables with.
   */
  static IMPORT String getVars(int mask);

  /**
   * Set the location where console variables will be saved.
   *
   * Only variables with the archive attribute will be saved.  The variables are
   * saved upon program termination.
   */
  static IMPORT void setSaveLocation(const char* fileName);

protected:

  friend struct System;

  static void save();
};

#endif // !CVAR_H_

