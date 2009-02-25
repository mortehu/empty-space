#ifndef PLUGINS_GIF_H_
#define PLUGINS_GIF_H_ 1

#include <espace/plugins.h>

struct GIF : public ImagePlugin
{
  uint32_t id();
  bool canHandle(File& file);
  Image* read(File& file);
};

#endif // !PLUGINS_GIF_H_

// vim: ts=2 sw=2 et
