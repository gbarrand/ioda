// Classe pour la gestion de persistance pour NDataBlock<T>
//                         C.Magneville          04/99-03/2000
// LAL (Orsay) / IN2P3-CNRS  DAPNIA/SPP (Saclay) / CEA
#include "sopnamsp.h"
#include "machdefs.h"
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <complex>
#include "pexceptions.h"
#include "datatype.h"
#include "fiondblock.h"
#include <typeinfo>

////////////////////////////////////////////////////////////////
// -------------------------------------------------------------------------
//   Les objets delegues pour la gestion de persistance 
// -------------------------------------------------------------------------
/*!
   \class SOPHYA::FIO_NDataBlock
   \ingroup BaseTools
   Handler for PPF persistence of NDataBlock<T> classes. 
*/

/*
template <class T>
void ObjFileIO< NDataBlock<T> >::ReadSelf(PInPersist& is)
template <class T>
void ObjFileIO< NDataBlock<T> >::WriteSelf(POutPersist& os)
*/


template <class T>
FIO_NDataBlock<T>::FIO_NDataBlock()
{
dobj=new NDataBlock<T>;
ownobj=true;
}

template <class T>
FIO_NDataBlock<T>::FIO_NDataBlock(string const & filename) 
{
dobj=new NDataBlock<T>;
ownobj=true; 
Read(filename);
}

template <class T>
FIO_NDataBlock<T>::FIO_NDataBlock(const NDataBlock<T> & obj) 
{ 
dobj = new NDataBlock<T>(obj);
ownobj=true; 
}

template <class T>
FIO_NDataBlock<T>::FIO_NDataBlock(NDataBlock<T> * obj) 
{ 
dobj = obj;
ownobj=false; 
}

template <class T>
FIO_NDataBlock<T>::~FIO_NDataBlock()
{
if (ownobj && dobj) delete dobj;
}

template <class T>
AnyDataObj* FIO_NDataBlock<T>::DataObj()
{
return(dobj);
}

template <class T>
void FIO_NDataBlock<T>::SetDataObj(AnyDataObj & o)
{
NDataBlock<T> * po = dynamic_cast< NDataBlock<T> * >(&o);
if (po == NULL) {
  char buff[160];
  sprintf(buff,"FIO_NDataBlock<T><%s>::SetDataObj(%s) - Object type  error ! ",
	  DataTypeInfo<T>::getTypeName().c_str(), typeid(o).name());
  throw TypeMismatchExc(PExcLongMessage(buff));    
}
if (ownobj && dobj) delete dobj;
dobj = po; ownobj = false;
} 

template <class T>
uint_8 FIO_NDataBlock<T>::getMemOId() const
{
  uint_8 rv = 0;
  if (dobj) rv = (uint_8)(dobj->DRefId());
  return(rv);
}

template <class T>
void FIO_NDataBlock<T>::ShareDataReference(PPersist & pp)
{
  FIO_NDataBlock<T> *ppo = dynamic_cast< FIO_NDataBlock<T> * >(&pp);
  if (ppo == NULL) throw TypeMismatchExc("FIO_NDataBlock<T>::ShareDataReference() - Type Mismatch Error");
  if (ppo->dobj) {
    if (dobj == NULL) { dobj = new NDataBlock<T>; ownobj = true; } 
    dobj->Share(*(ppo->dobj));
  }
}

template <class T>
PPersist* FIO_NDataBlock<T>::CloneSharedReference()
{
  FIO_NDataBlock<T> * ppo = new FIO_NDataBlock<T>;
  if (dobj) (ppo->dobj)->Share(*dobj);
  return(ppo);
}

//---------------------------------------------------------------------------
// Pour compatibilite de lecture avec PPF V2 
inline void PIOSReadArray(PInPersist & is, uint_1 * arr, size_t n)
{ is.GetBytes(arr, n); }
inline void PIOSReadArray(PInPersist & is, int_1 * arr, size_t n)
{ is.Get(arr, n); }
inline void PIOSReadArray(PInPersist & is, uint_2 * arr, size_t n)
{ is.Get(arr, n); }
inline void PIOSReadArray(PInPersist & is, int_2 * arr, size_t n)
{ is.Get(arr, n); }
inline void PIOSReadArray(PInPersist & is, uint_4 * arr, size_t n)
{ is.Get(arr, n); }
inline void PIOSReadArray(PInPersist & is, int_4 * arr, size_t n)
{ is.Get(arr, n); }
inline void PIOSReadArray(PInPersist & is, uint_8 * arr, size_t n)
{ is.Get(arr, n); }
inline void PIOSReadArray(PInPersist & is, int_8 * arr, size_t n)
{ is.Get(arr, n); }
inline void PIOSReadArray(PInPersist & is, r_4 * arr, size_t n)
{ is.Get(arr, n); }
inline void PIOSReadArray(PInPersist & is, r_8 * arr, size_t n)
{ is.Get(arr, n); }
inline void PIOSReadArray(PInPersist & is, complex<float> * arr, size_t n)
{ r_4 * pr = (r_4 *)arr; is.Get(pr, n*2); }
inline void PIOSReadArray(PInPersist & is, complex<double> * arr, size_t n)
{ r_8 * pr = (r_8 *)arr; is.Get(pr, n*2); }
#ifdef SO_LDBLE128
// ces fonctions ne devraient jamais etre appelees ( pas de r_16 en V_PPF <= 3 ) 
inline void PIOSReadArray(PInPersist & is, r_16 * arr, size_t n)  
{ is.Get(arr, n); }  
inline void PIOSReadArray(PInPersist & is, complex<long double> * arr, size_t n)
{ r_16 * pr = (r_16 *)arr; is.Get(pr, n*2); }
#endif
//---------------------------------------------------------------------------

