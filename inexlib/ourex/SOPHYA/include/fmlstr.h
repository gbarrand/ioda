// This may look like C code, but it is really -*- C++ -*-
//-----------------------------------------------------------
// Class  FMLString<N> : fixed maximum length string 
// SOPHYA class library - (C)  UPS+LAL IN2P3/CNRS  , CEA-Irfu 
// R. Ansari      UPS+LAL IN2P3/CNRS  2013
//-----------------------------------------------------------

#ifndef FMLSTRING_H_SEEN
#define FMLSTRING_H_SEEN

#include "machdefs.h"
#include <string.h> 
#include <strings.h> 
#include <string> 
#include <iostream> 

/*!
   \class SOPHYA::FMLString
   \ingroup SUtils 
   Template class for character strings with fixed maximum length. 
   Following C language convention, strings are null terminated.
   The template parameter defines the buffer size which includes 
   the null character at the end. An FMLStr<N> can thus hold a maximum 
   of N-1 useful characters. The template parameter type is unsigned 
   short, with a maximum value of 65535.  
   Equality and comparaison operators are also provided.

   \sa SOPHYA::SegDataBlock 
   \sa SOPHYA::SwSegDataBlock
*/

namespace SOPHYA {

template <uint_2 N>
class FMLString {

public:
  //! Default constructor - buffer initialized to null characters.
  explicit FMLString()
  { bzero(buff_,N); }
  //! Copy constructor
  FMLString(FMLString<N> const & fls)
  { memcpy(buff_,fls.buff_,N); }
  //! constructor from a c++ std::string 
  explicit FMLString(std::string const & s)
  { CopyFrom(s.c_str()); }
  //! constructor from a c null terminated string 
  explicit FMLString(const char* s)
  { CopyFrom(s); }
  //! return the buffer size. Maximum length corresponds to size()-1 
  inline uint_2 size() { return N; } 
  //! return the current length of the string 
  inline uint_2 length() const { return strlen(buff_); }
  //! access to the character buffer, can be used as a C NULL terminated character string - alias for cbuff_ptr()
  inline const char* c_str() const { return buff_; }
  //! access to the character buffer, can be used as a C NULL terminated character string
  inline const char* cbuff_ptr() const { return buff_; }
  //! Copy operator 
  FMLString<N>& operator = (FMLString<N> const & fls)
  { memcpy(buff_,fls.buff_,N); return (*this); } 
  //! Copy operator 
  FMLString<N>& operator = (std::string const & s)
  { CopyFrom(s.c_str());  return (*this); }
  //! Copy operator 
  FMLString<N>& operator = (const char* s)
  { CopyFrom(s);  return (*this); }
  //! print all information about the object
  ostream&  printall(ostream& os)  const  
  { os << "FMLString<" << N << "> length=" << length() << " buff:" << buff_ << endl; return os; }
  //! print all information about the object on cout
  inline ostream&  printall()  const  
  { return printall(cout); }
  /*! 
    \brief Compare two FMLString<N>: return <0 when (s1<s2), 0 (s1==s2), >0 (s1>s2) 
    standard library strcmp() function is used 
  */ 
  static int Compare(FMLString<N> const & s1, FMLString<N> const & s2)
  { return strcmp(s1.cbuff_ptr(), s2.cbuff_ptr()); }

protected:
  void CopyFrom(const char* s)
  {
    size_t l=strlen(s);
    if (l<N) {  
      memcpy(buff_,s,l);   bzero(buff_+l,N-l); 
    }
    else  { 
      memcpy(buff_,s,N-1);  buff_[N-1]='\0'; 
    }
    return;
  }
  char buff_[N];
};

/*! operator << overloading - Prints the FMLString<N> character buffer on the stream \b s */
template <uint_2 N>
ostream& operator << (ostream& s, FMLString<N> const & fsl)
{  s << fsl.cbuff_ptr(); return(s);  }

/*! Equality comparison operator == for FMLString<N>s : return true if (a == b)  */
template <uint_2 N>
inline bool operator == (FMLString<N> const& a, FMLString<N> const& b)
{ return ( FMLString<N>::Compare(a,b)==0); }

/*! Not equal comparison operator != for FMLString<N>s : return true if (a != b)  */
template <uint_2 N>
inline bool operator != (FMLString<N> const& a, FMLString<N> const& b)
{ return ( FMLString<N>::Compare(a,b)!=0); }

/*! Comparison operator < for FMLString<N>s : return true if (a < b)  */
template <uint_2 N>
inline bool operator < (FMLString<N> const& a, FMLString<N> const& b)
{ return ( FMLString<N>::Compare(a,b)<0); }

/*! Comparison operator > for FMLString<N>s : return true if (a > b)  */
template <uint_2 N>
inline bool operator > (FMLString<N> const& a, FMLString<N> const& b)
{ return ( FMLString<N>::Compare(a,b)>0); }

/*! Comparison operator <= for FMLString<N>s : return true if (a <= b)  */
template <uint_2 N>
inline bool operator <= (FMLString<N> const& a, FMLString<N> const& b)
{ return ( FMLString<N>::Compare(a,b)<=0); }

/*! Comparison operator >= for FMLString<N>s : return true if (a >= b)  */
template <uint_2 N>
inline bool operator >= (FMLString<N> const& a, FMLString<N> const& b)
{ return ( FMLString<N>::Compare(a,b)>=0); }

//! string with a maximum length of 15 (buffer size=16)
typedef FMLString<16> String16 ;
//! string with a maximum length of 31 (buffer size=32)
typedef FMLString<32> String32 ;
//! string with a maximum length of 63 (buffer size=64)
typedef FMLString<64> String64 ;
//! string with a maximum length of 127 (buffer size=128)
typedef FMLString<128> String128 ;


} // Fin namespace

#endif
