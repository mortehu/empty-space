/***************************************************************************
                              wav.cc  -  WAV loader
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

#include <espace/file.h>
#include <espace/output.h>
#include <espace/media.h>

#include "wav.h"

class WAVData : public Media
{
public:

  uint    audioLength();
  uint    audioRead(void* buffer, uint size);
  uint    audioFormat();
  uint    audioFrequency();
  bool    audioEOF();
  void    audioReset();

protected:

  ~WAVData();

  friend class WAV;

  File*   file;
  uint    format;
  uint    frequency;
  uint    remaining;
  uint    size;
  uint    bps;
  bool    swab;
};

// Change byte order of 16 bit words in buffer
static void swab(char* buffer, uint size)
{
  char* end = buffer + size;

  while(buffer < end)
  {
    char tmp = buffer[0];
    buffer[0] = buffer[1];
    buffer[1] = tmp;

    buffer += 2;
  }
}

uint32_t WAV::id()
{
  return 0x57415645; // "WAVE"
}

bool WAV::canHandle(File& file)
{
  if(file.length() < 44)
    return false;

  char magic[4];

  file.seek(0);
  file.read(magic, 4);

  if(memcmp(magic, "RIFF", 4))
    return false;

  file.seek(8);
  file.read(magic, 4);

  return !memcmp(magic, "WAVE", 4);
}

Media* WAV::open(File* file)
{
  file->seek(16);

  uint chunkSize = file->getU32();

  uint format = file->getU16();

  if(format != 0x01)
  {
    esWarning << "WAV: Only PCM is supported." << std::endl;

    return 0;
  }

  uint channels = file->getU16();

  if(channels == 0 || channels > 2)
  {
    esWarning << "WAV: Unsupported channel count \"" << channels
              << "\"." << std::endl;

    return 0;
  }

  uint frequency = file->getU32();

  file->skip(6); // Byte rate and block align

  uint bits = file->getU16();

  if(bits != 8 && bits != 16)
  {
    esWarning << "WAV: Unsupported number of bits per sample \"" << bits
              << "\"." << std::endl;

    return 0;
  }

  file->seek(20 + chunkSize + 4); // Extra parameters

  uint length = file->getU32();

  if(length > file->length() - file->tell())
  {
    esWarning << "WAV: Given sample length is too large to fit in file."
              << std::endl;

    return 0;
  }

  WAVData* sound = new WAVData;

  sound->file = file;
  sound->format =
      (channels == 1 && bits == 8)    ? Media::Mono8
    : (channels == 2 && bits == 8)    ? Media::Stereo8
    : (channels == 1 && bits == 16)   ? Media::Mono16
    : /* channels == 2 && bits == 16 */ Media::Stereo16;
  sound->bps = channels * bits / 8;
  sound->size = length / sound->bps;
  sound->remaining = length;
  sound->frequency = frequency;

  sound->swab = false;

  if(bits == 16)
  {
    uint16_t test = 0xABCD;
    uint8_t* testptr = reinterpret_cast<uint8_t*>(&test);

    if(*testptr == 0xAB) // Big endian
      sound->swab = true;
  }

  return sound;
}

WAVData::~WAVData()
{
  delete file;
}

uint WAVData::audioLength()
{
  return size;
}

uint WAVData::audioRead(void* buffer, uint size)
{
  size *= bps;

  if(size <= remaining)
  {
    file->read(buffer, size);

    remaining -= size;
  }
  else
  {
    file->read(buffer, remaining);

    size = remaining;

    remaining = 0;
  }

  if(this->swab)
    ::swab(reinterpret_cast<char*>(buffer), size);

  return size / bps;
}

uint WAVData::audioFormat()
{
  return format;
}

uint WAVData::audioFrequency()
{
  return frequency;
}

bool WAVData::audioEOF()
{
  return remaining == 0;
}

void WAVData::audioReset()
{
  file->seek(44);

  remaining = size * bps;
}

// vim: ts=2 sw=2 et
