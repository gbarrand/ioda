// G. Le Meur 04/2000

#include "sopnamsp.h"
#include "fiospherehealpix.h"
#include "pexceptions.h"
#include "fiondblock.h"
#include "datatype.h"
#include <typeinfo>

//*******************************************************************
// Class FIO_SphereHEALPix<T>
//  Les objets delegues pour la gestion de persistance 
//*******************************************************************

template <class T>
FIO_SphereHEALPix<T>::FIO_SphereHEALPix()
{
  dobj_= new SphereHEALPix<T>;
  ownobj= true;
}

template <class T>
FIO_SphereHEALPix<T>::FIO_SphereHEALPix(string const& filename)
{
  dobj_= new SphereHEALPix<T>;
  ownobj= true;
  Read(filename);
}

template <class T>
FIO_SphereHEALPix<T>::FIO_SphereHEALPix(const SphereHEALPix<T>& obj)
{
  dobj_= new SphereHEALPix<T>(obj, true);
  ownobj= true;
}

template <class T>
FIO_SphereHEALPix<T>::FIO_SphereHEALPix(SphereHEALPix<T>* obj)
{
  dobj_= obj;
  ownobj= false;
}

template <class T>
FIO_SphereHEALPix<T>::~FIO_SphereHEALPix()
{
  if (ownobj && dobj_) delete dobj_;
}

template <class T>
AnyDataObj* FIO_SphereHEALPix<T>::DataObj()
{
  return(dobj_);
}

template <class T>
void FIO_SphereHEALPix<T>::SetDataObj(AnyDataObj & o)
{
  SphereHEALPix<T> * po = dynamic_cast< SphereHEALPix<T> * >(&o);
  if (po == NULL) { 
    char buff[160];
    sprintf(buff,"FIO_SphereHEALPix<%s>::SetDataObj(%s) - Object type error ! ",
	    DataTypeInfo<T>::getTypeName().c_str(), typeid(o).name());
    throw TypeMismatchExc(PExcLongMessage(buff));
  }
  if (ownobj && dobj_) delete dobj_;
  dobj_ = po; ownobj = false;
} 

template <class T>
void FIO_SphereHEALPix<T>::ReadSelf(PInPersist& is)
{

  if(dobj_ == NULL) 
    {
      dobj_= new SphereHEALPix<T>;
      ownobj= true;      
    }

// On lit les 3 premiers uint_8
uint_8 itab[3];
is.Get(itab, 3);
 
//  Juin 2006 : NumVer 1->2 avec mise en place de HEALPix NESTED
//  Si itab[2]==1 --> NESTED 
bool fgring = true;
if (itab[2] == 1) fgring = false;

// Let's Read the SphereCoordSys object  -- ATTENTIOn - $CHECK$
  FIO_SphereCoordSys fio_scs( dobj_->GetCoordSys());
  fio_scs.Read(is);
  // Pour savoir s'il y avait un DVList Infodobj->Info();  associe
  char strg[256];
  is.GetLine(strg, 255);
  bool hadinfo= false;
  if(strncmp(strg+strlen(strg)-7, "HasInfo", 7) == 0)  hadinfo= true;
  if(hadinfo) 
    {    // Lecture eventuelle du DVList Info
      is >> dobj_->Info(); 
    }

  int_4 nSide;
  is.GetI4(nSide);
  int_4 nPix;
  is.GetI4(nPix);
  double Omega;
  is.GetR8(Omega);
  dobj_->setParameters(nSide,nPix, Omega, fgring);

// On lit les DataBlocks;
  FIO_NDataBlock<T> fio_nd(&dobj_->pixels_);
  fio_nd.Read(is);
  FIO_NDataBlock<int_4> fio_ndi1(&dobj_->sliceBeginIndex_);
  fio_ndi1.Read(is);
  FIO_NDataBlock<int_4> fio_ndi2(&dobj_->sliceLenght_);
  fio_ndi2.Read(is);


}
 
template <class T>
void FIO_SphereHEALPix<T>::WriteSelf(POutPersist& os) const
{
  if(dobj_ == NULL) 
    {
      cout << " WriteSelf:: dobj_= null " << endl;
      return;
    }

//  On ecrit 3 uint_8 
//  0 : Numero de version,  1 : Size index,  2  reserve a l
//  Juin 2006 : NumVer 1->2 avec mise en place de HEALPix NESTED
//  totalement back-compatible , on utilise itab[2] qu'on met a 1
//  pour NESTED
uint_8 itab[3];
itab[0] = 2;
itab[1] = dobj_->SizeIndex();
itab[2] = (dobj_->IfRING()) ? 0 : 1;
os.Put(itab, 3);
// Let's write the SphereCoordSys object
  FIO_SphereCoordSys fio_scs( dobj_->GetCoordSys());
  fio_scs.Write(os);

  char strg[256];
  int_4 nSide= dobj_->SizeIndex();
  int_4 nPix = dobj_->NbPixels();
 
  if(dobj_->ptrInfo()) 
    {
      sprintf(strg,"SphereHEALPix: NSide=%6d  NPix=%9d HasInfo",nSide,nPix);
      os.PutLine(strg);
      os << dobj_->Info();
    }
  else 
    { 
      sprintf(strg,"SphereHEALPix: NSide=%6d  NPix=%9d ",nSide,nPix);
      os.PutLine(strg);  
    }

  os.PutI4(nSide);
  os.PutI4(nPix);
  os.PutR8(dobj_->PixSolAngle());
// On ecrit les dataBlocks 
  FIO_NDataBlock<T> fio_nd(&dobj_->pixels_);
  fio_nd.Write(os);
  FIO_NDataBlock<int_4> fio_ndi1(&dobj_->sliceBeginIndex_);
  fio_ndi1.Write(os);
  FIO_NDataBlock<int_4> fio_ndi2(&dobj_->sliceLenght_);
  fio_ndi2.Write(os);
}

//*******************************************************************

#ifdef __CXX_PRAGMA_TEMPLATES__
#pragma define_template FIO_SphereHEALPix<uint_2>
#pragma define_template FIO_SphereHEALPix<int_4>
#pragma define_template FIO_SphereHEALPix<r_8>
#pragma define_template FIO_SphereHEALPix<r_4>
#pragma define_template FIO_SphereHEALPix< complex<r_4> >
#pragma define_template FIO_SphereHEALPix< complex<r_8> >
#endif
#if defined(ANSI_TEMPLATES) || defined(GNU_TEMPLATES)
namespace SOPHYA {
template class FIO_SphereHEALPix<uint_2>;
template class FIO_SphereHEALPix<int_4>;
template class FIO_SphereHEALPix<r_8>;
template class FIO_SphereHEALPix<r_4>;
template class FIO_SphereHEALPix< complex<r_4> >;
template class FIO_SphereHEALPix< complex<r_8> >;
}
#endif
