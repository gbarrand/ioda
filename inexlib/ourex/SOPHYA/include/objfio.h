// This may look like C code, but it is really -*- C++ -*-
// Classe de gestion des I/O fichiers des objets 
//                          R.Ansari          04/99
// LAL (Orsay) / IN2P3-CNRS  DAPNIA/SPP (Saclay) / CEA

#ifndef OBJFILEIO_H_SEEN
#define OBJFILEIO_H_SEEN

#include "machdefs.h"
#include <stdio.h>
#include "anydataobj.h"
#include "ppersist.h"

/*!
   \class SOPHYA::ObjFileIO
   \ingroup BaseTools
   Template class to ease implementation of PPersist handlers for a 
   class T , inheriting from AnyDataObj. The methodes ReadSelf() and 
   WriteSelf() must be implemented.
   \sa PPersist
*/

namespace SOPHYA {


template <class T>
class ObjFileIO : public PPersist {

public :
	    ObjFileIO() { dobj=NULL; ownobj=true; }
	    ObjFileIO(T * obj) { dobj = obj; ownobj=false; }
  virtual   ~ObjFileIO() { if (ownobj && dobj) delete dobj; }

  virtual   AnyDataObj* DataObj() { return(dobj); }
  virtual   void        SetDataObj(AnyDataObj & o) 
  {  
    T * po = dynamic_cast< T * >(& o); 
    if (po == NULL)  {
      char buff[160];
      sprintf(buff,"ObjFileIO<T>::SetDataObj(%s) - Object type  error ! ",
	      typeid(o).name());
      throw TypeMismatchExc(PExcLongMessage(buff));    
    }
    if (ownobj && dobj) delete dobj;  dobj = po; ownobj = false; 
  }    
  
  inline operator T&() { return(*dobj); }

protected :
  virtual void       ReadSelf(PInPersist&);           
  virtual void       WriteSelf(POutPersist&) const;  

  T * dobj;
  bool ownobj;       // True si dobj obtenu par new
};

/*
template <class T>
inline POutPersist& operator << (POutPersist& os, T const & obj)
{ ObjFileIO<T> fio((const_cast)&obj);  fio.Write(os);  return(os); } 

template <class T>
inline PInPersist& operator >> (PInPersist& is, T & obj)
{ ObjFileIO<T> fio(&obj); is.SkipToNextObject(); fio.Read(is); return(is); }  
*/


} // Fin namespace

#endif
