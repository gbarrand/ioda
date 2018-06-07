// this :
#include <HEPVis/nodes/SoStyle.h>

#include <Inventor/errors/SoDebugError.h>

#include <HEPVis/SbStyle.h>
#include <HEPVis/SbMath.h>

#define NoLimit (-1)

SO_NODE_SOURCE(SoStyle)

void SoStyle::initClass(){
  SO_NODE_INIT_CLASS(SoStyle,SoNode,"Node");
}

SoStyle::SoStyle(){
  SO_NODE_CONSTRUCTOR(SoStyle);

  SO_NODE_ADD_FIELD(color,(SbVec3f(0.5F,0.5F,0.5F)));
  SO_NODE_ADD_FIELD(transparency,(0));

  SO_NODE_ADD_FIELD(lineWidth,(1));
  SO_NODE_ADD_FIELD(linePattern,(SbLinePattern_solid));

  SO_NODE_ADD_FIELD(markerSize,(1));
  SO_NODE_ADD_FIELD(markerStyle,(DOT));

  SO_NODE_ADD_FIELD(areaStyle,(SOLID));

  SO_NODE_ADD_FIELD(fontName,("defaultFont"));
  SO_NODE_ADD_FIELD(fontSize,(10));
  SO_NODE_ADD_FIELD(scale,(1));

  SO_NODE_ADD_FIELD(hatching,(NONE));
  SO_NODE_ADD_FIELD(spacing,(0.05F));
  SO_NODE_ADD_FIELD(angle,(FM_PI/4));
  SO_NODE_ADD_FIELD(offset,(0));
  SO_NODE_ADD_FIELD(stripWidth,(0));

  SO_NODE_ADD_FIELD(visible,(TRUE));
  SO_NODE_ADD_FIELD(modeling,(SbModeling_boxes));
  SO_NODE_ADD_FIELD(lightModel,(SbLightModel_phong));
  SO_NODE_ADD_FIELD(encoding,(SbEncoding_none));
  SO_NODE_ADD_FIELD(smoothing,(FALSE));
  SO_NODE_ADD_FIELD(hinting,(FALSE));
  SO_NODE_ADD_FIELD(tickModeling,(SbTickModeling_hippo));

  SO_NODE_ADD_FIELD(painting,(UNIFORM));
  SO_NODE_ADD_FIELD(multiNodeLimit,(NoLimit));
  SO_NODE_ADD_FIELD(divisions,(510));
  SO_NODE_ADD_FIELD(rotationSteps,(24));

  SO_NODE_ADD_FIELD(barOffset,(0.25)); //Percent
  SO_NODE_ADD_FIELD(barWidth,(0.5)); //Percent

  SO_NODE_ADD_FIELD(editable,(FALSE));
  SO_NODE_ADD_FIELD(automated,(TRUE));

  SO_NODE_ADD_FIELD(options,(""));
  SO_NODE_ADD_FIELD(colorMapping,(""));
  SO_NODE_ADD_FIELD(enforced,(FALSE));
  SO_NODE_ADD_FIELD(translation,(SbVec3f(0,0,0)));

  SO_NODE_DEFINE_ENUM_VALUE(MarkerStyle,DOT);
  SO_NODE_DEFINE_ENUM_VALUE(MarkerStyle,PLUS);
  SO_NODE_DEFINE_ENUM_VALUE(MarkerStyle,ASTERISK);
  SO_NODE_DEFINE_ENUM_VALUE(MarkerStyle,CROSS);
  SO_NODE_DEFINE_ENUM_VALUE(MarkerStyle,STAR);
  SO_NODE_DEFINE_ENUM_VALUE(MarkerStyle,CIRCLE_LINE);
  SO_NODE_DEFINE_ENUM_VALUE(MarkerStyle,CIRCLE_FILLED);
  SO_NODE_DEFINE_ENUM_VALUE(MarkerStyle,TRIANGLE_UP_LINE);
  SO_NODE_DEFINE_ENUM_VALUE(MarkerStyle,TRIANGLE_UP_FILLED);
  SO_NODE_DEFINE_ENUM_VALUE(MarkerStyle,TRIANGLE_DOWN_LINE);
  SO_NODE_DEFINE_ENUM_VALUE(MarkerStyle,TRIANGLE_DOWN_FILLED);
  SO_NODE_DEFINE_ENUM_VALUE(MarkerStyle,DAVID_STAR_LINE);
  SO_NODE_DEFINE_ENUM_VALUE(MarkerStyle,DAVID_STAR_FILLED);
  SO_NODE_DEFINE_ENUM_VALUE(MarkerStyle,SWISS_CROSS_LINE);
  SO_NODE_DEFINE_ENUM_VALUE(MarkerStyle,SWISS_CROSS_FILLED);
  SO_NODE_DEFINE_ENUM_VALUE(MarkerStyle,DIAMOND_LINE);
  SO_NODE_DEFINE_ENUM_VALUE(MarkerStyle,DIAMOND_FILLED);
  SO_NODE_DEFINE_ENUM_VALUE(MarkerStyle,SQUARE_LINE);
  SO_NODE_DEFINE_ENUM_VALUE(MarkerStyle,SQUARE_FILLED);
  SO_NODE_DEFINE_ENUM_VALUE(MarkerStyle,PENTA_STAR_LINE);
  SO_NODE_DEFINE_ENUM_VALUE(MarkerStyle,PENTA_STAR_FILLED);

  SO_NODE_DEFINE_ENUM_VALUE(AreaStyle,SOLID);
  SO_NODE_DEFINE_ENUM_VALUE(AreaStyle,HATCHED);
  SO_NODE_DEFINE_ENUM_VALUE(AreaStyle,CHECKER);
  SO_NODE_DEFINE_ENUM_VALUE(AreaStyle,EDGED);

  SO_NODE_DEFINE_ENUM_VALUE(PaintingPolicy,UNIFORM);
  SO_NODE_DEFINE_ENUM_VALUE(PaintingPolicy,BY_VALUE);
  SO_NODE_DEFINE_ENUM_VALUE(PaintingPolicy,BY_LEVEL);
  SO_NODE_DEFINE_ENUM_VALUE(PaintingPolicy,GREY_SCALE);
  SO_NODE_DEFINE_ENUM_VALUE(PaintingPolicy,VIOLET_TO_RED);
  SO_NODE_DEFINE_ENUM_VALUE(PaintingPolicy,GREY_SCALE_INVERSE);

  SO_NODE_DEFINE_ENUM_VALUE(HatchingPolicy,NONE);
  SO_NODE_DEFINE_ENUM_VALUE(HatchingPolicy,RIGHT);
  SO_NODE_DEFINE_ENUM_VALUE(HatchingPolicy,LEFT);
  SO_NODE_DEFINE_ENUM_VALUE(HatchingPolicy,LEFT_AND_RIGHT);

  SO_NODE_SET_SF_ENUM_TYPE(markerStyle,MarkerStyle);
  SO_NODE_SET_SF_ENUM_TYPE(areaStyle,AreaStyle);
  SO_NODE_SET_SF_ENUM_TYPE(painting,PaintingPolicy);
  SO_NODE_SET_SF_ENUM_TYPE(hatching,HatchingPolicy);

}

