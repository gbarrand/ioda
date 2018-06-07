#ifndef HEPVis_SoHatching_h
#define HEPVis_SoHatching_h

/**
 *  Sohatching.h
 *  
 *
 *  Created by Laurent Garnier on Fri Dec 14 2004.
 *
 */

#include <Inventor/nodes/SoShape.h>
#include <HEPVis/SbHatching.h>
#include <Inventor/SbLinear.h>
#include <Inventor/fields/SoSFFloat.h>

class SoHatching : public SoShape { 
  SO_NODE_HEADER(SoHatching);  

 public :
  /**
  *   Absloute distance between two hatch in the polyline plan
  */
   SoSFFloat spacing;

 /**
  *  The angle (given in radians) is the one between the first
  *  line (point 1-point 0) and the hatch lines, in the polyline plan
  *  Given in the direct axis ((point1-point0),(lastPoint-point0),normalPlanVec)
  *  The angle in compute only one time for the first polyline.
  *  Changes on angle value for others polyline will not take effect.This is to
  *  perform correct hatching between the polylines 
  */
 SoSFFloat angle;
 
 /**
  * between 0-1. This value set the offset of the hatch.0 meen that the hatch
  * will touch the first point of the polyline, and 1 meen that first hatch will be draw
  * at a 'shift' distance to first point
  */
 SoSFFloat offset;

  /** strip with size : set to 0 by default
   * between 0 and 1.0 means no strip, 0.5 means strip size is half of distance between two hatches
   */
 SoSFFloat stripWidth;

 
 private:
 /**
  * object to contains a list a hatch
  */
  SbHatching fHatching;

  SbVec3f cornerMinBB;
  SbVec3f cornerMaxBB;
  
 protected :
  virtual ~SoHatching();
  void generatePrimitives(SoAction* aAction); 
  void computeBBox(SoAction*, SbBox3f& box, SbVec3f& center);
  
 public:
  SoHatching();
  static void initClass();
  
  /**
   * Call a SbHatching object to compute the hatching
   * return true if all is ok, false otherwise
   */
  int addPolyline(SbVec3f* Coord,int);
};

#endif
