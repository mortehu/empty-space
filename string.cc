/***************************************************************************
                           string.cc  -  String handler
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

#include <ctype.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#include <espace/string.h>
#include <espace/types.h>

String String::null;

String String::operator+(const char* str) const
{
  uint length = strlen(str);

  String ret;

  ret.data->allocate(data->length + length);

  memcpy(ret.data->data, data->data, data->length);
  memcpy(ret.data->data + data->length, str, length);

  return ret;
}

String String::operator+(const String& str) const
{
  String ret;

  ret.data->allocate(data->length + str.data->length);

  memcpy(ret.data->data, data->data, data->length);
  memcpy(ret.data->data + data->length, str.data->data, str.data->length);

  return ret;
}

String& String::operator+=(char c)
{
  detach();

  data->reallocate(data->length + 1);
  data->data[data->length - 1] = c;

  return *this;
}

String& String::operator+=(const char* str)
{
  detach();

  uint length = strlen(str);
  uint oldlength = data->length;

  data->reallocate(oldlength + length);

  memcpy(data->data + oldlength, str, length);

  return *this;
}

String String::stripWhiteSpace() const
{
  uint begin = 0;

  while(isspace(data->data[begin]))
    ++begin;

  String ret(data->data + begin);

  while(ret.data->length && isspace(ret.data->data[ret.data->length - 1]))
    ret.data->data[ret.data->length--] = 0;

  return ret;
}

String& String::replace(char before, char after)
{
  char* c = data->data;

  while(0 != (c = strchr(c, before)))
    *c = after;

  return *this;
}

String& String::replace(const String& before, const String& after)
{
  if(!before.length())
    return *this;

  uint i = 0;

  while(i <= length() - before.length())
  {
    if(!memcmp(data->data + i, before, before.length()))
    {
      *this = left(i) + after + right(length() - before.length() - i);

      i += after.length();
    }
    else
    {
      ++i;
    }
  }

  return *this;
}

int String::find(const char* str) const
{
  char* c = strstr(data->data, str);

  if(!c)
    return -1;

  return c - data->data;
}

bool String::contains(const char* str, bool cs) const
{
  if(cs)
  {
    return strstr(data->data, str);
  }
  else // !cs
  {
    uint length = strlen(str);

    if(data->length < length)
      return false;

    for(uint i = 0; i <= data->length - length; ++i)
    {
      uint j;

      for(j = 0; j < length && tolower(data->data[i + j]) == tolower(str[j]); ++j);

      if(j == length)
        return true;
    }

    return false;
  }
}

String String::left(uint length) const
{
  if(length >= data->length)
    return *this;

  String ret;

  ret.data->allocate(length);

  memcpy(ret.data->data, data->data, length);

  return ret;
}

String String::right(uint length) const
{
  if(length >= data->length)
    return *this;

  String ret;

  ret.data->allocate(length);

  memcpy(ret.data->data, data->data + (data->length - length), length);

  return ret;
}

String String::mid(uint p, uint n) const
{
  uint pos = (p > data->length) ? data->length : p;
  uint length = (pos + n > data->length) ? (data->length - pos) : n;

  String ret;

  ret.data->allocate(length);

  memcpy(ret.data->data, data->data + pos, length);

  return ret;
}

String String::lower() const
{
  String ret;

  ret.data->allocate(data->length);

  for(uint i = 0; i < data->length; ++i)
    ret.data->data[i] = tolower(data->data[i]);

  return ret;
}

String String::upper() const
{
  String ret;

  ret.data->allocate(data->length);

  for(uint i = 0; i < data->length; ++i)
    ret.data->data[i] = toupper(data->data[i]);

  return ret;
}

String& String::toLower()
{
  for(uint i = 0; i < data->length; ++i)
    data->data[i] = tolower(data->data[i]);

  return *this;
}

String& String::toUpper()
{
  for(uint i = 0; i < data->length; ++i)
    data->data[i] = toupper(data->data[i]);

  return *this;
}

int String::toInt(bool* ok) const
{
  int ret = 0;

  char* c = data->data;

  if(*c == '-')
  {
    ++c;

    while(*c >= '0' && *c <= '9')
      (ret *= 10) += (*c++ - '0');

    if(ok)
      *ok = !*c;

    return -ret;
  }
  else
  {
    while(*c >= '0' && *c <= '9')
      (ret *= 10) += (*c++ - '0');

    if(ok)
      *ok = !*c;

    return ret;
  }
}

uint String::toUInt(bool* ok) const
{
  uint ret = 0;
  char* c;

  for(c = data->data; *c >= '0' && *c <= '9'; ++c)
    (ret *= 10) += (*c - '0');

  if(ok)
    *ok = !*c;

  return ret;
}

float String::toFloat(bool* ok) const
{
  char* endptr;
  float ret = strtof(data->data, &endptr);

  if(ok)
    *ok = (endptr != data->data);

  return ret;
}

String String::number(int number)
{
  String ret;

  int length = static_cast<int>(((number > 0) ? log10(number) : (number < 0) ? (log10(-number) + 1) : 0) + 1);

  ret.data->allocate(length);

  if(number == 0)
  {
    ret.data->data[0] = '0';

    return ret;
  }

  int i = length - 1;

  if(number < 0)
  {
    ret.data->data[0] = '-';

    while(number)
    {
      ret.data->data[i--] = '0' - (number % 10);
      number /= 10;
    }
  }
  else
  {
    while(number)
    {
      ret.data->data[i--] = '0' + (number % 10);
      number /= 10;
    }
  }

  return ret;
}

String String::number(double number)
{
  char tmp[512];

  sprintf(tmp, "%g", number);

  return String(tmp);
}

bool String::beginsWith(const char* str) const
{
  const char* c = data->data;

  while(*str == *c && *c)
    ++str, ++c;

  return !*str;
}

bool String::endsWith(const char* str) const
{
  uint len = strlen(str);

  if(data->length < len)
    return false;

  const char* c = data->data + data->length - len;

  while(*str == *c && *c)
    ++str, ++c;

  return !*str;
}

// vim: ts=2 sw=2 et
