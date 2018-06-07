#ifndef HEPVis_SoAlternateRepAction_h
#define HEPVis_SoAlternateRepAction_h 

// Inheritance :
#include <Inventor/actions/SoAction.h>

#include <Inventor/actions/SoSubAction.h>

class SoAlternateRepAction : public SoAction {
  SO_ACTION_HEADER(SoAlternateRepAction);
public:
  enum ForWhat { DEFAULT, VRML };
public:
  SoAlternateRepAction();
  virtual ~SoAlternateRepAction();  
  static void initClass(void);
  void setGenerate(SbBool);
  SbBool getGenerate() const;
  void setForWhat(const ForWhat);
  ForWhat getForWhat() const;
private:
  static void nodeAction(SoAction*,SoNode*);
private:
  SbBool fGenerate;
  ForWhat fForWhat;
};

#define SO_ALTERNATEREP_DO_ACTION(a_Action) \
  if(a_Action->isOfType(SoAlternateRepAction::getClassTypeId())) {\
    if(((SoAlternateRepAction*)a_Action)->getGenerate()==TRUE) {\
      if(alternateRep.getValue()==NULL) {\
        generateAlternateRep(a_Action);\
        SoNode* altRep = alternateRep.getValue();\
        if((altRep!=NULL) && altRep->isOfType(SoGroup::getClassTypeId()))\
          altRep->doAction(a_Action);\
      }\
    } else {\
      SoNode* altRep = alternateRep.getValue();\
      if((altRep!=NULL) && altRep->isOfType(SoGroup::getClassTypeId()))\
        altRep->doAction(a_Action);\
      clearAlternateRep();\
    }\
    return;\
  }


#endif
