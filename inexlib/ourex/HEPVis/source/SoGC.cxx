// this :
#include <HEPVis/misc/SoGC.h>

#include <Inventor/nodes/SoLightModel.h>
#include <Inventor/errors/SoDebugError.h>

#include <HEPVis/SbProjector.h>
#include <HEPVis/SbPolyhedron.h>
#include <HEPVis/SbString.h>
#include <HEPVis/SbConvert.h>
#include <HEPVis/misc/SoStyleCache.h>
#include <HEPVis/misc/SoShapeCache.h>
#include <HEPVis/nodes/SoHighlightMaterial.h>
#include <HEPVis/nodes/SoPolyhedron.h>

SoGC::SoGC()
:fSeparator(0)
,fStyleCache(0)
,fLocalSetup(FALSE)
,fShapeCutDone(FALSE)
,fShapeCut(0)
,fShapeCache(0)
{}

SoGC::~SoGC() {
  if(fStyleCache) fStyleCache->unref();
  if(fShapeCache) fShapeCache->unref();
  delete fShapeCut;
}

void SoGC::clear(){
  if(fStyleCache) fStyleCache->unref();
  fStyleCache = 0;
  fSeparator = 0;
  fLocalSetup = FALSE;
  fShapeCutDone = FALSE;
  delete fShapeCut;
  fShapeCut = 0;
}

void SoGC::setDefaultStyleCache(){ 
  setStyleCache(new SoStyleCache());
}

void SoGC::setStyleCache(SoStyleCache* aValue){ 
  if(aValue && (aValue==fStyleCache)) return;
  if(fStyleCache) fStyleCache->unref();
  fStyleCache = aValue;
  if(fStyleCache) fStyleCache->ref();
}

SoStyleCache* SoGC::getStyleCache() const {return fStyleCache;}

void SoGC::reset() {
  SbStyle::reset();
  fShapeCutDone = FALSE;
  delete fShapeCut;
  fShapeCut = 0;
}

SoHighlightMaterial* SoGC::getHighlightMaterial() {
  if(getModeling()==SbModeling_solid) {
    return fStyleCache->getHighlightMaterial
      (getColor(),getHighlightColor(),getTransparency(),FALSE);
  } else {
    //SbModeling_reduced_wire_frame, SbModeling_wire_frame
    //Last TRUE is to set emissiveColor (for VRML production)
    return fStyleCache->getHighlightMaterial
      (getColor(),getHighlightColor(),getTransparency(),TRUE);
  }
}

SoLightModel* SoGC::getLightModel() {
  if(getModeling()==SbModeling_solid) {
    return fStyleCache->getLightModelPhong();
  } else {
    return fStyleCache->getLightModelBaseColor();
  }
}

SoDrawStyle* SoGC::getDrawStyle() {
  if(getModeling()==SbModeling_solid) {
    return fStyleCache->getFilled();
  } else {
    return fStyleCache->getLineStyle(getLinePattern(),(float)getLineWidth());
  }
}

SoPolyhedron* SoGC::getPolyhedron(const SbPolyhedron& aSbPolyhedron) {
  //FIXME : handle sGC->getProjectionType()
  //FIXME : use a SbBoolProc.
  SbPolyhedron* cut = getShapeCut();
  SbPolyhedron sbPolyhedron(aSbPolyhedron);
  if(cut) {
    sbPolyhedron = aSbPolyhedron.subtract(*cut);
  }
  SbBool solid;
  SbBool rwf;
  if(getModeling()==SbModeling_solid) {
    solid = TRUE;
    rwf = TRUE;
  } else if(getModeling()==SbModeling_wire_frame) {
    solid = FALSE;
    rwf = FALSE;
  } else if(getModeling()==SbModeling_reduced_wire_frame) {
    solid = FALSE;
    rwf = TRUE;
  } else {
    solid = TRUE;
    rwf = TRUE;
  }
  if(!fShapeCache) {
    fShapeCache = new SoShapeCache();
    fShapeCache->ref();
  }
  return fShapeCache->getPolyhedron(sbPolyhedron,solid,rwf);
}

SbPolyhedron* SoGC::getShapeCut() {
  if(fShapeCutDone==TRUE) return fShapeCut;
  fShapeCutDone = TRUE;
  SbString scut = getCut();
  SbPList words = SbStringGetWords(scut," ");
  int wordn = words.getLength();
  if(wordn<=0) {
    SbStringDelete(words);
    return 0;
  }
  SbString& word0 = *((SbString*)words[0]);
  if(word0=="none") {
    SbStringDelete(words);
    return 0;
  }
  if(wordn!=8) {
    SoDebugError::postInfo("SoGC::getCutShape",
      "In \"%s\", case not treated.\n",scut.getString());
    SbStringDelete(words);
    return 0;
  }
  // box sx sy sz translation x y z
  SbString& word4 = *((SbString*)words[4]);
  if( (word0=="box") && (word4=="translation") ) {
    SbString& word1 = *((SbString*)words[1]);
    float sx;
    if(SbConvertToFloat(word1,sx)==FALSE)  {
      SoDebugError::postInfo("SoGC::getCutShape",
        "In \"%s\", %s not a number.\n",scut.getString(),word1.getString());
      SbStringDelete(words);
      return 0;
    }
    SbString& word2 = *((SbString*)words[2]);
    float sy;
    if(SbConvertToFloat(word2,sy)==FALSE)  {
      SoDebugError::postInfo("SoGC::getCutShape",
        "In \"%s\", %s not a number.\n",scut.getString(),word2.getString());
      SbStringDelete(words);
      return 0;
    }
    SbString& word3 = *((SbString*)words[3]);
    float sz;
    if(SbConvertToFloat(word3,sz)==FALSE)  {
      SoDebugError::postInfo("SoGC::getCutShape",
        "In \"%s\", %s not a number.\n",scut.getString(),word3.getString());
      SbStringDelete(words);
      return 0;
    }
    // word4 = translation
    SbString& word5 = *((SbString*)words[5]);
    float x;
    if(SbConvertToFloat(word5,x)==FALSE)  {
      SoDebugError::postInfo("SoGC::getCutShape",
        "In \"%s\", %s not a number.\n",scut.getString(),word5.getString());
      SbStringDelete(words);
      return 0;
    }
    SbString& word6 = *((SbString*)words[6]);
    float y;
    if(SbConvertToFloat(word6,y)==FALSE)  {
      SoDebugError::postInfo("SoGC::getCutShape",
        "In \"%s\", %s not a number.\n",scut.getString(),word6.getString());
      SbStringDelete(words);
      return 0;
    }
    SbString& word7 = *((SbString*)words[7]);
    float z;
    if(SbConvertToFloat(word7,z)==FALSE)  {
      SoDebugError::postInfo("SoGC::getCutShape",
        "In \"%s\", %s not a number.\n",scut.getString(),word7.getString());
      SbStringDelete(words);
      return 0;
    }
    fShapeCut = new SbPolyhedronBox(sx,sy,sz);
    fShapeCut->Transform(HEPVis::SbRotation(),SbVec3d(x,y,z));
    SbStringDelete(words);    
    return fShapeCut;
  } else { 
    SoDebugError::postInfo("SoGC::getCutShape",
      "In \"%s\", case not treated.\n",scut.getString());
    SbStringDelete(words);    
    return 0;
  }
}


