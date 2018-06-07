//     Persistence manager for template numerical arrays
//               R. Ansari, C.Magneville   03/2000

#include "sopnamsp.h"
#include "pexceptions.h"
#include "fiondblock.h"
#include "fioarr.h"
#include "tmatrix.h"
#include "tvector.h"
#include "datatype.h"
#include <typeinfo>

// --------------------------------------------------------
//   Les objets delegues pour la gestion de persistance 
// --------------------------------------------------------
namespace SOPHYA {

/*!
  \class FIO_TArray
  \ingroup TArray

  \brief Manager class the PPF I/O (SOPHYA persistence) of TArray<T> objects.
 
  This class manages SOPHYA persistence (Read/Write in PPF format) for objects
  of class TArray<T>, TMatrix<T> and TVector<T>

 */
///////////////////////////////////////////////////////////

//! Default constructor
template <class T>
FIO_TArray<T>::FIO_TArray()
{
  dobj=NULL;
  ownobj=false;
}


//! Constructor from the file \b filename - Reads the TArray object from the specified file.
template <class T>
FIO_TArray<T>::FIO_TArray(string const & filename) 
{
  dobj=NULL;
  ownobj=false;
  Read(filename);
}

//! Constructor from the TArray \b obj
template <class T>
FIO_TArray<T>::FIO_TArray(const TArray<T> & obj) 
{ 
  const TVector<T> * tv = dynamic_cast<const TVector<T> *>(&obj);
  if (tv != NULL)  dobj = new TVector<T>(*tv, true);
  else {
  const TMatrix<T> * tm = dynamic_cast<const TMatrix<T> *>(&obj);
  if (tm != NULL)  dobj = new TMatrix<T>(*tm, true);
  else dobj = new TArray<T>(obj, true);
  }
  ownobj=true; 
}

//! Connect with a TArray \b obj
template <class T>
FIO_TArray<T>::FIO_TArray(TArray<T> * obj) 
{ 
  dobj = obj;
  ownobj=false; 
}

//! destructor
template <class T>
FIO_TArray<T>::~FIO_TArray()
{
  if (ownobj && dobj) delete dobj;
}

//! Return the pointer to the connected TArray<T> object
template <class T>
AnyDataObj* FIO_TArray<T>::DataObj()
{
  return(dobj);
}

//! Connect TArray \b o
template <class T>
void FIO_TArray<T>::SetDataObj(AnyDataObj & o)
{
  TArray<T> * po = dynamic_cast< TArray<T> * >(&o);
  if (po == NULL) {
    char buff[160];
    sprintf(buff,"FIO_TArray<%s>::SetDataObj(%s) - Object type  error ! ",
	    DataTypeInfo<T>::getTypeName().c_str(), typeid(o).name());
    throw TypeMismatchExc(PExcLongMessage(buff));    
  }
  if (ownobj && dobj) delete dobj;
  dobj = po; ownobj = false;
} 

//! Reads in the TArray<T> object from the PPF input stream \b is
template <class T>
void FIO_TArray<T>::ReadSelf(PInPersist& is)
{
// On lit les 5 premiers uint_4
//  0: Numero de version,  1 : Type (Array, matrix, Vector, ...)  2 != 0 , has Info
//          1:Type = 0 TArray , 12=(4+8) TMatrix , 48=(16+32) TVector
  uint_4 itab[5];
  is.Get(itab,5);

  // Checking version number
  if (itab[0] < 2) 
    FileFormatExc("FIO_TArray<T>::ReadSelf() - Unsupported (old V<2) version"); 

  if (dobj == NULL) {
    if (itab[1] == 12)   dobj = new TMatrix<T>;
    else if (itab[1] == 48)   dobj = new TVector<T>;
    else   dobj = new TArray<T>;
  }
// On lit les tailles, etc ...
  int_4 tmpi4s[5];
  is.Get(tmpi4s, 5);
  dobj->ndim_ = tmpi4s[0];
  dobj->marowi_ = tmpi4s[1];
  dobj->macoli_ = tmpi4s[2];
  dobj->veceli_ = tmpi4s[3];
// tmpi4s[4] Reserved for future use 

// Tous les sa_size_t sont ecrit/lu en int_8 afin de maintenir la compatibilite  
// entre version du programme utilisant int_4 OU int_8 pour sa_size_t 
  int_8 tmpi8s[BASEARRAY_MAXNDIMS];
  int kk;

  is.Get(tmpi8s, BASEARRAY_MAXNDIMS);
  for(kk=0; kk<BASEARRAY_MAXNDIMS; kk++) 
    dobj->size_[kk] = tmpi8s[kk];
  is.Get(tmpi8s, BASEARRAY_MAXNDIMS);
  for(kk=0; kk<BASEARRAY_MAXNDIMS; kk++) 
    dobj->step_[kk] = tmpi8s[kk];

  is.Get(tmpi8s, 5);
  dobj->totsize_ = tmpi8s[0];
  dobj->offset_ = tmpi8s[1];
  dobj->minstep_ = tmpi8s[2];
  dobj->moystep_ = tmpi8s[3];
// tmpi8s[4] Reserved for future use 

// On lit le datablock
  is >> dobj->DataBlock();
// On ecrit le DVList info si necessaire
  if (itab[2] != 0)  is >> dobj->Info();  
}

//! Writes the TArray<T> object to the output PPF  stream \b os
template <class T>
void FIO_TArray<T>::WriteSelf(POutPersist& os) const
{
  if (dobj == NULL)   return;
//  On ecrit 5 uint_4 .... 
//  0: Numero de version,  1 : Type (Array, matrix, Vector, ...)  2 != 0 , has Info
//          1:Type = 0 TArray , 12=(4+8) TMatrix , 48=(16+32) TVector
  uint_4 typa = 0;
  TVector<T> * tv = dynamic_cast<TVector<T> *>(dobj);
  if (tv != NULL)  typa = 48;
  else {
  TMatrix<T> * tm = dynamic_cast<TMatrix<T> *>(dobj);
  if (tm != NULL)  typa = 12;
  else  typa = 0;
  }

  uint_4 itab[5];
  itab[0] = 2;  // Numero de version a 2 depuis Aout 2000  - Reza
  itab[1] = typa;  // Real object type
  itab[2] = (dobj->mInfo != NULL) ? 1 : 0;
  itab[3] = itab[4] = 0;
  os.Put(itab,5);

// On ecrit les tailles, etc ...
  int_4 tmpi4s[5];
  //  os.Put(dobj->ndim_);
  //  os.Put(dobj->marowi_);
  //  os.Put(dobj->macoli_);
  //  os.Put(dobj->veceli_);
  tmpi4s[0] = dobj->ndim_;
  tmpi4s[1] = dobj->marowi_;
  tmpi4s[2] = dobj->macoli_;
  tmpi4s[3] = dobj->veceli_;
  tmpi4s[4] = 0;   // Reserved for future use
  os.Put(tmpi4s, 5);

// Tous les sa_size_t sont ecrit en int_8 afin de pouvoir etre ecrit/relu
// entre version du programme utilisant int_4 OU int_8 pour sa_size_t 
  int_8 tmpi8s[BASEARRAY_MAXNDIMS];
  int kk;
  //  os.Put(dobj->size_, BASEARRAY_MAXNDIMS);
  for(kk=0; kk<BASEARRAY_MAXNDIMS; kk++) 
    tmpi8s[kk] = dobj->size_[kk];
  os.Put(tmpi8s, BASEARRAY_MAXNDIMS);
  //  os.Put(dobj->step_, BASEARRAY_MAXNDIMS);
  for(kk=0; kk<BASEARRAY_MAXNDIMS; kk++) 
    tmpi8s[kk] = dobj->step_[kk];
  os.Put(tmpi8s, BASEARRAY_MAXNDIMS);

  //  os.Put(dobj->totsize_);
  //  os.Put(dobj->offset_);
  //  os.Put(dobj->minstep_);
  //  os.Put(dobj->moystep_);
  tmpi8s[0] = dobj->totsize_;
  tmpi8s[1] = dobj->offset_;
  tmpi8s[2] = dobj->minstep_;
  tmpi8s[3] = dobj->moystep_;
  tmpi8s[4] = 0;  // Reserved for future use 
  os.Put(tmpi8s, 5);
  
// On ecrit le datablock
  os << dobj->DataBlock();
// On ecrit le DVList info si necessaire
  if (itab[2] != 0)  os << dobj->Info();
}




///////////////////////////////////////////////////////////////
#ifdef __CXX_PRAGMA_TEMPLATES__
// Instances des delegues FileIO (PPersist)
#pragma define_template FIO_TArray<uint_1>
#pragma define_template FIO_TArray<uint_2>
#pragma define_template FIO_TArray<uint_4>
#pragma define_template FIO_TArray<uint_8>
#pragma define_template FIO_TArray<int_1>
#pragma define_template FIO_TArray<int_2>
#pragma define_template FIO_TArray<int_4>
#pragma define_template FIO_TArray<int_8>
#pragma define_template FIO_TArray<r_4>
#pragma define_template FIO_TArray<r_8>
#pragma define_template FIO_TArray< complex<r_4> >
#pragma define_template FIO_TArray< complex<r_8> >
#ifdef SO_LDBLE128
#pragma define_template FIO_TArray<r_16>
#pragma define_template FIO_TArray< complex<r_16> >
#endif
#endif

#if defined(ANSI_TEMPLATES) || defined(GNU_TEMPLATES)
// Instances des delegues FileIO (PPersist)
template class FIO_TArray<uint_1>;
template class FIO_TArray<uint_2>;
template class FIO_TArray<uint_4>;
template class FIO_TArray<uint_8>;
template class FIO_TArray<int_1>;
template class FIO_TArray<int_2>;
template class FIO_TArray<int_4>;
template class FIO_TArray<int_8>;
template class FIO_TArray<r_8>;
template class FIO_TArray<r_4>;
template class FIO_TArray< complex<r_4> >;
template class FIO_TArray< complex<r_8> >;
#ifdef SO_LDBLE128
template class FIO_TArray<r_16>;
template class FIO_TArray< complex<r_16> >;
#endif
#endif

} // FIN namespace SOPHYA 
