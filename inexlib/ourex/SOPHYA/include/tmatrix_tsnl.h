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

using SOPHYA::TArray<T>::SubArray  ;

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

using SOPHYA::TArray<T>::mNDBlock ;

using SOPHYA::TArray<T>::SetSeq ;
using SOPHYA::TArray<T>::SetBA ;
using SOPHYA::TArray<T>::Show ;
using SOPHYA::TArray<T>::SetUnits ;
using SOPHYA::TArray<T>::GetUnits ;

#endif  /* TSNMLUPG4 */

/* ---------------------------------------------- */
#ifdef TSNMLUPG4_V1
/* Version V1, for g++ version < 4.2 - Jan 2006 */
using SOPHYA::TArray<T>::MemoryMapping  ;
using SOPHYA::TArray<T>::AutoMemoryMapping  ;
using SOPHYA::TArray<T>::SameMemoryMapping  ;
using SOPHYA::TArray<T>::CMemoryMapping  ;
using SOPHYA::TArray<T>::FortranMemoryMapping  ;

using SOPHYA::TArray<T>::VectorType  ;
using SOPHYA::TArray<T>::AutoVectorType  ;
using SOPHYA::TArray<T>::SameVectorType  ;
using SOPHYA::TArray<T>::ColumnVector  ;
using SOPHYA::TArray<T>::RowVector  ;

using SOPHYA::TArray<T>::NbDimensions  ;
using SOPHYA::TArray<T>::RowsKA  ;
using SOPHYA::TArray<T>::ColsKA  ;
using SOPHYA::TArray<T>::VectKA  ;
using SOPHYA::TArray<T>::Size  ;

using SOPHYA::TArray<T>::GetMatProdOpt  ;
using SOPHYA::TArray<T>::SetMemoryMapping  ;
using SOPHYA::TArray<T>::GetMemoryMapping ;
using SOPHYA::TArray<T>::GetDefaultMemoryMapping ;
using SOPHYA::TArray<T>::UpdateMemoryMapping  ;

using SOPHYA::TArray<T>::SubArray  ;

// using SOPHYA::TArray<T>::  ;

using SOPHYA::TArray<T>::max_nprt_ ;
using SOPHYA::TArray<T>::ndim_ ;
using SOPHYA::TArray<T>::size_ ;
using SOPHYA::TArray<T>::totsize_ ;
using SOPHYA::TArray<T>::offset_ ;
using SOPHYA::TArray<T>::step_ ;
using SOPHYA::TArray<T>::minstep_ ;
using SOPHYA::TArray<T>::moystep_ ;

using SOPHYA::TArray<T>::marowi_  ;
using SOPHYA::TArray<T>::macoli_ ;
using SOPHYA::TArray<T>::veceli_  ;
using SOPHYA::TArray<T>::arrtype_  ;

using SOPHYA::TArray<T>::mNDBlock ;

using SOPHYA::TArray<T>::SetSeq ;
using SOPHYA::TArray<T>::SetBA ;
using SOPHYA::TArray<T>::Show ;
using SOPHYA::TArray<T>::SetUnits ;
using SOPHYA::TArray<T>::GetUnits ;

#endif  /* TSNMLUPG4_V1 */
