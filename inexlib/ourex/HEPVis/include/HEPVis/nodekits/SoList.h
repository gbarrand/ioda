#ifndef HEPVis_SoList_h
#define HEPVis_SoList_h

#include <Inventor/SbPList.h>
#include <Inventor/fields/SoSFString.h>
#include <Inventor/fields/SoMFString.h>
#include <Inventor/fields/SoSFInt32.h>
#include <Inventor/fields/SoSFEnum.h>
#include <Inventor/misc/SoCallbackList.h>

#include <HEPVis/nodekits/SoPrimitive.h>

class SoEventCallback;

/**
 * SoList is an Inventor implementation of the Motif XmList widget.
 * Main Motif XmList resources are transcripted in Inventor fields.
 * Someone can deposit (or remove) callback functions that are executed
 * when an SoList item shape receives a mouse button pressed event.
 */

class SoList : public SoPrimitive {
  SO_KIT_HEADER(SoList);
  SO_KIT_CATALOG_ENTRY_HEADER(textFont);
  SO_KIT_CATALOG_ENTRY_HEADER(textGroup);
public:
  SoSFString fontList;
  SoMFString items;
  SoSFInt32 visibleItemCount;
  SoSFInt32 topItemPosition;
  enum ListSizePolicy {
    CONSTANT,
    VARIABLE
  };
  SoSFEnum listSizePolicy;
  SoSFFloat listWidth; //Not in Motif.
public:
  SoList();
  void addSingleSelectionCallback(SoCB*,void* = NULL);
  void removeSingleSelectionCallback(SoCB*,void* = NULL);
  const SbString& getSelectedItem();
public:
  static void initClass();
protected: 
  virtual ~SoList();
  virtual void updateSceneGraph(); 
  virtual SbBox3f getFrontBBox(); 
private:
  void initialize();
  void invokeSingleSelectionCallbacks();
  SoCallbackList fSingleSelectionCallbackList;
  static void eventCB(void*,SoEventCallback*);
  SbPList fItemNodes;
  SbString fSelectedItem;
  SbBox3f fBox;
};


#endif

