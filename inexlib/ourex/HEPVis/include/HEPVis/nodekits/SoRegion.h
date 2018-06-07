#ifndef HEPVis_SoRegion_h
#define HEPVis_SoRegion_h

#include <Inventor/errors/SoDebugError.h>
#include <Inventor/nodekits/SoBaseKit.h>
#include <Inventor/fields/SoSFVec2f.h>
#include <Inventor/fields/SoSFBool.h>
#include <Inventor/fields/SoSFColor.h>
#include <Inventor/fields/SoSFShort.h>
#include <Inventor/fields/SoSFEnum.h>
#include <Inventor/SbLinear.h>

#include <list>
#include <cstring> //::strcmp

class SoViewportRegion;
class SoSeparator;
class SoCamera;
class SoGLRenderAction;
class SoStyleCache;

class RegionCamera {
public:
  RegionCamera();
  RegionCamera(const SoCamera&);
  RegionCamera(const RegionCamera&);
  RegionCamera& operator=(const RegionCamera&);
  virtual ~RegionCamera();
  void restore(SoCamera&);
private:
  int fViewportMapping;
  SbVec3f fPosition;
  SbRotation fOrientation;
  float fAspectRatio;
  float fNearDistance;
  float fFarDistance;
  float fFocalDistance;
  float fHeight;
};

class SbDoIt {
public:
  virtual ~SbDoIt() {}
public:
  virtual void* cast(const SbString&) const = 0;
};

class SbAddNode : public virtual SbDoIt {
public: //SbDoIt
  virtual void* cast(const SbString& aClass) const {
    if(!::strcmp(aClass.getString(),"SbAddNode")) return (SbAddNode*)this;
    else return 0;
  }
public:
  SbAddNode(SoNode* aNode,const SbString& aPlacement)
  :fNode(aNode),fPlacement(aPlacement)
  {}
  virtual ~SbAddNode(){if(fNode) fNode->unref();}
private: 
  //NOTE : doIt(SbAddNode(...)) don't compile if cp-cstor private.
  //       You have to do :
  //         SbAddNode sb(...);
  //         doIt(sb);
  SbAddNode(const SbAddNode&){}
  SbAddNode& operator=(const SbAddNode&){return *this;}
public:
  SoNode* getNode() const {return fNode;}
  const SbString& getPlacement() const {return fPlacement;}
  void reset() {fNode = 0;} //to be ablet to disconnect dstor unref.
private:
  SoNode* fNode;
  SbString fPlacement;
};

class SoRegion : public SoBaseKit {
  SO_KIT_HEADER(SoRegion);
  SO_KIT_CATALOG_ENTRY_HEADER(viewportRegion);
public:
  SoSFBool visible;
  SoSFBool verbose;
  SoSFColor color;
  SoSFVec2f origin; // In percentage of window size.
  SoSFVec2f size; // In percentage of window size.
  SoSFShort horizontalMargin; //In pixels
  SoSFShort verticalMargin; //In pixels

  // constaint fields (not used by the nodekit itself) :
  enum Unit {
    PERCENT,
    AXIS
  };
  SoSFEnum originUnit;

  // Placement relative to origin.
  enum HorizontalJustification {
    LEFT   = 0x00,
    CENTER = 0x01,
    RIGHT  = 0x02
  };
  enum VerticalJustification {
    BOTTOM = 0x00,
    MIDDLE = 0x01,
    TOP    = 0x02
  };
  SoSFEnum horizontalJustification;
  SoSFEnum verticalJustification;
public:
  SoRegion();
  SoViewportRegion* getViewportRegion() const;
  void pushCamera(); 
  void popCamera(); 
  void clearCameraStack(); 
  void viewRectangle(const SbVec2s&,const SbVec2s&,const SbVec2s&); 
  SoNode* findPart(const SbName&);
  SoNode* findNode(const SbString&,const SbString&);
  SoStyleCache* styleCache() const;
  void clearStyleCache();
  void setPAW_Placement(int,int,int);
  void getPAW_Placement(int&,int&,int&) const;
  void raiseUpdateFlag();
public:
  virtual void updateChildren();
  virtual SoSeparator* getTopSeparator() const;
  virtual SoCamera* getCamera() const;
  virtual void clear(const SbString& = "");
  virtual void refresh();
  virtual void resetCamera();
  virtual void setCamera(SoCamera*);
  virtual SbBool doIt(const SbDoIt&);
  virtual void undo();
  virtual void resetUndo();
  virtual SbBool connectFrom(SoRegion*);
  virtual SoNode* getNodeOfInterest(const SbString& = "") const;
  virtual void placeInPage(float,float,float,float,float,float,float,float);

  virtual SbBool setNode(const SbString&,const SbString&,const SbString&); 
  virtual void setPosition(float,float);
  virtual void setSize(float,float); 
  virtual SbBool readScene(const SbString& file,
                           const SbString& format,
                           const SbString& placement = "");
  virtual SoNode* getWriteNode(const SbString& = "") const;
public:
  static void initClass();
public: /*SoEXTENDER*/
  virtual void GLRender(SoGLRenderAction*);
protected:
  virtual ~SoRegion();
  SoSeparator* getAlternateRep();
private:
  void center(const SbVec2f&,float); 
protected:
  SbBool fUpdate;
  SoStyleCache* fStyleCache;
private:
  std::list<RegionCamera> fCameraStack;
  int fPAW_Rows,fPAW_Cols,fPAW_Index;
};

#endif
