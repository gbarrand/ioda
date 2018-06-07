#ifndef HEPVis_SoText_h
#define HEPVis_SoText_h

#include <Inventor/SbPList.h>
#include <Inventor/fields/SoSFString.h>
#include <Inventor/fields/SoMFString.h>
#include <Inventor/fields/SoSFInt32.h>
#include <Inventor/fields/SoSFEnum.h>
#include <Inventor/fields/SoSFBool.h>
#include <Inventor/misc/SoCallbackList.h>
#include <Inventor/events/SoKeyboardEvent.h>

#include <HEPVis/nodekits/SoPrimitive.h>

class SoEventCallback;

class SoText : public SoPrimitive {
  SO_KIT_HEADER(SoText);
  SO_KIT_CATALOG_ENTRY_HEADER(textFont);
  SO_KIT_CATALOG_ENTRY_HEADER(textGroup);
public:
  enum KeyboardType {
    ENGLISH,
    FRENCH
  };
  SoSFString fontList;
  SoMFString value;
  SoSFInt32 rows;
  SoSFInt32 topPosition;
  SoSFBool editable;
  SoSFInt32 cursorPosition;
  SoSFFloat textWidth; //Not in Motif.
public:
  SoText();
  void addActivateCallback(SoCB*,void* = NULL);
  void removeActivateCallback(SoCB*,void* = NULL);
public:
  static void initClass();
protected: 
  virtual ~SoText();
  virtual void updateSceneGraph(); 
  virtual SbBox3f getFrontBBox(); 
protected:
  SoEventCallback* fEventCallback;
private:
  void initialize();
  void invokeActivateCallbacks();
  SoCallbackList fActivateCallbackList;
  static void eventCB(void*,SoEventCallback*);
  SbBox3f fBox;
  SbPList fTextNodes;
  void insertCharacter(int,char);
  char getCharacter(SoKeyboardEvent::Key,SbBool,SbBool,SbBool);
  static SoText::KeyboardType fKeyboardType;
};


#endif

