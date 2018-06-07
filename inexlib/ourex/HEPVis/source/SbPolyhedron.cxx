// this :
#include <HEPVis/SbPolyhedron.h>

#include <iostream> 

#define perMillion 0.000001
#define deg (M_PI/180.0)

#include <float.h> //G.Barrand : to have DBL_EPSILON on Windows.

// G.Barrand : introduce iabs to avoid a mess with cmath and some compiler.
inline int iabs(int a) {
  return a < 0 ? -a : a;
}
HVPoint3D::HVPoint3D(): SbVec3d(0,0,0){}
HVPoint3D::HVPoint3D(double x,double y,double z): SbVec3d(x,y,z){}
HVPoint3D::HVPoint3D(const HVPoint3D& v): SbVec3d(v){}
HVPoint3D::HVPoint3D(const SbVec3d& v): SbVec3d(v){}
HVPoint3D& HVPoint3D::operator=(const HVPoint3D& v) {
  setValue(v[0],v[1],v[2]);
  return *this;
}
HVPoint3D& HVPoint3D::operator=(const SbVec3d& v) {
  setValue(v[0],v[1],v[2]);
  return *this;
}
HVPoint3D operator +(const HVPoint3D& v1,const HVPoint3D& v2) {
  return HVPoint3D(v1[0] + v2[0],v1[1] + v2[1],v1[2] + v2[2]);  
}


//--------------------------------------------------------------------//
// JFB:                                                               //
// SbPolyhedron was SbPolyhedron, retrofitted to Open Inventor       //
// infrastructure:                                                    //
//--------------------------------------------------------------------//


//
// ********************************************************************
// * DISCLAIMER                                                       *
// *                                                                  *
// * The following disclaimer summarizes all the specific disclaimers *
// * of contributors to this software. The specific disclaimers,which *
// * govern, are listed with their locations in:                      *
// *   http://cern.ch/geant4/license                                  *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.                                                             *
// *                                                                  *
// * This  code  implementation is the  intellectual property  of the *
// * GEANT4 collaboration.                                            *
// * By copying,  distributing  or modifying the Program (or any work *
// * based  on  the Program)  you indicate  your  acceptance of  this *
// * statement, and all its terms.                                    *
// ********************************************************************
//
//
//
// 
//
// G4 Polyhedron library
//
// History:
// 23.07.96 E.Chernyaev <Evgueni.Tcherniaev@cern.ch> - initial version
//
// 30.09.96 E.Chernyaev
// - added GetNextVertexIndex, GetVertex by Yasuhide Sawada
// - added GetNextUnitNormal, GetNextEdgeIndeces, GetNextEdge
//
// 15.12.96 E.Chernyaev
// - added GetNumberOfRotationSteps, RotateEdge, RotateAroundZ, SetReferences
// - rewritten G4PolyhedronCons;
// - added G4PolyhedronPara, ...Trap, ...Pgon, ...Pcon, ...Sphere, ...Torus
//
// 01.06.97 E.Chernyaev
// - modified RotateAroundZ, added SetSideFacets
//
// 19.03.00 E.Chernyaev
// - implemented boolean operations (add, subtract, intersect) on polyhedra;
//
// 25.05.01 E.Chernyaev
// - added GetSurfaceArea() and GetVolume();
//


/***********************************************************************
 *                                                                     *
 * Name: SbPolyhedron operator <<                   Date:    09.05.96 *
 * Author: E.Chernyaev (IHEP/Protvino)               Revised:          *
 *                                                                     *
 * Function: Print contents of G4 polyhedron                           *
 *                                                                     *
 ***********************************************************************/
int operator ==(const SbFacet& v1, const SbFacet& v2) { //G.Barrand
  for(int i=0;i<4;i++) {
    if(v1.edge[i].v != v2.edge[i].v) return FALSE;
    if(v1.edge[i].f != v2.edge[i].f) return FALSE;
  }
  return TRUE;
}
int operator !=(const SbFacet& v1, const SbFacet& v2) { //G.Barrand
  return !(v1 == v2);
}
std::ostream & operator<<(std::ostream & ostr, const SbFacet & facet) {
  for (int k=0; k<4; k++) {
    ostr << " " << facet.edge[k].v << "/" << facet.edge[k].f;
  }
  return ostr;
}
std::ostream & operator<<(std::ostream & ostr, const SbPolyhedron & ph) {
  ostr << std::endl;
  ostr << "Nverteces=" << ph.nvert << ", Nfacets=" << ph.nface << std::endl;
  int i;
  for (i=1; i<=ph.nvert; i++) {
     ostr << "xyz(" << i << ")="
          << ph.pV[i][0] << ' ' << ph.pV[i][1] << ' ' << ph.pV[i][2]
          << std::endl;
  }
  for (i=1; i<=ph.nface; i++) {
    ostr << "face(" << i << ")=" << ph.pF[i] << std::endl;
  }
  return ostr;
}

SbPolyhedron::SbPolyhedron(const SbPolyhedron &from)
/***********************************************************************
 *                                                                     *
 * Name: SbPolyhedron copy constructor             Date:    23.07.96  *
 * Author: E.Chernyaev (IHEP/Protvino)              Revised:           *
 *                                                                     *
 ***********************************************************************/
{
  name = 0; //G.Barrand
  if(from.name) name = new SbString(*from.name); //G.Barrand
  
  if (from.nvert > 0 && from.nface > 0) {
    nvert = from.nvert;
    nface = from.nface;
    pV = new HVPoint3D[nvert + 1];
    pF = new SbFacet[nface + 1];
    int i;
    for (i=1; i<=nvert; i++) pV[i] = from.pV[i];
    for (i=1; i<=nface; i++) pF[i] = from.pF[i];
  }else{
    nvert = 0; nface = 0; pV = 0; pF = 0;
  }
}

int operator ==(const SbPolyhedron& v1, const SbPolyhedron& v2) { //G.Barrand
  return v1.isEqual(v2);
}
int operator !=(const SbPolyhedron& v1, const SbPolyhedron& v2) { //G.Barrand
  return !(v1 == v2);
}

SbPolyhedron & SbPolyhedron::operator=(const SbPolyhedron &from)
/***********************************************************************
 *                                                                     *
 * Name: SbPolyhedron operator =                   Date:    23.07.96  *
 * Author: E.Chernyaev (IHEP/Protvino)              Revised:           *
 *                                                                     *
 * Function: Copy contents of one GEANT4 polyhedron to another         *
 *                                                                     *
 ***********************************************************************/
{
  if (this == &from) return *this;

  delete name;name = 0; //G.Barrand
  if(from.name) name = new SbString(*from.name); //G.Barrand

  delete [] pV;
  delete [] pF;
  if (from.nvert > 0  && from.nface > 0) {
    nvert = from.nvert;
    nface = from.nface;
    pV = new HVPoint3D[nvert + 1];
    pF = new SbFacet[nface + 1];
    int i;
    for (i=1; i<=nvert; i++) pV[i] = from.pV[i];
    for (i=1; i<=nface; i++) pF[i] = from.pF[i];
  }else{
    nvert = 0; nface = 0; pV = 0; pF = 0;
  }
  return *this;
}

//G.Barrand
bool SbPolyhedron::isEqual(const SbPolyhedron &from) const {
  if (this == &from) return true;
  if(nvert!=from.nvert) return false;
  if(nface!=from.nface) return false;
  int i;
  for (i=1; i<=nvert; i++) {
    if(pV[i]!=from.pV[i]) return false;
  }
  for (i=1; i<=nface; i++) {
    if(!pF[i].isEqual(from.pF[i])) return false;
  }
  return true;
}
bool SbPolyhedron::isConsistent(const char* aMsg) const {
  for(int i=1;i<=nface;i++) {
    const SbFacet& facet = pF[i];
    for(int j=0;j<4;j++) {
      int v,f;
      facet.GetEdge(j,v,f);
      if(iabs(v)>nvert) return false;
      if(iabs(f)>nvert) return false;
    }
  }
  return true;
}
void SbPolyhedron::dump() const {
  std::cout << " nface = " << nface << std::endl;
  for(int i=1;i<=nface;i++) {
    const SbFacet& facet = pF[i];
    for(int j=0;j<4;j++) {
      int v,f;
      facet.GetEdge(j,v,f);
      std::cout << " " << v << " " << f << std::endl;
    }
  }
}

int
SbPolyhedron::FindNeighbour(int iFace, int iNode, int iOrder) const
/***********************************************************************
 *                                                                     *
 * Name: SbPolyhedron::FindNeighbour                Date:    22.11.99 *
 * Author: E.Chernyaev                               Revised:          *
 *                                                                     *
 * Function: Find neighbouring face                                    *
 *                                                                     *
 ***********************************************************************/
{
  int i;
  for (i=0; i<4; i++) {
    if (iNode == iabs(pF[iFace].edge[i].v)) break;
  }
  if (i == 4) {
    std::cerr
      << "SbPolyhedron::FindNeighbour: face " << iFace
      << " has no node " << iNode
      << std::endl; 
    return 0;
  }
  if (iOrder < 0) {
    if ( --i < 0) i = 3;
    if (pF[iFace].edge[i].v == 0) i = 2;
  }
  return (pF[iFace].edge[i].v > 0) ? 0 : pF[iFace].edge[i].f;
}

HVNormal3D SbPolyhedron::FindNodeNormal(int iFace, int iNode) const
/***********************************************************************
 *                                                                     *
 * Name: SbPolyhedron::FindNodeNormal               Date:    22.11.99 *
 * Author: E.Chernyaev                               Revised:          *
 *                                                                     *
 * Function: Find normal at given node                                 *
 *                                                                     *
 ***********************************************************************/
{
  HVNormal3D  normal = GetUnitNormal(iFace);
  int          k = iFace, iOrder = 1, n = 1;

  for(;;) {
    k = FindNeighbour(k, iNode, iOrder);
    if (k == iFace) break; 
    if (k > 0) {
      n++;
      normal += GetUnitNormal(k);
    }else{
      if (iOrder < 0) break;
      k = iFace;
      iOrder = -iOrder;
    }
  }
  normal.normalize();
  return normal;
}

