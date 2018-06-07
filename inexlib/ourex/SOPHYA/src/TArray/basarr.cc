//      Base class for numerical arrays 
//                     R. Ansari, C.Magneville   03/2000

#include "sopnamsp.h"
#include "machdefs.h"
#include <stdio.h>
#include <stdlib.h>
#include "pexceptions.h"
#include "basarr.h"

/*!
  \class SOPHYA::BaseArray
  \ingroup TArray
  Base class for template arrays with number of dimensions up to 
  \ref BASEARRAY_MAXNDIMS "BASEARRAY_MAXNDIMS".
  This class is an abstract class and has no data connected to it.
  
  Define base methods, enum and defaults for TArray , TMatrix and TVector.
  BaseArray objects can be used in particular for performing operations on 
  arrays with unknown data types, or between arrays with different data types.
*/

// Variables statiques globales
const char * BaseArray::ck_op_msg_[6] =
     {"???", "Size(int )", "IsPacked(int )"
     ,"Stride(int )", "ElemCheckBound()", "operator()" };
sa_size_t BaseArray::max_nprt_ = 50;
int_4  BaseArray::prt_lev_ = 0;
short  BaseArray::default_memory_mapping = CMemoryMapping;
short  BaseArray::default_vector_type = ColumnVector;
sa_size_t BaseArray::openmp_size_threshold = 200000;
uint_2 BaseArray::matrix_product_optim = 1;

// ------ Methodes statiques globales --------

//! Set optimization strategy for matrix product
/*!
  \param opt : bit coded optimization strategy
  \warning Default is opt=1
  \verbatim
  bit 0 : choose matrix product optimization or not
        0=no optimization, 1=optimization
  bit 1 : force optimization in any case (only if bit0=1)
          (if not the TMatrix::Multiply method decide or
           not if the product should be optimized)
        0=do not force and let the method decide, 1=force
  bit 2 : optimize the product A * B when A-columns and
          B-rows are not packed (for example if the product
          is C = ("A Fortran-like" * "B C-like").
        . That will do a copy of one of the two matrices,
          so that will result in an increase of the memory
          space needed.
        . For big matrices that decrease the computing time.
        . Do not use this optimisation for small matrices
          because that would increase the computing time.
        0=do not optimze that way, 1=optimze that way
  \endverbatim
  \verbatim
   Sumary of the allowed values for "opt"
           0 = no optimization at all (whatever the other bits are)
           1 = optimize but let the method decides if optimization
               is needed regarding the sizes of the matrices.
           3 = force optimization whatever the sizes of the matrices are.
           5 = optimisation with method decision ("1")
               AND optimize by copying when A-columns and B-rows
               are not packed
           7 = force optimization ("3")
               AND optimize by copying when A-columns and B-rows
               are not packed
  \endverbatim
*/
void BaseArray::SetMatProdOpt(uint_2 opt)
{
  matrix_product_optim = opt;
}

//! Set maximum number of printed elements and print level
/*!
  \param nprt : maximum number of print
  \param lev  : print level
*/
void BaseArray::SetMaxPrint(sa_size_t nprt, int_4 lev)
{
  max_nprt_ = nprt;
  prt_lev_ = (lev < 3) ? lev : 3;
}

//! Set Size threshold for parallel routine call
/*!
  \param thr : thresold value
*/
void BaseArray::SetOpenMPSizeThreshold(sa_size_t thr)
{
  openmp_size_threshold = thr;
}


//! Compute totale size
/*!
  \param ndim : number of dimensions
  \param siz : array of size along the \b ndim dimensions
  \param step[ndim] : step value
  \param offset : offset value
  \return Total size of the array
*/
sa_size_t BaseArray::ComputeTotalSize(int_4 ndim, const sa_size_t * siz, sa_size_t step, sa_size_t offset)
{
  sa_size_t rs = step;
  for(sa_size_t k=0; k<ndim; k++) rs *= siz[k];
  return(rs+offset);
}

