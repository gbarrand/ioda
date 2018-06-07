// From root_v2/src/UNIX_System.cxx

// this :
#include <HEPVis/SbFile.h>

// Inventor :
#include <Inventor/SbPList.h>
#include <Inventor/SbString.h>

// HEPVis :
#include <HEPVis/SbBasic.h> // For HEPVis_UNIX,...
#include <HEPVis/SbString.h>

// Else :
#if defined(HEPVis_UNIX)

#if defined(__linux)
#ifndef _BSD_SOURCE
#define _BSD_SOURCE
#endif
// _BSD_SOURCE needed for S_IFMT and S_IFDIR
// but then there is a clash between math.h and the 
// #def of <math>f things in SbBasic.h with SGI-IV-2.1 ;
// we remove the clash by including first math.h
#include <math.h> 
#endif

#include <unistd.h>  //getcwd not found on HP-UX-aCC.

//extern "C" {
//char* getcwd(char*,size_t);
//}


#include <string.h>
#include <dirent.h>
#include <sys/stat.h>

#if defined(_POSIX_SOURCE)
#define REAL_DIR_ENTRY(dp) 1
#else
#define REAL_DIR_ENTRY(dp) (dp->d_ino != 0)
#endif

#elif defined(HEPVis_WINNT)
#include <string.h>
#include <direct.h>
#include <sys/stat.h>
#endif

#include <stdlib.h> //getenv
#include <stdio.h>  //remove

//////////////////////////////////////////////////////////////////////////////
const char* SbFile::getWorkingDirectory(
)
//////////////////////////////////////////////////////////////////////////////
// Return working directory.
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
#ifndef MAXPATHLEN
#define MAXPATHLEN 1024
#endif
#if defined(HEPVis_UNIX)
  static char cwd[MAXPATHLEN];
  if(getcwd(cwd,MAXPATHLEN)==NULL) return NULL;
  return cwd;
#elif defined(HEPVis_WINNT)
  static char cwd[MAXPATHLEN];
//  driveletter == 0 means return the working durectory for the default drive.
  char driveletter = '\0';
  char drive = driveletter ? toupper( driveletter ) - 'A' + 1 : 0;
  if(_getdcwd((int)drive,cwd,MAXPATHLEN)==NULL) return NULL;
  return cwd;
#else
  return NULL;
#endif
}
//////////////////////////////////////////////////////////////////////////////
void* SbFile::openDirectory(
 const char* aPath
)
//////////////////////////////////////////////////////////////////////////////
// Open a directory.
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  if (aPath==NULL) return NULL;
#if defined(HEPVis_UNIX)
  struct stat finfo;
  if (stat(aPath,&finfo) < 0) return NULL;
  if (!S_ISDIR(finfo.st_mode)) return NULL;
  return (void*)opendir(aPath);
#elif defined(HEPVis_WINNT)
  struct stat finfo;
  if (stat(aPath,&finfo) < 0) return NULL;
  if (!(finfo.st_mode & S_IFDIR)) return NULL;
  int length = strlen(aPath);
  char *entry = new char[strlen(aPath)+3];
  strcpy(entry,aPath);
  if (!(entry[length] == '/' || entry[length] == '\\' ))
    strcat(entry,"\\");
  strcat(entry,"*");
  HANDLE searchFile = FindFirstFile(entry,&fFindFileData);
  delete [] entry;
  if(searchFile == INVALID_HANDLE_VALUE) return NULL;
  return searchFile;
#else
  return NULL;
#endif
}
//////////////////////////////////////////////////////////////////////////////
const char* SbFile::getDirectoryEntry(
 void* aDirectory
)
//////////////////////////////////////////////////////////////////////////////
// Returns the next directory entry.
// Returns NULL if no more entries.
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  if (aDirectory==NULL) return NULL;
#if defined(HEPVis_UNIX)
  struct dirent* dp;
  //struct direct* dp;
  for (;;) {
    dp = readdir((DIR*)aDirectory);
    if (dp==NULL) 
      return NULL;
    if (REAL_DIR_ENTRY(dp))
      return dp->d_name;
  }
#elif defined(HEPVis_WINNT)
  HANDLE searchFile = (HANDLE)aDirectory;
  if(FindNextFile(searchFile,&fFindFileData)==FALSE) return NULL;
  return (const char *)(fFindFileData.cFileName);
#else
  return NULL;
#endif
}
//////////////////////////////////////////////////////////////////////////////
void SbFile::closeDirectory(
 void* aDirectory
)
//////////////////////////////////////////////////////////////////////////////
// Close a Unix file system directory.
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  if (aDirectory==NULL) return;
#if defined(HEPVis_UNIX)
  closedir((DIR*)aDirectory);
#elif defined(HEPVis_WINNT)
  ::FindClose(aDirectory);
