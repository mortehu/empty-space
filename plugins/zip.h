#ifndef PLUGINS_ZIP_H_
#define PLUGINS_ZIP_H_ 1

#include <vector>

#include <espace/plugins.h>

struct ZIP : public ArchivePlugin
{
  uint32_t id();
  bool canHandle(File& archive);
  void scan(File& archive, const String& archiveName,
            std::vector<FileEntry>& entries);
  void readEntry(File& archive, const FileEntry& entry, File& ret);

  int  open(const FileEntry& entry);
  void read(int handle, void* buffer, uint count);
  void seek(int handle, uint position);
  void close(int handle);
};

#endif // !PLUGINS_ZIP_H_

// vim: ts=2 sw=2 et
