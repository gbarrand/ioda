// This may look like C code, but it is really -*- C++ -*-
//  Classe pour faciliter entree-sortie PPF pour des pointeurs
// SOPHYA class library - (C)  UPS+LAL IN2P3/CNRS  , CEA-Irfu 
//       R. Ansari -  Mars 2005 , updated Nov 2012

#ifndef PPFTPOINTERIO_H_SEEN
#define PPFTPOINTERIO_H_SEEN

#include "ppersist.h"
#include "timestamp.h"

//----------------------------------------------------------------------
/*!
  \class SOPHYA::PPF_TPointer_IO
  \ingroup BaseTools
  A template utility class with static methods to ease handling of array 
  I/O in PPF streams (POutPersist and PInPersist). It is not intented for
  direct use in user programs, but for writing PPF persistence handler 
  classes (PPersist) and is used by SegDataBlock,SwSegDataBlock classes.

  The C++ I/O operators POutPersist& << and PInPersist& >> and the equal 
  operator = (T const&) should be defined for new types and classes.
  Alternatively, it is possible to redefine the Write,Read,Copy methods 

   \sa SOPHYA::SegDataBlock 
   \sa SOPHYA::SwSegDataBlock
   \sa SOPHYA::PPFDataSwapper
   \sa SOPHYA::SwPPFSegDataBlock
*/

