#ifndef HEPVis_SbPolyhedron_h
#define HEPVis_SbPolyhedron_h
//--------------------------------------------------------------------//
// JFB:                                                               //
// SbPolyhedron was HepPolyhedron, retrofitted to Open Inventor       //
// infrastructure:                                                    //
//--------------------------------------------------------------------//


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
// Class Description:
// SbPolyhedron is an intermediate class between description of a shape
// and visualization systems. It is intended to provide some service like:
//   - polygonization of shapes with triangulization (quadrilaterization)
//     of complex polygons;
//   - calculation of normals for faces and vertices;
//   - finding result of boolean operation on polyhedra;
//
// Public constructors:
//
//   SbPolyhedronBox (dx,dy,dz)
//                                        - create polyhedron for Box;
//   SbPolyhedronTrd1 (dx1,dx2,dy,dz)
//                                        - create polyhedron for G3 Trd1;
//   SbPolyhedronTrd2 (dx1,dx2,dy1,dy2,dz)
//                                        - create polyhedron for G3 Trd2;
//   SbPolyhedronTrap (dz,theta,phi, h1,bl1,tl1,alp1, h2,bl2,tl2,alp2)
//                                        - create polyhedron for G3 Trap;
//   SbPolyhedronPara (dx,dy,dz,alpha,theta,phi)
//                                        - create polyhedron for G3 Para;
//   SbPolyhedronTube (rmin,rmax,dz,nstep=0)
//                                        - create polyhedron for G3 Tube;
//   SbPolyhedronTubs (rmin,rmax,dz,phi1,dphi,nstep=0)
//                                        - create polyhedron for G3 Tubs;
//   SbPolyhedronCone (rmin1,rmax1,rmin2,rmax2,dz,nstep=0)
//                                        - create polyhedron for G3 Cone;
//   SbPolyhedronCons (rmin1,rmax1,rmin2,rmax2,dz,phi1,dphi,nstep=0)
//                                        - create polyhedron for G3 Cons;
//   SbPolyhedronPgon (phi,dphi,npdv,nz, z(*),rmin(*),rmax(*))
//                                        - create polyhedron for G3 Pgon;
//   SbPolyhedronPcon (phi,dphi,nz, z(*),rmin(*),rmax(*))
//                                        - create polyhedron for G3 Pcon;
//   SbPolyhedronSphere (rmin,rmax,phi,dphi,the,dthe,nstep=0)
//                                        - create polyhedron for Sphere;
//   SbPolyhedronTorus (rmin,rmax,rtor,phi,dphi,nstep=0)
//                                        - create polyhedron for Torus;
// Public functions:
//
//   GetNoVertices ()       - returns number of vertices;
//   GetNoFacets ()         - returns number of faces;
//   GetNextVertexIndex (index,edgeFlag) - get vertex indeces of the
//                            quadrilaterals in order;
//                            returns false when finished each face;
//   GetVertex (index)      - returns vertex by index;
//   GetNextVertex (vertex,edgeFlag) - get vertices with edge visibility
//                            of the quadrilaterals in order;
//                            returns false when finished each face;
//   GetNextVertex (vertex,edgeFlag,normal) - get vertices with edge
//                            visibility and normal of the quadrilaterals
//                            in order; returns false when finished each face;
//   GetNextEdgeIndeces (i1,i2,edgeFlag) - get indeces of the next edge;
//                            returns false for the last edge;
//   GetNextEdgeIndeces (i1,i2,edgeFlag,iface1,iface2) - get indeces of
//                            the next edge with indeces of the faces
//                            to which the edge belongs;
//                            returns false for the last edge;
//   GetNextEdge (p1,p2,edgeFlag) - get next edge;
//                            returns false for the last edge;
//   GetNextEdge (p1,p2,edgeFlag,iface1,iface2) - get next edge with indeces
//                            of the faces to which the edge belongs;
//                            returns false for the last edge;
//   GetFacet (index,n,nodes,edgeFlags=0,normals=0) - get face by index;
//   GetNextFacet (n,nodes,edgeFlags=0,normals=0) - get next face with normals
//                            at the nodes; returns false for the last face;
//   GetNormal (index)      - get normal of face given by index;
//   GetUnitNormal (index)  - get unit normal of face given by index;
//   GetNextNormal (normal) - get normals of each face in order;
//                            returns false when finished all faces;
//   GetNextUnitNormal (normal) - get normals of unit length of each face
//                            in order; returns false when finished all faces;
//   GetSurfaceArea()       - get surface area of the polyhedron;
//   GetVolume()            - get volume of the polyhedron;
//   GetNumberOfRotationSteps()   - get number of steps for whole circle;
//   SetNumberOfRotationSteps (n) - set number of steps for whole circle;
//   ResetNumberOfRotationSteps() - reset number of steps for whole circle
//                            to default value;
// History:
//
// 20.06.96 Evgeni Chernyaev <Evgueni.Tcherniaev@cern.ch> - initial version
//
// 23.07.96 John Allison
// - added GetNoVertices, GetNoFacets, GetNextVertex, GetNextNormal
//
// 30.09.96 E.Chernyaev
// - added GetNextVertexIndex, GetVertex by Yasuhide Sawada
// - added GetNextUnitNormal, GetNextEdgeIndeces, GetNextEdge
// - improvements: angles now expected in radians
//                 int -> G4int, double -> G4double  
// - G4ThreeVector replaced by either G4Point3D or G4Normal3D
//
// 15.12.96 E.Chernyaev
// - private functions G4PolyhedronAlloc, G4PolyhedronPrism renamed
//   to AllocateMemory and CreatePrism
// - added private functions GetNumberOfRotationSteps, RotateEdge,
//   RotateAroundZ, SetReferences
// - rewritten G4PolyhedronCons;
// - added G4PolyhedronPara, ...Trap, ...Pgon, ...Pcon, ...Sphere, ...Torus,
//   so full List of implemented shapes now looks like:
//   BOX, TRD1, TRD2, TRAP, TUBE, TUBS, CONE, CONS, PARA, PGON, PCON,
//   SPHERE, TORUS
//
// 01.06.97 E.Chernyaev
// - RotateAroundZ modified and SetSideFacets added to allow Rmin=Rmax
//   in bodies of revolution
//
// 24.06.97 J.Allison
// - added static private member fNumberOfRotationSteps and static public
//   functions void SetNumberOfRotationSteps (G4int n) and
//   void ResetNumberOfRotationSteps ().  Modified
//   GetNumberOfRotationSteps() appropriately.  Made all three functions
//   inline (at end of this .hh file).
//   Usage:
//    G4Polyhedron::SetNumberOfRotationSteps
//     (fpView -> GetViewParameters ().GetNoOfSides ());
//    pPolyhedron = solid.CreatePolyhedron ();
//    G4Polyhedron::ResetNumberOfRotationSteps ();
//
// 19.03.00 E.Chernyaev
// - added boolean operations (add, subtract, intersect) on polyhedra;
//
// 25.05.01 E.Chernyaev
// - added GetSurfaceArea() and GetVolume();
//

