// $Id: tvector.cc,v 1.30 2013/06/09 21:17:14 ansari Exp $
//                         C.Magneville          04/99
#include "sopnamsp.h"
#include "machdefs.h"
#include <stdlib.h>
#include "pexceptions.h"

#define TVECTOR_CC_BFILE  // avoid extern template declarations 
#include "tvector.h"

namespace SOPHYA {

/*!
  \class TVector
  \ingroup TArray

  The TVector class specializes the TMatrix class for representing 
  row or column vectors. TVector<T> objects can be initialised or converted 
  to std::vector<T>.

  \b Vector is a typedef for double precision floating point vectors ( TVector<r_8> ).

  \sa SOPHYA::TArray SOPHYA::TMatrix
  \sa SOPHYA::Range SOPHYA::Sequence
  \sa SOPHYA::MathArray

  The following sample code illustrates sub-vector manipulation:
  \code
  #include "array.h"
  // ...
  // Input Vector containing a noisy periodic signal
  Vector in(1024), out(1024);
  in = RandomSequence(RandomSequence::Gaussian, 0., 1.);
  for(int kk=0; kk<in.Size(); kk++)
    in(kk) += 2*sin(kk*0.05);
  // Compute the output vector by a simple low pass filter
  Vector out(1024);
  int w = 2;
  for(int k=w; k<in.Size()-w; k++)
    out(k) = in(Range(k-w, k+w).Sum()/(2.*w+1.);
  \endcode
*/

////////////////////////////////////////////////////////////////
//**** Createur, Destructeur

//! Default constructor
template <class T>
TVector<T>::TVector()
  : TMatrix<T>()
{
  arrtype_ = 2;   // Type = Vector
}

//! construct a vector
/*!
  \param n : number of elements
  \param lcv : line or column vector (BaseArray::AutoVectorType / SameVectorType / ColumnVector / RowVector)
  \param mm : memory mapping type
  \param fzero : if \b true , set vector elements to zero 
  \sa SelectVectorType
 */
template <class T>
TVector<T>::TVector(sa_size_t n, short lcv, short mm, bool fzero)
// Constructeur 
  : TMatrix<T>(1,1,mm,false)
{
  arrtype_ = 2;   // Type = Vector
  lcv = SelectVectorType(lcv);
  ReSize(n,lcv,fzero);
}

//! Constructor by copy
/*!
  \warning datas are \b SHARED with \b a.
  \sa NDataBlock::NDataBlock(const NDataBlock<T>&)
*/
template <class T>
TVector<T>::TVector(const TVector<T>& a)
// Constructeur par copie
  : TMatrix<T>(a)
{
  arrtype_ = 2;   // Type = Vector
}

//! Constructor by copy
/*!
  \param share : if true, share data. If false copy data
 */
template <class T>
TVector<T>::TVector(const TVector<T>& a, bool share)
// Constructeur par copie avec possibilite de forcer le partage ou non.
: TMatrix<T>(a, share)
{
  arrtype_ = 2;   // Type = Vector
}


//! Constructor of a vector from a TArray \b a
/*!
  \param a : TArray to be copied or shared
  \param share : if true, share data. If false copy data
  \param lcv : line or column vector ?
  \sa SelectVectorType
 */
template <class T>
TVector<T>::TVector(const TArray<T>& a, bool share, short lcv)
: TMatrix<T>(a, share)
{
  arrtype_ = 2;   // Type = Vector
  if (a.NbDimensions() == 0)  return;    // Reza-Nov 2011: we allow copy contrsuctor on non allocated arrays  
  if ( (size_[0] != 1) && (size_[1] != 1) )
    throw SzMismatchError("TVector<T>::TVector(const TArray<T>& a) NRows()!=1 && NCols()!=1 ");
  if ( (size_[0] == 1) && (size_[1] == 1) ) {
    if (lcv == SameVectorType) lcv = a.GetVectorType();
    if ( (lcv != ColumnVector) && (lcv != RowVector) ) lcv = GetDefaultVectorType();
    veceli_ = (lcv ==  ColumnVector ) ?  marowi_ : macoli_;
  }
}

//! Constructor of a vector from a TArray \b a , with a different data type
/*!
  vector size and memory layout are copied from the array \b a, or a packed vector is created if \b pack==true.
  \param a : original array, to copy sizes and data from
  \param pack : if \b true , create a packed vector, else same memory layout as \b a. 
*/
template <class T>
TVector<T>::TVector(const BaseArray& a, bool pack)
  : TMatrix<T>(a,pack)
{
  arrtype_ = 2;   // Type = Vector
  if (a.NbDimensions() == 0)  return;    // Reza-Nov 2011: we allow copy contrsuctor on non allocated arrays  
  if ( (size_[0] != 1) && (size_[1] != 1) )
    throw SzMismatchError("TVector<T>::TVector(const BaseArray& a,bool) NRows()!=1 && NCols()!=1 ");
}

//! Constructor from an STL vector
template <class T>
TVector<T>::TVector(const vector<T>& v, short lcv)
{
 sa_size_t n = v.size();
 if(n==0)
    throw SzMismatchError("TVector<T>::TVector(const vector<T>& v) size()==0 ");
 arrtype_ = 2;   // Type = Vector
 ReSize(n,lcv,false);
 for(sa_size_t i=0;i<n;i++) (*this)(i) = v[i];
}

//! Destructor
template <class T>
TVector<T>::~TVector()
{
}

//! Resize the vector
/*!
  \param n : number of elements
  \param lcv : line or column vector ?
  \sa SelectVectorType
 */
template <class T>
void TVector<T>::ReSize(sa_size_t n, short lcv, bool fzero)
{
  if( n == 0 ) 
    throw(SzMismatchError("TVector::ReSize() n = 0 "));
  sa_size_t r,c;
  if (lcv == SameVectorType)  lcv = GetVectorType();
  else if ( (lcv != ColumnVector) && (lcv != RowVector) ) lcv = GetDefaultVectorType();
  if (lcv == ColumnVector) { r = n;  c = 1; }
  else { c = n; r = 1; }
  TMatrix<T>::ReSize(r,c,BaseArray::SameMemoryMapping,fzero);
  veceli_ = (lcv ==  ColumnVector ) ?  marowi_ : macoli_;
}

//! Re-allocate space for the vector
/*!
  \param n : number of elements
  \param lcv : line or column vector ?
  \param force : if true re-allocation is forced, if not it occurs
          only if the required space is greater than the old one.
  \sa ReSize SelectVectorType
 */
template <class T>
void TVector<T>::Realloc(sa_size_t n, short lcv, bool force)
{
  if( n == 0 ) 
    throw(SzMismatchError("TVector::Realloc() n = 0 "));
  sa_size_t r,c;
  if (lcv == SameVectorType)  lcv = GetVectorType();
  else if ( (lcv != ColumnVector) && (lcv != RowVector) ) lcv = GetDefaultVectorType();
  if (lcv == ColumnVector) { r = n;  c = 1; }
  else { c = n; r = 1; }
  TMatrix<T>::Realloc(r,c,SameMemoryMapping,force);
  veceli_ = (lcv ==  ColumnVector ) ?  marowi_ : macoli_;
}

// $CHECK$ Reza 03/2000  Doit-on declarer cette methode const ?
//! Return a subvector define by \b Range \b relt
template <class T>
TVector<T> TVector<T>::SubVector(Range relt) const
{
  Range rr=Range::first();
  Range cr=Range::first();
  if (GetVectorType() == ColumnVector )  rr = relt; 
  else cr = relt;
  TMatrix<T> const & mtx = (*this);
  TVector sv( mtx(rr, cr) , true, GetVectorType() );
  return(sv);  
}

//! Return info on number of rows, column and type \b T
template <class T>
string TVector<T>::InfoString() const
{
  string rs = "TVector<";
  rs += typeid(T).name();
  char buff[64];
  sprintf(buff, ">(%ld) (nr=%ld, nc=%ld)", (long)NElts(), (long)NRows(), (long)NCols());
  rs += buff;
  return(rs);  

}

/*!
  \brief Fill (or copy) from an STL vector

  \param v : STL vector to copy
  \param noresize : "true" means TVector keeps its size, if "false", ReSize(, ,false) is called 
  \warning Filling is always done starting at the first element (*this)(0)
 */
template <class T>
sa_size_t TVector<T>::FillFr(const vector<T>& v,bool noresize)
{
 sa_size_t n = v.size();
 if(n==0) return 0; //STL vector de taille nulle!
 if(!noresize) ReSize(n,SameVectorType,false);
 if(Size()<n) n = Size();
 if(n==0) return 0; //TVector de taille nulle sans resize!
 for(sa_size_t i=0;i<n;i++) (*this)(i) = v[i];
 return n;
}

/*!
  \brief Fill an STL vector (copy elements to the std::vector<T> \b v) 

  \param v : STL vector to fill
  \param addtoend : "true" means TVector elements will be appended at the end of "v"
 */
template <class T>
sa_size_t TVector<T>::FillTo(vector<T>& v,bool addtoend)
{
 sa_size_t n = Size();
 if(n==0) return 0; //TVector de taille nulle!
 if(!addtoend) v.resize(0);
 for(sa_size_t i=0;i<n;i++) v.push_back((*this)(i));
 return n;
}

/*!
  \brief to std::vector<T> conversion function 

  Return an std::vector<T> with the same size as the original vector and with elements std::vec[i]=(*this)(i)
*/
template <class T>
vector<T> TVector<T>::ConvertTostdvec()
{
 sa_size_t n = Size();
 std::vector<T> rv(n);
 for(sa_size_t i=0;i<n;i++)  rv[i]=(*this)(i);
 return rv;
}

///////////////////////////////////////////////////////////////
#ifdef __CXX_PRAGMA_TEMPLATES__
#pragma define_template TVector<uint_1>
#pragma define_template TVector<uint_2>
#pragma define_template TVector<uint_4>
#pragma define_template TVector<uint_8>
#pragma define_template TVector<int_1>
#pragma define_template TVector<int_2>
#pragma define_template TVector<int_4>
#pragma define_template TVector<int_8>
#pragma define_template TVector<r_4>
#pragma define_template TVector<r_8>
#pragma define_template TVector< complex<r_4> > 
#pragma define_template TVector< complex<r_8> > 
#ifdef SO_LDBLE128
#pragma define_template TVector<r_16>
#pragma define_template TVector< complex<r_16> > 
#endif
#endif

#if defined(ANSI_TEMPLATES) || defined(GNU_TEMPLATES)
template class TVector<uint_1>;
template class TVector<uint_2>;
template class TVector<uint_4>;
template class TVector<uint_8>;
template class TVector<int_1>;
template class TVector<int_2>;
template class TVector<int_4>;
template class TVector<int_8>;
template class TVector<r_4>;
template class TVector<r_8>;
template class TVector< complex<r_4> >;
template class TVector< complex<r_8> >;
#ifdef SO_LDBLE128
template class TVector<r_16>;
template class TVector< complex<r_16> >;
#endif
#endif

} // FIN namespace SOPHYA 

