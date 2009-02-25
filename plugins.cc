/***************************************************************************
                          plugins.cc  -  Plugin interface
                               -------------------
      copyright            : (C) 2003 by Morten Hustveit
      email                : morten@debian.org
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <map>

#include <espace/file.h>
#include <espace/system.h>
#include <espace/output.h>
#include <espace/plugins.h>
#include <espace/predicates.h>

PluginMap(Archive) Plugin::archive;
PluginMap(Image)   Plugin::image;
PluginMap(Model)   Plugin::model;
PluginMap(Media)   Plugin::media;
PluginMap(Map)     Plugin::map;
PluginMap(Shader)  Plugin::shader;
PluginMap(Font)    Plugin::font;

typedef void (*t_dllfunc)(Plugin*** plugins, uint32_t* count);

static t_dllfunc dllfunc;

void Plugin::initialize()
{
  std::vector<String> plugins;

#ifndef WIN32
  File::readDirectory(String(LIBDIR) + "/empty-space");

  File::find(ContainsEndsWith("plugin_", ".so"), plugins);
#else
  File::readDirectory("./plugins");

  File::find(ContainsEndsWith("plugin_", ".dll"), plugins);
#endif

  for(std::vector<String>::iterator i = plugins.begin();
      i != plugins.end(); ++i)
  {
    String pluginPath = File::realName(*i);

    void* sohandle = System::dlopen(pluginPath);

    if(!sohandle)
    {
      esWarning << System::dlerror() << std::endl;

      continue;
    }

    if(!(dllfunc = (t_dllfunc) System::dlsym(sohandle, "plugins")))
    {
      esWarning << System::dlerror() << std::endl;

      System::dlclose(sohandle);

      continue;
    }

    Plugin** plugins;
    uint32_t pluginCount;

    dllfunc(&plugins, &pluginCount);

    esInfo << "Loading " << pluginCount << " plugins from \"" << *i
           << "\"." << std::endl;

    for(uint j = 0; j < pluginCount; ++j)
    {
      switch(plugins[j]->type())
      {
      case ArchivePluginType:

        archive[plugins[j]->id()] = dynamic_cast<ArchivePlugin*>(plugins[j]);

        break;

      case ImagePluginType:

        image[plugins[j]->id()] = dynamic_cast<ImagePlugin*>(plugins[j]);

        break;

      case ModelPluginType:

        model[plugins[j]->id()] = dynamic_cast<ModelPlugin*>(plugins[j]);

        break;

      case MediaPluginType:

        media[plugins[j]->id()] = dynamic_cast<MediaPlugin*>(plugins[j]);

        break;

      case MapPluginType:

        map[plugins[j]->id()] = dynamic_cast<MapPlugin*>(plugins[j]);

        break;

      case ShaderPluginType:

        shader[plugins[j]->id()] = dynamic_cast<ShaderPlugin*>(plugins[j]);

        break;

      case FontPluginType:

        font[plugins[j]->id()] = dynamic_cast<FontPlugin*>(plugins[j]);

        break;
      }
    }

    delete [] plugins;
  }
}

Plugin::Type ArchivePlugin::type()
{
  return Plugin::ArchivePluginType;
}

Plugin::Type ImagePlugin::type()
{
  return Plugin::ImagePluginType;
}

Plugin::Type ModelPlugin::type()
{
  return Plugin::ModelPluginType;
}

Plugin::Type MediaPlugin::type()
{
  return Plugin::MediaPluginType;
}

Plugin::Type MapPlugin::type()
{
  return Plugin::MapPluginType;
}

Plugin::Type ShaderPlugin::type()
{
  return Plugin::ShaderPluginType;
}

Plugin::Type FontPlugin::type()
{
  return Plugin::FontPluginType;
}

// vim: ts=2 sw=2 et
