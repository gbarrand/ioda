// this :
#include <HEPVis/actions/SoPainterAction.h>

//#define DEBUG

//#define DEBUG_CUT
//#define DEBUG_COVER


#ifdef __KCC
  #define u_short unsigned short
#endif

#include <Inventor/SoPrimitiveVertex.h>
#include <Inventor/nodes/SoNode.h>
#include <Inventor/nodes/SoShape.h>
#include <Inventor/elements/SoModelMatrixElement.h>
#include <Inventor/elements/SoProjectionMatrixElement.h>
#include <Inventor/elements/SoViewingMatrixElement.h>
#include <Inventor/elements/SoViewportRegionElement.h>
#include <Inventor/elements/SoLinePatternElement.h>
#include <Inventor/elements/SoLazyElement.h>

#include <HEPVis/SbMath.h>
#include <HEPVis/nodes/SoViewportRegion.h>
#include <HEPVis/nodes/SoMarkerSet.h>

#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <time.h>

class SbPainterPrimitive {
public:
  int fType;
  SbPainterContext fContext;
};

#define TRIANGLE 1
#define SIGNUS(aValue) ((aValue)>=0.?1:-1)

class SbPainterTriangle : public SbPainterPrimitive {
public:
  SbPainterTriangle(float*,float*,float*,const SbPainterContext&);
  SbBool inFrontOf(SbPainterTriangle*);
  float getProjectedArea();
  void dump();
private:
  SbBool zProjectionContain(float,float);
  float zOfPoint(float,float); 
public:
  float fXs[3];
  float fYs[3];
  float fZs[3];
  float fXmin,fXmax;
  float fYmin,fYmax;
  float fZmin,fZmax;
  //SbBool swapped;
#ifdef DEBUG_CUT
  SbBool fCut;
#endif
};

class TrianglePair {
public:
  TrianglePair(SbPainterTriangle* aFirst,SbPainterTriangle* aSecond) {
    fFirst = aFirst;
    fSecond = aSecond;
    fCount = 0;
  }
  SbPainterTriangle* fFirst;
  SbPainterTriangle* fSecond;
  int fCount;
};

#ifdef DEBUG_CUT
static SbBool sCut = FALSE;
#endif
//static SbBool sDump = FALSE;

/*
class SbSegment : public SbPrimitive {
public:
  SbTriangle(const SbVec3f& a1,
             const SbVec3f& a2) {
    fType = 2;
    f1 = a1;
    f2 = a2;
    fZ = (f1[3] + f2[2])/3.;
  }
  SbVec3f f1;
  SbVec3f f2;
  float fZ;
};
*/

static void shapeTriangleCB(void*,SoCallbackAction*,
                            const SoPrimitiveVertex*,
                            const SoPrimitiveVertex*,
                            const SoPrimitiveVertex*);
static void shapeLineSegmentCB(void*,SoCallbackAction*,
                               const SoPrimitiveVertex*,
                               const SoPrimitiveVertex*);
static void shapePointCB(void*,SoCallbackAction*,
                         const SoPrimitiveVertex*);
static void ProjectPointToViewport(float,float,float,
                                  const float*,
                                  const int[4],
                                  float*,float*,float*);
static void TransformPoint(float[4],const float[16],const float[4]);

extern "C" {
static int compareZ(const void*,const void*);
}

static TrianglePair* inSwapList(const SbPList&,
                                  SbPainterTriangle*,
                                  SbPainterTriangle*);

SO_ACTION_SOURCE(SoPainterAction)
//////////////////////////////////////////////////////////////////////////////
void SoPainterAction::initClass(
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  SO_ACTION_INIT_CLASS(SoPainterAction,SoCallbackAction);
  
  SO_ACTION_ADD_METHOD(SoViewportRegion,viewportRegionAction);
}
//////////////////////////////////////////////////////////////////////////////
SoPainterAction::SoPainterAction(
 const SbViewportRegion& aViewportRegion
,SbPainter* aPainter
,SbBool aSort
)
:fCollect(FALSE)
,fNumber(0)
,fList(NULL)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  SO_ACTION_CONSTRUCTOR(SoPainterAction);

#ifdef __COIN__
  SoCallbackAction::setViewportRegion(aViewportRegion);
