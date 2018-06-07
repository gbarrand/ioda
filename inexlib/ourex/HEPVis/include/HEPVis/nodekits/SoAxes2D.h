#ifndef HEPVis_SoAxes2D_h
#define HEPVis_SoAxes2D_h

/*
 *  SoAxes2D.h
 *  
 *
 *  Created by Laurent Garnier on 06/01/2004.
 *
 */

// Inheritance :
#include <Inventor/nodekits/SoBaseKit.h>

#include <Inventor/fields/SoSFUShort.h>

#include <HEPVis/nodekits/SoAxis.h>

class SoAxes2D : public SoBaseKit {
  SO_KIT_HEADER(SoAxes2D);
  
  SO_KIT_CATALOG_ENTRY_HEADER(topSeparator); 
  SO_KIT_CATALOG_ENTRY_HEADER(xAxis);
  SO_KIT_CATALOG_ENTRY_HEADER(yAxisSeparator);
  SO_KIT_CATALOG_ENTRY_HEADER(yAxisTransform);
  SO_KIT_CATALOG_ENTRY_HEADER(yAxis);
  SO_KIT_CATALOG_ENTRY_HEADER(grid);
  SO_KIT_CATALOG_ENTRY_HEADER(wall);
  SO_KIT_CATALOG_ENTRY_HEADER(topAxis);
  SO_KIT_CATALOG_ENTRY_HEADER(rightAxis);
 
public :
  enum LineType{
    SOLID = 0,
    DASHED,
    DOTTED,
    DASH_DOTTED
  };
  SoSFColor gridColor;  
  SoSFFloat gridTransparency; 
  SoSFUShort gridLinePattern;  
  SoSFBool gridVisible;  

  SoSFColor wallColor; 
  SoSFFloat wallTransparency; 
  SoSFFloat wallOffset; 
  SoSFBool wallVisible;  

  SoSFBool topAxisVisible;  
  SoSFFloat topAxisY;
  SoSFBool rightAxisVisible;  
  SoSFFloat rightAxisX;
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
  SoAxes2D();
  SoAxis* getXAxis();
  SoAxis* getYAxis();
protected:  
  virtual ~SoAxes2D();
private :
  void addGrid();
  void addWall();
  void buildWall(const SbVec3f&,const SbVec3f&,const SbVec3f&,
                 float,float,SoSeparator*);
  void addTopAxis();
  void addRightAxis();
private:
  SbColor fOld_gridColor;  
  float fOld_gridTransparency; 
  unsigned short fOld_gridLinePattern;  
  SbBool fOld_gridVisible;  

  SbColor fOld_wallColor; 
  float fOld_wallTransparency; 
  float fOld_wallOffset; 
  SbBool fOld_wallVisible;  

  SbBool fOld_topAxisVisible;  
  float fOld_topAxisY;
  SbBool fOld_rightAxisVisible;  
  float fOld_rightAxisX;
};

#endif