#endif
}
//////////////////////////////////////////////////////////////////////////////
SbBool SbFile::isDirectory(
 const char* aPath
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  if(aPath==NULL) return FALSE;
#if defined(HEPVis_UNIX) || defined(HEPVis_WINNT)
  struct stat finfo;
  if (stat(aPath,&finfo) < 0) return FALSE;
  return ( ((finfo.st_mode & S_IFMT) == S_IFDIR) ? TRUE : FALSE);
#else
  return FALSE;
#endif
}
//////////////////////////////////////////////////////////////////////////////
SbStringList* SbFile::getDirectoryFiles(
 const char* aPath
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  if (aPath==NULL) return NULL;
  SbFile sbFile;
  void* directory = sbFile.openDirectory(aPath);
  if(directory==NULL) return NULL;
  SbStringList* list = new SbStringList;
  if(list==NULL) return NULL;
  const char* fileName;
  while ((fileName = sbFile.getDirectoryEntry(directory))) {
    if ( (strcmp(fileName,".")==0) || (strcmp(fileName,"..")==0) ) continue;
    if (isDirectory(fileName)==FALSE) {
      list->append(new SbString(fileName));
    }
  }
  sbFile.closeDirectory(directory);
  return list;
}
//////////////////////////////////////////////////////////////////////////////
SbStringList* SbFile::getDirectoryDirectories(
 const char* aPath
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  if (aPath==NULL) return NULL;
  SbFile sbFile;
  void* directory = sbFile.openDirectory(aPath);
  if(directory==NULL) return NULL;
  SbStringList* list = new SbStringList;
  if(list==NULL) return NULL;
  const char* fileName;
  while ((fileName = sbFile.getDirectoryEntry(directory))) {
    if ( (strcmp(fileName,".")==0) || (strcmp(fileName,"..")==0) ) continue;
    if (isDirectory(fileName)==TRUE) {
      list->append(new SbString(fileName));
    }
  }
  sbFile.closeDirectory(directory);
  return list;
}
//////////////////////////////////////////////////////////////////////////////
void SbFile::dumpDirectory(
 const char* aPath
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  if (aPath==NULL) return;
  SbFile sbFile;
  void* directory = sbFile.openDirectory(aPath);
  if(directory==NULL) return;
  printf("%s\n",aPath);
  const char* fileName;
  while ((fileName = sbFile.getDirectoryEntry(directory))) {
    if ( (strcmp(fileName,".")==0) || (strcmp(fileName,"..")==0) ) continue;
    if (isDirectory(fileName)==TRUE) {
      printf("d : %s\n",fileName);
    } else {
      printf("- : %s\n",fileName);
    }
  }
  sbFile.closeDirectory(directory);
}

#ifdef _WIN32
#include <process.h>
#else
#include <unistd.h>
#endif

//////////////////////////////////////////////////////////////////////////////
SbBool SbFile::tmpname(
 SbString& aName
,const SbString& aDir
,const SbString& aPrefix
,const SbString& aSuffix
)
//////////////////////////////////////////////////////////////////////////////
//  For UNIX aDir must be a string ended with '/'. Exa : /tmp/ .
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  SbString head = "";
  if(aDir==".") {
  } else if(aDir!="") {
    head += aDir;
  } else {
#ifdef _WIN32
    SbString env = ::getenv("HOMEDRIVE");
    SbString drive = (env!="" ? env : SbString("C:"));
    head += drive;
    head += "\\";
#else
    head += "/tmp/";
#endif
  }
  head += aPrefix;
  char spid[128];
  ::sprintf(spid,"%d",::getpid());
  head += SbString(spid);
  head += "_";

  for(int count=0;count<1000;count++) {
    SbString tmpnm = head;
    tmpnm += SbStringPrintF(3,"%03d",count);
    tmpnm += aSuffix;
    struct stat finfo;
    if(::stat(tmpnm.getString(),&finfo) < 0) {
      // No directory or file found :
      aName = tmpnm; // Good file name.
      return TRUE; 
    }
  }
  // No good name found :
  aName = "";
  return FALSE;
}
//////////////////////////////////////////////////////////////////////////////
SbString SbFile::getFullName(
 const SbString& aString
)
//////////////////////////////////////////////////////////////////////////////
// Lib::fmanip::name.
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  SbString fname = aString;
  SbStringGetenv(fname); // Check for env variable in name :
#ifdef _WIN32
  SbStringReplace(fname,"/","\\");
#endif
  return fname;
}
//////////////////////////////////////////////////////////////////////////////
SbBool SbFile::exists(
 const SbString& aString
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  FILE* file = ::fopen(aString.getString(),"rb");
  if(!file) return FALSE;
  ::fclose(file);
  return TRUE;
}
//////////////////////////////////////////////////////////////////////////////
SbBool SbFile::remove(
 const SbString& aName
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  return (::remove(aName.getString()) ==0 ? TRUE : FALSE);
}
