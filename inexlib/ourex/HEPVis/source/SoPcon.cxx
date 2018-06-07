// this :
#include <HEPVis/nodes/SoPcon.h>

// Inventor :
#include <Inventor/nodes/SoFaceSet.h>
#include <Inventor/nodes/SoTransform.h>
#include <Inventor/nodes/SoVertexProperty.h>
#include <Inventor/nodes/SoShapeHints.h>

// Else :
#include <vector>
#include <float.h> // For FLT_MAX.
#include <assert.h>
#include <algorithm>

//////////////////////////////////////////////////////////////////////////////////
//
// G. Alverson, CMS/Northeastern University Software Group
// 12 June 1998
//
//
// SoPcon creates a PCON.
//
//
//
//////////////////////////////////////////////////////////////////////////////////


//  Initialise static/const values
const int SoPcon::V_DIM = 7;
const int SoPcon::V_ORDER = 3;
const int SoPcon::U_ORDER = 2;

// set up some typedefs so if we change the object we don't have to 
// change the code...

typedef std::vector<SbVec3f> OrderedPair;
using std::sort;

typedef OrderedPair::iterator OrderedPairIt;

// force instantiation to avoid problems when building as a library module
#ifndef _WIN32
template class std::vector<SbVec3f>;
#endif

SO_NODE_SOURCE(SoPcon)

SoPcon::SoPcon() {
        SO_NODE_CONSTRUCTOR(SoPcon);


        SO_NODE_ADD_FIELD(vtxPts, (SbVec3f(0,0,0)));
        SO_NODE_ADD_FIELD(center, (SbVec3f(0,0,0)));
        SO_NODE_ADD_FIELD(startAngle, (0));
        SO_NODE_ADD_FIELD(deltaAngle, (static_cast<float>(2*M_PI)));
        SO_NODE_ADD_FIELD(alternateRep, (NULL));
        children = new SoChildList(this);

      const SbVec3f defPcon[5] = {
              SbVec3f(0.2F,0,-1),SbVec3f(1,0,-0.8F),
              SbVec3f(1,0,0.8F),SbVec3f(0.2F,0,1),
              SbVec3f(0.2F,0,-1)};
        vtxPts.setValues(0,4,defPcon);
        ends = FALSE;
        nsurf = 1;

        centerLocal.setValue(0,0,0);
}

SoPcon::~SoPcon(){
 delete children;
}

void SoPcon::initClass(){
        SO_NODE_INIT_CLASS(SoPcon, SoShape, "Shape");
}

void SoPcon::generateChildren(){
        assert(children->getLength() ==0);
//  Create a separator upon which to hang the co-ords and the surface
        SoSeparator        *pconSeparator        = new SoSeparator();
        SoTranslation        *pconTranslation        = new SoTranslation();


//        Start with one coordinate4 and one surface; add or subtract others as necessary
//  Create the co-ordinates... 
    SoCoordinate4        *pconCoordinate4        = new SoCoordinate4;

//  Create the surface 
        SoNurbsSurface        *pconSurface = new SoNurbsSurface;

//  Tie them all together
        pconSeparator->addChild(pconTranslation);
        pconSeparator->addChild(pconCoordinate4);
        pconSeparator->addChild(pconSurface);

        children->append(pconSeparator);
        updateChildren();

//        Connect up to the outside world        
        pconTranslation->translation.connectFrom(&center);


}
void SoPcon::generateAlternateRep(SoAction*) {
        if (children->getLength() == 0) generateChildren();
        updateChildren();
        alternateRep.setValue(( *children)[0]);
}
void SoPcon::clearAlternateRep() {
        alternateRep.setValue(NULL);
}

