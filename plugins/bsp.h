#ifndef PLUGINS_BSP_H_
#define PLUGINS_BSP_H_ 1

#include <espace/plugins.h>

struct BSP : public MapPlugin
{
  uint32_t id();
  bool canHandle(File& file);
  Map* read(File& file);
};


#endif // !PLUGINS_BSP_H_

// vim: ts=2 sw=2 et
