#ifndef MEDIA_H_
#define MEDIA_H_

#ifndef SWIG
#include "image.h"
#include "types.h"
#endif

/**
 * Audio and video stream interface.
 * \author Morten Hustveit
 */
struct Media
{
  /**
   * Opens a media file for streaming.
   *
   * Returns NULL on error.
   */
  static IMPORT Media* acquire(const char* name);

  /**
   * Closes a media stream previously opened by acquire().
   */
  static IMPORT void unacquire(Media* media);

  enum Format
  {
    Mono8 = 0x1100,
    Mono16 = 0x1101,
    Stereo8 = 0x1102,
    Stereo16 = 0x1103
  };

  // VIDEO

  /**
   * Read one frame of video data.
   */
  virtual IMPORT Image& videoRead();

  /**
   * Skip video frames.
   *
   * \param count The amount of frames to skip.
   */
  virtual IMPORT void videoSkip(uint count);

  /**
   * Get the amount of frames per second.
   *
   * Returns 0 if stream contains no video.
   */
  virtual IMPORT uint videoFrameRate();

  /**
   * Returns whether end of file has been reached in the video stream.
   */
  virtual IMPORT bool videoEOF();

  /**
   * Repositions the video stream to the beginning.
   */
  virtual IMPORT void videoReset();

  // AUDIO

  /**
   * Get the amount of samples in audio stream.
   *
   * Returns 0 if the length is unknown.  If the stream contains no samples,
   * audioFrequency() also returns 0.
   */
  virtual IMPORT uint audioLength();

  /**
   * Reads audio data into a specified buffer.
   * \param buffer The buffer to read into
   * \param size   The amount of samples to read.
   * \return The amounts of samples read.  Returns less than size if, and
   *         only if, the end of the stream is reached.
   */
  virtual IMPORT uint audioRead(void* buffer, uint size);

  /**
   * Returns the format of the audio samples.
   */
  virtual IMPORT uint audioFormat();

  /**
   * Returns the amount of samples per second in the audio stream.
   *
   * Returns 0 if stream contains no audio.
   */
  virtual IMPORT uint audioFrequency();

  /**
   * Returns whether end of file has been reached in the audio stream.
   */
  virtual IMPORT bool audioEOF();

  /**
   * Repositions the audio stream to the beginning.
   */
  virtual IMPORT void audioReset();

protected:

  virtual IMPORT ~Media();
};

#endif // !MEDIA_H_

// vim: ts=2 sw=2 et