void SbPolyhedron::SetNumberOfRotationSteps(int n)
/***********************************************************************
 *                                                                     *
 * Name: SbPolyhedron::SetNumberOfRotationSteps     Date:    24.06.97 *
 * Author: J.Allison (Manchester University)         Revised:          *
 *                                                                     *
 * Function: Set number of steps for whole circle                      *
 *                                                                     *
 ***********************************************************************/
{
  const int nMin = 3;
  if (n < nMin) {
    std::cerr 
      << "SbPolyhedron::SetNumberOfRotationSteps: attempt to set the\n"
      << "number of steps per circle < " << nMin << "; forced to " << nMin
      << std::endl;
    fNumberOfRotationSteps = nMin;
  }else{
    fNumberOfRotationSteps = n;
  }    
}

void SbPolyhedron::AllocateMemory(int Nvert, int Nface)
/***********************************************************************
 *                                                                     *
 * Name: SbPolyhedron::AllocateMemory               Date:    19.06.96 *
 * Author: E.Chernyaev (IHEP/Protvino)               Revised:          *
 *                                                                     *
 * Function: Allocate memory for GEANT4 polyhedron                     *
 *                                                                     *
 * Input: Nvert - number of nodes                                      *
 *        Nface - number of faces                                      *
 *                                                                     *
 ***********************************************************************/
{
  nvert = Nvert;
  nface = Nface;
  pV    = new HVPoint3D[nvert+1];
  pF    = new SbFacet[nface+1];
}

void SbPolyhedron::CreatePrism()
/***********************************************************************
 *                                                                     *
 * Name: SbPolyhedron::CreatePrism                  Date:    15.07.96 *
 * Author: E.Chernyaev (IHEP/Protvino)               Revised:          *
 *                                                                     *
 * Function: Set facets for a prism                                    *
 *                                                                     *
 ***********************************************************************/
{
  enum {DUMMY, BOTTOM, LEFT, BACK, RIGHT, FRONT, TOP};

  pF[1] = SbFacet(1,LEFT,  4,BACK,  3,RIGHT,  2,FRONT);
  pF[2] = SbFacet(5,TOP,   8,BACK,  4,BOTTOM, 1,FRONT);
  pF[3] = SbFacet(8,TOP,   7,RIGHT, 3,BOTTOM, 4,LEFT);
  pF[4] = SbFacet(7,TOP,   6,FRONT, 2,BOTTOM, 3,BACK);
  pF[5] = SbFacet(6,TOP,   5,LEFT,  1,BOTTOM, 2,RIGHT);
  pF[6] = SbFacet(5,FRONT, 6,RIGHT, 7,BACK,   8,LEFT);
}

void SbPolyhedron::RotateEdge(int k1, int k2, double r1, double r2,
                              int v1, int v2, int vEdge,
                              bool ifWholeCircle, int ns, int &kface)
/***********************************************************************
 *                                                                     *
 * Name: SbPolyhedron::RotateEdge                   Date:    05.12.96 *
 * Author: E.Chernyaev (IHEP/Protvino)               Revised:          *
 *                                                                     *
 * Function: Create set of facets by rotation of an edge around Z-axis *
 *                                                                     *
 * Input: k1, k2 - end vertices of the edge                            *
 *        r1, r2 - radiuses of the end vertices                        *
 *        v1, v2 - visibility of edges produced by rotation of the end *
 *                 vertices                                            *
 *        vEdge  - visibility of the edge                              *
 *        ifWholeCircle - is true in case of whole circle rotation     *
 *        ns     - number of discrete steps                            *
 *        r[]    - r-coordinates                                       *
 *        kface  - current free cell in the pF array                   *
 *                                                                     *
 ***********************************************************************/
{
  if (r1 == 0. && r2 == 0) return;

  int i;
  int i1  = k1;
  int i2  = k2;
  int ii1 = ifWholeCircle ? i1 : i1+ns;
  int ii2 = ifWholeCircle ? i2 : i2+ns;
  int vv  = ifWholeCircle ? vEdge : 1;

  if (ns == 1) {
    if (r1 == 0.) {
      pF[kface++]   = SbFacet(i1,0,    v2*i2,0, (i2+1),0);
    }else if (r2 == 0.) {
      pF[kface++]   = SbFacet(i1,0,    i2,0,    v1*(i1+1),0);
    }else{
      pF[kface++]   = SbFacet(i1,0,    v2*i2,0, (i2+1),0, v1*(i1+1),0);
    }
  }else{
    if (r1 == 0.) {
      pF[kface++]   = SbFacet(vv*i1,0,    v2*i2,0, vEdge*(i2+1),0);
      for (i2++,i=1; i<ns-1; i2++,i++) {
        pF[kface++] = SbFacet(vEdge*i1,0, v2*i2,0, vEdge*(i2+1),0);
      }
      pF[kface++]   = SbFacet(vEdge*i1,0, v2*i2,0, vv*ii2,0);
    }else if (r2 == 0.) {
      pF[kface++]   = SbFacet(vv*i1,0,    vEdge*i2,0, v1*(i1+1),0);
      for (i1++,i=1; i<ns-1; i1++,i++) {
        pF[kface++] = SbFacet(vEdge*i1,0, vEdge*i2,0, v1*(i1+1),0);
      }
      pF[kface++]   = SbFacet(vEdge*i1,0, vv*i2,0,    v1*ii1,0);
    }else{
      pF[kface++]   = SbFacet(vv*i1,0,    v2*i2,0, vEdge*(i2+1),0,v1*(i1+1),0);
      for (i1++,i2++,i=1; i<ns-1; i1++,i2++,i++) {
        pF[kface++] = SbFacet(vEdge*i1,0, v2*i2,0, vEdge*(i2+1),0,v1*(i1+1),0);
      }  
      pF[kface++]   = SbFacet(vEdge*i1,0, v2*i2,0, vv*ii2,0,      v1*ii1,0);
    }
  }
}

void SbPolyhedron::SetSideFacets(int ii[4], int vv[4],
                                 int *kk, double *r,
                                 double dphi, int ns, int &kface)
/***********************************************************************
 *                                                                     *
 * Name: SbPolyhedron::SetSideFacets                Date:    20.05.97 *
 * Author: E.Chernyaev (IHEP/Protvino)               Revised:          *
 *                                                                     *
 * Function: Set side facets for the case of incomplete rotation       *
 *                                                                     *
 * Input: ii[4] - indeces of original verteces                         *
 *        vv[4] - visibility of edges                                  *
 *        kk[]  - indeces of nodes                                     *
 *        r[]   - radiuses                                             *
 *        dphi  - delta phi                                            *
 *        ns     - number of discrete steps                            *
 *        kface  - current free cell in the pF array                   *
 *                                                                     *
 ***********************************************************************/
{
  int k1, k2, k3, k4;
  if (fabs(dphi-M_PI) < perMillion) {          // half a circle
    for (int i=0; i<4; i++) {
      k1 = ii[i];
      k2 = (i == 3) ? ii[0] : ii[i+1];
      if (r[k1] == 0. && r[k2] == 0.) vv[i] = -1;      
    }
  }

  if (ii[1] == ii[2]) {
    k1 = kk[ii[0]];
    k2 = kk[ii[2]];
    k3 = kk[ii[3]];
    pF[kface++] = SbFacet(vv[0]*k1,0, vv[2]*k2,0, vv[3]*k3,0);
    if (r[ii[0]] != 0.) k1 += ns;
    if (r[ii[2]] != 0.) k2 += ns;
    if (r[ii[3]] != 0.) k3 += ns;
    pF[kface++] = SbFacet(vv[2]*k3,0, vv[0]*k2,0, vv[3]*k1,0);
  }else if (kk[ii[0]] == kk[ii[1]]) {
    k1 = kk[ii[0]];
    k2 = kk[ii[2]];
    k3 = kk[ii[3]];
    pF[kface++] = SbFacet(vv[1]*k1,0, vv[2]*k2,0, vv[3]*k3,0);
    if (r[ii[0]] != 0.) k1 += ns;
    if (r[ii[2]] != 0.) k2 += ns;
    if (r[ii[3]] != 0.) k3 += ns;
    pF[kface++] = SbFacet(vv[2]*k3,0, vv[1]*k2,0, vv[3]*k1,0);
  }else if (kk[ii[2]] == kk[ii[3]]) {
    k1 = kk[ii[0]];
    k2 = kk[ii[1]];
    k3 = kk[ii[2]];
    pF[kface++] = SbFacet(vv[0]*k1,0, vv[1]*k2,0, vv[3]*k3,0);
    if (r[ii[0]] != 0.) k1 += ns;
    if (r[ii[1]] != 0.) k2 += ns;
    if (r[ii[2]] != 0.) k3 += ns;
    pF[kface++] = SbFacet(vv[1]*k3,0, vv[0]*k2,0, vv[3]*k1,0);
  }else{
    k1 = kk[ii[0]];
    k2 = kk[ii[1]];
    k3 = kk[ii[2]];
    k4 = kk[ii[3]];
    pF[kface++] = SbFacet(vv[0]*k1,0, vv[1]*k2,0, vv[2]*k3,0, vv[3]*k4,0);
    if (r[ii[0]] != 0.) k1 += ns;
    if (r[ii[1]] != 0.) k2 += ns;
    if (r[ii[2]] != 0.) k3 += ns;
    if (r[ii[3]] != 0.) k4 += ns;
    pF[kface++] = SbFacet(vv[2]*k4,0, vv[1]*k3,0, vv[0]*k2,0, vv[3]*k1,0);
  }
}

void SbPolyhedron::RotateAroundZ(int nstep, double phi, double dphi,
                                 int np1, int np2,
                                 const double *z, double *r,
                                 int nodeVis, int edgeVis)
