#ifndef HEPVis_SbHatching_h
#define HEPVis_SbHatching_h

/**
 *  SbHatching.h
 *  
 *
 *  Created by Laurent Garnier on Fri Jan 30 2004.
 * 
 * This class builds a list of hatching. 
 * It does nothing more than call
 * SbHatch and store results in a vector of SbVec3
 *
 */

#include <Inventor/SbLinear.h>
#include <vector>

#include <HEPVis/SbHatch.h>

class SbHatching : public SbHatch { 
public:
  SbHatching(){
    fHatchPointsList.clear();
    fCornerMinBB = SbVec3f(.0f,.0f,.0f);
    fCornerMaxBB = SbVec3f(.0f,.0f,.0f);
  }
  virtual ~SbHatching(){}
public:
  SbHatching(const SbHatching& aFrom)
  :fHatchPointsList(aFrom.fHatchPointsList)
  ,fHatchNumVerticesList(aFrom.fHatchNumVerticesList)
  ,fCornerMinBB(aFrom.fCornerMinBB)
  ,fCornerMaxBB(aFrom.fCornerMaxBB)
  {}
  SbHatching& operator=(const SbHatching& aFrom) {
    fHatchPointsList = aFrom.fHatchPointsList;
    fHatchNumVerticesList = aFrom.fHatchNumVerticesList;
    fCornerMinBB = aFrom.fCornerMinBB;
    fCornerMaxBB = aFrom.fCornerMaxBB;
    return *this;
  }
public:  
  /** create a new polyline hatched and append result in a point vector
  * Store nothing if the hatch can't be draw
  * return TRUE if the polyline had been computed, false otherwise
  */
  int addPolyline(SbVec3f* Coord,int aNumber){
    int res = checkPolyline(Coord,aNumber);
  
    if (res == TRUE) {    
      if (getStripWidth() ==0) {
        res = computePolyline(Coord,aNumber);}
      else
        res = computePolyline(Coord,aNumber);
      for (unsigned int i=0;i<getNoVertices();i++) {
        fHatchNumVerticesList.push_back(getVertices()[i]);
      }    
      if (res == TRUE){
        for (unsigned int i=0;i<getNoPoints();i++) {
          fHatchPointsList.push_back(getPoints()[i]);
  
          if (getPoints()[i].getValue()[0] <fCornerMinBB[0]) fCornerMinBB[0] = getPoints()[i].getValue()[0];
          if (getPoints()[i].getValue()[1] <fCornerMinBB[1]) fCornerMinBB[1] = getPoints()[i].getValue()[1];
          if (getPoints()[i].getValue()[2] <fCornerMinBB[2]) fCornerMinBB[2] = getPoints()[i].getValue()[2];
          
          if (getPoints()[i].getValue()[0] >fCornerMaxBB[0]) fCornerMaxBB[0] = getPoints()[i].getValue()[0];
          if (getPoints()[i].getValue()[1] >fCornerMaxBB[1]) fCornerMaxBB[1] = getPoints()[i].getValue()[1];
          if (getPoints()[i].getValue()[2] >fCornerMaxBB[2]) fCornerMaxBB[2] = getPoints()[i].getValue()[2];
        }    
      } else {
        printf("SbHatching::addPolyline : ERROR in SbHatching::computePolyline. Points (%d) :\n",aNumber);
        for (int i=0;i<aNumber;i++) {
           printf("SbHatching::addPolyline :  %d : %g %g %g\n",i,Coord[i][0],Coord[i][1],Coord[i][2]);
        }
      }
    }
    return res;
  }
  
  
  /** Get the list of points of all the hatch already computed
   * You should also get the hatchNumVerticeList to draw the hatch
   * For a hatch with a 0 thickness (by default), the hatchNumberVericeList will be a full of '2'
   */
  const std::vector<SbVec3f>& getHatchPointsList() const {
    return fHatchPointsList;
  }

  /** Get the number of vertices list.
   * For a hatch with a 0 thickness (by default), the hatchNumberVericeList will be a full of '2'
   */
  const std::vector<unsigned int>& getHatchNumVerticesList() const {
    return fHatchNumVerticesList;
  }
 
  /** Get the minimum corner of Bounding box for all hatch already computed */
  const SbVec3f& getCornerMinBB() {return fCornerMinBB;}
  
  /** Get the maximum corner of Bounding box for all hatch already computed */
  const SbVec3f& getCornerMaxBB() {return fCornerMaxBB;}
  
  /** Get the number of points in the list
   * Be careful with this function because it can return a set of non convex
   * polygone if you have a non convex polygone at beginning !So, when you want
   * to draw it, you have to use a tesselisation algorithm first
   */
  const unsigned int getNumPointsList() const {
    return fHatchPointsList.size();
  }

  /** Get the number of vertices */
  const unsigned int getNumVertices() const {
    return fHatchNumVerticesList.size();
  }

private:
  std::vector<SbVec3f> fHatchPointsList;
  std::vector<unsigned int> fHatchNumVerticesList;
  SbVec3f fCornerMinBB;
  SbVec3f fCornerMaxBB;
};

#endif
