#ifndef HEPVis_SoSplineCurve_h
#define HEPVis_SoSplineCurve_h

// Inheritance :
#include <Inventor/nodes/SoShape.h>

#include <Inventor/fields/SoMFVec3f.h>
#include <Inventor/fields/SoSFNode.h>

class SoChildList;
class SbBox3f;
class SbVec3f;
class SbVec4f;

#ifdef _WIN32
#include <SoWinEnterScope.h>
#endif

#ifdef _WIN32
#include <SoWinLeaveScope.h>
#endif

/*! \class SoSplineCurve SoSplineCurve.h
 *
 *  \brief Provides a spline-based curve class which requires the
 *   knowledge of the trajectory and its slope at two separate points.
 *   This code is borrowed from the IGUANA SoSplineTrack written by 
 *   G.Alverson. July 1999. It had been renamed SoSplineCurve to 
 *   follow the OpenInventor conventions (for example SoNurbsCurve).
 *
 *  \author G.Barrand
 *  \date July 2004
 * 
 */

class SoSplineCurve : public SoShape
{
  SO_NODE_HEADER(SoSplineCurve);
public:
  SoMFVec3f points;
  SoMFVec3f tangents;
  SoSFNode  alternateRep;
public:
#if defined(_WIN32) && defined(BUILDING_DLL)
  // When building the node as a DLL under Win32 we must explicitly
  // declare this entry point as visible outside the DLL.  The macro
  // BUILDING_DLL is defined in the node's source file.
  _declspec(dllexport)
#endif
  SoSplineCurve();
  virtual void generateAlternateRep(SoAction*);
  virtual void clearAlternateRep();
  static void initClass();
protected:
  virtual ~SoSplineCurve();
  virtual void computeBBox(SoAction*,SbBox3f&,SbVec3f&);
  virtual void generatePrimitives(SoAction*);
  virtual SoChildList* getChildren() const; //!< inherited from SoShape
  virtual void doAction(SoAction*);
private:
  static float rootFunction(float x, void *data);
private:
  void updateChildren();
  void generateChildren();
  virtual SbVec4f midpoint(SbVec3f,SbVec3f,SbVec3f,SbVec3f);
  float findRoot(float (* func)( float, void * ),
                 float,float,float,void*,SbBool&);
private:
  static const int NORDER; // order of NurbsCurve
private:
  SoChildList* children;
  float fParamA; // input parameters to rootFunction: a,c,d
  float fParamC;
  float fParamD;
  int fParamSign;
};

#ifdef _WIN32
#include <SoWinEnterScope.h>
#endif

#endif
