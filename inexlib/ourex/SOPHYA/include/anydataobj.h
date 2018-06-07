// This may look like C code, but it is really -*- C++ -*-
// Ancetre commun de tous les objets pour rtti
//                          R.Ansari          04/99
// LAL (Orsay) / IN2P3-CNRS  DAPNIA/SPP (Saclay) / CEA
#ifndef ANYDATAOBJ_H_SEEN
#define ANYDATAOBJ_H_SEEN

#include "machdefs.h"

namespace SOPHYA {

//! Ancestor class for all data objects (for RTTI).

class AnyDataObj {
public:
  AnyDataObj() { } 
  virtual ~AnyDataObj() { }

//! Return a process wide unique identifier 
  static uint_8 getUniqueId();
};

}

#endif
