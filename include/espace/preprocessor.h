#ifndef PREPROCESSOR_H_
#define PREPROCESSOR_H_

#ifndef SWIG
#include "string.h"
#include "types.h"
#endif

/**
 * The Preprocessor class is a tokenizer and C-like preprocessor.
 * It supports #define and recursive #include's as a C preprocessor,
 * but not #ifdef and similar conditional statements.
 *
 * \author Morten Hustveit
 */
class IMPORT Preprocessor
{
public:

  /**
   * Opens a new file for preprocessing.
   *
   * \param filename The file to preprocess.
   */
  Preprocessor(const char* fileName);

  /**
   * Destroys a preprocessor object.
   */
  ~Preprocessor();

  /**
   * Check if file was successfully loaded.
   * \return True when the file was successfully opened, false otherwise.
   */
  bool isOpen() const;

  /**
   * \todo What's a token and what is not?
   * \return The next token.
   */
  String nextToken();

  /**
   * Put back a token to the top of the stack.
   * There are no limits to the amount of tokens unget'ed.
   */
  void unget(const String& token);

  /**
   * Preprocesses a file and adds the #define'ed symbols
   * to the current file.  No tokens are added to the stack. You do
   * not want to use this directly in your code, you should rather
   * #include the correct header in your data files.
   * \param fileName The file to extract symbols from.
   */
  void include(const char* fileName);

protected:

  class Internal;

  Internal* m;
};

#endif // !PREPROCESSOR_H_

// vim: ts=2 sw=2 et