#endif

  fViewportRegion = aViewportRegion;
  fPainter = aPainter;
  fSort = aSort;
  fBackgroundColor.setValue(0.5F,0.5F,0.5F);
  
  addTriangleCallback(SoShape::getClassTypeId(),shapeTriangleCB,NULL);
  addLineSegmentCallback(SoShape::getClassTypeId(),shapeLineSegmentCB,NULL);
  addPointCallback(SoShape::getClassTypeId(),shapePointCB,NULL);  
}
//////////////////////////////////////////////////////////////////////////////
void SoPainterAction::setPainter(
 SbPainter* aPainter
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  fPainter = aPainter;
}
//////////////////////////////////////////////////////////////////////////////
void SoPainterAction::setViewportRegion(
 const SbViewportRegion& aNewRegion
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  fViewportRegion = aNewRegion;
}
//////////////////////////////////////////////////////////////////////////////
const SbViewportRegion& SoPainterAction::getViewportRegion(
) const
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  return fViewportRegion;
}
//////////////////////////////////////////////////////////////////////////////
void SoPainterAction::setBackgroundColor(
 const SbColor& aColor
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  fBackgroundColor = aColor;
}
//////////////////////////////////////////////////////////////////////////////
void SoPainterAction::enableSort(
 SbBool aValue
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  fSort = aValue;
}
//////////////////////////////////////////////////////////////////////////////
void SoPainterAction::beginTraversal(
 SoNode* aNode
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  if(fPainter==NULL) return;

  SoViewportRegionElement::set(getState(),fViewportRegion);

  const SbVec2s& win = fViewportRegion.getWindowSize();
  //const SbVec2s& siz = fViewportRegion.getViewportSizePixels();

  int width = win[0];
  int height = win[1];
  if(width<=0) width  = 100;
  if(height<=0) height = 100;

  fPainter->setWindowSize(width,height);

  fPainter->beginTraversal();

  float red = fBackgroundColor[0];
  float green = fBackgroundColor[1];
  float blue = fBackgroundColor[2];
  fPainter->clearColorBuffer(red,green,blue);

  //printf("debug : SoPainterAction::beginTraversal %d %d \n",win[0],win[1]);
  if(fSort==TRUE) {
    // First pass, collect primitves :
    fCollect = TRUE;    
    fNumber = 0;
    fList = NULL;
    SoCallbackAction::beginTraversal(aNode);
    // Sort :
    //printf("debug : sort : %d\n",number);
#ifdef DEBUG_CUT
    sCut = FALSE;
#endif
    sortPrimitives();
    // Second  pass, render primitves :
    fPainter->enableEdges(TRUE);
    SbPainterPrimitive* prim;
    for(int count=0;count<fNumber;count++) {
      prim = (SbPainterPrimitive*)(fList[count]);
      if(prim->fType==TRIANGLE) {
#ifdef DEBUG_CUT
        if(sCut==TRUE) {
          if(((SbPainterTriangle*)prim)->fCut==TRUE) {
            prim->fContext.fRed = 1.;
            prim->fContext.fGreen = 1.;
            prim->fContext.fBlue = 0.;
            //printf("debug : draw \n");
            //((SbPainterTriangle*)prim)->dump();
#endif
            fPainter->drawPrimitive(SbPrimitivePolygon,3,
                                   ((SbPainterTriangle*)prim)->fXs,
                                   ((SbPainterTriangle*)prim)->fYs,
                                   ((SbPainterTriangle*)prim)->fZs,
                                   prim->fContext);
#ifdef DEBUG_CUT
          }
        } else {
          fPainter->drawPrimitive(SbPrimitivePolygon,3,
                                 ((SbPainterTriangle*)prim)->fXs,
                                 ((SbPainterTriangle*)prim)->fYs,
                                 ((SbPainterTriangle*)prim)->fZs,
                                 prim->fContext);
        }
#endif

        delete (SbPainterTriangle*)prim;
      }
    }
    if(fList!=NULL) free(fList);
    fNumber = 0;
    fList = NULL;
  } else {
    fCollect = FALSE;    
    fPainter->enableEdges(FALSE);
    SoCallbackAction::beginTraversal(aNode);
  }

  fPainter->endTraversal();
}
//////////////////////////////////////////////////////////////////////////////
void SoPainterAction::viewportRegionAction(
 SoAction* aAction
,SoNode* aNode
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  if(aNode->isOfType(SoViewportRegion::getClassTypeId())) {
    SbColor c = ((SoViewportRegion*)aNode)->backgroundColor.getValue();
    ((SoPainterAction*)aAction)->fPainter->clearColorBuffer(c[0],c[1],c[2]);
  }
  aNode->doAction(aAction);
}
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void shapeTriangleCB (
 void*
,SoCallbackAction* aAction
,const SoPrimitiveVertex* p1
,const SoPrimitiveVertex* p2
,const SoPrimitiveVertex* p3
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  float xs[3];
  float ys[3];
  float zs[3];

 {const SbVec3f& v = p1->getPoint();
  xs[0] = v[0];
  ys[0] = v[1];
  zs[0] = v[2];}

 {const SbVec3f& v = p2->getPoint();
  xs[1] = v[0];
  ys[1] = v[1];
  zs[1] = v[2];}

 {const SbVec3f& v = p3->getPoint();
  xs[2] = v[0];
  ys[2] = v[1];
  zs[2] = v[2];}

  ((SoPainterAction*)aAction)->drawPrimitive(SbPrimitivePolygon,3,xs,ys,zs);
}
//////////////////////////////////////////////////////////////////////////////
void shapeLineSegmentCB (
 void*
,SoCallbackAction* aAction
,const SoPrimitiveVertex* p1
,const SoPrimitiveVertex* p2
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  float xs[2];
  float ys[2];
  float zs[2];

 {const SbVec3f& v = p1->getPoint();
  xs[0] = v[0];
  ys[0] = v[1];
  zs[0] = v[2];}

 {const SbVec3f& v = p2->getPoint();
  xs[1] = v[0];
  ys[1] = v[1];
  zs[1] = v[2];}

  ((SoPainterAction*)aAction)->drawPrimitive(SbPrimitiveLineStrip,2,xs,ys,zs);
}
//////////////////////////////////////////////////////////////////////////////
void shapePointCB (
 void*
,SoCallbackAction* aAction
,const SoPrimitiveVertex* p1
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  float xs[1];
  float ys[1];
  float zs[1];

 {const SbVec3f& v = p1->getPoint();
  xs[0] = v[0];
  ys[0] = v[1];
  zs[0] = v[2];}

  ((SoPainterAction*)aAction)->drawPrimitive(SbPrimitivePoints,1,xs,ys,zs);
}
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void SoPainterAction::drawPrimitive(
 SbPrimitiveType aType
,int aPointn
,float* aXs
,float* aYs
,float* aZs
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  if(fPainter==NULL) return;
  if(aPointn<=0) return;
  // Get elements :
  SbMatrix modelMatrix = SoModelMatrixElement::get(getState());
  SbMatrix projectionMatrix = SoProjectionMatrixElement::get(getState());
  SbMatrix viewMatrix = SoViewingMatrixElement::get(getState());
  unsigned short linePattern = 
    (unsigned short)SoLinePatternElement::get(getState());
  const SbViewportRegion& vpr = SoViewportRegionElement::get(getState());
  const short* size = vpr.getViewportSizePixels().getValue ();
  const short* orig = vpr.getViewportOriginPixels().getValue ();
  const SbColor& color = SoLazyElement::getDiffuse(getState(),0);

  //Yes, multRight and not multLeft.
  modelMatrix.multRight(viewMatrix); //Should be before the below.
  modelMatrix.multRight(projectionMatrix);

  const float* matrix = (float*)modelMatrix;

  const float* rgba = color.getValue();
  
  int viewport[4];
  viewport[0] = orig[0];
  viewport[1] = orig[1];
  viewport[2] = size[0];
  viewport[3] = size[1];
  
  float* xs = new float[aPointn];
  float* ys = new float[aPointn];
  float* zs = new float[aPointn];
  for(int count=0;count<aPointn;count++) {
    ProjectPointToViewport(*(aXs+count),*(aYs+count),*(aZs+count),
                           matrix,viewport,
                           xs+count,ys+count,zs+count);
  }
  SbPainterContext context;
  context.fRed = rgba[0];
  context.fGreen = rgba[1];
  context.fBlue = rgba[2];
  context.fLinePattern = linePattern;
  context.fLineWidth = 1;
  context.fAreaStyle = SbAreaSolid;
  context.fMarkerStyle = SbMarkerDot;
  context.fMarkerSize = 1;

  SoNode* node = getCurPathTail();
  if(node && (node->isOfType(HEPVis_SoMarkerSet::getClassTypeId())==TRUE) ) {
    HEPVis_SoMarkerSet::getMarkerStyleSize(
              ((HEPVis_SoMarkerSet*)node)->markerIndex[0],
              context.fMarkerStyle,
              context.fMarkerSize);
  }

  if(fCollect==TRUE) {
    accumulatePrimitive(aType,aPointn,xs,ys,zs,context);
  } else {
    fPainter->drawPrimitive(aType,aPointn,xs,ys,zs,context);
  }

  delete [] xs;
  delete [] ys;
  delete [] zs;
}
//////////////////////////////////////////////////////////////////////////////
void SoPainterAction::accumulatePrimitive(
 SbPrimitiveType aType
,int //aPointn
,float* aXs
,float* aYs
,float* aZs
,const SbPainterContext& aAtb
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  switch(aType) {
  case SbPrimitivePoints:
    break;
  case SbPrimitiveLineStrip:
  case SbPrimitiveLineLoop:
    break;
  case SbPrimitivePolygon:{
    fList = (void**)(fNumber==0 ? 
                    malloc(sizeof(void*)) : 
                    realloc(fList,(fNumber+1)*sizeof(void*)));
    if(fList==NULL) return;
    SbPainterTriangle* triangle = new SbPainterTriangle(aXs,aYs,aZs,aAtb);
    if(triangle->getProjectedArea()>=1.) { 
      // Take only visible triangles (more than one pixel).
      //printf("debug : projected area : %g\n",triangle->getProjectedArea());
      fList[fNumber] = (void*)triangle;
      fNumber++;
    }
  }break;
  case SbPrimitiveLines:
   break;
  }
}
//////////////////////////////////////////////////////////////////////////////
void SoPainterAction::sortPrimitives(
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  if(fList==NULL) return;
  if(fNumber<=0) return;

  //printf("debug : sort : begin : %d\n",fNumber);

  qsort(fList,fNumber,sizeof(void*),compareZ);

  SbPainterTriangle* current;
  SbPainterTriangle* against;
  int i,j;

  /*
  for(i=0;i<fNumber;i++) {
    current = (SbPainterTriangle*)fList[i];
    for(j=i+1;j<fNumber;j++) {
      against = (SbPainterTriangle*)fList[j];
      if( (current->inFrontOf(against)==TRUE) && 
          (against->inFrontOf(current)==TRUE) ){
        printf("Can't decide triangle order : %lu %lu.\n",
               current,against);
        printf("debug : ------ current\n");
        current->dump();
        printf("debug : ------ against\n");
        against->dump();
      }
    }
  */

  SbPList swapList;
  TrianglePair* pair;
  SbBool stop = FALSE;

  for(i=0;i<fNumber;i++) {
    current = (SbPainterTriangle*)fList[i];
    //printf("debug : %d\n",i);
    for(j=i+1;j<fNumber;j++) {
      //printf("debug : --- %d\n",j);
      against = (SbPainterTriangle*)fList[j];
      /*
      if(against->fZmin > current->fZmax) {
        //printf("debug : ------ break\n");
        break;
      }
      */
      // z overlap :
#ifdef DEBUG_COVER
      //printf("debug : front of ?\n");
#endif
      if(current->inFrontOf(against)==TRUE) {
        if(against->inFrontOf(current)==TRUE) {
          //printf("Can't decide triangle order : %lu %lu.\n",current,against);
          printf("debug : ------ current\n");
          current->dump();
          printf("debug : ------ against\n");
          against->dump();
          against->inFrontOf(current);
#ifdef DEBUG_CUT
          if(sCut==FALSE) {
            current->fCut = TRUE;
            against->fCut = TRUE;
            sCut = TRUE;
            return;
          }
#endif
        } else { // Swap triangles :
#ifdef DEBUG_CUT
          /*
          printf("debug : ------ swap : %lu %lu\n",current,against);
          printf("debug : ------ current\n");
          current->dump();
          printf("debug : ------ against\n");
          against->dump();
          */
#endif

          if((pair = inSwapList(swapList,current,against))==NULL) {
            swapList.append(pair = new TrianglePair(current,against));
          }
          pair->fCount++;
          if(pair->fCount>=500) {
            printf("Infinite loop ?\n");
            stop = TRUE;
            break;;
          }
          fList[i] = against;
          fList[j] = current;
          // Start over j's again :
          i--;
          break;
        }
      }
    } 
    if(stop==TRUE) break;
  }

  /*
  if(stop==TRUE) {
    int number = swapList.getLength();
    printf("debug : stopped : %d\n",number);
    if(sDump==FALSE) {
      sDump = TRUE;
      for(int count=0;count<number;count++) {
        pair = (TrianglePair*)swapList[count];
        printf("debug : -- %d\n",pair->fCount);
      }
    }
    for(int count=0;count<number;count++) {
      pair = (TrianglePair*)swapList[count];
      if(pair->fCount>500) {
        printf("debug : %d\n",pair->fCount);
#ifdef DEBUG_CUT
        pair->fFirst->fCut = TRUE;
        pair->fSecond->fCut = TRUE;
        sCut = TRUE;
#endif
      }
    }
  }
  */

  int number = swapList.getLength();
  for(int count=0;count<number;count++) {
    pair = (TrianglePair*)swapList[count];
    delete pair;
  }

  //printf("debug : sort : end : swap list size %d\n",number);
}
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void ProjectPointToViewport (
 float objx
,float objy
,float objz
,const float* matrix
,const int viewport[4]
,float* winx
,float* winy
,float* winz
)
//////////////////////////////////////////////////////////////////////////////
// From Mesa/src-glu/glu.c/gluProject.
/* Projection du point (objx,objy,objz) sur l'ecran (winx,winy,winz).      */
/* Multiply *winz by -1 to have correct z order between 0 and 1.           */
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  float in[4];
  in[0] = objx; 
  in[1] = objy; 
  in[2] = objz; 
  in[3] = 1.0;

  float out[4];
  TransformPoint(out,matrix,in);

  // Normalize between -1 and 1 :
  out[0] /= out[3];
  out[1] /= out[3];
  out[2] /= out[3];
  
  // In screen coordinates :
  *winx = viewport[0] + (1 + out[0]) * viewport[2]/2;
  *winy = viewport[1] + (1 + out[1]) * viewport[3]/2;
  /* Between 0 and 1 in z */
  //Mesa : *winz = (1+out[2])/2;
  *winz = - (1+out[2])/2; //GB
  //*winz = out[2];
}
//////////////////////////////////////////////////////////////////////////////
void TransformPoint (
 float out[4]
,const float m[16]
,const float in[4] 
)
//////////////////////////////////////////////////////////////////////////////
// From Mesa/src-glu/glu.c/TransformPoint.
/*
 * Transform a point (column vector) by a 4x4 matrix.  I.e.  out = m * in
 * Input:  m - the 4x4 matrix
 *         in - the 4x1 vector
 * Output:  out - the resulting 4x1 vector.
 */
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
#define M(row,col)  m[col*4+row]
  out[0] = M(0,0) * in[0] + M(0,1) * in[1] + M(0,2) * in[2] + M(0,3) * in[3];
  out[1] = M(1,0) * in[0] + M(1,1) * in[1] + M(1,2) * in[2] + M(1,3) * in[3];
  out[2] = M(2,0) * in[0] + M(2,1) * in[1] + M(2,2) * in[2] + M(2,3) * in[3];
  out[3] = M(3,0) * in[0] + M(3,1) * in[1] + M(3,2) * in[2] + M(3,3) * in[3];
