#ifndef HEPVis_SoDragPoint2D_h
#define HEPVis_SoDragPoint2D_h

//#include <Inventor/draggers/SoDragPointDragger.h>
#include <Inventor/draggers/SoTranslate2Dragger.h>

// Inventor :
#include <Inventor/nodes/SoCoordinate3.h>

//class SoDragPoint2D : public SoDragPointDragger {
class SoDragPoint2D : public SoTranslate2Dragger {
public:
  virtual void updateCoordinate3(){}
public:
  SoDragPoint2D(SoCoordinate3& aCoord,int aIndex,float aScale)
  :fCoord(aCoord),fIndex(NotFound()),fScale(aScale){
    //NOTE : we assume that aCoord contains a closed polyline.
    fCoord.ref();
    int num = fCoord.point.getNum();
    if((aIndex>=0)&&(aIndex<(num-1))) {
      fIndex = aIndex;  
      fStart = fCoord.point[aIndex];
      SoDragger::addMotionCallback(motion_dragger_CB);
    }
  }
protected:
  virtual ~SoDragPoint2D(){
    fCoord.unref();
    if(fIndex!=NotFound()) {
      SoDragger::removeMotionCallback(motion_dragger_CB);
    }
  }
private:
  SoDragPoint2D(const SoDragPoint2D& aFrom):fCoord(aFrom.fCoord){}
  SoDragPoint2D& operator=(const SoDragPoint2D&){return *this;}

public:
  const SbVec3f& point() const {return fCoord.point[fIndex];}
  int index() const {return fIndex;}
  SoCoordinate3& coordinate3() const {return fCoord;}
private:
  static void motion_dragger_CB(void*,SoDragger* aDragger) {
    SoDragPoint2D* This = (SoDragPoint2D*)aDragger;
    const SbVec3f& v = This->translation.getValue();
    //printf("debug : motion_dragger_CB : %g %g %g\n",v[0],v[1],v[2]);
    This->fCoord.point.set1Value(This->fIndex,This->fStart+v*This->fScale);
    if(This->fIndex==0) {
      int num = This->fCoord.point.getNum();
      This->fCoord.point.set1Value(num-1,This->fStart+v*This->fScale);
    }
    This->updateCoordinate3();
  }
private:
  static int NotFound() {return (-1);}
private:
  SoCoordinate3& fCoord;
  int fIndex;
  float fScale;
  SbVec3f fStart;
};

#endif