//! Set Default Memory Mapping
/*!
  \param mm : Memory Mapping type
  \verbatim
     mm == CMemoryMapping : C like memory mapping
     mm == FortranMemoryMapping : Fortran like memory mapping
  \endverbatim
  \return default memory mapping value
  \verbatim
  # ===== For Matrices
  *** MATHEMATICS:   m(row,column) with indexes running [1,n])
                          | 11 12 13 |
     matrix Math = Mmath= | 21 22 23 |
                          | 31 32 33 |
  *** IDL, \b FORTRAN: indexes data in \b row-major format:
     indexes arrays in (column,row) order.
     index IDL running [0,n[ ; index FORTRAN running [1,n]
     M in memory: [ 11 12 13 : 21 22 23 : 31 32 33 : ... ]
                     line 1  : line 2   : line 3  : ...
     ex: Midl(0,2) = Mfor(1,3) = Mmath(3,1) = 31
         Midl(2,0) = Mfor(3,1) = Mmath(1,3) = 13
  *** C: indexes data in \b column-major format:
     indexes arrays in [row][column] order.
     index C running [0,n[
     M in memory: [ 11 21 31 : 12 22 32 : 13 23 33 : ... ]
                    column 1 : column 2 : column 3 : ...
     ex: Mc[2][0] = Mmath(3,1) = 31
         Mc[0][2] = Mmath(1,3) = 13
  *** SUMMARY difference Idl/Fortan/C/Math:
    Midl(col-1,row-1) = Mfor(col,row) = Mc[row-1][col-1] = Mmath(row,col)
    TRANSPOSE(column-major array) --> row-major array
  \endverbatim
 */
short BaseArray::SetDefaultMemoryMapping(short mm)
{
  default_memory_mapping = (mm != CMemoryMapping) ? FortranMemoryMapping : CMemoryMapping; 
  return default_memory_mapping;
}

//! Set Default Vector Type
/*!
  \param vt : vector type (ColumnVector,RowVector)
  \return default vector type value
 */
short BaseArray::SetDefaultVectorType(short vt)
{
  default_vector_type = (vt != ColumnVector) ? RowVector : ColumnVector ; 
  return default_vector_type;
}

//! Select Memory Mapping
/*!
  Do essentially nothing.
  \param mm : type of Memory Mapping (CMemoryMapping,FortranMemoryMapping)
  \return return \b mm if it makes sense or default memory mapping value
  \sa SetDefaultMemoryMapping
 */
short BaseArray::SelectMemoryMapping(short mm)
{
  if ( (mm == CMemoryMapping) || (mm == FortranMemoryMapping) )  return (mm) ; 
  else return (default_memory_mapping);
}

//! Select Vector type
/*!
  Do essentially nothing.
  \param vt : vector type (ColumnVector,RowVector)
  \return return \b vt if it makes sense or default vector type
  \sa SetDefaultVectorType
 */
short BaseArray::SelectVectorType(short vt)
{
  if ((vt == ColumnVector) || (vt == RowVector))  return(vt);
  else return(default_vector_type);
}

//! Update Memory Mapping
/*!
  Update variables marowi_ macoli_ veceli_
  \param mm : type of Memory Mapping (CMemoryMapping,FortranMemoryMapping)
  \sa SetDefaultMemoryMapping
 */
void BaseArray::UpdateMemoryMapping(short mm)
{
  short vt = default_vector_type;
  if ( (mm != CMemoryMapping) && (mm != FortranMemoryMapping) ) mm = default_memory_mapping;
  if (mm == CMemoryMapping) {
    marowi_  = 1;  macoli_ = 0;
  }
  else {
    marowi_ = 0;  macoli_ = 1;
  }

  if ( (ndim_ == 2) && ((size_[0] == 1) || (size_[1] == 1)) ) {
    // Choix automatique Vecteur ligne ou colonne
    if ( size_[macoli_] == 1)  veceli_ = marowi_;
    else veceli_ = macoli_;
  }
  else veceli_ = (vt ==  ColumnVector ) ?  marowi_ : macoli_;
}

//! Update Memory Mapping
/*!
  \param a  : Array to be compared with
  \param mm : type of Memory Mapping or memory mapping transfert
       (SameMemoryMapping,AutoMemoryMapping,CMemoryMapping,FortranMemoryMapping)
  \sa SetDefaultMemoryMapping
 */
