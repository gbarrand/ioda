/*-----------------------------HEPVis----------------------------------------*/
/*                                                                           */
/* Node:             SoDblSiStripDetKit                                      */
/* Description:      Represents DoubleSided Silcon Strip Detectors           */
/* Author:           Joe Boudreau Nov 11 1996                                */
/*                                                                           */
/*---------------------------------------------------------------------------*/
#ifndef HEPVis_SoDblSiStripDetKit_h
#define HEPVis_SoDblSiStripDetKit_h

#include <Inventor/nodekits/SoBaseKit.h>
#include <Inventor/nodes/SoEventCallback.h>
class SoSiStrips;
class SoSFNode;
class SoTransform;
class SoDblSiStripDetKit:public SoBaseKit {

  // The following is required:
  SO_KIT_HEADER(SoDblSiStripDetKit);
  ////////////////////////////////////////////
public:
  SoSFNode alternateRep; //public in order to query if alternateRep done.
private:
  ////////////////////////////////////////////
  SO_KIT_CATALOG_ENTRY_HEADER(topSeparator);      //!< Head of the node kit
  SO_KIT_CATALOG_ENTRY_HEADER(localTransform);    //!< Transform
  SO_KIT_CATALOG_ENTRY_HEADER(transformSep);      //!< Transform separator
  SO_KIT_CATALOG_ENTRY_HEADER(transformFlip);     //!< Reverse transformation
  SO_KIT_CATALOG_ENTRY_HEADER(translateDragger);  //!< dragger
  SO_KIT_CATALOG_ENTRY_HEADER(waferSeparator);    //!< Separator for wafer
  SO_KIT_CATALOG_ENTRY_HEADER(waferAppearance);   //!< Wafer appearance node
  SO_KIT_CATALOG_ENTRY_HEADER(wafer);             //!< Node for wafer
  SO_KIT_CATALOG_ENTRY_HEADER(stripsSeparator);   //!< Separator for strips
  SO_KIT_CATALOG_ENTRY_HEADER(stripsAppearance);  //!< Strips appearance node
  SO_KIT_CATALOG_ENTRY_HEADER(stripsScale);       //!< Strips scale node
  SO_KIT_CATALOG_ENTRY_HEADER(stripsDragger);     //!< Dragger for strip height
  SO_KIT_CATALOG_ENTRY_HEADER(phiStrips);         //!< Separator for phi strips
  SO_KIT_CATALOG_ENTRY_HEADER(ninetyDeg);         //!< Separator for ortho strips
  SO_KIT_CATALOG_ENTRY_HEADER(zStrips);           //!< Separator for z strips 
  SO_KIT_CATALOG_ENTRY_HEADER(textSeparator);     //!< Separator for text 
  SO_KIT_CATALOG_ENTRY_HEADER(textAppearance);    //!< Text appearance node 
  SO_KIT_CATALOG_ENTRY_HEADER(textReset);         //!< Text reset node 
  SO_KIT_CATALOG_ENTRY_HEADER(textTranslation);   //!< Text translation node
  SO_KIT_CATALOG_ENTRY_HEADER(text);              //!< The text
  ////////////////////////////////////////////

public:

  // Constructor, required
  SoDblSiStripDetKit();

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
  void createInitialStripDet();
  virtual ~SoDblSiStripDetKit();

private: 

  // This is the callback function that will be 
  // added to the callback list
  static void      activateStrip  (void *userData, SoEventCallback *eventCB);
  static void   activateDetector  (void *userData, SoEventCallback *eventCB);
  static void deactivateDetector  (void *userData, SoEventCallback *eventCB);
  static void translateDetector   (void *userData, SoEventCallback *eventCB);
  static void detranslateDetector (void *userData, SoEventCallback *eventCB);

  // Only this class 
  SbBool isActivated;
  SbBool isAligning;
  SbBool isAlignable;
};

#endif
