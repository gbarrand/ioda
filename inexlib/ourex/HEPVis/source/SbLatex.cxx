/* 
 *  SbTexToBitmap.cxx
 *  
 *
 *  Created by Laurent Garnier on Wen May 18 2005.
 *
 */ 

// this :
#include <HEPVis/SbLatex.h>

#include <HEPVis/SbFile.h>
#include <HEPVis/SbString.h>

#include <Inventor/errors/SoDebugError.h>

#if defined(__APPLE__)
#include <TargetConditionals.h>
#endif

#include <stdio.h>
#include <stdlib.h>

SbBool SbLatex::fileToDvi (const SbString& aFile,SbString& aDviFile,const SbString& aDviGenCommand)
{
#if TARGET_OS_IPHONE
  SoDebugError::postInfo("HEPVis_SbLatex::fileToDvi","::system() not available on iOS.");
  return FALSE;
#else
  // remove extention if exist
  SbString filename = aFile;
  int pos = SbStringReverseFind(filename,".tex");
  if (pos != SbStringNotFound) filename.deleteSubString(pos);

  aDviFile = "";
  SbString tmpDir;
  if (!changeToTmpDir(tmpDir)) // "cd" command has no effect on latex command
                               // but this is use to get the tmp directory
    return FALSE;

  SbString shortName = tmpDir;
#ifdef _WIN32
  pos = SbStringReverseFind(filename,"\\");
  if (pos != SbStringNotFound) {
    shortName += filename.getSubString(pos+1);
  } else {
    shortName = filename;
  }
#else
  pos = SbStringReverseFind(filename,"/");
  if (pos != SbStringNotFound) {
    shortName += filename.getSubString(pos+1);
  } else {
    shortName = filename;
  }
#endif
  SbString auxName = shortName;
  auxName += ".aux";
  SbString logName = shortName;
  logName += ".log";
  SbString nullName = shortName;
  nullName += "Null";

  SbString dviCommand = "cd ";
  dviCommand +=tmpDir;
  dviCommand +="; ";
  pos = SbStringFind(aDviGenCommand,"%M");
  if (pos!=-1) {
    dviCommand += aDviGenCommand.getSubString(0,pos-1);
    if (SbStringFind(aFile,"..") == 0) {
      dviCommand +=fCurrentDir;
#ifdef _WIN32
    } else if (SbStringFind(aFile,":\\") != 1) {
      dviCommand +=fCurrentDir;
#else
    } else if (SbStringFind(aFile,"/") != 0) {
      dviCommand +=fCurrentDir;
#endif
    }
    dviCommand +=aFile;
    pos = SbStringFind(aDviGenCommand,"%M");
    dviCommand +=aDviGenCommand.getSubString(pos+2);
    dviCommand +=nullName;
  } else {
    return FALSE;
  }
  // execute latex command
  if(::system(dviCommand.getString())){}
  ::remove(auxName.getString());
  ::remove(logName.getString());
  ::remove(nullName.getString());

  if (!changeToCurrentDir())
    return FALSE;
  aDviFile =shortName;
  aDviFile += ".dvi";
  return TRUE;
#endif //TARGET_OS_IPHONE
}
  

SbBool SbLatex::strToDvi (const SbString& aTexStr,SbString& aDviFile,const SbString& aTmpFile)
{
  SbString tmpDir;
  if (!changeToTmpDir(tmpDir))
    return FALSE;
  SbString texFile = tmpDir;
  texFile += aTmpFile;
  FILE *fp = fopen (texFile.getString(), "wb");
  fwrite(aTexStr.getString(), 1, aTexStr.getLength(), fp );
  fclose(fp);
  if (!changeToCurrentDir())
    return FALSE;
  if (!fileToDvi(texFile,aDviFile))
    return FALSE;
  if (!changeToTmpDir(tmpDir))
    return FALSE;
  remove(texFile.getString());
  if (!changeToCurrentDir())
    return FALSE;

  return TRUE;
}

SbBool SbLatex::changeToTmpDir(SbString& aTmpDir)
{
#if TARGET_OS_IPHONE
  SoDebugError::postInfo("HEPVis_SbLatex::changeToTmpDir","::system() not available on iOS.");
  return FALSE;
#else
  aTmpDir ="";
  const char* cDir = SbFile::getWorkingDirectory();
  fCurrentDir = SbString(cDir);
  if (cDir == NULL)
    return FALSE;
  if(!SbFile::tmpname(aTmpDir,"","",""))
    return FALSE;
#ifdef _WIN32
  aTmpDir = aTmpDir.getSubString(0,SbStringReverseFind(aTmpDir,"\\"));
  if (SbStringReverseFind(fCurrentDir,"\\") != (fCurrentDir.getLength()-1)) {
    fCurrentDir += "\\";
  }
#else
  aTmpDir = aTmpDir.getSubString(0,SbStringReverseFind(aTmpDir,"/"));
  if (SbStringReverseFind(fCurrentDir,"/") != (fCurrentDir.getLength()-1)) {
    fCurrentDir += "/";
  }
#endif
  SbFile sbFile;
  if (sbFile.openDirectory(aTmpDir.getString()) == NULL) {
    return FALSE;
  }
  if (aTmpDir == "")
    return FALSE;
  SbString cdCommand = "cd ";
  cdCommand += aTmpDir;
  if (system(cdCommand.getString()) == -1)
    return FALSE;
  return TRUE;
#endif //TARGET_OS_IPHONE  
}
SbBool SbLatex::changeToCurrentDir() const
{
#if TARGET_OS_IPHONE
  SoDebugError::postInfo("HEPVis_SbLatex::changeToCurrentDir","::system() not available on iOS.");
  return FALSE;
#else
  SbFile sbFile;
  if (sbFile.openDirectory(fCurrentDir.getString()) == NULL) {
    return FALSE;
  }
  SbString cdCommand = "cd ";
  cdCommand += fCurrentDir;
  if (system(cdCommand.getString()) == -1)
    return FALSE;
  return TRUE;
#endif //TARGET_OS_IPHONE
}
