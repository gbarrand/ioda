#ifndef HEPVis_SoAxis_h
#define HEPVis_SoAxis_h

/*
 *  SoAxis.h
 *  
 *
 *  Created by Laurent Garnier on Fri Dec 05 2003.
 *
 */

// Inheritance :
#include <Inventor/nodekits/SoBaseKit.h>

#include <Inventor/fields/SoSFNode.h>
#include <Inventor/fields/SoSFFloat.h>
#include <Inventor/fields/SoSFString.h>
#include <Inventor/fields/SoMFString.h>
#include <Inventor/fields/SoMFFloat.h>
#include <Inventor/fields/SoSFColor.h>
#include <Inventor/fields/SoSFUInt32.h>
#include <Inventor/fields/SoSFBool.h>
#include <Inventor/fields/SoSFInt32.h>
#include <Inventor/fields/SoSFEnum.h>
#include <Inventor/fields/SoSFUShort.h>
#include <Inventor/fields/SoSFVec3f.h>
#include <Inventor/fields/SoSFDouble.h>

#include <HEPVis/SbStyle.h>

#include <vector> //fSubTicks

class SoLineStyle;
class SoTextStyle;
class SoStyleCache;

class SoAxis : public SoBaseKit {
  SO_KIT_HEADER(SoAxis);
  
  SO_KIT_CATALOG_ENTRY_HEADER(topSeparator);
  SO_KIT_CATALOG_ENTRY_HEADER(lineSeparator);
  SO_KIT_CATALOG_ENTRY_HEADER(ticksSeparator);
  SO_KIT_CATALOG_ENTRY_HEADER(labelsSeparator);
  SO_KIT_CATALOG_ENTRY_HEADER(titleSeparator);
  SO_KIT_CATALOG_ENTRY_HEADER(magSeparator);
  // Styles :
  SO_KIT_CATALOG_ENTRY_HEADER(styleSeparator);
  SO_KIT_CATALOG_ENTRY_HEADER(lineStyle);
  SO_KIT_CATALOG_ENTRY_HEADER(ticksStyle);
  SO_KIT_CATALOG_ENTRY_HEADER(labelsStyle);
  SO_KIT_CATALOG_ENTRY_HEADER(titleStyle);
  SO_KIT_CATALOG_ENTRY_HEADER(magStyle);
public :
  SoSFNode alternateRep;
  SoSFBool verbose;
  SoSFString modeling; //hippo, hplot
  SoSFFloat width;
  SoSFFloat minimumValue;
  SoSFFloat maximumValue;
  SoSFInt32 divisions;
  SoSFBool logScale;
  SoSFBool tickUp;
  SoSFFloat tickLength;
  SoSFFloat labelToAxis;
  SoSFFloat labelHeight;
  SoSFString title;
  SoSFFloat titleToAxis;
  SoSFFloat titleHeight;
  enum Justification {
    LEFT   = 0x00,
    CENTER = 0x01,
    RIGHT  = 0x02
  };
  SoSFEnum titleToAxisJustification;

  SoSFFloat ttfScale;

  SoSFBool labelsEnforced;

  SoSFBool timeLabels;
  SoSFString timeFormat;
  SoSFDouble timeOffset;
  SoSFBool timeOffsetIsGMT;

  // filled by computeTicks
  SoSFInt32 tickNumber; //Output
  SoMFString labels;    //Output
  SoMFFloat values;     //Output
  SoMFFloat coords;     //Output
  SoMFFloat subCoords;  //Output
  SoSFInt32 magnitude;  //Output
public:
  static void initClass();
public: /*SoEXTENDER*/
  virtual void doAction(SoAction*);
  virtual void callback(SoCallbackAction*);
  virtual void GLRender(SoGLRenderAction*);
  virtual void getBoundingBox(SoGetBoundingBoxAction*);
  virtual void getMatrix(SoGetMatrixAction*);
  virtual void handleEvent(SoHandleEventAction*);
  virtual void rayPick(SoRayPickAction*);
  virtual void search(SoSearchAction*);
  virtual void write(SoWriteAction*);
  virtual void pick(SoPickAction*);
public:
  SoAxis();
  void reset();
  void updateChildren();
  SoLineStyle* getLineStyle(); 
  SoLineStyle* getTicksStyle(); 
  SoTextStyle* getLabelsStyle(); 
  SoTextStyle* getTitleStyle(); 
  SoTextStyle* getMagStyle(); 

  void setVisible(SbBool);

  virtual void generateAlternateRep(SoAction*);
  virtual void clearAlternateRep();
protected:
  virtual ~SoAxis();
private :
  void computeTicksHippo();
  void computeTicksHPLOT();
private:
  std::vector< std::pair<SbVec3f,SbVec3f> > fSubTicks;

  SbString fOld_modeling;
  float fOld_width;
  float fOld_minimumValue;
  float fOld_maximumValue;
  int fOld_divisions;
  SbBool fOld_logScale;
  SbBool fOld_tickUp;
  float fOld_tickLength;
  float fOld_labelToAxis;
  float fOld_labelHeight;
  SbString fOld_title;
  float fOld_titleToAxis;
  float fOld_titleHeight;
  int fOld_titleToAxisJustification;
  float fOld_ttfScale;
  SbBool fOld_labelsEnforced;

  int fOld_tickNumber;
  SoMFString* fOld_labels;
  SoMFFloat* fOld_values;
  SoMFFloat* fOld_coords;
  SoMFFloat* fOld_subCoords;
  //int fOld_magnitude;

  SbStyle fOld_lineStyle;
  SbStyle fOld_ticksStyle;
  SbStyle fOld_labelsStyle;
  SbStyle fOld_titleStyle;
  SbStyle fOld_magStyle;

  SoStyleCache* fStyleCache;
};

#endif