template <class T>
void FIO_NDataBlock<T>::ReadSelf(PInPersist& is)
{
// On lit les 3 premiers uint_8
uint_8 itab[3];
is.Get(itab, 3);
if (dobj == NULL) { 
  if (itab[1] > 0) dobj = new NDataBlock<T>(itab[1]);
  else dobj = new NDataBlock<T>(); 
}
else { 
  if (itab[1] != dobj->Size()) { 
    if (itab[1] > 0)  dobj->ReSize(itab[1]);
    else dobj->Dealloc(); 
  }
}
// lecture de l'unite si applicable (Version>=2 et itab[2].Bit[0] set)
Units un;
bool sdone=false;
if ((itab[0]>1)&&(itab[2]&1))  { is >> un;  sdone=true; }
if (dobj->Size() < 1)  return;  // Pas de donnees a lire ...
// On lit le tableau de nombres
if (is.Version() <= 2)   // lecture ancienne version PPF
  PIOSReadArray(is, dobj->Data(), dobj->Size()); 
else is.Get(dobj->Data(), dobj->Size());
if (sdone) dobj->SetUnits(un);
}


template <class T>
void FIO_NDataBlock<T>::WriteSelf(POutPersist& os) const
{
if (dobj == NULL)   return;  // Attention - $CHECK$ Reza 26/04/99
//  On ecrit 3 uint_8 
//  0: Numero de version,  1: Taille,  2: bits d'etat 
//  Numero de version 2 en Novembre 2012, apres ajout Units mSRef->unit_p
//      Bit 0 de itab[2] a 1 si  mSRef->unit_p non nul 
uint_8 itab[3];
itab[0] = 2;
itab[1] = dobj->Size();
itab[2] = 0;
bool sdone;
Units un=dobj->GetUnits(sdone);
// Unite definie -> (Version>=2 et itab[2].Bit[0] set)
if (sdone)  itab[2] |= 1;
os.Put(itab, 3);
if (sdone)  os<<un;   // ecriture de l'unite si definie
//  On ecrit le tableau de nombres 
if (dobj->Size() > 0)   // On ecrit les donnees, s'il y en a ... (sz>0)
  os.Put(dobj->Data(), dobj->Size());
}

///////////////////////////////////////////////////////////////
#ifdef __CXX_PRAGMA_TEMPLATES__
// Instances des delegues FileIO (PPersist)
#pragma define_template FIO_NDataBlock<uint_1>
#pragma define_template FIO_NDataBlock<uint_2>
#pragma define_template FIO_NDataBlock<uint_4>
#pragma define_template FIO_NDataBlock<uint_8>
#pragma define_template FIO_NDataBlock<int_1>
#pragma define_template FIO_NDataBlock<int_2>
#pragma define_template FIO_NDataBlock<int_4>
#pragma define_template FIO_NDataBlock<int_8>
#pragma define_template FIO_NDataBlock<r_8>
#pragma define_template FIO_NDataBlock<r_4>
#pragma define_template FIO_NDataBlock< complex<r_4> >
#pragma define_template FIO_NDataBlock< complex<r_8> >
#ifdef SO_LDBLE128
#pragma define_template FIO_NDataBlock<r_16>
#pragma define_template FIO_NDataBlock< complex<r_16> >
#endif

#endif

#if defined(ANSI_TEMPLATES) || defined(GNU_TEMPLATES)
namespace SOPHYA {
// Instances des delegues FileIO (PPersist)
template class FIO_NDataBlock<uint_1>;
template class FIO_NDataBlock<uint_2>;
template class FIO_NDataBlock<uint_4>;
template class FIO_NDataBlock<uint_8>;
template class FIO_NDataBlock<int_1>;
template class FIO_NDataBlock<int_2>;
template class FIO_NDataBlock<int_4>;
template class FIO_NDataBlock<int_8>;
template class FIO_NDataBlock<r_8>;
template class FIO_NDataBlock<r_4>;
template class FIO_NDataBlock< complex<r_4> >;
template class FIO_NDataBlock< complex<r_8> >;
#ifdef SO_LDBLE128
template class FIO_NDataBlock<r_16>;
template class FIO_NDataBlock< complex<r_16> >;
#endif
}
#endif
