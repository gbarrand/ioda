// This may look like C code, but it is really -*- C++ -*-
//      Base array class - Memory organisation management
//                     R. Ansari, C.Magneville   03/2000

#ifndef BaseArray_SEEN
#define BaseArray_SEEN

#include "machdefs.h"
#include <math.h>
#include <iostream>
#include "anydataobj.h"
#include "mutyv.h"
#include "dvlist.h"


//! Maximum number of dimensions for an array 
/*! \anchor BASEARRAY_MAXNDIMS */
#define BASEARRAY_MAXNDIMS  5

namespace SOPHYA {  

//   ------------ classe template Array ----------- 
//! Base class for template arrays
class BaseArray : public AnyDataObj {
public:
  //! To define Array or Matrix memory mapping
  enum MemoryMapping {
    AutoMemoryMapping = -1,  //!< define Auto Memory Mapping
    SameMemoryMapping = 0,   //!< define Same Memory Mapping
    CMemoryMapping = 1,      //!< define C Memory Mapping
    FortranMemoryMapping = 2 //!< define Fortran Memory Mapping
    };
  //! To define Vector type
  enum VectorType {
    AutoVectorType = -1,  //!< define Auto Vector Type
    SameVectorType = 0,   //!< define Same Vector Type
    ColumnVector = 1,     //!< define Column Vector Type
    RowVector = 2         //!< define Row Vector Type
    };

  // threshold for parallel routine call
  static void	 SetOpenMPSizeThreshold(sa_size_t thr=200000);
  //! Get Size threshold for parallel routine call
  static inline sa_size_t GetOpenMPSizeThreshold() { return openmp_size_threshold; }

  static void    SetMaxPrint(sa_size_t nprt=50, int_4 lev=0);   
  //! Get maximum number of printed elements
  static inline sa_size_t GetMaxPrint() { return max_nprt_; }
  //! Get print  level
  static inline int_4 GetPrintLevel() { return prt_lev_; }

  static short   SetDefaultMemoryMapping(short mm=CMemoryMapping);
  //! Get Default Memory Mapping
  static inline short GetDefaultMemoryMapping() { return default_memory_mapping; }
  static short   SetDefaultVectorType(short vt=ColumnVector);
  //! Get Default Vector Type
  static inline short GetDefaultVectorType() { return default_vector_type; }

  //! Optimization choice for matrix product
  static void SetMatProdOpt(uint_2 opt=1);
  static inline uint_2 GetMatProdOpt(void) {return matrix_product_optim;}
  
  // Creator / destructor
  BaseArray();
  virtual ~BaseArray();

  // Returns true if ndim and sizes are equal
  virtual bool CompareSizes(const BaseArray& a, bool& smo) const;

  // Array dimensions
  //! Return true if the array was allocated ( Rank() > 0 )
  inline bool IsAllocated() const { return( (ndim_ > 0) ? true : false ); }
  //! Return number of dimensions (array rank)
  inline int_4 NbDimensions() const { return( ndim_ ); }
  //! Return array rank (number of dimensions) 
  inline int_4 Rank() const { return( ndim_ ); }

  //! Return total size of the array
  inline sa_size_t Size() const { return(totsize_); }
  //! Return size along the first dimension
  inline sa_size_t SizeX() const { return(size_[0]); }
  //! Return size along the second dimension
  inline sa_size_t SizeY() const { return(size_[1]); }
  //! Return size along the third dimension
  inline sa_size_t SizeZ() const { return(size_[2]); }
  //! Return size along the \b ka th dimension
  inline sa_size_t Size(int_4 ka) const { return(size_[CheckDI(ka,1)]); }

  int_4 MaxSizeKA() const ;

  //! Get memory organization
  inline short  GetMemoryMapping() const 
         { return ( (marowi_ == 1) ? CMemoryMapping : FortranMemoryMapping) ; }
  //! line index dimension
  inline int_4    RowsKA() const {return marowi_; }
  //! column index dimension
  inline int_4    ColsKA() const {return macoli_; }
  //! Index dimension of the elements of a vector
  inline int_4    VectKA() const {return veceli_; }
  void          SetMemoryMapping(short mm=AutoMemoryMapping);

