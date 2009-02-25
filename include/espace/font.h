#ifndef FONT_H_
#define FONT_H_ 1

#include "types.h"

struct Font
{
  struct Glyph
  {
    int   height;
    int   top;
    int   bottom;
    int   pitch;
    int   xSkip;
    int   imageWidth;
    int   imageHeight;
    float s1;
    float t1;
    float s2;
    float t2;
    int   shaderHandle;
    char  shaderName[32];
  };

  Glyph glyphs[256];
  float glyphScale;
  char  name[64];

  static IMPORT Font* acquire(const char* fileName);
};

#endif // !FONT_H_

// vim: ts=2 sw=2 et
