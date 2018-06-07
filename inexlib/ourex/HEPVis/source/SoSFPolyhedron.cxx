// this :
#include <HEPVis/fields/SoSFPolyhedron.h>

#include <Inventor/SoInput.h>
#include <Inventor/SoOutput.h>

SO_SFIELD_SOURCE(SoSFPolyhedron, SbPolyhedron, const SbPolyhedron &)

void SoSFPolyhedron::initClass(void) {
  SO_SFIELD_INIT_CLASS(SoSFPolyhedron,SoSField);
}

SbBool SoSFPolyhedron::read_vec3d(SoInput * in,SbVec3d& v) {
  if(in->read(v[0])==FALSE) return FALSE;
  if(in->read(v[1])==FALSE) return FALSE;
  if(in->read(v[2])==FALSE) return FALSE;
  return TRUE;
}

SbBool SoSFPolyhedron::read_facet(SoInput * in,SbFacet& facet) {
  int vals[8];
  int* pos = vals;
  for(int i=0;i<4;i++) {
    if(in->read(*pos)==FALSE) return FALSE;
    pos++;
    if(in->read(*pos)==FALSE) return FALSE;
    pos++;
  }
  facet.Set(vals);
  return TRUE;
}

SbBool SoSFPolyhedron::readValue(SoInput * in) {
  //printf("debug : SoSFPolyhedron::readValue...\n");

  int nvert;
  HVPoint3D* pV = 0;

 {if(in->read(nvert)==FALSE) return FALSE;
  if(nvert>0) {
    pV = new HVPoint3D[nvert+1];
    pV[0].setValue(0,0,0);
    HVPoint3D* pos = pV+1;
    for(int i=0;i<nvert;i++,pos++) {
      if(read_vec3d(in,*pos)==FALSE) return FALSE;
    }
  }}

  int nface;
  SbFacet* pF = 0;

 {if(in->read(nface)==FALSE) return FALSE;
  if(nface>0) {
    pF = new SbFacet[nface+1];
    SbFacet* pos = pF+1;
    for(int i=0;i<nface;i++,pos++) {
      if(read_facet(in,*pos)==FALSE) return FALSE;
    }
  }}

  //printf("debug : SoSFPolyhedron::readValue %d %d\n",nvert,nface);

  this->value.Set(nvert,pV,nface,pF);

  return TRUE;
}


void SoSFPolyhedron::write_vec3d(SoOutput * out, const SbVec3d & v) {
  out->write(v[0]);
  if(!out->isBinary()) out->write(' ');
  out->write(v[1]);
  if(!out->isBinary()) out->write(' ');
  out->write(v[2]);
}

void SoSFPolyhedron::write_facet(SoOutput* out,const SbFacet& facet) {
  int v,f;

  facet.GetEdge(0,v,f);
  out->write(v);
  if(!out->isBinary()) out->write(' ');
  out->write(f);
  if(!out->isBinary()) out->write(' ');

  facet.GetEdge(1,v,f);
  out->write(v);
  if(!out->isBinary()) out->write(' ');
  out->write(f);
  if(!out->isBinary()) out->write(' ');

  facet.GetEdge(2,v,f);
  out->write(v);
  if(!out->isBinary()) out->write(' ');
  out->write(f);
  if(!out->isBinary()) out->write(' ');

  facet.GetEdge(3,v,f);
  out->write(v);
  if(!out->isBinary()) out->write(' ');
  out->write(f);
}

void SoSFPolyhedron::writeValue(SoOutput * out) const {
  const SbPolyhedron& p = this->getValue();

  //printf("debug : SoSFPolyhedron::writeValue %d %d\n",
  //  p.GetNoVertices(),p.GetNoFacets());

 {int nvert = p.GetNoVertices();
  out->write(nvert);
  if(!out->isBinary()) out->write(' ');
  if(nvert>0) {
    HVPoint3D* pV = p.GetPV()+1; //do not write the first unused vector.
    for(int i=0;i<nvert;i++,pV++) {
      if(i) if(!out->isBinary()) out->write(' ');
      write_vec3d(out,*pV);
    }
    if(!out->isBinary()) out->write(' ');
  }}

 {int nface = p.GetNoFacets();
  out->write(nface);
  if(!out->isBinary()) out->write(' ');
  if(nface>0) {
    SbFacet* pF = p.GetPF()+1; //do not write the first unused facet.
    for(int i=0;i<nface;i++,pF++) {
      if(i) if(!out->isBinary()) out->write(' ');
      write_facet(out,*pF);
    }
  }}
}
