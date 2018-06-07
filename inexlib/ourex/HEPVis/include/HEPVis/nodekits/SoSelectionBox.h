#ifndef HEPVis_SoSelectionBox_h
#define HEPVis_SoSelectionBox_h

#include <Inventor/fields/SoMFString.h>
#include <Inventor/misc/SoCallbackList.h>

#include <HEPVis/nodekits/SoForm.h>

class SoEventCallback;
class SoScrolledList;

class SoSelectionBox : public SoForm {
  SO_KIT_HEADER(SoSelectionBox);
public:
  SoMFString items;
public:
  SoSelectionBox();
  void addOkCallback(SoCB*,void* = NULL);
  void removeOkCallback(SoCB*,void* = NULL);
  const SbString& getSelection();
public:
  static void initClass();
protected: 
  virtual ~SoSelectionBox();
  virtual void updateSceneGraph(); 
private:
  void initialize();
  void setItems(); 
  void invokeOkCallbacks();
  SoCallbackList fOkCallbackList;
  SbString fSelection;
  SoScrolledList* fScrolledList;
  static void singleSelectionCB(void*,SoNode*);
};


#endif