#include <Inventor/SbVec3d.h>
#include <Inventor/SbString.h>

#include <HEPVis/SbRotation.h> //using doubles instead of floats.

//NOTE : SbVec3d() does not initialize its internal fields.
//       To mimick the CLHEP/Point3D that initializes its fields,
//       we introduce the below HVPoint3D.
class HVPoint3D : public SbVec3d {
public:
  HVPoint3D();
  HVPoint3D(double x,double y,double z);
  HVPoint3D(const HVPoint3D& v);
  HVPoint3D(const SbVec3d& v);
  HVPoint3D& operator=(const HVPoint3D& v);
  HVPoint3D& operator=(const SbVec3d& v);
  friend HVPoint3D operator +(const HVPoint3D& v1, const HVPoint3D& v2);
};

HVPoint3D operator +(const HVPoint3D& v1,const HVPoint3D& v2);

typedef HVPoint3D HVNormal3D;
typedef HVPoint3D HVVector3D;

#ifndef SWIG
#include <iostream>
#endif

#ifndef DEFAULT_NUMBER_OF_STEPS
#define DEFAULT_NUMBER_OF_STEPS 24
#endif

class SbFacet {
  friend class SbPolyhedron;
#ifndef SWIG
  friend std::ostream& operator<<(std::ostream&, const SbFacet &facet);
  //G.Barrand
  friend int operator == (const SbFacet & v1, const SbFacet & v2);
  friend int operator != (const SbFacet & v1, const SbFacet & v2);
#endif

 private:
  typedef struct { int v,f; } edge_t; //G.Barrand
  edge_t edge[4];

