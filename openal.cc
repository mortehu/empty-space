/***************************************************************************
                           openal.cc  -  OpenAL function declarations
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

#include <espace/cvar.h>
#include <espace/output.h>
#include <espace/system.h>

#include "openal.h"

namespace
{
  void* context = 0;
}

void AL::initialize()
{
  if(context)
    return;

  CVar alDriver = CVar::acquire("s_aldriver",
#ifndef WIN32
                                "libopenal.so.0",
#else
                                "openal32",
#endif
                                CVar::Archive);

  void* library = System::dlopen(alDriver.string);

  if(!library)
  {
    esWarning << "Failed to load OpenAL library \"" << alDriver.string << "\": "
              << System::dlerror() << ", sound disabled." << std::endl;

    return;
  }

  esInfo << "Loaded OpenAL library \"" << alDriver.string << "\"." << std::endl;

#define PROC(x) \
  (x) System::dlsym(library, #x)

  enable = PROC(alEnable);
  disable = PROC(alDisable);
  isEnabled = PROC(alIsEnabled);
  hint = PROC(alHint);
  getBoolean = PROC(alGetBoolean);
  getInteger = PROC(alGetInteger);
  getFloat = PROC(alGetFloat);
  getDouble = PROC(alGetDouble);
  getBooleanv = PROC(alGetBooleanv);
  getIntegerv = PROC(alGetIntegerv);
  getFloatv = PROC(alGetFloatv);
  getDoublev = PROC(alGetDoublev);
  getString = PROC(alGetString);
  getError = PROC(alGetError);
  isExtensionPresent = PROC(alIsExtensionPresent);
  getProcAddress = PROC(alGetProcAddress);
  getEnumValue = PROC(alGetEnumValue);

  listenerf = PROC(alListenerf);
  listenerfv = PROC(alListenerfv);
  getListeneri = PROC(alGetListeneri);
  getListenerf = PROC(alGetListenerf);
  getListeneriv = PROC(alGetListeneriv);
  getListenerfv = PROC(alGetListenerfv);

  genSources = PROC(alGenSources);
  deleteSources = PROC(alDeleteSources);
  isSource = PROC(alIsSource);
  sourcei = PROC(alSourcei);
  sourcef = PROC(alSourcef);
  source3f = PROC(alSource3f);
  sourcefv = PROC(alSourcefv);
  getSourcei = PROC(alGetSourcei);
  getSourceiv = PROC(alGetSourceiv);
  getSourcef = PROC(alGetSourcef);
  getSource3f = PROC(alGetSource3f);
  getSourcefv = PROC(alGetSourcefv);
  sourcePlay = PROC(alSourcePlay);
  sourcePause = PROC(alSourcePause);
  sourceStop = PROC(alSourceStop);
  sourceRewind = PROC(alSourceRewind);
  sourcePlayv = PROC(alSourcePlayv);
  sourcePausev = PROC(alSourcePausev);
  sourceStopv = PROC(alSourceStopv);
  sourceRewindv = PROC(alSourceRewindv);

  genBuffers = PROC(alGenBuffers);
  deleteBuffers = PROC(alDeleteBuffers);
  isBuffer = PROC(alIsBuffer);
  bufferData = PROC(alBufferData);
  getBufferi = PROC(alGetBufferi);
  getBufferf = PROC(alGetBufferf);
  getBufferiv = PROC(alGetBufferiv);
  getBufferfv = PROC(alGetBufferfv);

  queuei = PROC(alQueuei);
  sourceUnqueueBuffers = PROC(alSourceUnqueueBuffers);
  sourceQueueBuffers = PROC(alSourceQueueBuffers);

  createContext = PROC(alcCreateContext);
  makeContextCurrent = PROC(alcMakeContextCurrent);
  openDevice = PROC(alcOpenDevice);
  destroyContext = PROC(alcDestroyContext);

  ALCdevice* dev = openDevice(0);

  if(!dev)
  {
    esWarning << "Failed to create OpenAL device, sound disabled." << std::endl;

    return;
  }

  context = createContext(dev, 0);

  if(!context)
  {
    esWarning << "Failed to create OpenAL context, sound disabled." << std::endl;

    return;
  }

  makeContextCurrent(context);
}

bool AL::initialized()
{
  return context;
}

AL::alEnable             AL::enable;
AL::alDisable            AL::disable;
AL::alIsEnabled          AL::isEnabled;
AL::alHint               AL::hint;
AL::alGetBoolean         AL::getBoolean;
AL::alGetInteger         AL::getInteger;
AL::alGetFloat           AL::getFloat;
AL::alGetDouble          AL::getDouble;
AL::alGetBooleanv        AL::getBooleanv;
AL::alGetIntegerv        AL::getIntegerv;
AL::alGetFloatv          AL::getFloatv;
AL::alGetDoublev         AL::getDoublev;
AL::alGetString          AL::getString;
AL::alGetError           AL::getError;
AL::alIsExtensionPresent AL::isExtensionPresent;
AL::alGetProcAddress     AL::getProcAddress;
AL::alGetEnumValue       AL::getEnumValue;

AL::alListenerf          AL::listenerf;
AL::alListenerfv         AL::listenerfv;
AL::alGetListeneri       AL::getListeneri;
AL::alGetListenerf       AL::getListenerf;
AL::alGetListeneriv      AL::getListeneriv;
AL::alGetListenerfv      AL::getListenerfv;

AL::alGenSources         AL::genSources;
AL::alDeleteSources      AL::deleteSources;
AL::alIsSource           AL::isSource;
AL::alSourcei            AL::sourcei;
AL::alSourcef            AL::sourcef;
AL::alSource3f           AL::source3f;
AL::alSourcefv           AL::sourcefv;
AL::alGetSourcei         AL::getSourcei;
AL::alGetSourceiv        AL::getSourceiv;
AL::alGetSourcef         AL::getSourcef;
AL::alGetSource3f        AL::getSource3f;
AL::alGetSourcefv        AL::getSourcefv;
AL::alSourcePlay         AL::sourcePlay;
AL::alSourcePause        AL::sourcePause;
AL::alSourceStop         AL::sourceStop;
AL::alSourceRewind       AL::sourceRewind;
AL::alSourcePlayv        AL::sourcePlayv;
AL::alSourcePausev       AL::sourcePausev;
AL::alSourceStopv        AL::sourceStopv;
AL::alSourceRewindv      AL::sourceRewindv;

AL::alGenBuffers         AL::genBuffers;
AL::alDeleteBuffers      AL::deleteBuffers;
AL::alIsBuffer           AL::isBuffer;
AL::alBufferData         AL::bufferData;
AL::alGetBufferi         AL::getBufferi;
AL::alGetBufferf         AL::getBufferf;
AL::alGetBufferiv        AL::getBufferiv;
AL::alGetBufferfv        AL::getBufferfv;

AL::alQueuei               AL::queuei;
AL::alSourceUnqueueBuffers AL::sourceUnqueueBuffers;
AL::alSourceQueueBuffers   AL::sourceQueueBuffers;

AL::alcCreateContext       AL::createContext;
AL::alcMakeContextCurrent  AL::makeContextCurrent;
AL::alcOpenDevice          AL::openDevice;
AL::alcDestroyContext      AL::destroyContext;

// vim: ts=2 sw=2 et
