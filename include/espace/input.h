#ifndef KEYS_H_
#define KEYS_H_

#ifndef SWIG
#  include "types.h"
#endif

/**
 * Namespace for keyboard, mouse and joystick routines.
 * \author Morten Hustveit
 */
struct Input
{
#ifndef SWIG
  typedef void (*KeyHandler)(int key, bool keyPressed);
  typedef void (*MouseHandler)(int deltaX, int deltaY);
  typedef void (*JoystickHandler)(int joystick, int axis, int value);

  static IMPORT int  activeKeys[16];
  static IMPORT uint activeKeyCount;

  static IMPORT void            setKeyHandler(KeyHandler);
  static IMPORT KeyHandler      keyHandler();
  static IMPORT void            setMouseHandler(MouseHandler);
  static IMPORT MouseHandler    mouseHandler();
  static IMPORT void            setJoystickHandler(JoystickHandler);
  static IMPORT JoystickHandler joystickHandler();
#endif

  /**
   * Binds the specified key to a game command.
   *
   * The command will not be executed until API::flushCommands() is called.
   *
   * \see API::flushCommands(), API::addCommand()
   */
  static IMPORT void bind(int key, const char* command);

  /**
   * Returns the binding for the specified key.
   *
   * NULL is returned if no binding exists.
   */
  static IMPORT const char* binding(int key);

  /**
   * Removes the binding for the specified key.
   */
  static IMPORT void unbind(int key);

  /**
   * Removes the bindings for all keys.
   */
  static IMPORT void unbindAll();

  /**
   * Returns the ASCIIZ-encoded name of the specified key.
   *
   * "Key #<number>" is returned if no such key exists.
   */
  static IMPORT const char* name(int key);

  /**
   * Returns the key number for the specified key name.
   *
   * NULL is returned if no such key name exists.
   */
  static IMPORT int number(const char* name);

  /**
   * Returns whether the specified key is currently pressed.
   */
  static IMPORT bool isPressed(int key);

  enum KeyFlags
  {
    K_CHAR_FLAG = 0x400,
    K_ALT_FLAG = 0x800,
    K_CTRL_FLAG = 0x1000
  };

  enum Key
  {
    K_TAB = 9,
    K_ENTER = 13,
    K_ESCAPE = 27,
    K_SPACE = 32,

    K_BACKSPACE = 127,

    K_COMMAND = 128,
    K_CAPSLOCK,
    K_POWER,
    K_PAUSE,

    K_UPARROW,
    K_DOWNARROW,
    K_LEFTARROW,
    K_RIGHTARROW,

    K_ALT,
    K_CTRL,
    K_SHIFT,
    K_INS,
    K_DEL,
    K_PGDN,
    K_PGUP,
    K_HOME,
    K_END,

    K_F1,
    K_F2,
    K_F3,
    K_F4,
    K_F5,
    K_F6,
    K_F7,
    K_F8,
    K_F9,
    K_F10,
    K_F11,
    K_F12,
    K_F13,
    K_F14,
    K_F15,

    K_KP_HOME,
    K_KP_UPARROW,
    K_KP_PGUP,
    K_KP_LEFTARROW,
    K_KP_5,
    K_KP_RIGHTARROW,
    K_KP_END,
    K_KP_DOWNARROW,
    K_KP_PGDN,
    K_KP_ENTER,
    K_KP_INS,
    K_KP_DEL,
    K_KP_SLASH,
    K_KP_MINUS,
    K_KP_PLUS,
    K_KP_NUMLOCK,
    K_KP_STAR,
    K_KP_EQUALS,

    K_MOUSE1,
    K_MOUSE2,
    K_MOUSE3,
    K_MOUSE4,
    K_MOUSE5,

    K_MWHEELDOWN,
    K_MWHEELUP,

    K_JOY1,
    K_JOY2,
    K_JOY3,
    K_JOY4,
    K_JOY5,
    K_JOY6,
    K_JOY7,
    K_JOY8,
    K_JOY9,
    K_JOY10,
    K_JOY11,
    K_JOY12,
    K_JOY13,
    K_JOY14,
    K_JOY15,
    K_JOY16,
    K_JOY17,
    K_JOY18,
    K_JOY19,
    K_JOY20,
    K_JOY21,
    K_JOY22,
    K_JOY23,
    K_JOY24,
    K_JOY25,
    K_JOY26,
    K_JOY27,
    K_JOY28,
    K_JOY29,
    K_JOY30,
    K_JOY31,
    K_JOY32,