 public:
  SbFacet(int v1=0, int f1=0, int v2=0, int f2=0, 
          int v3=0, int f3=0, int v4=0, int f4=0)
  { edge[0].v=v1; edge[0].f=f1; edge[1].v=v2; edge[1].f=f2;
    edge[2].v=v3; edge[2].f=f3; edge[3].v=v4; edge[3].f=f4; }

  SbFacet(const SbFacet & aFrom) {
    edge[0].v = aFrom.edge[0].v;
    edge[0].f = aFrom.edge[0].f;
    edge[1].v = aFrom.edge[1].v;
    edge[1].f = aFrom.edge[1].f;
    edge[2].v = aFrom.edge[2].v;
    edge[2].f = aFrom.edge[2].f;
    edge[3].v = aFrom.edge[3].v;
    edge[3].f = aFrom.edge[3].f;
  }
  SbFacet& operator=(const SbFacet& aFrom) {
    edge[0].v = aFrom.edge[0].v;
    edge[0].f = aFrom.edge[0].f;
    edge[1].v = aFrom.edge[1].v;
    edge[1].f = aFrom.edge[1].f;
    edge[2].v = aFrom.edge[2].v;
    edge[2].f = aFrom.edge[2].f;
    edge[3].v = aFrom.edge[3].v;
    edge[3].f = aFrom.edge[3].f;
    return *this;
  }
  bool isEqual(const SbFacet& aFrom) const { //G.Barrand
    if(edge[0].v!=aFrom.edge[0].v) return false;
    if(edge[0].f!=aFrom.edge[0].f) return false;

    if(edge[1].v!=aFrom.edge[1].v) return false;
    if(edge[1].f!=aFrom.edge[1].f) return false;

    if(edge[2].v!=aFrom.edge[2].v) return false;
    if(edge[2].f!=aFrom.edge[2].f) return false;

    if(edge[3].v!=aFrom.edge[3].v) return false;
    if(edge[3].f!=aFrom.edge[3].f) return false;

    return true;
  }
  void GetEdge(int i,int& v,int& f) const { //G.Barrand
    v = edge[i].v;
    f = edge[i].f;
  }

  void Set(int v[8]) //G.Barrand 
  { edge[0].v = v[0]; edge[0].f = v[1]; 
    edge[1].v = v[2]; edge[1].f = v[3];
    edge[2].v = v[4]; edge[2].f = v[5]; 
    edge[3].v = v[6]; edge[3].f = v[7]; }
};

//G.Barrand :
int operator == (const SbFacet & v1, const SbFacet & v2);
int operator != (const SbFacet & v1, const SbFacet & v2);

class SbPolyhedron {
#ifndef SWIG
  friend std::ostream& operator<<(std::ostream&, const SbPolyhedron &ph);
  //G.Barrand
  friend int operator == (const SbPolyhedron & v1, const SbPolyhedron & v2);
  friend int operator != (const SbPolyhedron & v1, const SbPolyhedron & v2);
#endif

 private:
  static int fNumberOfRotationSteps;

 private: //G.Barrand
  SbString* name; //have a pointer to optimize memory.
 protected:
  int nvert, nface;
  HVPoint3D *pV;
  SbFacet    *pF;

  // Allocate memory for SbPolyhedron
  void AllocateMemory(int Nvert, int Nface);

  // Find neighbouring facet
  int FindNeighbour(int iFace, int iNode, int iOrder) const;

  // Find normal at node
  HVNormal3D FindNodeNormal(int iFace, int iNode) const;

  // Create SbPolyhedron for prism with quadrilateral base
  void CreatePrism();

  // Generate facets by revolving an edge around Z-axis
  void RotateEdge(int k1, int k2, double r1, double r2,
                  int v1, int v2, int vEdge,
                  bool ifWholeCircle, int ns, int &kface);

  // Set side facets for the case of incomplete rotation
  void SetSideFacets(int ii[4], int vv[4],
                     int *kk, double *r,
                     double dphi, int ns, int &kface);

  // Create SbPolyhedron for body of revolution around Z-axis
  void RotateAroundZ(int nstep, double phi, double dphi,
                     int np1, int np2,
                     const double *z, double *r,
                     int nodeVis, int edgeVis);

  // For each edge set reference to neighbouring facet
  void SetReferences();

  // Invert the order on nodes in facets
  void InvertFacets();

