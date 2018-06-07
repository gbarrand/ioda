
#include <HEPVis/Utils.h>

extern "C" {
  void HEPVisUtils_initClasses();
}

//////////////////////////////////////////////////////////////////////////////
void HEPVisUtils_initClasses(
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  if(HEPVis_SoFile::getClassTypeId()!=SoType::badType()) return; //done.
 
  HEPVis_SoFile::initClass();
  SoLineStyle::initClass();
  SoTextStyle::initClass();
  SoAreaStyle::initClass();
  SoStyle::initClass();
  SoBinsStyle::initClass();
  SoPointsStyle::initClass();
  SoFunctionStyle::initClass();
  SoRightHatchStyle::initClass();
  SoLeftHatchStyle::initClass();
  SoErrorsStyle::initClass();
  SoSceneGraph::initClass();
  HEPVis_SoMarkerSet::initClass();
  HEPVis_SoImage::initClass();
  SoImageWriter::initClass();
  SoViewportRegion::initClass();
  SoTextHershey::initClass();
  SoTextTTF::initClass();
  SoTextVtk::initClass();
  SoGrid::initClass();
  SoEdgedFaceSet::initClass();
  SoHighlightMaterial::initClass();
  SoSplineCurve::initClass();
  SoPyramid::initClass();

  SoAxis::initClass();
  SoAxes2D::initClass();
  SoAxes3D::initClass();
  SoSimpleButton::initClass();
  SoRegion::initClass();
  SoTextRegion::initClass();
  SoImageRegion::initClass();
  SoDisplayRegion::initClass();
  SoRulerRegion::initClass();
  SoFormRegion::initClass();
  SoCanvas::initClass();
  SoPage::initClass();

  SoPainterAction::initClass();
  SoGL2PSAction::initClass();
  SoCollectAction::initClass();
  SoCounterAction::initClass();
  SoAlternateRepAction::initClass();
  HEPVis_SoIntersectionDetectionAction::initClass();
}

