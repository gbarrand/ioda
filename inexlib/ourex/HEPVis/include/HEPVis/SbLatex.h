#ifndef HEPVis_SbLatex_h
#define HEPVis_SbLatex_h

#include <Inventor/SbString.h>

class SbLatex {
public:
  SbBool fileToDvi (const SbString&,SbString&,const SbString& = "latex -interaction=batchmode %M >");

  SbBool strToDvi (const SbString&,SbString&,const SbString& = "HEPVisSbLatexTempFile");
 private:
  SbBool changeToTmpDir(SbString&);
  SbBool changeToCurrentDir() const;
 private:
  SbString fCurrentDir;
};


#endif