void BaseArray::UpdateMemoryMapping(BaseArray const & a, short mm)
{
  short vt = default_vector_type;
  if (mm == SameMemoryMapping) {
    mm = ((a.marowi_ == 1) ? CMemoryMapping : FortranMemoryMapping);
    vt = (a.marowi_ == a.veceli_) ? ColumnVector : RowVector;
  }
  else if (mm == AutoMemoryMapping)   mm = default_memory_mapping;

  if ( (mm != CMemoryMapping) && (mm != FortranMemoryMapping) ) mm = default_memory_mapping;
  if (mm == CMemoryMapping) {
    marowi_  = 1;  macoli_ = 0;
  }
  else {
    marowi_ = 0;  macoli_ = 1;
  }
  if ( (ndim_ == 2) && ((size_[0] == 1) || (size_[1] == 1)) ) {
    // Choix automatique Vecteur ligne ou colonne
    if ( size_[macoli_] == 1)  veceli_ = marowi_;
    else veceli_ = macoli_;
  }
  else veceli_ = (vt ==  ColumnVector ) ?  marowi_ : macoli_;
}

//! Set Memory Mapping type
/*!
  Compute values for variables marowi_ macoli_ veceli_
  \param mm : Memory Mapping type (SameMemoryMapping,AutoMemoryMapping
                                  ,CMemoryMapping,FortranMemoryMapping)
  \sa SetDefaultMemoryMapping
 */
void BaseArray::SetMemoryMapping(short mm)
{
  if (mm == SameMemoryMapping) mm = GetMemoryMapping();
  else if (mm == AutoMemoryMapping)  mm = default_memory_mapping;
  if ( (mm != CMemoryMapping) && (mm != FortranMemoryMapping) ) mm = CMemoryMapping;
  short vt = GetVectorType(); 
  if (mm == CMemoryMapping) {
    marowi_ =  1;  macoli_ = 0;
  }
  else {
    marowi_ =  0;  macoli_ = 1;
  }
  if ( (ndim_ == 2) && ((size_[0] == 1) || (size_[1] == 1)) 
       && (size_[0] != size_[1]) ) {
    // Choix automatique Vecteur ligne ou colonne
    if ( size_[macoli_] == 1)  veceli_ = marowi_;
    else veceli_ = macoli_;
  }
  else  veceli_ = (vt ==  ColumnVector ) ?  marowi_ : macoli_;
}

//! Set Vector Type
/*!
  Compute values for variable veceli_
  \param vt : vector type ()
  \sa SetDefaultVectorType
 */
void BaseArray::SetVectorType(short vt)
{
  if (vt == SameVectorType) return;
  if (vt == AutoVectorType) vt =  default_vector_type;
  if ( (ndim_ == 2) && ((size_[0] == 1) || (size_[1] == 1)) ) {
    // Choix automatique Vecteur ligne ou colonne
    if ( size_[macoli_] == 1)  veceli_ = marowi_;
    else veceli_ = macoli_;
  }
  else  veceli_ = (vt ==  ColumnVector ) ?  marowi_ : macoli_;
}

// -------------------------------------------------------
//                Methodes de la classe
// -------------------------------------------------------

//! Default constructor
BaseArray::BaseArray()
  : mInfo(NULL)
{
  ndim_ = 0;
  for(int_4 k=0; k<BASEARRAY_MAXNDIMS; k++) step_[k] = size_[k] = 0;
  totsize_ = 0;
  minstep_ = 0;
  moystep_ = 0;
  offset_ = 0;
  // Default for matrices : Memory organisation and Vector type 
  if (default_memory_mapping == CMemoryMapping) {
    marowi_ =  1;  macoli_ = 0;
  }
  else {
    marowi_ =  0;  macoli_ = 1;
  }
  veceli_ = (default_vector_type ==  ColumnVector ) ?  marowi_ : macoli_;
  arrtype_ = 0;    // Default Array type, not a Matrix or Vector
  
}

