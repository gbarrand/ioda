// this :
#include <HEPVis/nodes/SoAreaStyle.h>

#include <Inventor/errors/SoDebugError.h>

SO_NODE_SOURCE(SoAreaStyle)

void SoAreaStyle::initClass(){
  SO_NODE_INIT_CLASS(SoAreaStyle,SoNode,"Node");
}

SoAreaStyle::SoAreaStyle(){
  SO_NODE_CONSTRUCTOR(SoAreaStyle);

  SO_NODE_ADD_FIELD(color,(SbVec3f(0.5F,0.5F,0.5F)));
  SO_NODE_ADD_FIELD(transparency,(0));
  SO_NODE_ADD_FIELD(areaStyle,(SOLID));
  SO_NODE_ADD_FIELD(visible,(TRUE));

  SO_NODE_DEFINE_ENUM_VALUE(AreaStyle,SOLID);
  SO_NODE_DEFINE_ENUM_VALUE(AreaStyle,HATCHED);
  SO_NODE_DEFINE_ENUM_VALUE(AreaStyle,CHECKER);
  SO_NODE_DEFINE_ENUM_VALUE(AreaStyle,EDGED);

}
SoAreaStyle::~SoAreaStyle(){}

void SoAreaStyle::reset(){
  color.setValue(SbVec3f(0.5F,0.5F,0.5F));
  transparency.setValue(0);
  areaStyle.setValue(SOLID);
  visible.setValue(TRUE);
}

SbBool SoAreaStyle::setFromString(const SbString& aString){
  SbStyle sbStyle;
  if(sbStyle.setFromString(aString)==FALSE) return FALSE;
  color.setValue(sbStyle.getColor());
  transparency.setValue(sbStyle.getTransparency());
  areaStyle.setValue(sbStyle.getAreaStyle());
  visible.setValue(sbStyle.getVisible());
  return TRUE;
}

SbStyle SoAreaStyle::getSbStyle() const{
  SbStyle sbStyle;

  sbStyle.setColor(color.getValue());
  sbStyle.setTransparency(transparency.getValue());
  sbStyle.setAreaStyle((SbAreaStyle)areaStyle.getValue());
  sbStyle.setVisible(visible.getValue());

  return sbStyle;
}
