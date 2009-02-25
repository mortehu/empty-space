#ifndef PLUGINS_MDS_H_
#define PLUGINS_MDS_H_ 1

#include <espace/plugins.h>

struct MDS : public ModelPlugin
{
  uint32_t id();
  bool canHandle(File& file);
  Model* read(File& file);
};

#endif // !PLUGINS_MDS_H_

// vim: ts=2 sw=2 et