/***********************************************************************
 *                                                                     *
 * Name: SbPolyhedron::RotateAroundZ                Date:    27.11.96 *
 * Author: E.Chernyaev (IHEP/Protvino)               Revised:          *
 *                                                                     *
 * Function: Create SbPolyhedron for a solid produced by rotation of  *
 *           two polylines around Z-axis                               *
 *                                                                     *
 * Input: nstep - number of discrete steps, if 0 then default          *
 *        phi   - starting phi angle                                   *
 *        dphi  - delta phi                                            *
 *        np1   - number of points in external polyline                *
 *                (must be negative in case of closed polyline)        *
 *        np2   - number of points in internal polyline (may be 1)     *
 *        z[]   - z-coordinates (+z >>> -z for both polylines)         *
 *        r[]   - r-coordinates                                        *
 *        nodeVis - how to Draw edges joing consecutive positions of   *
 *                  node during rotation                               *
 *        edgeVis - how to Draw edges                                  *
 *                                                                     *
 ***********************************************************************/
{
  static const double wholeCircle   = 2*M_PI; //G.Barrand : const
    
  //   S E T   R O T A T I O N   P A R A M E T E R S

  bool ifWholeCircle = (fabs(dphi-wholeCircle) < perMillion) ?
    true : false;
  double   delPhi  = ifWholeCircle ? wholeCircle : dphi;  

  int n_step = (nstep > 0) ? nstep : GetNumberOfRotationSteps(); //G.Barrand
  int nSphi = int(delPhi*n_step/wholeCircle+.5);

  if (nSphi == 0) nSphi = 1;
  int        nVphi    = ifWholeCircle ? nSphi : nSphi+1;
  bool ifClosed = np1 > 0 ? false : true;
  
  //   C O U N T   V E R T E C E S

  int absNp1 = iabs(np1);
  int absNp2 = iabs(np2);
  int i1beg = 0;
  int i1end = absNp1-1;
  int i2beg = absNp1;
  int i2end = absNp1+absNp2-1; 
  int i, j, k;

  for(i=i1beg; i<=i2end; i++) {
    if (fabs(r[i]) < perMillion) r[i] = 0.;
  }

  j = 0;                                                // external nodes
  for (i=i1beg; i<=i1end; i++) {
    j += (r[i] == 0.) ? 1 : nVphi;
  }

  bool ifSide1 = false;                           // internal nodes
  bool ifSide2 = false;

  if (r[i2beg] != r[i1beg] || z[i2beg] != z[i1beg]) {
    j += (r[i2beg] == 0.) ? 1 : nVphi;
    ifSide1 = true;
  }

  for(i=i2beg+1; i<i2end; i++) {
    j += (r[i] == 0.) ? 1 : nVphi;
  }
  
  if (r[i2end] != r[i1end] || z[i2end] != z[i1end]) {
    if (absNp2 > 1) j += (r[i2end] == 0.) ? 1 : nVphi;
    ifSide2 = true;
  }

  //   C O U N T   F A C E S

  k = ifClosed ? absNp1*nSphi : (absNp1-1)*nSphi;       // external faces

  if (absNp2 > 1) {                                     // internal faces
    for(i=i2beg; i<i2end; i++) {
      if (r[i] > 0. || r[i+1] > 0.)       k += nSphi;
    }

    if (ifClosed) {
      if (r[i2end] > 0. || r[i2beg] > 0.) k += nSphi;
    }
  }

  if (!ifClosed) {                                      // side faces
    if (ifSide1 && (r[i1beg] > 0. || r[i2beg] > 0.)) k += nSphi;
    if (ifSide2 && (r[i1end] > 0. || r[i2end] > 0.)) k += nSphi;
  }

  if (!ifWholeCircle) {                                 // phi_side faces
    k += ifClosed ? 2*absNp1 : 2*(absNp1-1);
  }

  //   A L L O C A T E   M E M O R Y

  AllocateMemory(j, k);

  //   G E N E R A T E   V E R T E C E S

  int *kk;
  kk = new int[absNp1+absNp2];

  k = 1;
  for(i=i1beg; i<=i1end; i++) {
    kk[i] = k;
    if (r[i] == 0.) { pV[k++] = HVPoint3D(0, 0, z[i]); } else { k += nVphi; }
  }

  i = i2beg;
  if (ifSide1) {
    kk[i] = k;
    if (r[i] == 0.) { pV[k++] = HVPoint3D(0, 0, z[i]); } else { k += nVphi; }
  }else{
    kk[i] = kk[i1beg];
  }

  for(i=i2beg+1; i<i2end; i++) {
    kk[i] = k;
    if (r[i] == 0.) { pV[k++] = HVPoint3D(0, 0, z[i]); } else { k += nVphi; }
  }

  if (absNp2 > 1) {
    i = i2end;
    if (ifSide2) {
      kk[i] = k;
      if (r[i] == 0.) pV[k] = HVPoint3D(0, 0, z[i]);
    }else{
      kk[i] = kk[i1end];
    }
  }

  double cosPhi, sinPhi;

  for(j=0; j<nVphi; j++) {
    cosPhi = cos(phi+j*delPhi/nSphi);
    sinPhi = sin(phi+j*delPhi/nSphi);
    for(i=i1beg; i<=i2end; i++) {
      if (r[i] != 0.) pV[kk[i]+j] = HVPoint3D(r[i]*cosPhi,r[i]*sinPhi,z[i]);
    }
  }

  //   G E N E R A T E   E X T E R N A L   F A C E S

  int v1,v2;

  k = 1;
  v2 = ifClosed ? nodeVis : 1;
  for(i=i1beg; i<i1end; i++) {
    v1 = v2;
    if (!ifClosed && i == i1end-1) {
      v2 = 1;
    }else{
      v2 = (r[i] == r[i+1] && r[i+1] == r[i+2]) ? -1 : nodeVis;
    }
    RotateEdge(kk[i], kk[i+1], r[i], r[i+1], v1, v2,
               edgeVis, ifWholeCircle, nSphi, k);
  }
  if (ifClosed) {
    RotateEdge(kk[i1end], kk[i1beg], r[i1end],r[i1beg], nodeVis, nodeVis,
               edgeVis, ifWholeCircle, nSphi, k);
  }

  //   G E N E R A T E   I N T E R N A L   F A C E S

  if (absNp2 > 1) {
    v2 = ifClosed ? nodeVis : 1;
    for(i=i2beg; i<i2end; i++) {
      v1 = v2;
      if (!ifClosed && i==i2end-1) {
        v2 = 1;
      }else{
        v2 = (r[i] == r[i+1] && r[i+1] == r[i+2]) ? -1 :  nodeVis;
      }
      RotateEdge(kk[i+1], kk[i], r[i+1], r[i], v2, v1,
                 edgeVis, ifWholeCircle, nSphi, k);
    }
    if (ifClosed) {
      RotateEdge(kk[i2beg], kk[i2end], r[i2beg], r[i2end], nodeVis, nodeVis,
                 edgeVis, ifWholeCircle, nSphi, k);
    }
  }

  //   G E N E R A T E   S I D E   F A C E S

  if (!ifClosed) {
    if (ifSide1) {
      RotateEdge(kk[i2beg], kk[i1beg], r[i2beg], r[i1beg], 1, 1,
                 -1, ifWholeCircle, nSphi, k);
    }
    if (ifSide2) {
      RotateEdge(kk[i1end], kk[i2end], r[i1end], r[i2end], 1, 1,
                 -1, ifWholeCircle, nSphi, k);
    }
  }

  //   G E N E R A T E   S I D E   F A C E S  for the case of incomplete circle

  if (!ifWholeCircle) {

    int  ii[4], vv[4];

    if (ifClosed) {
      for (i=i1beg; i<=i1end; i++) {
        ii[0] = i;
        ii[3] = (i == i1end) ? i1beg : i+1;
        ii[1] = (absNp2 == 1) ? i2beg : ii[0]+absNp1;
        ii[2] = (absNp2 == 1) ? i2beg : ii[3]+absNp1;
        vv[0] = -1;
        vv[1] = 1;
        vv[2] = -1;
        vv[3] = 1;
        SetSideFacets(ii, vv, kk, r, dphi, nSphi, k);
      }
    }else{
      for (i=i1beg; i<i1end; i++) {
        ii[0] = i;
        ii[3] = i+1;
        ii[1] = (absNp2 == 1) ? i2beg : ii[0]+absNp1;
        ii[2] = (absNp2 == 1) ? i2beg : ii[3]+absNp1;
        vv[0] = (i == i1beg)   ? 1 : -1;
        vv[1] = 1;
        vv[2] = (i == i1end-1) ? 1 : -1;
        vv[3] = 1;
        SetSideFacets(ii, vv, kk, r, dphi, nSphi, k);
      }
    }      
  }

  delete [] kk;

  if (k-1 != nface) {
    std::cerr
      << "Polyhedron::RotateAroundZ: number of generated faces ("
      << k-1 << ") is not equal to the number of allocated faces ("
      << nface << ")"
      << std::endl;
  }
}

