// This may look like C code, but it is really -*- C++ -*-
// Classe pour nom, taille, ... de types de donnees
//                         C.Magneville         10/2000
// LAL (Orsay) / IN2P3-CNRS  DAPNIA/SPP (Saclay) / CEA

#include "datatype.h"
#include <typeinfo>
#include <string.h>

#include <complex>

namespace SOPHYA {

template <class T>
DataTypeInfo<T>::DataTypeInfo()
{
}
#if defined(__SGICC__)
template <class T>
string DataTypeInfo<T>::getTypeName()
{
  if (typeid(T) == typeid(uint_1)) return("uint_1");
  else if (typeid(T) == typeid(int_1)) return("int_1");
  else if (typeid(T) == typeid(uint_2)) return("uint_2");
  else if (typeid(T) == typeid(int_2)) return("int_2");
  else if (typeid(T) == typeid(int_4)) return("int_4");
  else if (typeid(T) == typeid(int_8)) return("int_8");
  else if (typeid(T) == typeid(uint_4)) return("uint_4");
  else if (typeid(T) == typeid(uint_8)) return("uint_8");
  else if (typeid(T) == typeid(r_4)) return("r_4");
  else if (typeid(T) == typeid(r_8)) return("r_8");
  else if (typeid(T) == typeid(complex<r_4>)) return(" complex<r_4> ");
  else if (typeid(T) == typeid(complex<r_8>)) return(" complex<r_8> ");
#ifdef SO_LDBLE128
  else if (typeid(T) == typeid(r_16)) return("r_16");
  else if (typeid(T) == typeid(complex<r_16>)) return(" complex<r_16> ");
#endif
  else return("???unknowntype???");
}
template <class T>
int DataTypeInfo<T>::getTypeId()
{
  if (typeid(T) == typeid(uint_1)) return(1);
  else if (typeid(T) == typeid(int_1)) return(2);
  else if (typeid(T) == typeid(uint_2)) return(3);
  else if (typeid(T) == typeid(int_2)) return(4);
  else if (typeid(T) == typeid(uint_4)) return(5);
  else if (typeid(T) == typeid(int_4)) return(6);
  else if (typeid(T) == typeid(uint_8)) return(7);
  else if (typeid(T) == typeid(int_8)) return(8);
  else if (typeid(T) == typeid(r_4)) return(9);
  else if (typeid(T) == typeid(r_8)) return(10);
  else if (typeid(T) == typeid(complex<r_4>)) return(11);
  else if (typeid(T) == typeid(complex<r_8>)) return(12);
#ifdef SO_LDBLE128
  else if (typeid(T) == typeid(r_16)) return(13);
  else if (typeid(T) == typeid(complex<r_16>)) return(14);
#endif
  else return(0);
}
#else
//! Return the type name of T, as a string, (Example: T=uint_8 --> "uint_8")
template <class T>
string DataTypeInfo<T>::getTypeName() { return("???unknowntype???"); }
DECL_TEMP_SPEC
string DataTypeInfo<uint_1>::getTypeName() { return("uint_1"); }
DECL_TEMP_SPEC
string DataTypeInfo<int_1>::getTypeName() { return("int_1"); }
DECL_TEMP_SPEC
string DataTypeInfo<uint_2>::getTypeName() { return("uint_2"); }
DECL_TEMP_SPEC
string DataTypeInfo<int_2>::getTypeName() { return("int_2"); }
DECL_TEMP_SPEC
string DataTypeInfo<uint_4>::getTypeName() { return("uint_4"); }
DECL_TEMP_SPEC
string DataTypeInfo<int_4>::getTypeName() { return("int_4"); }
DECL_TEMP_SPEC
string DataTypeInfo<uint_8>::getTypeName() { return("uint_8"); }
DECL_TEMP_SPEC
string DataTypeInfo<int_8>::getTypeName() { return("int_8"); }
DECL_TEMP_SPEC
string DataTypeInfo<r_4>::getTypeName() { return("r_4"); }
DECL_TEMP_SPEC
string DataTypeInfo<r_8>::getTypeName() { return("r_8"); }
DECL_TEMP_SPEC
string DataTypeInfo< complex<r_4> >::getTypeName() { return(" complex<r_4> "); }
DECL_TEMP_SPEC
string DataTypeInfo< complex<r_8> >::getTypeName() { return(" complex<r_8> "); }
#ifdef SO_LDBLE128
DECL_TEMP_SPEC
string DataTypeInfo<r_16>::getTypeName() { return("r_16"); }
DECL_TEMP_SPEC
string DataTypeInfo< complex<r_16> >::getTypeName() { return(" complex<r_16> "); }
#endif
/*! 
  \ brief Return an identifier associated to T 
  uint_1 -> 1  , int_1 -> 2
  uint_2 -> 3  , int_2 -> 4
  uint_4 -> 5  , int_4 -> 6
  uint_8 -> 7  , int_8 -> 8
  r_4    -> 9  , r_8   -> 10
  complex<r_4> -> 11 , complex<r_8> -> 12
*/
template <class T>
int DataTypeInfo<T>::getTypeId() { return(0); }
DECL_TEMP_SPEC
int DataTypeInfo<uint_1>::getTypeId() { return(1); }
DECL_TEMP_SPEC
int DataTypeInfo<int_1>::getTypeId() { return(2); }
DECL_TEMP_SPEC
int DataTypeInfo<uint_2>::getTypeId() { return(3); }
DECL_TEMP_SPEC
int DataTypeInfo<int_2>::getTypeId() { return(4); }
DECL_TEMP_SPEC
int DataTypeInfo<uint_4>::getTypeId() { return(5); }
DECL_TEMP_SPEC
int DataTypeInfo<int_4>::getTypeId() { return(6); }
DECL_TEMP_SPEC
int DataTypeInfo<uint_8>::getTypeId() { return(7); }
DECL_TEMP_SPEC
int DataTypeInfo<int_8>::getTypeId() { return(8); }
DECL_TEMP_SPEC
int DataTypeInfo<r_4>::getTypeId() { return(9); }
DECL_TEMP_SPEC
int DataTypeInfo<r_8>::getTypeId() { return(10); }
DECL_TEMP_SPEC
int DataTypeInfo< complex<r_4> >::getTypeId() { return(11); }
DECL_TEMP_SPEC
int DataTypeInfo< complex<r_8> >::getTypeId() { return(12); }
#ifdef SO_LDBLE128
DECL_TEMP_SPEC
int DataTypeInfo<r_16>::getTypeId() { return(13); }
DECL_TEMP_SPEC
int DataTypeInfo< complex<r_16> >::getTypeId() { return(14); }
#endif

#endif


string DecodeGCCTypeName(string gcctype)
// seulement pour gcc/g++ qui ne code pas correctement
{
#if defined(__GNUG__)
  string type("");
  const char* str = gcctype.c_str();
  int lstr = (int) strlen(str);
  if(lstr<=0) return string("Linux Decoding error: lstr<=0");

  // Le dernier caractere donne le type
  lstr--;
  if     (str[lstr]=='v') type = "void";
  else if(str[lstr]=='b') type = "bool";
  else if(str[lstr]=='c') type = "char";
  else if(str[lstr]=='s') type = "short";
  else if(str[lstr]=='i') type = "int";
  else if(str[lstr]=='l') type = "long";
  else if(str[lstr]=='x') type = "long long";
  else if(str[lstr]=='f') type = "float";
  else if(str[lstr]=='d') type = "double";
  else if(str[lstr]=='r') type = "long double";
  else return string("Linux Decoding error: bad last char");
  if(lstr==0) return type;

  // Les caracteres precedents donnent: signed, unsigned, *, & ou const.
  // Mais si on a un Z, alors c'est un complexe (code ...t7complex1Z...)
  for(int i=0; i<(int)strlen(str); i++) {
    lstr--;
    if     (str[lstr]=='U') type = "unsigned " + type;
    else if(str[lstr]=='S') type = "signed " + type;
    else if(str[lstr]=='P') type = type + "*";
    else if(str[lstr]=='R') type = type + "&";
    else if(str[lstr]=='C') type = "const " + type;
    else if(str[lstr]=='Z') {  // Complexe
      type = "complex<" + type + ">";
      for(int j=0;j<(int)strlen(str);j++) {
        if     (str[j]=='P') type = type + "*";
        else if(str[j]=='R') type = type + "&";
        else if(str[j]=='C') type = "const " + type;
        else if(str[j]=='t') break;
        else return string("Linux Decoding error: bad char 1,2 for Z");
      }
      break;
    }
    else return string("Linux Decoding error: bad last-1,last-2 char");
    if(lstr==0) break;
  }

return type;
#else
return gcctype;
#endif
}


#ifdef __CXX_PRAGMA_TEMPLATES__
#pragma define_template DataTypeInfo<uint_1>
#pragma define_template DataTypeInfo<int_1>
#pragma define_template DataTypeInfo<uint_2>
#pragma define_template DataTypeInfo<int_2>
#pragma define_template DataTypeInfo<int_4>
#pragma define_template DataTypeInfo<int_8>
#pragma define_template DataTypeInfo<uint_4>
#pragma define_template DataTypeInfo<uint_8>
#pragma define_template DataTypeInfo<r_4>
#pragma define_template DataTypeInfo<r_8>
#pragma define_template DataTypeInfo< complex<r_4> >
#pragma define_template DataTypeInfo< complex<r_8> >
#ifdef SO_LDBLE128
#pragma define_template DataTypeInfo<r_16>
#pragma define_template DataTypeInfo< complex<r_16> >
#endif

#endif

#if defined(ANSI_TEMPLATES) || defined(GNU_TEMPLATES)
template class DataTypeInfo<uint_1>;
template class DataTypeInfo<int_1>;
template class DataTypeInfo<uint_2>;
template class DataTypeInfo<int_2>;
template class DataTypeInfo<int_4>;
template class DataTypeInfo<int_8>;
template class DataTypeInfo<uint_4>;
template class DataTypeInfo<uint_8>;
template class DataTypeInfo<r_4>;
template class DataTypeInfo<r_8>;
template class DataTypeInfo< complex<r_4> >;
template class DataTypeInfo< complex<r_8> >;
#ifdef SO_LDBLE128
template class DataTypeInfo<r_16>;
template class DataTypeInfo< complex<r_16> >;
#endif

#endif

} // FIN namespace SOPHYA 
