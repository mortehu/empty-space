/***************************************************************************
                           output.cc  -  Classes for human readable output
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

#include <iostream>

#include <espace/console.h>
#include <espace/cvar.h>
#include <espace/matrix.h>
#include <espace/output.h>
#include <espace/vector.h>

class Stream : public std::streambuf
{
public:

  Stream(const char* prefix)
    : prefix(prefix),
      lineStart(true)
  {
    setp(0, 0);
  }

protected:

  int overflow(int c = EOF);

  const char* prefix;
  bool lineStart;
};

class NullStream : public std::streambuf
{
public:

  NullStream()
  {
    setp(0, 0);
  }

protected:

  int overflow(int c = EOF);
};

std::ostream esError(new Stream("Error: "));
std::ostream esWarning(new Stream("Warning: "));
std::ostream esInfo(new Stream(""));

namespace
{
  std::ostream debug(new Stream("Debug: "));
  std::ostream null(new NullStream);
}

std::ostream& esDebug(int level)
{
  if(level > CVar::getInt("developer"))
    return null;

  return debug;
}

int Stream::overflow(int c)
{
  if(lineStart)
  {
    lineStart = false;

    for(uint i = 0; i < strlen(prefix); ++i)
      overflow(prefix[i]);
  }

  if(c == '\n')
    lineStart = true;

  std::cerr.put(c);
  Console::put(c);

  return 0;
}

int NullStream::overflow(int)
{
  return 0;
}

std::ostream& operator<<(std::ostream& stream, const Vector2& vector)
{
  return stream << vector(0) << " " << vector(1);
}

std::ostream& operator<<(std::ostream& stream, const Vector3& vector)
{
  return stream << vector(0) << " " << vector(1) << " " << vector(2);
}

std::ostream& operator<<(std::ostream& stream, const Matrix3x3& m)
{
  return stream << m(0, 0) << " " << m(1, 0) << " " << m(2, 0) << std::endl
                << m(0, 1) << " " << m(1, 1) << " " << m(2, 1) << std::endl
                << m(0, 2) << " " << m(1, 2) << " " << m(2, 2) << std::endl;
}

std::ostream& operator<<(std::ostream& stream, const Matrix4x4& m)
{
  return stream
    << m(0, 0) << " " << m(1, 0) << " " << m(2, 0) << " " << m(3, 0) << std::endl
    << m(0, 1) << " " << m(1, 1) << " " << m(2, 1) << " " << m(3, 1) << std::endl
    << m(0, 2) << " " << m(1, 2) << " " << m(2, 2) << " " << m(3, 2) << std::endl
    << m(0, 3) << " " << m(1, 3) << " " << m(2, 3) << " " << m(3, 3) << std::endl;
}

// vim: ts=2 sw=2 et
