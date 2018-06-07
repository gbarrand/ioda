#ifndef HEPVis_iv2pov_h
#define HEPVis_iv2pov_h

#include <Inventor/SbBasic.h>
#include <Inventor/SbString.h>

class SoGroup;

SbBool HEPVis_iv2pov(SoGroup*,
                     SbBool aHuntText,
                     SbBool aRmBack,
                     const SbString& aFile);

#endif
