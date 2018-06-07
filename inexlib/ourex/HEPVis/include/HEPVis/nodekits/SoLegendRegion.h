/*-----------------------------HEPVis----------------------------------------*/
/*                                                                           */
/* Node:             SoLegendRegion                                          */
/* Description:      A nodekit to handle a legend box for the plotter.       */
/* Author            Guy Barrand                                             */
/*                                                                           */
/*---------------------------------------------------------------------------*/

#ifndef HEPVis_SoLegendRegion_h
#define HEPVis_SoLegendRegion_h

/*
 * SoLegendRegion is a basekit to handle text within a viewport region.
 *  See the applications/Plotter example.
 */

#include <HEPVis/nodekits/SoRegion.h>

#include <Inventor/fields/SoMFString.h>
#include <Inventor/fields/SoSFNode.h>

#include <vector>

class SoStyle;
class SoMFString;

class SoLegendRegion : public SoRegion {
  SO_KIT_HEADER(SoLegendRegion);
  SO_KIT_CATALOG_ENTRY_HEADER(topSeparator);
  SO_KIT_CATALOG_ENTRY_HEADER(light);
  SO_KIT_CATALOG_ENTRY_HEADER(lightModel);
  SO_KIT_CATALOG_ENTRY_HEADER(camera);
  SO_KIT_CATALOG_ENTRY_HEADER(lines);
  SO_KIT_CATALOG_ENTRY_HEADER(styleSeparator);
  SO_KIT_CATALOG_ENTRY_HEADER(styles);
public:
  SoMFString text;
  SoSFBool markerOnLeftBorder;
  SoSFBool textInBlack;
  SoSFNode alternateRep;
public: /*SoINTERNAL*/
  static void initClass();
public:
  SoLegendRegion();
  SoStyle* getStyle(int);
  virtual void generateAlternateRep(SoAction*);
  virtual void clearAlternateRep();
public: //SoRegion
  virtual void updateChildren();
  virtual SoSeparator* getTopSeparator() const;
  virtual SoCamera* getCamera() const;
  virtual void resetCamera();
  virtual void clear(const SbString& = "");
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
protected:
  virtual ~SoLegendRegion();
private:
  void matchViewport(float,float);
  void setTextCharSize(float);
  void getInsideSize(float&,float&,float&,float&,float&,
                     std::vector<float>&,float&);
private:
  short fOldWidth;
  short fOldHeight;
  SoStyle* fDefaultStyle;
  SoMFString* fOldText;
};


#endif