void SoPcon::updateChildren() {



        assert(children->getLength()==1);
//  Check to see if we actually need to change anything
        if( startAngle.getValue() == startAngleLocal && 
                deltaAngle.getValue() == deltaAngleLocal &&
                center.getValue() == centerLocal &&
                vtxPts == vtxPtsLocal)
                return;

        startAngleLocal = startAngle.getValue();
        deltaAngleLocal = deltaAngle.getValue();
        centerLocal = center.getValue();
        vtxPtsLocal = vtxPts;

        SoSeparator        *pconSeparator = static_cast<SoSeparator *>( ( *children)[0]);
        SoCoordinate4 *pconCoordinate4 = static_cast<SoCoordinate4 *>( pconSeparator->getChild(1));
        SoNurbsSurface *pconSurface = static_cast<SoNurbsSurface *>( pconSeparator->getChild(2));

//
//  We generate a NURB surface to represent a surface of rotation
//        The v dimension represents the rotation. The u represents the cross section.
//
//  Number of control points = DIM.
//        DIM = 7, ORDER = 3 (quadratic)
//        Number of knots = DIM+ORDER=10.
//        It requires 2*nz + 1 to describe a closed polygon:
//        DIM = 2*nz+1, ORDER = 2 (linear)
//  Number of knots = DIM+ORDER = 2*nz+1+2



        assert(vtxPts.getNum()>1);
        int u_dim = vtxPts.getNum();


        pconSurface->numUControlPoints.setValue(u_dim);
    pconSurface->numVControlPoints.setValue(V_DIM);


//  Generate the knot vectors
//  For v, this should be {0,0,0,1,1,2,2,3,3,3}
        int iu, iv, ik, ik_count;
        SoMFFloat u_knot_vals;        
        SoMFFloat v_knot_vals;
//        Do the first point ORDER times        
        for (iv=0, ik=0, ik_count=0; iv <V_ORDER; iv++,ik++){ 
                v_knot_vals.set1Value(ik,(float)ik_count);}
        ik_count++;
//        All but the end point get ORDER-1
        for (iv=0; iv <(V_DIM+V_ORDER-2*V_ORDER)/(V_ORDER-1); iv++,ik_count++){
                for (int j=0; j<(V_ORDER-1);j++,ik++){
                v_knot_vals.set1Value(ik,(float)ik_count);}}
//        Now the last point, ORDER times
        for (iv=0; iv <V_ORDER; iv++,ik++){ 
                v_knot_vals.set1Value(ik,(float)ik_count);}
//        Now for u
        for (iu=0, ik=0, ik_count=0; iu <U_ORDER; iu++,ik++){ 
                u_knot_vals.set1Value(ik,(float)ik_count);}
        ik_count++;
        for (iu=0; iu <(u_dim+U_ORDER-2*U_ORDER)/(U_ORDER-1); iu++,ik_count++){
                for (int j=0; j<(U_ORDER-1);j++,ik++){
                u_knot_vals.set1Value(ik,(float)ik_count);}}
        for (iu=0; iu <U_ORDER; iu++,ik++){ 
                u_knot_vals.set1Value(ik,(float)ik_count);}
//  Should look like {0,0,1,2,3,...,n-1,n,n}


    pconSurface->uKnotVector.setValues(0,u_dim+U_ORDER,u_knot_vals.getValues(0));
    pconSurface->vKnotVector.setValues(0,V_DIM+V_ORDER,v_knot_vals.getValues(0));
        

    //  Set the number of co-ordinates...
    pconCoordinate4->point.setNum(u_dim*V_DIM);

    //  Generate the co-ordinates (Control Points)
    double weight, weightV, angleV;
    SbVec3f cpt;
        SbRotation rotator;

        //  Set up the basic grid of control points 
        float x,y,z;
        int i;
    for (iv=0, angleV=startAngle.getValue()+deltaAngle.getValue(); iv<V_DIM; iv++, angleV -= deltaAngle.getValue()/6) {
                rotator.setValue(SbVec3f(0,0,1),(float)angleV);
                if (iv%2 ==1) {
                        weightV = fabs(cos(deltaAngle.getValue()/6));}
                else {
                        weightV = 1.0;}
       for ( iu=0; iu<u_dim; iu++) {  
        i = iu + iv*u_dim;
                weight = weightV;
                const SbVec3f this_point = vtxPts[iu]; // get the order this way to set exterior of surf 
                this_point.getValue(x,y,z);
                x /= (float)weightV;
                cpt = SbVec3f(x,y,z);
                rotator.multVec(cpt,cpt);
                cpt *= (float)weight;
        pconCoordinate4->point.set1Value(i,cpt[0],cpt[1],cpt[2],(float)weight);
      }
    }

//        Now check to see if we need faces
        int ipart=2*nsurf+1;
        SoVertexProperty *faceProperty;
        SoFaceSet *face1, *face2;
        SoTransform *rot1, *rot2;
        SoShapeHints *hints2;
        if (2*M_PI != deltaAngle.getValue()) {
                if (ends==FALSE) {
                        faceProperty = new SoVertexProperty;
                        face1 = new SoFaceSet;
                        face2 = new SoFaceSet;
                        rot1 = new SoTransform;
                        rot2 = new SoTransform;
                        hints2 = new SoShapeHints;
                        pconSeparator->addChild(faceProperty);
                        pconSeparator->addChild(rot1);
                        pconSeparator->addChild(face1);
                        pconSeparator->addChild(rot2);
                        pconSeparator->addChild(hints2);
                        pconSeparator->addChild(face2);
                        ends = TRUE;
                }
                else {
                        faceProperty = static_cast<SoVertexProperty *>( pconSeparator->getChild(ipart++));
                        rot1 = static_cast<SoTransform *>( pconSeparator->getChild(ipart++));
                        face1 = static_cast<SoFaceSet *>( pconSeparator->getChild(ipart++));
                        rot2 = static_cast<SoTransform *>( pconSeparator->getChild(ipart++));
                        hints2 = static_cast<SoShapeHints *>( pconSeparator->getChild(ipart++));
                        face2 = static_cast<SoFaceSet *>( pconSeparator->getChild(ipart++));
                }
                faceProperty->vertex = vtxPts;
                int nfVert = u_dim - 1;
                face1->numVertices.setValue(nfVert);
                face2->numVertices.setValue(nfVert);
                rot1->rotation.setValue(SbVec3f(0.,0.,1.),startAngle.getValue());
                rot2->rotation.setValue(SbVec3f(0.,0.,1.),deltaAngle.getValue());
                hints2->vertexOrdering.setValue(SoShapeHints::CLOCKWISE);
        }
        else {
                if (ends==TRUE){
                        SoVertexProperty *faceProperty = static_cast<SoVertexProperty *>( pconSeparator->getChild(ipart++));
                        SoTransform *rot1 = static_cast<SoTransform *>( pconSeparator->getChild(ipart++));
                        SoFaceSet *face1 = static_cast<SoFaceSet *>( pconSeparator->getChild(ipart++));
                        SoTransform *rot2 = static_cast<SoTransform *>( pconSeparator->getChild(ipart++));
                        hints2 = static_cast<SoShapeHints *>( pconSeparator->getChild(ipart++));
                        SoFaceSet *face2 = static_cast<SoFaceSet *>( pconSeparator->getChild(ipart++));
                        face2->unref();
                        hints2->unref();
                        rot2->unref();
                        face1->unref();
                        rot1->unref();
                        faceProperty->unref();
                }
        }
}

