/***************************************************************************
                     preprocessor.cc  -  Script preprocessor
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

#include <map>
#include <vector>
#include <ctype.h>

#include <espace/file.h>
#include <espace/output.h>
#include <espace/preprocessor.h>
#include <espace/string.h>
#include <espace/stringlist.h>

class Preprocessor::Internal
{
public:

  Internal()
    : token(0),
      eof(false),
      stack(0)
  {
  }

  String nextToken();

  File*                        input;
  std::vector<const char*>     tokens;
  uint                         token;
  StringList                   currentLine;
  bool                         eof;
  std::map<String, StringList> symbols;
  char*                        stack;
};

Preprocessor::Preprocessor(const char* fileName)
  : m(new Internal)
{
  m->input = new File(fileName);

  if(m->input->isOpen())
  {
    for(;;)
    {
      String string = m->nextToken();

      if(string.isNull())
        break;

      m->tokens.push_back(strdup(string));
    }
  }

  delete m->input;
}

Preprocessor::~Preprocessor()
{
  if(m->stack)
    delete [] m->stack;

  delete m;
}

bool Preprocessor::isOpen() const
{
  return m->tokens.size() || m->eof;
}

String Preprocessor::nextToken()
{
  if(m->token == m->tokens.size())
    return String::null;

  if(m->currentLine.empty())
  {
    do
    {
      m->currentLine.push_back(m->tokens[m->token++]);
    }
    while(m->currentLine.back() != "\n");
  }

  String ret = m->currentLine.front();

  m->currentLine.pop_front();

  std::map<String, StringList>::const_iterator i = m->symbols.find(ret);

  if(i != m->symbols.end())
  {
    for(StringList::const_iterator j = i->second.begin(); j != i->second.end(); ++j)
      m->currentLine.push_front(*j);

    return nextToken();
  }

  return ret;
}

String Preprocessor::Internal::nextToken()
{
  for(;;)
  {
    if(eof)
      return String::null;

    if(!currentLine.empty())
    {
      String ret = currentLine.front();

      if(ret.length() < 2 || !(ret[0] == ret[1] && (ret[1] == '/' || ret[1] == '\\')))
      {
        currentLine.pop_front();

        std::map<String, StringList>::const_iterator i = symbols.find(ret);

        if(i != symbols.end())
        {
          for(StringList::const_iterator j = i->second.begin(); j != i->second.end(); ++j)
            currentLine.push_front(*j);

          return nextToken();
        }

        return ret;
      }

      currentLine.clear();
    }

    String line = input->readLine();

    if(line.isNull())
    {
      eof = true;

      return "\n";
    }
    else
    {
      uint begin = 0;

      while(begin < line.length())
      {
        while(isspace(line[begin]))
          ++begin;

        if(begin == line.length())
          break;

        uint end = begin;

        if(line[begin] == '"')
        {
          while(line[++end] != '"')
            if(end == line.length() - 1)
            {
              esWarning << "Preprocessor: Multi-line string constant." << std::endl;

              break;
            }
          ++end;
        }
        else
        {
          ++end;

          if(line[begin] != '{')
          {
            while(!isspace(line[end]) && line[end] != '}' && line[end] != ';' && line[end] != '"')
              if(end++ == line.length())
                break;
          }
        }

        currentLine.push_back(line.mid(begin, end - begin));

        begin = end;
      }

      if(!currentLine.empty() && currentLine.front() == "#define")
      {
        currentLine.pop_front();

        String name = currentLine.front();
        currentLine.pop_front();

        StringList& value = symbols[name];

        value.clear();

        String token;
        while((token = nextToken()) != "\n")
          value.push_front(token);

        currentLine.clear();
      }

      return "\n";
    }
  }
}

void Preprocessor::unget(const String& token)
{
  m->currentLine.push_front(token);
}

void Preprocessor::include(const char* fileName)
{
  Preprocessor pp(fileName);

  std::map<String, StringList>::const_iterator i;

  for(i = pp.m->symbols.begin(); i != pp.m->symbols.end(); ++i)
    m->symbols[i->first] = i->second;
}

// vim: ts=2 sw=2 et
