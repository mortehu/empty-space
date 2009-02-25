#ifndef STRINGLIST_H_
#define STRINGLIST_H_

#include <list>

#include "string.h"
#include "types.h"

/**
 * String list.
 * \author Morten Hustveit
 * \author Carl Henrik Holth Lunde
 */
class IMPORT StringList : public std::list<String>
{
public:

  /**
   * Split string.
   *
   * \param needle Delimiter.
   * \param string Haystack.
   * \return List of strings (std::list<String>)
   */
  static StringList split(const char* needle, const String& string);
};

#endif // !STRINGLIST_H

// vim: ts=2 sw=2 et
