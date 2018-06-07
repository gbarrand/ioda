// This may look like C code, but it is really -*- C++ -*-
// SOPHYA class library - (C)  UPS+LAL IN2P3/CNRS  , CEA-Irfu 
//  Classe pour faciliter entree-sortie PPF pour des pointeurs
//  Implementation specifique pour certains type de pointeurs
//       R. Ansari -  Mars 2005 , updated Nov 2012


#include "machdefs.h"
#include "ppftpointerio.h"
#include <string.h>

namespace SOPHYA {

//----------------------------------------------------------------------
//! \cond
// Specific implementation using table I/O in PPFBinaryIOStreams 
/* DECL_TEMP_SPEC   equivalent a template <> , pour SGI-CC en particulier */
void PPF_TPointer_IO< char >::Copy(char * dest, char const * src, size_t n)
  { memcpy((void *)dest, (const void *)src, n*sizeof(char)); }
void PPF_TPointer_IO< char >::Write(POutPersist & os, char const * arr, size_t n)
  { os.PutBytes(arr,n); }
void PPF_TPointer_IO< char >::Read(PInPersist & is, char * arr, size_t n)
  { is.GetBytes(arr,n); }

void PPF_TPointer_IO< int_1 >::Copy(int_1 * dest, int_1 const * src, size_t n)
  { memcpy((void *)dest, (const void *)src, n*sizeof(int_1)); }
void PPF_TPointer_IO< int_1 >::Write(POutPersist & os, int_1 const * arr, size_t n)
  { os.Put(arr,n); }
void PPF_TPointer_IO< int_1 >::Read(PInPersist & is, int_1 * arr, size_t n)
  { is.Get(arr,n); }

void PPF_TPointer_IO< uint_1 >::Copy(uint_1 * dest, uint_1 const * src, size_t n)
  { memcpy((void *)dest, (const void *)src, n*sizeof(uint_1)); }
void PPF_TPointer_IO< uint_1 >::Write(POutPersist & os, uint_1 const * arr, size_t n)
  { os.Put(arr,n); }
void PPF_TPointer_IO< uint_1 >::Read(PInPersist & is, uint_1 * arr, size_t n)
  { is.Get(arr,n); }

void PPF_TPointer_IO< int_2 >::Copy(int_2 * dest, int_2 const * src, size_t n)
  { memcpy((void *)dest, (const void *)src, n*sizeof(int_2)); }
void PPF_TPointer_IO< int_2 >::Write(POutPersist & os, int_2 const * arr, size_t n)
  { os.Put(arr,n); }
void PPF_TPointer_IO< int_2 >::Read(PInPersist & is, int_2 * arr, size_t n)
  { is.Get(arr,n); }

void PPF_TPointer_IO< uint_2 >::Copy(uint_2 * dest, uint_2 const * src, size_t n)
  { memcpy((void *)dest, (const void *)src, n*sizeof(uint_2)); }
void PPF_TPointer_IO< uint_2 >::Write(POutPersist & os, uint_2 const * arr, size_t n)
  { os.Put(arr,n); }
void PPF_TPointer_IO< uint_2 >::Read(PInPersist & is, uint_2 * arr, size_t n)
  { is.Get(arr,n); }

void PPF_TPointer_IO< int_4 >::Copy(int_4 * dest, int_4 const * src, size_t n)
  { memcpy((void *)dest, (const void *)src, n*sizeof(int_4)); }
void PPF_TPointer_IO< int_4 >::Write(POutPersist & os, int_4 const * arr, size_t n)
  { os.Put(arr,n); }
void PPF_TPointer_IO< int_4 >::Read(PInPersist & is, int_4 * arr, size_t n)
  { is.Get(arr,n); }

void PPF_TPointer_IO< uint_4> ::Copy(uint_4 * dest, uint_4 const * src, size_t n)
  { memcpy((void *)dest, (const void *)src, n*sizeof(uint_4)); }
void PPF_TPointer_IO< uint_4 >::Write(POutPersist & os, uint_4 const * arr, size_t n)
  { os.Put(arr,n); }
void PPF_TPointer_IO< uint_4 >::Read(PInPersist & is, uint_4 * arr, size_t n)
  { is.Get(arr,n); }

void PPF_TPointer_IO< int_8 >::Copy(int_8 * dest, int_8 const * src, size_t n)
  { memcpy((void *)dest, (const void *)src, n*sizeof(int_8)); }
void PPF_TPointer_IO< int_8 >::Write(POutPersist & os,  int_8 const * arr, size_t n)
  { os.Put(arr,n); }
void PPF_TPointer_IO< int_8 >::Read(PInPersist & is, int_8 * arr, size_t n)
  { is.Get(arr,n); }

void PPF_TPointer_IO< uint_8 >::Copy(uint_8 * dest, uint_8 const * src, size_t n)
  { memcpy((void *)dest, (const void *)src, n*sizeof(uint_8)); }
void PPF_TPointer_IO< uint_8 >::Write(POutPersist & os,  uint_8 const * arr, size_t n)
  { os.Put(arr,n); }
void PPF_TPointer_IO< uint_8 >::Read(PInPersist & is, uint_8 * arr, size_t n)
  { is.Get(arr,n); }


void PPF_TPointer_IO< r_4> ::Copy(r_4 * dest, r_4 const * src, size_t n)
  { memcpy((void *)dest, (const void *)src, n*sizeof(r_4)); }
void PPF_TPointer_IO< r_4 >::Write(POutPersist & os, r_4 const * arr, size_t n)
  { os.Put(arr,n); }
void PPF_TPointer_IO< r_4 >::Read(PInPersist & is, r_4 * arr, size_t n)
  { is.Get(arr,n); }

void PPF_TPointer_IO< r_8 >::Copy(r_8 * dest, r_8 const * src, size_t n)
  { memcpy((void *)dest, (const void *)src, n*sizeof(r_8)); }
void PPF_TPointer_IO< r_8 >::Write(POutPersist & os,  r_8 const * arr, size_t n)
  { os.Put(arr,n); }
void PPF_TPointer_IO< r_8 >::Read(PInPersist & is, r_8 * arr, size_t n)
  { is.Get(arr,n); }

void PPF_TPointer_IO< complex<r_4> >::Copy(complex<r_4> * dest, complex<r_4> const * src, size_t n)
  { memcpy((void *)dest, (const void *)src, n*sizeof(complex<r_4>)); }
void PPF_TPointer_IO< complex<r_4> >::Write(POutPersist & os, complex<r_4> const * arr, size_t n)
  { os.Put(arr,n); }
void PPF_TPointer_IO< complex<r_4> >::Read(PInPersist & is, complex<r_4> * arr, size_t n)
  { is.Get(arr,n); }

void PPF_TPointer_IO< complex<r_8> >::Copy(complex<r_8> * dest, complex<r_8> const * src, size_t n)
  { memcpy((void *)dest, (const void *)src, n*sizeof(complex<r_8>)); }
void PPF_TPointer_IO< complex<r_8> >::Write(POutPersist & os, complex<r_8> const * arr, size_t n)
  { os.Put(arr,n); }
void PPF_TPointer_IO< complex<r_8> >::Read(PInPersist & is, complex<r_8> * arr, size_t n)
  { is.Get(arr,n); }

void PPF_TPointer_IO< TimeStamp >::Copy(TimeStamp * dest, TimeStamp const * src, size_t n)
  { memcpy((void *)dest, (const void *)src, n*sizeof(TimeStamp)); }
void PPF_TPointer_IO< TimeStamp >::Write(POutPersist & os, TimeStamp const * arr, size_t n)
  { 
    int_8 * dp = new int_8[n];
    r_8 * sp = new r_8[n];
    for(size_t k=0; k<n; k++) { dp[k] = arr[k].DaysPart(); sp[k]=arr[k].SecondsPart(); }
    os.Put(dp,n);
    os.Put(sp,n);
    delete[] dp;
    delete[] sp;
  }
void PPF_TPointer_IO< TimeStamp >::Read(PInPersist & is, TimeStamp * arr, size_t n)
  { 
    int_8 * dp = new int_8[n];
    r_8 * sp = new r_8[n];
    is.Get(dp,n);
    is.Get(sp,n);
    for(size_t k=0; k<n; k++)  arr[k].Set(dp[k],sp[k]); 
    delete[] dp;
    delete[] sp;
  }

#ifdef SO_LDBLE128
void PPF_TPointer_IO< r_16 >::Copy(r_16 * dest, r_16 const * src, size_t n)
  { memcpy((void *)dest, (const void *)src, n*sizeof(r_16)); }
void PPF_TPointer_IO< r_16 >::Write(POutPersist & os,  r_16 const * arr, size_t n)
  { os.Put(arr,n); }
void PPF_TPointer_IO< r_16 >::Read(PInPersist & is, r_16 * arr, size_t n)
  { is.Get(arr,n); }

void PPF_TPointer_IO< complex<r_16> >::Copy(complex<r_16> * dest, complex<r_16> const * src, size_t n)
  { memcpy((void *)dest, (const void *)src, n*sizeof(complex<r_16>)); }
void PPF_TPointer_IO< complex<r_16> >::Write(POutPersist & os, complex<r_16> const * arr, size_t n)
  { os.Put(arr,n); }
void PPF_TPointer_IO< complex<r_16> >::Read(PInPersist & is, complex<r_16> * arr, size_t n)
  { is.Get(arr,n); }
#endif


//! \endcond

/*
#ifdef __CXX_PRAGMA_TEMPLATES__
#pragma define_template PPF_TPointer_IO< char >
#pragma define_template PPF_TPointer_IO< int_1 >
#pragma define_template PPF_TPointer_IO< uint_1 >
#pragma define_template PPF_TPointer_IO< int_2 >
#pragma define_template PPF_TPointer_IO< uint_2 >
#pragma define_template PPF_TPointer_IO< int_4 >
#pragma define_template PPF_TPointer_IO< uint_4 >
#pragma define_template PPF_TPointer_IO< int_8 >
#pragma define_template PPF_TPointer_IO< uint_8 >
#pragma define_template PPF_TPointer_IO< r_4 >
#pragma define_template PPF_TPointer_IO< r_8 >
#pragma define_template PPF_TPointer_IO< complex<r_4> >
#pragma define_template PPF_TPointer_IO< complex<r_8> >
#endif

#if defined(ANSI_TEMPLATES) || defined(GNU_TEMPLATES)
template class PPF_TPointer_IO< char >;
template class PPF_TPointer_IO< int_1 >;
template class PPF_TPointer_IO< uint_1 >;
template class PPF_TPointer_IO< int_2 >;
template class PPF_TPointer_IO< uint_2 >;
template class PPF_TPointer_IO< int_4 >;
template class PPF_TPointer_IO< uint_4 >;
template class PPF_TPointer_IO< int_8 >;
template class PPF_TPointer_IO< uint_8 >;
template class PPF_TPointer_IO< r_4 >;
template class PPF_TPointer_IO< r_8 >;
template class PPF_TPointer_IO< complex<r_4> >;
template class PPF_TPointer_IO< complex<r_8> >;
#endif
*/

}  // namespace SOPHYA