 public:
  // Constructor
  SbPolyhedron(int Nvert=0, int Nface=0)
    : name(0), //G.Barrand
      nvert(Nvert), nface(Nface),
      pV(Nvert ? new HVPoint3D[Nvert+1] : 0),
      pF(Nface ? new SbFacet[Nface+1] : 0) {}

  //G.Barrand : handle a name to help debugging.
  void setName(const SbString& aName) { 
    delete name;
    name = new SbString(aName);
  }
  SbString getName() const { 
    if(!name) return SbString();
    return *name;
  }
  //G.Barrand :end

  void Set(int Nvert, HVPoint3D* aV,
           int Nface, SbFacet* aF) //G.Barrand
  { delete [] pV; delete [] pF;
    nvert = Nvert; nface = Nface; pV = aV; pF = aF;}

  // Copy constructor
  SbPolyhedron(const SbPolyhedron & from);

  // Destructor
  virtual ~SbPolyhedron() { 
    delete name; //G.Barrand.
    delete [] pV; delete [] pF; 
  }

  // Assignment
  virtual SbPolyhedron & operator=(const SbPolyhedron & from);

  void Empty() //G.Barrand
  { nvert = 0; nface = 0; pV = 0;pF = 0;}

  // Get number of vertices
  int GetNoVertices() const { return nvert; }

  // Get number of facets
  int GetNoFacets() const { return nface; }

  // Transform the polyhedron
  //SbPolyhedron& Transform(const SbRotation& rot,const SbVec3f& trans);

  //G.Barrand : the same than upper but by using doubles.
  SbPolyhedron& Transform(const HEPVis::SbRotation& rot,
                          const SbVec3d& trans);

  //FIXME : have a double version.
  //SbPolyhedron & Transform(const SbRotation_d &rot, const SbVec3d& trans);

  // Get next vertex index of the quadrilateral
  inline //G.Barrand
  bool GetNextVertexIndex(int & index, int & edgeFlag) const;

  // Get vertex by index 
  HVPoint3D GetVertex(int index) const;
  const HVPoint3D& GetVertexFast(int index) const; //G.Barrand

  //G.Barrand : to optimize SoPolyhedron.
  HVPoint3D* GetPV() const {return pV;} //G.Barrand
  SbFacet* GetPF() const {return pF;} //G.Barrand

  // Get next vertex + edge visibility of the quadrilateral
  bool GetNextVertex(HVPoint3D & vertex, int & edgeFlag) const;

  // Get next vertex + edge visibility + normal of the quadrilateral
  bool GetNextVertex(HVPoint3D & vertex, int & edgeFlag,
                           HVNormal3D & normal) const;

  // Get indeces of the next edge with indeces of the faces
  bool GetNextEdgeIndeces(int & i1, int & i2, int & edgeFlag,
                                int & iface1, int & iface2) const;

  // Get indeces of the next edge
  bool GetNextEdgeIndeces(int & i1, int & i2, int & edgeFlag) const;

  // Get next edge
  bool GetNextEdge(HVPoint3D &p1, HVPoint3D &p2, int &edgeFlag) const;

  // Get next edge
  bool GetNextEdge(HVPoint3D &p1, HVPoint3D &p2, int &edgeFlag,
                         int &iface1, int &iface2) const;

  // Get face by index
  void GetFacet(int iFace, int &n, int *iNodes,
                int *edgeFlags = 0, int *iFaces = 0) const;

  // Get face by index
  void GetFacet(int iFace, int &n, HVPoint3D *nodes,
                int *edgeFlags = 0, HVNormal3D *normals = 0) const;

  // Get next face with normals at the nodes
  bool GetNextFacet(int &n, HVPoint3D *nodes,
                          int *edgeFlags=0, HVNormal3D *normals=0) const;

  // Get normal of the face given by index
  HVNormal3D GetNormal(int iFace) const;

  // Get unit normal of the face given by index
  HVNormal3D GetUnitNormal(int iFace) const;

  // Get normal of the next face
  bool GetNextNormal(HVNormal3D &normal) const;

  // Get normal of unit length of the next face 
  bool GetNextUnitNormal(HVNormal3D &normal) const;

  // Boolean operations 
  SbPolyhedron add(const SbPolyhedron &p) const;
  SbPolyhedron subtract(const SbPolyhedron &p) const;
  SbPolyhedron intersect(const SbPolyhedron &p) const;

