#ifndef PLUGINS_SIMPLESHADER_H_
#define PLUGINS_SIMPLESHADER_H_ 1

#include <espace/plugins.h>

struct SimpleShader : public ShaderPlugin
{
  uint32_t id();
  bool     canHandle(File& file);

  void     initialize();

  Shader*  acquire(const char* name, bool mipmaps = true);
};

#endif // !PLUGINS_SIMPLESHADER_H_

// vim: ts=2 sw=2 et