  //! Get Vector type ( \b Line or \b Column vector )
  inline short  GetVectorType() const 
                { return((marowi_ == veceli_) ? ColumnVector : RowVector); }
  void SetVectorType(short vt=AutoVectorType);

  // memory organisation - packing information
  //! return true if array is packed in memory
  inline bool   IsPacked() const { return(moystep_ == 1); }
  //! return true if array is packed along the first dimension
  inline bool   IsPackedX() const { return(step_[0] == 1); }
  //! return true if array is packed along the second dimension
  inline bool   IsPackedY() const { return(step_[1] == 1); }
  //! return true if array is packed along the third dimension
  inline bool   IsPackedZ() const { return(step_[2] == 1); }
  //! return true if array is packed along the \b ka th dimension
  inline bool   IsPacked(int_4 ka) const { return(step_[CheckDI(ka,2)] == 1); }

  //! return the minimum step value along all the dimensions
  inline sa_size_t MinStep() const  { return(minstep_); }
  //! return the average step value along all the dimensions
  inline sa_size_t AvgStep() const  { return(moystep_); }
  //! return the step along the first dimension
  inline sa_size_t StepX() const { return(step_[0]); }
  //! return the step along the second dimension
  inline sa_size_t StepY() const { return(step_[1]); }
  //! return the step along the third dimension
  inline sa_size_t StepZ() const { return(step_[2]); }
  //! return the step along the \b ka th dimension
  inline sa_size_t Step(int_4 ka) const { return(step_[CheckDI(ka,3)]); }

  int_4 MinStepKA() const ;

  // Offset of element ip
  sa_size_t Offset(sa_size_t ip=0) const ;
  // Offset of the i'th vector along axe ka 
  sa_size_t Offset(int_4 ka, sa_size_t i) const ;
  inline sa_size_t  Offset(sa_size_t ix, sa_size_t iy, sa_size_t iz, sa_size_t it=0, sa_size_t iu=0) const;  
  // Index values of element ip
  void	         IndexAtPosition(sa_size_t ip, sa_size_t & ix, sa_size_t & iy, sa_size_t & iz, 
				 sa_size_t & it, sa_size_t & iu) const;
  // an abstract element acces methode
  virtual MuTyV & ValueAtPosition(sa_size_t ip) const = 0;
  virtual MuTyV & ValueAtPositionDB(sa_size_t ip) const = 0;

  // Pour recuperer pas et numero d'axe pour operations sur deux arrays
  void   GetOpeParams(const BaseArray& a, bool smo, int_4& ax, int_4& axa, sa_size_t& step,
		      sa_size_t& stepa, sa_size_t& gpas, sa_size_t& naxa) const;
  // Impression, I/O, ...
  void           Show(ostream& os, bool si=false) const;
  //! Show information on \b cout
  inline void    Show() const { Show(cout); }
  virtual string InfoString() const;

  // Lecture,Ecriture sur fichier ASCII
  //! Reads an array from an ASCII stream
  virtual sa_size_t ReadASCII(istream& is, sa_size_t & nr, sa_size_t & nc,
			      char clm='#', const char* sep=" \t") = 0;
  //! Writes an array to an ASCII stream
  virtual void   WriteASCII(ostream& os) const = 0;

  // Access to the DVList info Object
  DVList& Info();
  //! return true if the  DVList info Object has been created (not empty)
  inline bool HasInfoObject() const { return ((mInfo!=NULL)?true:false); }  

//! Return the attached DVList object pointer (for special uses only TArray ... ) 
  inline DVList* getInfoPointer() const { return mInfo; }

protected:
  inline int_4 CheckDI(int_4 ka, int msg) const ;
  inline void CheckBound(sa_size_t ix, sa_size_t iy, sa_size_t iz, sa_size_t it, sa_size_t iu, int msg) const ;
  // Compacts size=1 array dimensions
  void CompactAllDim(); // suppresses all size==1 dimensions 
  void CompactTrailingDim(); // suppresses size==1 dimensions after the last size>1 dimension
  // Changing Sizes/NDim ... return true if OK
  bool UpdateSizes(int_4 ndim, const sa_size_t * siz, sa_size_t step, sa_size_t offset, string & exmsg);
  bool UpdateSizes(int_4 ndim, const sa_size_t * siz, const sa_size_t * step, sa_size_t offset, string & exmsg);
  bool UpdateSizes(const BaseArray& a, string & exmsg);
  static sa_size_t ComputeTotalSize(int_4 ndim, const sa_size_t * siz, sa_size_t step, sa_size_t offset) ;
  //  Organisation memoire
  static short SelectMemoryMapping(short mm);
  static short SelectVectorType(short vt);
  void UpdateMemoryMapping(short mm);
  void UpdateMemoryMapping(BaseArray const & a, short mm);

