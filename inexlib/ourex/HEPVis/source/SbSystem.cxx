// this :
#include <HEPVis/SbSystem.h>

#include <HEPVis/SbString.h>

//////////////////////////////////////////////////////////////////////////////
SbBool SbSystemIsMacApp(
 const SbString& aString
,SbString& aRoot
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  aRoot = "";
  SbBool status = FALSE;
  SbPList words = SbStringGetWords(aString,"/");
  int wordn = words.getLength();
  if(wordn>=3) {
    SbString& word_2 = *((SbString*)words[wordn-2]);
    SbString& word_3 = *((SbString*)words[wordn-3]);
    if((word_2=="MacOS")&&(word_3=="Contents")) {
      // It seems to be a MacOS binary from a .app bundle.
      aRoot = (aString[0]=='/'?"/":"");
      for(int index=0;index<(wordn-2);index++){
        SbString& word = *((SbString*)words[index]);
        if(index) aRoot += SbString("/");
        aRoot += word;
      }
      status = TRUE;
    }
  }
  SbStringDelete(words);
  return status;
}

#include <stdlib.h>
#ifdef __alpha
  extern "C" { extern int putenv(char *);}
#endif

#include <string.h> /* strcpy */
#include <stdlib.h> /* malloc,free */

//#define Lib_strdup(str)  ((str) != NULL ? (::strcpy((char*)::malloc((unsigned)::strlen(str) + 1), str)) : (char*)NULL)
//Linux : avoid a warning with putenv :
#define Lib_strdup(str)  (::strcpy((char*)::malloc((unsigned)::strlen(str) + 1), str))

//////////////////////////////////////////////////////////////////////////////
SbBool SbSystemPutenv(
 const SbString& aString
,const SbString& aValue
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  SbString value = aString;
  value += SbString("=");
  value += aValue;
  if(::putenv(Lib_strdup(value.getString()))) return FALSE;
  return TRUE;
}
//////////////////////////////////////////////////////////////////////////////
SbString SbSystemPathSeparator(
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
#ifdef _WIN32
  return ";";
#else
  return ":";
#endif
}
//////////////////////////////////////////////////////////////////////////////
SbString SbSystemFileSeparator(
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
#ifdef _WIN32
  return "\\";
#else
  return "/";
#endif
}
