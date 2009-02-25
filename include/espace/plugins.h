#ifndef PLUGINS_H_
#define PLUGINS_H_

#include <map>
#include <vector>
#include <stdint.h>

#include "string.h"
#include "types.h"

/**
 * \file plugins.h
 * \brief Plugin interface.
 * \author Morten Hustveit
 *
 * A plugin library must implement:
 *
 * <pre>
 * extern "C" void plugins(Plugin*** plugins, uint32_t* count);
 * </pre>
 */

struct File;
struct FileEntry;
struct String;

struct Image;
struct Model;
struct Sound;
struct Media;
struct Map;
struct Shader;
struct Font;

struct ArchivePlugin;
struct ImagePlugin;
struct ModelPlugin;
struct MediaPlugin;
struct MapPlugin;
struct ShaderPlugin;
struct FontPlugin;

#define PluginMap(type) \
  std::map<uint, type##Plugin*, std::less<uint>, \
           std::allocator<std::pair<const uint, type##Plugin*> > >

/**
 * The Plugin base struct.  The different plugin types will inherit
 * this struct, and the plugins should be derived structes of their plugin
 * type struct.
 */
struct Plugin
{
  static IMPORT void initialize();

  /**
   * The different plugin types.
   */
  enum Type
  {
    ArchivePluginType = 0,
    ImagePluginType = 1,
    ModelPluginType = 2,
    MediaPluginType = 4,
    MapPluginType = 5,
    ShaderPluginType = 6,
    FontPluginType = 7
  };

  /**
   * Get plugin type.
   * \return Type which the plugin is a substruct of.
   * \see enum Type
   */
  virtual IMPORT Type type() = 0;

  /**
   * Unique plugin magic.
   */
  virtual IMPORT uint32_t id() = 0;

  /**
   * Check if plugin can handle file.
   * \return True on success parsing the file.
   */
  virtual IMPORT bool canHandle(File& file) = 0;

  static IMPORT PluginMap(Archive) archive;
  static IMPORT PluginMap(Image)   image;
  static IMPORT PluginMap(Model)   model;
  static IMPORT PluginMap(Media)   media;
  static IMPORT PluginMap(Map)     map;
  static IMPORT PluginMap(Shader)  shader;
  static IMPORT PluginMap(Font)    font;
};

/**
 * A plugin interface for using Archives.
 */
struct ArchivePlugin : public Plugin
{
  IMPORT Type type();

  virtual IMPORT void scan(File& archive, const String& archiveName,
                           std::vector<FileEntry,
                                       std::allocator<FileEntry> >& entries) = 0;

  virtual IMPORT int  open(const FileEntry& entry) = 0;
  virtual IMPORT void read(int handle, void* buffer, uint count) = 0;
  virtual IMPORT void seek(int handle, uint position) = 0;
  virtual IMPORT void close(int handle) = 0;
};

/**
 * A plugin interface for opening Images.
 */
struct ImagePlugin : public Plugin
{
  IMPORT Type type();

  /**
   * Open a Image file with the given name.
   * \return A pointer to the Image object on success, NULL on failure.
   */
  virtual IMPORT Image* read(File& file) = 0;
};

/**
 * A plugin interface for opening Models.
 */
struct ModelPlugin : public Plugin
{
  IMPORT Type type();

  /**
   * Open a Model file with the given name.
   * \return A pointer to the Model object on success, NULL on failure.
   */
  virtual IMPORT Model* read(File& file) = 0;
};

/**
 * A plugin interface for opening Media streams.
 */
struct MediaPlugin : public Plugin
{
  IMPORT Type type();

  /**
   * Open a Media file with the given name.
   * \return A pointer to the Media object on success, NULL on failure.
   */
  virtual IMPORT Media* open(File* file) = 0;
};

/**
 * A plugin interface for opening Maps.
 */
struct MapPlugin : public Plugin
{
  IMPORT Type type();

  /**
   * Open a map with the given name.
   * \return A pointer to the Map object on success, NULL on failure.
   */
  virtual IMPORT Map* read(File& file) = 0;
};

/**
 * A plugin interface for opening Shaders.
 */
struct ShaderPlugin : public Plugin
{
  IMPORT Type type();

  /**
   * Open a shader with the given name.
   * \return A pointer to the shader on success, NULL on failure.
   */
  virtual IMPORT Shader* acquire(const char* name, bool mipmaps = true) = 0;
};

/**
 * A plugin for reading Fonts.
 */
struct FontPlugin : public Plugin
{
  IMPORT Type type();

  /**
   * Read a font with the given discreption.
   * \return A pointer to the font on success, NULL on failure.
   */
  virtual IMPORT Font* read(File& file) = 0;
};

#endif // !PLUGINS_H_

// vim: ts=2 sw=2 et
