//      template array class for numerical types
//                     R. Ansari, C.Magneville   03/2000

#include "machdefs.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <iomanip>

#include "pexceptions.h"

#define TARRAY_CC_BFILE  // avoid extern template declarations 
#include "tarray.h"

namespace SOPHYA {

/*!
  \class TArray
  \ingroup TArray
  Class for template arrays with numerical data types (int, float, complex).

  This class handles arrays with number of dimensions up to
  \ref BASEARRAY_MAXNDIMS "BASEARRAY_MAXNDIMS" (=5). The class has a performant 
  memory management system, including reference sharing for the array data.
  (copy constructor and sub-arrays (or slices)). 

  An important feature of this class is the transparent handling of sub-arrays,
  or slices. Arbitrary sub-arrays or slices can be defined, provided regular 
  spacing along each array axe (or dimension). 
  The second example below illustrate the use of this possibility.
  
  Standard arithmetic operations, sum or product as well as application of usual
  math functions (Sin, Cos ...) on numerical arrays are implemented.
  These operations usually provide high performance, despite of the complex 
  memory management pattern.

  ASCII input/output (or read write) and binary I/O (persistence) in different
  formats are also provided through helper (or handler) classes.

  This class is mainly intented for arrays with large number of data elements.
  (Size() \> 100 .. 1000). Arrays with few data elements (\< 10) have significant 
  memory overhead, due to variables describing array shape and memory organisation.
  However, a single higher dimensional array can be used to represent a large number 
  of identical size small arrays. For example, TArray<T> tab(2,2, 1000) can be used
  to hold 1000 2x2 matrices.

  \warning Notice that array elements along the X axis are contiguous in memory, 
  independent of the array rank (number of dimensions), for packed arrays.

  \b Array is a typedef for double precision floating point arrays ( TArray<r_8> )

  \sa SOPHYA::Range
  \sa SOPHYA::Sequence
  \sa SOPHYA::MathArray
  \sa SOPHYA::NDataBlock

  \code
  #include "array.h"
  // ...
  // Creating and initialising a 1-D array of integers
  TArray<int> ia(5);
  EnumeratedSequence es;
  es = 24, 35, 46, 57, 68;
  ia = es;
  cout << "Array<int> ia = \n" << ia;
  // 2-D array of floats 
  TArray<r_4> b(6,4), c(6,4);
  // Initializing b with a constant
  b = 2.71828;
  // Filling c with random numbers  
  c = RandomSequence(); 
  // Arithmetic operations
  TArray<r_4> d = b+0.3f*c;
  cout << "Array<float> d = \n" << d;  
  \endcode

  Example for sub-arrays, or slices
  \code 
  // Creating and initialising a 2-D (6 x 4) array of integers
  TArray<int> iaa(6, 4);
  iaa = RegularSequence(1,2);
  cout << "Array<int> iaa = \n" << iaa;
  // We extract a sub-array - data is shared with iaa
  TArray<int> iae = iaa(Range(1, Range::lastIndex(), 3) ,
                        Range::all(), Range::first() );
  cout << "Array<int> iae=subarray(iaa) = \n" << iae;
  // Changing iae elements changes corresponding iaa elements
  iae = 0;
  cout << "Array<int> iae=0 --> iaa = \n" << iaa;
  \endcode 
*/

/*! \ingroup TArray
  \typedef sa_size_t
  \brief Array index range and size, defined to be a 4-byte or 8-byte integer 
*/

// -------------------------------------------------------
//                Methodes de la classe
// -------------------------------------------------------

////////////////////////// Les constructeurs / destructeurs

//! Default constructor
template <class T>
TArray<T>::TArray()
  : BaseArray() , mNDBlock() 
{
}

//! Constructor
/*!
  \param ndim : number of dimensions (less or equal to
                \ref BASEARRAY_MAXNDIMS "BASEARRAY_MAXNDIMS")
  \param siz[ndim] : size along each dimension
  \param step : step (same for all dimensions)
  \param fzero : if \b true , set array elements to zero 
 */
template <class T>
TArray<T>::TArray(int_4 ndim, const sa_size_t * siz, sa_size_t step, bool fzero)
  : BaseArray() , mNDBlock(ComputeTotalSize(ndim, siz, step, 1), fzero)
{
  string exmsg = "TArray<T>::TArray(int_4, sa_size_t *, sa_size_t)";
  if (!UpdateSizes(ndim, siz, step, 0, exmsg))  throw( ParmError(exmsg) );
}

//! Constructor
/*!
  \param nx,ny,nz,nt,nu : sizes along first, second, third, fourth and fifth dimension
  \param fzero : if \b true , set array elements to zero 
 */
template <class T>
TArray<T>::TArray(sa_size_t nx, sa_size_t ny, sa_size_t nz, sa_size_t nt, sa_size_t nu, bool fzero)
  : BaseArray() , mNDBlock(nx*((ny>0)?ny:1)*((nz>0)?nz:1)*((nt>0)?nt:1)*((nu>0)?nu:1)) 
{
  sa_size_t size[BASEARRAY_MAXNDIMS];
  size[0] = nx;   size[1] = ny;   size[2] = nz;
  size[3] = nt;   size[4] = nu;
  int_4 ndim = 1;
  if ((size[1] > 0) && (size[2] > 0) && (size[3] > 0) && (size[4] > 0) ) ndim = 5;
  else if ((size[1] > 0) && (size[2] > 0) && (size[3] > 0) ) ndim = 4;
  else if ((size[1] > 0) && (size[2] > 0)) ndim = 3;
  else if (size[1] > 0)  ndim = 2;
  else ndim = 1;
  string exmsg = "TArray<T>::TArray(sa_size_t, sa_size_t, sa_size_t, sa_size_t, sa_size_t)";
  if (!UpdateSizes(ndim, size, 1, 0, exmsg))  throw( ParmError(exmsg) );
}

//! Constructor
/*!
  \param ndim : number of dimensions
  \param siz[ndim] : size along each dimension
  \param db : datas are given by this NDataBlock
  \param share : if true, data are shared, if false they are copied
  \param step : step (same for all dimensions) in data block
  \param offset : offset for first element in data block
 */
template <class T>
TArray<T>::TArray(int_4 ndim, const sa_size_t * siz, NDataBlock<T> & db, bool share, sa_size_t step, sa_size_t offset)
  : BaseArray() , mNDBlock(db, share) 
{
  string exmsg = "TArray<T>::TArray(int_4, sa_size_t *,  NDataBlock<T> & ... )";
  if (!UpdateSizes(ndim, siz, step, offset, exmsg))  throw( ParmError(exmsg) );
  if (mNDBlock.Size() < (size_t)ComputeTotalSize(ndim, siz, step, offset)) {
    exmsg += " DataBlock.Size() < ComputeTotalSize(...) " ;
    throw( ParmError(exmsg) );
  }
}

//! Constructor
/*!
  \param ndim : number of dimensions
  \param siz[ndim] : size along each dimension
  \param values : datas are given by this pointer
  \param share : if true, data are shared, if false they are copied
  \param step : step (same for all dimensions) in data block
  \param offset : offset for first element in data block
  \param br : if not NULL, dats are bridge with other datas
  \sa NDataBlock
 */
template <class T>
TArray<T>::TArray(int_4 ndim, const sa_size_t * siz, T* values, sa_size_t step, sa_size_t offset, Bridge* br)
  : BaseArray() , mNDBlock(ComputeTotalSize(ndim, siz, step, 1), values, br) 
{
  string exmsg = "TArray<T>::TArray(int_4, sa_size_t *, T* ... )";
  if (!UpdateSizes(ndim, siz, step, offset, exmsg))  throw( ParmError(exmsg) );
}

//! Constructor by copy
/*!
  \warning datas are \b SHARED with \b a.
  \sa NDataBlock::NDataBlock(const NDataBlock<T>&)
 */
template <class T>
TArray<T>::TArray(const TArray<T>& a)
  : BaseArray() , mNDBlock(a.mNDBlock) 
{
  if (a.NbDimensions() == 0)  return;    // Reza-Nov 2011: we allow copy contrsuctor on non allocated arrays  
  string exmsg = "TArray<T>::TArray(const TArray<T>&)";
  if (!UpdateSizes(a, exmsg))  throw( ParmError(exmsg) );
  if (a.mInfo) mInfo = new DVList(*(a.mInfo));
}

//! Constructor by copy
/*!
  \param share : if true, data are shared, if false they are copied
 */
template <class T>
TArray<T>::TArray(const TArray<T>& a, bool share)
  : BaseArray() , mNDBlock(a.mNDBlock, share) 
{
  if (a.NbDimensions() == 0)  return;  // Reza-Nov 2011: we allow copy contrsuctor on non allocated arrays  
  string exmsg = "TArray<T>::TArray(const TArray<T>&, bool)";
  if (!UpdateSizes(a, exmsg))  throw( ParmError(exmsg) );
  if (a.mInfo) mInfo = new DVList(*(a.mInfo));
}

//! Constructor with size and contents copied (after conversion) from an array with different data type.
/*!
  The array size and memory layout are copied from the array \b a, or a packed array is created if \b pack==true.
  \param a : original array, to copy sizes and data from
  \param pack : if \b true , create a packed array, else same memory layout as \b a. 
*/
template <class T>
TArray<T>::TArray(const BaseArray& a, bool pack)
  : BaseArray() , mNDBlock() 
{
  if (a.NbDimensions() == 0)  return;  
  string exmsg = "TArray<T>::TArray(const BaseArray&,bool)";
  ReSize(a,pack,false);
  ConvertAndCopyElt(a);
  if (a.HasInfoObject()) mInfo = new DVList(*(a.getInfoPointer()));     
}

//! Destructor 
template <class T>
TArray<T>::~TArray()
{
}

////////////////////////// Les methodes de copie/share

//! Set array equal to \b a and return *this
/*!
  If the array is already allocated, CopyElt() is called
  for checking that the two arrays have the same size and
  for copying the array element values. For non allocated 
  arrays,  CloneOrShare() is called. The array memory 
  organization is also copied from \b a.
  \warning Datas are copied (cloned) from \b a.
  \sa CopyElt 
  \sa CloneOrShare
  \sa NDataBlock::operator=(const NDataBlock<T>&)
*/
template <class T>
TArray<T>& TArray<T>::Set(const TArray<T>& a)
{
  if (this == &a) return(*this);
  if (a.NbDimensions() < 1) 
    throw RangeCheckError("TArray<T>::Set(a ) - Array a not allocated ! ");
  if (NbDimensions() < 1) CloneOrShare(a);
  else CopyElt(a);
  return(*this);
}

//! Set array elements equal to the \b a array elements, after conversion 
template <class T>
TArray<T>& TArray<T>::SetBA(const BaseArray& a)
{
  if (this == &a) return(*this);
  if (a.NbDimensions() < 1) 
    throw RangeCheckError("TArray<T>::SetBA(a ) - Array a not allocated ! ");
  if (NbDimensions() < 1) {
    string exmsg = "TArray<T>::SetBA(const BaseArray& a)";
    if (!UpdateSizes(a, exmsg))  throw( ParmError(exmsg) );
    mNDBlock.ReSize(totsize_);    
  }
  ConvertAndCopyElt(a);
  return(*this);
}

//! Clone array \b a
template <class T>
void TArray<T>::Clone(const TArray<T>& a)
{
  string exmsg = "TArray<T>::Clone()";
  if (!UpdateSizes(a, exmsg))  throw( ParmError(exmsg) );
  mNDBlock.Clone(a.mNDBlock);
  if (mInfo) {delete mInfo; mInfo = NULL;}
  if (a.mInfo) mInfo = new DVList(*(a.mInfo));
}

//! Clone if \b a is not temporary, share if temporary
/*! \sa NDataBlock::CloneOrShare(const NDataBlock<T>&) */
template <class T>
void TArray<T>::CloneOrShare(const TArray<T>& a)
{
  string exmsg = "TArray<T>::CloneOrShare()";
  if (!UpdateSizes(a, exmsg))  throw( ParmError(exmsg) );
  mNDBlock.CloneOrShare(a.mNDBlock);
  if (mInfo) {delete mInfo; mInfo = NULL;}
  if (a.mInfo) mInfo = new DVList(*(a.mInfo));
}

//! Share data with a
template <class T>
void TArray<T>::Share(const TArray<T>& a)
{
  string exmsg = "TArray<T>::Share()";
  if (!UpdateSizes(a, exmsg))  throw( ParmError(exmsg) );
  mNDBlock.Share(a.mNDBlock);
  if (mInfo) {delete mInfo; mInfo = NULL;}
  if (a.mInfo) mInfo = new DVList(*(a.mInfo));
}


//! Sets or changes the array size 
/*!
  \param ndim : number of dimensions
  \param siz[ndim] : size along each dimension
  \param step : step (same for all dimensions)
  \param fzero : if \b true , set array elements to zero 
 */
template <class T>
void TArray<T>::ReSize(int_4 ndim, sa_size_t * siz, sa_size_t step, bool fzero)
{
  if (arrtype_ != 0) {
    if (ndim != 2) 
       throw( ParmError("TArray<T>::ReSize(ndim!=2,...) for Matrix" ) ); 
    if ((arrtype_ == 2) && (siz[0] > 1) && (siz[1] > 1)) 
       throw( ParmError("TArray<T>::ReSize(,siz[0]>1 && size[1]>1) for Vector" ) ); 
  }
  string exmsg = "TArray<T>::ReSize(int_4 ...)";
  if (!UpdateSizes(ndim, siz, step, 0, exmsg))  throw( ParmError(exmsg) );
  mNDBlock.ReSize(totsize_, fzero);    
}

//! Sets or changes the array size.
/*!
  The array size and memory layout are copied from the array \b a.
  \param a : Array used as template for setting the size and memory layout.
  \param pack : if \b true , create a packed array, else same memory layout as \b a. 
  \param fzero : if \b true , set array elements to zero 
 */
template <class T>
void TArray<T>::ReSize(const BaseArray& a, bool pack, bool fzero)
{
  if (arrtype_ != 0) {
    if (a.NbDimensions() != 2) 
       throw( ParmError("TArray<T>::ReSize(a.NbDimensions()!=2,...) for Matrix" ) ); 
    if ((arrtype_ == 2) && (a.Size(0) > 1) && (a.Size(1) > 1)) 
       throw( ParmError("TArray<T>::ReSize(a.Size(0)>1 && a.Size(1)>1) for Vector" ) ); 
  }
  string exmsg = "TArray<T>::ReSize(const TArray<T>&)";
  if (pack) {
    sa_size_t siz[BASEARRAY_MAXNDIMS];
    int ksz;
    for(ksz=0; ksz<a.NbDimensions(); ksz++) siz[ksz] = a.Size(ksz);
    for(ksz=a.NbDimensions(); ksz<BASEARRAY_MAXNDIMS; ksz++) siz[ksz] = 1;
    if (!UpdateSizes(a.NbDimensions(), siz, 1, 0, exmsg))  throw( ParmError(exmsg) );
    SetMemoryMapping(a.GetMemoryMapping());
    mNDBlock.ReSize(totsize_, fzero);        
  }
  else {
    if (!UpdateSizes(a, exmsg))  throw( ParmError(exmsg) );
    mNDBlock.ReSize(totsize_);      
  }
}

//! Re-allocate space for array
/*!
  \param ndim : number of dimensions
  \param siz[ndim] : size along each dimension
  \param step : step (same for all dimensions)
  \param force : if true re-allocation is forced, if not it occurs
          only if the required space is greater than the old one.
 */
template <class T>
void TArray<T>::Realloc(int_4 ndim, sa_size_t * siz, sa_size_t step, bool force)
{
  if (arrtype_ != 0) {
    if (ndim != 2) 
       throw( ParmError("TArray<T>::Realloc(ndim!=2,...) for Matrix" ) ); 
    if ((arrtype_ == 2) && (siz[0] > 1) && (siz[1] > 1)) 
       throw( ParmError("TArray<T>::Realloc(,siz[0]>1 && size[1]>1) for Vector" ) ); 
  }
  string exmsg = "TArray<T>::Realloc()";
  if (!UpdateSizes(ndim, siz, step, 0, exmsg))  throw( ParmError(exmsg) );
  mNDBlock.Realloc(totsize_, force);    
}