void SbPolyhedron::SetReferences()
/***********************************************************************
 *                                                                     *
 * Name: SbPolyhedron::SetReferences                Date:    04.12.96 *
 * Author: E.Chernyaev (IHEP/Protvino)               Revised:          *
 *                                                                     *
 * Function: For each edge set reference to neighbouring facet         *
 *                                                                     *
 ***********************************************************************/
{
  if (nface <= 0) return;

  struct edgeListMember {
    edgeListMember *next;
    int v2;
    int iface;
    int iedge;
  } *edgeList, *freeList, **headList;

  
  //   A L L O C A T E   A N D   I N I T I A T E   L I S T S

  edgeList = new edgeListMember[2*nface];
  headList = new edgeListMember*[nvert];
  
  int i;
  for (i=0; i<nvert; i++) {
    headList[i] = 0;
  }
  freeList = edgeList;
  for (i=0; i<2*nface-1; i++) {
    edgeList[i].next = &edgeList[i+1];
  }
  edgeList[2*nface-1].next = 0;

  //   L O O P   A L O N G   E D G E S

  int iface, iedge, nedge, i1, i2, k1, k2;
  edgeListMember *prev, *cur;
  
  for(iface=1; iface<=nface; iface++) {
    nedge = (pF[iface].edge[3].v == 0) ? 3 : 4;
    for (iedge=0; iedge<nedge; iedge++) {
      i1 = iedge;
      i2 = (iedge < nedge-1) ? iedge+1 : 0;
      i1 = iabs(pF[iface].edge[i1].v);
      i2 = iabs(pF[iface].edge[i2].v);
      k1 = (i1 < i2) ? i1 : i2;          // k1 = ::min(i1,i2);
      k2 = (i1 > i2) ? i1 : i2;          // k2 = ::max(i1,i2);
      
      // check head of the List corresponding to k1
      cur = headList[k1];
      if (cur == 0) {
        headList[k1] = freeList;
        freeList = freeList->next;
        cur = headList[k1];
        cur->next = 0;
        cur->v2 = k2;
        cur->iface = iface;
        cur->iedge = iedge;
        continue;
      }        

      if (cur->v2 == k2) {
        headList[k1] = cur->next;
        cur->next = freeList;
        freeList = cur;      
        pF[iface].edge[iedge].f = cur->iface;
        pF[cur->iface].edge[cur->iedge].f = iface;
        i1 = (pF[iface].edge[iedge].v < 0) ? -1 : 1;
        i2 = (pF[cur->iface].edge[cur->iedge].v < 0) ? -1 : 1;
        if (i1 != i2) {
          std::cerr
            << "Polyhedron::SetReferences: different edge visibility "
            << iface << "/" << iedge << "/"
            << pF[iface].edge[iedge].v << " and "
            << cur->iface << "/" << cur->iedge << "/"
            << pF[cur->iface].edge[cur->iedge].v
            << std::endl;
        }
        continue;
      }

      // check List itself
      for (;;) {
        prev = cur;
        cur = prev->next;
        if (cur == 0) {
          prev->next = freeList;
          freeList = freeList->next;
          cur = prev->next;
          cur->next = 0;
          cur->v2 = k2;
          cur->iface = iface;
          cur->iedge = iedge;
          break;
        }

        if (cur->v2 == k2) {
          prev->next = cur->next;
          cur->next = freeList;
          freeList = cur;      
          pF[iface].edge[iedge].f = cur->iface;
          pF[cur->iface].edge[cur->iedge].f = iface;
          i1 = (pF[iface].edge[iedge].v < 0) ? -1 : 1;
          i2 = (pF[cur->iface].edge[cur->iedge].v < 0) ? -1 : 1;
            if (i1 != i2) {
              std::cerr
                << "Polyhedron::SetReferences: different edge visibility "
                << iface << "/" << iedge << "/"
                << pF[iface].edge[iedge].v << " and "
                << cur->iface << "/" << cur->iedge << "/"
                << pF[cur->iface].edge[cur->iedge].v
                << std::endl;
            }
          break;
        }
      }
    }
  }

  //  C H E C K   T H A T   A L L   L I S T S   A R E   E M P T Y

  for (i=0; i<nvert; i++) {
    if (headList[i] != 0) {
      std::cerr
        << "Polyhedron::SetReferences: List " << i << " is not empty"
        << std::endl;
    }
  }

  //   F R E E   M E M O R Y

  delete [] edgeList;
  delete [] headList;
}

void SbPolyhedron::InvertFacets()
/***********************************************************************
 *                                                                     *
 * Name: SbPolyhedron::InvertFacets                Date:    01.12.99  *
 * Author: E.Chernyaev                              Revised:           *
 *                                                                     *
 * Function: Invert the order of the nodes in the facets               *
 *                                                                     *
 ***********************************************************************/
{
  if (nface <= 0) return;
  int i, k, nnode, v[4],f[4];
  for (i=1; i<=nface; i++) {
    nnode =  (pF[i].edge[3].v == 0) ? 3 : 4;
    for (k=0; k<nnode; k++) {
      v[k] = (k+1 == nnode) ? pF[i].edge[0].v : pF[i].edge[k+1].v;
      if (v[k] * pF[i].edge[k].v < 0) v[k] = -v[k];
      f[k] = pF[i].edge[k].f;
    }
    for (k=0; k<nnode; k++) {
      pF[i].edge[nnode-1-k].v = v[k];
      pF[i].edge[nnode-1-k].f = f[k];
    }
  }
}

/*
SbPolyhedron & SbPolyhedron::Transform(
 const SbRotation& rotation
,const SbVec3f& translation
)
{
  if (nvert > 0) {
    for (int i=1; i<=nvert; i++) { 
      const HVPoint3D& pv = pV[i];
      SbVec3f tmp;
      rotation.multVec(SbVec3f(pv[0],pv[1],pv[2]),tmp);
      pV[i] = HVPoint3D(tmp[0],tmp[1],tmp[2])
             +HVPoint3D(translation[0],translation[1],translation[2]);
    }

    //  C H E C K   D E T E R M I N A N T   A N D
    //  I N V E R T   F A C E T S   I F   I T   I S   N E G A T I V E

    //FIXME : have the below done in double.
    SbVec3f x;  rotation.multVec(SbVec3f(1,0,0),x);
    SbVec3f y;  rotation.multVec(SbVec3f(0,1,0),y);
    SbVec3f z;  rotation.multVec(SbVec3f(0,0,1),z);
    if ((x.cross(y)).dot(z) < 0) InvertFacets();
  }
  return *this;
}
*/

SbPolyhedron & SbPolyhedron::Transform(
 const HEPVis::SbRotation& rotation
,const SbVec3d& translation
)
{
  if (nvert > 0) {
    SbVec3d tmp;
    for (int i=1; i<=nvert; i++) { 
      rotation.multVec(pV[i],tmp);
      pV[i] = tmp+translation;
    }
    SbVec3d x;  rotation.multVec(SbVec3d(1,0,0),x);
    SbVec3d y;  rotation.multVec(SbVec3d(0,1,0),y);
    SbVec3d z;  rotation.multVec(SbVec3d(0,0,1),z);
    if ((x.cross(y)).dot(z) < 0) InvertFacets();
  }
  return *this;
}

/*G.Barrand : inline 
bool SbPolyhedron::GetNextVertexIndex(int &index, int &edgeFlag) const
// ***********************************************************************
// *                                                                     *
// * Name: SbPolyhedron::GetNextVertexIndex          Date:    03.09.96   *
// * Author: Yasuhide Sawada                          Revised:           *
// *                                                                     *
// * Function:                                                           *
// *                                                                     *
// ***********************************************************************
{
  static int iFace = 1;
  static int iQVertex = 0;
  int vIndex = pF[iFace].edge[iQVertex].v;

  edgeFlag = (vIndex > 0) ? 1 : 0;
  index = iabs(vIndex);

  if(index>nvert) {
    std::cerr << "SbPolyhedron::GetNextVertexIndex: pV index problem " 
              << index << " exceed " << nvert << std::endl;
    index = 0;
  }

  if (iQVertex >= 3 || pF[iFace].edge[iQVertex+1].v == 0) {
    iQVertex = 0;
    if (++iFace > nface) iFace = 1;
    return false;  // Last Edge
  }else{
    ++iQVertex;
    return true;  // not Last Edge
  }
}
*/

HVPoint3D SbPolyhedron::GetVertex(int index) const
/***********************************************************************
 *                                                                     *
 * Name: SbPolyhedron::GetVertex                   Date:    03.09.96  *
 * Author: Yasuhide Sawada                          Revised: 17.11.99  *
 *                                                                     *
 * Function: Get vertex of the index.                                  *
 *                                                                     *
 ***********************************************************************/
{
  if (index <= 0 || index > nvert) {
    std::cerr
      << "SbPolyhedron::GetVertex: irrelevant index " << index
      << std::endl;
    return HVPoint3D();
  }
  return pV[index];
}

const HVPoint3D& SbPolyhedron::GetVertexFast(int index) const { //G.Barrand
  return pV[index];
}

bool
SbPolyhedron::GetNextVertex(HVPoint3D &vertex, int &edgeFlag) const
/***********************************************************************
 *                                                                     *
 * Name: SbPolyhedron::GetNextVertex               Date:    22.07.96  *
 * Author: John Allison                             Revised:           *
 *                                                                     *
 * Function: Get vertices of the quadrilaterals in order for each      *
 *           face in face order.  Returns false when finished each     *
 *           face.                                                     *
 *                                                                     *
 ***********************************************************************/
{
  int index;
  bool rep = GetNextVertexIndex(index, edgeFlag);
  vertex = pV[index];
  return rep;
}

bool SbPolyhedron::GetNextVertex(HVPoint3D &vertex, int &edgeFlag,
                                       HVNormal3D &normal) const
/***********************************************************************
 *                                                                     *
 * Name: SbPolyhedron::GetNextVertex               Date:    26.11.99  *
 * Author: E.Chernyaev                              Revised:           *
 *                                                                     *
 * Function: Get vertices with normals of the quadrilaterals in order  *
 *           for each face in face order.                              *
 *           Returns false when finished each face.                    *
 *                                                                     *
 ***********************************************************************/
{
  static int iFace = 1;
  static int iNode = 0;

  if (nface == 0) return false;  // empty polyhedron

  int k = pF[iFace].edge[iNode].v;
  if (k > 0) { edgeFlag = 1; } else { edgeFlag = -1; k = -k; }
  vertex = pV[k];
  normal = FindNodeNormal(iFace,k);
  if (iNode >= 3 || pF[iFace].edge[iNode+1].v == 0) {
    iNode = 0;
    if (++iFace > nface) iFace = 1;
    return false;                // last node
  }else{
    ++iNode;
    return true;                 // not last node
  }
}

