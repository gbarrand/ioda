#ifndef HEPVis_SbHatch_h
#define HEPVis_SbHatch_h

/**
 *  SbHatch.h
 *  
 * SbHatch is a class to draw Hatch in a 3D polyline plane
 * A hatch is caracterise by a direction (dirAngle), a spacing to get 
 * second hatch,
 * an offset, and a stripWidth :
 * - offset value : between 0-1, This value set the offset of
 *   the hatch.0 meen that the hatch  will touch the first point
 *   of the polyline, and 1 meen that first hatch will be draw
 *   at a 'spacing' distance to first point
 * - offsetVec : the 3D point from where a hatch had to pass. This is 
 *   very usefull to have hach continuing in different polygones
 *
 * The computePolyline() method<br> 
 * By default:
 * - spacing = .1;
 * - dirAngle = PI/4;
 * - offsetValue = 0;
 * - stripWidth=0.0;
 * 
 * A way to get all points and vertices and to draw them can be :
 * <pre>
 *  iindex =0;
 *  icoord =0;
 *  for (unsigned int a=0;a<sbHatch.getNoVertices();a++) {
 *    for (unsigned int b=0;b<sbHatch.getNoVertices()[a];b++) {
 *      coordinate3->point.set1Value(icoord,sbHatch.getPoints()[icoord]);
 *      indexedFaceSet->coordIndex.set1Value(iindex,icoord);
 *      iindex++;
 *      icoord ++;
 *    }
 *    indexedFaceSet->coordIndex.set1Value(iindex,SO_END_LINE_INDEX);
 *    iindex++;
 *  }
 * </pre>
 *
 * @author Laurent Garnier
 * Creation : on Fri Jan 05 2004
 * Last update : 9 April 2004
 *
 */

#include <Inventor/SbLinear.h>

#include <vector>

class SbHatch {
public:
  SbHatch();
  virtual ~SbHatch();
  
  /**
  * draw the hatch into the polyline bounding box given in argument
  * You have to get all compute points by the getPoints() method
  * Number of points can be get by getNoPoints()
  * The  number of vertices in the return polyline can be get by getNoVertices()
  * and vertice table by getVertices
  * @return FALSE if :
  *    - All points are not in the same plan
  *    - There is a precision error on one or more point
  */
  SbBool computePolyline (SbVec3f* listPoints,unsigned int number);


  /**
  * test if the polygone given is correct for hatching
  * @return FALSE if :
  *    - All points are not in the same plan
  *    - Number of points <3
  *    - Offset point is not in the same plan
  *    - There is less than three different points
  *    - The vector from point[0],point[1] is colinear to point[0],lastPoint
  */
  SbBool checkPolyline(SbVec3f* listPoints,unsigned int number);

  /** set the spacing for this hatch */
  void  setSpacing(float a);
  /** set the Direction angle for the hatch in radians */
  void  setAngle(float a);
  /** set the offset value for this hatch */
  void  setOffset(float a);
  /** set the offset Point for this hatch */
  void  setOffsetPoint(SbVec3f a);
  /** set the precision factor for computing (0.0001 is default) */
  void  setPrecisionFactor(float a);
  /** set the strip width value(0 is default and means no strip) */
  void  setStripWidth(float a);
  /** get the spacing */
  float getSpacing()const ;
  /** get the direction angle in radians*/
  float getAngle() const ;
  /** get the offset value */
  float getOffset() const ;
  /** get the Offset point */
  const SbVec3f& getOffsetPoint();
  /** get the precision factor */
  float getPrecisionFactor() const ;
  /** get the stripWidth value */
  float getStripWidth() const ;
 /** get the normal vector value */
  const SbVec3f& getNormal() ;

  /** get the number of points compute for this hatch */
  unsigned int getNoPoints() const ;
  /** vector of compute points<br>
   * Be careful with this function because it can return a set of non convex
   * polygone if you have a non convex polygone at beginning !So, when you want
   * to draw it, you have to use a tesselisation algorithm first
   */
  const std::vector<SbVec3f>& getPoints();
  /** get the number of vertices compute for this hatch */
  unsigned int getNoVertices() const ;
  /** vector of numbers of vertices */
  const std::vector<unsigned int>& getVertices();
 

private:
  
  /**
  * draw the hatch into the polyline bounding box given in argument
  * @return FALSE if :
  *    - All points are not in the same plan
  *    - There is a precision error on one or more point
  */
  SbBool computeSinglePolyline (SbVec3f* listPoints,unsigned int number);

  
  /**
  * Compute a vector system equation aA+bB=C
  * return SbVec2f(0,0) if there is an error
  * set the resolveResult variable to the error code :
  * COLINEAR if A and B are 
  * PRECISION_ERROR if there is a lack of precision in computing
  * Z_ERROR if there s no solution for Z
  * UNDEFINED never throw
  * return a SbVec2f  for result. a is 'x' value and b is 'y' if it is correct
  */
  SbVec2f resolveSystem(const SbVec3f& A,const SbVec3f& B,const SbVec3f& C);


private:
  /** normal vector for the current polyline */
  SbVec3f fNormal;

  /** Spacing vector between two hatch */
  float fShift; // Absloute distance between two hatch in the polyline plan  */
  
  /** The angle (given in radians) is the one between the first
   * line (point 1-point0) and the hatch lines, in the polyline plan.
   * Given in the direct axis ((point1-point0),(lastPoint-point0),
   * normalPlanVec). The angle in compute only one time for the first polyline.
   * Changes on angle value for others polyline will not take effect.This is to
   * perform correct hatching between the polylines
   */
  float fDirAngle;
  
  /** between 0-1. This value set the offset of the hatch.0 meen 
  * that the hatch will touch the first point of the polyline, 
  * and 1 meen that first hatch will be draw
  * at a 'spacing' distance to first point
  */
  float fOffsetValue;

  /** first point of the hatch. 
   * offset = firstPolylinePoint+ShiftVec*offsetValue 
   */
  SbVec3f fOffset;

  /** Orientation vector for the hatch  */
  SbVec3f fShiftVec;

  /** factor for compute error between two points */
  float fPrecisionFactor;
  
  /** hatch direction Vector */
  SbVec3f  fDirVec;

  /** strip with size : set to 0 by default
   * between 0 and 1.0 means no strip, 0.5 means strip size is 
   * half of distance between two hatches
   */
  float fStripWidth;

  /** vector list of points */
  std::vector<SbVec3f> fPoints;

  /** vector vertices number */
  std::vector<unsigned int> fVertices;
 
  /** conflict line table */
  std::vector< std::vector<int> > fConflictNumHatchLineTab;

  /** hatchShiftToMatchPointVec tab*/
  std::vector<float> fHatchShiftToMatchPointVec;

  /** first hatch number to draw */
  int fFirstNumHatch;

  /**number of hatch to draw */
  unsigned int fNumberHatchToDraw;

  unsigned int  fFirstPolyline;
  enum ResolveErrors{
    OK = 0,
    COLINEAR,
    Z_ERROR,
    PRECISION_ERROR,
    UNDEFINED
  };
  ResolveErrors fResolveResult;
};

#endif
