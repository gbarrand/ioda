#ifndef HEPVis_SbArguments_h
#define HEPVis_SbArguments_h

/*!
 * Author: Guy Barrand
 * Creation: September 6 2005
*/

#include <Inventor/SbString.h>

#include <vector>

class SbArguments {
public:
  SbArguments(int,char**);
  virtual ~SbArguments();
  SbBool isAnArgument(const SbString&) const;
  unsigned int number() const;
  SbBool find(const SbString&,SbString&) const;
  SbBool last(SbString& aName,SbString& aValue) const;
  SbBool file(SbString& aFile) const;
private:
  void initialize(const std::vector<SbString>&);
  std::vector< std::pair<SbString,SbString> > fArgs;
};

#endif



