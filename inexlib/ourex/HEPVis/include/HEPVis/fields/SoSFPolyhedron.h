#ifndef HEPVIS_SOSFPOLYHEDRON_H
#define HEPVIS_SOSFPOLYHEDRON_H

#include <Inventor/fields/SoSField.h>
#include <Inventor/fields/SoSubField.h>

#include <HEPVis/SbPolyhedron.h>

class SoSFPolyhedron : public SoSField {
  SO_SFIELD_HEADER(SoSFPolyhedron, SbPolyhedron, const SbPolyhedron &);
public:
  static void initClass(void);
private:
  static SbBool read_vec3d(SoInput*,SbVec3d&);
  static SbBool read_facet(SoInput*,SbFacet&);
  static void write_vec3d(SoOutput*,const SbVec3d&);
  static void write_facet(SoOutput*,const SbFacet&);
};

#endif
