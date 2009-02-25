#ifndef PLUGINS_MD3_H_
#define PLUGINS_MD3_H_ 1

#include <espace/plugins.h>

struct MD3 : public ModelPlugin
{
  uint32_t id();
  bool canHandle(File& file);
  Model* read(File& file);
};

#endif // !PLUGINS_MD3_H_

// vim: ts=2 sw=2 et
