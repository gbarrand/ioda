// This may look like C code, but it is really -*- C++ -*-
// Gestion de block de donnees swapable
//                         R. Ansari  Mars 2005
// LAL (Orsay) / IN2P3-CNRS  DAPNIA/SPP (Saclay) / CEA
#ifndef SWPPFSEGDB_H
#define SWPPFSEGDB_H


#include "machdefs.h"
#include "swsegdb.h"
#include "ppfswapper.h"

/*!
  \class SOPHYA::SwPPFSegDataBlock
  \ingroup BaseTools
  Specialization of SwSegDataBlock<T> using a PPFDataSwapper<T>
*/

namespace SOPHYA {

////////////////////////////////////////////////////////////////
//// ------------ Class SwPPFSegDataBlock<T> ---------------- //
////////////////////////////////////////////////////////////////

template <class T>
class SwPPFSegDataBlock : public SwSegDataBlock<T> {
public:
  //! Default constructor - SetSize(), SetSwapper() should be called after. SetSwapPosTagTable() might be called
  SwPPFSegDataBlock()
    : SwSegDataBlock<T>()
  {
  }
  //! Constructor - creation from swap position tags (values)
  SwPPFSegDataBlock(PPFDataSwapper<T> & dsw, vector<int_8> const & swpos, size_t segsz, size_t nitem=1)
    : SwSegDataBlock<T>(dsw, swpos, segsz, nitem)
  {
  }
  //! Constructor - optional specification of segment size and number of segments
  SwPPFSegDataBlock(PPFDataSwapper<T> & dsw, size_t segsz=32, size_t nbseg=0, size_t nitem=1)
    : SwSegDataBlock<T>(dsw, segsz, nbseg, nitem)
  {
  }
  //! copy constructor 
  SwPPFSegDataBlock(const SwPPFSegDataBlock<T>& a)
    : SwSegDataBlock<T>(a)
  {
  }
  //! Equal operator. Shares the data with \b a
  SwPPFSegDataBlock<T>&  operator = (const SwPPFSegDataBlock<T>& a)
  {
    SwSegDataBlock<T>::operator = (a);
    return *this;
  }
};

} // Fin du namespace

#endif
