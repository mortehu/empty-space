#ifndef PREDICATES_H_
#define PREDICATES_H_

#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#include "string.h"

#include <espace/output.h>
class EndsWith
{
private:

  size_t length;
  const char* needle;

public:

  EndsWith(const char* needle) : needle(needle) { length = strlen(needle); }

  bool operator()(const String& haystack)
  {
    if(haystack.length() < length)
      return false;

    return !strcmp(haystack + haystack.length() - length, needle);
  }
};

class StartsWith
{
private:

  size_t length;
  const char* needle;

public:

  StartsWith(const char* needle) : needle(needle) { length = strlen(needle); }

  bool operator()(const String& haystack)
  {
    if(haystack.length() < length)
      return false;

    return !memcmp(haystack, needle, length);
  }
};

class StartsEndsWith : private StartsWith, EndsWith
{
public:

  StartsEndsWith(const char* start, const char* end) : StartsWith(start), EndsWith(end) { }

  bool operator()(const String& haystack)
  {
    return StartsWith::operator()(haystack) && EndsWith::operator()(haystack);
  }
};

class Contains
{
private:

  const char* needle;

public:

  Contains(const char* needle) : needle(needle) { }

  bool operator()(const char* haystack)
  {
    return strstr(haystack, needle);
  }
};

class ContainsEndsWith : private Contains, EndsWith
{
public:

  ContainsEndsWith(const char* mid, const char* end) : Contains(mid), EndsWith(end)  { }

  bool operator()(const char* haystack)
  {
    return EndsWith::operator()(haystack) && Contains::operator()(haystack);
  }
};

class LessThanCaseInsensitive
{
public:

  bool operator()(const char* a, const char* b)
  {
    while(tolower(*a) == tolower(*b) && *a)
      ++a, ++b;

    return tolower(*a) < tolower(*b);
  }
};

#endif // !PREDICATES_H_

// vim: ts=2 sw=2 et
