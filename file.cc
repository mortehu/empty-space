/***************************************************************************
                             file.cc  -  Buffered file I/O
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

#include <algorithm>
#include <map>

#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#ifdef _POSIX_MAPPED_FILES
#  include <sys/mman.h>
#endif
#ifdef WIN32
#  include <io.h>
#endif

#include <espace/file.h>
#include <espace/string.h>
#include <espace/output.h>
#include <espace/plugins.h>

File::EntryList File::entries;

namespace
{
  std::vector<File::FileHook> fileHooks;
}

static bool operator==(const FileEntry& entry, const String& string)
{
  return entry.name == string;
}

bool File::readDirectory(const char* _directory, const char* prefix)
{
  String directory = _directory;

  if(directory.beginsWith("./"))
  {
    char pwd[PATH_MAX];

    getcwd(pwd, PATH_MAX);

    directory = String(pwd) + directory.right(directory.length() - 1);
  }
  else if(directory.beginsWith("~/"))
  {
    directory = String(getenv("HOME")) + directory.right(directory.length() - 1);
  }

  if(!*prefix)
    esInfo << "Scanning directory \"" << directory << "\"... ";

  DIR* dir = opendir(directory);

  if(!dir)
  {
    if(!*prefix)
      esInfo << strerror(errno) << "." << std::endl;

    return false;
  }

  struct dirent* ent;

#ifndef WIN32
  struct stat buf;
  stat(directory, &buf);

  int subdirsLeft = buf.st_nlink - 2; // "directory" and "directory/."
#else
  int subdirsLeft = -1;
#endif

  entries.reserve(8192); // More than the amount of files in RtCW with expansion packs.

  while(0 != (ent = readdir(dir)))
  {
    if(!strcmp(ent->d_name, ".") || !strcmp(ent->d_name, ".."))
      continue;

    // Skip hidden files
    if(ent->d_name[0] == '.')
      continue;

    String fileName = String(directory) + "/" + ent->d_name;

    if(subdirsLeft)
    {
      if(readDirectory(fileName, String(prefix) + ent->d_name + "/"))
      {
        --subdirsLeft;

        continue;
      }
    }

    PluginMap(Archive)::iterator i;

    // Only check for archives in the top directory.
    if(!*prefix)
    {
      File file(fileName, File::Read, 4);

      if(!file.isOpen())
        continue;

      for(i = Plugin::archive.begin(); i != Plugin::archive.end(); ++i)
      {
        if(i->second->canHandle(file))
        {
          i->second->scan(file, fileName, entries);

          break;
        }
      }
    }
    else
    {
      i = Plugin::archive.end();
    }

    // If there's no compatible archive plugins, treat as ordinary file
    if(i == Plugin::archive.end())
    {
      FileEntry& entry = *entries.insert(entries.end(), FileEntry());

      entry.type = 0;
      entry.name = (String(prefix) + String(ent->d_name).replace('\\', '/')).toLower();
      entry.archive = fileName;

      for(std::vector<FileHook>::iterator i = fileHooks.begin();
          i != fileHooks.end(); ++i)
      {
        (*i)(entry.name);
      }
    }
  }

  closedir(dir);

  if(!*prefix)
    esInfo << "done." << std::endl;

  return true;
}

String File::realName(const char* name)
{
  EntryList::const_iterator i = std::find(entries.begin(), entries.end(),
                                          String(name).toLower());

  if(i == entries.end())
    return String::null;

  return i->archive;
}

void File::addFileHook(FileHook fileHook)
{
  fileHooks.push_back(fileHook);
}

File::File(const char* _fileName, uint flags, uint bufferSize)
  : bufferSize(bufferSize),
    bufferPosition(0),
    position(0),
    writing(false),
    archive(0)
{
  if((flags & Read) && (flags & Write))
  {
    esWarning << "Opening a file for both reading and writing is not supported."
              << std::endl;

    return;
  }

  if(!(flags & (Read | Write)))
  {
    esWarning << "A file must be opened for either reading or writing."
              << std::endl;

    return;
  }

  bool absolute = false;
  String fileName = _fileName;

#ifndef WIN32
  if(fileName.beginsWith("~/"))
  {
    fileName = String(getenv("HOME")) + fileName.right(fileName.length() - 1);

    absolute = true;
  }
  else if(fileName[0] == '/' || entries.empty())
#else
  if((fileName[0] && fileName[1] == ':') || entries.empty())
#endif
  {
    absolute = true;
  }

  if((flags & Write && !absolute) || fileName.beginsWith("./"))
  {
    absolute = true;

    char pwd[PATH_MAX];

    getcwd(pwd, PATH_MAX);

    if(fileName.beginsWith("./"))
    {
      fileName = String(pwd) + fileName.right(fileName.length() - 1);
    }
    else
    {
      fileName = String(pwd) + "/" + fileName;
    }
  }

  if(flags & Write)
  {
    writing = true;

#ifndef WIN32
    fd = open(fileName,
              O_WRONLY | O_CREAT | ((flags & Truncate) ? O_TRUNC : O_APPEND), 0644);
#else
    fd = open(fileName,
              O_WRONLY | O_CREAT | O_BINARY | ((flags & Truncate) ? O_TRUNC : O_APPEND));
#endif

    if(fd < 0)
      return;

    if(bufferSize)
      buffer = new unsigned char[bufferSize];
  }
  else // (flags & Read)
  {
    if(!absolute)
    {
      String searchName = String(fileName).toLower();

      EntryList::const_iterator i;

      for(i = entries.begin(); i != entries.end(); ++i)
        if((*i).name == searchName)
          break;

      if(i == entries.end())
      {
        fd = -1;

        return;
      }

      if((*i).type) // File is within an archive
      {
        archive = Plugin::archive[(*i).type];

        fd = archive->open(*i);

        if(fd < 0)
          return;

        fileSize = (*i).size;

        if(fileSize < this->bufferSize)
          this->bufferSize = fileSize;

        if(!this->bufferSize)
          return;

        buffer = new unsigned char[this->bufferSize];

        archive->read(fd, buffer, this->bufferSize);

        return;
      }
      else // File is not in an archive
      {
  #ifndef WIN32
        fd = open((*i).archive, O_RDONLY);
  #else
        fd = open((*i).archive, O_RDONLY | O_BINARY);
  #endif

        if(fd < 0)
          return;
      }
    }
    else // Use absolute path
    {
  #ifndef WIN32
      fd = open(fileName, O_RDONLY);
  #else
      fd = open(fileName, O_RDONLY | O_BINARY);
  #endif

      if(fd < 0)
        return;
    }

    struct stat buf;

    fstat(fd, &buf);

    fileSize = buf.st_size;

#ifdef _POSIX_MAPPED_FILES
    this->bufferSize = fileSize;

    if(!this->bufferSize)
      return;

    buffer = reinterpret_cast<unsigned char*>(mmap(0, this->bufferSize,
                                                   PROT_READ, MAP_PRIVATE,
                                                   fd, 0));

    if(buffer == MAP_FAILED)
    {
      esWarning << "Failed to mmap \"" << fileName << "\": " << strerror(errno) << "." << std::endl;

      close(fd);

      fd = -1;

      return;
    }
#else // !_POSIX_MAPPED_FILES
    if(fileSize < this->bufferSize)
      this->bufferSize = fileSize;

    if(!this->bufferSize)
      return;

    buffer = new unsigned char[this->bufferSize];

    ::read(fd, buffer, this->bufferSize);
#endif
  }
}

File::~File()
{
  if(fd >= 0)
  {
    if(writing && bufferPosition)
      flush();

    if(archive)
    {
      archive->close(fd);
    }
    else
    {
      close(fd);
    }

    if(bufferSize)
    {
      if(archive || writing)
      {
        delete [] buffer;
      }
      else
      {
#ifdef _POSIX_MAPPED_FILES
        munmap(buffer, bufferSize);
#else // !_POSIX_MAPPED_FILES
        delete [] buffer;
#endif
      }
    }
  }
}

uint File::modified()
{
  struct stat fileState;

  fstat(fd, &fileState);

  return fileState.st_mtime;
}

uint8_t File::getU8()
{
#ifdef _POSIX_MAPPED_FILES
  if(archive && position + 1 > bufferSize)
#else // !_POSIX_MAPPED_FILES
  if(position + 1 > bufferSize)
#endif
  {
    if(position > bufferSize)
      sysSeek(bufferPosition + position);

    bufferPosition += position;
    position = 0;

    if(bufferPosition + bufferSize < fileSize)
      sysRead(buffer, bufferSize);
    else
      sysRead(buffer, fileSize - bufferPosition);
  }

  return buffer[position++];
}

uint16_t File::getU16()
{
#ifdef _POSIX_MAPPED_FILES
  if(archive && position + 2 > bufferSize)
#else // !_POSIX_MAPPED_FILES
  if(position + 2 > bufferSize)
#endif
  {
    uint16_t ret;

    ret = getU8();
    ret |= getU8() << 8;

    return ret;
  }

  position += 2;

  return
      buffer[position - 2]
    | (buffer[position - 1] << 8);
}

uint32_t File::getU32()
{
#ifdef _POSIX_MAPPED_FILES
  if(archive && position + 4 > bufferSize)
#else // !_POSIX_MAPPED_FILES
  if(position + 4 > bufferSize)
#endif
  {
    uint32_t ret;

    ret = getU16();
    ret |= getU16() << 16;

    return ret;
  }

  position += 4;

  return
      buffer[position - 4]
    | (buffer[position - 3] << 8)
    | (buffer[position - 2] << 16)
    | (buffer[position - 1] << 24);
}

String File::readLine()
{
  if(eof())
    return String::null;

  String ret("");

  do
  {
    char c = (position < bufferSize) ? buffer[position++] : getU8();

    if(c == '\r')
      continue;

    if(c == '\n')
      break;

    ret += c;
  }
  while(bufferPosition + position < fileSize);

  return ret;
}

void File::read(void* buffer, uint count)
{
#ifdef _POSIX_MAPPED_FILES
  if(!archive || position + count < bufferSize)
#else // !_POSIX_MAPPED_FILES
  if(position + count < bufferSize)
#endif
  {
    memcpy(buffer, this->buffer + position, count);

    position += count;
  }
  else
  {
    int amount = 0;

    if(position < bufferSize)
    {
      amount = bufferSize - position;

      memcpy(buffer, this->buffer + position, amount);
    }
    else if(position > bufferSize)
    {
      sysSeek(bufferPosition + position);
    }

    sysRead(static_cast<char*>(buffer) + amount, count - amount);

    position += count;
  }
}

uint8_t* File::data()
{
#ifdef _POSIX_MAPPED_FILES
  if(archive)
#endif
  {
    if(bufferSize == fileSize)
      return buffer;

    uint8_t* newBuffer = new uint8_t[fileSize];
    memcpy(newBuffer + bufferPosition, buffer, bufferSize);

    if(bufferPosition + bufferSize < fileSize)
    {
      sysSeek(bufferPosition + bufferSize);

      sysRead(newBuffer + bufferPosition + bufferSize, fileSize - (bufferPosition + bufferSize));
    }

    if(bufferPosition > 0)
    {
      sysSeek(0);

      sysRead(newBuffer, bufferPosition);
    }

    delete [] buffer;

    position += bufferPosition;
    bufferPosition = 0;
    bufferSize = fileSize;
    buffer = newBuffer;
  }

  return buffer;
}

void File::seek(uint position)
{
#ifdef _POSIX_MAPPED_FILES
  if(archive && position < bufferPosition)
#else // !_POSIX_MAPPED_FILES
  if(position < bufferPosition)
#endif
  {
    bufferPosition = position;
    this->position = 0;

    sysSeek(bufferPosition);

    if(bufferPosition + bufferSize < fileSize)
      sysRead(buffer, bufferSize);
    else
      sysRead(buffer, fileSize - bufferPosition);

    return;
  }

  this->position = position - bufferPosition;
}

void File::put(uint8_t v)
{
  if(!bufferSize)
  {
    ::write(fd, &v, 1);

    return;
  }

  if(bufferPosition == bufferSize)
    flush();

  buffer[bufferPosition++] = v;
}

void File::put(uint16_t v)
{
  put(static_cast<uint8_t>(v));
  put(static_cast<uint8_t>(v >> 8));
}

void File::put(uint32_t v)
{
  put(static_cast<uint8_t>(v));
  put(static_cast<uint8_t>(v >> 8));
  put(static_cast<uint8_t>(v >> 16));
  put(static_cast<uint8_t>(v >> 24));
}

void File::put(const String& v)
{
  uint length = v.length();

  for(uint i = 0; i < length; ++i)
    put(static_cast<uint8_t>(v[i]));
}

void File::write(const void* buffer, uint count)
{
  if(bufferPosition)
    flush();

  ::write(fd, buffer, count);
}

void File::flush()
{
  ::write(fd, buffer, bufferPosition);

  bufferPosition = 0;
}

void File::skip(uint amount)
{
  seek(bufferPosition + position + amount);
}

bool File::eof() const
{
  return bufferPosition + position >= fileSize;
}

void File::sysRead(void* buffer, uint length)
{
#ifndef _POSIX_MAPPED_FILES
  if(archive)
#endif
  {
    archive->read(fd, buffer, length);
  }
#ifndef _POSIX_MAPPED_FILES
  else
  {
    ::read(fd, buffer, length);
  }
#endif
}

void File::sysSeek(uint position)
{
#ifndef _POSIX_MAPPED_FILES
  if(archive)
#endif
  {
    archive->seek(fd, position);
  }
#ifndef _POSIX_MAPPED_FILES
  else
  {
    ::lseek(fd, position, SEEK_SET);
  }
#endif
}

// vim: ts=2 sw=2 et
