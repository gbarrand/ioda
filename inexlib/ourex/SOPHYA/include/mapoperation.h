// This may look like C code, but it is really -*- C++ -*-
//--------------------------------------------------------------------------
// File and Version Information:
//        Simple Operations on Maps 
// Description:
//
// History (add to end):
//      Sophie   April, 2000  - creation
//        code from Reza
//------------------------------------------------------------------------

#ifndef MapOperation_SEEN
#define MapOperation_SEEN

#include "machdefs.h"
#include <math.h>
#include "ndatablock.h"
#include "pixelmap.h"

namespace SOPHYA {
  
  //! Class for simple mathematical operation on maps
  
template <class T>
  class MapOperations {
  public:
  void meanSig(NDataBlock<T> const & dbl, double& gmoy, double& gsig);
  void sphTosph(PixelMap<T>& in, PixelMap<T>& out);
  
  void Sph2Sph(PixelMap<T>& in, PixelMap<T>& out);
  void MeanSig(NDataBlock<T> const & dbl, double& gmoy, double& gsig);
};
 
template <class T>
  inline void Sph2Sph(PixelMap<T>& in, PixelMap<T>& out)
  { MapOperations<T> a; a.sphTosph(in,out); }
 
template <class T>
  inline void MeanSig(NDataBlock<T> const & dbl, double& gmoy, double& gsig)
  { MapOperations<T> a; a.meanSig(dbl,gmoy,gsig); } 

}  // namespace SOPHYA

#endif
