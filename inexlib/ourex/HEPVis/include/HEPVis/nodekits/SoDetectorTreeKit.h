/*-----------------------------HEPVis----------------------------------------*/
/*                                                                           */
/* Node:             SoDetectorTreeKit                                       */
/* Description:      Easy way of browsing through a tree of detectors        */
/* Author:           Joe Boudreau Nov 11 1996                                */
/*                                                                           */
/*---------------------------------------------------------------------------*/
#ifndef HEPVis_SoDetectorTreeKit_h
#define HEPVis_SoDetectorTreeKit_h

// Inheritance :
#include <Inventor/nodekits/SoBaseKit.h>

#include <Inventor/misc/SoCallbackList.h>

class SoEventCallback;
class SoSeparator;

class SoDetectorTreeKit:public SoBaseKit {

  // The following is required:
  SO_KIT_HEADER(SoDetectorTreeKit);
  ////////////////////////////////////////////
public:
  SoSFNode alternateRep; //public in order to query if alternateRep done.
private:
  ////////////////////////////////////////////
  SO_KIT_CATALOG_ENTRY_HEADER(topSeparator);
    SO_KIT_CATALOG_ENTRY_HEADER(pickStyle);
    SO_KIT_CATALOG_ENTRY_HEADER(appearance);
    SO_KIT_CATALOG_ENTRY_HEADER(units);
    SO_KIT_CATALOG_ENTRY_HEADER(transform);
    SO_KIT_CATALOG_ENTRY_HEADER(texture2Transform);
    SO_KIT_CATALOG_ENTRY_HEADER(childList);
      SO_KIT_CATALOG_ENTRY_HEADER(previewSeparator);
      SO_KIT_CATALOG_ENTRY_HEADER(fullSeparator);
   

public:

  // Constructor, required
  SoDetectorTreeKit();

  // This is required
  virtual SbBool affectsState() const;

  // Class Initializer, required
  static void initClass();
public: /*SoEXTENDER*/
  virtual void doAction(SoAction*);
  virtual void callback(SoCallbackAction*);
  virtual void getBoundingBox(SoGetBoundingBoxAction*);
  virtual void getMatrix(SoGetMatrixAction*);
  virtual void search(SoSearchAction*);
  virtual void write(SoWriteAction*);
public:
  // Turn the preview on or off
  virtual void setPreview(SbBool Flag);

  // Return the preview state
  virtual SbBool getPreview() const;

  // Set SoSwitch::whichChild = SO_SWITCH_ALL
  void setPreviewAndFull();

  // Return the preview Separator
  virtual SoSeparator *getPreviewSeparator() const;

  // Return the full Separator
  virtual SoSeparator *getFullSeparator() const;

  // Generate AlternateRep, required.  Generating an alternate representation
  // must be done upon users request.  It allows an Inventor program to read
  // back the file without requiring *this* code to be dynamically linked. 
  // If the users expects that *this* code will be dynamically linked, he
  // need not invoke this method.  
  virtual void generateAlternateRep(SoAction*);

  // We better be able to clear it, too!
  virtual void clearAlternateRep();

  void addPickCallback(SoCallbackListCB*,void* = NULL);
  void removePickCallback(SoCallbackListCB*,void* = NULL);
protected:

  // Destructor.
  virtual ~SoDetectorTreeKit();
private: 

  // This is needed as well
  void createInitialTree();

  // This is the callback function that will be 
  // added to the callback list
  static void   pick_cbk     (void *userData, SoEventCallback *eventCB);
  static void   expand_cbk   (void *userData, SoEventCallback *eventCB);
  static void   contract_cbk (void *userData, SoEventCallback *eventCB);

private:
  SoCallbackList fPickCallbackList;
};

#endif
