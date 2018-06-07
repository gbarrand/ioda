//#include "stdafx.h"   //for use in a WIN32 environment
#include <math.h>
#include <HEPVis/nodekits/SoIdealBeamTrackKit.h>
//
// SoIdeal Beam Track
//
SO_KIT_SOURCE(SoIdealBeamTrackKit)

void SoIdealBeamTrackKit::initClass()
{
      SO_KIT_INIT_CLASS(SoIdealBeamTrackKit, SoIdealTrackKit, "SoIdealTrackKit");
}

SoIdealBeamTrackKit::SoIdealBeamTrackKit()
{
     SO_KIT_CONSTRUCTOR(SoIdealBeamTrackKit);
     
     SO_KIT_INIT_INSTANCE();
     
     setUpConnections(TRUE,TRUE);
}
SoIdealBeamTrackKit::~SoIdealBeamTrackKit()
{
}                         
void SoIdealBeamTrackKit::initTrack(double vx, double vy, double vz, double px, double py, 
                  double pz, float t0, ParticleChar *p_code) 
{
//set up the starting values as if we were heading in the opposite direction
    SoIdealTrackKit::initTrack(vx, vy, vz, -px, -py, -pz, t0, p_code);
//we're going to touch the fields    
    setUpConnections(FALSE,TRUE);
    SbVec2f tmp_xy = time_to_xy(t_1);
    SbVec3f vertex = SbVec3f(tmp_xy[0], tmp_xy[1], time_to_z(t_1));
    Vertex.setValue(vertex); 
    float t_tmp = t_1;
    t_1 = t_0;
    t_0 = -t_tmp;
    charge = -charge;
    Radius.setValue(-Radius.getValue());
	Zeta.setValue(-Zeta.getValue());
    T0.setValue(t_0);
	T_0.setValue(t_0);
	T_1.setValue(t_1);
    DeltaT.setValue(t_1-t_0);
    setUpConnections(TRUE,TRUE);
    
}                  