namespace SOPHYA {

template <class T>
class PPF_TPointer_IO {
public:
  //! Copy the \b n objects from the array pointed to by \b src to the array \b dest.
  static void Copy(T * dest, T const * src, size_t n)
  {
    for(size_t k=0; k<n; k++) dest[k]=src[k];
  }
  //! Writes the array pointed to by \b arr (size \b n) to POutPersist stream \b os
  static void Write(POutPersist & os, T  const * arr, size_t n)
  {
    for(size_t k=0; k<n; k++) os << arr[k];
  }
  //! Reads from the PInPersist \b is into array pointed to by \b arr (size \b n)
  static void Read(PInPersist & is, T * arr, size_t n)
  {
    for(size_t k=0; k<n; k++) is >> arr[k];
  }
  /*! 
    \brief return true if PPF I/O for object T has a fixed size. 
    
    The default implementation return false, except for numerical types 
    (int_1/2/4/8, uint_1/2/4/8, r_4/8, complex< r_4/8 >.   
    This method is used for example by the PPFDataSwapper<T> class. 
    if true is returned, the PPFDataSwapper object attempt to rewrite 
    the data block in the same file area (position), if the corresponding 
    data was previously swapped to file.
  */
  static bool isFixed_IO_RecordSize() { return false; } 
};

//! \cond
// declaration specifique pour ecriture sous forme de tableaux PPF
DECL_TEMP_SPEC  /* equivalent a template <> , pour SGI-CC en particulier */
class PPF_TPointer_IO<char> {
public:
  static void Copy(char * dest, char const * src, size_t n);
  static void Write(POutPersist & os, char  const * arr, size_t n);
  static void Read(PInPersist & is, char * arr, size_t n);
  static bool isFixed_IO_RecordSize() { return true; } 
};

DECL_TEMP_SPEC  /* equivalent a template <> , pour SGI-CC en particulier */
class PPF_TPointer_IO<int_1> {
public:
  static void Copy(int_1 * dest, int_1 const * src, size_t n);
  static void Write(POutPersist & os, int_1  const * arr, size_t n);
  static void Read(PInPersist & is, int_1 * arr, size_t n);
  static bool isFixed_IO_RecordSize() { return true; } 
};
DECL_TEMP_SPEC  /* equivalent a template <> , pour SGI-CC en particulier */
class PPF_TPointer_IO<uint_1> {
public:
  static void Copy(uint_1 * dest, uint_1 const * src, size_t n);
  static void Write(POutPersist & os, uint_1  const * arr, size_t n);
  static void Read(PInPersist & is, uint_1 * arr, size_t n);
  static bool isFixed_IO_RecordSize() { return true; } 
};

DECL_TEMP_SPEC  /* equivalent a template <> , pour SGI-CC en particulier */
class PPF_TPointer_IO<int_2> {
public:
  static void Copy(int_2 * dest, int_2 const * src, size_t n);
  static void Write(POutPersist & os, int_2  const * arr, size_t n);
  static void Read(PInPersist & is, int_2 * arr, size_t n);
  static bool isFixed_IO_RecordSize() { return true; } 
};
DECL_TEMP_SPEC  /* equivalent a template <> , pour SGI-CC en particulier */
class PPF_TPointer_IO<uint_2> {
public:
  static void Copy(uint_2 * dest, uint_2 const * src, size_t n);
  static void Write(POutPersist & os, uint_2  const * arr, size_t n);
  static void Read(PInPersist & is, uint_2 * arr, size_t n);
  static bool isFixed_IO_RecordSize() { return true; } 
};

DECL_TEMP_SPEC  /* equivalent a template <> , pour SGI-CC en particulier */
class PPF_TPointer_IO<int_4> {
public:
  static void Copy(int_4 * dest, int_4 const * src, size_t n);
  static void Write(POutPersist & os,  int_4  const * arr, size_t n);
  static void Read(PInPersist & is, int_4 * arr, size_t n);
  static bool isFixed_IO_RecordSize() { return true; } 
};
DECL_TEMP_SPEC  /* equivalent a template <> , pour SGI-CC en particulier */
class PPF_TPointer_IO<uint_4> {
public:
  static void Copy(uint_4 * dest, uint_4 const * src, size_t n);
  static void Write(POutPersist & os,  uint_4  const * arr, size_t n);
  static void Read(PInPersist & is, uint_4 * arr, size_t n);
  static bool isFixed_IO_RecordSize() { return true; } 
};

DECL_TEMP_SPEC  /* equivalent a template <> , pour SGI-CC en particulier */
class PPF_TPointer_IO<int_8> {
public:
  static void Copy(int_8 * dest, int_8 const * src, size_t n);
  static void Write(POutPersist & os, int_8  const * arr, size_t n);
  static void Read(PInPersist & is, int_8 * arr, size_t n);
  static bool isFixed_IO_RecordSize() { return true; } 
};
DECL_TEMP_SPEC  /* equivalent a template <> , pour SGI-CC en particulier */
class PPF_TPointer_IO<uint_8> {
public:
  static void Copy(uint_8 * dest, uint_8 const * src, size_t n);
  static void Write(POutPersist & os, uint_8  const * arr, size_t n);
  static void Read(PInPersist & is, uint_8 * arr, size_t n);
  static bool isFixed_IO_RecordSize() { return true; } 
};

DECL_TEMP_SPEC  /* equivalent a template <> , pour SGI-CC en particulier */
class PPF_TPointer_IO<r_4> {
public:
  static void Copy(r_4 * dest, r_4 const * src, size_t n);
  static void Write(POutPersist & os, r_4  const * arr, size_t n);
  static void Read(PInPersist & is, r_4 * arr, size_t n);
  static bool isFixed_IO_RecordSize() { return true; } 
};
DECL_TEMP_SPEC  /* equivalent a template <> , pour SGI-CC en particulier */
class PPF_TPointer_IO<r_8> {
public:
  static void Copy(r_8 * dest, r_8 const * src, size_t n);
  static void Write(POutPersist & os, r_8  const * arr, size_t n);
  static void Read(PInPersist & is, r_8 * arr, size_t n);
  static bool isFixed_IO_RecordSize() { return true; } 
};

DECL_TEMP_SPEC  /* equivalent a template <> , pour SGI-CC en particulier */
class PPF_TPointer_IO< complex<r_4> > {
public:
  static void Copy(complex<r_4> * dest, complex<r_4> const * src, size_t n);
  static void Write(POutPersist & os, complex<r_4>  const * arr, size_t n);
  static void Read(PInPersist & is, complex<r_4> * arr, size_t n);
  static bool isFixed_IO_RecordSize() { return true; } 
};
DECL_TEMP_SPEC  /* equivalent a template <> , pour SGI-CC en particulier */
class PPF_TPointer_IO< complex<r_8> > {
public:
  static void Copy(complex<r_8> * dest, complex<r_8> const * src, size_t n);
  static void Write(POutPersist & os, complex<r_8>  const * arr, size_t n);
  static void Read(PInPersist & is, complex<r_8> * arr, size_t n);
  static bool isFixed_IO_RecordSize() { return true; } 
};
DECL_TEMP_SPEC  /* equivalent a template <> , pour SGI-CC en particulier */
class PPF_TPointer_IO< TimeStamp > {
public:
  static void Copy(TimeStamp * dest, TimeStamp const * src, size_t n);
  static void Write(POutPersist & os, TimeStamp const * arr, size_t n);
  static void Read(PInPersist & is, TimeStamp * arr, size_t n);
  static bool isFixed_IO_RecordSize() { return true; } 
};

#ifdef SO_LDBLE128
DECL_TEMP_SPEC  /* equivalent a template <> , pour SGI-CC en particulier */
class PPF_TPointer_IO<r_16> {
public:
  static void Copy(r_16 * dest, r_16 const * src, size_t n);
  static void Write(POutPersist & os, r_16  const * arr, size_t n);
  static void Read(PInPersist & is, r_16 * arr, size_t n);
  static bool isFixed_IO_RecordSize() { return true; } 
};

DECL_TEMP_SPEC  /* equivalent a template <> , pour SGI-CC en particulier */
class PPF_TPointer_IO< complex<r_16> > {
public:
  static void Copy(complex<r_16> * dest, complex<r_16> const * src, size_t n);
  static void Write(POutPersist & os, complex<r_16>  const * arr, size_t n);
  static void Read(PInPersist & is, complex<r_16> * arr, size_t n);
  static bool isFixed_IO_RecordSize() { return true; } 
};
#endif

//! \endcond
}  // namespace SOPHYA

#endif   /* PPFTPOINTERIO_H_SEEN */
