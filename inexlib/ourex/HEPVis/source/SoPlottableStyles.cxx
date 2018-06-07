// this :
#include <HEPVis/nodes/SoPlottableStyles.h>

#include <HEPVis/SbColors.h>
#include <HEPVis/SbMath.h>

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
SO_NODE_SOURCE(SoBinsStyle)
void SoBinsStyle::initClass(){SO_NODE_INIT_CLASS(SoBinsStyle,SoStyle,"SoStyle");}
SoBinsStyle::SoBinsStyle(){
  SO_NODE_CONSTRUCTOR(SoBinsStyle);
  // override some default of SoStyle :  
  color.setValue(SbColor_darkslategrey);
  encoding.setValue(SbEncoding_PAW);  //Have encoding for legend box.
}
SoBinsStyle::~SoBinsStyle(){}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
SO_NODE_SOURCE(SoPointsStyle)
void SoPointsStyle::initClass(){SO_NODE_INIT_CLASS(SoPointsStyle,SoStyle,"SoStyle");}
SoPointsStyle::SoPointsStyle(){
  SO_NODE_CONSTRUCTOR(SoPointsStyle);
  // override some default of SoStyle :  
  color.setValue(SbColor_darkslategrey);
  modeling.setValue(SbModeling_points);
  encoding.setValue(SbEncoding_PAW);  //Have encoding for legend box.
}
SoPointsStyle::~SoPointsStyle(){}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
SO_NODE_SOURCE(SoFunctionStyle)
void SoFunctionStyle::initClass(){SO_NODE_INIT_CLASS(SoFunctionStyle,SoStyle,"SoStyle");}
SoFunctionStyle::SoFunctionStyle(){
  SO_NODE_CONSTRUCTOR(SoFunctionStyle);
  // override some default of SoStyle :  
  color.setValue(SbColor_black);
  modeling.setValue(SbModeling_lines);
  encoding.setValue(SbEncoding_PAW);  //Have encoding for legend box.
}
SoFunctionStyle::~SoFunctionStyle(){}


//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
SO_NODE_SOURCE(SoRightHatchStyle)
void SoRightHatchStyle::initClass(){SO_NODE_INIT_CLASS(SoRightHatchStyle,SoStyle,"SoStyle");}
SoRightHatchStyle::SoRightHatchStyle(){
  SO_NODE_CONSTRUCTOR(SoRightHatchStyle);
  // override some default of SoStyle :  
  color.setValue(SbColor_black);
  spacing.setValue(.02F);
}
SoRightHatchStyle::~SoRightHatchStyle(){}


//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
SO_NODE_SOURCE(SoLeftHatchStyle)
void SoLeftHatchStyle::initClass(){SO_NODE_INIT_CLASS(SoLeftHatchStyle,SoStyle,"SoStyle");}
SoLeftHatchStyle::SoLeftHatchStyle(){
  SO_NODE_CONSTRUCTOR(SoLeftHatchStyle);
  // override some default of SoStyle :  
  color.setValue(SbColor_black);
  spacing.setValue(.02F);
  angle.setValue(-FM_PI/4.0F);
}
SoLeftHatchStyle::~SoLeftHatchStyle(){}


//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
SO_NODE_SOURCE(SoErrorsStyle)
void SoErrorsStyle::initClass(){SO_NODE_INIT_CLASS(SoErrorsStyle,SoStyle,"SoStyle");}
SoErrorsStyle::SoErrorsStyle(){
  SO_NODE_CONSTRUCTOR(SoErrorsStyle);
  // override some default of SoStyle :  
  color.setValue(SbColor_black);
  visible.setValue(FALSE);
  markerStyle.setValue(PLUS);
}
SoErrorsStyle::~SoErrorsStyle(){}



