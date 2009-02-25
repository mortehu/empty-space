#ifndef PLUGINS_VORBIS_H_
#define PLUGINS_VORBIS_H_ 1

#include <espace/plugins.h>

struct Vorbis : public MediaPlugin
{
  uint32_t id();
  bool canHandle(File& file);
  Media* open(File* file);
};

#endif // !PLUGINS_VORBIS_H_

// vim: ts=2 sw=2 et
