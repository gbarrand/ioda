// G. Le Meur 04/2000

#include "sopnamsp.h"
#include "fiospherethetaphi.h"
#include "pexceptions.h"
#include "fiondblock.h"
#include "datatype.h"
#include <typeinfo>

///////////////////////////////////////////////////////////
// --------------------------------------------------------
//   Les objets delegues pour la gestion de persistance 
// --------------------------------------------------------
//////////////////////////////////////////////////////////

template <class T>
FIO_SphereThetaPhi<T>::FIO_SphereThetaPhi()
{
  dobj= new SphereThetaPhi<T>;
  ownobj= true;
}

template <class T>
FIO_SphereThetaPhi<T>::FIO_SphereThetaPhi(string const& filename)
{
  dobj= new SphereThetaPhi<T>;
  ownobj= true;
  Read(filename);
}

template <class T>
FIO_SphereThetaPhi<T>::FIO_SphereThetaPhi(const SphereThetaPhi<T>& obj)
{
  dobj= new SphereThetaPhi<T>(obj, true);
  ownobj= true;
}

template <class T>
FIO_SphereThetaPhi<T>::FIO_SphereThetaPhi(SphereThetaPhi<T>* obj)
{
  dobj= obj;
  ownobj= false;
}

template <class T>
FIO_SphereThetaPhi<T>::~FIO_SphereThetaPhi()
{
  if (ownobj && dobj) delete dobj;
}

template <class T>
AnyDataObj* FIO_SphereThetaPhi<T>::DataObj()
{
  return(dobj);
}


template <class T>
void FIO_SphereThetaPhi<T>::SetDataObj(AnyDataObj & o)
{
  SphereThetaPhi<T> * po = dynamic_cast< SphereThetaPhi<T> * >(&o);
  if (po == NULL) {
    char buff[160];
    sprintf(buff,"FIO_SphereThetaPhi<%s>::SetDataObj(%s) - Object type  error ! ",
	    DataTypeInfo<T>::getTypeName().c_str(), typeid(o).name());
    throw TypeMismatchExc(PExcLongMessage(buff));    
  }

  if (ownobj && dobj) delete dobj;
  dobj = po; ownobj = false;
} 

template <class T>
void FIO_SphereThetaPhi<T>::ReadSelf(PInPersist& is)
{

  if(dobj == NULL) 
    {
      dobj= new SphereThetaPhi<T>;
      ownobj= true;      
    }

  // On lit les 3 premiers uint_8
  uint_8 itab[3];
  is.Get(itab, 3);
// Let's Read the SphereCoordSys object  -- ATTENTIOn - $CHECK$
  FIO_SphereCoordSys fio_scs( dobj->GetCoordSys());
  fio_scs.Read(is);

  // Pour savoir s'il y avait un DVList Info associe
  char strg[256];
  is.GetLine(strg, 255);
  bool hadinfo= false;
  if(strncmp(strg+strlen(strg)-7, "HasInfo", 7) == 0)  hadinfo= true;
  if(hadinfo) 
    {    // Lecture eventuelle du DVList Info
      is >> dobj->Info(); 
    }

  int_4 mNTheta;
  is.GetI4(mNTheta);  
  int_4 mNPix;
  is.GetI4(mNPix);
  double mOmeg;
  is.GetR8(mOmeg);
  dobj->setParameters( mNTheta, mNPix, mOmeg); 
  
  //  FIO_NDataBlock<int_4> fio_nd_nphi( &dobj->NPhi_);	
  //  fio_nd_nphi.Read(is);
  //  FIO_NDataBlock<int_4> fio_nd_Tnphi(&dobj->TNphi_);
  //  fio_nd_Tnphi.Read(is);
  //  FIO_NDataBlock<r_8> fio_nd_Theta( &dobj->Theta_);
  //  fio_nd_Theta.Read(is);
  //  FIO_NDataBlock<T> fio_nd(&dobj->pixels_);
  //  fio_nd.Read(is);

  //  Reza 30/8/2000 , - Methode de lecture plus facile  

  is >> dobj->NPhi_ ;    
  is >> dobj->TNphi_ ;   
  is >> dobj->Theta_;
  is >> dobj->pixels_;
}

template <class T>
void FIO_SphereThetaPhi<T>::WriteSelf(POutPersist& os) const
{

  if(dobj == NULL) 
    {
      cout << " WriteSelf:: dobj= null " << endl;
      return;
    }

//  On ecrit 3 uint_8 
//  0 : Numero de version,  1 : Size index,  2  reserve a l
uint_8 itab[3];
itab[0] = 1;
itab[1] = dobj->SizeIndex();
itab[2] = 0;
os.Put(itab, 3);
// Let's write the SphereCoordSys object
  FIO_SphereCoordSys fio_scs( dobj->GetCoordSys());
  fio_scs.Write(os);
  
  char strg[256];
  int_4 mNTheta= dobj->SizeIndex();
  int_4 mNPix  = dobj->NbPixels();
 
  if(dobj->ptrInfo()) 
    {
      sprintf(strg,"SphereThetaPhi: NSlices=%6d  NPix=%9d HasInfo",mNTheta,mNPix);
      os.PutLine(strg);
      os << dobj->Info();
    }
  else 
    { 
      sprintf(strg,"SphereThetaPhi: NSlices=%6d  NPix=%9d ",mNTheta,mNPix);
      os.PutLine(strg);  
    }

  os.PutI4(mNTheta);  
  os.PutI4(mNPix);
  os.PutR8(dobj->PixSolAngle());
  
  //  FIO_NDataBlock<int_4> fio_nd_nphi(&dobj->NPhi_);	
  //  fio_nd_nphi.Write(os);
  //  FIO_NDataBlock<int_4> fio_nd_Tnphi(&dobj->TNphi_);
  //  fio_nd_Tnphi.Write(os);
  //  FIO_NDataBlock<r_8> fio_nd_Theta(&dobj->Theta_);
  //  fio_nd_Theta.Write(os);
  //  FIO_NDataBlock<T> fio_nd(&dobj->pixels_);
  //  fio_nd.Write(os);

  //  Reza 30/8/2000 , - Methode d'ecriture plus facile  

  os << dobj->NPhi_;
  os << dobj->TNphi_;
  os << dobj->Theta_;
  os << dobj->pixels_;
}

#ifdef __CXX_PRAGMA_TEMPLATES__
#pragma define_template FIO_SphereThetaPhi<int_4>
#pragma define_template FIO_SphereThetaPhi<r_8>
#pragma define_template FIO_SphereThetaPhi<r_4>
#pragma define_template FIO_SphereThetaPhi< complex<r_4> >
#pragma define_template FIO_SphereThetaPhi< complex<r_8> >
#endif
#if defined(ANSI_TEMPLATES) || defined(GNU_TEMPLATES)
namespace SOPHYA {
template class FIO_SphereThetaPhi<int_4>;
template class FIO_SphereThetaPhi<r_8>;
template class FIO_SphereThetaPhi<r_4>;
template class FIO_SphereThetaPhi< complex<r_4> >;
template class FIO_SphereThetaPhi< complex<r_8> >;
}
#endif
