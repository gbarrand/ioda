// G. Le Meur 04/2000

#ifndef FIOSPHEREHEALPIX_SEEN
#define FIOSPHEREHEALPIX_SEEN
#include "sphericalmap.h"
#include "tvector.h"
#include "ndatablock.h"
#include "spherehealpix.h"

#include "anydataobj.h"
#include "ppersist.h"


//
// ------------- Classes pour la gestion de persistance --
//

namespace SOPHYA {


template <class T>
class FIO_SphereHEALPix : public PPersist  
{
public:

FIO_SphereHEALPix();
FIO_SphereHEALPix(string const & filename); 
FIO_SphereHEALPix(const SphereHEALPix<T>& obj);
FIO_SphereHEALPix(SphereHEALPix<T>* obj);
virtual ~FIO_SphereHEALPix();
virtual AnyDataObj* DataObj();
virtual void SetDataObj(AnyDataObj & o);
inline operator SphereHEALPix<T>() { return(*dobj_); }

protected :

virtual void ReadSelf(PInPersist&);           
virtual void WriteSelf(POutPersist&) const;  
SphereHEALPix<T>* dobj_;
bool ownobj;
};

/*! \ingroup SkyMap \fn operator<<(POutPersist&,SphereHEALPix<T>&)
  \brief Write SphereHEALPix \b obj into POutPersist stream \b os */
template <class T>
inline POutPersist& operator << (POutPersist& os, SphereHEALPix<T> & obj)
{ FIO_SphereHEALPix<T> fio(&obj);  fio.Write(os);  return(os); }

/*! \ingroup SkyMap \fn operator>>(PInPersist&,SphereHEALPix<T>&)
  \brief Read SphereHEALPix \b obj from PInPersist stream \b os */
template <class T>
inline PInPersist& operator >> (PInPersist& is, SphereHEALPix<T> & obj)
{ FIO_SphereHEALPix<T> fio(&obj); is.SkipToNextObject(); fio.Read(is); return(is); }

} // Fin du namespace

#endif
