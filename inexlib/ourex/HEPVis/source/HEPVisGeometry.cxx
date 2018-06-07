#include <HEPVis/Geometry.h>

extern "C" {
  void HEPVisGeometry_initClasses();
}

//////////////////////////////////////////////////////////////////////////////
void HEPVisGeometry_initClasses(
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  if(SoSFPolyhedron::getClassTypeId()!=SoType::badType()) return; //done.

  SoSFPolyhedron::initClass();
  SoPolyhedron::initClass();
  SoArrow::initClass();
  SoEllipsoid::initClass();
  SoEllipsoidSegment::initClass();
  SoEllipticalPrism::initClass();
  SoBox::initClass();
  SoCons::initClass();
  SoTrap::initClass();
  SoTrd::initClass();
  SoTubs::initClass();
  SoTorus::initClass();
  SoWedge::initClass();
  SoPcon::initClass();
  SoPcons::initClass();
  SoPolyVol::initClass();
  So3DErrorBar::initClass();
  SoCoordinateAxis::initClass();
  So2DArrow::initClass();
  SoCircleArc::initClass();
  SoDisk::initClass();
  SoPolygon::initClass();
  SoQuad::initClass();
  SoRing::initClass();
  SoTriangle::initClass();
}