bool SbPolyhedron::GetNextEdgeIndeces(int &i1, int &i2, int &edgeFlag,
                                            int &iface1, int &iface2) const
/***********************************************************************
 *                                                                     *
 * Name: SbPolyhedron::GetNextEdgeIndeces          Date:    30.09.96  *
 * Author: E.Chernyaev                              Revised: 17.11.99  *
 *                                                                     *
 * Function: Get indeces of the next edge together with indeces of     *
 *           of the faces which share the edge.                        *
 *           Returns false when the last edge.                         *
 *                                                                     *
 ***********************************************************************/
{
  static int iFace    = 1;
  static int iQVertex = 0;
  static int iOrder   = 1;
  int  k1, k2, kflag, kface1, kface2;

  if (iFace == 1 && iQVertex == 0) {
    k2 = pF[nface].edge[0].v;
    k1 = pF[nface].edge[3].v;
    if (k1 == 0) k1 = pF[nface].edge[2].v;
    if (iabs(k1) > iabs(k2)) iOrder = -1;
  }

  do {
    k1     = pF[iFace].edge[iQVertex].v;
    kflag  = k1;
    k1     = iabs(k1);
    kface1 = iFace; 
    kface2 = pF[iFace].edge[iQVertex].f;
    if (iQVertex >= 3 || pF[iFace].edge[iQVertex+1].v == 0) {
      iQVertex = 0;
      k2 = iabs(pF[iFace].edge[iQVertex].v);
      iFace++;
    }else{
      iQVertex++;
      k2 = iabs(pF[iFace].edge[iQVertex].v);
    }
  } while (iOrder*k1 > iOrder*k2);

  i1 = k1; i2 = k2; edgeFlag = (kflag > 0) ? 1 : 0;
  iface1 = kface1; iface2 = kface2; 

  if (iFace > nface) {
    iFace  = 1; iOrder = 1;
    return false;
  }else{
    return true;
  }
}

bool
SbPolyhedron::GetNextEdgeIndeces(int &i1, int &i2, int &edgeFlag) const
/***********************************************************************
 *                                                                     *
 * Name: SbPolyhedron::GetNextEdgeIndeces          Date:    17.11.99  *
 * Author: E.Chernyaev                              Revised:           *
 *                                                                     *
 * Function: Get indeces of the next edge.                             *
 *           Returns false when the last edge.                         *
 *                                                                     *
 ***********************************************************************/
{
  int kface1, kface2;
  return GetNextEdgeIndeces(i1, i2, edgeFlag, kface1, kface2);
}

bool
SbPolyhedron::GetNextEdge(HVPoint3D &p1,
                           HVPoint3D &p2,
                           int &edgeFlag) const
/***********************************************************************
 *                                                                     *
 * Name: SbPolyhedron::GetNextEdge                 Date:    30.09.96  *
 * Author: E.Chernyaev                              Revised:           *
 *                                                                     *
 * Function: Get next edge.                                            *
 *           Returns false when the last edge.                         *
 *                                                                     *
 ***********************************************************************/
{
  int i1,i2;
  bool rep = GetNextEdgeIndeces(i1,i2,edgeFlag);
  p1 = pV[i1];
  p2 = pV[i2];
  return rep;
}

bool
SbPolyhedron::GetNextEdge(HVPoint3D &p1, HVPoint3D &p2,
                          int &edgeFlag, int &iface1, int &iface2) const
/***********************************************************************
 *                                                                     *
 * Name: SbPolyhedron::GetNextEdge                 Date:    17.11.99  *
 * Author: E.Chernyaev                              Revised:           *
 *                                                                     *
 * Function: Get next edge with indeces of the faces which share       *
 *           the edge.                                                 *
 *           Returns false when the last edge.                         *
 *                                                                     *
 ***********************************************************************/
{
  int i1,i2;
  bool rep = GetNextEdgeIndeces(i1,i2,edgeFlag,iface1,iface2);
  p1 = pV[i1];
  p2 = pV[i2];
  return rep;
}

void SbPolyhedron::GetFacet(int iFace, int &n, int *iNodes,
                            int *edgeFlags, int *iFaces) const
/***********************************************************************
 *                                                                     *
 * Name: SbPolyhedron::GetFacet                    Date:    15.12.99  *
 * Author: E.Chernyaev                              Revised:           *
 *                                                                     *
 * Function: Get face by index                                         *
 *                                                                     *
 ***********************************************************************/
{
  if (iFace < 1 || iFace > nface) {
    std::cerr 
      << "SbPolyhedron::GetFacet: irrelevant index " << iFace
      << std::endl;
    n = 0;
  }else{
    int i, k;
    for (i=0; i<4; i++) { 
      k = pF[iFace].edge[i].v;
      if (k == 0) break;
      if (iFaces != 0) iFaces[i] = pF[iFace].edge[i].f;
      if (k > 0) { 
        iNodes[i] = k;
        if (edgeFlags != 0) edgeFlags[i] = 1;
      }else{
        iNodes[i] = -k;
        if (edgeFlags != 0) edgeFlags[i] = -1;
      }
    }
    n = i;
  }
}

void SbPolyhedron::GetFacet(int index, int &n, HVPoint3D *nodes,
                            int *edgeFlags, HVNormal3D *normals) const
/***********************************************************************
 *                                                                     *
 * Name: SbPolyhedron::GetFacet                    Date:    17.11.99  *
 * Author: E.Chernyaev                              Revised:           *
 *                                                                     *
 * Function: Get face by index                                         *
 *                                                                     *
 ***********************************************************************/
{
  int iNodes[4];
  GetFacet(index, n, iNodes, edgeFlags);
  if (n != 0) {
    for (int i=0; i<4; i++) {
      nodes[i] = pV[iNodes[i]];
      if (normals != 0) normals[i] = FindNodeNormal(index,iNodes[i]);
    }
  }
}

bool
SbPolyhedron::GetNextFacet(int &n, HVPoint3D *nodes,
                           int *edgeFlags, HVNormal3D *normals) const
/***********************************************************************
 *                                                                     *
 * Name: SbPolyhedron::GetNextFacet                Date:    19.11.99  *
 * Author: E.Chernyaev                              Revised:           *
 *                                                                     *
 * Function: Get next face with normals of unit length at the nodes.   *
 *           Returns false when finished all faces.                    *
 *                                                                     *
 ***********************************************************************/
{
  static int iFace = 1;

  if (edgeFlags == 0) {
    GetFacet(iFace, n, nodes);
  }else if (normals == 0) {
    GetFacet(iFace, n, nodes, edgeFlags);
  }else{
    GetFacet(iFace, n, nodes, edgeFlags, normals);
  }

  if (++iFace > nface) {
    iFace  = 1;
    return false;
  }else{
    return true;
  }
}

//G.Barrand
#define CHECK_INDEX(a_method,a_index)\
  if(a_index>nvert) {\
    std::cerr << "SbPolyhedron::" << a_method << " :"\
              << " pV " << #a_index " index problem "\
              << a_index << " exceed " << nvert << std::endl;\
    return HVNormal3D();\
  }

HVNormal3D SbPolyhedron::GetNormal(int iFace) const
/***********************************************************************
 *                                                                     *
 * Name: SbPolyhedron::GetNormal                    Date:    19.11.99 *
 * Author: E.Chernyaev                               Revised:          *
 *                                                                     *
 * Function: Get normal of the face given by index                     *
 *                                                                     *
 ***********************************************************************/
{
  if (iFace < 1 || iFace > nface) {
    std::cerr 
      << "SbPolyhedron::GetNormal: irrelevant index " << iFace 
      << std::endl;
    return HVNormal3D();
  }

  int i0  = iabs(pF[iFace].edge[0].v);
  int i1  = iabs(pF[iFace].edge[1].v);
  int i2  = iabs(pF[iFace].edge[2].v);
  int i3  = iabs(pF[iFace].edge[3].v);
  if (i3 == 0) i3 = i0;

  CHECK_INDEX("GetNormal",i0)
  CHECK_INDEX("GetNormal",i1)
  CHECK_INDEX("GetNormal",i2)
  CHECK_INDEX("GetNormal",i3)

  return (pV[i2] - pV[i0]).cross(pV[i3] - pV[i1]);
}

HVNormal3D SbPolyhedron::GetUnitNormal(int iFace) const
/***********************************************************************
 *                                                                     *
 * Name: SbPolyhedron::GetNormal                    Date:    19.11.99 *
 * Author: E.Chernyaev                               Revised:          *
 *                                                                     *
 * Function: Get unit normal of the face given by index                *
 *                                                                     *
 ***********************************************************************/
{
  if (iFace < 1 || iFace > nface) {
    std::cerr 
      << "SbPolyhedron::GetUnitNormal: irrelevant index " << iFace
      << std::endl;
    return HVNormal3D();
  }

  int i0  = iabs(pF[iFace].edge[0].v);
  int i1  = iabs(pF[iFace].edge[1].v);
  int i2  = iabs(pF[iFace].edge[2].v);
  int i3  = iabs(pF[iFace].edge[3].v);
  if (i3 == 0) i3 = i0;

  CHECK_INDEX("GetUnitNormal",i0)
  CHECK_INDEX("GetUnitNormal",i1)
  CHECK_INDEX("GetUnitNormal",i2)
  CHECK_INDEX("GetUnitNormal",i3)

  HVNormal3D nm = (pV[i2] - pV[i0]).cross(pV[i3] - pV[i1]);
  nm.normalize();
  return nm;
}

bool SbPolyhedron::GetNextNormal(HVNormal3D &normal) const
/***********************************************************************
 *                                                                     *
 * Name: SbPolyhedron::GetNextNormal               Date:    22.07.96  *
 * Author: John Allison                             Revised: 19.11.99  *
 *                                                                     *
 * Function: Get normals of each face in face order.  Returns false    *
 *           when finished all faces.                                  *
 *                                                                     *
 ***********************************************************************/
{
  static int iFace = 1;
  normal = GetNormal(iFace);
  if (++iFace > nface) {
    iFace = 1;
    return false;
  }else{
    return true;
  }
}

