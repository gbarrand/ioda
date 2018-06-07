#ifndef HEPVis_SbMutex_h
#define HEPVis_SbMutex_h

// pure abstract interface.

#include <Inventor/SbBasic.h>

namespace HEPVis {
class SbMutex {
public:
  virtual ~SbMutex(){}
public:
  virtual SbBool lock() const = 0;
  virtual SbBool unlock() const = 0;
};
}

#endif
