#ifndef HEPVis_SoTorus_h
#define HEPVis_SoTorus_h

#include <math.h>
#include <Inventor/SbLinear.h>
#include <Inventor/nodes/SoNurbsSurface.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoCoordinate4.h>
#include <Inventor/nodes/SoCoordinate3.h>
#include <Inventor/nodes/SoDrawStyle.h>
#include <Inventor/nodes/SoPointSet.h>
#include <Inventor/fields/SoSFFloat.h>
#include <Inventor/fields/SoSFVec3f.h>
#include <Inventor/fields/SoSFNode.h>
#include <Inventor/nodes/SoTranslation.h>
#include <Inventor/misc/SoChildList.h>
#include <Inventor/SbBox.h>

#define SO_TORUS_U_DIM 7
#define SO_TORUS_V_DIM 7
#define SO_TORUS_U_ORDER 3
#define SO_TORUS_V_ORDER 3

//adding fields 
#ifdef _WIN32
#include <SoWinEnterScope.h>
#endif


// Defining Class
#ifdef _WIN32
#include <SoWinLeaveScope.h>
#endif

/*! \class SoTorus SoTorus.h HEPVis/SoTorus.h
 *  \brief Provides a Torus shape based on a NURB surface
 *
 *  \author G. Alverson
 *  \date June 1998
 */

class SoTorus : public SoShape {
  SO_NODE_HEADER(SoTorus);
public:
// Fields
  SoSFFloat rMinor; //!< radius of minor axis
  SoSFFloat rMajor; //!< radius of major axis
  SoSFVec3f center; //!< position of the center
  SoSFNode alternateRep; //!< an alternate rep for transportability
public:
#if defined(_WIN32) && defined(BUILDING_DLL)
  // When building the node as a DLL under Win32 we must explicitly
  // declare this entry point as visible outside the DLL.  The macro
  // BUILDING_DLL is defined in the node's source file.
  _declspec(dllexport)
#endif
    SoTorus(); //!< constructor
  virtual void generateAlternateRep(SoAction*); //!< generate the alternate rep
  virtual void clearAlternateRep(); //!< remove the alternate rep
  static void initClass(); //!< required for initialization of the class
protected:
  static const int NPTS; //!< number of points describing the surface
  static const float u_knot_vals[SO_TORUS_U_DIM + SO_TORUS_U_ORDER];
  static const float v_knot_vals[SO_TORUS_V_DIM + SO_TORUS_V_ORDER];
  virtual SoChildList *getChildren() const;
  virtual void computeBBox(SoAction *action, SbBox3f &box, SbVec3f &TheCenter );
  virtual void generatePrimitives(SoAction *action);
  float rMinorLocal;
  float rMajorLocal;
  SbVec3f centerLocal;
  virtual ~SoTorus();
private:
  void updateChildren();
  void generateChildren();
  SoChildList *children;
};
#ifdef _WIN32
#include <SoWinEnterScope.h>
#endif

#endif



