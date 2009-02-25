#ifndef PLUGINS_JPEG_H_
#define PLUGINS_JPEG_H_

#include <espace/plugins.h>

struct JPEG : public ImagePlugin
{
  uint32_t id();
  bool canHandle(File& file);
  Image* read(File& file);
};

#endif // !PLUGINS_JPEG_H_

// vim: ts=2 sw=2 et
