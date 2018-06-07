#ifndef HEPVis_SbConvert_h
#define HEPVis_SbConvert_h

// used in SbStyle.h

#include <Inventor/SbString.h>

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <clocale>

inline SbBool SbConvertToInt(const SbString& aString,int& aValue){
  const char* string = aString.getString();
  char* s;
  long value = ::strtol(string,&s,10);
  if(s==string) {
    aValue = 0L;
    return FALSE;
  }
  aValue = (int)value;
  return TRUE;
}

/*
inline SbBool SbConvertToUnsignedLong(const SbString& aString,unsigned long& aValue){
  aValue = 0L;
  if(::sscanf(aString.getString(),"%lx",&aValue)!=1) {
    if(::sscanf(aString.getString(),"%lu",&aValue)!=1) {
      aValue = 0L;
      return FALSE;
    }
  }
  return TRUE;
}
*/

inline SbBool SbConvertToFloat(const SbString& aString,float& aValue){
  SbString old(::setlocale(LC_NUMERIC,0));
  ::setlocale(LC_NUMERIC,"C");

  const char* string = aString.getString();
  char* s;
  double value = ::strtod(string,&s); //LC_NUMERIC C protected.

  ::setlocale(LC_NUMERIC,old.getString());

  if(s==string) {
    aValue = 0;  
    return FALSE;
  }
  aValue = (float)value;
  return TRUE;
}

inline SbBool SbConvertToBool(const SbString& aString,SbBool& aValue){
  if( (aString=="1") 
      || (aString=="true") || (aString=="TRUE") 
      || (aString=="yes") || (aString=="YES") ) {
    aValue = TRUE;
    return TRUE;
  } else if( (aString=="0") 
             || (aString=="false") || (aString=="FALSE")
             || (aString=="no") || (aString=="NO") ) {
    aValue = FALSE;
    return TRUE;
  } else {
    aValue = FALSE;
    return FALSE;
  }
}

#include <vector>

inline std::vector<SbString> SbGetWords(const SbString& aString,const char* aLimiter){
  std::vector<SbString> list;
  if(aString.getLength()<=0) return list;
  SbString string = aString;
  char* token = (char*)string.getString();
  while(1) { 
    char* pos = ::strstr(token,aLimiter);
    if(pos!=NULL) {
      *pos = '\0';
      if(*token!='\0') {
        list.push_back(SbString(token));
      }
      token = pos + ::strlen(aLimiter);          
    } else {
      /*last word*/
      if(*token!='\0') {
        list.push_back(SbString(token));
      }
      break;
    }
  }
  return list;
}

inline std::vector<SbString> SbGetLines(const SbString& aString){
  // Items separated by "\n" or "\\n"
  std::vector<SbString> list;
  int length = aString.getLength();
  if(length<=0) return list;
  char* string = new char[aString.getLength()+1];
  string = ::strcpy(string,aString.getString());
  int pos = 0;
  length++;
  int count;
  for(count=0;count<length;count++) {
    if( (string[count]=='\n') || 
        (string[count]=='\0') || 
        ( (string[count]=='\\') && (string[count+1]=='n') ) ) { 
      char shift_one = (string[count]=='\n' ? 1 : 0);
      string[count] = '\0';
      list.push_back(SbString(string+pos));
      if(shift_one==1) {
        pos = count+1;
      } else {
        pos = count+2;
        count++;
      }
    }
  }
  delete [] string;
  return list;
}

#endif