//! Destructor 
BaseArray::~BaseArray()
{
  if (mInfo) { delete mInfo; mInfo = NULL; }
}


//! Returns true if the two arrays have compatible dimensions.
/*!
  \param a : array to be compared
  \param smo : Return flag = true if the two arrays have the same memory organisation
  \return true if \c NbDimensions() and \c Size() are equal, false if not

  If the array (on which the operation is being performed, \c this) 
  is a \b Matrix or a \b Vector, the matrix dimensions \c NRows() \c NCols()
  are checked. The flag \c smo is returned true if the two arrays, viewed 
  as a matrix have the same memory organisation.
  Otherwise, (if the array is of not a Matrix or a Vector) 
  the size compatibility viewed as a TArray is checked <tt> 
  (Size(k) == a.Size(k), k=0,...NbDimensions()), </tt> disregard of the memory 
  organisation and the row and column index. The flag \c smo is returned true 
  in this case.
*/
bool BaseArray::CompareSizes(const BaseArray& a, bool& smo) const
{
  if (ndim_ != a.ndim_)  return(false);
  if (arrtype_ == 0) {  // Simple TArray, not a matrix 
    smo = true;
    for(int_4 k=0; k<ndim_; k++) 
      if (size_[k] != a.size_[k])  return(false);
    return(true);
  }
  else {
    smo = false;
    if ( (size_[marowi_] != a.size_[a.marowi_]) || 
	 (size_[macoli_] != a.size_[a.macoli_])  ) return(false);
    if (ndim_ > 2)  
      for(int_4 k=2; k<ndim_; k++) 
	if (size_[k] != a.size_[k])  return(false);
    // Octobre 2009 (RA) : test plus restrictif , ' ||  (veceli_ == a.veceli_)  supprime 
    if ((macoli_ == a.macoli_) && (marowi_ == a.marowi_))   smo = true;
    return(true);
  }
}

//! Compact arrays - supresses size=1 axes.
void BaseArray::CompactAllDim()
{
  if (ndim_ < 2)  return;
  int_4 ndim = 0;
  sa_size_t size[BASEARRAY_MAXNDIMS];
  sa_size_t step[BASEARRAY_MAXNDIMS];
  for(int_4 k=0; k<ndim_; k++) {
    if (size_[k] < 2)  continue;
    size[ndim] = size_[k];
    step[ndim] = step_[k];
    ndim++;
  }
  if (ndim == 0)  {
    size[0] = size_[0];
    step[0] = step_[0];
    ndim = 1;
  }
  string exmsg = "BaseArray::CompactAllDim() ";
  if (!UpdateSizes(ndim, size, step, offset_, exmsg))  throw( ParmError(exmsg) );
  return;
}

//! Compact array taling dimensions, for size=1 traling axes.
void BaseArray::CompactTrailingDim()
{
  if (ndim_ < 2)  return;
  int_4 ndim = 0;
  sa_size_t size[BASEARRAY_MAXNDIMS];
  sa_size_t step[BASEARRAY_MAXNDIMS];
  for(int_4 k=0; k<ndim_; k++) {
    size[k] = size_[k];
    step[k] = step_[k];
    if (size_[k] > 1)  ndim=k+1;
  }
  if (ndim == 0)  ndim = 1;
  string exmsg = "BaseArray::CompactTrailingDim() ";
  if (!UpdateSizes(ndim, size, step, offset_, exmsg))  throw( ParmError(exmsg) );
  return;
}

//! return minimum value for step[ndim]
int_4  BaseArray::MinStepKA() const
{
  for(int_4 ka=0; ka<ndim_; ka++)
    if (step_[ka] == minstep_) return((int)ka);
  return(0);
}

//! return maximum value for step[ndim]
int_4  BaseArray::MaxSizeKA() const
{
  int_4 ka = 0;
  sa_size_t mx = size_[0];
  for(int_4 k=1; k<ndim_; k++)  
    if (size_[k] > mx) {  ka = k;  mx = size_[k]; } 
  return(ka);
}


