#ifndef HEPVis_SoPushButton_h
#define HEPVis_SoPushButton_h

#include <Inventor/fields/SoSFString.h>
#include <Inventor/fields/SoMFString.h>
#include <Inventor/misc/SoCallbackList.h>

#include <HEPVis/nodekits/SoPrimitive.h>

class SoEventCallback;

/** 
 * SoPushButton is an Inventor implementation of the Motif XmPushButton
 * widget. Motif XmPushButton resources are transcripted in Inventor fields.
 * Someone can deposit (or remove) callback functions that are executed
 * when the SoPushButton shape receives a mouse button pressed event.
 */

class SoPushButton : public SoPrimitive {
  SO_KIT_HEADER(SoPushButton);
  SO_KIT_CATALOG_ENTRY_HEADER(labelFont);
  SO_KIT_CATALOG_ENTRY_HEADER(labelText);
public:
  /** Label font. Default : "Helvetica" */
  SoSFString fontList;
  /** Label. Default : "PushButton". */
  SoMFString labelString;
public:
  SoPushButton();
  /** Add an activate callback. */
  void addActivateCallback(SoCB*,void* = NULL);
  /** Remove an activate callback. */
  void removeActivateCallback(SoCB*,void* = NULL);
public:
  static void initClass();
protected:
  virtual ~SoPushButton();
  virtual void updateSceneGraph(); 
  virtual SbBox3f getFrontBBox(); 
private:
  void initialize();
  void invokeActivateCallbacks();
  SoCallbackList fActivateCallbackList;
  static void eventCB(void*,SoEventCallback*);
};


#endif

