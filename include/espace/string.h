#ifndef STRING_H_
#define STRING_H_

#include <string.h>

#include "types.h"

/**
 * ASCIIZ string container.
 *
 * \author Morten Hustveit
 */
class IMPORT String
{
public:

  static String null; /**< The NULL string. */
  static const int notfound = -1;

  /**
   * Creates an empty string.
   */
  String()
    : data(new Data)
  {
  }

  /**
   * Creates a copy of a string.
   */
  String(const char* s)
    : data(new Data(s))
  {
  }

  /**
   * Creates an uninitialized string of the specified length.
   */
  explicit String(int length)
    : data(new Data)
  {
    data->allocate(length);
  }

  /**
   * Creates a reference counted copy of a string.
   */
  String(const String& s)
    : data(s.data)
  {
    ++data->refCount;
  }

  /**
   * Destroys a string object.
   */
  ~String()
  {
    if(!--data->refCount)
      delete data;
  }

  /**
   * Creates a reference counted copy of a string.
   */
  String& operator=(const String& s)
  {
    if(&s == this)
      return *this;

    if(!--data->refCount)
      delete data;

    data = s.data;

    ++data->refCount;

    return *this;
  }

  /**
   * Get the length of the string.
   */
  unsigned int length() const
  {
    return data->length;
  }

  /**
   * Returns true if, and only if, this string is the NULL string.
   */
  bool isNull() const
  {
    return data->data == 0;
  }

  /**
   * Returns true if, and only if, the length of this string is 0.
   */
  bool isEmpty() const
  {
    return data->length == 0;
  }

  /**
   * Compare with another string for equality.
   */
  bool operator==(const char* str) const
  {
    if(!data->data)
      return !str;

    if(!str)
      return !data->data;

    unsigned int length = strlen(str);

    if(data->length != length)
      return false;

    return !memcmp(data->data, str, length);
  }

  /**
   * Compare with another string for equality.
   */
  bool operator==(const String& str) const
  {
    if(data->length != str.data->length)
      return false;

    if(!data->data)
      return !str.data->data;

    if(!str.data->data)
      return !data->data;

    return !memcmp(data->data, str.data->data, data->length);
  }

  /**
   * Compare with another string for inequality.
   */
  bool operator!=(const char* str) const
  {
    if(!data->data)
      return str;

    if(!str)
      return data->data;

    unsigned int length = strlen(str);

    if(data->length != length)
      return true;

    return memcmp(data->data, str, length);
  }

  /**
   * Compare with another string for inequality.
   */
  bool operator!=(const String& str) const
  {
    if(data->length != str.data->length)
      return true;

    if(!data->data)
      return str.data->data;

    if(!str.data->data)
      return data->data;

    return memcmp(data->data, str.data->data, data->length);
  }

  /**
   * Case sensitive less-than operator.
   */
  bool operator<(const String& str) const
  {
    if(!str.data->data)
      return false;

    if(!data->data)
      return true;

    return strcmp(data->data, str.data->data) < 0;
  }

  /**
   * Case sensitive less-than-or-equal operator.
   */
  bool operator<=(const String& str) const
  {
    if(!data->data)
      return true;

    if(!str.data->data)
      return false;

    return strcmp(data->data, str.data->data) <= 0;
  }

  /**
   * Case sensitive greater-than operator.
   */
  bool operator>(const String& str) const
  {
    if(!data->data)
      return false;

    if(!str.data->data)
      return true;

    return strcmp(data->data, str.data->data) > 0;
  }

  /**
   * Case sensitive greater-than-or-equal operator.
   */
  bool operator>=(const String& str) const
  {
    if(!str.data->data)
      return true;

    if(!data->data)
      return false;

    return strcmp(data->data, str.data->data) >= 0;
  }

  /**
   * Returns a C string representation of this string.
   */
  operator const char*() const
  {
    return data->data;
  }

  /**
   * Returns a C string representation of this string.
   */
  operator char*()
  {
    return data->data;
  }

