#include <HEPVis/Plotter.h>

extern "C" {
  void HEPVisPlotter_initClasses();
}

//////////////////////////////////////////////////////////////////////////////
void HEPVisPlotter_initClasses(
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  if(SoPlotter::getClassTypeId()!=SoType::badType()) return; //done.

  SoPlotter::initClass();
  SoLegendRegion::initClass();
  SoInfosRegion::initClass();
  SoPlotterRegion::initClass();
}

