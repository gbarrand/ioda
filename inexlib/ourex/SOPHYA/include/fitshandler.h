/*    
   --- SOPHYA software - FitsIOServer module ---
   R. Ansari, 2005 
   (C) UPS+LAL IN2P3/CNRS     (C) DAPNIA-SPP/CEA 
*/
#ifndef FITSHANDLER_H
#define FITSHANDLER_H

#include "machdefs.h"
#include <string>
#include <typeinfo>
#include "fitsinoutfile.h"

namespace SOPHYA {

/*! 
  \ingroup FitsIOServer
  \brief Interface definition for classes handling object storage retrieval in FITS Format 
*/
class FitsHandlerInterface {

public:

  virtual ~FitsHandlerInterface() {}
  //! Return the real data object
  virtual AnyDataObj* DataObj() = 0;       // Retourne l'objet reel 

  /*! 
    \brief Return a positive value if I/O for object \b o can be handled

    - Rc= 0 -> Can NOT handle fits I/O operations for the object \b o 
    - Rc= 1 -> Can handle fits I/O operations for the object \b o 
    - Rc= 2 -> This is a specific handler for object \b o
    - Rc > 2 -> Higher Rc values can be returned if needed 
      (for subclasses with specific handlers)

  */
  virtual int         CheckHandling(AnyDataObj & o) = 0;
  //! Read/write operation will use the object o
  virtual void        SetDataObj(AnyDataObj & o) = 0;

  //! 
  /*! 
    \brief Return a positive value if current HDU can be read by the handler

    - Rc= 0 -> Can NOT read the current HDU 
    - Rc= 1 -> Can read the current HDU (generic reader)
    - Rc= 2 -> Can read the current HDU, as a specific reader
    - Rc > 2 -> Higher Rc values can be returned if needed 
      (when multiple specific handlers are registered) 

  */
  virtual int         CheckReadability(FitsInOutFile& is) = 0;
  
  //! Clone (duplicate) the handler class
  virtual FitsHandlerInterface* Clone() = 0;

  //! Perform the actual write operation to the output fits file
  virtual void        Write(FitsInOutFile& os) = 0;
  //! Perform the actual read operation from input fits file
  virtual void        Read(FitsInOutFile& is) = 0;  
};

/*! 
  \ingroup FitsIOServer
  \brief Generic implementation of FitsHandlerInterface 
*/
template <class T>
class FitsHandler : public FitsHandlerInterface {

public :
	    FitsHandler() { dobj=NULL; ownobj=true; }
	    FitsHandler(T & obj) { dobj = &obj; ownobj=false; }
  virtual   ~FitsHandler() { if (ownobj && dobj) delete dobj; }

  virtual   AnyDataObj* DataObj() { return(dobj); }
  virtual   int         CheckHandling(AnyDataObj & o) 
  {  
    if (typeid(o) == typeid(T)) return 2;
    T * po = dynamic_cast< T * >(& o); 
    if (po == NULL) return 0;
    else return 1;
  }
  virtual   void        SetDataObj(AnyDataObj & o) 
  {  
    T * po = dynamic_cast< T * >(& o); 
    if (po == NULL)  {
      string msg = "FitsHandler<T>::SetDataObj() Wrong object type: " ;
      msg += typeid(o).name(); 
      throw TypeMismatchExc(msg);    
    }
    if (ownobj && dobj) delete dobj;  dobj = po; ownobj = false; 
  }

  virtual int         CheckReadability(FitsInOutFile& is);
    
  virtual FitsHandlerInterface* Clone() 
    { return new FitsHandler<T>() ; }

  inline operator T&() { return(*dobj); }

  virtual void        Read(FitsInOutFile& is);
  virtual void        Write(FitsInOutFile& os);

protected :
  T * dobj;
  bool ownobj;       // True si dobj obtenu par new
};



} // Fin du namespace

#endif

