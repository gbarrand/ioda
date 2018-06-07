/*-----------------------------HEPVis----------------------------------------*/
/*                                                                           */
/* Node:             SoPage                                                  */
/* Description:      An SoCanvas with a global title and mthods to handle    */
/*                   grid of regions..                                       */
/*                   See the applications/Page example.                      */
/* Author:           Guy Barrand                                             */
/*                                                                           */
/*---------------------------------------------------------------------------*/

#ifndef HEPVis_SoPage_h
#define HEPVis_SoPage_h

// Inheritance :
#include <HEPVis/nodekits/SoCanvas.h>

#include <Inventor/SbLinear.h>
#include <Inventor/fields/SoSFBool.h>
#include <Inventor/fields/SoSFFloat.h>
#include <Inventor/fields/SoMFString.h>
#include <Inventor/fields/SoSFEnum.h>
#include <Inventor/fields/SoSFVec2f.h>
#include <Inventor/fields/SoSFNode.h>

class SoImageWriter;
class SoTextRegion;

class SoPage : public SoCanvas {
  SO_KIT_HEADER(SoPage);
  SO_KIT_CATALOG_ENTRY_HEADER(pageSeparator);
  SO_KIT_CATALOG_ENTRY_HEADER(titleRegion);
  SO_KIT_CATALOG_ENTRY_HEADER(imageWriter);
public:
  SoMFString title;
  SoSFBool titleVisible;
  SoSFFloat titleHeight;
  SoSFBool titleGeometryAutomated;
  enum ResizePolicy {
    LEAVE_ALONE,
    PAW_GRID
  };
  SoSFEnum regionsResizePolicy;
  // PAW_GRID parameters :
  SoSFFloat pageWidth;         //PAW XSIZ
  SoSFFloat pageHeight;        //PAW YSIZ
  SoSFFloat leftMargin;        //PAW XMGL
  SoSFFloat rightMargin;       //PAW XMGR
  SoSFFloat bottomMargin;      //PAW YMGL
  SoSFFloat topMargin;         //PAW YMGU
  SoSFFloat horizontalSpacing; //PAW XWIN
  SoSFFloat verticalSpacing;   //PAW YWIN
  SoSFNode alternateRep;
public: /*SoINTERNAL*/
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
  SoPage();
protected:
  virtual ~SoPage();
public:
  void refresh();
  void raiseUpdateFlag();
  void updateChildren();
  SoImageWriter* getImageWriter() const;
  SoTextRegion* getTitleRegion() const;
  void setTitleVisible(SbBool);
  void createRegions(const SbString&,int = 1, int = 1, int = 0);
  void createRegions(int = 1, int = 1, int = 0);
  void addRegions(const SbString&,int = 1, int = 1);
  void mapRegionToPage(SoRegion*);
  void mapHighlightedRegionToPage();
  SoRegion* createGridRegion(const SbString&,int,int,int);
  void resetGrid();
  //void enforceWinSize(const SbVec2s&);
public:
  virtual void generateAlternateRep(SoAction*);
  virtual void clearAlternateRep();
private:
  void addGridOfRootRegion(const SbString&,float,float,float,float,int,int);
private:
  float fDisplayHeight;
  short fOldWidth;
  short fOldHeight;
  SoMFString* fOld_title;
  SbBool fOld_titleVisible;
  float fOld_titleHeight;
  SbBool fOld_titleGeometryAutomated;
  int fOld_regionsResizePolicy;
  int fOld_numberOfRegions;

  float fOld_pageWidth;
  float fOld_pageHeight;
  float fOld_leftMargin;
  float fOld_rightMargin;
  float fOld_bottomMargin;
  float fOld_topMargin;
  float fOld_horizontalSpacing;
  float fOld_verticalSpacing;

  SbBool fUpdate;
};

#endif
