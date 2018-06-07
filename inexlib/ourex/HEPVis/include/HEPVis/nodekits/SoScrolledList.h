#ifndef HEPVis_SoScrolledList_h
#define HEPVis_SoScrolledList_h

#include <Inventor/nodes/SoSubNode.h>
#include <Inventor/fields/SoSFString.h>
#include <Inventor/fields/SoMFString.h>
#include <Inventor/fields/SoSFInt32.h>

#include <HEPVis/nodekits/SoList.h>
#include <HEPVis/nodekits/SoForm.h>

class SoForm;
class SoArrowButton;

class SoScrolledList : public SoForm {
  SO_KIT_HEADER(SoScrolledList);
public:
  SoSFString fontList;
  SoMFString items;
  SoSFInt32 visibleItemCount;
  SoSFInt32 topItemPosition;
  SoSFFloat listWidth;
public:
  SoScrolledList();
  void addSingleSelectionCallback(SoCB*,void* = NULL);
  void removeSingleSelectionCallback(SoCB*,void* = NULL);
  const SbString& getSelectedItem();
public:
  static void initClass();
protected: 
  virtual ~SoScrolledList();
  virtual void updateSceneGraph(); 
private:
  void initialize();
  static void upCB(void*,SoNode*);
  static void downCB(void*,SoNode*);
  SoList* fList;
  SoArrowButton* fUp;
  SoArrowButton* fDown;
};

#endif

