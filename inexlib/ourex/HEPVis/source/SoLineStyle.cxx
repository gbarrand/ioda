// this :
#include <HEPVis/nodes/SoLineStyle.h>

SO_NODE_SOURCE(SoLineStyle)

void SoLineStyle::initClass(){
  SO_NODE_INIT_CLASS(SoLineStyle,SoNode,"Node");
}

SoLineStyle::SoLineStyle(){
  SO_NODE_CONSTRUCTOR(SoLineStyle);

  SO_NODE_ADD_FIELD(color,(SbVec3f(0.5F,0.5F,0.5F)));
  SO_NODE_ADD_FIELD(transparency,(0));
  SO_NODE_ADD_FIELD(width,(1));
  SO_NODE_ADD_FIELD(pattern,(SbLinePattern_solid));
  SO_NODE_ADD_FIELD(visible,(TRUE));
}

SoLineStyle::~SoLineStyle(){}

void SoLineStyle::reset(){
  color.setValue(SbVec3f(0.5F,0.5F,0.5F));
  transparency.setValue(0);
  width.setValue(1);
  pattern.setValue(SbLinePattern_solid);
  visible.setValue(TRUE);
}

SbBool SoLineStyle::setFromString(const SbString& aString){
  SbStyle sbStyle;
  if(sbStyle.setFromString(aString)==FALSE) return FALSE;
  color.setValue(sbStyle.getColor());
  transparency.setValue(sbStyle.getTransparency());
  width.setValue(sbStyle.getLineWidth());
  pattern.setValue(sbStyle.getLinePattern());
  visible.setValue(sbStyle.getVisible());
  return TRUE;
}

SbStyle SoLineStyle::getSbStyle() const{
  SbStyle sbStyle;

  sbStyle.setColor(color.getValue());
  sbStyle.setTransparency(transparency.getValue());
  sbStyle.setLineWidth(width.getValue());
  sbStyle.setLinePattern(pattern.getValue());
  sbStyle.setVisible(visible.getValue());

  return sbStyle;
}
