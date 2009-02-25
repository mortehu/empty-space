/***************************************************************************
                       stringlist.cc  -  String list
                               -------------------
      copyright            : (C) 2003 by Carl Henrik Lunde, Morten Hustveit
      email                : chlunde@ifi.uio.no, morten@debian.org
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <espace/stringlist.h>

static const char* strof(const char* haystack, const char* needle)
{
  while(*haystack)
  {
    if(strchr(needle, *haystack))
      return haystack;

    ++haystack;
  }

  return 0;
}

StringList StringList::split(const char* needle, const String& string)
{
  StringList ret;

  const char* begin = string;
  const char* end;

  while(0 != (end = strof(begin, needle)))
  {
    ret.push_back(string.mid(begin - string, end - begin));

    begin = end;

    while(*begin && strchr(needle, *begin))
      ++begin;
  }

  if(*begin)
    ret.push_back(begin);

  return ret;
}

// vim: ts=2 sw=2 et