#undef M
}
//////////////////////////////////////////////////////////////////////////////
int compareZ(
 const void* a1,
 const void* a2
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  // Assume that triangles do not cut each other :
  SbPainterTriangle* p1 = *((SbPainterTriangle**)a1);
  SbPainterTriangle* p2 = *((SbPainterTriangle**)a2);

  if(p1->fZmin>p2->fZmin) return 1;
  if(p1->fZmin<p2->fZmin) return -1;

  return 0;
}
//////////////////////////////////////////////////////////////////////////////
TrianglePair* inSwapList(
 const SbPList& aList
,SbPainterTriangle* aFirst                 
,SbPainterTriangle* aSecond
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  int number = aList.getLength();
  TrianglePair* pair;
  for(int count=0;count<number;count++) {
    pair = (TrianglePair*)aList[count];
    if((aFirst==pair->fFirst) && (aSecond==pair->fSecond)) 
      return pair;
  }
  return NULL;
}
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
SbPainterTriangle::SbPainterTriangle(
 float* aXs
,float* aYs
,float* aZs
,const SbPainterContext& aContext
) 
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  fType = TRIANGLE;
  fXs[0] = aXs[0];fXs[1] = aXs[1];fXs[2] = aXs[2];
  fYs[0] = aYs[0];fYs[1] = aYs[1];fYs[2] = aYs[2];
  fZs[0] = aZs[0];fZs[1] = aZs[1];fZs[2] = aZs[2];
  fContext = aContext;
  fXmin = SbMinimum(SbMinimum(fXs[0],fXs[1]),fXs[2]);
  fXmax = SbMaximum(SbMaximum(fXs[0],fXs[1]),fXs[2]);
  fYmin = SbMinimum(SbMinimum(fYs[0],fYs[1]),fYs[2]);
  fYmax = SbMaximum(SbMaximum(fYs[0],fYs[1]),fYs[2]);
  fZmin = SbMinimum(SbMinimum(fZs[0],fZs[1]),fZs[2]);
  fZmax = SbMaximum(SbMaximum(fZs[0],fZs[1]),fZs[2]);