bool SbPolyhedron::GetNextUnitNormal(HVNormal3D &normal) const
/***********************************************************************
 *                                                                     *
 * Name: SbPolyhedron::GetNextUnitNormal           Date:    16.09.96  *
 * Author: E.Chernyaev                              Revised:           *
 *                                                                     *
 * Function: Get normals of unit length of each face in face order.    *
 *           Returns false when finished all faces.                    *
 *                                                                     *
 ***********************************************************************/
{
  bool rep = GetNextNormal(normal);
  normal.normalize();
  return rep;
}

double SbPolyhedron::GetSurfaceArea() const
/***********************************************************************
 *                                                                     *
 * Name: SbPolyhedron::GetSurfaceArea              Date:    25.05.01  *
 * Author: E.Chernyaev                              Revised:           *
 *                                                                     *
 * Function: Returns area of the surface of the polyhedron.            *
 *                                                                     *
 ***********************************************************************/
{
  double s = 0.;
  for (int iFace=1; iFace<=nface; iFace++) {
    int i0 = iabs(pF[iFace].edge[0].v);
    int i1 = iabs(pF[iFace].edge[1].v);
    int i2 = iabs(pF[iFace].edge[2].v);
    int i3 = iabs(pF[iFace].edge[3].v);
    if (i3 == 0) i3 = i0;
    s += ((pV[i2] - pV[i0]).cross(pV[i3] - pV[i1])).length();
  }
  return s/2.;
}

double SbPolyhedron::GetVolume() const
/***********************************************************************
 *                                                                     *
 * Name: SbPolyhedron::GetVolume                   Date:    25.05.01  *
 * Author: E.Chernyaev                              Revised:           *
 *                                                                     *
 * Function: Returns volume of the polyhedron.                         *
 *                                                                     *
 ***********************************************************************/
{
  double v = 0.;
  for (int iFace=1; iFace<=nface; iFace++) {
    int i0 = iabs(pF[iFace].edge[0].v);
    int i1 = iabs(pF[iFace].edge[1].v);
    int i2 = iabs(pF[iFace].edge[2].v);
    int i3 = iabs(pF[iFace].edge[3].v);
    HVPoint3D g;
    if (i3 == 0) {
      i3 = i0;
      g  = (pV[i0]+pV[i1]+pV[i2]) * (1.0/3.0);
    }else{
      g  = (pV[i0]+pV[i1]+pV[i2]+pV[i3]) * 0.25;
    }
    v += ((pV[i2] - pV[i0]).cross(pV[i3] - pV[i1])).dot(g);
  }
  return v/6.;
}

SbPolyhedronTrd2::SbPolyhedronTrd2(double Dx1, double Dx2,
                                     double Dy1, double Dy2,
                                     double Dz)
/***********************************************************************
 *                                                                     *
 * Name: SbPolyhedronTrd2                           Date:    22.07.96 *
 * Author: E.Chernyaev (IHEP/Protvino)               Revised:          *
 *                                                                     *
 * Function: Create GEANT4 TRD2-trapezoid                              *
 *                                                                     *
 * Input: Dx1 - half-length along X at -Dz           8----7            *
 *        Dx2 - half-length along X ay +Dz        5----6  !            *
 *        Dy1 - half-length along Y ay -Dz        !  4-!--3            *
 *        Dy2 - half-length along Y ay +Dz        1----2               *
 *        Dz  - half-length along Z                                    *
 *                                                                     *
 ***********************************************************************/
{
  AllocateMemory(8,6);

  pV[1] = HVPoint3D(-Dx1,-Dy1,-Dz);
  pV[2] = HVPoint3D( Dx1,-Dy1,-Dz);
  pV[3] = HVPoint3D( Dx1, Dy1,-Dz);
  pV[4] = HVPoint3D(-Dx1, Dy1,-Dz);
  pV[5] = HVPoint3D(-Dx2,-Dy2, Dz);
  pV[6] = HVPoint3D( Dx2,-Dy2, Dz);
  pV[7] = HVPoint3D( Dx2, Dy2, Dz);
  pV[8] = HVPoint3D(-Dx2, Dy2, Dz);

  CreatePrism();
}

SbPolyhedronTrd2::~SbPolyhedronTrd2() {}

SbPolyhedronTrd1::SbPolyhedronTrd1(double Dx1, double Dx2,
                                     double Dy, double Dz)
  : SbPolyhedronTrd2(Dx1, Dx2, Dy, Dy, Dz) {}

SbPolyhedronTrd1::~SbPolyhedronTrd1() {}

SbPolyhedronBox::SbPolyhedronBox(double Dx, double Dy, double Dz)
  : SbPolyhedronTrd2(Dx, Dx, Dy, Dy, Dz) {}

SbPolyhedronBox::~SbPolyhedronBox() {}

SbPolyhedronTrap::SbPolyhedronTrap(double Dz,
                                     double Theta,
                                     double Phi,
                                     double Dy1,
                                     double Dx1,
                                     double Dx2,
                                     double Alp1,
                                     double Dy2,
                                     double Dx3,
                                     double Dx4,
                                     double Alp2)
/***********************************************************************
 *                                                                     *
 * Name: SbPolyhedronTrap                           Date:    20.11.96 *
 * Author: E.Chernyaev                               Revised:          *
 *                                                                     *
 * Function: Create GEANT4 TRAP-trapezoid                              *
 *                                                                     *
 * Input: DZ   - half-length in Z                                      *
 *        Theta,Phi - polar angles of the line joining centres of the  *
 *                    faces at Z=-Dz and Z=+Dz                         *
 *        Dy1  - half-length in Y of the face at Z=-Dz                 *
 *        Dx1  - half-length in X of low edge of the face at Z=-Dz     *
 *        Dx2  - half-length in X of top edge of the face at Z=-Dz     *
 *        Alp1 - angle between Y-axis and the median joining top and   *
 *               low edges of the face at Z=-Dz                        *
 *        Dy2  - half-length in Y of the face at Z=+Dz                 *
 *        Dx3  - half-length in X of low edge of the face at Z=+Dz     *
 *        Dx4  - half-length in X of top edge of the face at Z=+Dz     *
 *        Alp2 - angle between Y-axis and the median joining top and   *
 *               low edges of the face at Z=+Dz                        *
 *                                                                     *
 ***********************************************************************/
{
  double DzTthetaCphi = Dz*tan(Theta)*cos(Phi);
  double DzTthetaSphi = Dz*tan(Theta)*sin(Phi);
  double Dy1Talp1 = Dy1*tan(Alp1);
  double Dy2Talp2 = Dy2*tan(Alp2);
  
  AllocateMemory(8,6);

  pV[1] = HVPoint3D(-DzTthetaCphi-Dy1Talp1-Dx1,-DzTthetaSphi-Dy1,-Dz);
  pV[2] = HVPoint3D(-DzTthetaCphi-Dy1Talp1+Dx1,-DzTthetaSphi-Dy1,-Dz);
  pV[3] = HVPoint3D(-DzTthetaCphi+Dy1Talp1+Dx2,-DzTthetaSphi+Dy1,-Dz);
  pV[4] = HVPoint3D(-DzTthetaCphi+Dy1Talp1-Dx2,-DzTthetaSphi+Dy1,-Dz);
  pV[5] = HVPoint3D( DzTthetaCphi-Dy2Talp2-Dx3, DzTthetaSphi-Dy2, Dz);
  pV[6] = HVPoint3D( DzTthetaCphi-Dy2Talp2+Dx3, DzTthetaSphi-Dy2, Dz);
  pV[7] = HVPoint3D( DzTthetaCphi+Dy2Talp2+Dx4, DzTthetaSphi+Dy2, Dz);
  pV[8] = HVPoint3D( DzTthetaCphi+Dy2Talp2-Dx4, DzTthetaSphi+Dy2, Dz);

  CreatePrism();
}

SbPolyhedronTrap::~SbPolyhedronTrap() {}

SbPolyhedronPara::SbPolyhedronPara(double Dx, double Dy, double Dz,
                                     double Alpha, double Theta,
                                     double Phi)
  : SbPolyhedronTrap(Dz, Theta, Phi, Dy, Dx, Dx, Alpha, Dy, Dx, Dx, Alpha) {}

SbPolyhedronPara::~SbPolyhedronPara() {}

SbPolyhedronCons::SbPolyhedronCons(double Rmn1,
                                     double Rmx1,
                                     double Rmn2,
                                     double Rmx2, 
                                     double Dz,
                                     double Phi1,
                                     double Dphi,
                                     int nstep) //G.Barrand
