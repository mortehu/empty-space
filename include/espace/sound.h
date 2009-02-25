#ifndef SOUND_H_
#define SOUND_H_

#ifndef SWIG
#include "types.h"
#include "media.h"
#include "vector.h"
#endif

/**
 * Sound interface.
 *
 * A sound is different from an audio-only media stream in that it is always
 * loaded completely into memory, to allow rapid response, and multiple
 * simultaneous asynchronous playbacks.
 *
 * \author Morten Hustveit
 */
class IMPORT Sound
{
public:

  /**
   * Initialize audio driver.
   *
   * This function must be called before any playback functionality can be used,
   * and before any sounds can be acquired.
   */
  static void initialize();

  /**
   * Acquire a sound.
   *
   * This function opens a media stream and reads all its audio data into
   * memory.
   *
   * Returns NULL if an error occurs.
   *
   * \param name File name of the sound to read.
   * \param keep Whether to keep the sound in memory after uploading to sound
   *             driver.
   */
  static Sound* acquire(const char* name, bool keep = false);

  /**
   * Acquire an integer handle for a sound.
   *
   * This function opens a media stream and reads all its audio data into
   * memory.
   *
   * Returns 0 if an error occurs.
   *
   * \param name File name of the sound to read.
   * \param keep Whether to keep the sound in memory after uploading to sound
   *            driver.
   */
  static uint acquireHandle(const char* name, bool keep = false);

  /**
   * Returns the pointer handle for a given integer handle.
   *
   * Returns 0 if the handle is invalid.
   */
  static Sound* soundForHandle(uint handle);

  /**
   * Unacquire a sound previously acquired by a call to acquire().
   */
  static void unacquire(Sound* sound);

  /**
   * Sets the position of the listener.
   */
  static void setPosition(const Vector3&);

  /**
   * Sets the velocity of the listener.
   */
  static void setVelocity(const Vector3&);

  /**
   * Sets the orientation of the listener.
   */
  static void setOrientation(const Vector3& direction, const Vector3& up);

  uint  format;
  char* data;
  uint  size;
  uint  freq;

  /**
   * Returns the amount of bytes per sample for this sound.
   */
  uint bytesPerSample() const;

protected:

  friend class SoundSource;
  friend class System;

  static void idle();

  Sound();
  ~Sound();

  uint handle;
  uint refCount;
};

/**
 * An audio channel with associated parameters.
 */
class IMPORT SoundSource
{
public:

  /**
   * Creates a new sound source.
   */
  SoundSource();

  /**
   * Copies a sound source.
   *
   * This function's primary purpose is to allow SoundSource() to be used in STL
   * containers.
   */
  SoundSource(const SoundSource& source);

  /**
   * Destroy a sound source.
   */
  ~SoundSource();

  /**
   * Sets the position.
   */
  void setPosition(const Vector3&);

  /**
   * Sets the velocity.
   */
  void setVelocity(const Vector3&);

  /**
   * Sets the gain (volume).
   *
   * The value should be in the range [0, 1].
   */
  void setGain(float);

  /**
   * Sets the maximum distance from which sounds are unattenuated.
   */
  void setReferenceDistance(float);

  /**
   * Plays a sound.
   */
  void play(const Sound*, bool loop = false);

  /**
   * Plays a Media stream.
   */
  void play(Media*, bool loop = false);

  /**
   * Enqueues a sound for playback.
   */
  void enqueue(const Sound*, bool loop = false);

  /**
   * Stops playback.
   */
  void stop();

  /**
   * Returns true if, and only if, the stream is not currently playing.
   */
  bool isStopped();

protected:

  struct DataProxy
  {
    uint refCount;
    uint handle;
  };

  DataProxy* data;
};

#endif // !SOUND_H_

// vim: ts=2 sw=2 et
