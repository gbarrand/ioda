
#include <HEPVis/SbMath.h>
#include <HEPVis/nodes/SoTorus.h>

#include <assert.h>

//////////////////////////////////////////////////////////////////////////////////
//
// G. Alverson, CMS/Northeastern University Software Group
// 12 June 1998
//
//
// SoTorus creates a torus.
//
//
// The torus is constructed as a NURB surface and as such has all the
// advantages (automatic selection of appropriate granularity, concise representation,
// relatively easy construction) and the disadvantages (NURBs are buggy in OIV until
// release 2.5 with the NURBs patch).
//
//////////////////////////////////////////////////////////////////////////////////

const int SoTorus::NPTS = SO_TORUS_U_DIM*SO_TORUS_V_DIM;
const float SoTorus::u_knot_vals[]= {0,0,0,1,1,2,2,3,3,3};
const float SoTorus::v_knot_vals[]= {0,0,0,1,1,2,2,3,3,3};

SO_NODE_SOURCE(SoTorus)

void SoTorus::initClass(){
        SO_NODE_INIT_CLASS(SoTorus, SoShape, "Shape");
}

SoTorus::SoTorus() {
        SO_NODE_CONSTRUCTOR(SoTorus);

        SO_NODE_ADD_FIELD(rMajor, (2));
        SO_NODE_ADD_FIELD(rMinor, (1));
        SO_NODE_ADD_FIELD(center, (SbVec3f(0,0,0)));
        SO_NODE_ADD_FIELD(alternateRep, (NULL));
        children = new SoChildList(this);

        centerLocal.setValue(0,0,0);
}

SoTorus::~SoTorus(){
 delete children;
}

void SoTorus::generateChildren(){
        assert(children->getLength() ==0);
//  Create a separator to hang the co-ords and the surface off of 
        SoSeparator        *torSeparator        = new SoSeparator();
        SoTranslation        *torTranslation        = new SoTranslation();

//  Create the co-ordinates... 
    SoCoordinate4        *torCoordinate4        = new SoCoordinate4;
    torCoordinate4->point.setNum(SoTorus::NPTS);

//  Create the surface 
    SoNurbsSurface        *torSurface = new SoNurbsSurface;
    torSurface->numUControlPoints.setValue(SO_TORUS_U_DIM);
    torSurface->numVControlPoints.setValue(SO_TORUS_V_DIM);
    torSurface->uKnotVector.setValues(0,SO_TORUS_U_DIM+SO_TORUS_U_ORDER,u_knot_vals);
    torSurface->vKnotVector.setValues(0,SO_TORUS_V_DIM+SO_TORUS_V_ORDER,v_knot_vals);

//  Tie them all together
    torSeparator->addChild(torTranslation);
    torSeparator->addChild(torCoordinate4);
    torSeparator->addChild(torSurface);
    
    torTranslation->translation.connectFrom(&center);
    
    children->append(torSeparator);
    
    updateChildren();
}
void SoTorus::generateAlternateRep(SoAction*) {
        if (children->getLength() == 0) generateChildren();
        updateChildren();
        alternateRep.setValue(( *children)[0]);
}
void SoTorus::clearAlternateRep() {
        alternateRep.setValue(NULL);
}

void SoTorus::updateChildren() {



        assert(children->getLength()==1);
//  Check to see if we actually need to change anything
        if( rMajor.getValue() == rMajorLocal && 
                rMinor.getValue() == rMinorLocal &&
                center.getValue() == centerLocal)
                return;

        rMajorLocal = rMajor.getValue();
        rMinorLocal = rMinor.getValue();
        centerLocal = center.getValue();


        SoSeparator* torSeparator = (SoSeparator*)((*children)[0]);
        SoCoordinate4 *torCoordinate4 = (SoCoordinate4*)(torSeparator->getChild(1));
        //SoNurbsSurface *torSurface = (SoNurbsSurface*)(torSeparator->getChild(2));

        int iu, iv, i;
        double angleU, angleV;

/*  Generate the Control Points */
    double weight, weightU, weightV;
    SbVec3f cpt;
        SbRotation rotator;

/*  Set up the basic grid of control points */
    for (iv=0, angleV=0; iv<SO_TORUS_V_DIM; iv++, angleV += M_PI/3) {
                rotator.setValue(SbVec3f(0,0,1),(float)angleV);
                if (iv%2 ==1) {
                        weightV = 0.5;}
                else {
                        weightV = 1;}
       for (iu=0,angleU=0; iu<SO_TORUS_U_DIM; iu++, angleU -= M_PI/3) {  /* get the order this way to set exterior of surf */
        i = iu + iv*SO_TORUS_U_DIM;
        if (iu%2 == 1) {
                        weightU = 0.5;}
        else {
                        weightU = 1;
        }
                weight = weightU*weightV;
        cpt = SbVec3f(rMajor.getValue(),0,0)/(float)weightV + 
          SbVec3f((float)(rMinor.getValue()*cos(angleU)/weight),
                  0,
                  (float)(rMinor.getValue()*sin(angleU)/weightU));
                rotator.multVec(cpt,cpt);
                cpt *= (float)weight;
        torCoordinate4->point.set1Value(i,cpt[0],cpt[1],cpt[2],(float)weight);
      }
    }
}
void SoTorus::computeBBox(SoAction *, SbBox3f &box, SbVec3f &TheCenter ){
// replacement for pure virtual function in SoShape
        float rExtent = rMajor.getValue()+rMinor.getValue();
        float zExtent = rMinor.getValue();
        SbVec3f extent(rExtent,rExtent,zExtent);
        box.setBounds(center.getValue()-extent,center.getValue()+extent);
        TheCenter = center.getValue();
}
void SoTorus::generatePrimitives(SoAction *action){
//replacement for pure virtual function in SoShape
        if (children->getLength()==0) generateChildren();
        updateChildren();
        children->traverse(action);
}
SoChildList *SoTorus::getChildren() const {
//replacement for SoShape virtual function
        return children;
}