#ifdef DEBUG_CUT
  fCut = FALSE;
#endif
}
//////////////////////////////////////////////////////////////////////////////
float SbPainterTriangle::getProjectedArea(
) 
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  return  
   FFABS(((fXs[1]-fXs[0])*(fYs[2]-fYs[1])-(fYs[1]-fYs[0])*(fXs[2]-fXs[1]))/2.);
}
//////////////////////////////////////////////////////////////////////////////
SbBool SbPainterTriangle::zProjectionContain(
 float aX
,float aY
) 
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  // To optimize do computation directly (do not use SbVec3f).
  // v01 ^ v20 :
  int sz = 
    SIGNUS((fXs[1]-fXs[0])*(fYs[0]-fYs[2])-(fYs[1]-fYs[0])*(fXs[0]-fXs[2]));
  //
  // v01 ^ v0P :
  float z01_P = 
    (fXs[1] - fXs[0])*(aY-fYs[0])-(fYs[1]-fYs[0])*(aX - fXs[0]);
  if( z01_P == 0. ) return FALSE; // On edge.
  if( sz == SIGNUS(z01_P) ) return FALSE;
  //
  // v12 ^ v1P :
  float z12_P = 
    (fXs[2] - fXs[1])*(aY-fYs[1])-(fYs[2]-fYs[1])*(aX - fXs[1]);
  if( z12_P == 0. ) return FALSE; // On edge.
  if( sz == SIGNUS(z12_P) ) return FALSE;
  //
  // v20 ^ v2P :
  float z20_P = 
    (fXs[0] - fXs[2])*(aY-fYs[2])-(fYs[0]-fYs[2])*(aX - fXs[2]);
  if( z20_P == 0. ) return FALSE; // On edge.
  if( sz == SIGNUS(z20_P) ) return FALSE;
  //
  return TRUE;
}
//////////////////////////////////////////////////////////////////////////////
float SbPainterTriangle::zOfPoint(
 float aX
,float aY
) 
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  float v01_cross_v12_x = 
    (fYs[1]-fYs[0])*(fZs[2]-fZs[1])-(fZs[1]-fZs[0])*(fYs[2]-fYs[1]);
  float v01_cross_v12_y = 
    (fZs[1]-fZs[0])*(fXs[2]-fXs[1])-(fXs[1]-fXs[0])*(fZs[2]-fZs[1]);
  float v01_cross_v12_z = 
    (fXs[1]-fXs[0])*(fYs[2]-fYs[1])-(fYs[1]-fYs[0])*(fXs[2]-fXs[1]);
  return ((  v01_cross_v12_z * fZs[0]
             - v01_cross_v12_x * (aX - fXs[0])
             - v01_cross_v12_y * (aY - fYs[0]))/v01_cross_v12_z);
}
//////////////////////////////////////////////////////////////////////////////
void SbPainterTriangle::dump(
) 
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  printf("0 : %g %g %g\n",fXs[0],fYs[0],fZs[0]);
  printf("1 : %g %g %g\n",fXs[1],fYs[1],fZs[1]);
  printf("2 : %g %g %g\n",fXs[2],fYs[2],fZs[2]);
}
//////////////////////////////////////////////////////////////////////////////
SbBool SbPainterTriangle::inFrontOf(
 SbPainterTriangle* aTriangle
)
//////////////////////////////////////////////////////////////////////////////
// Assume that triangles do not cut each other :
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
#ifdef DEBUG_COVER
  //printf("debug : inFrontOf : start\n");
