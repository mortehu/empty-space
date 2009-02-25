#ifndef PLUGINS_MDC_H_
#define PLUGINS_MDC_H_ 1

#include <espace/plugins.h>

struct MDC : public ModelPlugin
{
  uint32_t id();
  bool canHandle(File& file);
  Model* read(File& file);
};

#endif // !PLUGINS_MDC_H_

// vim: ts=2 sw=2 et
