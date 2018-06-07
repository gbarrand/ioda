#include <HEPVis/actions/SoGL2PSAction.h>
#include <HEPVis/actions/SoAlternateRepAction.h>
#include <HEPVis/nodes/SoHighlightMaterial.h>
#include <HEPVis/nodes/SoMarkerSet.h>
#include <HEPVis/nodes/SoSceneGraph.h>

#include <HEPVis/nodes/SoStyle.h>
#include <HEPVis/nodes/SoLineStyle.h>
#include <HEPVis/nodes/SoTextStyle.h>
#include <HEPVis/nodes/SoPlottableStyles.h>

#include <HEPVis/nodes/SoGrid.h>
#include <HEPVis/nodes/SoTextHershey.h>
#include <HEPVis/nodes/SoTextTTF.h>
#include <HEPVis/nodekits/SoAxis.h>
#include <HEPVis/nodekits/SoPlotter.h>

extern "C" {
  void SoPlotter_initClasses();
}

void SoPlotter_initClasses(){

  // SoPlotter :
  SoGL2PSAction::initClass();
  SoAlternateRepAction::initClass();
  SoHighlightMaterial::initClass();
  HEPVis_SoMarkerSet::initClass();
  SoSceneGraph::initClass();

  SoStyle::initClass();
  SoLineStyle::initClass();
  SoTextStyle::initClass();
  SoBinsStyle::initClass();
  SoPointsStyle::initClass();
  SoFunctionStyle::initClass();
  SoRightHatchStyle::initClass();
  SoLeftHatchStyle::initClass();
  SoErrorsStyle::initClass();

  SoGrid::initClass();
  SoTextHershey::initClass();
  SoTextTTF::initClass();
  SoAxis::initClass();
  SoPlotter::initClass();
}
