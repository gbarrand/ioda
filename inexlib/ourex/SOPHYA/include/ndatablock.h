// This may look like C code, but it is really -*- C++ -*-
// Gestion de block de donnees avec partage de references
//                         C.Magneville          04/99
// LAL (Orsay) / IN2P3-CNRS  DAPNIA/SPP (Saclay) / CEA
#ifndef NDATABLOCK_H
#define NDATABLOCK_H

#include "machdefs.h"
#include <stdlib.h>        /* pour que size_t soit defini */
#include "anydataobj.h"
#include "sunitpcst.h"
#include <complex>
#include <iostream>

namespace SOPHYA {

////////////////////////////////////////////////////////////////
//// ------------------- Class Bridge ----------------------- //
////////////////////////////////////////////////////////////////
/*!
  \class Bridge
  \ingroup BaseTools
  This class is use by NDataBlock. It allows sharing of datas
  with external structures : by example, if you want to connect
  a Blitz data structure with a NDataBlock or a TMatrix or ...
  \sa NDataBlock
*/

// Classe pour permettre de partager des donnees avec 
// un autre systeme de gestion de references (ex avec Blitz)
//! Empty class which allows data sharing with external structures (for NDataBlock)
class Bridge {
public:
  Bridge() { } 
  virtual ~Bridge() { }
};


class ThSafeOp; //forward class declaration for ThreadSafe operations (Ref.Count/Share) 

////////////////////////////////////////////////////////////////
//// ----------------- Class NDataBlock --------------------- //
////////////////////////////////////////////////////////////////
// classe de container avec partage de reference
//! Container of data with reference sharing
template <class T>
class NDataBlock : public AnyDataObj {

public:

  // Methodes statiques pour debug.
  static void SetPrintDebug(int prtdbglevel=1);
  static void ResetDebug(size_t nallocdata=0, size_t nallocsref=0);
  static void PrintDebug();

  // Creation / destruction 
  NDataBlock(size_t n, bool fzero=true);
  NDataBlock(size_t n, T* data, Bridge* br=NULL);
  NDataBlock();
  NDataBlock(const NDataBlock<T>& a);
  NDataBlock(const NDataBlock<T>& a,bool share);
  virtual ~NDataBlock();

  // Temporaire?
  //! Return true if data block is temporay
  inline bool IsTemp(void) const {return mIsTemp;}
  //! Set temporary caracter of data block
  inline void SetTemp(bool temp=false) const {mIsTemp = temp;}
  // Depuis que le createur par copie partage les donnees
  // la seule utilisation de SetTemp et pour faire les operations du type:
  // NDataBlock = NDataBlock + NDataBlock + NDataBlock ...

  // Gestion taille/Remplissage
  void Clone(const NDataBlock<T>& a);
  void CloneOrShare(const NDataBlock<T>& a);
  void Share(const NDataBlock<T>& a);
  void FillFrom(size_t n,T* data);
  //! Re-set all data values to \b v
  inline void Reset(T v=0)
         {if(mSz==0) return; T *p=Begin(),*pe=End(); while(p<pe) *p++=v;}

  // ReSize redimmensionne une structure pour "n" donnees.
  // Les donnees precedentes sont perdues (pour cette classe)
  // et le nouveau tableau mis a zero si fzero=true. La nouvelle structure de
  // donnees n'a qu'une reference (celle de cette classe).
  //! Re-size the data structure
  /*! Old datas are lost (for this class). The new values are set 
    to zero if \b fzero=true .
    The new data structure has only one reference (itself!). */
  inline void ReSize(size_t n, bool fzero=true) {Alloc(n,NULL,NULL,fzero);}

  void Realloc(size_t nnew,bool force=false);
  
  //  Public, thread safe interface to Delete() (set the size to zero)
  void Dealloc();

