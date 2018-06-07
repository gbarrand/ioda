#ifndef HEPVis_SbProjector_h
#define HEPVis_SbProjector_h 

#include <HEPVis/SbProjectorT.h>

#include <Inventor/SbVec3f.h>
#include <Inventor/SbVec3d.h>

namespace HEPVis {

typedef SbProjectorT<float,SbVec3f> SbProjector; //backward compatibility
typedef SbProjectorT<double,SbVec3d> SbProjector_d;
typedef SbProjectorT<float,SbVec3f> SbProjector_f;

}

#endif
