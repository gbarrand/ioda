/*-----------------------------HEPVis----------------------------------------*/
/*                                                                           */
/* Node:             SoInfosRegion                                           */
/* Description:      A nodekit to handle plotter infos (histo stats, fit     */
/*                   results, etc...)                                        */
/*                   within a viewport region.                               */
/* Author            Guy Barrand                                             */
/*                                                                           */
/*---------------------------------------------------------------------------*/

#ifndef HEPVis_SoInfosRegion_h
#define HEPVis_SoInfosRegion_h

/*
 * SoInfosRegion is a basekit to handle text within a viewport region.
 *  See the applications/Plotter example.
 */

#include <HEPVis/nodekits/SoRegion.h>

#include <Inventor/fields/SoSFString.h>
#include <Inventor/fields/SoMFString.h>
#include <Inventor/fields/SoSFEnum.h>
#include <Inventor/fields/SoSFInt32.h>
#include <Inventor/fields/SoSFNode.h>

#include <vector>

class SoLineStyle;
class SoTextStyle;
class SoMFString;

class SoInfosRegion : public SoRegion {
  SO_KIT_HEADER(SoInfosRegion);
  SO_KIT_CATALOG_ENTRY_HEADER(topSeparator);
  SO_KIT_CATALOG_ENTRY_HEADER(light);
  SO_KIT_CATALOG_ENTRY_HEADER(lightModel);
  SO_KIT_CATALOG_ENTRY_HEADER(camera);
  SO_KIT_CATALOG_ENTRY_HEADER(titleSeparator);
  SO_KIT_CATALOG_ENTRY_HEADER(lines);
  SO_KIT_CATALOG_ENTRY_HEADER(styleSeparator);
  SO_KIT_CATALOG_ENTRY_HEADER(lineStyle);
  SO_KIT_CATALOG_ENTRY_HEADER(textStyle);
public:
  SoSFNode alternateRep;
  SoSFString title;
  SoMFString text;
  SoSFInt32 inBetweenSpaces; //Number of spaces between left and right texts.
public: /*SoINTERNAL*/
  static void initClass();
public:
  SoInfosRegion();
public:
  SoLineStyle* getLineStyle() const;
  SoTextStyle* getTextStyle() const;
  void getContentSize(float&,float&);
  void matchViewport(float,float);
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
  virtual ~SoInfosRegion();
private:
  void setCharSize(float);
  void getInsideSize(float&,float&,
                     float&,float&,float&, 
                     float&,float&,float&,std::vector<float>&,float&);
private:
  SbString fOld_title;
  SoMFString* fOld_text;
  int fOld_inBetweenSpaces;
  short fOldWidth;
  short fOldHeight;
};

#endif
