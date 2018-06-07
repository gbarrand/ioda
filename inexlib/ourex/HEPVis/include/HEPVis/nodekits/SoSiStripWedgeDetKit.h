/*-----------------------------HEPVis----------------------------------------*/
/*                                                                           */
/* Node:             SoSiStripWedgeDetKit                                    */
/* Description:      Represents a single sided silicon strip wedge detector  */
/* Author:           Peter Hamel                                             */
/*                                                                           */
/*---------------------------------------------------------------------------*/
#ifndef __SoSiStripWedgeDetKit_hh__
#define __SoSiStripWedgeDetKit_hh__

// Inheritance :
#include <Inventor/nodekits/SoBaseKit.h>

#include <Inventor/nodes/SoEventCallback.h>
#include <Inventor/nodes/SoShape.h>

class SoSiWedgeStrips;
class SoWedge;
class SoSFNode;
class SoTransform;
class SoSiStripWedgeDetKit:public SoBaseKit {

  // The following is required:
  SO_KIT_HEADER(SoSiStripWedgeDetKit);
  ////////////////////////////////////////////
public:

  SoSFFloat halfHeight;
  SoSFFloat halfWidth1;
  SoSFFloat halfWidth2;
  SoSFFloat thickness;
  float myHalfHeight, myHalfWidth1, myHalfWidth2, myThickness;

  SoSFNode alternateRep; //public in order to query if alternateRep done.
private:
  ////////////////////////////////////////////
  SO_KIT_CATALOG_ENTRY_HEADER(topSeparator);
    SO_KIT_CATALOG_ENTRY_HEADER(localTransform);
    SO_KIT_CATALOG_ENTRY_HEADER(transformSep);
      SO_KIT_CATALOG_ENTRY_HEADER(transformFlip);
      SO_KIT_CATALOG_ENTRY_HEADER(translateDragger);
    SO_KIT_CATALOG_ENTRY_HEADER(wedgeSeparator);
      SO_KIT_CATALOG_ENTRY_HEADER(wedgeAppearance);
      SO_KIT_CATALOG_ENTRY_HEADER(wedge);
    SO_KIT_CATALOG_ENTRY_HEADER(stripsSeparator);
      SO_KIT_CATALOG_ENTRY_HEADER(stripsScale);
      SO_KIT_CATALOG_ENTRY_HEADER(stripsDragger);
      SO_KIT_CATALOG_ENTRY_HEADER(phiSeparator);
        SO_KIT_CATALOG_ENTRY_HEADER(phiStripsAppearance);
        SO_KIT_CATALOG_ENTRY_HEADER(phiRot);
        SO_KIT_CATALOG_ENTRY_HEADER(phiTranslation);
        SO_KIT_CATALOG_ENTRY_HEADER(phiStrips);
    SO_KIT_CATALOG_ENTRY_HEADER(textSeparator);
      SO_KIT_CATALOG_ENTRY_HEADER(textAppearance);
      SO_KIT_CATALOG_ENTRY_HEADER(textReset);
      SO_KIT_CATALOG_ENTRY_HEADER(textTranslation);
      SO_KIT_CATALOG_ENTRY_HEADER(text);
  ////////////////////////////////////////////

public:

  // Constructor, required
  SoSiStripWedgeDetKit();

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
  // Destructor.  Required.
  virtual ~SoSiStripWedgeDetKit();

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