  // Get area of the surface of the polyhedron
  double GetSurfaceArea() const;

  // Get volume of the polyhedron
  double GetVolume() const;

  bool isEqual(const SbPolyhedron &p) const; //G.Barrand
  bool isConsistent(const char* = 0) const; //G.Barrand
  void dump() const;

  // Get number of steps for whole circle
  static int GetNumberOfRotationSteps(); //G.Barrand : have the code in .cxx.

  // Set number of steps for whole circle
  static void SetNumberOfRotationSteps(int n);

  // Reset number of steps for whole circle to default value
  static void ResetNumberOfRotationSteps(); //G.Barrand : have code in .cxx.
};

//G.Barrand :
int operator == (const SbPolyhedron & v1, const SbPolyhedron & v2);
int operator != (const SbPolyhedron & v1, const SbPolyhedron & v2);

// G.Barrand : introduce iabs to avoid a mess with cmath and some compiler.
inline int Sb_iabs(int a) {
  return a < 0 ? -a : a;
}

inline //G.Barrand
bool SbPolyhedron::GetNextVertexIndex(int &index, int &edgeFlag) const
/***********************************************************************
 *                                                                     *
 * Name: SbPolyhedron::GetNextVertexIndex          Date:    03.09.96  *
 * Author: Yasuhide Sawada                          Revised:           *
 *                                                                     *
 * Function:                                                           *
 *                                                                     *
 ***********************************************************************/
{
  static int iFace = 1;
  static int iQVertex = 0;
  //G.Barrand : int vIndex = pF[iFace].edge[iQVertex].v;
  SbFacet::edge_t* edge = pF[iFace].edge; //G.Barrand : optimize.
  int vIndex = edge[iQVertex].v;

  edgeFlag = (vIndex > 0) ? 1 : 0;
  index = Sb_iabs(vIndex);

  if(index>nvert) {
    std::cerr << "SbPolyhedron::GetNextVertexIndex: pV index problem " 
              << index << " exceed " << nvert << std::endl;
    index = 0;
  }

  //G.Barrand : if (iQVertex >= 3 || pF[iFace].edge[iQVertex+1].v == 0) {
  if (iQVertex >= 3 || edge[iQVertex+1].v == 0) {
    iQVertex = 0;
    if (++iFace > nface) iFace = 1;
    return false;  // Last Edge
  }else{
    ++iQVertex;
    return true;  // not Last Edge
  }
}

class SbPolyhedronTrd2 : public SbPolyhedron {
 public:
  SbPolyhedronTrd2(double Dx1, double Dx2,
                    double Dy1, double Dy2, double Dz);
  virtual ~SbPolyhedronTrd2();
  virtual SbPolyhedron& operator = (const SbPolyhedron& from) {
    return SbPolyhedron::operator = (from);
  }
};

class SbPolyhedronTrd1 : public SbPolyhedronTrd2 {
 public:
  SbPolyhedronTrd1(double Dx1, double Dx2,
                    double Dy, double Dz);
  virtual ~SbPolyhedronTrd1();
  virtual SbPolyhedron& operator = (const SbPolyhedron& from) {
    return SbPolyhedron::operator = (from);
  }
};

class SbPolyhedronBox : public SbPolyhedronTrd2 {
 public:
  SbPolyhedronBox(double Dx, double Dy, double Dz);
  virtual ~SbPolyhedronBox();
  virtual SbPolyhedron& operator = (const SbPolyhedron& from) {
    return SbPolyhedron::operator = (from);
  }
};

class SbPolyhedronTrap : public SbPolyhedron {
public:
  SbPolyhedronTrap(double Dz, double Theta, double Phi,
                    double Dy1,
                    double Dx1, double Dx2, double Alp1,
                    double Dy2,
                    double Dx3, double Dx4, double Alp2);
  virtual ~SbPolyhedronTrap();
  virtual SbPolyhedron& operator = (const SbPolyhedron& from) {
    return SbPolyhedron::operator = (from);
  }
};

class SbPolyhedronPara : public SbPolyhedronTrap {
public:
  SbPolyhedronPara(double Dx, double Dy, double Dz,
                    double Alpha, double Theta, double Phi);
  virtual ~SbPolyhedronPara();
  virtual SbPolyhedron& operator = (const SbPolyhedron& from) {
    return SbPolyhedron::operator = (from);
  }
};

