/***************************************************************************
                            sound.cc  -  Sound handler
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

#include <list>
#include <map>

#include <espace/cvar.h>
#include <espace/media.h>
#include <espace/output.h>
#include <espace/sound.h>
#include <espace/string.h>
#include <espace/vector.h>

#include "openal.h"

class Event
{
public:

  enum Type
  {
    Stream,
    Inactive
  };

  Type   type;
  ALuint handle;
  Media* media;
  Sound* buffers[2];
  bool   loop;
};

namespace
{
  std::map<String, Sound*> sounds;
  std::list<Event>         events;
}

struct Chunk
{
  uint   size;
  char*  data;
  Chunk* next;
};

Sound::Sound()
  : data(0),
    handle(0),
    refCount(1)
{
}

Sound::~Sound()
{
  if(handle && AL::initialized())
    AL::deleteBuffers(1, &handle);

  if(data)
    delete [] data;
}

void Sound::initialize()
{
  if(CVar::getInt("nosound"))
    return;

  AL::initialize();
}

Sound* Sound::acquire(const char* _name, bool keep)
{
  if(!AL::initialized())
    return 0;

  String name = String(_name).replace('\\', '/');

  std::map<String, Sound*>::iterator i = sounds.find(name);

  if(i != sounds.end())
  {
    ++i->second->refCount;

    return i->second;
  }

  Media* media = Media::acquire(name);

  if(!media)
  {
    esWarning << "Failed to open media stream \"" << name << "\"." << std::endl;

    return 0;
  }

  if(!media->audioFrequency())
  {
    esWarning << "Media stream \"" << name << "\" does not contain audio."
              << std::endl;

    Media::unacquire(media);

    return 0;
  }

  Sound* sound = new Sound;

  sound->freq = media->audioFrequency();
  sound->size = media->audioLength();
  sound->format = media->audioFormat();

  if(sound->size)
  {
    sound->data = new char[sound->size * sound->bytesPerSample()];

    media->audioRead(sound->data, sound->size);
  }
  else
  {
    std::list<Chunk> chunks;

    for(;;)
    {
      Chunk& chunk = *chunks.insert(chunks.end(), Chunk());

      chunk.size = 1024 * 1024;
      chunk.data = new char[chunk.size * sound->bytesPerSample()];

      uint ret = media->audioRead(chunk.data, chunk.size);

      sound->size += ret;

      if(ret < chunk.size)
      {
        chunk.size = ret;

        break;
      }
    }

    sound->data = new char[sound->size * sound->bytesPerSample()];
    uint offset = 0;

    for(std::list<Chunk>::iterator i = chunks.begin(); i != chunks.end(); ++i)
    {
      memcpy(sound->data + offset, i->data, i->size * sound->bytesPerSample());

      delete [] i->data;

      offset += i->size * sound->bytesPerSample();
    }
  }

  Media::unacquire(media);

  AL::genBuffers(1, &sound->handle);

  AL::bufferData(sound->handle, sound->format, sound->data,
                 sound->size * sound->bytesPerSample(), sound->freq);

  sounds[name] = sound;

  if(!keep)
    delete [] sound->data;

  return sound;
}

uint Sound::acquireHandle(const char* name, bool keep)
{
  Sound* sound = acquire(name, keep);

  if(!sound)
    return 0;

  return sound->handle;
}

Sound* Sound::soundForHandle(uint handle)
{
  for(std::map<String, Sound*>::iterator i = sounds.begin();
      i != sounds.end(); ++i)
  {
    if(i->second->handle == handle)
      return i->second;
  }

  return 0;
}

void Sound::unacquire(Sound* sound)
{
  if(--sound->refCount)
    return;

  for(std::map<String, Sound*>::iterator i = sounds.begin();
      i != sounds.end(); ++i)
  {
    if(i->second == sound)
    {
      sounds.erase(i);

      delete sound;

      return;
    }
  }
}

void Sound::idle()
{
  if(!AL::initialized())
    return;

  for(std::list<Event>::iterator i = events.begin(); i != events.end(); ++i)
  {
    switch(i->type)
    {
    case Event::Stream:

      {
        ALint queued;
        ALint processed;

        AL::getSourcei(i->handle, AL_BUFFERS_QUEUED, &queued);
        AL::getSourcei(i->handle, AL_BUFFERS_PROCESSED, &processed);

        if(processed + 1 >= queued)
        {
          Sound& buffer = *i->buffers[queued % 2];

          uint ret = i->media->audioRead(buffer.data, buffer.size);

          if(ret < buffer.size)
          {
            if(i->loop)
            {
              while(ret < buffer.size)
              {
                i->media->audioReset();

                ret += i->media->audioRead(buffer.data + ret
                                           * buffer.bytesPerSample(),
                                           buffer.size - ret);
              }
            }
            else // !i->loop
            {
              memset(buffer.data + ret * buffer.bytesPerSample(), 0,
                     (buffer.size - ret) * buffer.bytesPerSample());

              i->type = Event::Inactive;
            }
          }

#ifdef WIN32
          AL::deleteBuffers(1, &buffer.handle);
          AL::genBuffers(1, &buffer.handle);
#endif

          AL::bufferData(buffer.handle, buffer.format, buffer.data,
                         buffer.size * buffer.bytesPerSample(), buffer.freq);

          AL::sourceQueueBuffers(i->handle, 1, &buffer.handle);
        }
      }

      break;

    case Event::Inactive:

      break;
    }
  }
}

void Sound::setPosition(const Vector3& position)
{
  if(AL::initialized())
    AL::listenerfv(AL_POSITION, Vector3(position).data());
}

void Sound::setVelocity(const Vector3& velocity)
{
  if(AL::initialized())
    AL::listenerfv(AL_VELOCITY, Vector3(velocity).data());
}

void Sound::setOrientation(const Vector3& direction, const Vector3& up)
{
  if(!AL::initialized())
    return;

  ALfloat orientation[6] =
  {
    direction(0), direction(1), direction(2),
    up(0), up(1), up(2)
  };

  AL::listenerfv(AL_ORIENTATION, orientation);
}

SoundSource::SoundSource()
{
  data = new DataProxy;
  data->refCount = 1;

  if(AL::initialized())
    AL::genSources(1, &data->handle);
}

SoundSource::SoundSource(const SoundSource& source)
  : data(source.data)
{
  ++data->refCount;
}

SoundSource::~SoundSource()
{
  if(--data->refCount)
    return;

  stop();

  if(AL::initialized())
    AL::deleteSources(1, &data->handle);

  delete data;
}

void SoundSource::setPosition(const Vector3& position)
{
  if(AL::initialized())
    AL::sourcefv(data->handle, AL_POSITION, Vector3(position).data());
}

void SoundSource::setVelocity(const Vector3& velocity)
{
  if(AL::initialized())
    AL::sourcefv(data->handle, AL_VELOCITY, Vector3(velocity).data());
}

void SoundSource::setGain(float gain)
{
  if(AL::initialized())
    AL::sourcef(data->handle, AL_GAIN, gain);
}

void SoundSource::setReferenceDistance(float distance)
{
  if(AL::initialized())
    AL::sourcef(data->handle, AL_REFERENCE_DISTANCE, distance);
}

void SoundSource::play(const Sound* sound, bool loop)
{
  if(!AL::initialized())
    return;

  AL::sourcei(data->handle, AL_LOOPING, loop);
  AL::sourcei(data->handle, AL_BUFFER, sound->handle);
  AL::sourcePlay(data->handle);
}

void SoundSource::play(Media* media, bool loop)
{
  if(!AL::initialized())
    return;

  if(!media->audioFrequency())
    return;

  Sound* buffers[2];

  for(uint i = 0; i < 2; ++i)
  {
    buffers[i] = new Sound;

    Sound& buffer = *buffers[i];

    buffer.freq = media->audioFrequency();
    buffer.size = 65536;
    buffer.format = media->audioFormat();

    buffer.data = new char[buffer.size * buffer.bytesPerSample()];

    AL::genBuffers(1, &buffer.handle);
  }

  Event event;
  event.type = Event::Stream;
  event.handle = data->handle;
  event.media = media;
  event.buffers[0] = buffers[0];
  event.buffers[1] = buffers[1];
  event.loop = loop;

  events.push_back(event);

  Sound::idle();

  AL::sourcei(data->handle, AL_LOOPING, AL_FALSE);
  AL::sourcePlay(data->handle);
}

void SoundSource::enqueue(const Sound* sound, bool loop)
{
  if(!AL::initialized())
    return;

  AL::sourceQueueBuffers(data->handle, 1, const_cast<ALuint*>(&sound->handle));

  if(loop)
  {
    // XXX: Add loop event.
  }
}

void SoundSource::stop()
{
  if(!AL::initialized())
    return;

  AL::sourceStop(data->handle);

  for(std::list<Event>::iterator i = events.begin(); i != events.end(); ++i)
  {
    if(i->handle == data->handle)
    {
      if(i->buffers[0])
        delete i->buffers[0];

      if(i->buffers[1])
        delete i->buffers[1];

      events.erase(i);

      break;
    }
  }
}

bool SoundSource::isStopped()
{
  if(!AL::initialized())
    return true;

  ALint state;

  AL::getSourcei(data->handle, AL_SOURCE_STATE, &state);

  return state == AL_STOPPED;
}

uint Sound::bytesPerSample() const
{
  switch(format)
  {
  case AL_FORMAT_MONO8: return 1;
  case AL_FORMAT_MONO16: return 2;
  case AL_FORMAT_STEREO8: return 2;
  case AL_FORMAT_STEREO16: return 4;
  default: return 0;
  }
}

// vim: ts=2 sw=2 et