/***********************************************************************
 *                                                                     *
 * Name: SbPolyhedronCons::SbPolyhedronCons        Date:    15.12.96 *
 * Author: E.Chernyaev (IHEP/Protvino)               Revised: 15.12.96 *
 *                                                                     *
 * Function: Constructor for CONS, TUBS, CONE, TUBE                    *
 *                                                                     *
 * Input: Rmn1, Rmx1 - inside and outside radiuses at -Dz              *
 *        Rmn2, Rmx2 - inside and outside radiuses at +Dz              *
 *        Dz         - half length in Z                                *
 *        Phi1       - starting angle of the segment                   *
 *        Dphi       - segment range                                   *
 *                                                                     *
 ***********************************************************************/
{
  static const double wholeCircle = 2*M_PI; //G.Barrand : const

  //   C H E C K   I N P U T   P A R A M E T E R S

  int k = 0;
  if (Rmn1 < 0. || Rmx1 < 0. || Rmn2 < 0. || Rmx2 < 0.)        k = 1;
  if (Rmn1 > Rmx1 || Rmn2 > Rmx2)                              k = 1;
  if (Rmn1 == Rmx1 && Rmn2 == Rmx2)                            k = 1;

  if (Dz <= 0.) k += 2;
 
  double phi1, phi2, dphi;
  if (Dphi < 0.) {
    phi2 = Phi1; phi1 = phi2 - Dphi;
  }else if (Dphi == 0.) {
    phi1 = Phi1; phi2 = phi1 + wholeCircle;
  }else{
    phi1 = Phi1; phi2 = phi1 + Dphi;
  }
  dphi  = phi2 - phi1;
  if (fabs(dphi-wholeCircle) < perMillion) dphi = wholeCircle;
  if (dphi > wholeCircle) k += 4; 

  if (k != 0) {
    std::cerr << "SbPolyhedronCone(s)/Tube(s): error in input parameters";
    if ((k & 1) != 0) std::cerr << " (radiuses)";
    if ((k & 2) != 0) std::cerr << " (half-length)";
    if ((k & 4) != 0) std::cerr << " (angles)";
    std::cerr << std::endl;
    std::cerr << " Rmn1=" << Rmn1 << " Rmx1=" << Rmx1;
    std::cerr << " Rmn2=" << Rmn2 << " Rmx2=" << Rmx2;
    std::cerr << " Dz=" << Dz << " Phi1=" << Phi1 << " Dphi=" << Dphi
              << std::endl;
    return;
  }
  
  //   P R E P A R E   T W O   P O L Y L I N E S

  double zz[4], rr[4];
  zz[0] =  Dz; 
  zz[1] = -Dz; 
  zz[2] =  Dz; 
  zz[3] = -Dz; 
  rr[0] =  Rmx2;
  rr[1] =  Rmx1;
  rr[2] =  Rmn2;
  rr[3] =  Rmn1;

  //   R O T A T E    P O L Y L I N E S

  //G.Barrand : nstep
  RotateAroundZ(nstep, phi1, dphi, 2, 2, zz, rr, -1, -1); 
  SetReferences();
}

SbPolyhedronCons::~SbPolyhedronCons() {}

SbPolyhedronCone::SbPolyhedronCone(double Rmn1, double Rmx1, 
                                   double Rmn2, double Rmx2,
                                   double Dz,
                                   int nstep) //G.Barrand
:  SbPolyhedronCons(Rmn1, Rmx1, Rmn2, Rmx2, Dz, 0*deg, 360*deg, nstep) {}

SbPolyhedronCone::~SbPolyhedronCone() {}

SbPolyhedronTubs::SbPolyhedronTubs(double Rmin, double Rmax,
                                     double Dz, 
                                     double Phi1, double Dphi,
                                     int nstep) //G.Barrand
:   SbPolyhedronCons(Rmin, Rmax, Rmin, Rmax, Dz, Phi1, Dphi, nstep) {}

SbPolyhedronTubs::~SbPolyhedronTubs() {}

SbPolyhedronTube::SbPolyhedronTube (double Rmin, double Rmax,
                                    double Dz,
                                    int nstep) //G.Barrand
: SbPolyhedronCons(Rmin, Rmax, Rmin, Rmax, Dz, 0*deg, 360*deg, nstep) {}

SbPolyhedronTube::~SbPolyhedronTube () {}

SbPolyhedronPgon::SbPolyhedronPgon(double phi,
                                     double dphi,
                                     int    npdv,
                                     int    nz,
                                     const double *z,
                                     const double *rmin,
                                     const double *rmax)
/***********************************************************************
 *                                                                     *
 * Name: SbPolyhedronPgon                           Date:    09.12.96 *
 * Author: E.Chernyaev                               Revised:          *
 *                                                                     *
 * Function: Constructor of polyhedron for PGON, PCON                  *
 *                                                                     *
 * Input: phi  - initial phi                                           *
 *        dphi - delta phi                                             *
 *        npdv - number of steps along phi                             *
 *        nz   - number of z-planes (at least two)                     *
 *        z[]  - z coordinates of the slices                           *
 *        rmin[] - smaller r at the slices                             *
 *        rmax[] - bigger  r at the slices                             *
 *                                                                     *
 ***********************************************************************/
{
  //   C H E C K   I N P U T   P A R A M E T E R S

  if (dphi <= 0. || dphi > 2*M_PI) {
    std::cerr
      << "SbPolyhedronPgon/Pcon: wrong delta phi = " << dphi
      << std::endl;
    return;
  }    
    
  if (nz < 2) {
    std::cerr
      << "SbPolyhedronPgon/Pcon: number of z-planes less than two = " << nz
      << std::endl;
    return;
  }

  if (npdv < 0) {
    std::cerr
      << "SbPolyhedronPgon/Pcon: error in number of phi-steps =" << npdv
      << std::endl;
    return;
  }

  int i;
  for (i=0; i<nz; i++) {
    if (rmin[i] < 0. || rmax[i] < 0. || rmin[i] > rmax[i]) {
      std::cerr
        << "SbPolyhedronPgon: error in radiuses rmin[" << i << "]="
        << rmin[i] << " rmax[" << i << "]=" << rmax[i]
        << std::endl;
      return;
    }
  }

  //   P R E P A R E   T W O   P O L Y L I N E S

  double *zz, *rr;
  zz = new double[2*nz];
  rr = new double[2*nz];

  if (z[0] > z[nz-1]) {
    for (i=0; i<nz; i++) {
      zz[i]    = z[i];
      rr[i]    = rmax[i];
      zz[i+nz] = z[i];
      rr[i+nz] = rmin[i];
    }
  }else{
    for (i=0; i<nz; i++) {
      zz[i]    = z[nz-i-1];
      rr[i]    = rmax[nz-i-1];
      zz[i+nz] = z[nz-i-1];
      rr[i+nz] = rmin[nz-i-1];
    }
  }

  //   R O T A T E    P O L Y L I N E S

  RotateAroundZ(npdv, phi, dphi, nz, nz, zz, rr, -1, (npdv == 0) ? -1 : 1); 
  SetReferences();
  
  delete [] zz;
  delete [] rr;
}

SbPolyhedronPgon::~SbPolyhedronPgon() {}

SbPolyhedronPcon::SbPolyhedronPcon(double phi, double dphi, int nz,
                                     const double *z,
                                     const double *rmin,
                                     const double *rmax)
  : SbPolyhedronPgon(phi, dphi, 0, nz, z, rmin, rmax) {}

SbPolyhedronPcon::~SbPolyhedronPcon() {}

SbPolyhedronSphere::SbPolyhedronSphere(double rmin, double rmax,
                                       double phi, double dphi,
                                       double the, double dthe,
                                       int nphi, //G.Barrand
                                       int nthe) //G.Barrand
/***********************************************************************
 *                                                                     *
 * Name: SbPolyhedronSphere                         Date:    11.12.96 *
 * Author: E.Chernyaev (IHEP/Protvino)               Revised:          *
 *                                                                     *
 * Function: Constructor of polyhedron for SPHERE                      *
 *                                                                     *
 * Input: rmin - internal radius                                       *
 *        rmax - external radius                                       *
 *        phi  - initial phi                                           *
 *        dphi - delta phi                                             *
 *        the  - initial theta                                         *
 *        dthe - delta theta                                           *
 *                                                                     *
 ***********************************************************************/
{
  //   C H E C K   I N P U T   P A R A M E T E R S

  if (dphi <= 0. || dphi > 2*M_PI) {
    std::cerr
      << "SbPolyhedronSphere: wrong delta phi = " << dphi
      << std::endl;
    return;
  }    

  if (the < 0. || the > M_PI) {
    std::cerr
      << "SbPolyhedronSphere: wrong theta = " << the
      << std::endl;
    return;
  }    
  
  if (dthe <= 0. || dthe > M_PI) {
    std::cerr
      << "SbPolyhedronSphere: wrong delta theta = " << dthe
      << std::endl;
    return;
  }    

  if ( (the+dthe >= M_PI) && (the+dthe < M_PI + 2*DBL_EPSILON) ) 
    dthe = M_PI - the; //G.Barrand : coming from LHCb/S.Ponce.

  if (the+dthe > M_PI) {
    std::cerr
      << "SbPolyhedronSphere: wrong theta + delta theta = "
      << the << " " << dthe
      << std::endl;
    return;
  }    
  
  if (rmin < 0. || rmin >= rmax) {
    std::cerr
      << "SbPolyhedronSphere: error in radiuses"
      << " rmin=" << rmin << " rmax=" << rmax
      << std::endl;
    return;
  }

  //   P R E P A R E   T W O   P O L Y L I N E S

  int n_the = (nthe>0) ? nthe : GetNumberOfRotationSteps(); //G.Barrand.
  int ns = (n_the + 1) / 2;

  int np1 = int(dthe*ns/M_PI+.5) + 1;
  if (np1 <= 1) np1 = 2;
  int np2 = rmin < perMillion ? 1 : np1;

  double *zz, *rr;
  zz = new double[np1+np2];
  rr = new double[np1+np2];

  double a = dthe/(np1-1);
  double cosa, sina;
  for (int i=0; i<np1; i++) {
    cosa  = cos(the+i*a);
    sina  = sin(the+i*a);
    zz[i] = rmax*cosa;
    rr[i] = rmax*sina;
    if (np2 > 1) {
      zz[i+np1] = rmin*cosa;
      rr[i+np1] = rmin*sina;
    }
  }
  if (np2 == 1) {
    zz[np1] = 0.;
    rr[np1] = 0.;
  }

  //   R O T A T E    P O L Y L I N E S

  //G.Barrand : nphi.
  RotateAroundZ(nphi, phi, dphi, np1, np2, zz, rr, -1, -1); 
  SetReferences();
  
  delete [] zz;
  delete [] rr;
}

SbPolyhedronSphere::~SbPolyhedronSphere() {}

SbPolyhedronTorus::SbPolyhedronTorus(double rmin,
                                       double rmax,
                                       double rtor,
                                       double phi,
                                       double dphi,
                                       int nphi, //G.Barrand
                                       int nthe) //G.Barrand