  // Pour Extraction de sous-tableau 
  virtual void UpdateSubArraySizes(BaseArray & ra, int_4 ndim, sa_size_t * siz, sa_size_t * pos, sa_size_t * step) const;

  // Pour mettre les tailles a zero
  virtual void SetZeroSize();

  int_4  ndim_; //!< number of dimensions of array
  sa_size_t size_[BASEARRAY_MAXNDIMS]; //!< array of the size in each dimension
  sa_size_t totsize_; //!< Total number of elements
  sa_size_t offset_;  //!< global offset -\> position of elem[0] in DataBlock
  //! two consecutive elements distance in a given dimension 
  sa_size_t step_[BASEARRAY_MAXNDIMS];
  sa_size_t minstep_; //!< minimal step (in any axes)
  sa_size_t moystep_; //!< mean step, if == 0 --\> non regular steps
  int_2  marowi_; //!< For matrices, Row index in dimensions
  int_2  macoli_; //!< For matrices, Column index in dimensions
  int_2  veceli_; //!< For vectors, dimension index = marowi_/macoli_ (Row/Col vectors)
  int_2 arrtype_; //!< 0 a TArray, 1 TMatrix , 2 TVector
  DVList* mInfo;    //!< Infos (variables) attached to the array

  static const char * ck_op_msg_[6]; //!< Operation messages for CheckDI() CheckBound()
  static sa_size_t max_nprt_; //!< maximum number of printed elements
  static int_4 prt_lev_;  //!< Print level
  static short default_memory_mapping; //!< Default memory mapping
  static short default_vector_type;    //!< Default vector type Row/Column
  static sa_size_t openmp_size_threshold; //!< Size limit for parallel routine calls
  static uint_2 matrix_product_optim; //!< optimization level for matrix product
};

// --------------------------------------------------
//        Methodes inline de verification
// --------------------------------------------------
//! to verify the compatibility of the dimension index
inline int_4 BaseArray::CheckDI(int_4 ka, int msg)  const
{
  if ( (ka < 0) || (ka >= ndim_) ) {
    string txt = "BaseArray::CheckDimensionIndex/Error ";   txt += ck_op_msg_[msg];
    throw(RangeCheckError(txt));
  }
  return(ka);
}

//! to verify the compatibility of the indexes in all dimensions
inline void BaseArray::CheckBound(sa_size_t ix, sa_size_t iy, sa_size_t iz, sa_size_t it, sa_size_t iu, int msg)  const
{
  if ( (ix >= size_[0]) || (ix < 0) || (iy >= size_[1]) || (iy < 0) || 
       (iz >= size_[2]) || (iz < 0) || (it >= size_[3]) || (it < 0) ||
       (iu >= size_[4]) || (iu < 0) )  {
    string txt = "BaseArray::CheckArrayBound/Error ";   txt += ck_op_msg_[msg];
    throw(RangeCheckError(txt));
  }
  return;
}



// --------------------------------------------------
//        Position d'un element
// --------------------------------------------------
//! Offset of element (ix,iy,iz,it,iu)
inline sa_size_t BaseArray::Offset(sa_size_t ix, sa_size_t iy, sa_size_t iz, sa_size_t it, sa_size_t iu) const
{
#ifdef SO_BOUNDCHECKING
  CheckBound(ix, iy, iz, it, iu, 4);
#endif
  return ( offset_+  ix*step_[0] + iy*step_[1] + iz*step_[2] +
	      it*step_[3] + iu*step_[4] );
}


} // Fin du namespace

#endif
