
// this :
#include <HEPVis/SbArguments.h>

#include <Inventor/SbPList.h>

#include <HEPVis/SbString.h>

//////////////////////////////////////////////////////////////////////////////
SbArguments::SbArguments(
 int aArgc
,char** aArgv
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  std::vector<SbString> args;
  for(int index=0;index<aArgc;index++) args.push_back(SbString(aArgv[index]));
  initialize(args);
}
//////////////////////////////////////////////////////////////////////////////
SbArguments::~SbArguments(
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
}
//////////////////////////////////////////////////////////////////////////////
void SbArguments::initialize(
 const std::vector<SbString>& aArgs
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  unsigned int argc = aArgs.size();
  for(unsigned int index=0;index<argc;index++) {
    SbString s = aArgs[index];
    SbPList words = SbStringGetWords(s,"=");
    int wordn = words.getLength();
    if(wordn==1) {
      SbString& word0 = *((SbString*)words[0]);
      fArgs.push_back( std::pair<SbString,SbString>(word0,"")); 
    } else if(wordn>=2) {
      SbString& word0 = *((SbString*)words[0]);
      SbString& word1 = *((SbString*)words[1]);
      fArgs.push_back( std::pair<SbString,SbString>(word0,word1)); 
    }
    SbStringDelete(words);
  }
}
//////////////////////////////////////////////////////////////////////////////
unsigned int SbArguments::number(
) const
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  return fArgs.size();
}
//////////////////////////////////////////////////////////////////////////////
SbBool SbArguments::isAnArgument(
 const SbString& aString
) const
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  for(unsigned int index=0;index<fArgs.size();index++) {
    if(aString==fArgs[index].first) return TRUE;
  }
  return FALSE;
}
//////////////////////////////////////////////////////////////////////////////
SbBool SbArguments::find(
 const SbString& aString
,SbString& aValue
) const
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  aValue = "";
  for(unsigned int index=0;index<fArgs.size();index++) {
    if(aString==fArgs[index].first) {
      aValue = fArgs[index].second;
      return TRUE;
    }
  }
  return FALSE;
}
//////////////////////////////////////////////////////////////////////////////
SbBool SbArguments::last(
 SbString& aName
,SbString& aValue
) const
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  aName = "";
  aValue = "";
  if(!fArgs.size()) return FALSE;
  aName = fArgs[fArgs.size()-1].first;
  aValue = fArgs[fArgs.size()-1].second;
  return TRUE;
}
//////////////////////////////////////////////////////////////////////////////
SbBool SbArguments::file(
 SbString& aFile
) const
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  SbString slast;
  SbString s;
  if((number()!=1) 
     && last(slast,s) 
     && (slast.find("-")!=0) 
     && (s=="") ) {
    aFile = slast; //Last argument is not an option.
    return TRUE;
  } else {
    aFile = "";
    return FALSE;
  }
}