//  Acces lineaire aux elements ....  Calcul d'offset
// --------------------------------------------------
// Position de l'element 0 du vecteur i selon l'axe ka
// --------------------------------------------------
//! return position of first element for vector \b i alond \b ka th axe.
sa_size_t BaseArray::Offset(int_4 ka, sa_size_t i) const
{

  if ( (ndim_ < 1) || (i == 0) )  return(offset_);
  //#ifdef SO_BOUNDCHECKING
  if (ka >= ndim_) 
    throw RangeCheckError("BaseArray::Offset(int_4 ka, sa_size_t i) Axe KA Error");
  if ( i*size_[ka] >= totsize_ )  
    throw RangeCheckError("BaseArray::Offset(int_4 ka, sa_size_t i) Index Error");
  //#endif
  sa_size_t idx[BASEARRAY_MAXNDIMS];
  int_4 k;
  sa_size_t rest = i;
  idx[ka] = 0;
  for(k=0; k<ndim_; k++) {
    if (k == ka) continue;
    idx[k] = rest%size_[k];   rest /= size_[k];
  }
  sa_size_t off = offset_;
  for(k=0; k<ndim_; k++)  off += idx[k]*step_[k];
  return (off);
}

//! return position of element \b ip.
sa_size_t BaseArray::Offset(sa_size_t ip) const
{
  if ( (ndim_ < 1) || (ip == 0) )  return(offset_);
  //#ifdef SO_BOUNDCHECKING
  if (ip >= totsize_)
    throw RangeCheckError("BaseArray::Offset(sa_size_t ip) Out of range index ip");
  //#endif

  sa_size_t idx[BASEARRAY_MAXNDIMS];
  int_4 k;
  sa_size_t rest = ip;
  for(k=0; k<ndim_; k++) {
    idx[k] = rest%size_[k];   rest /= size_[k];
  }
  //#ifdef SO_BOUNDCHECKING
  if (rest != 0) 
    throw PError("BaseArray::Offset(sa_size_t ip) BUG !!! rest != 0");
  //#endif
//   if (rest != 0) cerr << " BUG ---- BaseArray::Offset( " << ip << " )" << rest << endl;
//   cerr << " DBG-Offset( " << ip << ")" ;
//   for(k=0; k<ndim_; k++) cerr << idx[k] << "," ;
//   cerr << " ZZZZ " << endl;
  sa_size_t off = offset_;
  for(k=0; k<ndim_; k++)  off += idx[k]*step_[k];
  return (off);
}
//! return index of element \b ip, along the five array axes 
void BaseArray::IndexAtPosition(sa_size_t ip, sa_size_t & ix, sa_size_t & iy, 
				sa_size_t & iz, sa_size_t & it, sa_size_t & iu) const
{
  ix = iy = iz = it = iu = 0;
  if ( (ndim_ < 1) || (ip == 0) )  return;
  if (ip >= totsize_)
    throw RangeCheckError("BaseArray::IndexAtPosition(...) Out of range index ip");
  sa_size_t idx[BASEARRAY_MAXNDIMS]={0,0,0,0,0};
  int_4 k;
  sa_size_t rest = ip;
  for(k=0; k<ndim_; k++) {
    idx[k] = rest%size_[k];   rest /= size_[k];
    if (rest == 0)  break;
  }
  if (rest != 0) 
    throw PError("BaseArray::IndexAtPosition(...) BUG !!! rest != 0");
  ix = idx[0];
  iy = idx[1];
  iz = idx[2];
  it = idx[3];
  iu = idx[4];  
  return;
}

//! return various parameters for double loop operations on two arrays.
void BaseArray::GetOpeParams(const BaseArray& a, bool smo, int_4& ax, int_4& axa, sa_size_t& step,
			     sa_size_t& stepa, sa_size_t& gpas, sa_size_t& naxa) const
{
  if (smo) { // Same memory organisation
    ax = axa = MaxSizeKA();
  }
  else {
    if (Size(RowsKA()) >= Size(ColsKA()) ) {
      ax = RowsKA();
      axa = a.RowsKA();
    }
    else {
      ax = ColsKA();
      axa = a.ColsKA();
    }
  }
  step = Step(ax);
  stepa = a.Step(axa);
  gpas = Size(ax)*step;
  naxa = Size()/Size(ax);
  return;
}

