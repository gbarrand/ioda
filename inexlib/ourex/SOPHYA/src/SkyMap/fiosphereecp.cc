// G. Le Meur 04/2000

#include "sopnamsp.h"
#include "fiosphereecp.h"
#include "fioarr.h"
#include "pexceptions.h"
#include "datatype.h"
#include <typeinfo>

///////////////////////////////////////////////////////////
// --------------------------------------------------------
//   Les classes delegues pour la gestion de persistance 
// --------------------------------------------------------
//////////////////////////////////////////////////////////
namespace SOPHYA {

template <class T>
FIO_SphereECP<T>::FIO_SphereECP()
{
  dobj= new SphereECP<T>;
  ownobj= true;
}

template <class T>
FIO_SphereECP<T>::FIO_SphereECP(string const& filename)
{
  dobj= new SphereECP<T>;
  ownobj= true;
  Read(filename);
}

template <class T>
FIO_SphereECP<T>::FIO_SphereECP(const SphereECP<T>& obj)
{
  dobj= new SphereECP<T>(obj, true);
  ownobj= true;
}

template <class T>
FIO_SphereECP<T>::FIO_SphereECP(SphereECP<T>* obj)
{
  dobj= obj;
  ownobj= false;
}

template <class T>
FIO_SphereECP<T>::~FIO_SphereECP()
{
  if (ownobj && dobj) delete dobj;
}

template <class T>
AnyDataObj* FIO_SphereECP<T>::DataObj()
{
  return(dobj);
}


template <class T>
void FIO_SphereECP<T>::SetDataObj(AnyDataObj & o)
{
  SphereECP<T> * po = dynamic_cast< SphereECP<T> * >(&o);
  if (po == NULL) {
    char buff[160];
    sprintf(buff,"FIO_SphereECP<%s>::SetDataObj(%s) - Object type  error ! ",
	    DataTypeInfo<T>::getTypeName().c_str(), typeid(o).name());
    throw TypeMismatchExc(PExcLongMessage(buff));    
  }

  if (ownobj && dobj) delete dobj;
  dobj = po; ownobj = false;
} 

template <class T>
void FIO_SphereECP<T>::ReadSelf(PInPersist& is)
{
  if(dobj == NULL)  {
    dobj= new SphereECP<T>;
    ownobj= true;      
  }
  
  // On lit les 3 premiers uint_8
  int_4 itab[4];
  is.Get(itab, 4);
  dobj->_partial = (itab[1] & 1) ? true : false;

  is >> dobj->_theta1 >> dobj->_theta2 ; 
  is >> dobj->_phi1 >> dobj->_phi2; 
  is >> dobj->_dtheta >> dobj->_dphi;
  is >> dobj->_outofmapidx >> dobj->_outofmappix >> dobj->_outofmapval;
  // Let's Read the SphereCoordSys object  -- ATTENTIOn - $CHECK$
  FIO_SphereCoordSys fio_scs( dobj->GetCoordSys());
  fio_scs.Read(is);
  // S'il y avait un objet Info() a lire 
  if (itab[1] & 2)  is >> dobj->Info();  
  // On lit le tableau des pixels
  is >> dobj->_pixels;
}

template <class T>
void FIO_SphereECP<T>::WriteSelf(POutPersist& os) const
{
  if(dobj == NULL) {
      cout << " FIO_SphereECP<T>::WriteSelf:: dobj= null " << endl;
      return;
    }
//  On ecrit 4 int_4 
//  itab[0] : Numero de version,  
//itab[1] = 0 full map, , =1 partial map  (bit 0) , bit 1 set -> has info
//  itab[2], itab[3]  reserve a un usage futur
  int_4 itab[4];
  itab[0] = 1;
  itab[1] = (dobj->IsPartial()) ? 1 : 0;
  if (dobj->ptrInfo() != NULL)  itab[1] |= 
  itab[2] = itab[3] = 0;
  os.Put(itab, 4);

  os << dobj->_theta1 << dobj->_theta2 ; 
  os << dobj->_phi1 << dobj->_phi2; 
  os << dobj->_dtheta << dobj->_dphi;
  os << dobj->_outofmapidx << dobj->_outofmappix << dobj->_outofmapval;

  // Let's write the SphereCoordSys object
  FIO_SphereCoordSys fio_scs( dobj->GetCoordSys());
  fio_scs.Write(os);
  // On ecrit l'objet info si necessaire
  if (dobj->ptrInfo() != NULL) os << dobj->Info();

  // On ecrit le tableau des pixels
  os << dobj->_pixels;
}

}// Fin du namespace

#ifdef __CXX_PRAGMA_TEMPLATES__
#pragma define_template FIO_SphereECP<int_4>
#pragma define_template FIO_SphereECP<r_8>
#pragma define_template FIO_SphereECP<r_4>
#pragma define_template FIO_SphereECP< complex<r_4> >
#pragma define_template FIO_SphereECP< complex<r_8> >
#endif
#if defined(ANSI_TEMPLATES) || defined(GNU_TEMPLATES)
namespace SOPHYA {
template class FIO_SphereECP<int_4>;
template class FIO_SphereECP<r_8>;
template class FIO_SphereECP<r_4>;
template class FIO_SphereECP< complex<r_4> >;
template class FIO_SphereECP< complex<r_8> >;
}
#endif

