#ifndef FILE_H_
#define FILE_H_

#ifndef SWIG
#include <vector>
#include <stdint.h>

#include "string.h"
#include "types.h"
#include "vector.h"

class ArchivePlugin;

typedef struct FileEntry
{
  uint32_t type;
  String   name;
  String   archive;
  uint32_t offset;
  uint32_t size;
  uint8_t  flag;
  uint32_t key;
};
#endif

/**
 * Buffered file I/O.
 * \author Morten Hustveit
 *
 * This class provides endian safe buffered file I/O.  Files can also be
 * entries in archives such as tar-balls and zip-files.
 */
class File
{
public:

  enum Flags
  {
    Read = 0x0001,
    Write = 0x0002,
    Truncate = 0x0004
  };

  /**
   * Open a file for reading or writing.
   *
   * \see isOpen()
   */
  File(const char* fileName, uint flags = Read, uint bufferSize = 65536) IMPORT;

  /**
   * Closes the file and destroys the file object.
   */
  IMPORT ~File();

  /**
   * Add contents of directory to list of searchable files.
   */
  static IMPORT bool readDirectory(const char* directory, const char* prefix = "");

  /**
   * Real name of file.
   * \return The archive containing the file or String::null.
   */
  static IMPORT String realName(const char* name);

#ifndef SWIG
  typedef void (*FileHook)(const char* name);

  /**
   * Adds a hook to be called for each file found by readDirectory().
   */
  static IMPORT void addFileHook(FileHook);

  typedef std::vector<FileEntry> EntryList;

  /**
   * Search for files.  Finds files matching the given predicate
   * and puts them in the collection.
   *
   * A small example:
   * <pre>
   * #include &lt;espace/file.h&gt;
   * #include &lt;espace/string.h&gt;
   * #include &lt;espace/predicates.h&gt;
   * #include &lt;vector&gt;
   *
   * std::vector<String> scripts;
   *
   * File::find(EndsWith(".script"), scripts);
   * </pre>
   *
   * \param predicate A functor with a operator() taking a String-object or
   *                  compatible (such as const char*).
   * \param collection A STL-like container class supporting the push_back
   *                   method.
   */
  template<class Function, class Collection>
  static void find(Function predicate, Collection& result)
  {
    for(EntryList::const_iterator i = entries.begin();
        i != entries.end(); ++i)
    {
      if(predicate((*i).name))
        result.push_back((*i).name);
    }
  }
#endif

  /**
   * Time of last modification.
   * \return The time of last modification, given in seconds since Epoch (00:00:00 UTC, January 1, 1970).
   */
  IMPORT uint modified();

  /**
   * Check if file was successfully opened.
   * \return True on success.
   */
  bool isOpen() const
  {
    return fd >= 0;
  }

  /**
   * Reads eight bit integer from file.
   */
  IMPORT uint8_t  getU8();

  /**
   * Reads 16 bit integer from file.  Endian safe.
   */
  IMPORT uint16_t getU16();

  /**
   * Reads 32 bit integer from file.  Endian safe.
   */
  IMPORT uint32_t getU32();

  /**
   * Reads signed eight bit integer from file.
   */
  int8_t getS8()
  {
    return static_cast<int8_t>(getU8());
  }

  /**
   * Reads signed 16 bit integer from file.
   */
  int16_t getS16()
  {
    return static_cast<int16_t>(getU16());
  }

  /**
   * Reads signed 32 bit integer from file.
   */
  int32_t getS32()
  {
    return static_cast<int32_t>(getU32());
  }

  /**
   * Read a 32 bit float from file.  Endian safe.
   */
  float getFloat()
  {
    uint32_t tmp = getU32();
    return *reinterpret_cast<float*>(&tmp);
  }

  /**
   * Read a Vector2 (Two 32 bit floats) from file.  Endian safe.
   */
  Vector2 getVector2()
  {
    Vector2 tmp;
    tmp(0) = getFloat();
    tmp(1) = getFloat();
    return tmp;
  }

