// G. Le Meur 04/2000



#ifndef FIOSPHERETHETAPHI_SEEN
#define FIOSPHERETHETAPHI_SEEN
#include "sphericalmap.h"
#include "tvector.h"
#include "ndatablock.h"
#include "spherethetaphi.h"

#include "anydataobj.h"
#include "ppersist.h"


//
// ------------- Classes pour la gestion de persistance --
//

namespace SOPHYA {

template <class T>
class FIO_SphereThetaPhi : public PPersist  
{
public:

FIO_SphereThetaPhi();
FIO_SphereThetaPhi(string const & filename); 
FIO_SphereThetaPhi(const SphereThetaPhi<T>& obj);
FIO_SphereThetaPhi(SphereThetaPhi<T>* obj);
virtual ~FIO_SphereThetaPhi();
virtual AnyDataObj* DataObj();
void SetDataObj(AnyDataObj & o);
inline operator SphereThetaPhi<T>() { return(*dobj); }

protected :

virtual void ReadSelf(PInPersist&);           
virtual void WriteSelf(POutPersist&) const;  
SphereThetaPhi<T>* dobj;
bool ownobj;
};

/*! \ingroup SkyMap \fn operator<<(POutPersist&,SphereThetaPhi<T>&)
  \brief Write SphereThetaPhi \b obj into POutPersist stream \b os */
template <class T>
inline POutPersist& operator << (POutPersist& os, SphereThetaPhi<T> & obj)
{ FIO_SphereThetaPhi<T> fio(&obj);  fio.Write(os);  return(os); }

/*! \ingroup SkyMap \fn operator>>(PInPersist&,SphereThetaPhi<T>&)
  \brief Read SphereThetaPhi \b obj from PInPersist stream \b os */
template <class T>
inline PInPersist& operator >> (PInPersist& is, SphereThetaPhi<T> & obj)
{ FIO_SphereThetaPhi<T> fio(&obj); is.SkipToNextObject(); fio.Read(is); return(is); }


} // Fin du namespace

#endif
