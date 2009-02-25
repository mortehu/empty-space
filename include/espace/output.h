#ifndef OUTPUT_H_
#define OUTPUT_H_

#include <ostream>

#include "types.h"

/**
 * \file output.h
 * \brief Output stream declarations.
 *
 * There are four standard C++ ostreams provided for output:
 *  - esError
 *  - esWarning
 *  - esDebug
 *  - esInfo
 *
 * Messages printed using these streams are printed both in the Console and on
 * standard error.
 *
 * esError, esWarning and esDebug prepends new lines with `Error:', `Warning'
 * and `Debug:'.
 *
 * \see Console
 */

extern IMPORT std::ostream esError;   /**< Errors considered \b fatal. */
extern IMPORT std::ostream esWarning; /**< Abnormal events. */
extern IMPORT std::ostream esInfo;    /**< Normal messages. */

/**
 * Returns the appropriate stream for debugging messages at the given level.
 *
 * 0 - Always display when debugging is not disabled.
 * 1 - Display at level 2 or above.
 * 2 - Display at level 3 or above.
 * 3 - Display at level 4 only.
 */
extern IMPORT std::ostream& esDebug(int level = 0);

struct Vector2;
struct Vector3;
struct Matrix3x3;
struct Matrix4x4;

extern IMPORT std::ostream& operator<<(std::ostream&, const Vector2&);
extern IMPORT std::ostream& operator<<(std::ostream&, const Vector3&);
extern IMPORT std::ostream& operator<<(std::ostream&, const Matrix3x3&);
extern IMPORT std::ostream& operator<<(std::ostream&, const Matrix4x4&);

#endif // !OUTPUT_H_

// vim: ts=2 sw=2 et