/***********************************************************************
 *                                                                     *
 * Name: SbPolyhedronTorus                          Date:    11.12.96 *
 * Author: E.Chernyaev (IHEP/Protvino)               Revised:          *
 *                                                                     *
 * Function: Constructor of polyhedron for TORUS                       *
 *                                                                     *
 * Input: rmin - internal radius                                       *
 *        rmax - external radius                                       *
 *        rtor - radius of torus                                       *
 *        phi  - initial phi                                           *
 *        dphi - delta phi                                             *
 *                                                                     *
 ***********************************************************************/
{
  //   C H E C K   I N P U T   P A R A M E T E R S

  if (dphi <= 0. || dphi > 2*M_PI) {
    std::cerr
      << "SbPolyhedronTorus: wrong delta phi = " << dphi
      << std::endl;
    return;
  }

  if (rmin < 0. || rmin >= rmax || rmax >= rtor) {
    std::cerr
      << "SbPolyhedronTorus: error in radiuses"
      << " rmin=" << rmin << " rmax=" << rmax << " rtorus=" << rtor
      << std::endl;
    return;
  }

  //   P R E P A R E   T W O   P O L Y L I N E S

  int n_the = (nthe>0) ? nthe : GetNumberOfRotationSteps(); //G.Barrand.
  int np1 = n_the;

  int np2 = rmin < perMillion ? 1 : np1;

  double *zz, *rr;
  zz = new double[np1+np2];
  rr = new double[np1+np2];

  double a = 2*M_PI/np1;
  double cosa, sina;
  for (int i=0; i<np1; i++) {
    cosa  = cos(i*a);
    sina  = sin(i*a);
    zz[i] = rmax*cosa;
    rr[i] = rtor+rmax*sina;
    if (np2 > 1) {
      zz[i+np1] = rmin*cosa;
      rr[i+np1] = rtor+rmin*sina;
    }
  }
  if (np2 == 1) {
    zz[np1] = 0.;
    rr[np1] = rtor;
    np2 = -1;
  }

  //   R O T A T E    P O L Y L I N E S

  //G.Barrand : nphi.
  RotateAroundZ(nphi, phi, dphi, -np1, -np2, zz, rr, -1,-1); 
  SetReferences();
  
  delete [] zz;
  delete [] rr;
}

SbPolyhedronTorus::~SbPolyhedronTorus() {}

int SbPolyhedron::fNumberOfRotationSteps = DEFAULT_NUMBER_OF_STEPS;
// G.Barrand : begin.
int SbPolyhedron::GetNumberOfRotationSteps() { 
  return fNumberOfRotationSteps; 
}
void SbPolyhedron::ResetNumberOfRotationSteps() {
  fNumberOfRotationSteps = DEFAULT_NUMBER_OF_STEPS;
}
// G.Barrand : end.
/***********************************************************************
 *                                                                     *
 * Name: SbPolyhedron::fNumberOfRotationSteps       Date:    24.06.97 *
 * Author: J.Allison (Manchester University)         Revised:          *
 *                                                                     *
 * Function: Number of steps for whole circle                          *
 *                                                                     *
 ***********************************************************************/

#include "BooleanProcessor.icc"
//G.Barrand : static HEPVis_BooleanProcessor processor;

SbPolyhedron SbPolyhedron::add(const SbPolyhedron & p) const 
/***********************************************************************
 *                                                                     *
 * Name: SbPolyhedron::add                          Date:    19.03.00 *
 * Author: E.Chernyaev                               Revised:          *
 *                                                                     *
 * Function: Boolean "union" of two polyhedra                          *
 *                                                                     *
 ***********************************************************************/
{
  HEPVis_BooleanProcessor processor; //G.Barrand
  int err;
  return processor.execute(OP_UNION, *this, p, err);
}

SbPolyhedron SbPolyhedron::intersect(const SbPolyhedron & p) const 
/***********************************************************************
 *                                                                     *
 * Name: SbPolyhedron::intersect                    Date:    19.03.00 *
 * Author: E.Chernyaev                               Revised:          *
 *                                                                     *
 * Function: Boolean "intersection" of two polyhedra                   *
 *                                                                     *
 ***********************************************************************/
{
  HEPVis_BooleanProcessor processor; //G.Barrand
  int err;
  return processor.execute(OP_INTERSECTION, *this, p, err);
}

SbPolyhedron SbPolyhedron::subtract(const SbPolyhedron & p) const 
/***********************************************************************
 *                                                                     *
 * Name: SbPolyhedron::add                          Date:    19.03.00 *
 * Author: E.Chernyaev                               Revised:          *
 *                                                                     *
 * Function: Boolean "subtraction" of "p" from "this"                  *
 *                                                                     *
 ***********************************************************************/
{
  HEPVis_BooleanProcessor processor; //G.Barrand
  int err;
  return processor.execute(OP_SUBTRACTION, *this, p, err);
}


//G.Barrand : begin
SbPolyhedronProcessor::SbPolyhedronProcessor(){}
SbPolyhedronProcessor::~SbPolyhedronProcessor(){}
//private for the moment.
SbPolyhedronProcessor::SbPolyhedronProcessor(const SbPolyhedronProcessor&){}
SbPolyhedronProcessor& SbPolyhedronProcessor::operator=(const SbPolyhedronProcessor&){return *this;}

//public
void SbPolyhedronProcessor::push_back(Operation a_op,const SbPolyhedron& a_polyhedron) {
  m_ops.push_back(op_t(a_op,a_polyhedron));
}
void SbPolyhedronProcessor::clear() { m_ops.clear();}

SbBool SbPolyhedronProcessor::is_same_op() const {
  if(!m_ops.size()) return TRUE;
  Operation op = m_ops[0].first;
  std::vector<op_t>::const_iterator it;
  for(it=m_ops.begin();it!=m_ops.end();++it) {
    if((*it).first!=op) return FALSE;
  }
  return TRUE;
}

#include <list>

static bool is_in(unsigned int a_index,
                  const std::list<unsigned int>& a_is) {
  std::list<unsigned int>::const_iterator it;
  for(it=a_is.begin();it!=a_is.end();++it) {
    if(*it==a_index) return true;
  }
  return false;
}
static void dump(const std::vector<unsigned int>& a_is) {
  unsigned int number = a_is.size();
  for(unsigned int index=0;index<number;index++) {
    printf("%d ",a_is[index]);
  }
  printf("\n");
}

namespace HEPVis {
class bijection_visitor {
public:
  typedef std::vector<unsigned int> is_t;
  virtual bool visit(const is_t&) = 0;
public:
  bijection_visitor(unsigned int a_number):m_number(a_number){}
  bool visitx() {
    m_is.resize(m_number,0);
    std::list<unsigned int> is;
    return visit(0,is);
  }
private:
  bool visit(unsigned int a_level,std::list<unsigned int>& a_is) {
    for(unsigned int index=0;index<m_number;index++) {
      if(is_in(index,a_is)) {
      } else {
        a_is.push_back(index);
        m_is[a_level] = index; 
        if(a_level==m_number-1) {
          if(!visit(m_is)) return false;
        } else {
          if(!visit(a_level+1,a_is)) return false;
        }
        a_is.pop_back();
      }
    }
    return true;
  }
private:
  unsigned int m_number;
  is_t m_is;
};

class bijection_dump : public HEPVis::bijection_visitor {
public:
  bijection_dump(unsigned int a_number)
  : HEPVis::bijection_visitor(a_number)
  {}
  virtual bool visit(const is_t& a_is) {
    dump(a_is);
    return true;//continue
  }
};

} //namespace HEPVis

class SbPolyhedron_exec : public HEPVis::bijection_visitor {
public:
  SbPolyhedron_exec(unsigned int a_number,
       SbPolyhedronProcessor& a_proc,
       SbPolyhedron& a_poly)
  : HEPVis::bijection_visitor(a_number)
  ,m_proc(a_proc)
  ,m_poly(a_poly)
  {}
  virtual bool visit(const is_t& a_is) {
    if(m_proc.execute1(m_poly,a_is)==TRUE) return false; //stop
    return true;//continue
  }
private:
  SbPolyhedronProcessor& m_proc;
  SbPolyhedron& m_poly;
};

SbBool SbPolyhedronProcessor::execute(SbPolyhedron& a_poly) {
  //{for(unsigned int index=0;index<5;index++) {
  //  printf("debug : bijection : %d\n",index);  
  //  HEPVis::bijection_dump bd(index);
  //  bd.visitx();
  //}}

  SbPolyhedron_exec e(m_ops.size(),*this,a_poly);
  if(!e.visitx()) return TRUE;
  //std::cerr << "SbPolyhedronProcessor::execute :"
  //          << " all shifts and combinatory tried."
  //          << " Boolean operations failed."
  //          << std::endl;
  return FALSE;
}
SbBool SbPolyhedronProcessor::execute1(
 SbPolyhedron& a_poly
,const std::vector<unsigned int>& a_is
) {
  SbPolyhedron result(a_poly);
  unsigned int number = m_ops.size();
  int num_shift = HEPVis_BooleanProcessor::get_num_shift();
  for(int ishift=0;ishift<num_shift;ishift++) {
    HEPVis_BooleanProcessor::set_shift(ishift);

    result = a_poly;
    bool done = true;
    for(unsigned int index=0;index<number;index++) {
      HEPVis_BooleanProcessor processor; //take a fresh one.
      const op_t& elem = m_ops[a_is[index]];
      int err;
      result = processor.execute(elem.first,result,elem.second,err);
      if(err) {
        done = false;
        break;
      }
    }
    if(done) {
      a_poly = result;
      return TRUE;      
    }
  }

  //std::cerr << "SbPolyhedronProcessor::execute :"
  //          << " all shifts tried. Boolean operations failed."
  //          << std::endl;

  //a_poly = result;
  return FALSE;
}
//G.Barrand : end
