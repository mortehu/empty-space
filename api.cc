/***************************************************************************
                              api.cc  -  VM system calls
                               -------------------
      copyright            : (C) 2003 by Morten Hustveit, Carl Henrik Lunde
      email                : morten@rashbox.org, chlunde@ifi.uio.no
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

#include <ctype.h>
#include <stdarg.h>
#include <stdlib.h>

#include <espace/api.h>
#include <espace/console.h>
#include <espace/cvar.h>
#include <espace/file.h>
#include <espace/output.h>
#include <espace/string.h>
#include <espace/system.h>

#include "api_commands.h"

using namespace APICommands;

namespace
{
  std::deque<String> commandQueue;
  std::deque<String> vmCommand;
  String             vmCommandArgs;

  typedef std::map<String, API::Command> CommandList;

  std::deque<API::CommandHandler> commandHandlers;
  API::ClientCommandHandler clientCommandHandler = 0;

  CommandList commands;
}

void API::initialize()
{
  setCommand("bind", bind);
  setCommand("bindlist", bindlist);
  setCommand("clear", Console::clear);
  setCommand("cmdlist", cmdlist);
  setCommand("connect", connect);
  setCommand("disconnect", disconnect);
  setCommand("echo", echo);
  setCommand("exec", exec);
  setCommand("print", echo);
  setCommand("quit", quit);
  setCommand("set", set);
  setCommand("seta", set);
  setCommand("sets", set);
  setCommand("setu", set);
  setCommand("toggle", toggle);
  setCommand("toggleconsole", Console::toggle);
  setCommand("unbind", unbind);
  setCommand("unbindall", unbindall);
  setCommand("vstr", vstr);
}

void API::setCommand(const char* name, Command command)
{
  commands[name] = command;

  Console::addCommand(name);
}

void API::addCommandHandler(CommandHandler handler)
{
  commandHandlers.push_back(handler);
}

void API::appendCommand(const char* command)
{
  commandQueue.push_back(command);
}

void API::prependCommand(const char* command)
{
  commandQueue.push_front(command);
}

void API::flushCommands()
{
  static uint nextFlush;

  if(nextFlush)
  {
    --nextFlush;

    if(nextFlush)
      return;
  }

  for(;;)
  {
    if(commandQueue.empty())
      return;

    String command = commandQueue.front().stripWhiteSpace();
    commandQueue.pop_front();

    if(command == "wait")
    {
      return;
    }
    else if(command.left(5) == "wait ")
    {
      bool ok;

      int count = command.right(command.length() - 5).toInt(&ok);

      if(!ok)
        esDebug() << "Unable to parse \"" << command << "\"." << std::endl;
      else
        nextFlush += count;

      return;
    }
    else
    {
      executeCommand(command);
    }
  }
}

void API::executeCommand(const char* _command)
{
  String command = _command;

  vmCommand.clear();

  // From preprocessor.cc

  uint begin = 0;

  while(begin < command.length())
  {
    while(isspace(command[begin]))
      ++begin;

    if(begin == command.length())
      break;

    uint end = begin;

    if(command[begin] == '"')
    {
      while(command[++end] != '"')
        if(end == command.length() - 1)
        {
          esDebug() << "Multi-line string constant." << std::endl;

          break;
        }

      ++end;
    }
    else
    {
      ++end;

      while(!isspace(command[end]))
        if(end++ == command.length())
          break;
    }

    vmCommand.push_back(command.mid(begin, end - begin));

    if(vmCommand.size() == 1)
      vmCommandArgs = command.right(command.length() - end - 1);

    begin = end;
  }

  if(vmCommand.empty())
    return;

  vmCommand[0] = vmCommand[0].toLower();

  CommandList::const_iterator i = commands.find(vmCommand[0]);

  if(i != commands.end())
  {
    i->second();

    return;
  }

  for(std::deque<CommandHandler>::iterator i = commandHandlers.begin();
      i != commandHandlers.end(); ++i)
  {
    if((*i)())
      return;
  }

  String value = CVar::getString(command);

  if(!value.isNull())
  {
    esInfo << value << std::endl;

    return;
  }

  esWarning << "Unknown command \"" << command << "\"." << std::endl;
}

void API::executeClientCommand(const char* _command, int clientNum)
{
  String command = _command;

  vmCommand.clear();

  // From preprocessor.cc

  uint begin = 0;

  while(begin < command.length())
  {
    while(isspace(command[begin]))
      ++begin;

    if(begin == command.length())
      break;

    uint end = begin;

    if(command[begin] == '"')
    {
      while(command[++end] != '"')
        if(end == command.length() - 1)
        {
          esDebug() << "Multi-line string constant." << std::endl;

          break;
        }

      ++end;
    }
    else
    {
      ++end;

      while(!isspace(command[end]))
        if(end++ == command.length())
          break;
    }

    vmCommand.push_back(command.mid(begin, end - begin));

    begin = end;
  }

  if(vmCommand.empty())
    return;

  if(!clientCommandHandler)
  {
    esWarning << "Client command received but not client command handler "
              << "set." << std::endl;

    // INFO: Set to gameMain(GAME_CLIENT_COMMAND, Client::clientNum)

    return;
  }

  // XXX: This must normally be transmitted over the network
  clientCommandHandler(clientNum);
}

uint API::argc()
{
  return vmCommand.size();
}

String API::argv(uint index)
{
  return vmCommand[index];
}

String API::args()
{
  return vmCommandArgs;
}

// vim: ts=2 sw=2 et
