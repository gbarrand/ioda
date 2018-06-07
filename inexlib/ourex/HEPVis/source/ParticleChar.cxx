//#include "stdafx.h"  //use this include in the WIN32 environment
#ifdef _WIN32
#pragma warning(once: 4786)
#endif
#include <math.h>
#include <HEPVis/ParticleChar.h>

CMapToParticleList* ParticleChar::particleList = NULL;
SbBool ParticleChar::inited = FALSE;
          
ParticleChar::ParticleChar(int g, int p, int isa, const char *nm, const char *lbl, float  mas, float q) 
:geantCode(g), pythiaCode(p), isajetCode(isa), particleMaterial(0), particleStyle(0), name(nm), label(lbl), mass(mas), charge(q){
  particleList->insert(CMapToParticleList::value_type(std::string(nm), this));
}

ParticleChar::~ParticleChar(){
  particleMaterial->unref();
  particleMaterial = NULL;
  particleStyle->unref();
  particleStyle = NULL;
}

void ParticleChar::setMaterial(SoMaterial *m){
  if(particleMaterial) particleMaterial->unref();
  particleMaterial = m; 
  m->ref();
}

void ParticleChar::setStyle(SoDrawStyle *s){
  if(particleStyle) particleStyle->unref();
  particleStyle=s;
  s->ref();
}

float ParticleChar::getMass() const {return mass;}

float ParticleChar::getCharge()const {return charge;}

const char* ParticleChar::getName() const {return name.getString();}

void ParticleChar::deleteParticles(){
  if (inited==FALSE) return; //Done.
  CMapToParticleList::const_iterator i = particleList->begin();
  for (i = particleList->begin(); i != particleList->end(); i++){
    ParticleChar* pChar = (*i).second;
    delete pChar;
  }
  delete particleList;
  particleList = NULL;
  inited = FALSE;
}

void ParticleChar::initParticles(){
  if (inited==TRUE) return;
  inited = TRUE;
  particleList = new CMapToParticleList();
  ParticleChar *unknown=new ParticleChar(-1, 0, 0, "unknown", "unk-", 1, -1);
  /*ParticleChar *antiunknown =*/ new ParticleChar(-1, 0, 0, "antiunknown", "unk+", 1, +1);
  ParticleChar *electron=new ParticleChar(3, 11, 12, "electron","e-", 0.000510999F, -1);
  ParticleChar *positron=new ParticleChar(2, -11, -12, "positron","e+", 0.000510999F, +1);
  ParticleChar *muon=new ParticleChar(6, 13, 14, "muon", "mu-", 0.105658F, -1);
  ParticleChar *antimuon=new ParticleChar(5, -13, -14, "antimuon", "mu+", 0.105658F, +1);
  ParticleChar *proton=new ParticleChar(14, 2212, 1120, "proton", "p", 0.9382723F, 1);
  ParticleChar *antiproton=new ParticleChar(15, -2212, -1120, "antiproton", "pbar", 0.9382723F, -1);
  /*ParticleChar *neutron =*/ new ParticleChar(13, 2112, 1220, "neutron", "n", 0.939566F, 0);
  /*ParticleChar *antineutron =*/ new ParticleChar(25, -2112, -1220, "antineutron", "nbar", 0.939566F, 0);
  ParticleChar *pion=new ParticleChar(9, -211, -120, "pion", "pi-", 0.139567F, -1);
  ParticleChar *antipion=new ParticleChar(8, 211, 120, "antipion", "pi+", 0.139567F, +1);
  /*ParticleChar *deuteron =*/  new ParticleChar(45, 0, 0, "deuteron", "d", 1.875613F, 1);
  /*ParticleChar *tritium = */ new ParticleChar(46, 0, 0, "tritium", "tritium", 2.80925F, 1);
  /*ParticleChar *alpha =*/ new ParticleChar(47, 0, 0, "alpha", "alpha", 3.727417F, 2);
  /*ParticleChar *he3 =*/ new ParticleChar(49, 0, 0, "he3", "he3", 2.80923F, 2);

  SoDrawStyle *curveStyle = new SoDrawStyle();
  curveStyle->lineWidth = 0.5;
  curveStyle->pointSize = 4;
  SoMaterial *curveColour = new SoMaterial();
  curveColour->diffuseColor.setValue(0.2F, 0.2F, 0.2F);

  
  CMapToParticleList::const_iterator i = particleList->begin();
  for (i = particleList->begin(); i != particleList->end(); i++){
    ParticleChar *pChar = (*i).second;
    pChar-> setMaterial(curveColour);
    pChar-> setStyle(curveStyle);
  }

  
  curveColour = new SoMaterial();
  curveColour->diffuseColor.setValue(1, 0, 0);
  muon->setMaterial(curveColour);
  muon->setStyle(curveStyle);
  antimuon->setMaterial(curveColour);
  antimuon->setStyle(curveStyle);
   
   curveColour = new SoMaterial();
   curveColour->diffuseColor.setValue(0, 1, 0);
   electron->setMaterial(curveColour);
   electron->setStyle(curveStyle);
   positron->setMaterial(curveColour);
   positron->setStyle(curveStyle);
   
   curveColour = new SoMaterial();
   curveColour->diffuseColor.setValue(0, 0, 1);
   pion->setMaterial(curveColour);
   pion->setStyle(curveStyle);
   antipion->setMaterial(curveColour);
   antipion->setStyle(curveStyle);
   
   curveColour = new SoMaterial();
   curveColour->diffuseColor.setValue(0.5F, 0.5F, 0);
   proton->setMaterial(curveColour);
   proton->setStyle(curveStyle);
   
   curveColour = new SoMaterial();
   curveColour->diffuseColor.setValue(0.45F, 0.55F, 0);
   antiproton->setMaterial(curveColour);
   antiproton->setStyle(curveStyle);

   curveColour = new SoMaterial();
   curveColour->diffuseColor.setValue(0.2F,0.2F,0.2F);
   unknown->setMaterial(curveColour);
   unknown->setStyle(curveStyle);
}
ParticleChar *ParticleChar::getByName(const char *string){
  if (inited==TRUE) {
    CMapToParticleList::iterator it = particleList->find(string);
    if (it != particleList->end()) {
      return (*it).second; }
    else {
      return NULL;}
  }
  else {
    return NULL;
  }      
}
ParticleChar *ParticleChar::getByGeantID(const int p_code){
  CMapToParticleList::const_iterator i = particleList->begin();
  for (i = particleList->begin(); i != particleList->end(); i++){
    ParticleChar *pChar = (*i).second;
    if (pChar->geantCode == p_code) {
      return pChar;}
  }
// nothing found
  return getByName("unknown");
}
ParticleChar *ParticleChar::getByPythiaID(const int p_code){

  CMapToParticleList::const_iterator i = particleList->begin();
  for (i = particleList->begin(); i != particleList->end(); i++){
    ParticleChar *pChar = (*i).second;
    if (pChar->pythiaCode == p_code) {
      return pChar;}
  }
// nothing found
  return getByName("unknown");
}
ParticleChar *ParticleChar::getByIsajetID(const int p_code){

  CMapToParticleList::const_iterator i = particleList->begin();
  for (i = particleList->begin(); i != particleList->end(); i++){
    ParticleChar *pChar = (*i).second;
    if (pChar->isajetCode == p_code) {
      return pChar;}
  }
// nothing found
  return getByName("unknown");
}