  /**
   * Read a Vector3 (Three 32 bit floats) from file.  Endian safe.
   */
  Vector3 getVector3()
  {
    Vector3 tmp;
    tmp(0) = getFloat();
    tmp(1) = getFloat();
    tmp(2) = getFloat();
    return tmp;
  }

  /**
   * Read a line.
   * Reads the line until a newline-character is reached. Both <pre>\\r</pre> and <pre>\\n</pre>
   * is stripped from the string.
   * \return String without newline-character.
   */
  IMPORT String readLine();

  /**
   * Read data from file.
   * \warning This is not endian safe.  You will need to swap the
   *          data in the buffer.
   * \param buffer Pointer to buffer with at least `count' bytes.
   * \param count  Number of bytes to write to buffer.
   */
  IMPORT void read(void* buffer, uint count);

  /**
   * Returns all the contents of a file.
   * \warning Will allocate a lot of memory if the file large and the platform
   *          does not support mmap.
   */
  IMPORT uint8_t* data();

  /**
   * Get file size.
   * \return Size of file, in bytes.
   */
  uint length() const
  {
    return fileSize;
  }

  /**
   * Get position.
   * \return Current position, in bytes.
   */
  uint tell() const
  {
    return bufferPosition + position;
  }

  /**
   * Seek to position in file.
   * \param position The absolute position, given in bytes.
   */
  IMPORT void seek(uint position);

  /**
   * Jump forward when reading.
   * \param amount The number of bytes to skip.
   */
  IMPORT void skip(uint amount);

  /**
   * Write eight bits to file.
   */
  IMPORT void put(uint8_t);

  /**
   * Write 16 bits to file.  Endian-safe.
   */
  IMPORT void put(uint16_t);

  /**
   * Write 32 bits to file.  Endian-safe.
   */
  IMPORT void put(uint32_t);

  /**
   * Write eight bits to file.
   */
  void put(int8_t v)
  {
    put(static_cast<uint8_t>(v));
  }

  /**
   * Write 16 bits to file.  Endian-safe.
   */
  void put(int16_t v)
  {
    put(static_cast<uint16_t>(v));
  }

  /**
   * Write 32 bits to file.  Endian-safe.
   */
  void put(int32_t v)
  {
    put(static_cast<uint32_t>(v));
  }

  /**
   * Write a 32 bit floating point number to file.  Endian-safe.
   */
  void put(float v)
  {
    put(*reinterpret_cast<uint32_t*>(&v));
  }

  /**
   * Write a Vector2 (two 32 bit floats) to file.  Endian safe.
   */
  void put(const Vector2& v)
  {
    put(v(0)); put(v(1));
  }

  /**
   * Write a Vector3 (three 32 bit floats) to file.  Endian safe.
   */
  void put(const Vector3& v)
  {
    put(v(0)); put(v(1)); put(v(2));
  }

  /**
   * Writes a String to the file, not including the NULL-byte.
   */
  IMPORT void put(const String&);

  /**
   * Write data to file.
   * \warning This is not endian safe.  You will need to swap the
   *          data in the buffer.
   * \param buffer Pointer to buffer with at least `count' bytes.
   * \param count  Number of bytes to read from the buffer.
   */
  IMPORT void write(const void* buffer, uint count);

  /**
   * Writes the output buffer to the file if the file was opened for writing.
   */
  IMPORT void flush();

  /**
   * Check if we have reached the end of the file.
   * \returns True on end of file.
   */
  IMPORT bool eof() const;

protected:

  int fd;
  uint bufferSize;
  unsigned char* buffer;
  uint bufferPosition;
  uint position;
  uint fileSize;
  bool writing;
  ArchivePlugin* archive;

  void sysRead(void* buffer, uint length);
  void sysSeek(uint position);

  static IMPORT EntryList entries;
};

#endif // !FILE_H_

// vim: ts=2 sw=2 et
