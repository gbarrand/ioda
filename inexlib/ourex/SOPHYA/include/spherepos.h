// This may look like C code, but it is really -*- C++ -*-
#ifndef SPHEREPOS_H_SEEN
#define SPHEREPOS_H_SEEN

#include "machdefs.h"
#include "ppersist.h"
#include <string>
#include "unitvector.h"
#include "anydataobj.h"


namespace SOPHYA {

 
class SphereCoordSys : public  AnyDataObj {
public:
  
  enum SphereCoordSysIds { NEUTRAL =0, 
                           ROTATION =1,
                           OTHER = 0xFFFF
  };

  SphereCoordSys();
  SphereCoordSys(const SphereCoordSys& a);
                 SphereCoordSys(int id, const string& description);
  virtual        ~SphereCoordSys();

  virtual int    Id() const {return id_; }
  virtual string description() const {return description_; }
  SphereCoordSys& operator = (const SphereCoordSys& a);


private:
  int id_;
  string description_;
};

// Classe pour la gestion de persistance

class FIO_SphereCoordSys : public  PPersist  {
public:
  FIO_SphereCoordSys();
  FIO_SphereCoordSys(string const & filename);
  FIO_SphereCoordSys(const SphereCoordSys & obj);
  FIO_SphereCoordSys(SphereCoordSys * obj);
  virtual ~FIO_SphereCoordSys();
  virtual AnyDataObj* DataObj();
  virtual   void        SetDataObj(AnyDataObj & o);
  inline operator SphereCoordSys() { return(*dobj); }
protected :
  virtual void       ReadSelf(PInPersist&);
  virtual void       WriteSelf(POutPersist&) const;  
  SphereCoordSys * dobj;
  bool ownobj;
};


class SpherePosition : public UnitVector {
public:
  
                 SpherePosition();
// cs should be created by new SphereCoordSys...
// It will be deleted by the destructor
                 SpherePosition(double theta, double phi, SphereCoordSys* cs = NULL);
                 SpherePosition(double x, double y, double z, SphereCoordSys* cs = NULL);
                 SpherePosition(const UnitVector& v, SphereCoordSys* cs = NULL);
  virtual        ~SpherePosition();

  inline double  phi() const {return Phi(); }
  inline double  theta() const { return(Theta()); }
  inline SphereCoordSys *   GetCoordSys() const { return(cs_); } 
  virtual UnitVector Transform(const SphereCoordSys& cs) const ;

  virtual double     Separation(const SpherePosition& gamma) const;

protected:
  SphereCoordSys* cs_;
};

} // Fin du namespace

#endif
