#ifndef PLUGINS_RTCWFONT_H_
#define PLUGINS_RTCWFONT_H_ 1

#include <espace/plugins.h>

struct Font;

struct RtCWFont : public FontPlugin
{
  uint32_t id();
  bool canHandle(File& file);
  Font* read(File& file);
};

#endif // !PLUGINS_RTCWFONT_H_

// vim: ts=2 sw=2 et
