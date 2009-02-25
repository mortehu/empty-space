/***************************************************************************
                              zip.cc  -  ZIP loader
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
#include <zlib.h>

#include <espace/file.h>
#include <espace/output.h>

#include "zip.h"

namespace
{
  class Handle
  {
  public:

    File*    archive;
    uint     start;
    uint     offset;
    uint     compMethod;
    uint     compSize;
    uint     compOffset;
    z_stream zStream;
    char*    buffer;
    uint     bufferSize;
  };

  std::map<int, Handle>   handles;
  int                     nextHandle;
  std::map<String, File*> fileHandles;
}

uint32_t ZIP::id()
{
  return 0x504B0304; // "PK\3\4"
}

bool ZIP::canHandle(File& archive)
{
  if(archive.length() < 4)
    return false;

  char magic[4];

  archive.seek(0);
  archive.read(magic, 4);

  return !memcmp(magic, "PK\3\4", 4);
}

void ZIP::scan(File& archive, const String& archiveName, std::vector<FileEntry>& entries)
{
  archive.seek(0);

  for(;;)
  {
    char header[4];

    archive.read(header, 4);

    if(!memcmp(header, "PK\5\6", 4))
      break;

    if(!memcmp(header, "PK\3\4", 4))
    {
      archive.skip(2);
      uint16_t flags = archive.getU16();
      archive.skip(10);
      uint32_t compSize = archive.getU32();
      archive.skip(4);
      uint16_t nameLength = archive.getU16();
      uint16_t extraLength = archive.getU16();

      archive.skip(nameLength);
      archive.skip(extraLength);

      if(flags & 8)
      {
        esWarning << "ZIP: Unsupported feature (unknown length) used in \"" << archiveName << "\"." << std::endl;

        return;
      }

      archive.skip(compSize);
    }
    else if(!memcmp(header, "PK\1\2", 4))
    {
      archive.skip(16);
      uint32_t compSize = archive.getU32();
      uint32_t uncompSize = archive.getU32();
      uint16_t nameLength = archive.getU16();
      uint16_t extraLength = archive.getU16();
      uint16_t commentLength = archive.getU16();
      archive.skip(8);
      uint32_t offset = archive.getU32();

      char* name = new char[nameLength + 1];
      archive.read(name, nameLength);
      name[nameLength] = 0;

      if(name[nameLength - 1] != '/')
      {
        FileEntry& entry = *entries.insert(entries.end(), FileEntry());

        entry.type = id();
        entry.name = name;
        entry.archive = archiveName;
        entry.offset = offset;
        entry.size = uncompSize;
      }

      archive.skip(extraLength);
      archive.skip(commentLength);
    }
    else
    {
      esWarning << "ZIP: Corrupt archive \"" << archiveName << "\"." << std::endl;

      return;
    }
  }
}

int ZIP::open(const FileEntry& entry)
{
  Handle handle;

  std::map<String, File*>::iterator i = fileHandles.find(entry.archive);

  if(i == fileHandles.end())
  {
    handle.archive = new File(entry.archive);

    if(!handle.archive->isOpen())
      return -1;

    fileHandles[entry.archive] = handle.archive;
  }
  else // i != fileHandles.end()
  {
    handle.archive = i->second;
  }

  File& archive = *handle.archive;

  archive.seek(entry.offset + 6);
  uint16_t flags = archive.getU16();
  handle.compMethod = archive.getU16();
  archive.skip(8);
  handle.compSize = archive.getU32();
  archive.skip(4);
  uint16_t nameLength = archive.getU16();
  uint16_t extraLength = archive.getU16();

  archive.skip(nameLength);
  archive.skip(extraLength);

  handle.start = archive.tell();

  handle.offset = 0;
  handle.compOffset = 0;

  memset(&handle.zStream, 0, sizeof(z_stream));

  if(handle.compMethod == 8)
    inflateInit2(&handle.zStream, -MAX_WBITS);

  handle.bufferSize = 65536;
  handle.buffer = new char[handle.bufferSize];

  handles[nextHandle++] = handle;

  return nextHandle - 1;
}

void ZIP::read(int _handle, void* buffer, uint count)
{
  std::map<int, Handle>::iterator i = handles.find(_handle);

  if(i == handles.end())
    return;

  Handle& handle = i->second;
  File& archive = *handle.archive;

  archive.seek(handle.start + handle.compOffset);

  switch(handle.compMethod)
  {
  case 0:

    archive.read(buffer, count);

    handle.compOffset += count;
    handle.offset += count;

    break;

  case 1:

    esWarning << "ZIP: Unsupported compression method \"Shrink\"." << std::endl;

    break;

  case 2:
  case 3:
  case 4:
  case 5:

    esWarning << "ZIP: Unsupported compression method \"Reduce\"." << std::endl;

    return;

  case 6:

    esWarning << "ZIP: Unsupported compression method \"Implode\"." << std::endl;

    return;

  case 7:

    esWarning << "ZIP: Unsupported compression method \"Tokenize\"." << std::endl;

    return;

  case 8: // Deflate

    {
      handle.zStream.avail_out = count;
      handle.zStream.next_out = reinterpret_cast<Bytef*>(buffer);

      while(handle.zStream.avail_out)
      {
        if(!handle.zStream.avail_in)
        {
          handle.zStream.avail_in = handle.compSize - handle.compOffset;

          if(handle.zStream.avail_in > handle.bufferSize)
            handle.zStream.avail_in = handle.bufferSize;

          if(!handle.zStream.avail_in)
          {
            inflate(&handle.zStream, Z_SYNC_FLUSH);

            break;
          }

          archive.read(handle.buffer, handle.zStream.avail_in);

          handle.compOffset += handle.zStream.avail_in;

          handle.zStream.next_in = reinterpret_cast<Bytef*>(handle.buffer);
        }

        inflate(&handle.zStream, Z_SYNC_FLUSH);
      }

      handle.offset = handle.zStream.total_out;
    }

    break;

  case 9:

    esWarning << "ZIP: Unsupported compression method \"Deflate64\"." << std::endl;

    return;

  case 10:

    esWarning << "ZIP: Unsupported compression method \"PKWARE Date\"." << std::endl;

    return;

  default:

    esWarning << "ZIP: Unknown compression method \"" << handle.compMethod << "\"." << std::endl;

    return;
  }
}

void ZIP::seek(int _handle, uint position)
{
  std::map<int, Handle>::iterator i = handles.find(_handle);

  if(i == handles.end())
    return;

  Handle& handle = i->second;
  File& archive = *handle.archive;

  if(position < handle.offset) // Backward seek
  {
    // Reset the stream so the problem is reduced to a forward seek

    if(handle.compMethod == 8)
    {
      inflateEnd(&i->second.zStream);

      memset(&handle.zStream, 0, sizeof(z_stream));

      inflateInit2(&handle.zStream, -MAX_WBITS);
    }

    handle.compOffset = 0;
    handle.offset = 0;
  }

  if(position > handle.offset) // Forward seek
  {
    uint amount = position - handle.offset;

    // XXX: It's suboptimal to seek by reading into a temporary buffer :P

    char* tmp = new char[amount];

    read(_handle, tmp, amount);

    delete [] tmp;
  }
}

void ZIP::close(int handle)
{
  std::map<int, Handle>::iterator i = handles.find(handle);

  if(i == handles.end())
    return;

  if(i->second.compMethod == 8)
    inflateEnd(&i->second.zStream);

  delete [] i->second.buffer;

  handles.erase(i);
}

// vim: ts=2 sw=2 et
