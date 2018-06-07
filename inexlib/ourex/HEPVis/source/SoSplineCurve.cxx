// this : 
#include <HEPVis/nodes/SoSplineCurve.h>

// Inventor :
#include <Inventor/SbLinear.h>
#include <Inventor/SbBox.h>
#include <Inventor/misc/SoChildList.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoNurbsCurve.h>
#include <Inventor/nodes/SoCoordinate4.h>

// HEPVis :
#include <HEPVis/SbMath.h>
#include <HEPVis/actions/SoAlternateRepAction.h>

// Else :
#include <float.h> /* Defines FLT_MAX.  */

//#define DEBUG
#ifdef DEBUG
#include <iostream>
#endif

//////////////////////////////////////////////////////////////////////////////
//
// Original SoSplineTrack version from :
//   G. Alverson, CMS/Northeastern University Software Group
//   11 June 1999
//   SoSplineCurve is yet another spiral curve class.
//
// SoSplineCurve version from :
//   G. Barrand, LAL. eXtreme debugger group.
//   July 2004
//
//////////////////////////////////////////////////////////////////////////////

const float M_SQRT3_2 =  0.8660254037844F; // sqrt(3)/2 for sin(60 degrees)
const int SoSplineCurve::NORDER = 3;

SO_NODE_SOURCE(SoSplineCurve)

void SoSplineCurve::initClass(){
  SO_NODE_INIT_CLASS( SoSplineCurve, SoShape, "Shape");
}

SoSplineCurve::SoSplineCurve()
{
  SO_NODE_CONSTRUCTOR( SoSplineCurve );

  SO_NODE_ADD_FIELD( points, ( SbVec3f( 0, 0, 0 )));
  SO_NODE_ADD_FIELD( tangents, ( SbVec3f( 0, 0, 0 )));
  SO_NODE_ADD_FIELD( alternateRep, ( NULL ));

  children = new SoChildList( this );

  //  straight line
  SbVec3f defPoints[3] = {
    SbVec3f (1.F,0.F,0.F),
    SbVec3f (2.F,0.F,0.F),
    SbVec3f (3.F,0.F,0.F)
  };
  SbVec3f defTangents[3] = {
    SbVec3f (1.F,0.F,0.F),
    SbVec3f (1.F,0.F,0.F),
    SbVec3f (1.F,0.F,0.F)
  };
  points.setValues(0,3,defPoints);
  tangents.setValues(0,3,defTangents);
}

SoSplineCurve::~SoSplineCurve() {
  delete children;
}

void SoSplineCurve::generateChildren()
{
  if(children->getLength()!=0) return; //Done.

  SoSeparator* spiralSeparator = new SoSeparator();

  SoCoordinate4* spiralCoordinate4 = new SoCoordinate4;
  spiralSeparator->addChild(spiralCoordinate4);

  SoNurbsCurve* spiralNurbsCurve = new SoNurbsCurve;
  spiralSeparator->addChild(spiralNurbsCurve);

  children->append(spiralSeparator);
  updateChildren();
}

void SoSplineCurve::generateAlternateRep(SoAction*)
{
  if (children->getLength() == 0) generateChildren();
  updateChildren();
  alternateRep = ( *children)[0];
}

void SoSplineCurve::clearAlternateRep()
{
  alternateRep = NULL;
}

void SoSplineCurve::updateChildren()
{
  if (children->getLength() != 1) return;
  int npts = points.getNum();  
  if(npts<=1) return;

  SbBool flag = enableNotify(FALSE);

  SoSeparator* spiralSeparator = (SoSeparator*)(*children)[0];
  SoCoordinate4* spiralCoordinate4 = 
    (SoCoordinate4*)spiralSeparator->getChild(0);
  SoNurbsCurve* spiralNurbsCurve =  
    (SoNurbsCurve*)spiralSeparator->getChild(1);

  // Start by assuming that we won't have to insert any extra points
  SbVec4f* control_points = new SbVec4f[2*npts-1];

  //dimension of the control_points + NORDER
  float* knot_vals = new float[ 2 * npts - 1 + SoSplineCurve::NORDER ];

  int ik = 0; //pointer for knots
  int ipi = 0; // in pointer for control points 
  int ik_count = 0; //current value for knot
  int ipo = 0;  // out pointer for control points
  float x,y,z,w = 1.0F; //x,y,z co-ords of SbVec4f, w = weight
  // the end points require an extra knot
  knot_vals[ik++] = (float)ik_count;
  // Now do the loop over points, converting to control points
  while (ipi<npts-1) {
    knot_vals[ik++] = (float)ik_count;
    knot_vals[ik++] = (float)(ik_count++);
    points[ipi].getValue(x,y,z);
    control_points[ipo++] = SbVec4f(x,y,z,w);
    // Calculate the mid control point :
    control_points[ipo++] = 
      midpoint(points[ipi],points[ipi+1],tangents[ipi],tangents[ipi+1]);
    ipi++;
  }

  // Put in the last point
  points[ipi].getValue(x,y,z);
  control_points[ipo++] = SbVec4f(x,y,z,w);
  knot_vals[ik++] = (float)ik_count;
  knot_vals[ik++] = (float)ik_count;
  knot_vals[ik++] = (float)ik_count;

  // Now stuff it all in the NURB
  spiralCoordinate4->point.setValues(0,ipo,control_points);

  spiralNurbsCurve->knotVector.setValues(0,ik,knot_vals);
  spiralNurbsCurve->numControlPoints.setValue(ipo);

  delete [] knot_vals;
  delete [] control_points;

  enableNotify(flag);
}

