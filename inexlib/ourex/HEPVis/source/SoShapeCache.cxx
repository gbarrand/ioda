// this :
#include <HEPVis/misc/SoShapeCache.h>

#include <HEPVis/nodes/SoPolyhedron.h>
#include <HEPVis/SbPolyhedron.h>

//////////////////////////////////////////////////////////////////////////////
SoShapeCache::SoShapeCache(
) 
:fPolyhedrons(0)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  fPolyhedrons = new SoGroup;
  addChild(fPolyhedrons);
}
//////////////////////////////////////////////////////////////////////////////
SoShapeCache::~SoShapeCache(
) 
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
}
//////////////////////////////////////////////////////////////////////////////
SoPolyhedron* SoShapeCache::getPolyhedron(
 const SbPolyhedron& aPolyhedron
,SbBool aSolid
,SbBool aReducedWireFrame
) 
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  int number = fPolyhedrons->getNumChildren();
  for(int index=0;index<number;index++) { 
    SoPolyhedron* soPolyhedron = (SoPolyhedron*)fPolyhedrons->getChild(index);
    const SbPolyhedron& sb = soPolyhedron->polyhedron.getValue();
    if( (sb==aPolyhedron) &&
        (soPolyhedron->solid.getValue()==aSolid) &&
        (soPolyhedron->reducedWireFrame.getValue()==aReducedWireFrame) ){
      return soPolyhedron;
    }
  }
  SoPolyhedron* soPolyhedron = new SoPolyhedron(aPolyhedron);
  if(aSolid==TRUE) {
    soPolyhedron->solid.setValue(TRUE);
  } else {
    soPolyhedron->solid.setValue(FALSE);
    soPolyhedron->reducedWireFrame.setValue(aReducedWireFrame);
  }
  fPolyhedrons->addChild(soPolyhedron);
  return soPolyhedron;
}
