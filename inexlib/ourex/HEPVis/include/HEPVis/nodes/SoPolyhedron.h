#ifndef HEPVis_SoPolyhedron_h
#define HEPVis_SoPolyhedron_h

// Inheritance :
#include <Inventor/nodes/SoShape.h>

#include <Inventor/fields/SoSFBool.h>
#include <Inventor/fields/SoSFNode.h>
#include <Inventor/fields/SoSFInt32.h>

#include <HEPVis/fields/SoSFPolyhedron.h>

#include <HEPVis/SbStyles.h>

class SbPolyhedron;

/**
 *  SoPolyhedron is an Inventor encapsulation of the HepPolyedron
 * class written by E.Chernyaev. In particular SoPolyhedron permits
 * to represent boolean operations over solids.
 *  To avoid clashes with other libraries (Geant4) where the HepPolyhedron
 * classes may be found, the HepPolyhedron (through usage of CPP macros)
 * had been renamed SbPolyhedron in the HEPVis lib.
 *  The solids are modeled with SbPolyedron objects. The SbPolyhedron
 * permits to produce a new SbPolyhedron according to the boolean 
 * operation done on them. The resulting SbPolyhedron  is then given
 * to an SoPolyhedron for rendering.
 *  Note that a boolean operation could be rendered in wire frame
 * by drawing the contour of the resulting solid (not by drawing
 * the wire frame of a triangulation).
 *  See the applications/Polyhedron example.
 */

class SoPolyhedron : public SoShape {
  SO_NODE_HEADER(SoPolyhedron);
public:
  SoSFBool solid;
  SoSFBool reducedWireFrame;
  SoSFInt32 projection;
  SoSFPolyhedron polyhedron;
  SoSFNode alternateRep;
public:
  SoPolyhedron();
  SoPolyhedron(const SbPolyhedron&);
  SoPolyhedron(const SbPolyhedron*);
  SoPolyhedron(const SbPolyhedron&,SbProjectionType);
  virtual void generateAlternateRep(SoAction*);
  virtual void clearAlternateRep();
public:
  static void initClass();
protected:
  virtual void computeBBox(SoAction*,SbBox3f&,SbVec3f&);
  virtual void generatePrimitives(SoAction*);
  virtual void doAction(SoAction*);
  virtual void write(SoWriteAction*);
  virtual void callback(SoCallbackAction*);
protected:
  virtual ~SoPolyhedron();
};

#endif
