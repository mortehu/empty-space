/***************************************************************************
                              roq.cc  -  RoQ decoder
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

#include <stdint.h>

#include <espace/file.h>
#include <espace/image.h>
#include <espace/media.h>

#include "roq.h"

class RoQData : public Media
{
public:

  Image&  videoRead();
  void    videoSkip(uint count);
  uint    videoFrameRate();
  bool    videoEOF();
  void    videoReset();

  uint    audioLength();
  uint    audioRead(void* buffer, uint size);
  uint    audioFormat();
  uint    audioFrequency();
  bool    audioEOF();
  void    audioReset();

protected:

  ~RoQData();

  friend class RoQ;

  enum ChunkType
  {
    Info = 0x1001,
    QuadCodeBook = 0x1002,
    QuadVQ = 0x1011,
    EndOfFile = 0x1012,
    SoundMono = 0x1020,
    SoundStereo = 0x1021
  };

  enum BlockType
  {
    Skip = 0,
    Motion = 1,
    CodeBookIndex = 2,
    Subdivide = 3
  };

  class Pixel2x2
  {
  public:

    uint8_t Y[4];
    uint8_t Cb;
    uint8_t Cr;
  };

  File* file;

  Image* front;
  Image* back;

  uint8_t* frontData;
  uint8_t* backData;

  uint width;
  uint height;

  int  blockTypes;
  int  blockTypeCount;

  Pixel2x2 codeBook[256];
  uint8_t  codeBookIndices[256 * 4];

  int  meanX;
  int  meanY;

  uint videoNextChunk;
  uint frame;

  uint audioNextChunk;
  uint audioChunkRemaining;
  uint audioPosition;
  int  lastSample[2];

  uint bps;
  uint channelCount;
  uint frequency;

  uint audioDecode(uint8_t* sampleBuffer, uint sampleCount);

  ChunkType videoReadChunk();
  BlockType getBlockType();
  template<int blockSize> void handleBlock(uint x, uint y);
  template<int blockSize> void copyFromCodeBook(uint x, uint y, uint8_t index);
  template<int blockSize> void move(uint backX, uint backY, uint frontX, uint frontY);
};

uint32_t RoQ::id()
{
  return 0x8410FFFF;
}

bool RoQ::canHandle(File& file)
{
  if(file.length() < 6)
    return false;

  const unsigned char magic[] = { 0x84, 0x10, 0xff, 0xff, 0xff, 0xff };
  char tmp[6];

  file.seek(0);
  file.read(tmp, 6);

  return !memcmp(tmp, magic, 6);
}

Media* RoQ::open(File* file)
{
  RoQData* media = new RoQData;

  media->file = file;

  media->videoReset();
  media->audioReset();

  media->front = 0;
  media->back = 0;

  media->frequency = 0;

  file->seek(8 /* First chunk */);

  for(uint tested = 0; tested < 10 && !media->frequency; ++tested)
  {
    RoQData::ChunkType chunkType = static_cast<RoQData::ChunkType>(file->getU16());
    file->skip(file->getU32() /* Chunk size */ + 2);

    if(chunkType == RoQData::SoundStereo)
    {
      media->channelCount = 2;
      media->bps = media->channelCount * 2;
      media->frequency = 22050;
    }
    else if(chunkType == RoQData::SoundMono)
    {
      media->channelCount = 1;
      media->bps = media->channelCount * 2;
      media->frequency = 22050;
    }
  }

  return media;
}

RoQData::~RoQData()
{
  delete file;

  if(front)
  {
    delete front;
    delete back;
  }
}

RoQData::BlockType RoQData::getBlockType()
{
  if(!blockTypeCount)
  {
    blockTypes = file->getU16();
    blockTypeCount = 8;
  }

  return static_cast<BlockType>((blockTypes >> (--blockTypeCount * 2)) & 3);
}

template<>
void RoQData::handleBlock<16>(uint x, uint y)
{
  handleBlock<8>(x,     y);
  handleBlock<8>(x + 8, y);
  handleBlock<8>(x    , y + 8);
  handleBlock<8>(x + 8, y + 8);
}

