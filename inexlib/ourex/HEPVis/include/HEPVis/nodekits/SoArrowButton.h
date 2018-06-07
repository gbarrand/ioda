#ifndef HEPVis_SoArrowButton_h
#define HEPVis_SoArrowButton_h

#include <Inventor/fields/SoSFEnum.h>
#include <Inventor/misc/SoCallbackList.h>

#include <HEPVis/nodekits/SoPrimitive.h>

class SoEventCallback;
class SoSensor;
class SoFieldSensor;

class SoArrowButton : public SoPrimitive {
  SO_KIT_HEADER(SoArrowButton);
  SO_KIT_CATALOG_ENTRY_HEADER(arrowTransform);
  SO_KIT_CATALOG_ENTRY_HEADER(arrowCone);
public:
  enum ArowDirection {
    ARROW_UP = 0, // Values are importants, they are used by an SoCalculator.
    ARROW_LEFT = 1,
    ARROW_DOWN = 2,
    ARROW_RIGHT = 3
  };
  SoSFEnum arrowDirection;
public:
  SoArrowButton();
  void addActivateCallback(SoCB*,void* = NULL);
  void removeActivateCallback(SoCB*,void* = NULL);
public:
  static void initClass();
protected:
  virtual ~SoArrowButton();
  virtual void updateSceneGraph(); 
private:
  void initialize();
  void invokeActivateCallbacks();
  SoCallbackList fActivateCallbackList;
  static void eventCB(void*,SoEventCallback*);
  static void sensorCB(void*,SoSensor*);
  SoFieldSensor* fSensor;
};


#endif

