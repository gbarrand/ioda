// Classes SphereCoordSys , SpherePosition
//      G. Le Meur  2000
//      R. Ansari 2006 (documentation/commentaire) 
// LAL (Orsay) / IN2P3-CNRS  DAPNIA/SPP (Saclay) / CEA

// 04/01/00 : implantation de la persistance par classe deleguee - Guy Le Meur

#include "sopnamsp.h"
#include "spherepos.h"
#include "datatype.h"
#include <typeinfo>



//................. SphereCoordSys class .................

/*!
   \class SOPHYA::SphereCoordSys
   \ingroup SkyMap
   \brief  Class which describes the coordinate system used in spherical maps
   Current implementation (~ 2006 ) does NOT perform any coordinate transformation
   \sa SOPHYA::PixelMap
*/

SphereCoordSys::SphereCoordSys(){
  id_ = SphereCoordSys::NEUTRAL;
  description_ = "NEUTRAL SphereCoordSystem";
}
SphereCoordSys::SphereCoordSys(const SphereCoordSys& a )
{
  id_ = a.id_;
  description_ = a.description_;
}

SphereCoordSys::SphereCoordSys(int id, 
			       const string& description){
  id_ = id;
  description_ = description;
}

SphereCoordSys::~SphereCoordSys()
{
}
SphereCoordSys& SphereCoordSys::operator = (const SphereCoordSys& a)
{
  id_ = a.id_;
  description_ = a.description_;
  return *this;
}


////////////////////////////////////////////////////////////////
// -------------------------------------------------------------------------
//   Les objets delegues pour la gestion de persistance 
// -------------------------------------------------------------------------

FIO_SphereCoordSys::FIO_SphereCoordSys()
  {
    dobj=new SphereCoordSys;
    ownobj=true;
  }
FIO_SphereCoordSys::FIO_SphereCoordSys(string const & filename)
  {
    dobj=new SphereCoordSys;
    ownobj=true; 
    Read(filename);
  }
FIO_SphereCoordSys::FIO_SphereCoordSys(const SphereCoordSys & obj)
  {
    dobj = new SphereCoordSys(obj);
    ownobj=true; 
  }
FIO_SphereCoordSys::FIO_SphereCoordSys(SphereCoordSys * obj)
  {
    dobj = obj;
    ownobj=false; 
  }
FIO_SphereCoordSys::~FIO_SphereCoordSys()
  {
    if (ownobj && dobj) delete dobj;
  }
AnyDataObj* FIO_SphereCoordSys::DataObj()
  {
    return(dobj);
  }

void  FIO_SphereCoordSys::SetDataObj(AnyDataObj & o) 
{
  SphereCoordSys * po = dynamic_cast<SphereCoordSys *>(&o);
  if (po == NULL) {
    char buff[160];
    sprintf(buff,"FIO_SphereCoordSys::SetDataObj(%s) - Object type  error ! ",
	    typeid(o).name());
    throw TypeMismatchExc(PExcLongMessage(buff));    
  }

  if (ownobj && dobj) delete dobj;  
  dobj = po; ownobj = false;
}


void  FIO_SphereCoordSys::ReadSelf(PInPersist& is)
  {
uint_8 itab[3];
 string description;
is.Get(itab, 3);
is.GetStr(description);
if (dobj == NULL) dobj = new SphereCoordSys(itab[1], description);
 else *dobj= SphereCoordSys(itab[1], description);
  }           
void       FIO_SphereCoordSys::WriteSelf(POutPersist& os) const
{
if (dobj == NULL)   return;  // Attention - $CHECK$ Guy 04/01/00
//  On ecrit 3 uint_8 
//  0 : Numero de version,  1 : Id,  2  reserve a l
uint_8 itab[3];
itab[0] = 1;
itab[1] = dobj->Id();
itab[2] = 0;
os.Put(itab, 3);
os.PutStr(dobj->description());
} 



//................. SpherePosition class .................
/*!
   \class SOPHYA::SpherePosition
   \ingroup SkyMap
   \brief  Class to define a (angular) position on a sphere, combined a coordinate system
   Current implementation (~ 2006 ) does NOT perform any coordinate transformation
   \sa SOPHYA::PixelMap
*/

SpherePosition::SpherePosition() 
        : UnitVector()
{
  cs_ = new SphereCoordSys;
}

SpherePosition::SpherePosition(double theta, double phi, SphereCoordSys* cs)
        : UnitVector(theta, phi)
{
if (cs == NULL) cs_ = new SphereCoordSys;
else cs_ = cs;
}

SpherePosition::SpherePosition(double x, double y, double z, SphereCoordSys* cs)
        : UnitVector(x, y, z)
{
if (cs == NULL) cs_ = new SphereCoordSys;
else cs_ = cs;
}

SpherePosition::~SpherePosition()
{
if (cs_) delete cs_;
}

UnitVector SpherePosition::Transform(const SphereCoordSys& /*cs*/) const
{
return(*this);
}

double SpherePosition::Separation(const SpherePosition& gamma) const 
{
return(this->SepAngle(gamma));
}
