// This may look like C code, but it is really -*- C++ -*-
// Classe pour la gestion de persistance pour NDataBlock<T>
//                         C.Magneville          04/99-03/2000
// LAL (Orsay) / IN2P3-CNRS  DAPNIA/SPP (Saclay) / CEA

#ifndef FIONDATABLOCK_H
#define FIONDATABLOCK_H

#include "machdefs.h"
#include "ppersist.h"
#include "ndatablock.h"
#include <iostream>

namespace SOPHYA {

template <class T>
class FIO_NDataBlock : public  PPersist  {
public:
  FIO_NDataBlock();
  FIO_NDataBlock(string const & filename); 
  FIO_NDataBlock(const NDataBlock<T> & obj);
  FIO_NDataBlock(NDataBlock<T> * obj);
  virtual ~FIO_NDataBlock();
  virtual AnyDataObj* DataObj();
  virtual void        SetDataObj(AnyDataObj & o);
  inline operator NDataBlock<T>() { return(*dobj); }

  virtual uint_8     getMemOId() const ;      
  virtual void       ShareDataReference(PPersist &);       
  virtual PPersist*  CloneSharedReference();

protected :
  virtual void       ReadSelf(PInPersist&);           
  virtual void       WriteSelf(POutPersist&) const;  
  NDataBlock<T> * dobj;
  bool ownobj;
};

template <class T>
inline POutPersist& operator << (POutPersist& os, NDataBlock<T> & obj)
{ FIO_NDataBlock<T> fio(&obj);  fio.Write(os);  return(os); }
template <class T>
inline PInPersist& operator >> (PInPersist& is, NDataBlock<T> & obj)
{ FIO_NDataBlock<T> fio(&obj); is.SkipToNextObject(); fio.Read(is); return(is); }

} // Fin du namespace

#endif
