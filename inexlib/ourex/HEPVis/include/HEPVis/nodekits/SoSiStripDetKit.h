/*-----------------------------HEPVis----------------------------------------*/
/*                                                                           */
/* Node:             SoSiStripDetKit                                            */
/* Description:      Represents SingleSided Silcon Strip Detectors           */
/* Author:           Joe Boudreau Nov 11 1996                                */
/*                                                                           */
/*---------------------------------------------------------------------------*/
#ifndef HEPVis_SoSiStripDetKit_h
#define HEPVis_SoSiStripDetKit_h

// Inheritance :
#include <Inventor/nodekits/SoBaseKit.h>

#include <Inventor/nodes/SoEventCallback.h>

class SoSiStrips;
class SoSFNode;
class SoTransform;
class SoSiStripDetKit:public SoBaseKit {

  // The following is required:
  SO_KIT_HEADER(SoSiStripDetKit);
  ////////////////////////////////////////////
public:
  SoSFNode alternateRep; //public in order to query if alternateRep done.
private:
  ////////////////////////////////////////////
  SO_KIT_CATALOG_ENTRY_HEADER(topSeparator);
    SO_KIT_CATALOG_ENTRY_HEADER(localTransform);
    SO_KIT_CATALOG_ENTRY_HEADER(transformSep);
      SO_KIT_CATALOG_ENTRY_HEADER(transformFlip);
      SO_KIT_CATALOG_ENTRY_HEADER(translateDragger);
    SO_KIT_CATALOG_ENTRY_HEADER(waferSeparator);
      SO_KIT_CATALOG_ENTRY_HEADER(waferAppearance);
      SO_KIT_CATALOG_ENTRY_HEADER(wafer);
    SO_KIT_CATALOG_ENTRY_HEADER(stripsSeparator);
      SO_KIT_CATALOG_ENTRY_HEADER(stripsAppearance);
      SO_KIT_CATALOG_ENTRY_HEADER(stripsScale);
      SO_KIT_CATALOG_ENTRY_HEADER(stripsDragger);
      SO_KIT_CATALOG_ENTRY_HEADER(strips);
    SO_KIT_CATALOG_ENTRY_HEADER(textSeparator);
      SO_KIT_CATALOG_ENTRY_HEADER(textAppearance);
      SO_KIT_CATALOG_ENTRY_HEADER(textReset);
      SO_KIT_CATALOG_ENTRY_HEADER(textTranslation);
      SO_KIT_CATALOG_ENTRY_HEADER(text);
  ////////////////////////////////////////////

public:

  // Constructor, required
  SoSiStripDetKit();

  // This is required
  virtual SbBool affectsState() const;

  // Class Initializer, required
  static void initClass();

  // Generate AlternateRep, required.  Generating an alternate representation
  // must be done upon users request.  It allows an Inventor program to read
  // back the file without requiring *this* code to be dynamically linked. 
  // If the users expects that *this* code will be dynamically linked, he
  // need not invoke this method.  
  virtual void generateAlternateRep(SoAction*);

  // We better be able to clear it, too!
  virtual void clearAlternateRep();

  // Function to control whether this wafer is alignable or not:
  virtual void setAlignable (SbBool flag);

  // Function to fetch back the local transform which is used in alignment:
  virtual SoTransform* getLocalTransform() const;

protected:
  virtual ~SoSiStripDetKit();

private: 

  // This is needed as well
  void createInitialStripDet();

  // This is the callback function that will be 
  // added to the callback list
  static void   activateDetector (void *userData, SoEventCallback *eventCB);
  static void      activateStrip (void *userData, SoEventCallback *eventCB);
  static void deactivateDetector (void *userData, SoEventCallback *eventCB);
  static void translateDetector   (void *userData, SoEventCallback *eventCB);
  static void detranslateDetector (void *userData, SoEventCallback *eventCB);

  // Only this class 
  SbBool isActivated;
  SbBool isAligning;
  SbBool isAlignable;

};

#endif