  /**
   * Concatenate with another string.
   */
  String operator+(const char* str) const;

  /**
   * concatenate with another string.
   */
  String  operator+(const String& str) const;

  /**
   * concatenate with another string.
   */
  String& operator+=(char c);

  /**
   * concatenate with another string.
   */
  String& operator+=(const char* str);

  /**
   * Remove all whitespace from start and end of string.
   */
  String stripWhiteSpace() const;

  /**
   * Replace all occurences of a character with another character.
   */
  String& replace(char before, char after);

  /**
   * Replace all occurences of a substring with another string.
   */
  String& replace(const String& before, const String& after);

  /**
   * Find first occurence of a substring.
   *
   * Returns String::notfound if the substring is not found.
   */
  int find(const char* str) const;

  /**
   * Check whether this string contains a specified substring.
   */
  bool contains(const char* str, bool cs = true) const;

  /**
   * Check whether this string begins with a specified substring.
   */
  bool beginsWith(const char* str) const;

  /**
   * Check whether this string ends with a specified substring.
   */
  bool endsWith(const char* str) const;

  /**
   * Return the left part of this string.
   *
   * If the length given is longer than the length of the string, the entire
   * string is returned.
   */
  String left(unsigned int length) const;

  /**
   * Return the right part of this string.
   *
   * If the length given is longer than the length of the string, the entire
   * string is returned.
   */
  String right(unsigned int length) const;

  /**
   * Return a substring of this string.
   *
   * \param pos Start position of substring to return.
   * \param length Length of substring to return.
   */
  String mid(unsigned int pos, unsigned int length) const;

  /**
   * Return a lowercase representation of this string.
   */
  String lower() const;

  /**
   * Return an uppercase representation of this string.
   */
  String upper() const;

  /**
   * Convert this string to lowercase in-place.
   */
  String& toLower();

  /**
   * Convert this string to uppercase in-place.
   */
  String& toUpper();

  /**
   * Convert this string to a signed integer.
   */
  int toInt(bool* ok = 0) const;

  /**
   * Convert this string to a unsigned integer.
   */
  unsigned int toUInt(bool* ok = 0) const;

  /**
   * Convert this string to a floating point value.
   */
  float toFloat(bool* ok = 0) const;

  /**
   * Return the string representation of an integer.
   */
  static String number(int number);

  /**
   * Return the string representation of a floating point value.
   */
  static String number(double number);

  /*
   * Return a string created by a `printf' style format.
   */
  static String format(const char* fmt, ...);

protected:

  void detach()
  {
    if(data->refCount > 1)
    {
      --data->refCount;

      data = new Data(data->data);
    }
  }

  class Data
  {
  public:

    Data()
      : data(0),
        length(0),
        refCount(1)
    {
    }

    Data(const char* str)
      : refCount(1)
    {
      allocate(strlen(str));

      memcpy(data, str, length);
    }

    ~Data()
    {
      if(data)
        delete [] data;
    }

    static const int blockSize = 16;

    void allocate(unsigned int n)
    {
      length = n;

      data = new char[(length + blockSize) & ~(blockSize - 1)];

      data[length] = 0;
    }

    void reallocate(unsigned int n)
    {
      if(length && (n + 1) <= ((length + blockSize) & ~(blockSize - 1)))
      {
        length = n;

        data[length] = 0;

        return;
      }

      char* olddata = data;
      unsigned int oldlength = length;

      allocate(n);

      if(olddata)
      {
        memcpy(data, olddata, oldlength + 1);

        delete [] olddata;
      }
    }

    char* data;
    unsigned int length;
    unsigned int refCount;
  };

  Data* data;
};

inline bool operator==(const char* a, const String& b)
{
  return b == a;
}

inline bool operator!=(const char* a, const String& b)
{
  return b != a;
}

inline String operator+(const char* a, const String& b)
{
  return String(a) + b;
}

#endif

// vim: ts=2 sw=2 et