// ----------------------------------------------------
//       Impression, etc ...
// ----------------------------------------------------

//! Show infos on stream \b os (\b si to display DvList)
void BaseArray::Show(ostream& os, bool si) const
{
  if (ndim_ < 1) {
    os << "#--- " << BaseArray::InfoString() << " Unallocated Array ! " << endl;
    return;
  } 
  os << "#--- " << InfoString() ; 
  os << " ND=" << ndim_ << " SizeX*Y*...= " ;
  for(int_4 k=0; k<ndim_; k++) { 
    os << size_[k];
    if (k<ndim_-1)  os << "x";
  }
  os << " ---" << endl;
  if (prt_lev_ > 0) {
    os <<  " TotSize= " << totsize_ << " Step(X Y ...)="  ;
    for(int_4 k=0; k<ndim_; k++)     os << step_[k] << "  " ;
    os << " Offset= " << offset_  << endl;
  }
  if (prt_lev_ > 1) {
    os << " MemoryMapping=" << GetMemoryMapping() << " VecType= " << GetVectorType()
       << " RowsKA= " << RowsKA() << " ColsKA= " << ColsKA() 
       << " VectKA=" << VectKA() << " ArrayType=" << arrtype_ << endl;
  }
  if (!si && (prt_lev_ < 2)) return;
  if (mInfo != NULL) os << (*mInfo) << endl;
 
}

//! Return BaseArray Type
string BaseArray::InfoString() const
{
  string rs = "BaseArray Type= ";
  rs +=  typeid(*this).name() ;
  return rs;
}

//! Return the attached DVList info object 
DVList& BaseArray::Info()
{
if (mInfo == NULL)  mInfo = new DVList;
return(*mInfo);
}



//! Update sizes and information for array
/*!
  \param ndim : dimension
  \param siz[ndim] : sizes
  \param step : step (must be the same on all dimensions)
  \param offset : offset of the first element
  \return true if all OK, false if problems appear
  \return string \b exmsg for explanation in case of problems
 */
bool BaseArray::UpdateSizes(int_4 ndim, const sa_size_t * siz, sa_size_t step, sa_size_t offset, string & exmsg)
{
  if (ndim > BASEARRAY_MAXNDIMS) {
    exmsg += " NDim Error";  return false;
  }
  if (step < 1) {
    exmsg += " Step(=0) Error";  return false;
  }

  minstep_ = moystep_ = step;

  // Flagging bad updates ...
  ndim_ = 0;

  totsize_ = 1;
  int_4 k;
  for(k=0; k<BASEARRAY_MAXNDIMS; k++) {
    size_[k] = 1;
    step_[k] = 0;
  }
  for(k=0; k<ndim; k++) {
    size_[k] = siz[k] ;
    step_[k] = totsize_*step;
    totsize_ *= size_[k];
  }
  if (totsize_ < 1) {
    exmsg += " Size Error";  return false;
  }
  offset_ = offset;
  // Update OK 
  ndim_ = ndim;
  // Default for matrices : Memory organisation and Vector type 
  SetMemoryMapping(BaseArray::SameMemoryMapping);
  return true;
}

//! Update sizes and information for array
/*!
  \param ndim : dimension
  \param siz[ndim] : sizes
  \param step[ndim] : steps
  \param offset : offset of the first element
  \return true if all OK, false if problems appear
  \return string \b exmsg for explanation in case of problems
 */