#endif
  if( (fXmin>=aTriangle->fXmax) ||
      (aTriangle->fXmin>=fXmax) ||
      (fYmin>=aTriangle->fYmax) ||
      (aTriangle->fYmin>=fYmax) ){
#ifdef DEBUG_COVER
    //printf("debug : inFrontOf : no xy overlap\n");
#endif
    return FALSE;
  }

  // Overlap in x, y. 
  // Which one is in front of the other ?

  if(fZmin > aTriangle->fZmax) return TRUE;
  if(aTriangle->fZmin > fZmax) return FALSE;

  // If flat in same plane !
  SbBool flat1 = (fZmin == fZmax ? TRUE : FALSE);
  SbBool flat2 = (aTriangle->fZmin == aTriangle->fZmax ? TRUE : FALSE);
  if( (flat1==TRUE) && (flat2==TRUE) ) {
    if(fZmin==aTriangle->fZmin)
      return FALSE;  // Flat and in same plane.
  }

#ifdef DEBUG_COVER
  //printf("debug : inFrontOf : check projection intersections\n");
#endif

  // Use double to have higher precision :
  double a1x,a1y,a1z,b1x,b1y,b1z;
  double a2x,a2y,a2z,b2x,b2y,b2z;
  double delta,s1,s2,z1,z2;
  int i,j;
  static int k[3] = {1,2,0};
  SbBool cutWithSameZ = FALSE;
  for(i=0;i<3;i++) {
    a1x = fXs[i]; 
    a1y = fYs[i]; 
    a1z = fZs[i];
    b1x = fXs[k[i]]; 
    b1y = fYs[k[i]]; 
    b1z = fZs[k[i]];
    for(j=0;j<3;j++) {
      a2x = aTriangle->fXs[j]; 
      a2y = aTriangle->fYs[j]; 
      a2z = aTriangle->fZs[j];
      b2x = aTriangle->fXs[k[j]]; 
      b2y = aTriangle->fYs[k[j]]; 
      b2z = aTriangle->fZs[k[j]];

      // Test equality of points because the below intersection
      // computation failed sometime due to precision :
      if( 
         ( (a1x==a2x) && (a1y==a2y) && (a1z==a2z) ) || 
         ( (a1x==b2x) && (a1y==b2y) && (a1z==b2z) ) ||
         ( (b1x==a2x) && (b1y==a2y) && (b1z==a2z) ) || 
         ( (b1x==b2x) && (b1y==b2y) && (b1z==b2z) )
         ) { 
        break;
      }

      // Get projected intersection of both segments,
      // compare z of intersection for two triangles :
      //   x = (b1x - a1x) * s1 + a1x;  s1 in [0,1]
      //   y = (b1y - a1y) * s1 + a1y;  s1 in [0,1]

      //   x = (b2x - a2x) * s2 + a2x;  s2 in [0,1]
      //   y = (b2y - a2y) * s2 + a2y;  s2 in [0,1]

      //   (b1x - a1x) * s1 + a1x = (b2x - a2x) * s2 + a2x
      //   (b1y - a1y) * s1 + a1y = (b2y - a2y) * s2 + a2y

      //   (b1x - a1x) * s1 - (b2x - a2x) * s2 = a2x - a1x
      //   (b1y - a1y) * s1 - (b2y - a2y) * s2 = a2y - a1y

      delta = - (b1x - a1x) * (b2y - a2y) + (b1y - a1y) * (b2x - a2x);
      //printf("debug : determinant %g\n",delta);
      if(delta==0.) {
        //debug printf("null determinant\n");
      } else {
        s1 = - (a2x - a1x) * (b2y - a2y) + (a2y - a1y) * (b2x - a2x);
        s2 =   (a2y - a1y) * (b1x - a1x) - (a2x - a1x) * (b1y - a1y);
        s1 /= delta;
        s2 /= delta;

        //  Stop when finding two edges that cut each other ;
        // ie s1 and s2 in ]0,1[
        if(  ((s1>0.)&&(s1<1.)) && ((s2>0.)&&(s2<1.))  ) {
          //  We have a (x,y) point that belong to both triangles,
          // compute z for both triangles :

#define ZERO(aValue) (fabs(aValue)<1.e-3)
          if(  
             ( (ZERO(s1)) && (ZERO(s2)) ) ||
             ( (ZERO(s1)) && (ZERO(1.-s2)) ) ||
             ( (ZERO(1.-s1)) && (ZERO(s2)) ) ||
             ( (ZERO(1.-s1)) && (ZERO(1.-s2)) ) ) {
            break;
          }


#ifdef DEBUG_COVER
          /*
          double x1 = (b1x-a1x) * s1 + a1x;
          double y1 = (b1y-a1y) * s1 + a1y;
          double x2 = (b2x-a2x) * s2 + a2x;
          double y2 = (b2y-a2y) * s2 + a2y;
          */
#endif

          z1 = (b1z - a1z) * s1 + a1z;
          z2 = (b2z - a2z) * s2 + a2z;
          if(z1==z2) {
            /*
            printf("debug : same z, continue : %g %g :%lu %lu : %d %d : %g %g : (%g %g) (%g %g) (%g %g)\n",
                   z1,z2,this,aTriangle,i,j,s1,s2,
                   a1x,a2x,a1y,a2y,a1z,a2z);
                   */
            cutWithSameZ = TRUE;
          } else if(z1>z2) {
#ifdef DEBUG_COVER
            printf("debug : front : %lu %lu : %d %d : %g %g : %g %g\n",
                   this,aTriangle,i,j,s1,s2,z1,z2);
            /*
            printf("debug : front : %d %d : %g %g : %g %g\n",i,j,z1,z2,s1,s2);
            printf("debug : -- %g %g : %g %g\n",a1x,a1y,b1x,b1y);
            printf("debug : -- %g %g : %g %g\n",a2x,a2y,b2x,b2y);
            */
#endif
            return TRUE;
          } else if(z1<z2) {
#ifdef DEBUG_COVER
            printf("debug : back : %lu %lu : %d %d : %g %g : %g %g\n",
                   this,aTriangle,i,j,s1,s2,z1,z2);
#endif
            return FALSE;
          }
        }
      }
    }
  }

  if(cutWithSameZ==TRUE) {
    //printf("Triangles cut with same Z (%lu %lu) :\n",this,aTriangle);
    dump();
    printf("to :\n");
    aTriangle->dump();
    return FALSE;
  }
  // Could cut but with same z for all cut points !

