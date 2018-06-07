// G. Le Meur 04/2000

#ifndef FIOLOCALMAP_SEEN
#define FIOLOCALMAP_SEEN
#include "sphericalmap.h"
#include "localmap.h"
#include "tvector.h"
#include "ndatablock.h"

#include "anydataobj.h"
#include "ppersist.h"


//
// ------------- Classes pour la gestion de persistance --
//

namespace SOPHYA {

//!    Delegated objects for persitance management
template <class T>
class FIO_LocalMap : public PPersist  
{

public:

FIO_LocalMap();
FIO_LocalMap(string const & filename); 
FIO_LocalMap(const LocalMap<T>& obj);
FIO_LocalMap(LocalMap<T>* obj);
virtual ~FIO_LocalMap();
virtual AnyDataObj* DataObj();
virtual void        SetDataObj(AnyDataObj & o);
inline operator LocalMap<T>() { return(*dobj); }

protected :

virtual void ReadSelf(PInPersist&);           
virtual void WriteSelf(POutPersist&) const;  
LocalMap<T>* dobj;
bool ownobj;
};

/*! \ingroup SkyMap \fn operator<<(POutPersist&,LocalMap<T>&)
  \brief Write LocalMap \b obj into POutPersist stream \b os */
template <class T>
inline POutPersist& operator << (POutPersist& os, LocalMap<T> & obj)
{ FIO_LocalMap<T> fio(&obj);  fio.Write(os);  return(os); }

/*! \ingroup SkyMap \fn operator>>(PInPersist&,LocalMap<T>&)
  \brief Read LocalMap \b obj from PInPersist stream \b os */
template <class T>
inline PInPersist& operator >> (PInPersist& is, LocalMap<T> & obj)
{ FIO_LocalMap<T> fio(&obj); is.SkipToNextObject(); fio.Read(is); return(is); }


} // Fin du namespace

#endif