SbVec4f SoSplineCurve::midpoint( 
 SbVec3f pa
,SbVec3f pb
,SbVec3f ta
,SbVec3f tb 
)
{
  // generate the control point between two given points: pa and pb, and 
  // two tangents: ta and tb.
  // return the zero vector if there's a problem

  const float eps = 0.000001F;

  //  Will solve for an approximate helix using the available invariants.
  //      Parametrize result as h[phi] = rho ( cos[phi], sin[phi], zeta*phi)
  //  Refer to this frame as the MidPoint (MP) frame
  fParamA = tb.dot(ta);
  fParamC = (pb-pa).dot(pb-pa);
  fParamD = tb.dot(ta.cross(pb-pa));

  SbVec3f midpt3s((pa+pb)/2.F);
  SbVec4f midpt4s(midpt3s[0],midpt3s[1],midpt3s[2],1.F);
  //      Tests for pathological cases
  //      Straight Line   
  if( FFABS(fParamC)<eps || 
      ( FFABS(fParamA-1) < eps && FFABS(fParamD)/fParamC < eps) ) {
    return midpt4s;
  }

  //      Linearly dependent but not straight
  SbVec3f perp = ta.cross(tb);
  if (FFABS(perp.dot(perp)) < eps) return midpt4s;

  // indeterminate solution in the root finder: add a tiny 
  // bit to fParamD, enough to kick it out of danger but not 
  // enough to perturb the solution much.
  if (FFABS(fParamD)<eps) fParamD = -1.5f*eps;

  //  try for a solution
  fParamSign = 1;

  SbBool err;
  float delta = findRoot( static_cast< float (*)( float, void * )>( SoSplineCurve::rootFunction),0.01F, FM_PI, eps, this, err );

  if (err==TRUE) {
    fParamSign = -1;
    delta = findRoot( static_cast< float (*)( float, void *)>( SoSplineCurve::rootFunction ),0.01F, FM_PI, eps, this, err );
    if (err==TRUE) {
#ifdef DEBUG
      std::cout << "Problem in SoSplineCurve. Can't find root. " << std::endl;
      float x,y,z;
      pa.getValue(x,y,z);
      std::cout << "pa: " << x << ", " << y << ", " << z << std::endl;
      pb.getValue(x,y,z);
      std::cout << "pb: " << x << ", " << y << ", " << z << std::endl;
      ta.getValue(x,y,z);
      std::cout << "ta: " << x << ", " << y << ", " << z << std::endl;
      tb.getValue(x,y,z);
      std::cout << "tb: " << x << ", " << y << ", " << z << std::endl;
#endif //DEBUG                  
      return midpt4s;
    }
  }
        
  float zeta = fParamSign * FSQRT(FFABS((fParamA-FCOS(delta))/(1.0-fParamA)));
  float rho = FSQRT(fParamC/(2+delta*delta*zeta*zeta-2*FCOS(delta)));

  float wt = FFABS(FCOS(delta/2.));
#ifdef DEBUG
  if (wt < eps) {
    std::cout << "Problem in SoSplineCurve. Wt = " << wt << std::endl;
    std::cout << "    params: " << fParamA <<","<< fParamC << "," << fParamD << std::endl;
  }
#endif //DEBUG  

  //  Calculate mid-point at delta/2
  SbVec3f midpt3(rho*FCOS(delta/2.f)/wt,
                 rho*FSIN(delta/2.f)/wt,
                 rho*zeta*delta/2.f);

  //  Rotate to lab frame by aligning ta in this (MidPoint) frame 
  //with ta in lab

  SbVec3f taMP(0,1,zeta);
  taMP.normalize();
  SbRotation rotBack(taMP,ta);

  //  To transform from MP frame to lab frame:
  SbMatrix xform;
  xform.setRotate(rotBack); //xform now represents the rotation

  //  Need to translate back from pa to transformed origin
  rotBack.inverse();  // form inverse rotation 
  SbVec3f paMP(rho,0,0); 
  SbVec3f xlateBack;
  //get vector from pa to origin (of curve) in lab frame
  rotBack.multVec(-paMP,xlateBack);

  //add displacement to pa in lab frame => vector from origin of curve
  xlateBack += pa;

  //to origin of curve in lab
  SbMatrix transMat;
  transMat.setTranslate(xlateBack);
  xform.multRight(transMat);  //add translation back to total transform

  xform.multVecMatrix(midpt3,midpt3);
  SbVec4f midpt(midpt3[0],midpt3[1],midpt3[2],1.F); //convert from Vec3 to Vec4
  midpt *= wt; //and set the correct overall weight
        
  return midpt;
}

