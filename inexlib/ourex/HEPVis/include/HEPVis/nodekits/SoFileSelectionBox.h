#ifndef HEPVis_SoFileSelectionBox_h
#define HEPVis_SoFileSelectionBox_h

#include <Inventor/fields/SoSFString.h>
#include <Inventor/misc/SoCallbackList.h>

#include <HEPVis/nodekits/SoForm.h>

class SoEventCallback;
class SoScrolledList;

class SoFileSelectionBox : public SoForm {
  SO_KIT_HEADER(SoFileSelectionBox);
public:
  SoSFString directory;
  //SoSFString selection;
public:
  SoFileSelectionBox();
  void addOkCallback(SoCB*,void* = NULL);
  void removeOkCallback(SoCB*,void* = NULL);
  const SbString& getSelectedFile();
public:
  static void initClass();
protected: 
  virtual ~SoFileSelectionBox();
  virtual void updateSceneGraph(); 
private:
  void initialize();
  void invokeOkCallbacks();
  SoCallbackList fOkCallbackList;
  SbString fSelectedFile;
  SoScrolledList* fScrolledList;
  static void singleSelectionCB(void*,SoNode*);
  void setDirectory();
  SoFieldSensor* fSensor;
  static void sensorCB(void*,SoSensor*);
};


#endif

