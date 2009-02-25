/***************************************************************************
                           vorbis.cc  -  OGG/Vorbis library interface
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

#include <vorbis/codec.h>
#include <vorbis/vorbisfile.h>

#include <espace/file.h>
#include <espace/media.h>

#include "vorbis.h"

class VorbisData : public Media
{
public:

  uint    audioRead(void* buffer, uint size);
  uint    audioFormat();
  uint    audioFrequency();
  bool    audioEOF();
  void    audioReset();

protected:

  ~VorbisData();

  friend class Vorbis;

  OggVorbis_File vorbisFile;
  int     section;

  uint    frequency;
  uint    sampleSize;
  bool    eof;
  bool    bigEndian;
};

namespace
{
  size_t read(void *ptr, size_t size, size_t nmemb, void* file)
  {
    File* f = reinterpret_cast<File*>(file);

    uint start = f->tell();

    uint length = nmemb * size;

    if(f->tell() + length > f->length())
      length = f->length() - f->tell();

    f->read(ptr, length);

    return f->tell() - start;
  }

  int seek(void* file, ogg_int64_t offset, int whence)
  {
    File* f = reinterpret_cast<File*>(file);

    uint position = 0;

    switch(whence)
    {
    case SEEK_SET:

      position = offset;

      break;

    case SEEK_CUR:

      position = f->tell() + offset;

      break;

    case SEEK_END:

      position = f->length() + offset;

      break;
    }

    f->seek(position);

    return f->tell();
  }

  int close(void* file)
  {
    delete reinterpret_cast<File*>(file);

    return 0;
  }

  long tell(void* file)
  {
    return reinterpret_cast<File*>(file)->tell();
  }
}

uint32_t Vorbis::id()
{
  return 0x4f676753; // OggS
}

bool Vorbis::canHandle(File& file)
{
  if(file.length() < 35)
    return false;

  char tmp[7];

  file.seek(0);
  file.read(tmp, 4);

  if(memcmp(tmp, "OggS", 4))
    return false;

  file.seek(28);
  file.read(tmp, 7);

  return !memcmp(tmp, "\1vorbis", 7);
}

Media* Vorbis::open(File* file)
{
  ov_callbacks fileCallbacks;

  fileCallbacks.read_func = read;
  fileCallbacks.seek_func = seek;
  fileCallbacks.close_func = close;
  fileCallbacks.tell_func = tell;

  VorbisData* sound = new VorbisData;

  file->seek(0);

  if(ov_open_callbacks(file, &sound->vorbisFile, NULL, 0, fileCallbacks) < 0)
  {
    delete sound;

    return 0;
  }

  uint16_t test = 0xABCD;
  uint8_t* testptr = reinterpret_cast<uint8_t*>(&test);

  sound->bigEndian = (*testptr == 0xAB);

  vorbis_info* info = ov_info(&sound->vorbisFile, -1);

  sound->frequency = info->rate;
  sound->sampleSize = (info->channels == 2) ? 4 : 2;
  sound->eof = false;

  return sound;
}

VorbisData::~VorbisData()
{
  ov_clear(&vorbisFile);
}

uint VorbisData::audioRead(void* buff, uint bufferSize)
{
  char* buffer = reinterpret_cast<char*>(buff);
  bufferSize *= sampleSize;
  uint offset = 0;

  while(offset < bufferSize)
  {
    uint ret = ov_read(&vorbisFile, buffer + offset, bufferSize - offset,
                       bigEndian, 2 /* word */, 1 /* signed */, &section);

    if(ret == 0)
    {
      memset(buffer + offset, 0, bufferSize - offset);
      eof = true;

      break;
    }

    offset += ret;
  }

  return offset / sampleSize;
}

uint VorbisData::audioFormat()
{
  return (sampleSize == 4) ? Media::Stereo16 : Media::Mono16;
}

uint VorbisData::audioFrequency()
{
  return frequency;
}

bool VorbisData::audioEOF()
{
  return eof;
}

void VorbisData::audioReset()
{
  if(ov_seekable(&vorbisFile))
    ov_time_seek(&vorbisFile, 0);
}

// vim: ts=2 sw=2 et
