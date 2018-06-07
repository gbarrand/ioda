#ifndef HEPVis_SoCommand_h
#define HEPVis_SoCommand_h

#include <HEPVis/nodekits/SoText.h>

class SoEventCallback;

class SoCommand : public SoText {
  SO_NODE_HEADER(SoCommand);
public:
  SoMFString historyItems;
public:
  SoCommand();
public:
  static void initClass();
protected: 
  virtual ~SoCommand();
private:
  static void eventCB(void*,SoEventCallback*);
  static void activateCB(void*,SoNode*);
  int fHistoryPosition;
};


#endif
