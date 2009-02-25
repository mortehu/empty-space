/***************************************************************************
                          console.cc  -  Engine console UI
                               -------------------
      copyright            : (C) 2003 by Carl Henrik Lunde
      email                : chlunde@ifi.uio.no
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <algorithm>
#include <deque>
#include <set>
#include <vector>

#include <math.h>

#include <espace/api.h>
#include <espace/color.h>
#include <espace/console.h>
#include <espace/font.h>
#include <espace/input.h>
#include <espace/opengl.h>
#include <espace/predicates.h>
#include <espace/renderer.h>
#include <espace/shader.h>
#include <espace/string.h>
#include <espace/system.h>

namespace
{
  typedef std::set<String, LessThanCaseInsensitive> CommandList;

  const Color colorTable[] =
  {
    Color(0,     0,   0), // Black
    Color(255,   0,   0), // Red
    Color(0,   255,   0), // Green
    Color(255, 255,   0), // Yellow
    Color(0,     0, 255), // Blue
    Color(0,   255, 255), // Cyan
    Color(255,   0, 127), // Magenta
    Color(255, 255, 255), // White
  };

  const uint         colorTableSize = sizeof(colorTable) / sizeof(Color);

  const uint         margin = 3;
  const float        animationLength = 0.5;
  const char         cursor = '_';

  Shader*            backgroundShader[2];
  Shader*            borderShader;

  Font*              font = 0;
  int                lineHeight;

  float              x;
  float              y;

  bool               initialized = false;

  double             toggleTime = 0.0;

  CommandList        commands;
  std::deque<String> buffer;
  std::deque<String> commandHistory;
  String             inputBuffer;
  const char*        prompt = "# ";
  String             commandLine = "";
  uint               page;
  uint               cursorPosition;
  uint               commandHistoryPosition;
  uint               textColor = 7;

  bool               toggled = false;

  Input::KeyHandler  oldKeyHandler = 0;

  String             fontName = "fonts/fontImage_12.dat";
  String             background = "console-16bit";
  String             logo = "console2-16bit";
}

void Console::configure(const char* font, const char* _background,
                        const char* _logo)
{
  fontName = font;
  background = _background;
  logo = _logo;
}

void Console::initialize()
{
  font = Font::acquire(fontName);

  for(uint character = 0; character < 256; ++character)
    lineHeight = std::max(font->glyphs[character].height, lineHeight);

  borderShader = Shader::acquire("simple:*white");

  // RtCW MP shaders
  backgroundShader[0] = Shader::acquire(background);

  if(logo)
    backgroundShader[1] = Shader::acquire(logo);

  initialized = true;
}

void Console::toggle()
{
  if(toggled = !toggled)
  {
    if(!initialized)
      initialize();

    oldKeyHandler = Input::keyHandler();
    Input::setKeyHandler(keyHandler);
  }
  else
  {
    Input::setKeyHandler(oldKeyHandler);
  }

  float time = System::time();

  if((time - toggleTime) < animationLength)
    toggleTime = time + (time - toggleTime) - animationLength;
  else
    toggleTime = time;
}

void Console::clear()
{
  buffer.clear();
  commandHistory.clear();
}

void Console::put(int character)
{
  inputBuffer += character;

  if(inputBuffer.length() > 1 && character == '\n')
  {
    buffer.push_front(inputBuffer);
    inputBuffer = "";
  }
}

void Console::put(const String& text)
{
  inputBuffer += text;

  int position;

  while(String::notfound != (position = inputBuffer.find("\n")))
  {
    const unsigned int bufferSize = 500;
    if(buffer.size() > bufferSize)
      buffer.pop_back();

    buffer.push_front(inputBuffer.left(position + 1));

    if(position < static_cast<int>(inputBuffer.length()))
      inputBuffer = inputBuffer.right(inputBuffer.length() - position - 1);
    else
      inputBuffer = "";
  }
}

void Console::keyHandler(int key, bool keyPressed)
{
  if(!keyPressed)
    return;

  switch(key)
  {
  case Input::K_PGUP:
  case Input::K_KP_PGUP:

    ++page;

    break;

  case Input::K_PGDN:
  case Input::K_KP_PGDN:

    if(page)
      --page;

    break;

  default:

    page = 0;
  }

  switch(key)
  {
  case Input::K_LEFTARROW:

    if(cursorPosition)
      --cursorPosition;

    break;

  case Input::K_RIGHTARROW:

    ++cursorPosition;

    break;
  }

  switch(key)
  {
  case Input::K_DOWNARROW:

    if(commandHistoryPosition)
      --commandHistoryPosition;

    break;

  case Input::K_UPARROW:

    ++commandHistoryPosition;

    break;

  default:

    commandHistoryPosition = 0;
  }

  switch(key)
  {
  case Input::K_TAB:

    {
      put(prompt);
      put(commandLine + "\n");

      std::vector<String> matches;
      String userInput = commandLine.left(cursorPosition).toLower();

      for(CommandList::const_iterator i = commands.lower_bound(userInput);
          i != commands.end(); ++i)
      {
        if(!i->beginsWith(userInput))
          break;

        matches.push_back(*i);
      }

      if(matches.size() > 1)
      {
        for(unsigned int i = 0; i < matches.size(); ++i)
          put(matches[i] + "\n");

        while(matches[0][commandLine.length()]
              == matches[matches.size() - 1][commandLine.length()])
          commandLine += matches[0][commandLine.length()];
      }
      else if(matches.size() == 1)
      {
        commandLine = matches[0] + " ";
      }

      cursorPosition = commandLine.length();
    }

    break;

  case Input::K_BACKSPACE:

    if(cursorPosition)
    {
      commandLine = commandLine.left(cursorPosition - 1)
                  + commandLine.right(commandLine.length() - cursorPosition);
      --cursorPosition;
    }

    break;

  case Input::K_ENTER:

    put(prompt);
    put(commandLine + "\n");

    commandHistory.push_front(commandLine);

    commandLine = "";

    if(commandHistory.front().length())
      API::executeCommand(commandHistory.front());

    break;

  default:

    if((key & Input::K_CHAR_FLAG)
    && isprint(key & ~Input::K_CHAR_FLAG))
    {
      String newCommandLine = commandLine.left(cursorPosition);
      newCommandLine += static_cast<unsigned char>(key & ~Input::K_CHAR_FLAG);
      newCommandLine += commandLine.right(commandLine.length()
                                          - cursorPosition);
      commandLine = newCommandLine;
      ++cursorPosition;
    }
  }

  if(commandHistoryPosition > commandHistory.size())
    commandHistoryPosition = commandHistory.size();

  if(commandHistoryPosition)
  {
    commandLine = commandHistory[commandHistoryPosition - 1];
    cursorPosition = commandLine.length();
  }

  if(cursorPosition > commandLine.length())
    cursorPosition = commandLine.length();
}

void Console::print(int character)
{
  if(character == '\n')
  {
    x = margin;
    y -= lineHeight;
    textColor = 7;

    return;
  }

  Font::Glyph& glyph = font->glyphs[character];

  Shader* shader = Shader::shaderForHandle(glyph.shaderHandle);

  if(shader)
  {
    Renderer::drawQuad2D(
      x, y - glyph.top,
      glyph.imageWidth,
      glyph.imageHeight,
      glyph.s1, glyph.t1,
      glyph.s2, glyph.t2,
      shader);
  }

  x += glyph.xSkip;
}

void Console::print(const char* str)
{
  Renderer::setColor(colorTable[textColor]);

  while(*str)
  {
    if(*str == '^')
    {
      if(!*++str)
        break;

      textColor = (*str - '0') % colorTableSize;

      Renderer::setColor(colorTable[textColor]);
    }
    else
    {
      print(*str);
    }

    ++str;
  }
}

void Console::render()
{
  if(toggled && Input::keyHandler() != keyHandler)
    toggled = false;

  float progress = (System::time() - toggleTime) / animationLength;

  if(progress > 1)
    progress = 1;

  if(!toggled)
    progress = 1 - progress;

  if(!progress)
    return;

  if(!backgroundShader[0])
    return;

  const int height = GL::config.height / 2;
  y = progress * height - height;

  Renderer::setColor(Color(255, 255, 255));

  // RtCW shaders
  // Background
  Renderer::drawQuad2D(0, y,
                       GL::config.width, height - 2,
                       0, 0,
                       0.5, 0.5,
                       backgroundShader[0]);

  if(backgroundShader[1])
  {
    // (Almost) centered logo
    Renderer::drawQuad2D(GL::config.width * ((1 - 0.4) / 2), y + height * 0.3,
                         GL::config.width * 0.4, height * 0.5,
                         0, 0,
                         1, 1,
                         backgroundShader[1]);
  }

  Renderer::setColor(Color(0.0, 0.0, 0.0, 0.5));

  Renderer::drawQuad2D(0, y + height - 2,
                       GL::config.width, 2,
                       0, 0,
                       1, 1,
                       borderShader);

  x = margin;
  y += height - margin;

  if(!page)
  {
    print(prompt);
    print(commandLine.left(cursorPosition));

    if(fmod(System::time(), 1.0) < 0.7)
      Renderer::setColor(Color(0, 0, 0, 0));

    print(cursor);

    print(commandLine.right(commandLine.length() - cursorPosition));

    print('\n');
  }

  const int pageSize = 10;
  std::deque<String>::iterator line = buffer.begin() + std::min(buffer.size(), page * pageSize);

  for(;y > 0 && line != buffer.end(); ++line)
    print(*line);
}

void Console::addCommand(const String& command)
{
  commands.insert(command);
}

void Console::removeCommand(const String& command)
{
  commands.erase(command);
}

uint Console::commandCount()
{
  return commands.size();
}

const String& Console::command(uint index)
{
  CommandList::const_iterator i = commands.begin();

  std::advance(i, index);

  return *i;
}

// vim: ts=2 sw=2 et