void SoSplineCurve::computeBBox( 
 SoAction*
,SbBox3f& box
,SbVec3f& TheCenter
)
{
  if (children->getLength()==0) generateChildren();
  updateChildren();

  // replacement for pure virtual function in SoShape
  float zExtentMax = -FLT_MAX, zExtentMin = FLT_MAX;
  float xExtentMax = -FLT_MAX, xExtentMin = FLT_MAX;
  float yExtentMax = -FLT_MAX, yExtentMin = FLT_MAX;

  SoSeparator* spiralSeparator = (SoSeparator*)(*children)[0];
  SoCoordinate4* spiralCoordinate4 = 
    (SoCoordinate4*)spiralSeparator->getChild(0);
  SoNurbsCurve* spiralNurbsCurve =  
    (SoNurbsCurve*)spiralSeparator->getChild(1);

  const SbVec4f *pointCopy = spiralCoordinate4->point.getValues(0);
  float x, y, z, w;
  for (int i=0; 
       i<spiralNurbsCurve->numControlPoints.getValue(); 
       i++, pointCopy++) {
    pointCopy->getValue(x,y,z,w);
    x = x/w; y = y/w; z = z/w;
    if (x > xExtentMax) xExtentMax = x;
    if (x < xExtentMin) xExtentMin = x;
    if (y > yExtentMax) yExtentMax = y;
    if (y < yExtentMin) yExtentMin = y;
    if (z > zExtentMax) zExtentMax = z;
    if (z < zExtentMin) zExtentMin = z;
  }
  box.setBounds(SbVec3f(xExtentMin,yExtentMin,zExtentMin),
                SbVec3f(xExtentMax,yExtentMax,zExtentMax));
  TheCenter = 0.5f*SbVec3f(xExtentMin+xExtentMax,
                           yExtentMin+yExtentMax,
                           zExtentMin+zExtentMax);
}

void SoSplineCurve::generatePrimitives( SoAction* action ) {
  if (children->getLength()==0) generateChildren();
  updateChildren();
  children->traverse(action);
}

SoChildList* SoSplineCurve::getChildren() const
{
  //replacement for SoNode virtual function
  return children;
}

float SoSplineCurve::rootFunction( float x, void* This)
{
  // rootFunction evaluates the formula for the equation defining
  // the opening angle of a helical curve in terms of the invariants
  // (which are passed by the void pointer This). The argument x represents
  // the possible values of the angle.
  SoSplineCurve* t = (SoSplineCurve*)This;
  float a = t->fParamA;  
  float c = t->fParamC; 
  float d = t->fParamD; 
  int sign = t->fParamSign;

  float f = -sign*d;
  float denom = -2*(1-FCOS(x))*(1-a) +x*x*(FCOS(x)-a); 
  float numerator = c * (FCOS(x)-a);
  float radical = numerator / denom; //might as well return INF if denom=0
  if (radical > 0) {
    radical = FSQRT(radical);  //return Re part
  } else {
    radical = 0; //-sqrt(-radical); //return -Im part
  }
                
  f += radical * (1-a)*(-2+2*FCOS(x)+x*FSIN(x))/(FCOS(x)-1);
  return f;
}

float SoSplineCurve::findRoot( 
 float (* func )( float, void * )
,float x1
,float x2
,float xacc
,void* data
,SbBool& err 
)
{
  // Find a root using bisection (see Numerical Recipies for complete documentation)
  err = FALSE;
  int j;
  float dx,f,fmid,xmid,rtb;
  const int JMAX = 40;

  f=(*func)(x1, data);
  fmid=(*func)(x2,data);
  //      Might want to insert a check here to see if either of the endpoints or the midpoint
  //      are within xacc of 0...
  if (f*fmid >=0.0) {
    err = TRUE;
#ifdef DEBUG
    std::cout << "SoSplineCurve f*fmid >= 0" << std::endl;
    std::cout << "  f="<< f << "  fmid=" << fmid << std::endl;
#endif //DEBUG
    return 0.0;
  } else {
    rtb = f < 0.0 ? (dx=x2-x1,x1) : (dx=x1-x2,x2);
    for (j=1; j<JMAX; j++) {
      fmid=(*func)(xmid=rtb+(dx *= 0.5),data);
      if (fmid <= 0.0) rtb=xmid;
      if (FFABS(dx) < xacc || fmid == 0.0f) return rtb;
    }
    err = TRUE;
#ifdef DEBUG
    std::cout << "SoSplineCurve: findRoot failed to converge" << std::endl;
    std::cout << "  f="<< f << "  fmid=" << fmid << "  rtb=" << rtb 
              << std::endl;
#endif //DEBUG
    return 0;
  }
}

//////////////////////////////////////////////////////////////////////////////
void SoSplineCurve::doAction(
 SoAction* aAction
) 
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  SO_ALTERNATEREP_DO_ACTION(aAction)
  SoShape::doAction(aAction);
}