class SbPolyhedronCons : public SbPolyhedron {
public:
  SbPolyhedronCons(double Rmn1, double Rmx1, 
                   double Rmn2, double Rmx2, double Dz,
                   double Phi1, double Dphi,
                   int nstep = 0); //G.Barrand
  virtual ~SbPolyhedronCons();
  virtual SbPolyhedron& operator = (const SbPolyhedron& from) {
    return SbPolyhedron::operator = (from);
  }
};

class SbPolyhedronCone : public SbPolyhedronCons {
public:
  SbPolyhedronCone(double Rmn1, double Rmx1, 
                   double Rmn2, double Rmx2, double Dz,
                   int nstep = 0); //G.Barrand
  virtual ~SbPolyhedronCone();
  virtual SbPolyhedron& operator = (const SbPolyhedron& from) {
    return SbPolyhedron::operator = (from);
  }
};

class SbPolyhedronTubs : public SbPolyhedronCons {
public:
  SbPolyhedronTubs(double Rmin, double Rmax, double Dz, 
                   double Phi1, double Dphi,
                   int nstep = 0); //G.Barrand
  virtual ~SbPolyhedronTubs();
  virtual SbPolyhedron& operator = (const SbPolyhedron& from) {
    return SbPolyhedron::operator = (from);
  }
};

class SbPolyhedronTube : public SbPolyhedronCons {
public:
  SbPolyhedronTube (double Rmin, double Rmax, double Dz,
                    int nstep = 0); //G.Barrand
  virtual ~SbPolyhedronTube();
  virtual SbPolyhedron& operator = (const SbPolyhedron& from) {
    return SbPolyhedron::operator = (from);
  }
};

class SbPolyhedronPgon : public SbPolyhedron {
public:
  SbPolyhedronPgon(double phi, double dphi, int npdv, int nz,
                    const double *z,
                    const double *rmin,
                    const double *rmax);
  virtual ~SbPolyhedronPgon();
  virtual SbPolyhedron& operator = (const SbPolyhedron& from) {
    return SbPolyhedron::operator = (from);
  }
};

class SbPolyhedronPcon : public SbPolyhedronPgon {
public:
  SbPolyhedronPcon(double phi, double dphi, int nz,
                    const double *z,
                    const double *rmin,
                    const double *rmax);
  virtual ~SbPolyhedronPcon();
  virtual SbPolyhedron& operator = (const SbPolyhedron& from) {
    return SbPolyhedron::operator = (from);
  }
};

class SbPolyhedronSphere : public SbPolyhedron {
public:
  SbPolyhedronSphere(double rmin, double rmax,
                     double phi, double dphi,
                     double the, double dthe,
                     int nphi = 0,
                     int nthe = 0); //G.Barrand
  virtual ~SbPolyhedronSphere();
  virtual SbPolyhedron& operator = (const SbPolyhedron& from) {
    return SbPolyhedron::operator = (from);
  }
};

class SbPolyhedronTorus : public SbPolyhedron {
public:
  SbPolyhedronTorus(double rmin, double rmax, double rtor,
                    double phi, double dphi,
                    int nphi = 0,
                    int nthe = 0); //G.Barrand
  virtual ~SbPolyhedronTorus();
  virtual SbPolyhedron& operator = (const SbPolyhedron& from) {
    return SbPolyhedron::operator = (from);
  }
};

#include <vector>

//G.Barrand : begin
class SbPolyhedronProcessor {
public:
  enum Operation { //Must be the same than BooleanProcessor OP_XXX.
     UNION  = 0
    ,INTERSECTION = 1
    ,SUBTRACTION = 2
  };
private:
  typedef std::pair<Operation,SbPolyhedron> op_t;
public:
  SbPolyhedronProcessor();
  virtual ~SbPolyhedronProcessor();
private:
  SbPolyhedronProcessor(const SbPolyhedronProcessor&);
  SbPolyhedronProcessor& operator=(const SbPolyhedronProcessor&);
public:
  void push_back(Operation,const SbPolyhedron&);
  SbBool execute(SbPolyhedron&);
  void clear();
  SbBool is_same_op() const;
//private:
  SbBool execute1(SbPolyhedron&,const std::vector<unsigned int>&);
private:
  std::vector<op_t> m_ops; 
};
//G.Barrand : end


#endif