SoStyle::~SoStyle(){}

void SoStyle::set(const SbStyle& aSb){
  color.setValue(aSb.getColor());
  transparency.setValue(aSb.getTransparency());
  lineWidth.setValue(aSb.getLineWidth());
  linePattern.setValue(aSb.getLinePattern());
  markerSize.setValue(aSb.getMarkerSize());
  markerStyle.setValue(aSb.getMarkerStyle());
  areaStyle.setValue(aSb.getAreaStyle());
  fontName.setValue(aSb.getFontName());
  fontSize.setValue(aSb.getFontSize());
  scale.setValue(aSb.getScale());
  hatching.setValue(aSb.getHatchingPolicy());
  spacing.setValue(aSb.getSpacing());
  angle.setValue(aSb.getAngle());
  offset.setValue(aSb.getOffset());
  stripWidth.setValue(aSb.getStripWidth());
  visible.setValue(aSb.getVisible());
  modeling.setValue(aSb.getModeling());
  lightModel.setValue(aSb.getLightModel());
  tickModeling.setValue(aSb.getTickModeling());
  encoding.setValue(aSb.getEncoding());
  smoothing.setValue(aSb.getSmoothing());
  hinting.setValue(aSb.getHinting());
  painting.setValue(aSb.getPaintingPolicy());
  multiNodeLimit.setValue(aSb.getMultiNodeLimit());
  divisions.setValue(aSb.getDivisions());
  rotationSteps.setValue(aSb.getRotationSteps());
  barOffset.setValue(aSb.getBarOffset());
  barWidth.setValue(aSb.getBarWidth());
  editable.setValue(aSb.getEditable());
  automated.setValue(aSb.getAutomated());
  options.setValue(aSb.getOptions());
  colorMapping.setValue(aSb.getColorMapping());
  enforced.setValue(aSb.getEnforced());
  translation.setValue(SbVec3f(0,0,0));
}

SbBool SoStyle::setFromString(const SbString& aString){
  SbStyle sbStyle;
  if(sbStyle.setFromString(aString)==FALSE) return FALSE;
  set(sbStyle);
  return TRUE;
}

SbStyle SoStyle::getSbStyle() const{
  SbStyle sbStyle;

  sbStyle.setColor(color.getValue());
  sbStyle.setTransparency(transparency.getValue());
  sbStyle.setLineWidth(lineWidth.getValue());
  sbStyle.setLinePattern(linePattern.getValue());
  sbStyle.setMarkerSize(markerSize.getValue());
  sbStyle.setMarkerStyle(SbMarkerStyle(markerStyle.getValue()));
  sbStyle.setAreaStyle(SbAreaStyle(areaStyle.getValue()));
  sbStyle.setFontName(fontName.getValue());
  sbStyle.setFontSize(fontSize.getValue());
  sbStyle.setScale(scale.getValue());
  sbStyle.setHatchingPolicy(SbHatchingPolicy(hatching.getValue()));
  sbStyle.setSpacing(spacing.getValue());
  sbStyle.setAngle(angle.getValue());
  sbStyle.setOffset(offset.getValue());
  sbStyle.setStripWidth(stripWidth.getValue());
  sbStyle.setVisible(visible.getValue());
  sbStyle.setModeling(modeling.getValue());
  sbStyle.setLightModel(lightModel.getValue());
  sbStyle.setTickModeling(tickModeling.getValue());
  sbStyle.setEncoding(encoding.getValue());
  sbStyle.setSmoothing(smoothing.getValue());
  sbStyle.setHinting(hinting.getValue());
  sbStyle.setPaintingPolicy(SbPaintingPolicy(painting.getValue()));
  sbStyle.setMultiNodeLimit(multiNodeLimit.getValue());
  sbStyle.setDivisions(divisions.getValue());
  sbStyle.setRotationSteps(rotationSteps.getValue());
  sbStyle.setBarOffset(barOffset.getValue());
  sbStyle.setBarWidth(barWidth.getValue());
  sbStyle.setEditable(editable.getValue());
  sbStyle.setAutomated(automated.getValue());
  sbStyle.setOptions(options.getValue());
  sbStyle.setColorMapping(colorMapping.getValue());
  sbStyle.setEnforced(enforced.getValue());
  sbStyle.setTranslation(translation.getValue());

  return sbStyle;
}
