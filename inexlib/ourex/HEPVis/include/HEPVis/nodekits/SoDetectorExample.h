/*-----------------------------HEPVis----------------------------------------*/
/*                                                                           */
/* Node:             SoDetectorExample                                       */
/* Description:      An example of usage of SoDetectorTreeKit.               */
/* Author            Guy Barrand                                             */
/*                                                                           */
/*---------------------------------------------------------------------------*/

#ifndef HEPVis_SoDetectorExample_h
#define HEPVis_SoDetectorExample_h

// Inheritance :
#include <Inventor/nodekits/SoBaseKit.h>

#include <Inventor/fields/SoSFNode.h>

#include <Inventor/misc/SoCallbackList.h>

class SoDetectorTreeKit;

class SoDetectorExample : public SoBaseKit {
  SO_KIT_HEADER(SoDetectorExample);
  SO_KIT_CATALOG_ENTRY_HEADER(topSeparator);
public:
  SoSFNode alternateRep;
public: /*SoINTERNAL*/
  static void initClass();
public:
  SoDetectorExample();
  virtual void generateAlternateRep(SoAction*);
  virtual void clearAlternateRep();
protected:
  virtual ~SoDetectorExample();
  virtual void doAction(SoAction*);
  virtual void write(SoWriteAction*);
private:
  void addWafer(SoDetectorTreeKit*,float);
  void addWedge(SoDetectorTreeKit*,float,SbBool);
  void addBarrel(SoDetectorTreeKit*,float);
};


#endif
