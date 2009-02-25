/***************************************************************************
                            input.cc  -  Keyboard routines
                               -------------------
      copyright            : (C) 2003 by Morten Hustveit
      email                : morten@debian.org
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <deque>
#include <map>

#include <espace/api.h>
#include <espace/input.h>
#include <espace/string.h>
#include <espace/stringlist.h>

namespace
{
  std::map<int, String> keymap;

  Input::KeyHandler      _keyHandler = 0;
  Input::MouseHandler    _mouseHandler = 0;
  Input::JoystickHandler _joystickHandler = 0;
}

int  Input::activeKeys[16];
uint Input::activeKeyCount = 0;

void Input::setKeyHandler(KeyHandler handler)
{
  _keyHandler = handler;
}

Input::KeyHandler Input::keyHandler()
{
  return _keyHandler;
}

void Input::setMouseHandler(Input::MouseHandler handler)
{
  _mouseHandler = handler;
}

Input::MouseHandler Input::mouseHandler()
{
  return _mouseHandler;
}

void Input::setJoystickHandler(Input::JoystickHandler handler)
{
  _joystickHandler = handler;
}

Input::JoystickHandler Input::joystickHandler()
{
  return _joystickHandler;
}

void Input::bind(int key, const char* command)
{
  keymap[key] = command;
}

const char* Input::binding(int key)
{
  std::map<int, String>::const_iterator i = keymap.find(key);

  if(i == keymap.end())
    return 0;

  return i->second;
}

void Input::unbind(int key)
{
  keymap.erase(key);
}

void Input::unbindAll()
{
  keymap.clear();
}

const char* Input::name(int num)
{
  switch(num)
  {
  case K_TAB: return "TAB";
  case K_ESCAPE: return "ESCAPE";
  case K_ENTER: return "ENTER";
  case K_KP_ENTER: return "KP_ENTER";
  case K_SPACE: return "SPACE";
  case K_BACKSPACE: return "BACKSPACE";
  case K_CAPSLOCK: return "CAPSLOCK";
  case K_KP_UPARROW: return "KP_UPARROW";
  case K_KP_DOWNARROW: return "KP_DOWNARROW";
  case K_KP_LEFTARROW: return "KP_LEFTARROW";
  case K_KP_RIGHTARROW: return "KP_RIGHTARROW";
  case K_UPARROW: return "UPARROW";
  case K_DOWNARROW: return "DOWNARROW";
  case K_LEFTARROW: return "LEFTARROW";
  case K_RIGHTARROW: return "RIGHTARROW";
  case K_ALT: return "ALT";
  case K_CTRL: return "CTRL";
  case K_SHIFT: return "SHIFT";
  case K_KP_INS: return "KP_INS";
  case K_KP_DEL: return "KP_DEL";
  case K_KP_PGUP: return "KP_PGUP";
  case K_KP_PGDN: return "KP_PGDN";
  case K_KP_HOME: return "KP_HOME";
  case K_KP_END: return "KP_END";
  case K_INS: return "INS";
  case K_DEL: return "DEL";
  case K_PGUP: return "PGUP";
  case K_PGDN: return "PGDN";
  case K_HOME: return "HOME";
  case K_END: return "END";
  case K_F1: return "F1";
  case K_F2: return "F2";
  case K_F3: return "F3";
  case K_F4: return "F4";
  case K_F5: return "F5";
  case K_F6: return "F6";
  case K_F7: return "F7";
  case K_F8: return "F8";
  case K_F9: return "F9";
  case K_F10: return "F10";
  case K_F11: return "F11";
  case K_F12: return "F12";
  case K_F13: return "F13";
  case K_F14: return "F14";
  case K_F15: return "F15";
  case K_PAUSE: return "PAUSE";
  case K_KP_STAR: return "KP_STAR";
  case K_MOUSE1: return "MOUSE1";
  case K_MOUSE2: return "MOUSE2";
  case K_MOUSE3: return "MOUSE3";
  case K_MOUSE4: return "MOUSE4";
  case K_MOUSE5: return "MOUSE5";
  case K_MWHEELUP: return "MWHEELUP";
  case K_MWHEELDOWN: return "MWHEELDOWN";
  case K_JOY1: return "K_JOY1";
  case K_JOY2: return "K_JOY2";
  case K_JOY3: return "K_JOY3";
  case K_JOY4: return "K_JOY4";
  case K_JOY5: return "K_JOY5";
  case K_JOY6: return "K_JOY6";
  case K_JOY7: return "K_JOY7";
  case K_JOY8: return "K_JOY8";
  case K_JOY9: return "K_JOY9";
  case K_JOY10: return "K_JOY10";
  case K_JOY11: return "K_JOY11";
  case K_JOY12: return "K_JOY12";
  case K_JOY13: return "K_JOY13";
  case K_JOY14: return "K_JOY14";
  case K_JOY15: return "K_JOY15";
  case K_JOY16: return "K_JOY16";
  case K_JOY17: return "K_JOY17";
  case K_JOY18: return "K_JOY18";
  case K_JOY19: return "K_JOY19";
  case K_JOY20: return "K_JOY20";
  case K_JOY21: return "K_JOY21";
  case K_JOY22: return "K_JOY22";
  case K_JOY23: return "K_JOY23";
  case K_JOY24: return "K_JOY24";
  case K_JOY25: return "K_JOY25";
  case K_JOY26: return "K_JOY26";
  case K_JOY27: return "K_JOY27";
  case K_JOY28: return "K_JOY28";
  case K_JOY29: return "K_JOY29";
  case K_JOY30: return "K_JOY30";
  case K_JOY31: return "K_JOY31";
  case K_JOY32: return "K_JOY32";
  case K_AUX1: return "K_AUX1";
  case K_AUX2: return "K_AUX2";
  case K_AUX3: return "K_AUX3";
  case K_AUX4: return "K_AUX4";
  case K_AUX5: return "K_AUX5";
  case K_AUX6: return "K_AUX6";
  case K_AUX7: return "K_AUX7";
  case K_AUX8: return "K_AUX8";
  case K_AUX9: return "K_AUX9";
  case K_AUX10: return "K_AUX10";
  case K_AUX11: return "K_AUX11";
  case K_AUX12: return "K_AUX12";
  case K_AUX13: return "K_AUX13";
  case K_AUX14: return "K_AUX14";
  case K_AUX15: return "K_AUX15";
  case K_AUX16: return "K_AUX16";
  case K_JOY2_1: return "K_JOY2_1";
  case K_JOY2_2: return "K_JOY2_2";
  case K_JOY2_3: return "K_JOY2_3";
  case K_JOY2_4: return "K_JOY2_4";
  case K_JOY2_5: return "K_JOY2_5";
  case K_JOY2_6: return "K_JOY2_6";
  case K_JOY2_7: return "K_JOY2_7";
  case K_JOY2_8: return "K_JOY2_8";
  case K_JOY2_9: return "K_JOY2_9";
  case K_JOY2_10: return "K_JOY2_10";
  case K_JOY2_11: return "K_JOY2_11";
  case K_JOY2_12: return "K_JOY2_12";
  case K_JOY2_13: return "K_JOY2_13";
  case K_JOY2_14: return "K_JOY2_14";
  case K_JOY2_15: return "K_JOY2_15";
  case K_JOY2_16: return "K_JOY2_16";
  case K_JOY2_17: return "K_JOY2_17";
  case K_JOY2_18: return "K_JOY2_18";
  case K_JOY2_19: return "K_JOY2_19";
  case K_JOY2_20: return "K_JOY2_20";
  case K_JOY2_21: return "K_JOY2_21";
  case K_JOY2_22: return "K_JOY2_22";
  case K_JOY2_23: return "K_JOY2_23";
  case K_JOY2_24: return "K_JOY2_24";
  case K_JOY2_25: return "K_JOY2_25";
  case K_JOY2_26: return "K_JOY2_26";
  case K_JOY2_27: return "K_JOY2_27";
  case K_JOY2_28: return "K_JOY2_28";
  case K_JOY2_29: return "K_JOY2_29";
  case K_JOY2_30: return "K_JOY2_30";
  case K_JOY2_31: return "K_JOY2_31";
  case K_JOY2_32: return "K_JOY2_32";
  case K_JOY3_1: return "K_JOY3_1";
  case K_JOY3_2: return "K_JOY3_2";
  case K_JOY3_3: return "K_JOY3_3";
  case K_JOY3_4: return "K_JOY3_4";
  case K_JOY3_5: return "K_JOY3_5";
  case K_JOY3_6: return "K_JOY3_6";
  case K_JOY3_7: return "K_JOY3_7";
  case K_JOY3_8: return "K_JOY3_8";
  case K_JOY3_9: return "K_JOY3_9";
  case K_JOY3_10: return "K_JOY3_10";
  case K_JOY3_11: return "K_JOY3_11";
  case K_JOY3_12: return "K_JOY3_12";
  case K_JOY3_13: return "K_JOY3_13";
  case K_JOY3_14: return "K_JOY3_14";
  case K_JOY3_15: return "K_JOY3_15";
  case K_JOY3_16: return "K_JOY3_16";
  case K_JOY3_17: return "K_JOY3_17";
  case K_JOY3_18: return "K_JOY3_18";
  case K_JOY3_19: return "K_JOY3_19";
  case K_JOY3_20: return "K_JOY3_20";
  case K_JOY3_21: return "K_JOY3_21";
  case K_JOY3_22: return "K_JOY3_22";
  case K_JOY3_23: return "K_JOY3_23";
  case K_JOY3_24: return "K_JOY3_24";
  case K_JOY3_25: return "K_JOY3_25";
  case K_JOY3_26: return "K_JOY3_26";
  case K_JOY3_27: return "K_JOY3_27";
  case K_JOY3_28: return "K_JOY3_28";
  case K_JOY3_29: return "K_JOY3_29";
  case K_JOY3_30: return "K_JOY3_30";
  case K_JOY3_31: return "K_JOY3_31";
  case K_JOY3_32: return "K_JOY3_32";
  case K_JOY4_1: return "K_JOY4_1";
  case K_JOY4_2: return "K_JOY4_2";
  case K_JOY4_3: return "K_JOY4_3";
  case K_JOY4_4: return "K_JOY4_4";
  case K_JOY4_5: return "K_JOY4_5";
  case K_JOY4_6: return "K_JOY4_6";
  case K_JOY4_7: return "K_JOY4_7";
  case K_JOY4_8: return "K_JOY4_8";
  case K_JOY4_9: return "K_JOY4_9";
  case K_JOY4_10: return "K_JOY4_10";
  case K_JOY4_11: return "K_JOY4_11";
  case K_JOY4_12: return "K_JOY4_12";
  case K_JOY4_13: return "K_JOY4_13";
  case K_JOY4_14: return "K_JOY4_14";
  case K_JOY4_15: return "K_JOY4_15";
  case K_JOY4_16: return "K_JOY4_16";
  case K_JOY4_17: return "K_JOY4_17";
  case K_JOY4_18: return "K_JOY4_18";
  case K_JOY4_19: return "K_JOY4_19";
  case K_JOY4_20: return "K_JOY4_20";
  case K_JOY4_21: return "K_JOY4_21";
  case K_JOY4_22: return "K_JOY4_22";
  case K_JOY4_23: return "K_JOY4_23";
  case K_JOY4_24: return "K_JOY4_24";
  case K_JOY4_25: return "K_JOY4_25";
  case K_JOY4_26: return "K_JOY4_26";
  case K_JOY4_27: return "K_JOY4_27";
  case K_JOY4_28: return "K_JOY4_28";
  case K_JOY4_29: return "K_JOY4_29";
  case K_JOY4_30: return "K_JOY4_30";
  case K_JOY4_31: return "K_JOY4_31";
  case K_JOY4_32: return "K_JOY4_32";
  case '*': return "*";
  case ',': return ",";
  case '-': return "-";
  case '.': return ".";
  case '/': return "/";
  case '[': return "[";
  case ']': return "]";
  case '\\': return "\\";
  case '=': return "=";
  case '\'': return "'";
  case '~': return "~";
  case '`': return "`";
  case '0': return "0";
  case '1': return "1";
  case '2': return "2";
  case '3': return "3";
  case '4': return "4";
  case '5': return "5";
  case '6': return "6";
  case '7': return "7";
  case '8': return "8";
  case '9': return "9";
  case 'A': return "A";
  case 'B': return "B";
  case 'C': return "C";
  case 'D': return "D";
  case 'E': return "E";
  case 'F': return "F";
  case 'G': return "G";
  case 'H': return "H";
  case 'I': return "I";
  case 'J': return "J";
  case 'K': return "K";
  case 'L': return "L";
  case 'M': return "M";
  case 'N': return "N";
  case 'O': return "O";
  case 'P': return "P";
  case 'Q': return "Q";
  case 'R': return "R";
  case 'S': return "S";
  case 'T': return "T";
  case 'U': return "U";
  case 'V': return "V";
  case 'W': return "W";
  case 'X': return "X";
  case 'Y': return "Y";
  case 'Z': return "Z";
  default: return String("Key #") + num;
  }
}

int Input::number(const char* _name)
{
  // XXX: Does not handle "Key #<number>", as returned from name()

  String name = _name;

  name.toUpper();

  // Copy from above, and do:
  // s/case \([^:]*\): return "\([^"]*\)";/else if(name == "\2") return \1;/
  if(name == "TAB") return K_TAB;
  else if(name == "ESCAPE") return K_ESCAPE;
  else if(name == "ENTER") return K_ENTER;
  else if(name == "KP_ENTER") return K_KP_ENTER;
  else if(name == "SPACE") return K_SPACE;
  else if(name == "BACKSPACE") return K_BACKSPACE;
  else if(name == "CAPSLOCK") return K_CAPSLOCK;
  else if(name == "KP_UPARROW") return K_KP_UPARROW;
  else if(name == "KP_DOWNARROW") return K_KP_DOWNARROW;
  else if(name == "KP_LEFTARROW") return K_KP_LEFTARROW;
  else if(name == "KP_RIGHTARROW") return K_KP_RIGHTARROW;
  else if(name == "UPARROW") return K_UPARROW;
  else if(name == "DOWNARROW") return K_DOWNARROW;
  else if(name == "LEFTARROW") return K_LEFTARROW;
  else if(name == "RIGHTARROW") return K_RIGHTARROW;
  else if(name == "ALT") return K_ALT;
  else if(name == "CTRL") return K_CTRL;
  else if(name == "SHIFT") return K_SHIFT;
  else if(name == "KP_INS") return K_KP_INS;
  else if(name == "KP_DEL") return K_KP_DEL;
  else if(name == "KP_PGUP") return K_KP_PGUP;
  else if(name == "KP_PGDN") return K_KP_PGDN;
  else if(name == "KP_HOME") return K_KP_HOME;
  else if(name == "KP_END") return K_KP_END;
  else if(name == "INS") return K_INS;
  else if(name == "DEL") return K_DEL;
  else if(name == "PGUP") return K_PGUP;
  else if(name == "PGDN") return K_PGDN;
  else if(name == "HOME") return K_HOME;
  else if(name == "END") return K_END;
  else if(name == "F1") return K_F1;
  else if(name == "F2") return K_F2;
  else if(name == "F3") return K_F3;
  else if(name == "F4") return K_F4;
  else if(name == "F5") return K_F5;
  else if(name == "F6") return K_F6;
  else if(name == "F7") return K_F7;
  else if(name == "F8") return K_F8;
  else if(name == "F9") return K_F9;
  else if(name == "F10") return K_F10;
  else if(name == "F11") return K_F11;
  else if(name == "F12") return K_F12;
  else if(name == "F13") return K_F13;
  else if(name == "F14") return K_F14;
  else if(name == "F15") return K_F15;
  else if(name == "PAUSE") return K_PAUSE;
  else if(name == "KP_STAR") return K_KP_STAR;
  else if(name == "MOUSE1") return K_MOUSE1;
  else if(name == "MOUSE2") return K_MOUSE2;
  else if(name == "MOUSE3") return K_MOUSE3;
  else if(name == "MOUSE4") return K_MOUSE4;
  else if(name == "MOUSE5") return K_MOUSE5;
  else if(name == "MWHEELUP") return K_MWHEELUP;
  else if(name == "MWHEELDOWN") return K_MWHEELDOWN;
  else if(name == "K_JOY1") return K_JOY1;
  else if(name == "K_JOY2") return K_JOY2;
  else if(name == "K_JOY3") return K_JOY3;
  else if(name == "K_JOY4") return K_JOY4;
  else if(name == "K_JOY5") return K_JOY5;
  else if(name == "K_JOY6") return K_JOY6;
  else if(name == "K_JOY7") return K_JOY7;
  else if(name == "K_JOY8") return K_JOY8;
  else if(name == "K_JOY9") return K_JOY9;
  else if(name == "K_JOY10") return K_JOY10;
  else if(name == "K_JOY11") return K_JOY11;
  else if(name == "K_JOY12") return K_JOY12;
  else if(name == "K_JOY13") return K_JOY13;
  else if(name == "K_JOY14") return K_JOY14;
  else if(name == "K_JOY15") return K_JOY15;
  else if(name == "K_JOY16") return K_JOY16;
  else if(name == "K_JOY17") return K_JOY17;
  else if(name == "K_JOY18") return K_JOY18;
  else if(name == "K_JOY19") return K_JOY19;
  else if(name == "K_JOY20") return K_JOY20;
  else if(name == "K_JOY21") return K_JOY21;
  else if(name == "K_JOY22") return K_JOY22;
  else if(name == "K_JOY23") return K_JOY23;
  else if(name == "K_JOY24") return K_JOY24;
  else if(name == "K_JOY25") return K_JOY25;
  else if(name == "K_JOY26") return K_JOY26;
  else if(name == "K_JOY27") return K_JOY27;
  else if(name == "K_JOY28") return K_JOY28;
  else if(name == "K_JOY29") return K_JOY29;
  else if(name == "K_JOY30") return K_JOY30;
  else if(name == "K_JOY31") return K_JOY31;
  else if(name == "K_JOY32") return K_JOY32;
  else if(name == "K_AUX1") return K_AUX1;
  else if(name == "K_AUX2") return K_AUX2;
  else if(name == "K_AUX3") return K_AUX3;
  else if(name == "K_AUX4") return K_AUX4;
  else if(name == "K_AUX5") return K_AUX5;
  else if(name == "K_AUX6") return K_AUX6;
  else if(name == "K_AUX7") return K_AUX7;
  else if(name == "K_AUX8") return K_AUX8;
  else if(name == "K_AUX9") return K_AUX9;
  else if(name == "K_AUX10") return K_AUX10;
  else if(name == "K_AUX11") return K_AUX11;
  else if(name == "K_AUX12") return K_AUX12;
  else if(name == "K_AUX13") return K_AUX13;
  else if(name == "K_AUX14") return K_AUX14;
  else if(name == "K_AUX15") return K_AUX15;
  else if(name == "K_AUX16") return K_AUX16;
  else if(name == "K_JOY2_1") return K_JOY2_1;
  else if(name == "K_JOY2_2") return K_JOY2_2;
  else if(name == "K_JOY2_3") return K_JOY2_3;
  else if(name == "K_JOY2_4") return K_JOY2_4;
  else if(name == "K_JOY2_5") return K_JOY2_5;
  else if(name == "K_JOY2_6") return K_JOY2_6;
  else if(name == "K_JOY2_7") return K_JOY2_7;
  else if(name == "K_JOY2_8") return K_JOY2_8;
  else if(name == "K_JOY2_9") return K_JOY2_9;
  else if(name == "K_JOY2_10") return K_JOY2_10;
  else if(name == "K_JOY2_11") return K_JOY2_11;
  else if(name == "K_JOY2_12") return K_JOY2_12;
  else if(name == "K_JOY2_13") return K_JOY2_13;
  else if(name == "K_JOY2_14") return K_JOY2_14;
  else if(name == "K_JOY2_15") return K_JOY2_15;
  else if(name == "K_JOY2_16") return K_JOY2_16;
  else if(name == "K_JOY2_17") return K_JOY2_17;
  else if(name == "K_JOY2_18") return K_JOY2_18;
  else if(name == "K_JOY2_19") return K_JOY2_19;
  else if(name == "K_JOY2_20") return K_JOY2_20;
  else if(name == "K_JOY2_21") return K_JOY2_21;
  else if(name == "K_JOY2_22") return K_JOY2_22;
  else if(name == "K_JOY2_23") return K_JOY2_23;
  else if(name == "K_JOY2_24") return K_JOY2_24;
  else if(name == "K_JOY2_25") return K_JOY2_25;
  else if(name == "K_JOY2_26") return K_JOY2_26;
  else if(name == "K_JOY2_27") return K_JOY2_27;
  else if(name == "K_JOY2_28") return K_JOY2_28;
  else if(name == "K_JOY2_29") return K_JOY2_29;
  else if(name == "K_JOY2_30") return K_JOY2_30;
  else if(name == "K_JOY2_31") return K_JOY2_31;
  else if(name == "K_JOY2_32") return K_JOY2_32;
  else if(name == "K_JOY3_1") return K_JOY3_1;
  else if(name == "K_JOY3_2") return K_JOY3_2;
  else if(name == "K_JOY3_3") return K_JOY3_3;
  else if(name == "K_JOY3_4") return K_JOY3_4;
  else if(name == "K_JOY3_5") return K_JOY3_5;
  else if(name == "K_JOY3_6") return K_JOY3_6;
  else if(name == "K_JOY3_7") return K_JOY3_7;
  else if(name == "K_JOY3_8") return K_JOY3_8;
  else if(name == "K_JOY3_9") return K_JOY3_9;
  else if(name == "K_JOY3_10") return K_JOY3_10;
  else if(name == "K_JOY3_11") return K_JOY3_11;
  else if(name == "K_JOY3_12") return K_JOY3_12;
  else if(name == "K_JOY3_13") return K_JOY3_13;
  else if(name == "K_JOY3_14") return K_JOY3_14;
  else if(name == "K_JOY3_15") return K_JOY3_15;
  else if(name == "K_JOY3_16") return K_JOY3_16;
  else if(name == "K_JOY3_17") return K_JOY3_17;
  else if(name == "K_JOY3_18") return K_JOY3_18;
  else if(name == "K_JOY3_19") return K_JOY3_19;
  else if(name == "K_JOY3_20") return K_JOY3_20;
  else if(name == "K_JOY3_21") return K_JOY3_21;
  else if(name == "K_JOY3_22") return K_JOY3_22;
  else if(name == "K_JOY3_23") return K_JOY3_23;
  else if(name == "K_JOY3_24") return K_JOY3_24;
  else if(name == "K_JOY3_25") return K_JOY3_25;
  else if(name == "K_JOY3_26") return K_JOY3_26;
  else if(name == "K_JOY3_27") return K_JOY3_27;
  else if(name == "K_JOY3_28") return K_JOY3_28;
  else if(name == "K_JOY3_29") return K_JOY3_29;
  else if(name == "K_JOY3_30") return K_JOY3_30;
  else if(name == "K_JOY3_31") return K_JOY3_31;
  else if(name == "K_JOY3_32") return K_JOY3_32;
  else if(name == "K_JOY4_1") return K_JOY4_1;
  else if(name == "K_JOY4_2") return K_JOY4_2;
  else if(name == "K_JOY4_3") return K_JOY4_3;
  else if(name == "K_JOY4_4") return K_JOY4_4;
  else if(name == "K_JOY4_5") return K_JOY4_5;
  else if(name == "K_JOY4_6") return K_JOY4_6;
  else if(name == "K_JOY4_7") return K_JOY4_7;
  else if(name == "K_JOY4_8") return K_JOY4_8;
  else if(name == "K_JOY4_9") return K_JOY4_9;
  else if(name == "K_JOY4_10") return K_JOY4_10;
  else if(name == "K_JOY4_11") return K_JOY4_11;
  else if(name == "K_JOY4_12") return K_JOY4_12;
  else if(name == "K_JOY4_13") return K_JOY4_13;
  else if(name == "K_JOY4_14") return K_JOY4_14;
  else if(name == "K_JOY4_15") return K_JOY4_15;
  else if(name == "K_JOY4_16") return K_JOY4_16;
  else if(name == "K_JOY4_17") return K_JOY4_17;
  else if(name == "K_JOY4_18") return K_JOY4_18;
  else if(name == "K_JOY4_19") return K_JOY4_19;
  else if(name == "K_JOY4_20") return K_JOY4_20;
  else if(name == "K_JOY4_21") return K_JOY4_21;
  else if(name == "K_JOY4_22") return K_JOY4_22;
  else if(name == "K_JOY4_23") return K_JOY4_23;
  else if(name == "K_JOY4_24") return K_JOY4_24;
  else if(name == "K_JOY4_25") return K_JOY4_25;
  else if(name == "K_JOY4_26") return K_JOY4_26;
  else if(name == "K_JOY4_27") return K_JOY4_27;
  else if(name == "K_JOY4_28") return K_JOY4_28;
  else if(name == "K_JOY4_29") return K_JOY4_29;
  else if(name == "K_JOY4_30") return K_JOY4_30;
  else if(name == "K_JOY4_31") return K_JOY4_31;
  else if(name == "K_JOY4_32") return K_JOY4_32;
  else if(name.length() == 1) return name[0];
  else
    return 0;
}

void Input::charReceived(char letter)
{
  if(_keyHandler)
    _keyHandler(letter | K_CHAR_FLAG, true);
}

void Input::keyPressed(int keySym)
{
  if(activeKeyCount == 16)
    --activeKeyCount;

  activeKeys[activeKeyCount++] = keySym;

  std::map<int, String>::const_iterator binding;

  int keyFlags = 0;

  if(isPressed(K_CTRL))
    keyFlags |= K_CTRL_FLAG;

  if(isPressed(K_ALT))
    keyFlags |= K_ALT_FLAG;

  binding = keymap.find(keySym | keyFlags);

  if(binding == keymap.end())
    binding = keymap.find(keySym);

  if(binding != keymap.end())
  {
    if(!_keyHandler
    || binding->second == "toggleconsole")
    {
      StringList commands = StringList::split(";", binding->second);

      for(StringList::iterator i = commands.begin(); i != commands.end(); ++i)
        API::appendCommand(*i);

      return;
    }
  }

  if(_keyHandler)
    _keyHandler(keySym, true);
}

void Input::keyReleased(int keySym)
{
  for(uint i = 0; i < activeKeyCount; ++i)
  {
    if(activeKeys[i] == keySym)
    {
      --activeKeyCount;

      for(; i < activeKeyCount; ++i)
        activeKeys[i] = activeKeys[i + 1];

      break;
    }
  }

  if(!_keyHandler)
  {
    std::map<int, String>::const_iterator binding = keymap.find(keySym);

    if((binding != keymap.end())
    && (binding->second.find(";") == String::notfound))
    {
      if(binding->second[0] == '+')
        API::appendCommand(String("-") + binding->second.right(binding->second.length() - 1));
    }

    return;
  }

  _keyHandler(keySym, 0);
}

void Input::mouseMoved(int deltaX, int deltaY)
{
  if(_mouseHandler)
    _mouseHandler(deltaX, deltaY);

  std::map<int, String>::const_iterator binding = keymap.find(E_MOUSE_MOVED);

  if(binding != keymap.end())
  {
    String command = binding->second;

    command.replace("%1", String::number(deltaX))
           .replace("%2", String::number(deltaY));

    API::appendCommand(command);
  }
}

void Input::joystickMoved(int joystick, int axis, int value)
{
  if(_joystickHandler)
    _joystickHandler(joystick, axis, value);

  std::map<int, String>::const_iterator binding = keymap.find(E_JOYSTICK_MOVED);

  if(binding != keymap.end())
  {
    String command = binding->second;

    command.replace("%1", String::number(joystick))
           .replace("%2", String::number(axis))
           .replace("%3", String::number(value));

    API::appendCommand(command);
  }
}

bool Input::isPressed(int keySym)
{
  for(uint i = 0; i < activeKeyCount; ++i)
    if(activeKeys[i] == keySym)
      return true;

  return false;
}

// vim: ts=2 sw=2 et
