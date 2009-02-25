#ifndef OPENAL_H_
#define OPENAL_H_

#define AL_NO_PROTOTYPES
#include <AL/al.h>
#include <AL/alc.h>

#ifdef _WIN32
struct _AL_device;
typedef struct _AL_device ALCdevice;
#endif

struct AL
{
  static void initialize();
  static bool initialized();

  typedef void      (*alEnable)(ALenum capability);
  typedef void      (*alDisable)(ALenum capability);
  typedef ALboolean (*alIsEnabled)(ALenum capability);
  typedef void      (*alHint)(ALenum target, ALenum mode);
  typedef ALboolean (*alGetBoolean)(ALenum param);
  typedef ALint     (*alGetInteger)(ALenum param);
  typedef ALfloat   (*alGetFloat)(ALenum param);
  typedef ALdouble  (*alGetDouble)(ALenum param);
  typedef void      (*alGetBooleanv)(ALenum param, ALboolean* data);
  typedef void      (*alGetIntegerv)(ALenum param, ALint* data);
  typedef void      (*alGetFloatv)(ALenum param, ALfloat* data);
  typedef void      (*alGetDoublev)(ALenum param, ALdouble* data);
  typedef const ALubyte* (*alGetString)(ALenum param);
  typedef ALenum    (*alGetError)();
  typedef ALboolean (*alIsExtensionPresent)(const ALubyte* fname);
  typedef void*     (*alGetProcAddress)(const ALubyte* fname);
  typedef ALenum    (*alGetEnumValue)(const ALubyte* ename);
  typedef void      (*alListenerf)(ALenum pname, ALfloat param);
  typedef void      (*alListenerfv)(ALenum pname, ALfloat* param);
  typedef void      (*alGetListeneri)(ALenum pname, ALint* value);
  typedef void      (*alGetListenerf)(ALenum pname, ALfloat* value);
  typedef void      (*alGetListeneriv)(ALenum pname, ALint* values);
  typedef void      (*alGetListenerfv)(ALenum pname, ALfloat* values);
  typedef void      (*alGenSources)(ALsizei n, ALuint* sources);
  typedef void      (*alDeleteSources)(ALsizei n, ALuint* sources);
  typedef ALboolean (*alIsSource)(ALuint sid);
  typedef void      (*alSourcei)(ALuint sid, ALenum param, ALint value);
  typedef void      (*alSourcef)(ALuint sid, ALenum param, ALfloat value);
  typedef void      (*alSource3f)(ALuint sid, ALenum param, ALfloat f1,
                                  ALfloat f2, ALfloat f3);
  typedef void      (*alSourcefv)(ALuint sid, ALenum param, ALfloat* values);
  typedef void      (*alGetSourcei)(ALuint sid, ALenum pname, ALint* value);
  typedef void      (*alGetSourceiv)(ALuint sid, ALenum pname, ALint* values);
  typedef void      (*alGetSourcef)(ALuint sid, ALenum pname, ALfloat* value);
  typedef void      (*alGetSource3f)(ALuint sid, ALenum pname, ALfloat* value1,
                                     ALfloat* value2, ALfloat* value3);
  typedef void      (*alGetSourcefv)(ALuint sid, ALenum pname, ALfloat* values);
  typedef void      (*alSourcePlay)(ALuint sid);
  typedef void      (*alSourcePause)(ALuint sid);
  typedef void      (*alSourceStop)(ALuint sid);
  typedef void      (*alSourceRewind)(ALuint sid);
  typedef void      (*alSourcePlayv)(ALsizei ns, ALuint *ids);
  typedef void      (*alSourceStopv)(ALsizei ns, ALuint *ids);
  typedef void      (*alSourceRewindv)(ALsizei ns, ALuint *ids);
  typedef void      (*alSourcePausev)(ALsizei ns, ALuint *ids);
  typedef void      (*alGenBuffers)(ALsizei n, ALuint* buffers);
  typedef void      (*alDeleteBuffers)(ALsizei n, ALuint* buffers);
  typedef ALboolean (*alIsBuffer)(ALuint buffer);
  typedef void      (*alBufferData)(ALuint buffer, ALenum format, ALvoid* data,
                                    ALsizei size, ALsizei freq);
  typedef void      (*alGetBufferi)(ALuint buffer, ALenum param, ALint* value);
  typedef void      (*alGetBufferf)(ALuint buffer, ALenum param, ALfloat* value);
  typedef void      (*alGetBufferiv)(ALuint buffer, ALenum param, ALint* value);
  typedef void      (*alGetBufferfv)(ALuint buffer, ALenum param, ALfloat* value);

