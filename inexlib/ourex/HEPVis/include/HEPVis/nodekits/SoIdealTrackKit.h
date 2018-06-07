#ifndef INCLUDE_SoIdealTrackKit
#define INCLUDE_SoIdealTrackKit


#include <HEPVis/ParticleChar.h>

//adding fields to Kit
#ifdef _WIN32
#include <SoWinEnterScope.h>
#endif

#include <Inventor/SbLinear.h>
#include <Inventor/nodekits/SoBaseKit.h>
#include <Inventor/nodes/SoSubNode.h>
#include <Inventor/fields/SoSFVec3f.h>
#include <Inventor/fields/SoSFFloat.h>
#include <Inventor/fields/SoSFString.h>
//basic kit
#include <Inventor/nodekits/SoShapeKit.h>

#include <Inventor/sensors/SoSensor.h>
#include <Inventor/sensors/SoNodeSensor.h>
#include <Inventor/sensors/SoFieldSensor.h>



// SoIdealTrackKit.h file...
#ifdef _WIN32
#include <SoWinLeaveScope.h>
#endif
/*! \class SoIdealTrackKit SoIdealTrackKit.h HEPVis/nodekits/SoIdealTrackKit.h
 *  \brief Represents an ideal track (i.e., a perfect helix) as a NURB
 *
 *  \author G. Alverson
 *
 *  SoIdealTrackKit is a dynamic kit in that it checks against a global time to 
 *  determine how it represents a track. As the time changes, the track grows to 
 *  match.
 *
 *  Times are measured in 0.01 nanosecond ticks, magnetic fields in Tesla, and distances in meters.
 */
class SoIdealTrackKit : public SoShapeKit {
        SO_KIT_HEADER(SoIdealTrackKit);
        
        SO_KIT_CATALOG_ENTRY_HEADER(debugPoints);
  public:
//!     default constructor
        SoIdealTrackKit();      
        SoSFFloat Phi;
        SoSFFloat Radius;
//! Radius/tan(theta), theta being the dip angle
        SoSFFloat Zeta;  
        SoSFVec3f Vertex;
//! time at beginning of visible track
        SoSFFloat T0;      
//! timespan of visible track
        SoSFFloat DeltaT;  
//! time at beginning of track (not necessarily visible)    
        SoSFFloat T_0;     
//! time at end of track (not necessarily visible)
        SoSFFloat T_1;     
        SoSFString ParticleType;
//! for those w/o access to this kit, invoking this method will provide a basic representation
        SoSFNode alternateRep; 
        void updateTrack();
//! sets the magnetic field (Tesla)
        static void set_bfield(float bf);
//! max r from origin (in m):  a limit is required to keep the tracks from heading to infinity
        static void set_rExtent(float rExtent); 
//! max abs(z) from origin (in m)
        static void set_zExtent(float zExtent); 
/*! initialize the track using one of the initTrack methods
 * \arg \c vx vertex location in the x dimension
   \arg \c vy vertex location in the y dimension
   \arg \c vz vertex location in the z dimension
   \arg \c px momentum (at the vertex) in the x direction
   \arg \c py momentum (at the vertex) in the y direction
   \arg \c pz momentum (at the vertex) in the z direction
   \arg \c t0 time of the creation of the vertex
   \arg \c p_code the type of particle

 */
        void initTrack(double *vx, double *vy, double *vz, 
                 double *px, double *py, double *pz, 
                 float *t0, ParticleChar *p_code){
                 initTrack(*vx, *vy, *vz, *px, *py, *pz, *t0, p_code);}
        void initTrack(double vx, double vy, double vz, 
                 double px, double py, double pz, 
                 float t0, ParticleChar *p_code) ;
        void initTrack(double vx, double vy, double vz, 
                 double px, double py, double pz, 
                 float t0, int p_id) {
                        initTrack(vx, vy, vz, px, py, pz, t0, 
                                ParticleChar::getByGeantID(p_id));};
        void print_me() const;
                virtual void generateAlternateRep(SoAction*);
                virtual void clearAlternateRep();
  SoINTERNAL public:
#if defined(_WIN32) && defined(BUILDING_DLL)
   // When building the node as a DLL under Win32 we must explicitly
   // declare this entry point as visible outside the DLL.  The macro
   // BUILDING_DLL is defined in the node's source file.
      _declspec(dllexport)
#endif
//!  This method must be invoke to initialize the class.
        static void initClass();
  protected:
        ParticleChar  *particleChar;
        float t_0;    //volatile copy of T_0
        float t_1;    //volatile copy of T_1
        float ptot;   //total momentum
        float pt;     //transverse momentum
        float charge;
        ~SoIdealTrackKit();
        SoFieldSensor *fieldSensorT0;
        SoFieldSensor *fieldSensorDeltaT;
        virtual SbBool setUpConnections(SbBool onOff, SbBool doItAlways = FALSE);
        static void fieldsChangedCB(void *data, SoSensor *sens );
        void init_endpts();
        float time_to_angle(float time);
        float z_to_time(float z);
        float time_to_z(float time);
        float angle_to_time(float angle);
        SbVec2f time_to_xy(float angle);
        static const double TWOPI;
        static const int NORDER;
        static const double SPEEDOLIGHT;
        static double  rmax;         //max radius in central tracker
        static double  zmax;         //max z extent
        static double  bfield;       //constant bfield
};
#ifdef _WIN32
#include <SoWinEnterScope.h>
#endif

#endif
