
#include <HEPVis/Detector.h>

extern "C" {
  void HEPVisDetector_initClasses();
}

//////////////////////////////////////////////////////////////////////////////
void HEPVisDetector_initClasses(
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  if(SoHelicalTrack::getClassTypeId()!=SoType::badType()) return; //done.

  SoHelicalTrack::initClass();
  SoCrystalHit::initClass();
  SoJet::initClass();
  SoReconTrack::initClass();
  SoSiStrips::initClass();
  SoSiStripWedgeDetKit::initClass();
  SoVtxReconTrack::initClass();
  SoHadEmCrystalHit::initClass();

  SoDetectorTreeKit::initClass();
  SoMeterStickKit::initClass();
  SoIdealTrackKit::initClass();
  SoIdealBeamTrackKit::initClass();
  SoDblSiStripDetKit::initClass();
  SoSiStripDetKit::initClass();
  SoStereoSiStripDetKit::initClass();
  SoDblSiStripWedgeDetKit::initClass();
  SoSiWedgeStrips::initClass();
  SoDetectorExample::initClass();
}

