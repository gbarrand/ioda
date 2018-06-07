#ifndef HEPVis_SbPrimitives_h
#define HEPVis_SbPrimitives_h

#include <HEPVis/SbPlottableThings.h>

#include <Inventor/SbString.h>
#include <Inventor/SbColor.h>

#include <string.h>

#define SbPlottableText_s     "SbPlottableText"

class SbPlottableText : public virtual SbPlottablePrimitive {
public: //SbPlottablePrimitive
  virtual void* cast(const char* aClass) const {
    if(!::strcmp(aClass,SbPlottableText_s)) {
      return (void*)static_cast<const SbPlottableText*>(this);
    } else if(!::strcmp(aClass,SbPlottablePrimitive_s)) {
      return (void*)static_cast<const SbPlottablePrimitive*>(this);
    } else {
      return 0;
    }
  }
public:
  SbPlottableText(const SbString& aTEXT,float aX,float aY,float aSIZE,float aANGLE = 0,char aJUST = 'L',const SbString& aFONT = "Hershey",float aSCALE = 1,SbBool aSMOOTHING = FALSE,SbBool aHINTING = FALSE)
    :fTEXT(aTEXT),fX(aX),fY(aY),fSIZE(aSIZE),fANGLE(aANGLE),fJUST(aJUST),fFONT(aFONT),fSCALE(aSCALE),fSMOOTHING(aSMOOTHING),fHINTING(aHINTING)
  {}
public:
  SbPlottableText(const SbPlottableText& aFrom)
  :fTEXT(aFrom.fTEXT),fX(aFrom.fX),fY(aFrom.fY),fSIZE(aFrom.fSIZE),fANGLE(aFrom.fANGLE),fJUST(aFrom.fJUST),fFONT(aFrom.fFONT),fSCALE(aFrom.fSCALE),fSMOOTHING(aFrom.fSMOOTHING),fHINTING(aFrom.fHINTING){}
  SbPlottableText& operator=(const SbPlottableText& aFrom) {
    fTEXT = aFrom.fTEXT;
    fX = aFrom.fX;
    fY = aFrom.fY;
    fSIZE = aFrom.fSIZE;
    fANGLE = aFrom.fANGLE;
    fJUST = aFrom.fJUST;
    fFONT = aFrom.fFONT;
    fSCALE = aFrom.fSCALE;
    fSMOOTHING = aFrom.fSMOOTHING;
    fHINTING = aFrom.fHINTING;
    return *this;
  }
public:
  SbString fTEXT;
  float fX;
  float fY;
  float fSIZE;
  float fANGLE;
  char fJUST;
  SbString fFONT;
  float fSCALE;
  SbBool fSMOOTHING;
  SbBool fHINTING;
};

#define SbPlottableBox_s     "SbPlottableBox"

class SbPlottableBox : public virtual SbPlottablePrimitive {
public: //SbPlottablePrimitive
  virtual void* cast(const char* aClass) const {
    if(!::strcmp(aClass,SbPlottableBox_s)) {
      return (void*)static_cast<const SbPlottableBox*>(this);
    } else if(!::strcmp(aClass,SbPlottablePrimitive_s)) {
      return (void*)static_cast<const SbPlottablePrimitive*>(this);
    } else {
      return 0;
    }
  }
public:
  enum FillAreaStyle {
    HOLLOW = 0,
    SOLID = 1,
    PATTERN = 2,
    HATCHED = 3
  };
public:
  SbPlottableBox(float aX1,float aY1,float aX2,float aY2,
                 FillAreaStyle aFAIS,const SbColor& aFACI,
                 int aFASI,
                 SbBool aBORD,const SbColor& aPLCI)
  :fX1(aX1),fY1(aY1),fX2(aX2),fY2(aY2)
  ,fFAIS(aFAIS),fFACI(aFACI),fFASI(aFASI),fBORD(aBORD),fPLCI(aPLCI)
  {}
public:
  SbPlottableBox(const SbPlottableBox& aFrom)
  :fX1(aFrom.fX1),fY1(aFrom.fY1)
  ,fX2(aFrom.fX2),fY2(aFrom.fY2)
  ,fFAIS(aFrom.fFAIS),fFACI(aFrom.fFACI),fFASI(aFrom.fFASI),fBORD(aFrom.fBORD),fPLCI(aFrom.fPLCI)
{}
  SbPlottableBox& operator=(const SbPlottableBox& aFrom) {
    fX1 = aFrom.fX1;
    fY1 = aFrom.fY1;
    fX2 = aFrom.fX2;
    fY2 = aFrom.fY2;
    fFAIS = aFrom.fFAIS;
    fFACI = aFrom.fFACI;
    fFASI = aFrom.fFASI;
    fBORD = aFrom.fBORD;
    fPLCI = aFrom.fPLCI;
    return *this;
  }
public:
  float fX1;
  float fY1;
  float fX2;
  float fY2;
  FillAreaStyle fFAIS;
  SbColor fFACI;
  int fFASI;
  SbBool fBORD;
  SbColor fPLCI;
};

#endif