#ifdef DEBUG_COVER
  //printf("debug : inFrontOf : no intersection\n");
#endif

  // The projection of one could be contained in the other :

  SbBool c1[3];
  c1[0] = zProjectionContain(aTriangle->fXs[0],aTriangle->fYs[0]);
  c1[1] = zProjectionContain(aTriangle->fXs[1],aTriangle->fYs[1]);
  c1[2] = zProjectionContain(aTriangle->fXs[2],aTriangle->fYs[2]);

  SbBool c2[3];
  c2[0] = aTriangle->zProjectionContain(fXs[0],fYs[0]);
  c2[1] = aTriangle->zProjectionContain(fXs[1],fYs[1]);
  c2[2] = aTriangle->zProjectionContain(fXs[2],fYs[2]);

  if( ( (c1[0]==FALSE) && (c1[1]==FALSE) && (c1[2]==FALSE) ) ||
      ( (c2[0]==FALSE) && (c2[1]==FALSE) && (c2[2]==FALSE) ) ) {
    // No overlap :
#ifdef DEBUG_COVER
    printf("debug : inFrontOf : %lu %lu : no overlap\n",this,aTriangle);
#endif
    return FALSE;
  }

  // Projection of one in the other :
  
  for(i=0;i<3;i++) {
    if(c1[i]==TRUE) {
      // aTriangle in 'this' :
      z1 = zOfPoint(aTriangle->fXs[i],aTriangle->fYs[i]); 
      z2 = aTriangle->zOfPoint(aTriangle->fXs[i],aTriangle->fYs[i]); 
      if(z1>z2) {
#ifdef DEBUG_COVER
        printf("debug : inFrontOf : %lu %lu : one in the other : front1 %d : %d %d %d\n",this,aTriangle,i,c1[0],c1[1],c1[2]);
#endif
        return TRUE;
      } else if(z1<z2) {
#ifdef DEBUG_COVER
        printf("debug : inFrontOf : %lu %lu : one in the other : back1 %d : %d %d %d\n",this,aTriangle,i,c1[0],c1[1],c1[2]);
#endif
        return FALSE;
      }
    }
  }

  for(i=0;i<3;i++) {
    if(c2[i]==TRUE) {
      // 'this' in aTriangle :
      z1 = zOfPoint(fXs[i],fYs[i]); 
      z2 = aTriangle->zOfPoint(fXs[i],fYs[i]); 
      if(z1>z2) {
#ifdef DEBUG_COVER
        printf("debug : inFrontOf : %lu %lu : one in the other : front2 %d : %d %d %d\n",this,aTriangle,i,c2[0],c2[1],c2[2]);
#endif
        return TRUE;
      } else if(z1<z2) {
#ifdef DEBUG_COVER
        printf("debug : inFrontOf : %lu %lu : one in the other : back2 %d : %d %d %d\n",this,aTriangle,i,c2[0],c2[1],c2[2]);
#endif
        return FALSE;
      }
    }
  }

  printf("Can't compare triangles (cut each other ? ) :\n");
  dump();
  printf("to :\n");
  aTriangle->dump();

  return FALSE;
}
