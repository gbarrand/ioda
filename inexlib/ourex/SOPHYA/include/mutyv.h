// This may look like C code, but it is really -*- C++ -*-
// Classe MuTyV : Variable multi-type numerique
//                               Reza Ansari - 96-2000

#ifndef MUTYV_H_SEEN
#define MUTYV_H_SEEN

#include "machdefs.h"

#include <string>
#include <complex>
#include "timestamp.h"

#include <iostream>

namespace SOPHYA {

// Classe utilitaire pour manipuler des variables typees 
//! A simple class for holding string, integer, float and complex type values.

class MuTyV {
public:

  enum MTVType {
    MTVInteger,
    MTVFloat,
    MTVComplex,
    MTVString,
    MTVTimeStamp
  };

  inline MuTyV() { typ = MTVInteger; iv = 0;  dv = dv_im = 0.;  strv = NULL; }
         MuTyV(MuTyV const & a);
  inline MuTyV(uint_1 i) { typ = MTVInteger; iv = (int_8)i;  dv = (r_8)i;  dv_im = 0.;  strv = NULL; }
  inline MuTyV(uint_2 i) { typ = MTVInteger; iv = (int_8)i;  dv = (r_8)i;  dv_im = 0.;  strv = NULL; }
  inline MuTyV(uint_4 i) { typ = MTVInteger; iv = (int_8)i;  dv = (r_8)i;  dv_im = 0.;  strv = NULL; }
  inline MuTyV(uint_8 i) { typ = MTVInteger; iv = (int_8)i;  dv = (r_8)i;  dv_im = 0.;  strv = NULL; }
  inline MuTyV(int_1 i)  { typ = MTVInteger; iv = (int_8)i;  dv = (r_8)i;  dv_im = 0.;  strv = NULL; }
  inline MuTyV(int_2 i)  { typ = MTVInteger; iv = (int_8)i;  dv = (r_8)i;  dv_im = 0.;  strv = NULL; }
  inline MuTyV(int_4 i)  { typ = MTVInteger; iv = (int_8)i;  dv = (r_8)i;  dv_im = 0.;  strv = NULL; }
  inline MuTyV(int_8 i)  { typ = MTVInteger; iv = i;  dv = (r_8)i;  dv_im = 0.;  strv = NULL; }
  inline MuTyV(r_4 f) { typ = MTVFloat; dv = (r_8)f; iv = (int_8)f;  dv_im = 0.;  strv = NULL; } 
  inline MuTyV(r_8 d) { typ = MTVFloat; dv = d;  iv = (int_8)d;  dv_im = 0.;  strv = NULL; } 
  inline MuTyV(complex<r_4> const& z) { typ = MTVComplex; dv = (r_8)z.real();  dv_im = z.imag();  
                                 iv = (int_8)dv;  strv = NULL; } 
  inline MuTyV(complex<r_8> const& z) { typ = MTVComplex; dv = z.real();  dv_im = z.imag();  
                                 iv = (int_8)dv;  strv = NULL; } 

         MuTyV(char const* s);  
         MuTyV(string const& s); 
         MuTyV(TimeStamp const& ts); 

         ~MuTyV();
 
         MuTyV & operator= (MuTyV const & a);
  inline MuTyV & operator= (uint_1 v)  { typ = MTVInteger; iv = (int_8)v;  dv = (r_8)v;  dv_im = 0.;  return(*this); }
  inline MuTyV & operator= (uint_2 v)  { typ = MTVInteger; iv = (int_8)v;  dv = (r_8)v;  dv_im = 0.;  return(*this); }
  inline MuTyV & operator= (uint_4 v)  { typ = MTVInteger; iv = (int_8)v;  dv = (r_8)v;  dv_im = 0.;  return(*this); }
  inline MuTyV & operator= (uint_8 v)  { typ = MTVInteger; iv = (int_8)v;  dv = (r_8)v;  dv_im = 0.;  return(*this); }
  inline MuTyV & operator= (int_1 v)  { typ = MTVInteger; iv = (int_8)v;  dv = (r_8)v;  dv_im = 0.;  return(*this); }
  inline MuTyV & operator= (int_2 v)  { typ = MTVInteger; iv = (int_8)v;  dv = (r_8)v;  dv_im = 0.;  return(*this); }
  inline MuTyV & operator= (int_4 v)  { typ = MTVInteger; iv = (int_8)v;  dv = (r_8)v;  dv_im = 0.;  return(*this); }
  inline MuTyV & operator= (int_8 v)  { typ = MTVInteger; iv = v;  dv = (r_8)v; dv_im = 0.;  return(*this); }
  inline MuTyV & operator= (r_4 v)  { typ = MTVFloat; dv = (r_8)v; iv = (int_8)v;  dv_im = 0.;  return(*this); }
  inline MuTyV & operator= (r_8 v) { typ = MTVFloat; dv = v; iv = (int_8)v;  dv_im = 0.;  return(*this); }
  inline MuTyV & operator= (complex<r_4> const& v) { typ = MTVComplex; dv = (r_8)v.real();  dv_im = (r_8)v.imag();
                                                    iv = (int_8)dv;  return(*this); }
  inline MuTyV & operator= (complex<r_8> const& v) { typ = MTVComplex; dv = (r_8)v.real();  dv_im = v.imag();
                                                    iv = (int_8)dv;  return(*this); }
#ifdef SO_LDBLE128
  inline MuTyV & operator= (r_16 v) { typ = MTVFloat; dv = (r_8)v; iv = (int_8)v;  dv_im = 0.;  return(*this); }
  inline MuTyV & operator= (complex<r_16> const& v) { typ = MTVComplex; dv = (r_8)v.real();  dv_im = (r_8)v.imag();
                                                    iv = (int_8)dv;  return(*this); }
#endif
  const char*  operator= (const char* s);
  string const& operator= (string const & s);
  TimeStamp const& operator= (TimeStamp const & s);

