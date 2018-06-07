// this :
#include <HEPVis/actions/SoCollectAction.h>

#ifdef __KCC
#define u_short unsigned short
#endif

#include <Inventor/SoPrimitiveVertex.h>
#include <Inventor/nodes/SoShape.h>
#include <Inventor/elements/SoModelMatrixElement.h>
#include <Inventor/elements/SoViewportRegionElement.h>
#include <Inventor/elements/SoViewVolumeElement.h>
#include <Inventor/errors/SoDebugError.h>

#include <HEPVis/nodes/SoViewportRegion.h>

SO_ACTION_SOURCE(SoCollectAction)

void SoCollectAction::initClass(){
  SO_ACTION_INIT_CLASS(SoCollectAction,SoCallbackAction);
  SO_ACTION_ADD_METHOD(SoViewportRegion,viewportRegionAction);
}

SoCollectAction::SoCollectAction(const SbViewportRegion& aViewportRegion)
:fViewportRegion(aViewportRegion)
,fStopNode(FALSE)
,fCurrentNode(0)
{
  SO_ACTION_CONSTRUCTOR(SoCollectAction);

#ifdef __COIN__
  SoCallbackAction::setViewportRegion(aViewportRegion);
#endif

  fSearchViewport[0] = 0;
  fSearchViewport[1] = 0;
  fSearchViewport[2] = 1;
  fSearchViewport[3] = 1;

  addTriangleCallback(SoShape::getClassTypeId(),shapeTriangleCB,NULL);
  addLineSegmentCallback(SoShape::getClassTypeId(),shapeLineSegmentCB,NULL);
  addPointCallback(SoShape::getClassTypeId(),shapePointCB,NULL);  

}

SoCollectAction::~SoCollectAction(){fPaths.truncate(0);}

void SoCollectAction::setSearchArea(int aX,int aY,unsigned int aWidth,unsigned int aHeight){
  fSearchViewport[0] = (float)aX;
  fSearchViewport[1] = (float)aY;
  fSearchViewport[2] = (float)(aWidth ? aWidth : 1);
  fSearchViewport[3] = (float)(aHeight ? aHeight : 1);
}

void SoCollectAction::beginTraversal(SoNode* aNode){
  SoViewportRegionElement::set(getState(),fViewportRegion);
  fPaths.truncate(0);
  SoCallbackAction::beginTraversal(aNode);
}

SoPathList& SoCollectAction::paths(){return fPaths;}

void SoCollectAction::viewportRegionAction(SoAction* aAction,SoNode* aNode){aNode->doAction(aAction);}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

void SoCollectAction::shapeTriangleCB(void*,SoCallbackAction* aAction,const SoPrimitiveVertex* p1,const SoPrimitiveVertex* p2,const SoPrimitiveVertex* p3){
  SoCollectAction* action = (SoCollectAction*)aAction;
  if(action->fStopNode==TRUE) {
    SoNode* node = action->getCurPathTail();
    if(node==action->fCurrentNode) return;
    // New node to examine :
    action->fStopNode = FALSE;
    action->fCurrentNode = 0;
  }
  std::vector<SbVec3f> pts;
  pts.push_back(p1->getPoint());
  pts.push_back(p2->getPoint());
  pts.push_back(p3->getPoint());
  if(action->clip(pts)==TRUE) {
    SoPath* path = action->getCurPath()->copy();
    action->fPaths.append(path);
    // Stop processing this node :
    action->fStopNode = TRUE;
    action->fCurrentNode = action->getCurPathTail();
  }
}

void SoCollectAction::shapeLineSegmentCB(void*,SoCallbackAction* aAction,const SoPrimitiveVertex* p1,const SoPrimitiveVertex* p2){
  SoCollectAction* action = (SoCollectAction*)aAction;
  if(action->fStopNode==TRUE) {
    SoNode* node = action->getCurPathTail();
    if(node==action->fCurrentNode) return;
    // New node to examine :
    action->fStopNode = FALSE;
    action->fCurrentNode = 0;
  }
  std::vector<SbVec3f> pts;
  pts.push_back(p1->getPoint());
  pts.push_back(p2->getPoint());
  if(action->clip(pts)==TRUE) {
    SoPath* path = action->getCurPath()->copy();
    action->fPaths.append(path);
    // Stop processing this node :
    action->fStopNode = TRUE;
    action->fCurrentNode = action->getCurPathTail();
  }
}

void SoCollectAction::shapePointCB(void*,SoCallbackAction* aAction,const SoPrimitiveVertex* p1){
  SoCollectAction* action = (SoCollectAction*)aAction;
  if(action->fStopNode==TRUE) {
    SoNode* node = action->getCurPathTail();
    if(node==action->fCurrentNode) return;
    // New node to examine :
    action->fStopNode = FALSE;
    action->fCurrentNode = 0;
  }
  std::vector<SbVec3f> pts;
  pts.push_back(p1->getPoint());
  if(action->clip(pts)==TRUE) {
    SoPath* path = action->getCurPath()->copy();
    action->fPaths.append(path);
    // Stop processing this node :
    action->fStopNode = TRUE;
    action->fCurrentNode = action->getCurPathTail();
  }
}