bool BaseArray::UpdateSizes(int_4 ndim, const sa_size_t * siz, const sa_size_t * step, sa_size_t offset, string & exmsg)
{
  if (ndim > BASEARRAY_MAXNDIMS) {
    exmsg += " NDim Error";  return false;
  }

  // Flagging bad updates ...
  ndim_ = 0;

  totsize_ = 1;
  int_4 k;
  for(k=0; k<BASEARRAY_MAXNDIMS; k++) {
    size_[k] = 1;
    step_[k] = 0;
  }
  sa_size_t minstep = step[0];
  for(k=0; k<ndim; k++) {
    size_[k] = siz[k] ;
    step_[k] = step[k];
    totsize_ *= size_[k];
    if (step_[k] < minstep)  minstep = step_[k];
  }
  if (minstep < 1) {
    exmsg += " Step(=0) Error";  return false;
  }
  if (totsize_ < 1) {
    exmsg += " Size Error";  return false;
  }
  sa_size_t plast = 0;
  for(k=0; k<ndim; k++)   plast += (siz[k]-1)*step[k];
  if (plast == minstep*(totsize_-1) )  moystep_ = minstep;
  else moystep_ = 0;
  minstep_ = minstep;
  offset_ = offset;
  // Update OK 
  ndim_ = ndim;
  // Default for matrices : Memory organisation and Vector type 
  SetMemoryMapping(BaseArray::SameMemoryMapping);
  return true;
}

//! Update sizes and information relative to array \b a
/*!
  \param a : array to be compare with
  \return true if all OK, false if problems appear
  \return string \b exmsg for explanation in case of problems
 */
bool BaseArray::UpdateSizes(const BaseArray& a, string & exmsg)
{
  if (a.ndim_ > BASEARRAY_MAXNDIMS) {
    exmsg += " NDim Error";  return false;
  }

  // Flagging bad updates ...
  ndim_ = 0;

  totsize_ = 1;
  int_4 k;
  for(k=0; k<BASEARRAY_MAXNDIMS; k++) {
    size_[k] = 1;
    step_[k] = 0;
  }
  sa_size_t minstep = a.step_[0];
  for(k=0; k<a.ndim_; k++) {
    size_[k] = a.size_[k] ;
    step_[k] = a.step_[k];
    totsize_ *= size_[k];
    if (step_[k] < minstep)  minstep = step_[k];
  }
  if (minstep < 1) {
    exmsg += " Step(=0) Error";  return false;
  }
  if (totsize_ < 1) {
    exmsg += " Size Error";  return false;
  }

  minstep_ = a.minstep_;
  moystep_ = a.moystep_;
  offset_ = a.offset_;
  macoli_ = a.macoli_;
  marowi_ = a.marowi_;
  veceli_ = a.veceli_;
  // Update OK 
  ndim_ = a.ndim_;
  return true;
}


//! Update sizes information for sub-array \b ra
/*!
  \param ra : sub-array for which size information has to be computed 
  \param ndim : number of dimensions for \b ra
  \param siz[ndim],pos[ndim],step[ndim] : number of elements, offset and step along each dimension,
   relative to the original array.
  \warning throw SzMismatchError in case of incompatible dimensions.
 */
void BaseArray::UpdateSubArraySizes(BaseArray & ra, int_4 ndim, sa_size_t * siz, sa_size_t * pos, sa_size_t * step) const
{
  if ( (ndim > ndim_) || (ndim < 1) ) 
    throw(SzMismatchError("BaseArray::UpdateSubArraySizes( ... ) NDim Error") );
  int_4 k;
  for(k=0; k<ndim; k++) 
    if ( ((siz[k]-1)*step[k]+pos[k]) >= size_[k] )  
      throw(SzMismatchError("BaseArray::UpdateSubArraySizes( ... ) Size/Pos Error") );
  sa_size_t offset = offset_;
  for(k=0; k<ndim_; k++) { 
    offset += pos[k]*step_[k]; 
    step[k] *= step_[k];
  }
  string exm = "BaseArray::UpdateSubArraySizes() ";
  if (!ra.UpdateSizes(ndim, siz, step, offset,  exm))
     throw( ParmError(exm) );
  return;
}

//! Set the array sizes to zero, corresponding to an unallocated array
void BaseArray::SetZeroSize()
{
  ndim_ = 0;
  for(int_4 k=0; k<BASEARRAY_MAXNDIMS; k++) step_[k] = size_[k] = 0;
  totsize_ = 0;
  minstep_ = 0;
  moystep_ = 0;
  offset_ = 0;
  // On ne change pas l'organisation memoire, le type de vecteur
  // et le array type
}