void SoPcon::computeBBox(SoAction *, SbBox3f &box, SbVec3f &TheCenter ){
// replacement for pure virtual function in SoShape
        float rExtent = 0;
        float zExtentMax = -FLT_MAX, zExtentMin = FLT_MAX;
        const SbVec3f *vtxPtsCopy = vtxPts.getValues(0);
        float x, y, z;
        for (int i=0; i<vtxPts.getNum(); i++, vtxPtsCopy++) {
                vtxPtsCopy->getValue(x,y,z);
                if (x > rExtent) rExtent = x;
                if (z > zExtentMax) zExtentMax = z;
                if (z < zExtentMin) zExtentMin = z;
        }
        box.setBounds(center.getValue()+SbVec3f(-rExtent,-rExtent,zExtentMin),
                          center.getValue()+SbVec3f(rExtent,rExtent,zExtentMax));
        TheCenter = center.getValue();
}
void SoPcon::generatePrimitives(SoAction *action){
//replacement for pure virtual function in SoShape
        if (children->getLength()==0) generateChildren();
        updateChildren();
        children->traverse(action);
}
SoChildList *SoPcon::getChildren() const {
//replacement for SoShape virtual function
        return children;
}
// Use this routine to order SbVec3f's by the z co-ordinate
bool vec3fCompare (const SbVec3f &v1, const SbVec3f &v2){
        float x1,y1,z1, x2,y2,z2;
        v1.getValue(x1,y1,z1);
        v2.getValue(x2,y2,z2);
        return z1>z2;
}
// Use this routine to order SbVec3f's by the reverse of the z co-ordinate
bool vec3fCompareReverse (const SbVec3f &v1, const SbVec3f &v2){
        float x1,y1,z1, x2,y2,z2;
        v1.getValue(x1,y1,z1);
        v2.getValue(x2,y2,z2);
        return z1<z2;
}
void SoPcon::setPconPts(const int nzee, const float , const float , 
                           const float *zvals, const float *rmin, const float *rmax){


        assert(nzee > 1);


//        Sort the incoming points so that the path is counterclockwise viewed
//        along the y axis; Start with upmost inner z and work down, then up
        OrderedPair pts(2*nzee+1);
        int i;
        for (i=0; i < nzee; i++){
                pts[i]=SbVec3f(rmin[i],0,zvals[i]);
                pts[i+nzee]=SbVec3f(rmax[nzee-i-1],0,zvals[nzee-i-1]);}
//Order the points: first rmin, then rmax; then invert the rmax set to get a loop
        OrderedPairIt startDown, endDown, startUp, endUp;
        startDown = pts.begin();
        endDown = startDown+nzee; //can do this with vector iterators
        startUp = endDown;
        endUp = startUp+nzee;
        sort(startDown,endDown,vec3fCompare);
        sort(startUp,endUp,vec3fCompareReverse);
        pts[2*nzee]=pts[0];
//  Stuff the points into the SoMFVec3f
        vtxPts.setNum(2*nzee+1);
        SbVec3f *sbPt = vtxPts.startEditing();
        for (i=0; i< 2*nzee+1; i++, sbPt++){
                *sbPt = pts[i];}
        vtxPts.finishEditing();
}