  inline operator uint_1() const { return((uint_1)iv); }
  inline operator uint_2() const { return((uint_2)iv); }
  inline operator uint_4() const { return((uint_4)iv); }
  inline operator uint_8() const { return((uint_8)iv); }
  inline operator int_1()  const { return((int_1)iv); }
  inline operator int_2()  const { return((int_2)iv); }
  inline operator int_4()  const { return((int_4)iv); }
  inline operator int_8()  const { return(iv); }
  inline operator r_4() const  { return((r_4)dv); }
  inline operator r_8() const  { return(dv); }
  inline operator complex<r_4>() const { return(complex<r_4>((r_4)dv, (r_4)dv_im)); }
  inline operator complex<r_8>() const { return(complex<r_8>(dv, dv_im)); }
#ifdef SO_LDBLE128
  inline operator r_16() const  { return((r_16)dv); }
  inline operator complex<r_16>() const { return(complex<r_16>((r_16)dv, (r_16)dv_im)); }
#endif

  operator string() const ; 
  operator TimeStamp() const ; 

  inline uint_1 Convert(uint_1& x) const { x = (uint_1)iv; return x; }
  inline uint_2 Convert(uint_2& x) const { x = (uint_2)iv; return x; }
  inline uint_4 Convert(uint_4& x) const { x = (uint_4)iv; return x; }
  inline uint_8 Convert(uint_8& x) const { x = (uint_8)iv; return x; }
  inline int_1 Convert(int_1& x) const { x = (int_1)iv; return x; }
  inline int_2 Convert(int_2& x) const { x = (int_2)iv; return x; }
  inline int_4 Convert(int_4& x) const { x = (int_4)iv; return x; }
  inline int_8 Convert(int_8& x) const { x = iv; return x; }
  inline r_4 Convert(r_4& x) const { x = (r_4)dv; return x; }
  inline r_8 Convert(r_8& x) const { x = dv; return x; }
  inline complex<r_4> Convert(complex<r_4> & x) const 
    { x = complex< r_4 > ((r_4)dv, (r_4)dv_im); return x; }
  inline complex<r_8> Convert(complex<r_8> & x) const 
    { x = complex< r_8 > (dv, dv_im); return x; }
#ifdef SO_LDBLE128
  inline r_16 Convert(r_16& x) const { x = (r_16)dv; return x; }
  inline complex<r_16> Convert(complex<r_16> & x) const 
    { x = complex< r_16 > ((r_16)dv, (r_16)dv_im); return x; }
#endif  
  string&  Convert(string& x) const ; 
  TimeStamp& Convert(TimeStamp& x) const ; 

  inline MTVType Type() const { return typ; }
  inline int_8   GetIntPart() const { return iv; }
  inline r_8     GetRealPart() const { return dv; }
  inline r_8     GetImagPart() const { return dv_im; }
  inline string*  GetStringPointer() const { return (strv); } 

protected:
  int_8 iv;
  r_8 dv;
  r_8 dv_im;   /* for holding imaginary part of a complex */
  string * strv;
  MTVType typ;

};

inline ostream& operator << (ostream& s, MuTyV const & mtv)
{  s << (string)mtv;   return(s); }
 

} // namespace SOPHYA

#endif /* MUTYV_H_SEEN */


