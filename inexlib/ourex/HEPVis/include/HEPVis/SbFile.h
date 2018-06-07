#ifndef HEPVis_SbFile_h
#define HEPVis_SbFile_h

#include <Inventor/SbBasic.h>
#include <Inventor/SbString.h>

#ifdef _WIN32
#include <windows.h>
#endif

class SbStringList;

class SbFile {
public:
  void* openDirectory(const char*);
  const char* getDirectoryEntry(void*);
  void closeDirectory(void*);
public:
  static SbStringList* getDirectoryFiles(const char*);
  static SbStringList* getDirectoryDirectories(const char*);
  static void dumpDirectory(const char*);
  static SbBool isDirectory(const char*);
  static const char* getWorkingDirectory();
  static SbBool tmpname(SbString&,const SbString& = "",const SbString& = "tmp",const SbString& = "");
  static SbString getFullName(const SbString&);
  static SbBool exists(const SbString&);
  static SbBool remove(const SbString&);
private:
#ifdef _WIN32
  WIN32_FIND_DATA fFindFileData;
#endif
};


#endif

