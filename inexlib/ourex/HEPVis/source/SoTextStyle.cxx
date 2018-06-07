// this :
#include <HEPVis/nodes/SoTextStyle.h>

SO_NODE_SOURCE(SoTextStyle)

void SoTextStyle::initClass(){
  SO_NODE_INIT_CLASS(SoTextStyle,SoNode,"Node");
}
SoTextStyle::SoTextStyle(){
  SO_NODE_CONSTRUCTOR(SoTextStyle);

  SO_NODE_ADD_FIELD(color,(SbVec3f(0.5F,0.5F,0.5F)));
  SO_NODE_ADD_FIELD(transparency,(0));
  SO_NODE_ADD_FIELD(fontName,("defaultFont"));
  SO_NODE_ADD_FIELD(fontSize,(10));
  //SO_NODE_ADD_FIELD(angle,(0));
  SO_NODE_ADD_FIELD(visible,(TRUE));
  SO_NODE_ADD_FIELD(encoding,(SbEncoding_none));
  SO_NODE_ADD_FIELD(smoothing,(FALSE));
  SO_NODE_ADD_FIELD(hinting,(FALSE));
  SO_NODE_ADD_FIELD(horizontalJustification,(LEFT));
  SO_NODE_ADD_FIELD(verticalJustification,(BOTTOM));
  SO_NODE_ADD_FIELD(scale,(1));
  SO_NODE_ADD_FIELD(orientationX,(1,0,0));
  SO_NODE_ADD_FIELD(orientationY,(0,1,0));
  SO_NODE_ADD_FIELD(rotated,(FALSE));
  SO_NODE_ADD_FIELD(lineWidth,(1));
  SO_NODE_ADD_FIELD(linePattern,(SbLinePattern_solid));
  SO_NODE_ADD_FIELD(enforced,(FALSE));
  SO_NODE_ADD_FIELD(translation,(SbVec3f(0,0,0)));

  SO_NODE_DEFINE_ENUM_VALUE(HorizontalJustification,LEFT);
  SO_NODE_DEFINE_ENUM_VALUE(HorizontalJustification,CENTER);
  SO_NODE_DEFINE_ENUM_VALUE(HorizontalJustification,RIGHT);

  SO_NODE_DEFINE_ENUM_VALUE(VerticalJustification,BOTTOM);
  SO_NODE_DEFINE_ENUM_VALUE(VerticalJustification,MIDDLE);
  SO_NODE_DEFINE_ENUM_VALUE(VerticalJustification,TOP);

  SO_NODE_SET_SF_ENUM_TYPE(horizontalJustification,HorizontalJustification);
  SO_NODE_SET_SF_ENUM_TYPE(verticalJustification,VerticalJustification);
}

SoTextStyle::~SoTextStyle(){}

void SoTextStyle::reset(){
  color.setValue(SbVec3f(0.5F,0.5F,0.5F));
  transparency.setValue(0);
  fontName.setValue("defaultFont");
  fontSize.setValue(10);
  //angle.setValue(0);
  visible.setValue(TRUE);
  encoding.setValue(SbEncoding_none);
  smoothing.setValue(FALSE);
  hinting.setValue(FALSE);
  horizontalJustification.setValue(LEFT);
  verticalJustification.setValue(BOTTOM);
  scale.setValue(1);
  orientationX.setValue(1,0,0);
  orientationY.setValue(0,1,0);
  rotated.setValue(FALSE);
  lineWidth.setValue(1);
  linePattern.setValue(SbLinePattern_solid);
  enforced.setValue(FALSE);
  translation.setValue(SbVec3f(0,0,0));
}

SbBool SoTextStyle::setFromString(const SbString& aString){
  SbStyle sbStyle;
  if(sbStyle.setFromString(aString)==FALSE) return FALSE;
  color.setValue(sbStyle.getColor());
  transparency.setValue(sbStyle.getTransparency());
  fontName.setValue(sbStyle.getFontName());
  fontSize.setValue(sbStyle.getFontSize());
  encoding.setValue(sbStyle.getEncoding());
  smoothing.setValue(sbStyle.getSmoothing());
  hinting.setValue(sbStyle.getHinting());
  //angle.setValue(sbStyle.getAngle());
  visible.setValue(sbStyle.getVisible());
  scale.setValue(sbStyle.getScale());
  //orientationX.setValue(1,0,0);
  //orientationY.setValue(0,1,0);
  //rotated.setValue(FALSE);
  lineWidth.setValue(sbStyle.getLineWidth());
  linePattern.setValue(sbStyle.getLinePattern());
  enforced.setValue(sbStyle.getEnforced());
  translation.setValue(sbStyle.getTranslation());
  return TRUE;
}

SbStyle SoTextStyle::getSbStyle() const{
  SbStyle sbStyle;
  sbStyle.setColor(color.getValue());
  sbStyle.setTransparency(transparency.getValue());
  sbStyle.setFontName(fontName.getValue());
  sbStyle.setFontSize(fontSize.getValue());
  sbStyle.setEncoding(encoding.getValue());
  sbStyle.setSmoothing(smoothing.getValue());
  sbStyle.setHinting(hinting.getValue());
  sbStyle.setVisible(visible.getValue());
  sbStyle.setScale(scale.getValue());
  sbStyle.setLineWidth(lineWidth.getValue());
  sbStyle.setLinePattern(linePattern.getValue());
  sbStyle.setEnforced(enforced.getValue());
  sbStyle.setTranslation(translation.getValue());
  return sbStyle;
}
