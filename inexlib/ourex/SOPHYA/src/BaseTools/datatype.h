// This may look like C code, but it is really -*- C++ -*-
// Classe pour nom, taille, ... de types de donnees
//                         C.Magneville         10/2000
// LAL (Orsay) / IN2P3-CNRS  DAPNIA/SPP (Saclay) / CEA

#ifndef DATATYPE_H
#define DATATYPE_H

#include "machdefs.h"

#include <string>

namespace SOPHYA {

//! class returning type name for usual numrical types (int_2,int_4,r_4,r_8 ...)
template <class T>
class DataTypeInfo {
public:
  static string getTypeName();
  static int    getTypeId();
  DataTypeInfo();
};

string DecodeGCCTypeName(string gcctype);

} // Fin du namespace

#endif
