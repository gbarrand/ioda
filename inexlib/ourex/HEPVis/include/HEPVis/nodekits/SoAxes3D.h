#ifndef HEPVis_SoAxes3D_h
#define HEPVis_SoAxes3D_h

/*
 *  SoAxes3D.h
 *  
 *
 *  Created by Laurent Garnier on 06/01/2004.
 *
 */

// Inheritance :
#include <Inventor/nodekits/SoBaseKit.h>

#include <Inventor/fields/SoSFUShort.h>
#include <HEPVis/nodekits/SoAxis.h>

class SoAxes3D : public SoBaseKit {
  SO_KIT_HEADER(SoAxes3D);
  
  SO_KIT_CATALOG_ENTRY_HEADER(topSeparator); 
  SO_KIT_CATALOG_ENTRY_HEADER(xAxisSeparator);
  SO_KIT_CATALOG_ENTRY_HEADER(xAxisTransform);
  SO_KIT_CATALOG_ENTRY_HEADER(xAxis);
  SO_KIT_CATALOG_ENTRY_HEADER(yAxisSeparator);
  SO_KIT_CATALOG_ENTRY_HEADER(yAxisTransform);
  SO_KIT_CATALOG_ENTRY_HEADER(yAxis);
  SO_KIT_CATALOG_ENTRY_HEADER(zAxisSeparator);
  SO_KIT_CATALOG_ENTRY_HEADER(zAxisTransform);
  SO_KIT_CATALOG_ENTRY_HEADER(zAxis);
  SO_KIT_CATALOG_ENTRY_HEADER(grid);
  SO_KIT_CATALOG_ENTRY_HEADER(wall);
public :
  SoSFColor gridColor;  
  SoSFFloat gridTransparency; 
  SoSFUShort gridLinePattern;  
  SoSFBool gridVisible;  

  SoSFColor wallColor; 
  SoSFFloat wallTransparency; 
  SoSFFloat wallOffset; 
  SoSFBool wallVisible;  
public:
  static void initClass();
public: /*SoEXTENDER*/
  virtual void doAction(SoAction*);
  virtual void callback(SoCallbackAction*);
  virtual void GLRender(SoGLRenderAction*);
  virtual void rayPick(SoRayPickAction*);
  virtual void pick(SoPickAction*);
  virtual void getBoundingBox(SoGetBoundingBoxAction*);
public:
  SoAxes3D();
  SoAxis* getXAxis();
  SoAxis* getYAxis();
  SoAxis* getZAxis();
protected:  
  virtual ~SoAxes3D();  
private :
  void addGridToAxis();
  void addWallToAxis();
  void buildWall(const SbVec3f&,const SbVec3f&,const SbVec3f&,
                 float,float,SoSeparator*);
private:
  SbColor fOld_gridColor;  
  float fOld_gridTransparency; 
  unsigned short fOld_gridLinePattern;  
  SbBool fOld_gridVisible;  

  SbColor fOld_wallColor; 
  float fOld_wallTransparency; 
  float fOld_wallOffset; 
  SbBool fOld_wallVisible;  
};

#endif