  // Informations pointeur/data
  //! Return pointer on data structure.
  inline T* Data()
         {if(mSRef) return mSRef->data; else return NULL;}
  //! Return pointer on data structure.
  inline T* Data() const
         {if(mSRef) return mSRef->data; else return NULL;}
  //! Return the size of the data structure
  inline size_t Size() const    {return mSz;}
  //! Return the \b i th element of  the data structure
  inline T& operator()(size_t i)       {return *(mSRef->data+i);}
  //! Return the \b i th element of  the data structure
  inline T  operator()(size_t i) const {return *(mSRef->data+i);}
  //! Return pointer to the beginning of the data structure.
  inline T*        Begin()        {return mSRef->data;}
  //! Return pointer to the beginning of the data structure.
  inline T const*  Begin() const  {return mSRef->data;} 
  //! Return pointer to the end of the data structure.
  inline T*        End()          {return mSRef->data+mSz;}
  //! Return pointer to the end of the data structure.
  inline T const*  End() const    {return mSRef->data+mSz;}
  //! Return the number of references to the data structure
  inline size_t NRef() const {if(mSRef) return mSRef->nref;  else return 0; }

  //! Define or change the physical unit of the data 
  void SetUnits(const Units& un);
  //! Return the physical unit of the data. Flag sdone is set to true if SetUnits() has been called.
  Units GetUnits(bool& sdone) const;
  //! Return the physical unit of the data 
  inline Units GetUnits() const  {bool fg; return GetUnits(fg); }

  // Impression
  void Print(ostream& os, size_t i1=0,size_t n=10) const;
  //! print infos and datas (from \b i1 to \b i2) on stdout.
  inline void Print(size_t i1=0,size_t n=0) const {Print(cout,i1,n);}

  //
  T Sum(size_t i1=0,size_t n=0) const;
  T Product(size_t i1=0,size_t n=0) const;

  // Surcharge d'operateurs INPLACE: A = x , A = B , A @= x , A @= B
  NDataBlock<T>& operator = (const NDataBlock<T>& a);
  NDataBlock<T>& operator = (T v);

  NDataBlock<T>& operator += (T b);
  NDataBlock<T>& operator -= (T b);
  NDataBlock<T>& operator *= (T b);
  NDataBlock<T>& operator /= (T b);

  NDataBlock<T>& operator += (const NDataBlock<T>& a);
  NDataBlock<T>& operator -= (const NDataBlock<T>& a);
  NDataBlock<T>& operator *= (const NDataBlock<T>& a);
  NDataBlock<T>& operator /= (const NDataBlock<T>& a);

  // Surcharge d'operateurs: C = A @ x , C = A @ B
  NDataBlock<T> Add(T b) const;
  NDataBlock<T> Sub(T b,bool fginv=false) const;
  NDataBlock<T> Mul(T b) const;
  NDataBlock<T> Div(T b,bool fginv=false) const;

  NDataBlock<T> Add(const NDataBlock<T>& b) const;
  NDataBlock<T> Sub(const NDataBlock<T>& b) const;
  NDataBlock<T> Mul(const NDataBlock<T>& b) const;
  NDataBlock<T> Div(const NDataBlock<T>& b) const;

  //! Return thye associated object Id (or DataRef Id)
  inline uint_8 DRefId() { return ((mSRef)?mSRef->dsid:0); }
  //! assign a new object Id (or DataRef Id) - useful for PPF write operations 
  inline void RenewDRefId() { if (mSRef) mSRef->dsid = AnyDataObj::getUniqueId(); } 
  //! assign a new object Id (or DataRef Id) - useful for PPF write operations 
  inline void RenewObjId() { if (mSRef) mSRef->dsid = AnyDataObj::getUniqueId(); }
  

protected:
  //! NDREF structure for reference management
  typedef struct {
    size_t nref;      //!< Number of references to the data structure
    uint_8 dsid;      //!< Data structure Id - Used by FIO_NDataBlock
    T* data;          //!< Pointer to data structure itself
    Bridge* bridge;   //!< Pointer to a bridge for the data structure
    Units* unit_p;    //!<  Optional physical unit of the data 
  } NDREF;

  void Alloc(size_t n,T* data=NULL,Bridge* br=NULL, bool zero=true);
  void Delete(void);