template<int blockSize>
void RoQData::handleBlock(uint x, uint y)
{
  switch(getBlockType())
  {
  case Skip:

    break;

  case Motion:

    {
      int arg = file->getU8();

      int deviationX = 8 - (arg >> 4) - meanX;
      int deviationY = 8 - (arg & 0xF) - meanY;

      move<blockSize>(x, y, x + deviationX, y + deviationY);
    }

    break;

  case CodeBookIndex:

    {
      int i = file->getU8() * 4;

      for(int j = 0; j < 4; ++j)
      {
        copyFromCodeBook<blockSize / 2>(
          x + (j % 2) * blockSize / 2,
          y + (j / 2) * blockSize / 2,
          codeBookIndices[i + j]);
      }
    }

    break;

  case Subdivide:

    if(blockSize == 8)
    {
      for(int i = 0; i < 4; ++i)
      {
        handleBlock<4>(
          x + (i % 2) * 4,
          y + (i / 2) * 4);
      }
    }
    else // blockSize == 4
    {
      for(int i = 0; i < 4; ++i)
      {
        copyFromCodeBook<2>(
          x + (i % 2) * 2,
          y + (i / 2) * 2,
          file->getU8());
      }
    }
  }
}

template<int blockSize>
void RoQData::copyFromCodeBook(uint x, uint y, uint8_t index)
{
  Pixel2x2& source = codeBook[index];
  int n = blockSize - 1;

  for(int i = 0; i < blockSize; ++i)
  {
    for(int j = 0; j < blockSize; ++j)
    {
      backData[(y + i) * width + (x + j)] =
         (source.Y[0] * (n - i) * (n - j)
        + source.Y[1] * (n - i) * (    j)
        + source.Y[2] * (    i) * (n - j)
        + source.Y[3] * (    i) * (    j)) / (n * n);
    }
  }

  for(int i = 0; i < blockSize / 2; ++i)
  {
    for(int j = 0; j < blockSize; j += 2)
    {
      backData[(height + y / 2 + i) * width + x + j] = source.Cb;
      backData[(height + y / 2 + i) * width + x + j + 1] = source.Cr;
    }
  }
}

template<int blockSize>
void RoQData::move(uint backX, uint backY, uint frontX, uint frontY)
{
  for(int y = 0; y < blockSize; ++y, ++frontY, ++backY)
  {
    if(frontY >= height)
      return;

    memcpy(&backData[backY * width + backX],
           &frontData[frontY * width + frontX], blockSize);

    if(!(y & 1))
    {
      memcpy(&backData[(height + backY / 2) * width + (backX & ~1)],
             &frontData[(height + frontY / 2) * width + (frontX & ~1)],
             blockSize);
    }
  }
}

RoQData::ChunkType RoQData::videoReadChunk()
{
  file->seek(videoNextChunk);

  ChunkType chunkType = static_cast<ChunkType>(file->getU16());

  uint chunkSize = file->getU32();
  videoNextChunk = file->tell() + chunkSize + 2;

  switch(chunkType)
  {
  case Info:

    {
      file->skip(2); // Unused argument

      if(!front)
      {
        width = file->getU16();
        height = file->getU16();

        front = new Image(width, height, Image::NV12);
        frontData = front->data();

        back = new Image(width, height, Image::NV12);
        backData = back->data();
      }
    }

    break;

  case QuadCodeBook:

    {
      uint indexCount = file->getU8();
      uint pixel2x2Count = file->getU8();

      if(pixel2x2Count == 0)
        pixel2x2Count = 256;

      if(indexCount == 0 && pixel2x2Count * sizeof(Pixel2x2) < chunkSize)
        indexCount = 256;

      for(uint i = 0; i < pixel2x2Count; ++i)
      {
        for(uint j = 0; j < 4; ++j)
          codeBook[i].Y[j] = file->getU8();

        codeBook[i].Cb = file->getU8();
        codeBook[i].Cr = file->getU8();
      }

      file->read(codeBookIndices, indexCount * 4);
    }

    break;

  case QuadVQ:

    {
      blockTypeCount = 0;

      // In case of any "Skip" blocks in the second frame
      if(frame == 1)
      {
        memcpy(
         backData,
         frontData,
         width * height + (height / 2) * width);
      }

      meanY = file->getS8();
      meanX = file->getS8();

      const int blockSize = 16;

      for(uint y = 0; y < height; y += blockSize)
        for(uint x = 0; x < width; x += blockSize)
          handleBlock<blockSize>(x, y);

      ++frame;
    }

    break;

  case EndOfFile:

    videoNextChunk = file->length();

    break;

  default: ; // Avoid warning about unhandled enums
  }

  return chunkType;
}