  typedef void      (*alQueuei)(ALuint sid, ALenum param, ALint value);
  typedef void      (*alSourceUnqueueBuffers)(ALuint sid, ALsizei numEntries,
                                              ALuint *bids);
  typedef void      (*alSourceQueueBuffers)(ALuint sid, ALsizei numEntries,
                                            ALuint *bids);

  typedef void* (*alcCreateContext)(ALCdevice *dev, ALint* attrlist);
  typedef ALCenum   (*alcMakeContextCurrent)(void *alcHandle);
  typedef ALCdevice* (*alcOpenDevice)(const ALubyte *tokstr);
  typedef ALCenum (*alcDestroyContext)(void *alcHandle);

  static alEnable               enable;
  static alDisable              disable;
  static alIsEnabled            isEnabled;
  static alHint                 hint;
  static alGetBoolean           getBoolean;
  static alGetInteger           getInteger;
  static alGetFloat             getFloat;
  static alGetDouble            getDouble;
  static alGetBooleanv          getBooleanv;
  static alGetIntegerv          getIntegerv;
  static alGetFloatv            getFloatv;
  static alGetDoublev           getDoublev;
  static alGetString            getString;
  static alGetError             getError;
  static alIsExtensionPresent   isExtensionPresent;
  static alGetProcAddress       getProcAddress;
  static alGetEnumValue         getEnumValue;

  static alListenerf            listenerf;
  static alListenerfv           listenerfv;
  static alGetListeneri         getListeneri;
  static alGetListenerf         getListenerf;
  static alGetListeneriv        getListeneriv;
  static alGetListenerfv        getListenerfv;

  static alGenSources           genSources;
  static alDeleteSources        deleteSources;
  static alIsSource             isSource;
  static alSourcei              sourcei;
  static alSourcef              sourcef;
  static alSource3f             source3f;
  static alSourcefv             sourcefv;
  static alGetSourcei           getSourcei;
  static alGetSourceiv          getSourceiv;
  static alGetSourcef           getSourcef;
  static alGetSource3f          getSource3f;
  static alGetSourcefv          getSourcefv;
  static alSourcePlay           sourcePlay;
  static alSourcePause          sourcePause;
  static alSourceStop           sourceStop;
  static alSourceRewind         sourceRewind;
  static alSourcePlayv          sourcePlayv;
  static alSourcePausev         sourcePausev;
  static alSourceStopv          sourceStopv;
  static alSourceRewindv        sourceRewindv;

  static alGenBuffers           genBuffers;
  static alDeleteBuffers        deleteBuffers;
  static alIsBuffer             isBuffer;
  static alBufferData           bufferData;
  static alGetBufferi           getBufferi;
  static alGetBufferf           getBufferf;
  static alGetBufferiv          getBufferiv;
  static alGetBufferfv          getBufferfv;

  static alQueuei               queuei;
  static alSourceUnqueueBuffers sourceUnqueueBuffers;
  static alSourceQueueBuffers   sourceQueueBuffers;

  static alcCreateContext       createContext;
  static alcMakeContextCurrent  makeContextCurrent;
  static alcOpenDevice          openDevice;
  static alcDestroyContext      destroyContext;
};

#endif // !OPENAL_H_

// vim: ts=2 sw=2 et