  static int Debug_NDataBlock; //!< DEBUG: set debug level (all type<T> classes)
  static size_t NallocData; //!< DEBUG: number of allocations (all type<T> classes)
  static size_t NallocSRef; //!< DEBUG: number of references (all type<T> classes)
  static ThSafeOp* gThsop;  //!< Mutex For thread safe operation 

  size_t       mSz;      //!< size of data structure
  NDREF*       mSRef;    //!< NDREF structure for reference management
  mutable bool mIsTemp;  //!< true if class is temporary
};

//! Define operator \<\< for printing
template<class T>
inline ostream& operator << (ostream& os, const NDataBlock<T>& a)
                      {a.Print(os); return(os);}

//! Add a constant to datas and return NDataBlock : ND = NDa + b
template<class T>
inline NDataBlock<T> operator + (const NDataBlock<T>& a,T b)
                      {return a.Add(b);}
//! Add a constant to datas and return NDataBlock : ND = b + NDa
template<class T>
inline NDataBlock<T> operator + (T b,const NDataBlock<T>& a)
                      {return a.Add(b);}
//! Substract a constant to datas and return NDataBlock : ND = NDa - b
template<class T>
inline NDataBlock<T> operator - (const NDataBlock<T>& a,T b)
                      {return a.Sub(b);}
//! Substract a constant to datas and return NDataBlock : ND = b - NDa
template<class T>
inline NDataBlock<T> operator - (T b,const NDataBlock<T>& a)
                      {return a.Sub(b,true);}
//! Multiply datas by a constant and return NDataBlock : ND = NDa * b
template<class T>
inline NDataBlock<T> operator * (const NDataBlock<T>& a,T b)
                      {return a.Mul(b);}
//! Multiply datas by a constant and return NDataBlock : ND = b * NDa
template<class T>
inline NDataBlock<T> operator * (T b,const NDataBlock<T>& a)
                      {return a.Mul(b);}
//! Divide datas by a constant and return NDataBlock : ND = NDa / b
template<class T>
inline NDataBlock<T> operator / (const NDataBlock<T>& a,T b)
                      {return a.Div(b);}
//! Divide a constant by datas and return NDataBlock : ND = b / NDa
template<class T>
inline NDataBlock<T> operator / (T b,const NDataBlock<T>& a)
                      {return a.Div(b,true);}

//! Add datas of two data blocks and return NDataBlock : ND = NDa + NDb
template<class T>
inline NDataBlock<T> operator + (const NDataBlock<T>& a,const NDataBlock<T>& b)
                      {return a.Add(b);}
//! Substract datas of two data blocks and return NDataBlock : ND = NDa - NDb
template<class T>
inline NDataBlock<T> operator - (const NDataBlock<T>& a,const NDataBlock<T>& b)
                      {return a.Sub(b);}
//! Multiply datas of two data blocks and return NDataBlock : ND = NDa * NDb
template<class T>
inline NDataBlock<T> operator * (const NDataBlock<T>& a,const NDataBlock<T>& b)
                      {return a.Mul(b);}
//! Divide datas of two data blocks and return NDataBlock : ND = NDa / NDb
template<class T>
inline NDataBlock<T> operator / (const NDataBlock<T>& a,const NDataBlock<T>& b)
                      {return a.Div(b);}

//--------- extern template declarations (if needed) -----------
#if defined ( NEED_EXT_DECL_TEMP ) && !defined( NDATABLOCK_CC_BFILE )
extern template class NDataBlock<uint_1>;
extern template class NDataBlock<uint_2>;
extern template class NDataBlock<int_1>;
extern template class NDataBlock<int_2>;
extern template class NDataBlock<int_4>;
extern template class NDataBlock<int_8>;
extern template class NDataBlock<uint_4>;
extern template class NDataBlock<uint_8>;
extern template class NDataBlock<r_4>;
extern template class NDataBlock<r_8>;
extern template class NDataBlock< complex<r_4> >;
extern template class NDataBlock< complex<r_8> >;
#ifdef SO_LDBLE128
extern template class NDataBlock<r_16>;
extern template class NDataBlock< complex<r_16> >;
#endif

#endif  // Fin de if defined ( NEED_EXT_DECL_TEMP )

} // Fin du namespace

#endif