Image& RoQData::videoRead()
{
  while(!videoEOF())
    if(QuadVQ == videoReadChunk())
      break;

  Image* tmp = back;
  back = front;
  front = tmp;

  backData = back->data();
  frontData = front->data();

  return *front;
}

void RoQData::videoSkip(uint count)
{
  while(count--)
  {
    while(!videoEOF())
      if(QuadVQ == videoReadChunk())
        break;

    Image* tmp = back;
    back = front;
    front = tmp;

    backData = back->data();
    frontData = front->data();
  }
}

uint RoQData::videoFrameRate()
{
  return 24;
}

bool RoQData::videoEOF()
{
  return !file->isOpen() || videoNextChunk == file->length();
}

void RoQData::videoReset()
{
  frame = 0;
  videoNextChunk = 8;
}

uint RoQData::audioLength()
{
  return 0;
}

uint RoQData::audioDecode(uint8_t* _sample, uint sampleCount)
{
  int16_t* sample = reinterpret_cast<int16_t*>(_sample);

  uint size = std::min(audioChunkRemaining, sampleCount);

  for(uint i = 0; i < size; ++i)
  {
    for(uint channel = 0; channel < channelCount; ++channel)
    {
      int prediction = file->getU8();

      if(prediction < 128)
        prediction = prediction * prediction;
      else
        prediction = -((prediction - 128) * (prediction - 128));

      *sample++ = (lastSample[channel] += prediction);
    }
  }

  audioChunkRemaining -= size;

  return size;
}

uint RoQData::audioRead(void* buf, uint size)
{
  uint8_t* buffer = reinterpret_cast<uint8_t*>(buf);

  uint read = 0;

  if(audioChunkRemaining)
  {
    file->seek(audioPosition);

    read += audioDecode(buffer, size);
  }

  while(read < size && !audioEOF())
  {
    file->seek(audioNextChunk);

    ChunkType chunkType = static_cast<ChunkType>(file->getU16());

    uint chunkSize = file->getU32();
    audioNextChunk = file->tell() + chunkSize + 2;

    switch(chunkType)
    {
    case SoundMono:

      lastSample[0] = file->getS16();

      audioChunkRemaining = chunkSize / channelCount;

      read += audioDecode(buffer + read * bps, size - read);

      break;

    case SoundStereo:

      {
        int argument = file->getU16();

        lastSample[0] = argument & 0xFF00;
        lastSample[1] = argument << 8;

        audioChunkRemaining = chunkSize / channelCount;

        read += audioDecode(buffer + read * bps, size - read);
      }

      break;

    case EndOfFile:

      audioNextChunk = file->length();

      break;

    default: ; // Avoid warning about unhandled enums
    }
  }

  audioPosition = file->tell();

  return read;
}

uint RoQData::audioFormat()
{
  return (channelCount == 1) ? Media::Mono16 : Media::Stereo16;
}

uint RoQData::audioFrequency()
{
  return frequency;
}

bool RoQData::audioEOF()
{
  return !file->isOpen() || audioNextChunk == file->length();
}

void RoQData::audioReset()
{
  audioNextChunk = 8;
  audioChunkRemaining = 0;
}

// vim: ts=2 sw=2 et
