/***************************************************************************
                     api_commands.cc  -  Console commands
                               -------------------
      copyright            : (C) 2003 by Morten Hustveit, Carl Henrik Lunde
      email                : morten@debian.org, chlunde@ifi.uio.no
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <map>

#include <espace/api.h>
#include <espace/console.h>
#include <espace/cvar.h>
#include <espace/file.h>
#include <espace/input.h>
#include <espace/map.h>
#include <espace/network.h>
#include <espace/opengl.h>
#include <espace/output.h>
#include <espace/sound.h>
#include <espace/string.h>
#include <espace/system.h>

#include "api_commands.h"

namespace APICommands
{
  void unbindall()
  {
    Input::unbindAll();
  }

  void unbind()
  {
    if(API::argc() < 2)
    {
      esInfo << "Usage: unbind <keycode>" << std::endl;

      return;
    }

    if(int key = Input::number(API::argv(1)))
      Input::unbind(key);
  }

  void bind()
  {
    if(API::argc() < 3)
    {
      esInfo << "Usage: bind <keycode> <command>" << std::endl;

      return;
    }

    String key = API::argv(1);

    int keyFlags = 0;

    if(key.beginsWith("CTRL+"))
    {
      key = key.right(key.length() - 5);
      keyFlags |= Input::K_CTRL_FLAG;
    }

    if(key.beginsWith("ALT+"))
    {
      key = key.right(key.length() - 4);
      keyFlags |= Input::K_ALT_FLAG;
    }

    int keyNum = Input::number(key);

    if(!keyNum)
    {
      esWarning << "bind: Couldn't find key code for \""
              << key << "\"." << std::endl;

      return;
    }

    String binding = API::argv(2);

    for(uint i = 3; i < API::argc(); ++i)
      binding += String(" ") + API::argv(i);

    if(binding[0] == '"' && binding[binding.length() - 1] == '"')
      binding = binding.mid(1, binding.length() - 2);

    Input::bind(keyNum | keyFlags, binding);
  }

  void set()
  {
    if(API::argc() < 3)
    {
      esInfo << "Usage: " << API::argv(0) << " <cvar> <value>" << std::endl;

      return;
    }

    int flag = 0;

    switch(API::argv(0)[3])
    {
      case 'a': flag = CVar::Archive; break;
      case 's': flag = CVar::SystemInfo; break;
      case 'u': flag = CVar::UserInfo; break;
    }

    String value;

    for(uint i = 2; i < API::argc(); ++i)
    {
      if(i != 2)
        value += " ";

      value += API::argv(i);
    }

    if(value[0] == '\"' && value[value.length() - 1] == '\"')
      value = value.mid(1, value.length() - 2);

    CVar cVar = CVar::acquire(API::argv(1), "", flag);

    cVar = value;

    cVar.commit();
  }

  void toggle()
  {
    if(API::argc() != 2)
    {
      esInfo << "Usage: toggle <cvar>" << std::endl;

      return;
    }

    CVar value = CVar::acquire(API::argv(1));

    value = value.integer ? 0 : 1;

    value.commit();
  }

  void vstr()
  {
    if(API::argc() != 2)
    {
      esInfo << "Usage: vstr <cvar>" << std::endl;

      return;
    }

    String command = CVar::getString(API::argv(1));

    if(command.isNull())
      return;

    API::executeCommand(command);
  }

  void cmdlist()
  {
    for(uint i = 0; i < Console::commandCount(); ++i)
    {
      const String& command = Console::command(i);

      if(command[0] != '+' && command[0] != '-')
        esInfo << command << std::endl;
    }
  }

  void connect()
  {
    if(API::argc() != 2)
    {
      esInfo << "Usage: connect <host>" << std::endl;

      return;
    }

    Network::connect(API::argv(1));
  }

  void disconnect()
  {
    Network::disconnect();
  }

  void bindlist()
  {
    for(int i = 0; i < Input::K_LAST_KEY; ++i)
    {
      const char* binding = Input::binding(i);

      if(!binding)
        continue;

      esInfo << Input::name(i) << " \"" << binding << "\"" << std::endl;
    }
  }

  void exec()
  {
    if(API::argc() != 2)
    {
      esInfo << "Usage: exec <scriptname>" << std::endl;

      return;
    }

    File file(API::argv(1));

    if(!file.isOpen())
    {
      esWarning << "exec: Failed to open \"" << API::argv(1) << "\"." << std::endl;

      return;
    }

    while(!file.eof())
    {
      String line = file.readLine();

      int comment = line.find("//");

      if(comment != -1)
        line = line.left(comment);

      line = line.stripWhiteSpace();

      if(line.isEmpty())
        continue;

      API::executeCommand(line);
    }
  }

  void echo()
  {
    for(uint i = 1; i < API::argc(); ++i)
    {
      if(i != 1)
        esInfo << " ";

      esInfo << API::argv(i);
    }

    esInfo << std::endl;
  }

  void quit()
  {
    System::exit();
  }
} // namespace APICommands

// vim: ts=2 sw=2 et
