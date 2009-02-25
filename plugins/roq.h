#ifndef PLUGINS_ROQ_H_
#define PLUGINS_ROQ_H_ 1

#include <espace/plugins.h>

struct RoQ : public MediaPlugin
{
  uint32_t id();
  bool canHandle(File& file);
  Media* open(File* file);
};

#endif // !PLUGINS_ROQ_H_

// vim: ts=2 sw=2 et