    K_AUX1,
    K_AUX2,
    K_AUX3,
    K_AUX4,
    K_AUX5,
    K_AUX6,
    K_AUX7,
    K_AUX8,
    K_AUX9,
    K_AUX10,
    K_AUX11,
    K_AUX12,
    K_AUX13,
    K_AUX14,
    K_AUX15,
    K_AUX16,

    K_RTCW_LAST_KEY,

    K_JOY2_1,
    K_JOY2_2,
    K_JOY2_3,
    K_JOY2_4,
    K_JOY2_5,
    K_JOY2_6,
    K_JOY2_7,
    K_JOY2_8,
    K_JOY2_9,
    K_JOY2_10,
    K_JOY2_11,
    K_JOY2_12,
    K_JOY2_13,
    K_JOY2_14,
    K_JOY2_15,
    K_JOY2_16,
    K_JOY2_17,
    K_JOY2_18,
    K_JOY2_19,
    K_JOY2_20,
    K_JOY2_21,
    K_JOY2_22,
    K_JOY2_23,
    K_JOY2_24,
    K_JOY2_25,
    K_JOY2_26,
    K_JOY2_27,
    K_JOY2_28,
    K_JOY2_29,
    K_JOY2_30,
    K_JOY2_31,
    K_JOY2_32,
    K_JOY3_1,
    K_JOY3_2,
    K_JOY3_3,
    K_JOY3_4,
    K_JOY3_5,
    K_JOY3_6,
    K_JOY3_7,
    K_JOY3_8,
    K_JOY3_9,
    K_JOY3_10,
    K_JOY3_11,
    K_JOY3_12,
    K_JOY3_13,
    K_JOY3_14,
    K_JOY3_15,
    K_JOY3_16,
    K_JOY3_17,
    K_JOY3_18,
    K_JOY3_19,
    K_JOY3_20,
    K_JOY3_21,
    K_JOY3_22,
    K_JOY3_23,
    K_JOY3_24,
    K_JOY3_25,
    K_JOY3_26,
    K_JOY3_27,
    K_JOY3_28,
    K_JOY3_29,
    K_JOY3_30,
    K_JOY3_31,
    K_JOY3_32,
    K_JOY4_1,
    K_JOY4_2,
    K_JOY4_3,
    K_JOY4_4,
    K_JOY4_5,
    K_JOY4_6,
    K_JOY4_7,
    K_JOY4_8,
    K_JOY4_9,
    K_JOY4_10,
    K_JOY4_11,
    K_JOY4_12,
    K_JOY4_13,
    K_JOY4_14,
    K_JOY4_15,
    K_JOY4_16,
    K_JOY4_17,
    K_JOY4_18,
    K_JOY4_19,
    K_JOY4_20,
    K_JOY4_21,
    K_JOY4_22,
    K_JOY4_23,
    K_JOY4_24,
    K_JOY4_25,
    K_JOY4_26,
    K_JOY4_27,
    K_JOY4_28,
    K_JOY4_29,
    K_JOY4_30,
    K_JOY4_31,
    K_JOY4_32,

    K_LAST_KEY,

    E_MOUSE_MOVED = 1024,
    E_JOYSTICK_MOVED,

    E_LAST_EVENT
  };

#ifndef SWIG
  /**
   * Called by system functions when a character is received.
   */
  static void charReceived(char letter);

  /**
   * Called by system functions when a key is pressed.
   */
  static void keyPressed(int key);

  /**
   * Called by system functions when a key is released.
   */
  static void keyReleased(int key);

  /**
   * Called by system functions when the mouse is moved.
   *
   * \todo Support multiple mice
   */
  static void mouseMoved(int deltaX, int deltaY);

  /**
   * Called by system functions when a joystick is moved.
   */
  static void joystickMoved(int joystick, int axis, int value);
#endif
};

#endif // !KEYS_H_

// vim: ts=2 sw=2 et