SbBool SoCollectAction::clip(const std::vector<SbVec3f>& aPoints){
  const SbViewVolume& vv = SoViewVolumeElement::get(state);
  const SbViewportRegion& vpr = SoViewportRegionElement::get(state);
  const SbMatrix& mat = SoModelMatrixElement::get(state);

  const SbVec2s& vpsize = vpr.getViewportSizePixels();
  const SbVec2s& vporig = vpr.getViewportOriginPixels();

  std::vector<SbVec3f> pts(aPoints.size());

  for(unsigned int index=0;index<aPoints.size();index++) {
    mat.multVecMatrix(aPoints[index],pts[index]);
    vv.projectToScreen(pts[index],pts[index]);
    // Map to window coordinates :
    pts[index][0] *= float(vpsize[0]);
    pts[index][1] *= float(vpsize[1]);
    pts[index][0] += float(vporig[0]);
    pts[index][1] += float(vporig[1]);
    // Map to searching region NDC [-1,1][-1,1]:
    pts[index][0] -= fSearchViewport[0]+fSearchViewport[2]/2;
    pts[index][1] -= fSearchViewport[1]+fSearchViewport[3]/2;
    pts[index][0] /= fSearchViewport[2];
    pts[index][1] /= fSearchViewport[3];
    pts[index][0] *= 2;
    pts[index][1] *= 2;
  }

  if(pts.size()==1) { // Point

    return (clipPoint(pts[0][0],pts[0][1]) == 0L ? TRUE : FALSE);

  } else if(pts.size()==2) { // Line

    return clipLine(pts[0][0],pts[0][1],pts[1][0],pts[1][1]);

  } else if(pts.size()==3) { // Triangle

    if(clipLine(pts[0][0],pts[0][1],pts[1][0],pts[1][1])) return TRUE;
    if(clipLine(pts[1][0],pts[1][1],pts[2][0],pts[2][1])) return TRUE;
    return clipLine(pts[2][0],pts[2][1],pts[0][0],pts[0][1]);

  }

  return FALSE;
}

unsigned long SoCollectAction::clipPoint(float aX,float aY){
  unsigned long out = 0L;
  if(aY>  1.F) out |= (1<<0);
  if(aY< -1.F) out |= (1<<1);
  if(aX>  1.F) out |= (1<<2);
  if(aX< -1.F) out |= (1<<3);
  return out;
}

SbBool SoCollectAction::clipLine(float aBeginX,float aBeginY,float aEndX,float aEndY){
/* 
   begin in, end  in: no toggle. 
    (toggle means begin contains end and end contains begin).
   begin out, end out: no toggle.
   begin out, end  in: no toggle, begin = clipping point.
   begin in, end out: toggled, begin = clipping point.

   At out begin contain clipping point.
   toggle = TRUE -> (begin,end) has been toggled.
*/
  //SbBool toggled = FALSE;
  SbBool accept = FALSE;
  SbBool done = FALSE;
  do {
    unsigned long outbegin = clipPoint(aBeginX,aBeginY);
    unsigned long outend = clipPoint(aEndX,aEndY);
    SbBool reject = ( (outbegin & outend ) !=0  ? TRUE : FALSE);
    if(reject==TRUE) {
      done = TRUE;
    } else {
      accept = ( (outbegin!=0L)||(outend  !=0L)  ? FALSE : TRUE) ;
      if(accept==TRUE) {
        done = TRUE;
      } else {
        if(outbegin==0L) {
          // begin inside. toggle begin and end.
          unsigned long out = outend;
          float x = aEndX;
          float y = aEndY;
          outend   = outbegin;
          aEndX = aBeginX;
          aEndY = aBeginY;
          outbegin = out;
          aBeginX = x;
          aBeginY = y;
          //toggled = TRUE;
        }
        if((outbegin & (1<<0))!=0) {
          // y = 1
          float t = aEndY - aBeginY;
          if(t==0.F) {
            SoDebugError::post("SoCollectAction::clipLine",
                               "try to divide by zero.");
            accept = FALSE;
            break;
          }
          t = (1.F - aBeginY)/t;
          aBeginX += (aEndX - aBeginX) * t;
          aBeginY = 1.F;
        } else if((outbegin & (1<<1))!=0) {
          // y = -1
          float t = aEndY - aBeginY;
          if(t==0.F) {
            SoDebugError::post("SoCollectAction::clipLine",
                               "try to divide by zero.");
            accept = FALSE;
            break;
          }
          t = (-1.F - aBeginY)/t;
          aBeginX += (aEndX - aBeginX) * t;
          aBeginY = -1.F;
        } else if((outbegin & (1<<2))!=0) {
          // x = 1
          float t = aEndX - aBeginX;
          if(t==0.F) {
            SoDebugError::post("SoCollectAction::clipLine",
                               "try to divide by zero.");
            accept = FALSE;
            break;
          }
          t = (1.F - aBeginX)/t;
          aBeginX = 1.F;
          aBeginY += (aEndY - aBeginY) * t;
        } else if((outbegin & (1<<3))!=0) {
          // x = -1
          float t = aEndX - aBeginX;
          if(t==0.F) {
            SoDebugError::post("SoCollectAction::clipLine",
                               "try to divide by zero.");
            accept = FALSE;
            break;
          }
          t = (-1.F - aBeginX)/t;
          aBeginX = -1.F;
          aBeginY += (aEndY - aBeginY) * t;
        }
      }
    }
  } while (!done);
  return accept;
}
