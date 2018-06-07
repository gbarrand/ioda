#ifdef TSNMLUPG4
/* This is a specific declaration file to cope with two level
   name look-up scheme , introduced by gcc >= 3.4
   THIS FILE SHOULD NOT NORMALLY BE INCLUDED IN USER PROGRAMS
   R. Ansari -  Jan 2006 - Updated Nov 2012 for compilation with clang (V2) 
   See version V1 below for gcc < 4.2 
*/

using SOPHYA::BaseArray::MemoryMapping  ;
using SOPHYA::BaseArray::AutoMemoryMapping  ;
using SOPHYA::BaseArray::SameMemoryMapping  ;
using SOPHYA::BaseArray::CMemoryMapping  ;
using SOPHYA::BaseArray::FortranMemoryMapping  ;

using SOPHYA::BaseArray::VectorType  ;
using SOPHYA::BaseArray::AutoVectorType  ;
using SOPHYA::BaseArray::SameVectorType  ;
using SOPHYA::BaseArray::ColumnVector  ;
using SOPHYA::BaseArray::RowVector  ;
/*
using SOPHYA::TMatrix<T>::MemoryMapping  ;
using SOPHYA::TMatrix<T>::AutoMemoryMapping  ;
using SOPHYA::TMatrix<T>::SameMemoryMapping  ;
using SOPHYA::TMatrix<T>::CMemoryMapping  ;
using SOPHYA::TMatrix<T>::FortranMemoryMapping  ;

using SOPHYA::TMatrix<T>::VectorType  ;
using SOPHYA::TMatrix<T>::AutoVectorType  ;
using SOPHYA::TMatrix<T>::SameVectorType  ;
using SOPHYA::TMatrix<T>::ColumnVector  ;
using SOPHYA::TMatrix<T>::RowVector  ;
*/

using SOPHYA::BaseArray::SelectVectorType  ;
using SOPHYA::BaseArray::GetDefaultVectorType  ;
using SOPHYA::BaseArray::GetVectorType  ;


using SOPHYA::BaseArray::NbDimensions  ;
using SOPHYA::BaseArray::RowsKA  ;
using SOPHYA::BaseArray::ColsKA  ;
using SOPHYA::BaseArray::VectKA  ;
using SOPHYA::BaseArray::Size  ;
using SOPHYA::BaseArray::Step  ;

using SOPHYA::BaseArray::GetMatProdOpt  ;
using SOPHYA::BaseArray::SetMemoryMapping  ;
using SOPHYA::BaseArray::GetMemoryMapping ;
using SOPHYA::BaseArray::GetDefaultMemoryMapping ;
using SOPHYA::BaseArray::UpdateMemoryMapping  ;

using SOPHYA::TMatrix<T>::SubArray  ;

using SOPHYA::TArray<T>::Set  ;
using SOPHYA::TArray<T>::SetT  ;

using SOPHYA::TArray<T>::AddCst  ;
using SOPHYA::TArray<T>::SubCst  ;
using SOPHYA::TArray<T>::MulCst  ;
using SOPHYA::TArray<T>::DivCst  ;

using SOPHYA::TArray<T>::AddElt  ;
using SOPHYA::TArray<T>::SubElt  ;
using SOPHYA::TArray<T>::MulElt  ;
using SOPHYA::TArray<T>::DivElt  ;

using SOPHYA::TMatrix<T>::NRows  ;
using SOPHYA::TMatrix<T>::NCols  ;

// using SOPHYA::TMatrix<T>::  ;

using SOPHYA::BaseArray::max_nprt_ ;
using SOPHYA::BaseArray::ndim_ ;
using SOPHYA::BaseArray::size_ ;
using SOPHYA::BaseArray::totsize_ ;
using SOPHYA::BaseArray::offset_ ;
using SOPHYA::BaseArray::step_ ;
using SOPHYA::BaseArray::minstep_ ;
using SOPHYA::BaseArray::moystep_ ;

using SOPHYA::BaseArray::marowi_  ;
using SOPHYA::BaseArray::macoli_ ;
using SOPHYA::BaseArray::veceli_  ;
using SOPHYA::BaseArray::arrtype_  ;

using SOPHYA::TMatrix<T>::mNDBlock ;

using SOPHYA::TMatrix<T>::SetSeq ;
using SOPHYA::TMatrix<T>::SetBA ;
using SOPHYA::TMatrix<T>::Show ;

#endif  /* TSNMLUPG4 */


/* ---------------------------------------------------------*/
#ifdef TSNMLUPG4_V1 
/* Version V1, for g++ version < 4.2 - Jan 2006 */

using SOPHYA::TMatrix<T>::MemoryMapping  ;
using SOPHYA::TMatrix<T>::AutoMemoryMapping  ;
using SOPHYA::TMatrix<T>::SameMemoryMapping  ;
using SOPHYA::TMatrix<T>::CMemoryMapping  ;
using SOPHYA::TMatrix<T>::FortranMemoryMapping  ;

using SOPHYA::TMatrix<T>::VectorType  ;
using SOPHYA::TMatrix<T>::AutoVectorType  ;
using SOPHYA::TMatrix<T>::SameVectorType  ;
using SOPHYA::TMatrix<T>::ColumnVector  ;
using SOPHYA::TMatrix<T>::RowVector  ;

using SOPHYA::TMatrix<T>::SelectVectorType  ;
using SOPHYA::TMatrix<T>::GetDefaultVectorType  ;
using SOPHYA::TMatrix<T>::GetVectorType  ;


using SOPHYA::TMatrix<T>::NbDimensions  ;
using SOPHYA::TMatrix<T>::RowsKA  ;
using SOPHYA::TMatrix<T>::ColsKA  ;
using SOPHYA::TMatrix<T>::VectKA  ;
using SOPHYA::TMatrix<T>::Size  ;

using SOPHYA::TMatrix<T>::GetMatProdOpt  ;
using SOPHYA::TMatrix<T>::SetMemoryMapping  ;
using SOPHYA::TMatrix<T>::GetMemoryMapping ;
using SOPHYA::TMatrix<T>::GetDefaultMemoryMapping ;
using SOPHYA::TMatrix<T>::UpdateMemoryMapping  ;

using SOPHYA::TMatrix<T>::SubArray  ;

using SOPHYA::TMatrix<T>::NRows  ;
using SOPHYA::TMatrix<T>::NCols  ;

// using SOPHYA::TMatrix<T>::  ;

using SOPHYA::TMatrix<T>::max_nprt_ ;
using SOPHYA::TMatrix<T>::ndim_ ;
using SOPHYA::TMatrix<T>::size_ ;
using SOPHYA::TMatrix<T>::totsize_ ;
using SOPHYA::TMatrix<T>::offset_ ;
using SOPHYA::TMatrix<T>::step_ ;
using SOPHYA::TMatrix<T>::minstep_ ;
using SOPHYA::TMatrix<T>::moystep_ ;

using SOPHYA::TMatrix<T>::marowi_  ;
using SOPHYA::TMatrix<T>::macoli_ ;
using SOPHYA::TMatrix<T>::veceli_  ;
using SOPHYA::TMatrix<T>::arrtype_  ;

using SOPHYA::TMatrix<T>::mNDBlock ;

using SOPHYA::TMatrix<T>::SetSeq ;
using SOPHYA::TMatrix<T>::SetBA ;
using SOPHYA::TMatrix<T>::Show ;

#endif  /* TSNMLUPG4_V1 */