  //! To clear the array sizes - corresponding to an unallocated array.
template <class T>
TArray<T>&  TArray<T>::ZeroSize()
{
  if (NbDimensions() == 0)  return (*this);  
  SetZeroSize();
  mNDBlock.Dealloc();
  return (*this); 
}

/*!
  \brief Compact arrays - supresses size=1 axes.
  Changes the array rank (number of dimensions), suppressing all axes with size equal 1.
  Example: 
  Compacting Rank=NDim=5  Sizes=3x1x6x1x1  =====\>  Rank=NDim=2  Sizes=3x6 
*/
template <class T>
TArray<T>& TArray<T>::CompactAllDimensions()
{
  CompactAllDim();
  return(*this);
}

/*!
  \brief Compact array taling dimensions, for size=1 traling axes.
  Changes the array rank (number of dimensions), suppressing all axes with size equal 1,
  after the last axe with size \> 1
  Example: 
  Compacting Rank=NDim=5  Sizes=3x1x6x1x1  =====\>  Rank=NDim=3  Sizes=3x1x6 
*/
template <class T>
TArray<T>& TArray<T>::CompactTrailingDimensions()
{
  CompactTrailingDim();
  return(*this);
}

/*! 
  \brief Return the value (as a MuTyV) for element at position \b ip in the array.
  This method is used for conversion between arrays of different types. 
  \param ip : element position in the array 
 */
template <class T>
MuTyV & TArray<T>::ValueAtPosition(sa_size_t ip) const
{
#ifdef SO_BOUNDCHECKING
  if ( (ip >= totsize_) || (ip < 0) )   
    throw( ParmError("TArray<T>::ValueAtPosition(sa_size_t ip) Out-of-bound Error") );
#endif
  my_mtv = *(mNDBlock.Begin()+Offset(ip));
  return( my_mtv );
}

/*! 
  \brief Return the value (as a MuTyV) for element at position \b ip in the datablock.
  This method is used for conversion between arrays of different types. 
  \param ip : element position in the array DataBlock, regardless of 
   the array memory organisation
 */
template <class T>
MuTyV & TArray<T>::ValueAtPositionDB(sa_size_t ip) const
{
#ifdef SO_BOUNDCHECKING
  if ( (ip >= mNDBlock.Size() ) || (ip < 0) )   
    throw( ParmError("TArray<T>::ValueAtPositionDB(sa_size_t ip) Out-of-bound Error") );
#endif
  my_mtv = *(mNDBlock.Begin()+ip);
  return( my_mtv );
}

/*!
  \brief Return a new array with elements packed in memory


  \param force : if true, pack elements in a new array.
                 If false and array is already packed, return
                 an array that share data with the current one.
  \return packed array
 */
template <class T>
TArray<T> TArray<T>::PackElements(bool force) const
{
  if (NbDimensions() < 1) 
    throw RangeCheckError("TArray<T>::PackElements() - Not Allocated Array ! ");
  if ( !force && (AvgStep() == 1) ) {
    TArray<T> ra;
    ra.Share(*this);
    return(ra);
  }
  else {
    TArray<T> ra(ndim_, size_, 1);
    ra.CopyElt(*this);
    return(ra);
  }
}

// SubArrays 
// $CHECK$ Reza 03/2000  Doit-on declarer cette methode const ?
//! Extract a sub-array
/*!
  \param rx,ry,rz,rt,ru : range of extraction along dimensions
  \param compact : if \b compact == true, compact trailing dimensions (suppressed if =1)
  (See CompactTrailingDimensions() )
  \sa SOPHYA::Range
 */
template <class T>
TArray<T> TArray<T>::SubArray(Range rx, Range ry, Range rz, Range rt, Range ru, bool compact) const
{
  if (NbDimensions() < 1) 
    throw RangeCheckError("TArray<T>::operator () (Range, ...) - Not Allocated Array ! ");
  int_4 ndim = 0;

  // Updating Range objects using actual array size
  rx.Update(SizeX());
  ry.Update(SizeY());
  rz.Update(SizeZ());
  if (NbDimensions() > 3)  rt.Update(Size(3));
  else rt.Update(0);
  if (NbDimensions() > 4)  ru.Update(Size(4));
  else  ru.Update(0);

  sa_size_t size[BASEARRAY_MAXNDIMS];
  sa_size_t step[BASEARRAY_MAXNDIMS];
  sa_size_t pos[BASEARRAY_MAXNDIMS];
  size[0] = rx.Size();
  size[1] = ry.Size();
  size[2] = rz.Size();
  size[3] = rt.Size();
  size[4] = ru.Size();

  step[0] = rx.Step();
  step[1] = ry.Step();
  step[2] = rz.Step();
  step[3] = rt.Step();
  step[4] = ru.Step();

  pos[0] = rx.Start();
  pos[1] = ry.Start();
  pos[2] = rz.Start();
  pos[3] = rt.Start();
  pos[4] = ru.Start();

  ndim = ndim_;
  TArray<T> ra;
  UpdateSubArraySizes(ra, ndim, size, pos, step); 
  ra.DataBlock().Share(this->DataBlock());
  if (compact) ra.CompactTrailingDim();
  return(ra);
}

//   ...... Operation de calcul sur les tableaux ......
//              ------- Attention --------
//  Boucles normales prenant en compte les steps ....
//  Possibilite de // , vectorisation

//! Fill TArray with Sequence \b seq
/*!
  \param seq : sequence to fill the array
  \sa Sequence
 */
template <class T>
TArray<T>& TArray<T>::SetSeq(Sequence const & seq)
{
  if (NbDimensions() < 1) 
    throw RangeCheckError("TArray<T>::SetSeq(Sequence ) - Not Allocated Array ! ");
  
  T * pe;
  sa_size_t j,k;
  int_4 ka;
  if (arrtype_ == 0)  ka = 0;
  else ka = macoli_;
  sa_size_t step = Step(ka);
  sa_size_t gpas = Size(ka);
  sa_size_t naxa = Size()/Size(ka);
  for(j=0; j<naxa; j++)  {
    pe = mNDBlock.Begin()+Offset(ka,j);
/*  
    Appel explicite de l'operateur de conversion 
    suite a la suggestion de M. Reinecke, Reza 31/7/2002 
#if !defined(__GNUG__)
    for(k=0; k<gpas; k++)  pe[k*step] = (T) seq(j*gpas+k);
#else
    // g++ (up to 2.95.1) se melange les pinceaux  s'il y a le cast (T) pour l'instanciation des complexes 
    for(k=0; k<gpas; k++)  pe[k*step] = seq(j*gpas+k);
#endif
   --- Appel explicite de l'operateur de conversion sur l'objet MuTyV 
*/
    for(k=0; k<gpas; k++)  seq(j*gpas+k).Convert(pe[k*step]);
//REMPLACE suite pb compil gcc4    for(k=0; k<gpas; k++)  pe[k*step] = seq(j*gpas+k).operator T();
  }
  return(*this);
}

//  >>>> Operations avec 2nd membre de type scalaire 

//! Fill an array with a constant value \b x
template <class T>
TArray<T>& TArray<T>::SetCst(T x)
{
  if (NbDimensions() < 1) 
    throw RangeCheckError("TArray<T>::SetCst(T )  - Not Allocated Array ! ");
  T * pe;
  sa_size_t j,k;
  if (AvgStep() > 0)   {  // regularly spaced elements
    sa_size_t step = AvgStep();
    sa_size_t maxx = totsize_*step;
    pe = Data();
    for(k=0; k<maxx; k+=step )  pe[k] = x;
  }
  else {    // Non regular data spacing ...
    int_4 ka = MaxSizeKA();
    sa_size_t step = Step(ka);
    sa_size_t gpas = Size(ka)*step;
    sa_size_t naxa = Size()/Size(ka);
    for(j=0; j<naxa; j++)  {
      pe = mNDBlock.Begin()+Offset(ka,j);
      for(k=0; k<gpas; k+=step)  pe[k] = x;
    }
  }
  return(*this);
}

//! Add a constant value \b x to the source array and store the result in \b res. 
/*! 
Add a constant to the source array \b this and store the result in \b res (res = *this+x).  

If not initially allocated, and if the source array (this) is not flagged as 
temporary, the output array \b res is automatically 
resized as a packed array with the same sizes as the source (this) array.
If \b res is not allocated and (this) is temporary, data is shared between \b res and this.

Returns a reference to the output array \b res. 

\param x : constant to add to the array elements
\param res : Output array containing the result (res=this+x). 
*/
template <class T>
TArray<T>& TArray<T>::AddCst(T x, TArray<T>& res) const
{
  if (NbDimensions() < 1) 
    throw RangeCheckError("TArray<T>::AddCst(T,res)  - Not allocated source array ");
  if (res.NbDimensions() < 1)  {
    if ( IsTemp() )  res.Share(*this);
    else res.SetSize(*this, true, false);
  }
  bool smo;
  if (!CompareSizes(res, smo)) 
    throw(SzMismatchError("TArray<T>::AddCst(T, res) SizeMismatch(this,res) ")) ;

  const T * pe;
  T * per;
  sa_size_t j,k;
  if (smo && (IsPacked() > 0) && (res.IsPacked() > 0))   {  // regularly spaced elements
    sa_size_t maxx = totsize_;
    pe = Data();
    per = res.Data();
    for(k=0; k<maxx; k++)  *per++ = *pe++ + x;  
  }
  else {    // Non regular data spacing ...
    int_4 ax,axr;
    sa_size_t step, stepr;
    sa_size_t gpas, naxa;
    GetOpeParams(res, smo, ax, axr, step, stepr, gpas, naxa);
    for(j=0; j<naxa; j++)  {
      pe = mNDBlock.Begin()+Offset(ax,j);
      per = res.DataBlock().Begin()+res.Offset(axr,j);
      for(k=0;  k<gpas;  k+=step, pe+=step, per+=stepr)  *per = *pe+x; 
    }
  }
  return(res);
}

//! Subtract a constant value \b x from the source array and store the result in \b res. 
/*! 
Subtract a constant from the source array \b this and store the result in \b res (res = *this-x).  

If not initially allocated, and if the source array (this) is not flagged as 
temporary, the output array \b res is automatically 
resized as a packed array with the same sizes as the source (this) array.
If \b res is not allocated and (this) is temporary, data is shared between \b res and this.

Returns a reference to the output array \b res. 

\param x : constant to subtract from the array elements
\param res : Output array containing the result (res=this+x or res=x-this). 
\param fginv == true : Invert subtraction argument order (res = x-(*this)) 
*/
template <class T>
TArray<T>& TArray<T>::SubCst(T x, TArray<T>& res, bool fginv) const
{
  if (NbDimensions() < 1) 
    throw RangeCheckError("TArray<T>::SubCst(T,res)  - Not allocated source array ");
  if (res.NbDimensions() < 1)  {
    if ( IsTemp() )  res.Share(*this);
    else res.SetSize(*this, true, false);
  }
  bool smo;
  if (!CompareSizes(res, smo)) 
    throw(SzMismatchError("TArray<T>::SubCst(T, res) SizeMismatch(this,res) ")) ;

  const T * pe;
  T * per;
  sa_size_t j,k;
  if (smo && (IsPacked() > 0) && (res.IsPacked() > 0))   {  // regularly spaced elements
    sa_size_t maxx = totsize_;
    pe = Data();
    per = res.Data();
    if (!fginv) 
      for(k=0; k<maxx; k++)  *per++ = *pe++ - x;  
    else 
      for(k=0; k<maxx; k++)  *per++ = x - *pe++; 
  }
  else {    // Non regular data spacing ...
    int_4 ax,axr;
    sa_size_t step, stepr;
    sa_size_t gpas, naxa;
    GetOpeParams(res, smo, ax, axr, step, stepr, gpas, naxa);
    for(j=0; j<naxa; j++)  {
      pe = mNDBlock.Begin()+Offset(ax,j);
      per = res.DataBlock().Begin()+res.Offset(axr,j);
      if (!fginv) 
        for(k=0;  k<gpas;  k+=step, pe+=step, per+=stepr)  *per = *pe-x; 
       else
        for(k=0;  k<gpas;  k+=step, pe+=step, per+=stepr)  *per = x-*pe; 
    }
  }
  return(res);
}

//! Multiply the source array by a constant value \b x and store the result in \b res. 
/*! 
Multiply the source array \b this by a constant \b x and store the result in \b res (res = *this*x).  

If not initially allocated, and if the source array (this) is not flagged as 
temporary, the output array \b res is automatically 
resized as a packed array with the same sizes as the source (this) array.
If \b res is not allocated and (this) is temporary, data is shared between \b res and this.

Returns a reference to the output array \b res. 

\param x : Array elements are multiplied by x
\param res : Output array containing the result (res=this*x). 
*/
template <class T>
TArray<T>& TArray<T>::MulCst(T x, TArray<T>& res) const
{
  if (NbDimensions() < 1) 
    throw RangeCheckError("TArray<T>::MulCst(T,res)  - Not allocated source array ");
  if (res.NbDimensions() < 1)  {
    if ( IsTemp() )  res.Share(*this);
    else res.SetSize(*this, true, false);
  }
  bool smo;
  if (!CompareSizes(res, smo)) 
    throw(SzMismatchError("TArray<T>::MulCst(T, res) SizeMismatch(this,res) ")) ;

  const T * pe;
  T * per;
  sa_size_t j,k;
  if (smo && (IsPacked() > 0) && (res.IsPacked() > 0))   {  // regularly spaced elements
    sa_size_t maxx = totsize_;
    pe = Data();
    per = res.Data();
    for(k=0; k<maxx; k++)  *per++ = *pe++ * x;  
  }
  else {    // Non regular data spacing ...
    int_4 ax,axr;
    sa_size_t step, stepr;
    sa_size_t gpas, naxa;
    GetOpeParams(res, smo, ax, axr, step, stepr, gpas, naxa);
    for(j=0; j<naxa; j++)  {
      pe = mNDBlock.Begin()+Offset(ax,j);
      per = res.DataBlock().Begin()+res.Offset(axr,j);
      for(k=0;  k<gpas;  k+=step, pe+=step, per+=stepr)  *per = (*pe)*x; 
    }
  }
  return(res);
}

//! Divide the source array by a constant value \b x and store the result in \b res.
/*! 
Divide the source array \b this by a constant \b x and store the result in \b res (res = *this/x).  

If not initially allocated, and if the source array (this) is not flagged as 
temporary, the output array \b res is automatically 
resized as a packed array with the same sizes as the source (this) array.
If \b res is not allocated and (this) is temporary, data is shared between \b res and this.

Returns a reference to the output array \b res. 

\param x : Array elements are divied by x
\param res : Output array containing the result (res=(*this)/x or res=x/(*this)). 
\param fginv == true : Invert the division argument order (res = x/(*this)) 
*/
template <class T>
TArray<T>& TArray<T>::DivCst(T x, TArray<T>& res, bool fginv) const
{
  if (NbDimensions() < 1) 
    throw RangeCheckError("TArray<T>::DivCst(T,res)  - Not allocated source array ! ");
  if (!fginv && (x == (T) 0) ) 
    throw MathExc("TArray<T>::DivCst(T,res)  - Divide by zero ! ");
  if (res.NbDimensions() < 1)  {
    if ( IsTemp() )  res.Share(*this);
    else res.SetSize(*this, true, false);
  }
  bool smo;
  if (!CompareSizes(res, smo)) 
    throw(SzMismatchError("TArray<T>::DivCst(T, res) SizeMismatch(this,res) ")) ;

  const T * pe;
  T * per;
  sa_size_t j,k;
  if (smo && (IsPacked() > 0) && (res.IsPacked() > 0))   {  // regularly spaced elements
    sa_size_t maxx = totsize_;
    pe = Data();
    per = res.Data();
    if (!fginv) 
      for(k=0; k<maxx; k++)  *per++ = *pe++ / x;  
    else 
      for(k=0; k<maxx; k++) *per++ = x / *pe++; 
  }
  else {    // Non regular data spacing ...
    int_4 ax,axr;
    sa_size_t step, stepr;
    sa_size_t gpas, naxa;
    GetOpeParams(res, smo, ax, axr, step, stepr, gpas, naxa);
    for(j=0; j<naxa; j++)  {
      pe = mNDBlock.Begin()+Offset(ax,j);
      per = res.DataBlock().Begin()+res.Offset(axr,j);
      if (!fginv) 
        for(k=0;  k<gpas;  k+=step, pe+=step, per+=stepr)  *per = (*pe)/x; 
       else
        for(k=0;  k<gpas;  k+=step, pe+=step, per+=stepr)  *per = x/(*pe); 
    }
  }
  return(res);
}


//! Stores the opposite of the source array in \b res (res=-(*this)). 
/*! 
If not initially allocated, and if the source array (this) is not flagged as 
temporary, the output array \b res is automatically 
resized as a packed array with the same sizes as the source (this) array.
If \b res is not allocated and (this) is temporary, data is shared between \b res and this.

Returns a reference to the output array \b res. 
*/
template <class T>
TArray<T>& TArray<T>::NegateElt(TArray<T>& res) const
{
  if (NbDimensions() < 1) 
    throw RangeCheckError("TArray<T>::NegateElt(res)  - Not allocated source array ");
  if (res.NbDimensions() < 1)  {
    if ( IsTemp() )  res.Share(*this);
    else res.SetSize(*this, true, false);
  }
  bool smo;
  if (!CompareSizes(res, smo)) 
    throw(SzMismatchError("TArray<T>::NegateElt(res) SizeMismatch(this,res) ")) ;

  const T * pe;
  T * per;
  sa_size_t j,k;
  if (smo && (IsPacked() > 0) && (res.IsPacked() > 0))   {  // regularly spaced elements
    sa_size_t maxx = totsize_;
    pe = Data();
    per = res.Data();
    for(k=0; k<maxx; k++)  *per++ = -(*pe++);  
  }
  else {    // Non regular data spacing ...
    int_4 ax,axr;
    sa_size_t step, stepr;
    sa_size_t gpas, naxa;
    GetOpeParams(res, smo, ax, axr, step, stepr, gpas, naxa);
    for(j=0; j<naxa; j++)  {
      pe = mNDBlock.Begin()+Offset(ax,j);
      per = res.DataBlock().Begin()+res.Offset(axr,j);
      for(k=0;  k<gpas;  k+=step, pe+=step, per+=stepr)  *per = -(*pe); 
    }
  }
  return(res);
}

//  >>>> Operations avec 2nd membre de type tableau

//! Two TArrays element by element addition
/*! 
Perform element by element addition of the source array (this) and the \b a array 
and store the result in \b res (res = *this+a). The source and argument arrays (this, a)
should have the same sizes.

If not initially allocated, and if none of the source arrays is flagged as 
temporary, the output array \b res is automatically 
resized as a packed array with the same sizes as the source (this) array.
If \b res is not allocated and one of the source array is temporary,
data is shared between \b res and the temporary source array.

Returns a reference to the output array \b res. 

\param a : Array to be added to the source array.
\param res : Output array containing the result (res=this+a). 
*/
template <class T>
TArray<T>& TArray<T>::AddElt(const TArray<T>& a, TArray<T>& res) const 
{
  if (NbDimensions() < 1) 
    throw RangeCheckError("TArray<T>::AddElt(...) - Not allocated source array ! ");
  bool smoa;
  if (!CompareSizes(a, smoa)) 
    throw(SzMismatchError("TArray<T>::AddElt(...) SizeMismatch(this,a)")) ;
  if (res.NbDimensions() < 1)  {
    if ( IsTemp() )  res.Share(*this);
    else if ( a.IsTemp() ) res.Share(a);
    else res.SetSize(*this, true, false);
  }
  bool smor;
  if (!CompareSizes(res, smor)) 
    throw(SzMismatchError("TArray<T>::AddElt(...) SizeMismatch(this,res) ")) ;

  bool smora;
  a.CompareSizes(res, smora);

  bool smo = smoa && smor;  // The three arrays have same memory organisation

  const T * pe;
  const T * pea;
  T * per;
  sa_size_t j,k;
  if (smo && IsPacked() && a.IsPacked() && res.IsPacked() )   {  // All packed arrays
    sa_size_t maxx = totsize_;
    pe = Data();
    pea = a.Data();
    per = res.Data();
    //    for(k=0; k<maxx;  k++, pe++, pea++, per++)  *per = *pe + *pea ;
    for(k=0; k<maxx;  k++)  *per++ = *pe++ + *pea++ ;
  }
  else {    // Non regular data spacing ...
    int_4 ax,axa,axr;
    sa_size_t step, stepa;
    sa_size_t gpas, naxa;
    sa_size_t stepr, stgpas;
    if ( !smo && smora ) {   // same mem-org for a,res , different from this
      a.GetOpeParams(*this, smo, axa, ax, stepa, step, gpas, naxa);
      a.GetOpeParams(res, smo, axa, axr, stepa, stepr, gpas, naxa);
      stgpas = stepa;
    }
    else { // same mem-org for all, or same (this,a) OR same(this,res)
      GetOpeParams(a, smo, ax, axa, step, stepa, gpas, naxa);
      GetOpeParams(res, smo, ax, axr, step, stepr, gpas, naxa);
      stgpas = step;
    }
    for(j=0; j<naxa; j++)  {
      pe = mNDBlock.Begin()+Offset(ax,j);
      pea = a.DataBlock().Begin()+a.Offset(axa,j);
      per = res.DataBlock().Begin()+res.Offset(axr,j);
      for(k=0; k<gpas; k+=stgpas, pe+=step, pea+=stepa, per+=stepr) *per = *pe + *pea ; 
    }
  }
  return(res);
}

//! Two TArrays element by element subtraction
/*! 
Perform element by element subtraction of the source array (this) and the \b a array 
and the store result in \b res (res = *this-a or res=a-(*this)). 
The source and argument arrays (this, a) should have the same sizes.

If not initially allocated, and if none of the source arrays is flagged as 
temporary, the output array \b res is automatically 
resized as a packed array with the same sizes as the source (this) array.
If \b res is not allocated and one of the source array is temporary,
data is shared between \b res and the temporary source array.

Returns a reference to the output array \b res. 

\param a : Array to be added to the source array.
\param res : Output array containing the result (res=*this+x). 
\param fginv == true : Invert subtraction argument order (res = a-(*this))  
*/

template <class T>
TArray<T>& TArray<T>::SubElt(const TArray<T>& a, TArray<T>& res, bool fginv) const
{
  if (NbDimensions() < 1) 
    throw RangeCheckError("TArray<T>::SubElt(...) - Not allocated source array ! ");
  bool smoa;
  if (!CompareSizes(a, smoa)) 
    throw(SzMismatchError("TArray<T>::SubElt(...) SizeMismatch(this,a)")) ;
  if (res.NbDimensions() < 1)  {
    if ( IsTemp() )  res.Share(*this);
    else if ( a.IsTemp() ) res.Share(a);
    else res.SetSize(*this, true, false);
  }
  bool smor;
  if (!CompareSizes(res, smor)) 
    throw(SzMismatchError("TArray<T>::SubElt(...) SizeMismatch(this,res) ")) ;

  bool smora;
  a.CompareSizes(res, smora);

  bool smo = smoa && smor;  // The three arrays have same memory organisation

  const T * pe;
  const T * pea;
  T * per;
  sa_size_t j,k;
  if (smo && IsPacked() && a.IsPacked() && res.IsPacked() )   {  // All packed arrays
    sa_size_t maxx = totsize_;
    pe = Data();
    pea = a.Data();
    per = res.Data();
    if (!fginv) 
      for(k=0; k<maxx;  k++)  *per++ = *pe++ - *pea++ ;
    else 
      for(k=0; k<maxx;  k++)  *per++ = *pea++ - *pe++ ;
  }
  else {    // Non regular data spacing ...
    int_4 ax,axa,axr;
    sa_size_t step, stepa;
    sa_size_t gpas, naxa;
    sa_size_t stepr, stgpas;
    if ( !smo && smora ) {   // same mem-org for a,res , different from this
      a.GetOpeParams(*this, smo, axa, ax, stepa, step, gpas, naxa);
      a.GetOpeParams(res, smo, axa, axr, stepa, stepr, gpas, naxa);
      stgpas = stepa;
    }
    else { // same mem-org for all, or same (this,a) OR same(this,res)
      GetOpeParams(a, smo, ax, axa, step, stepa, gpas, naxa);
      GetOpeParams(res, smo, ax, axr, step, stepr, gpas, naxa);
      stgpas = step;
    }
    for(j=0; j<naxa; j++)  {
      pe = mNDBlock.Begin()+Offset(ax,j);
      pea = a.DataBlock().Begin()+a.Offset(axa,j);
      per = res.DataBlock().Begin()+res.Offset(axr,j);
      if (!fginv) 
        for(k=0; k<gpas; k+=stgpas, pe+=step, pea+=stepa, per+=stepr) *per = *pe - *pea ; 
      else 
        for(k=0; k<gpas; k+=stgpas, pe+=step, pea+=stepa, per+=stepr) *per = *pea - *pea ; 
    }
  }
  return(res);
}


//! Two TArrays element by element multiplication
/*! 
Perform element by element multiplication of the source array (this) and the \b a array 
and store the result in \b res (res = *this*a). The source and argument arrays (this, a)
should have the same sizes.

If not initially allocated, and if none of the source arrays is flagged as 
temporary, the output array \b res is automatically 
resized as a packed array with the same sizes as the source (this) array.
If \b res is not allocated and one of the source array is temporary,
data is shared between \b res and the temporary source array.

Returns a reference to the output array \b res. 

\param a : Array to be added to the source array.
\param res : Output array containing the result (res=(*this)*a). 
*/
template <class T>
TArray<T>& TArray<T>::MulElt(const TArray<T>& a, TArray<T>& res) const 
{
  if (NbDimensions() < 1) 
    throw RangeCheckError("TArray<T>::MulElt(...) - Not allocated source array ! ");
  bool smoa;
  if (!CompareSizes(a, smoa)) 
    throw(SzMismatchError("TArray<T>::MulElt(...) SizeMismatch(this,a)")) ;
  if (res.NbDimensions() < 1)  {
    if ( IsTemp() )  res.Share(*this);
    else if ( a.IsTemp() ) res.Share(a);
    else res.SetSize(*this, true, false);
  }
  bool smor;
  if (!CompareSizes(res, smor)) 
    throw(SzMismatchError("TArray<T>::MulElt(...) SizeMismatch(this,res) ")) ;

  bool smora;
  a.CompareSizes(res, smora);

  bool smo = smoa && smor;  // The three arrays have same memory organisation

  const T * pe;
  const T * pea;
  T * per;
  sa_size_t j,k;
  if (smo && IsPacked() && a.IsPacked() && res.IsPacked() )   {  // All packed arrays
    sa_size_t maxx = totsize_;
    pe = Data();
    pea = a.Data();
    per = res.Data();
    for(k=0; k<maxx;  k++)  *per++ = *pe++ * *pea++ ;
  }
  else {    // Non regular data spacing ...
    int_4 ax,axa,axr;
    sa_size_t step, stepa;
    sa_size_t gpas, naxa;
    sa_size_t stepr, stgpas;
    if ( !smo && smora ) {   // same mem-org for a,res , different from this
      a.GetOpeParams(*this, smo, axa, ax, stepa, step, gpas, naxa);
      a.GetOpeParams(res, smo, axa, axr, stepa, stepr, gpas, naxa);
      stgpas = stepa;
    }
    else { // same mem-org for all, or same (this,a) OR same(this,res)
      GetOpeParams(a, smo, ax, axa, step, stepa, gpas, naxa);
      GetOpeParams(res, smo, ax, axr, step, stepr, gpas, naxa);
      stgpas = step;
    }
    for(j=0; j<naxa; j++)  {
      pe = mNDBlock.Begin()+Offset(ax,j);
      pea = a.DataBlock().Begin()+a.Offset(axa,j);
      per = res.DataBlock().Begin()+res.Offset(axr,j);
      for(k=0; k<gpas; k+=stgpas, pe+=step, pea+=stepa, per+=stepr) *per = (*pe) * (*pea); 
    }
  }
  return(res);
}


//! Two TArrays element by element division
/*! 
Perform element by element division of the source array (this) and the \b a array 
and store the result in \b res (res = *this/a). The source and argument arrays (this, a)
should have the same sizes.

If not initially allocated, and if none of the source arrays is flagged as 
temporary, the output array \b res is automatically 
resized as a packed array with the same sizes as the source (this) array.
If \b res is not allocated and one of the source array is temporary,
data is shared between \b res and the temporary source array.

Returns a reference to the output array \b res. 

\param a : Array to be added to the source array.
\param res : Output array containing the result (res=*this/a). 
\param fginv == true : Inverts the division argument order (res = a/(*this)) 
\param divzero == true : Result is set to zero (res(i)=0) if the operation's 
second argument is equal to zero (a(i)/(*this)(i)==0) 
*/
template <class T>
TArray<T>& TArray<T>::DivElt(const TArray<T>& a, TArray<T>& res, bool fginv, bool divzero) const 
{
  if (NbDimensions() < 1) 
    throw RangeCheckError("TArray<T>::DivElt(...) - Not allocated source array ! ");
  bool smoa;
  if (!CompareSizes(a, smoa)) 
    throw(SzMismatchError("TArray<T>::DivElt(...) SizeMismatch(this,a)")) ;
  if (res.NbDimensions() < 1)  {
    if ( IsTemp() )  res.Share(*this);
    else if ( a.IsTemp() ) res.Share(a);
    else res.SetSize(*this, true, false);
  }
  bool smor;
  if (!CompareSizes(res, smor)) 
    throw(SzMismatchError("TArray<T>::DivElt(...) SizeMismatch(this,res) ")) ;

  bool smora;
  a.CompareSizes(res, smora);

  bool smo = smoa && smor;  // The three arrays have same memory organisation

  const T * pe;
  const T * pea;
  T * per;
  sa_size_t j,k;
  if (smo && IsPacked() && a.IsPacked() && res.IsPacked() )   {  // All packed arrays
    sa_size_t maxx = totsize_;
    pe = Data();
    pea = a.Data();
    per = res.Data();
    if(divzero) {
      if (!fginv) 
        for(k=0; k<maxx;  k++)  
          if (*pea==(T)0) *per = (T)0;  else   *per++ = *pe++ / *pea++ ;
      else 
        for(k=0; k<maxx;  k++)  
          if (*pe==(T)0)  *per = (T)0;  else   *per++ = *pea++ / *pe++ ;
    }
    else {
      if (!fginv) 
        for(k=0; k<maxx;  k++)  *per++ = *pe++ / *pea++ ;
      else
        for(k=0; k<maxx;  k++)  *per = *pea++ / *pe++ ;
    }
  }
  else {    // Non regular data spacing ...
    int_4 ax,axa,axr;
    sa_size_t step, stepa;
    sa_size_t gpas, naxa;
    sa_size_t stepr, stgpas;
    if ( !smo && smora ) {   // same mem-org for a,res , different from this
      a.GetOpeParams(*this, smo, axa, ax, stepa, step, gpas, naxa);
      a.GetOpeParams(res, smo, axa, axr, stepa, stepr, gpas, naxa);
      stgpas = stepa;
      }
    else { // same mem-org for all, or same (this,a) OR same(this,res)
      GetOpeParams(a, smo, ax, axa, step, stepa, gpas, naxa);
      GetOpeParams(res, smo, ax, axr, step, stepr, gpas, naxa);
      stgpas = step;
     }
    // DBG cout << "DBG-A-DIVELT naxa=" << naxa << " gpas= " << gpas 
    //	 << " step=" << step << " stepa=" << stepa << " stepr=" << stepr 
    //	 << " ax= " << ax << " axa= " << axa << " axr= " << axr << endl;
    for(j=0; j<naxa; j++)  {
      pe = mNDBlock.Begin()+Offset(ax,j);
      pea = a.DataBlock().Begin()+a.Offset(axa,j);
      per = res.DataBlock().Begin()+res.Offset(axr,j);
      if(divzero) {
        if (!fginv) 
          for(k=0; k<gpas; k+=stgpas, pe+=step, pea+=stepa, per+=stepr)  
            if (*pea==(T)0) *per = (T)0;  else   *per = *pe / *pea ;
	else
          for(k=0; k<gpas; k+=stgpas, pe+=step, pea+=stepa, per+=stepr)  
            if (*pe==(T)0)  *per = (T)0;  else   *per = *pea / *pe ;
      }
      else {
        if (!fginv) 
	  for(k=0; k<gpas; k+=stgpas, pe+=step, pea+=stepa, per+=stepr)  
	    *per = *pe / *pea ;
	else
          for(k=0; k<gpas; k+=stgpas, pe+=step, pea+=stepa, per+=stepr)  
	    *per = *pea / *pe ;
      }
    }
  }
  return(res);
}


//! Copy elements of \b a
template <class T>
TArray<T>& TArray<T>::CopyElt(const TArray<T>& a)
{
  if (NbDimensions() < 1) 
    throw RangeCheckError("TArray<T>::CopyElt(const TArray<T>& )  - Not Allocated Array ! ");
  bool smo;
  if (!CompareSizes(a, smo)) 
    throw(SzMismatchError("TArray<T>::CopyElt(const TArray<T>&) SizeMismatch")) ;

  T * pe;
  const T * pea;
  sa_size_t j,k;
  if (smo && (AvgStep() > 0) && (a.AvgStep() > 0) )   {  // regularly spaced elements
    if (IsPacked() && a.IsPacked())  memcpy(Data(), a.Data(), totsize_*sizeof(T)); // Packed arrays
    else {
      sa_size_t step = AvgStep();
      sa_size_t stepa = a.AvgStep();
      sa_size_t maxx = totsize_*step;
      pe = Data();
      pea = a.Data();
      for(k=0;  k<maxx;  k+=step, pe+=step, pea+=stepa )  *pe = *pea ;
    }
  }
  else {    // Non regular data spacing ...
    int_4 ax,axa;
    sa_size_t step, stepa;
    sa_size_t gpas, naxa;
    GetOpeParams(a, smo, ax, axa, step, stepa, gpas, naxa);
    for(j=0; j<naxa; j++)  {
      pe = mNDBlock.Begin()+Offset(ax,j);
      pea = a.DataBlock().Begin()+a.Offset(axa,j);
      for(k=0;  k<gpas;  k+=step, pe+=step, pea+=stepa)  *pe = *pea; 
    }
  }
  return(*this);
}

//! Converts and Copy elements of \b a
template <class T>
TArray<T>& TArray<T>::ConvertAndCopyElt(const BaseArray& a)
{
  if (NbDimensions() < 1) 
    throw RangeCheckError("TArray<T>::ConvertAndCopyElt(const TArray<T>& )  - Not Allocated Array ! ");
  bool smo;
  if (!CompareSizes(a, smo)) 
    throw(SzMismatchError("TArray<T>::ConvertAndCopyElt(const TArray<T>&) SizeMismatch")) ;

  T * pe;
  sa_size_t j,k,ka;
  sa_size_t offa;
  // Non regular data spacing ...
  int_4 ax,axa;
  sa_size_t step, stepa;
  sa_size_t gpas, naxa;
  GetOpeParams(a, smo, ax, axa, step, stepa, gpas, naxa);
  for(j=0; j<naxa; j++)  {
    pe = mNDBlock.Begin()+Offset(ax,j);
    offa = a.Offset(axa,j);
/*  
    Appel explicite de l'operateur de conversion 
    suite a la suggestion de M. Reinecke, Reza 31/7/2002 
#if !defined(__GNUG__)
    for(k=0, ka=0;  k<gpas;  k+=step, ka+=stepa)  pe[k] = (T)a.ValueAtPosition(offa+ka); 
#else
    // g++ (up to 2.95.1) se melange les pinceaux  s'il y a le cast (T) pour l'instanciation des complexes 
    for(k=0, ka=0;  k<gpas;  k+=step, ka+=stepa)  pe[k] = a.ValueAtPosition(offa+ka); 
#endif
   --- Appel explicite de l'operateur de conversion sur l'objet MuTyV 
*/
    /*  ----- Janvier 2006 ------
      Un bug important etait semble-t-il present depuis longtemps
      On appelait a.ValueAtPosition(ip) qui renvoie l'element ip en tenant compte
      de la structure du tableau , alors qu'on veut acceder l'element ip du datablock
      Methode ValueAtPositionDB(ip) ajoute et utilisee a la place de  ValueAtPosition(ip)
     */
    for(k=0, ka=0;  k<gpas;  k+=step, ka+=stepa) 
      a.ValueAtPositionDB(offa+ka).Convert(pe[k]); 
      //REMPLACE Suite pb compil gcc4   pe[k] = a.ValueAtPosition(offa+ka).operator T(); 
  }
  return(*this);
}

//! Return the the scalar product of the two arrays (Sum_k[(*this)(k)*a(k)])
template <class T>
T TArray<T>::ScalarProduct(const TArray<T>& a)  const
{
  if (NbDimensions() < 1) 
    throw RangeCheckError("TArray<T>::ScalarProduct(...)  - Not allocated source array ");
  bool smo;
  if (!CompareSizes(a, smo)) 
    throw(SzMismatchError("TArray<T>::ScalarProduct(...) SizeMismatch(this,a) ")) ;

  T res = (T)(0);
  const T * pe;
  const T * pea;
  sa_size_t j,k;
  if (smo && (IsPacked() > 0) && (a.IsPacked() > 0))   {  // regularly spaced elements
    sa_size_t maxx = totsize_;
    pe = Data();
    pea = a.Data();
    for(k=0; k<maxx; k++)  res += *pe++ * *pea++;
  }
  else {    // Non regular data spacing ...
    int_4 ax,axa;
    sa_size_t step, stepa;
    sa_size_t gpas, naxa;
    GetOpeParams(a, smo, ax, axa, step, stepa, gpas, naxa);
    for(j=0; j<naxa; j++)  {
      pe = mNDBlock.Begin()+Offset(ax,j);
      pea = a.DataBlock().Begin()+a.Offset(axa,j);
      for(k=0;  k<gpas; k+=step, pe+=step, pea+=stepa)  res += (*pe)*(*pea);
    }
  }
  return(res);
}


// Somme et produit des elements
//! Returns the sum of all array elements 
template <class T>
T TArray<T>::Sum() const
{
  if (NbDimensions() < 1) 
    throw RangeCheckError("TArray<T>::Sum()  - Not Allocated Array ! ");
  T ret=0;
  const T * pe;
  sa_size_t j,k;
  if (AvgStep() > 0)   {  // regularly spaced elements
    sa_size_t step = AvgStep();
    sa_size_t maxx = totsize_*step;
    pe = Data();
    for(k=0; k<maxx; k+=step )  ret += pe[k];
  }
  else {    // Non regular data spacing ...
    int_4 ka = MaxSizeKA();
    sa_size_t step = Step(ka);
    sa_size_t gpas = Size(ka)*step;
    sa_size_t naxa = Size()/Size(ka);
    for(j=0; j<naxa; j++)  {
      pe = mNDBlock.Begin()+Offset(ka,j);
      for(k=0; k<gpas; k+=step)  ret += pe[k] ;
    }
  }
  return ret;
}

//! Return the product of all elements
template <class T>
T TArray<T>::Product() const
{
  if (NbDimensions() < 1) 
    throw RangeCheckError("TArray<T>::Product()  - Not Allocated Array ! ");
  T ret=(T)1;
  const T * pe;
  sa_size_t j,k;
  if (AvgStep() > 0)   {  // regularly spaced elements
    sa_size_t step = AvgStep();
    sa_size_t maxx = totsize_*step;
    pe = Data();
    for(k=0; k<maxx; k+=step )  ret *= pe[k];
  }
  else {    // Non regular data spacing ...
    int_4 ka = MaxSizeKA();
    sa_size_t step = Step(ka);
    sa_size_t gpas = Size(ka)*step;
    sa_size_t naxa = Size()/Size(ka);
    for(j=0; j<naxa; j++)  {
      pe = mNDBlock.Begin()+Offset(ka,j);
      for(k=0; k<gpas; k+=step)  ret *= pe[k] ;
    }
  }
  return ret;
}

//! Returns the sum of all array elements squared (Sum_k((*this)(k)*(*this)(k)).
template <class T>
T TArray<T>::SumSq() const
{
  if (NbDimensions() < 1) 
    throw RangeCheckError("TArray<T>::SumSq()  - Not Allocated Array ! ");
  T ret=0;
  const T * pe;
  sa_size_t j,k;
  if (AvgStep() > 0)   {  // regularly spaced elements
    sa_size_t step = AvgStep();
    sa_size_t maxx = totsize_*step;
    pe = Data();
    for(k=0; k<maxx; k+=step )  ret += pe[k]*pe[k];
  }
  else {    // Non regular data spacing ...
    int_4 ka = MaxSizeKA();
    sa_size_t step = Step(ka);
    sa_size_t gpas = Size(ka)*step;
    sa_size_t naxa = Size()/Size(ka);
    for(j=0; j<naxa; j++)  {
      pe = mNDBlock.Begin()+Offset(ka,j);
      for(k=0; k<gpas; k+=step)  ret += pe[k]*pe[k] ;
    }
  }
  return ret;
}

/*!
\brief Returns the array norm squared, defined as  Sum_k [ el(k)* el(k) ] 
  For arrays with integer or real data, this method calls SumSq(), which computes
  the sum of array elements squared. For complex arrays, it computes and returns 
  the sum of array elements module squared (= Sum_k [el(k)*conj(el(k))]
*/
template <class T>
T TArray<T>::Norm2() const
{
  return SumSq();
}


// Fonction auxiliaire pour specialisation de la methode Norm2() pour tableaux complexes
template <class T> 
complex<T>  _ComputeComplexNorm_Private_(TArray< complex<T> > const & ca) 
{
  if (ca.NbDimensions() < 1) 
    throw RangeCheckError("TArray< complex<T> >::Norm2()  - Not Allocated Array ! ");
  complex<T> ret= complex<T>(0., 0.);
  const complex<T> * pe;
  sa_size_t j,k;
  if (ca.AvgStep() > 0)   {  // regularly spaced elements
    sa_size_t step = ca.AvgStep();
    sa_size_t maxx = ca.Size()*step;
    pe = ca.Data();
    for(k=0; k<maxx; k+=step )  ret += pe[k]*conj(pe[k]);
  }
  else {    // Non regular data spacing ...
    int_4 ka = ca.MaxSizeKA();
    sa_size_t step = ca.Step(ka);
    sa_size_t gpas = ca.Size(ka)*step;
    sa_size_t naxa = ca.Size()/ca.Size(ka);
    for(j=0; j<naxa; j++)  {
      pe = ca.DataBlock().Begin()+ca.Offset(ka,j);
      for(k=0; k<gpas; k+=step)  ret += pe[k]*conj(pe[k]) ;
    }
  }
  return ret;

}

// --- Specialisation de la methode Norm2() pour tableaux complexes ---
DECL_TEMP_SPEC  /* equivalent a template <> , pour SGI-CC en particulier */
complex<r_4> TArray< complex<r_4> >::Norm2() const
{
  return  _ComputeComplexNorm_Private_(*this);
}
DECL_TEMP_SPEC  /* equivalent a template <> , pour SGI-CC en particulier */
complex<r_8> TArray< complex<r_8> >::Norm2() const
{
  return  _ComputeComplexNorm_Private_(*this);
}
//-------------------

//! Return the minimum and the maximum values of the array elements 
/*!
  This method generates an exception (\c MathExc) if called for complex arrays
*/

template <class T>
void TArray<T>::MinMax(T& min, T& max) const
{
  const T * pe;
  sa_size_t j,k;
  int_4 ka = MaxSizeKA();
  sa_size_t step = Step(ka);
  sa_size_t gpas = Size(ka)*step;
  sa_size_t naxa = Size()/Size(ka);
  min = (*this)[0];
  max = (*this)[0];
  for(j=0; j<naxa; j++)  {
    pe = mNDBlock.Begin()+Offset(ka,j);
    for(k=0; k<gpas; k+=step) {
      if (pe[k]<min)  min = pe[k];
      else if (pe[k]>max)  max = pe[k];      
    }
  }
  return;
}

DECL_TEMP_SPEC  /* equivalent a template <> , pour SGI-CC en particulier */
void TArray< complex<r_4> >::MinMax(complex<r_4>& min, complex<r_4>& max) const
{
  throw MathExc("TArray< complex<r_4> >::MinMax(...) - No order in complex");
}
DECL_TEMP_SPEC  /* equivalent a template <> , pour SGI-CC en particulier */
void TArray< complex<r_8> >::MinMax(complex<r_8>& min, complex<r_8>& max) const
{
  throw MathExc("TArray< complex<r_4> >::MinMax(...) - No order in complex");
}
#ifdef SO_LDBLE128
DECL_TEMP_SPEC  /* equivalent a template <> , pour SGI-CC en particulier */
void TArray< complex<r_16> >::MinMax(complex<r_16>& min, complex<r_16>& max) const
{
  throw MathExc("TArray< complex<r_16> >::MinMax(...) - No order in complex");
}
#endif

// ----------------------------------------------------
//       Impression, etc ...
// ----------------------------------------------------

//! Return a string that contain the type \b T of the array
template <class T>
string TArray<T>::InfoString() const
{
  string rs = "TArray<" ;
  rs += typeid(T).name();
  rs += "> ";
  return(rs);
}

//! Print array
/*!
  \param os : output stream
  \param maxprt : maximum number of elements printed
  \param si : if true,  display attached DvList and Units (if applicable)
  \param ascd : if true, suppresses the display of line numbers,
  suitable for ascii dump format. 
  \sa SetMaxPrint
  \sa WriteASCII
 */
template <class T>
ostream& TArray<T>::Print(ostream& os, sa_size_t maxprt, bool si, bool ascd) const
{
  if (maxprt < 0)  maxprt = max_nprt_;
  sa_size_t npr = 0;
  // keep stream's io flags 
  // ios_base::fmtflags ioflg = os.flags(); compile pas sur OSF-cxx
  // os << right ;   compile pas sur OSF-cxx
  
  Show(os, si);
  if (ndim_ < 1) return os;
  if (si) {
    bool fgdu;
    cout << " *DBG* TArray<T>::Print() - Appel a GetUnits() ... " << endl;
    Units un = GetUnits(fgdu);
    cout << " *DBG* TArray<T>::Print() - APRES GetUnits() fgdu= " << fgdu << endl;  
    cout << " *DBG* TArray<T>::Print() - APRES GetUnits() un: "; un.Print(cout,2) << endl;    
  
    if (fgdu) os << "Units: " << un.Name() << endl;
  }

  // Calcul de la largeur d'impression pour chaque element
  int fprtw = os.precision()+7;
  int prtw = 5;

  if ( (typeid(T) == typeid( int_4 )) || (typeid(T) == typeid( uint_4 )) ) prtw = 8;
  else if ( (typeid(T) == typeid( int_8 )) || (typeid(T) == typeid( uint_8 )) ) prtw = 11;
  else if ( typeid(T) == typeid( r_4 ) ) prtw = fprtw;
  else if ( typeid(T) == typeid( r_8 ) ) prtw = fprtw;
  else if ( typeid(T) == typeid(complex<r_4>) ) prtw = fprtw;
  else if ( typeid(T) == typeid(complex<r_8>) ) prtw = fprtw;


  sa_size_t k0,k1,k2,k3,k4;
  for(k4=0; k4<size_[4]; k4++) {
    if ((size_[4] > 1) && !ascd) 
      os << "\n ----- Dimension 5 (U) K4= " << k4 << endl;
    for(k3=0; k3<size_[3]; k3++) {
      if ((size_[3] > 1) && !ascd) 
	os << "\n ----- Dimension 4 (T) K3= " << k3 << endl;
      for(k2=0; k2<size_[2]; k2++) {
	if ((size_[2] > 1) && !ascd) 
	  os << "\n ----- Dimension 3 (Z) K2= " << k2 << endl;
	for(k1=0; k1<size_[1]; k1++) {
	  if ( (size_[1] > 1) && (size_[0] > 10) && !ascd) 
	    os << "----- Dimension 2 (Y) K1= " << k1 << endl;
	  for(k0=0; k0<size_[0]; k0++) {
            if(k0 > 0) os << " ";  
            os << setw(prtw) << Elem(k0, k1, k2, k3, k4);     npr++;
	    if (npr >= (sa_size_t) maxprt)  {
	      if (npr < totsize_)  os << "\n     .... " << endl; return os;
	    }
	  }
          os << endl;
	}
      }
    }
  }
  os <<  endl;
  //compile pas sur OSF-cxx os.flags(ioflg);  // reset stream io flags
  return os;
}

//! Fill the array, decoding the ASCII input stream
/*!
  \param is : input stream (ASCII)
  \param nr : Number of non empty (or comment) lines in stream (return value)
  \param nc : Number of columns (= ntot/nlines) (return value)
  \param clm : Lines starting with clm character are treated as comment lines
  \param sep : word separator in lines
  \return Number of decoded elements
*/
template <class T>
sa_size_t TArray<T>::ReadASCII(istream& is, sa_size_t & nr, sa_size_t & nc,
			       char clm, const char* sep)
{
  EnumeratedSequence es;
  sa_size_t n = es.FillFromFile(is, nr, nc, clm, sep);
  if ( (n < 1) || (nr < 1) || (nc < 1) ) return(n); 
  if (!IsAllocated()) {
    sa_size_t sz[2];
    if (arrtype_ == 2) { // C'est un vecteur 
      sz[0] = sz[1] = 1;
      sz[veceli_] = n;
    }
    else {
      sz[RowsKA()] = nr;
      sz[ColsKA()] = nc;
    }
    ReSize(2, sz);
  }
  SetSeq(es);
  if (BaseArray::GetPrintLevel()>0) 
    cout << "TArray<T>::ReadASCII()/Info: " << n << " elements read from stream " 
	 << " (Row,Col= " << nr << "," << nc << ")" << endl;
  return(n);
}

//! Writes the array content to the output stream, (in ASCII) 
/*!
  \param os : output stream (ASCII)
  \sa Print
 */
template <class T>
void TArray<T>::WriteASCII(ostream& os) const
{
  Print(os, Size(), false, true);
}



///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////
#ifdef __CXX_PRAGMA_TEMPLATES__
#pragma define_template TArray<uint_1>
#pragma define_template TArray<uint_2>
#pragma define_template TArray<uint_4>
#pragma define_template TArray<uint_8>
#pragma define_template TArray<int_1>
#pragma define_template TArray<int_2>
#pragma define_template TArray<int_4>
#pragma define_template TArray<int_8>
#pragma define_template TArray<r_4>
#pragma define_template TArray<r_8>
#pragma define_template TArray< complex<r_4> > 
#pragma define_template TArray< complex<r_8> > 
#ifdef SO_LDBLE128
#pragma define_template TArray<r_16>
#pragma define_template TArray< complex<r_16> > 
#endif
#endif

#if defined(ANSI_TEMPLATES) || defined(GNU_TEMPLATES)
template class TArray<uint_1>; 
template class TArray<uint_2>;
template class TArray<uint_4>;
template class TArray<uint_8>;
template class TArray<int_1>; 
template class TArray<int_2>;  
template class TArray<int_4>;
template class TArray<int_8>;
template class TArray<r_4>;
template class TArray<r_8>;
template class TArray< complex<r_4> >;
template class TArray< complex<r_8> >;
#ifdef SO_LDBLE128
template class TArray<r_16>;
template class TArray< complex<r_16> >;
#endif
#endif

} // FIN namespace SOPHYA 
