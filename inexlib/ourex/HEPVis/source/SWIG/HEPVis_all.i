//////////////////////////////////////////////////////////////////////////////
/// Pre SWIG /////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

// Utils :
RefCount(SoLineStyle)
RefCount(SoTextStyle)
RefCount(SoAreaStyle)
RefCount(SoStyle)
RefCount(SoMarkerSet)
RefCount(SoImage)
RefCount(SoImageWriter)
RefCount(SoViewportRegion)
RefCount(SoTextHershey)
RefCount(SoTextVtk)
RefCount(SoTextTTF)
RefCount(SoGrid)
RefCount(SoEdgedFaceSet)
RefCount(SoHighlightMaterial)
RefCount(SoSplineCurve)
RefCount(SoPyramid)
RefCount(SoAxis)
RefCount(SoAxes2D)
RefCount(SoAxes3D)
RefCount(SoSimpleButton)
RefCount(SoRegion)
RefCount(SoTextRegion)
RefCount(SoImageRegion)
RefCount(SoDisplayRegion)
RefCount(SoRulerRegion)
RefCount(SoFormRegion)
RefCount(SoCanvas)
RefCount(SoPage)

// Geometry :
RefCount(SoPolyhedron)
RefCount(SoTubs)
RefCount(SoBox)
RefCount(SoTrd)
RefCount(SoBox)
RefCount(SoCons)
RefCount(SoTrap)
RefCount(SoTrd)
RefCount(SoTubs)
RefCount(SoArrow)
RefCount(SoEllipsoid)
RefCount(SoEllipsoidSegment)
RefCount(SoEllipticalPrism)
RefCount(SoTorus)
RefCount(SoPcon)
RefCount(SoPcons)
RefCount(SoPolyVol)
RefCount(So3DErrorBar)
RefCount(SoCoordinateAxis)
RefCount(SoWedge)
RefCount(So2DArrow)
RefCount(SoCircleArc)
RefCount(SoDisk)
RefCount(SoPolygon)
RefCount(SoQuad)
RefCount(SoRing)
RefCount(SoTriangle)

// Detector :
RefCount(SoHelicalTrack)
RefCount(SoCrystalHit)
RefCount(SoJet)
RefCount(SoReconTrack)
RefCount(SoSiStrips)
RefCount(SoSiWedgeStrips)
RefCount(SoVtxReconTrack)
RefCount(SoHadEmCrystalHit)
RefCount(SoDetectorTreeKit)
RefCount(SoMeterStickKit)
RefCount(SoIdealTrackKit)
RefCount(SoIdealBeamTrackKit)
RefCount(SoDblSiStripDetKit)
RefCount(SoSiStripDetKit)
RefCount(SoStereoSiStripDetKit)
RefCount(SoSiStripWedgeDetKit)
RefCount(SoDblSiStripWedgeDetKit)
RefCount(SoDetectorExample)

// Plotter :
RefCount(SoPlotter)
RefCount(SoLegendRegion)
RefCount(SoInfosRegion)
RefCount(SoPlotterRegion)

// GUI :
RefCount(SoBackPrimitive)
RefCount(SoPrimitive)
RefCount(SoForm)
RefCount(SoPushButton)
RefCount(SoArrowButton)
RefCount(SoScrolledList)
RefCount(SoSelectionBox)
RefCount(SoList)
RefCount(SoText)
RefCount(SoCommand)
RefCount(SoFileSelectionBox)
RefCount(SoShell)

// Else :
RefCount(SoStyleCache)


%include std_string.i
%include std_vector.i
%include std_pair.i
%template() std::vector<std::string>;
%template() std::vector<double>;
%template() std::vector<int>;
%template() std::vector<SbStyleColor>;
%template() std::pair<SbString,SbColor>;

%include <HEPVis/SbAll.h>
%include <HEPVis/Utils.h>
%include <HEPVis/Geometry.h>
%include <HEPVis/Detector.h>
%include <HEPVis/Plotter.h>
%include <HEPVis/GUI.h>

%include <HEPVis/misc/SoTools.h>
%include <HEPVis/misc/SoStyleCache.h>

extern "C" {
  void HEPVisUtils_initClasses();
  void HEPVisGeometry_initClasses();
  void HEPVisDetector_initClasses();
  void HEPVisPlotter_initClasses();
  void HEPVisGUI_initClasses();

  void SoPlotter_initClasses();
}

%inline {
void initClasses() {
  HEPVisUtils_initClasses();
  HEPVisGeometry_initClasses();
  HEPVisDetector_initClasses();
  HEPVisPlotter_initClasses();
  HEPVisGUI_initClasses();
}
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

// WARNING : %import and not %include to not rewrap.

%ignore vsprintf; //else the plugin does not compile.
%ignore vsnpf;    //idem.

%import <inlib/histo/base_histo>

%template(base_histo_duidd) inlib::histo::base_histo<double,unsigned int,unsigned int,double,double>;

%import <inlib/histo/b1>
%import <inlib/histo/b2>

%template(b1_duidd) inlib::histo::b1<double,unsigned int,unsigned int,double,double>;
%template(b2_duidd) inlib::histo::b2<double,unsigned int,unsigned int,double,double>;

%import <inlib/histo/h1>
%import <inlib/histo/h2>
%import <inlib/histo/p1>
%import <inlib/histo/p2>

%template(h1_duidd)  inlib::histo::h1<double,unsigned int,unsigned int,double,double>;
%template(h2_duidd)  inlib::histo::h2<double,unsigned int,unsigned int,double,double>;
%template(p1_duiddd) inlib::histo::p1<double,unsigned int,unsigned int,double,double,double>;
%template(p2_duiddd) inlib::histo::p2<double,unsigned int,unsigned int,double,double,double>;

%import <inlib/histo/h1d>
%import <inlib/histo/h2d>
%import <inlib/histo/p1d>
%import <inlib/histo/p2d>
%import <inlib/safe_cast>

%include <HEPVis/inlib_plot>

//////////////////////////////////////////////////////////////
/// Post SWIG ////////////////////////////////////////////////
//////////////////////////////////////////////////////////////


