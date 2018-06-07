#ifndef INCLUDE_SoIdealBeamTrackKit
#define INCLUDE_SoIdealBeamTrackKit

#include <HEPVis/nodekits/SoIdealTrackKit.h>
#ifdef _WIN32
#include <SoWinLeaveScope.h>
#endif

/*! \class SoIdealBeamTrackKit SoIdealBeamTrackKit.h HEPVis/nodekits/SoIdealTrackKit.h
 *  \brief A sub-class of SoIdealTrackKit specialized for beam tracks.
 */
class SoIdealBeamTrackKit : public SoIdealTrackKit {
        SO_KIT_HEADER(SoIdealBeamTrackKit);
  public:
        SoIdealBeamTrackKit();
        void initTrack(double vx, double vy, double vz, double px, double py, 
                  double pz, float t0, ParticleChar *p_code); 
SoINTERNAL public:
#if defined(_WIN32) && defined(BUILDING_DLL)
   // When building the node as a DLL under Win32 we must explicitly
   // declare this entry point as visible outside the DLL.  The macro
   // BUILDING_DLL is defined in the node's source file.
      _declspec(dllexport)
#endif
        static void initClass();
  protected:
        ~SoIdealBeamTrackKit();
};
#ifdef _WIN32
#include <SoWinEnterScope.h>
#endif
#endif
