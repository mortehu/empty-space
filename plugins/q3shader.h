#ifndef PLUGINS_Q3SHADER_H_
#define PLUGINS_Q3SHADER_H_ 1

#include <espace/plugins.h>

struct Q3ShaderData;

struct Q3Shader : public ShaderPlugin
{
  Q3Shader();

  uint32_t id();
  bool     canHandle(File& file);

  Shader*  acquire(const char* name, bool mipmaps = true);
};

#endif // !PLUGINS_Q3SHADER_H_

// vim: ts=2 sw=2 et
