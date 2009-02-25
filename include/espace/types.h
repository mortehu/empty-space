#ifndef TYPES_H_
#define TYPES_H_

typedef unsigned int uint;

#ifndef __i386__
#  define __i386__ 0
#endif

#ifndef __powerpc__
#  define __powerpc__ 0
#endif

#if defined(WIN32) && !defined(espace_DLL)
#  define IMPORT __declspec(dllimport)
#elif defined(WIN32) // DLL
#  define IMPORT __declspec(dllexport)
#else // !WIN32
#  define IMPORT
#endif

#endif // !TYPES_H_

// vim: ts=2 sw=2 et
