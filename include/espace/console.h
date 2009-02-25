#ifndef _CONSOLE_H_
#define _CONSOLE_H_

#ifndef SWIG
#include "input.h"
#include "types.h"

class String;
#endif

/**
 * Console for entering commands and examining textual output run-time.
 * \author Carl Henrik Holth Lunde
 */
struct Console
{
  /**
   * Configure the look of the console.
   *
   * The shaders and font will be loaded on initialization.
   *
   * \warning Calls to this function after the console has been toggled will
   *          have no effect.
   *
   * \param font The font data file.  Both fixed width and variable width
   *             fonts are supported.
   * \param background The background shader used all over the console. This
   *                   will cover the whole screen width and half the height.
   * \param logo This shader will be rendered on top of the background on the
   *             center of the screen.  If logo is NULL no logo is rendered.
   *
   * \see initialize(), toggle()
   */
  static IMPORT void configure(const char* font = "fonts/fontImage_12.dat",
                               const char* background = "console-16bit",
                               const char* logo = "console2-16bit");
  /**
   * Toggle the console. This is usually only invoked by the toggleconsole
   * console command which you can bind to a key.
   * \todo How do we know if the console is already toggled?
   */
  static IMPORT void toggle();

  /**
   * Render the console.  This should be called even when the console is
   * not toggled to animate the console sliding away.
   */
  static IMPORT void render();

  /**
   * Put one character on the text buffer.
   * \see put(const String& line)
   */
  static IMPORT void put(int);

  /**
   * Put a string of characters in the text buffer.
   *
   * No newlines
   * are added and no line-wrapping is performed.
   *
   * To change the color of the text use one of the following
   * codes prepended by ^:
   *
   *   - 0: Black
   *   - 1: Red
   *   - 2: Green
   *   - 3: Yellow
   *   - 4: Blue
   *   - 5: Cyan
   *   - 6: Magenta
   *   - 7: White
   *
   * The default color is white.
   */
  static IMPORT void put(const String& line);

  /**
   * Clear the console buffer and command history.
   */
  static IMPORT void clear();

#ifndef SWIG
  static IMPORT void addCommand(const String&);
  static IMPORT void removeCommand(const String&);
  static IMPORT uint commandCount();
  static IMPORT const String& command(uint);
#endif

protected:

  /**
   * This function is protected and is called the first time the console is
   * toggled.  It loads the font and shaders set by configure.
   *
   * \see toggle(), configure()
   */
  static void initialize();
  static void keyHandler(int key, bool keyPressed);
  static void print(int character);
  static void print(const char* str);
};

#endif

// vim: ts=2 sw=2 et
