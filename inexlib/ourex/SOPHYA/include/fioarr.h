// This may look like C code, but it is really -*- C++ -*-
//      template array class for numerical types
//                     R. Ansari, C.Magneville   03/2000

#ifndef FIOTArray_SEEN
#define FIOTArray_SEEN

#include "machdefs.h"
#include <iostream>
#include "tarray.h"
#include "tmatrix.h"
#include "tvector.h"
#include "ppersist.h"

namespace SOPHYA {
///////////////////////////////////////////////////////////////////////// 
// Gestionnaire PPF de TArray<T>, ainsi que TMatrix<T>/TVector<T>
///////////////////////////////////////////////////////////////////////// 
template <class T>
class FIO_TArray : public  PPersist  {
public:
  FIO_TArray();
  FIO_TArray(string const & filename); 
  FIO_TArray(const TArray<T> & obj);
  FIO_TArray(TArray<T> * obj);
  virtual ~FIO_TArray();
  virtual AnyDataObj* DataObj();
  virtual void        SetDataObj(AnyDataObj & o);
  inline operator TArray<T>() { return(*dobj); }
protected :
  virtual void ReadSelf(PInPersist&);           
  virtual void WriteSelf(POutPersist&) const;  
  TArray<T> * dobj; //!< Pointer to the connected TArray
  bool ownobj; //!< true if the connected object belong to the class
};

/*! \ingroup TArray \fn operator<<(POutPersist&,TArray<T>&)
  \brief Write TArray \b obj into POutPersist stream \b os */
template <class T>
inline POutPersist& operator << (POutPersist& os, TArray<T> & obj)
{ FIO_TArray<T> fio(&obj);  fio.Write(os);  return(os); }

/*! \ingroup TArray \fn operator>>(PInPersist&,TArray<T>&)
  \brief Read TArray \b obj from PInPersist stream \b os */
template <class T>
inline PInPersist& operator >> (PInPersist& is, TArray<T> & obj)
{ FIO_TArray<T> fio(&obj); is.SkipToNextObject(); fio.Read(is); return(is); }


} // Fin du namespace

#endif
