#include <espace/file.h>
#include <espace/font.h>
#include <espace/output.h>
#include <espace/plugins.h>
#include <espace/string.h>

Font* Font::acquire(const char* _fileName)
{
  String fileName(_fileName);

  File file(fileName);

  if(!file.isOpen())
  {
    esWarning << "Font: Failed to open \"" << fileName << "\"."
              << std::endl;

    return 0;
  }

  Font* font = 0;

  for(PluginMap(Font)::iterator i = Plugin::font.begin();
      i != Plugin::font.end(); ++i)
  {
    if(i->second->canHandle(file))
    {
      font = i->second->read(file);

      if(font)
        return font;
    }
  }

  esWarning << "Font: No plugin found for \"" << fileName
            << "\"." << std::endl;

  return 0;
}

