// This may look like C code, but it is really -*- C++ -*-
//                         C.Magneville          05/99
#ifndef TVector_SEEN
#define TVector_SEEN

#include "tmatrix.h"

namespace SOPHYA {

//! Class of vector (line or column)
template <class T>
class TVector : public TMatrix<T> {
public:

#include "tvector_tsnl.h"  /* For two level name look-up gcc >= 3.4 */

  // Creation / destruction 
  TVector();
  TVector(sa_size_t n, short lcv=BaseArray::AutoVectorType, short mm=BaseArray::AutoMemoryMapping, bool fzero=true);
  TVector(const TVector<T>& v);
  TVector(const TVector<T>& v, bool share);
  TVector(const TArray<T>& a, bool share=true, short lcv=BaseArray::AutoVectorType);
  TVector(const BaseArray& a, bool pack=true);
  TVector(const vector<T>& v, short lcv=BaseArray::AutoVectorType);

  virtual ~TVector();

  //! Operator = 
  /*! \warning Datas are copied (cloned) from \b a.
      \sa NDataBlock::operator=(const NDataBlock<T>&) */
  inline  TVector<T>& operator = (const TVector<T>& a)
                       { Set(a);  return(*this); }
  //! Operator = between a vector and a matrix 
  inline  TVector<T>& operator = (const TMatrix<T>& a)
                       { Set(a);  return(*this); }
  //! Operator = between a vector and an array 
  inline  TVector<T>& operator = (const TArray<T>& a)
                       { Set(a);  return(*this); }
  //! Operator = between Vectors with different types
  inline  TVector<T>& operator = (const BaseArray& a)
                       { SetBA(a);  return(*this); }
    
  // Gestion taille/Remplissage
  void ReSize(sa_size_t n, short lcv=BaseArray::SameVectorType, bool fzero=true); 
  //! a synonym (alias) for method ReSize(sa_size_t, short)
  inline void SetSize(sa_size_t n, short lcv=BaseArray::SameVectorType, bool fzero=true)
              { ReSize(n, lcv, fzero); }
  void Realloc(sa_size_t n, short lcv=BaseArray::SameVectorType, bool force=false);
    
  // Remplissage from/to a STL vector
  sa_size_t FillFr(const vector<T>& v,bool noresize=false);
  //! Copy the std::vector<T> \b v content (to *this)  (alias for FillFr )
  inline sa_size_t CopyFrom(const vector<T>& v,bool noresize=false)
  { return FillFr(v,noresize) ; }
  sa_size_t FillTo(vector<T>& v,bool addtoend=false);
  //! Copy object content to the std::vector<T> \b v  (alias for FillTo )
  inline sa_size_t CopyTo(vector<T>& v,bool addtoend=false)
  { return FillTo(v, addtoend); }
  vector<T> ConvertTostdvec();

  // Sub-Vector extraction $CHECK$ Reza 03/2000  Doit-on declarer cette methode const ?
  TVector<T> SubVector(Range relt) const ;
  //! Extract a vector define by Range \b relt
  inline TVector<T> operator () (Range relt) const 
                    { return SubVector(relt); }

  // Informations pointeur/data
  //! return the number of elements
  inline sa_size_t NElts() const {return Size(); }
  
  // Inline element acces methods 
  inline T const& operator()(sa_size_t n) const;
  inline T&       operator()(sa_size_t n);

  // Operateur d'affectation
  //! Fill the vector with Sequence \b seq
  inline  TVector<T>&  operator = (Sequence const & seq) { SetSeq(seq); return(*this); }

  // Operations diverses  avec une constante
  //! Set vector elements to constant value \b x
  inline  TVector<T>&  operator = (T x)             { SetT(x); return(*this); }
  //! Add constant value \b x to vector elements
  inline  TVector<T>&  operator += (T x)            { AddCst(x,*this); return(*this); }
  //! Substract constant value \b x to vector elements
  inline  TVector<T>&  operator -= (T x)            { SubCst(x,*this); return(*this); }
  //! Multiply vector elements by constant value \b x
  inline  TVector<T>&  operator *= (T x)            { MulCst(x,*this); return(*this); }
  //! Divide vector elements by constant value \b x
  inline  TVector<T>&  operator /= (T x)            { DivCst(x,*this); return(*this); }

  //  operations avec matrices 
  //! += : add a vector in place
  inline  TVector<T>&  operator += (const TVector<T>& a)  { AddElt(a,*this); return(*this); }
  //! += : substract a vector in place
  inline  TVector<T>&  operator -= (const TVector<T>& a)  { SubElt(a,*this); return(*this); }

  virtual string InfoString() const;

};

//  ---- inline acces methods ------

//! Return the value of element \b n
template <class T>
inline T const& TVector<T>::operator()(sa_size_t n) const
{
#ifdef SO_BOUNDCHECKING
  if (veceli_ == 0)   CheckBound(n, 0, 0, 0, 0, 4);
  else   CheckBound(0, n, 0, 0, 0, 4);
#endif
  return ( *( mNDBlock.Begin()+ offset_ + n*step_[veceli_] ) );
}

//! Return the value of element \b n
template <class T>
inline T & TVector<T>::operator()(sa_size_t n) 
{
#ifdef SO_BOUNDCHECKING
  if (veceli_ == 0)   CheckBound(n, 0, 0, 0, 0, 4);
  else   CheckBound(0, n, 0, 0, 0, 4);
#endif
  return ( *( mNDBlock.Begin()+ offset_ + n*step_[veceli_] ) );
}

// Typedef pour simplifier et compatibilite Peida
/*! \ingroup TArray
  \typedef Vector
  \brief To simplified TVector<r_8> writing
*/
typedef TVector<r_8> Vector;

//--------- extern template declarations (if needed) -----------
#if defined ( NEED_EXT_DECL_TEMP ) && !defined( TVECTOR_CC_BFILE )
extern template class TVector<uint_1>;
extern template class TVector<uint_2>;
extern template class TVector<uint_4>;
extern template class TVector<uint_8>;
extern template class TVector<int_1>;
extern template class TVector<int_2>;
extern template class TVector<int_4>;
extern template class TVector<int_8>;
extern template class TVector<r_4>;
extern template class TVector<r_8>;
extern template class TVector< complex<r_4> >;
extern template class TVector< complex<r_8> >;
#ifdef SO_LDBLE128
extern template class TVector<r_16>;
extern template class TVector< complex<r_16> >;
#endif
#endif  // Fin de if defined ( NEED_EXT_DECL_TEMP )

} // Fin du namespace

#endif
