#ifndef HEPVis_SoForm_h
#define HEPVis_SoForm_h

#include <HEPVis/nodekits/SoPrimitive.h>

class SoSensor;
class SbVec3f;
class SbPList;

class SoForm : public SoPrimitive {
  SO_KIT_HEADER(SoForm);
public:
  SoForm();
  void addChild(SoPrimitive*);
public:
  static void initClass();
protected:
  virtual ~SoForm();
  virtual void updateSceneGraph(); 
private:
  void initialize();
  static void sensorCB(void*,SoSensor*);
  static SbVec3f getTranslation(SoPrimitive*); 
  SbPList fSensors;
  SbBool isNodeRightAttached(SoPrimitive*);
};


#endif

