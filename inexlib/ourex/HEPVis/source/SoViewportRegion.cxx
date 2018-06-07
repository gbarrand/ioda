// this :
#include <HEPVis/nodes/SoViewportRegion.h>

#include <Inventor/SbViewportRegion.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/errors/SoDebugError.h>
#include <Inventor/actions/SoGLRenderAction.h>
#include <Inventor/actions/SoRayPickAction.h>
#include <Inventor/elements/SoViewportRegionElement.h>
#include <Inventor/elements/SoViewVolumeElement.h>
#include <Inventor/elements/SoModelMatrixElement.h>
#include <Inventor/events/SoMouseButtonEvent.h>

#include <HEPVis/SbGL.h>
#include <HEPVis/SbMath.h>
#include <HEPVis/actions/SoGL2PSAction.h>
#include <HEPVis/actions/SoAlternateRepAction.h>

#include <stdlib.h>

static SbBool withinWindow(int,int,int&,int&,int&,int&); 
static SbBool inBorder(int,int,int,int,int,int,int,int);
static int inCorner(int,int,int,int,int,int,int,int);

SO_NODE_SOURCE(SoViewportRegion)

void SoViewportRegion::initClass() {
  SO_NODE_INIT_CLASS(SoViewportRegion,SoNode,"Node");
}

SoViewportRegion::SoViewportRegion()
:fIsDragging(FALSE)
,fDragCorner(0)
,fShow(FALSE)
,fShowOldVisible(TRUE)
{
  SO_NODE_CONSTRUCTOR(SoViewportRegion);

  SO_NODE_ADD_FIELD(visible,(TRUE));
  // Percentage in window size :
  SO_NODE_ADD_FIELD(originUnit,(PERCENT));
  SO_NODE_ADD_FIELD(sizeUnit,(PERCENT));
  // The origin is the lower left corner ; including the border.
  SO_NODE_ADD_FIELD(origin,(SbVec2f(0,0)));
  SO_NODE_ADD_FIELD(size,(SbVec2f(1,1)));
  SO_NODE_ADD_FIELD(horizontalBorder,(1));
  SO_NODE_ADD_FIELD(verticalBorder,(1));
  // If background is TRUE, handle borders by drawing
  // a larger fill area before the effective inside 
  // viewport area. 
  // If FALSE, borders a drawn with 
  // four rectangular surrounding the inside viewport area
  // and then the background looks like transparent.
  SO_NODE_ADD_FIELD(background,(TRUE));
  SO_NODE_ADD_FIELD(backgroundColor,(SbColor(1,1,1)));
  SO_NODE_ADD_FIELD(borderColor,(SbColor(0,0,0)));
  SO_NODE_ADD_FIELD(highlighted,(FALSE));
  SO_NODE_ADD_FIELD(highlightColor,(SbColor(1,0,0)));
  SO_NODE_ADD_FIELD(horizontalHighlightBorder,(2));
  SO_NODE_ADD_FIELD(verticalHighlightBorder,(2));
  SO_NODE_ADD_FIELD(horizontalShadow,(0));
  SO_NODE_ADD_FIELD(verticalShadow,(0));
  SO_NODE_ADD_FIELD(sensitiveBorder,(TRUE));
  SO_NODE_ADD_FIELD(horizontalOriginOffset,(0));
  SO_NODE_ADD_FIELD(verticalOriginOffset,(0));
  SO_NODE_ADD_FIELD(showHighlight,(TRUE));

  SO_NODE_ADD_FIELD(alternateRep,(NULL));

  SO_NODE_DEFINE_ENUM_VALUE(Unit,PERCENT);
  SO_NODE_DEFINE_ENUM_VALUE(Unit,PIXELS);

  SO_NODE_SET_SF_ENUM_TYPE(originUnit,Unit);
  SO_NODE_SET_SF_ENUM_TYPE(sizeUnit,Unit);

  //SO_NODE_ADD_FIELD(renderSizePercent,(SbVec2f(0,0)));
  //SO_NODE_ADD_FIELD(renderSizePixels,(SbVec2s(0,0)));
  //SO_NODE_ADD_FIELD(renderOriginPercent,(SbVec2f(0,0)));
  //SO_NODE_ADD_FIELD(renderOriginPixels,(SbVec2s(0,0)));
  //FIXME : needed ?
  //renderSizePercent.enableNotify(FALSE);
  //renderSizePixels.enableNotify(FALSE);
  //renderOriginPercent.enableNotify(FALSE);
  //renderOriginPixels.enableNotify(FALSE);

  fShowOldHighlightBorder.setValue(0,0);
  fShowOldBorder.setValue(0,0);

  fDragPos.setValue(0,0);
  fDragOldOrigin.setValue(0,0);
  fDragOldSize.setValue(0,0);
  
  f_renderSizePercent.setValue(0,0);
  f_renderSizePixels.setValue(0,0);
  f_renderOriginPercent.setValue(0,0);
  f_renderOriginPixels.setValue(0,0);
}

SoViewportRegion::~SoViewportRegion(){}

void SoViewportRegion::reset() {
  originUnit.setValue(PERCENT);
  sizeUnit.setValue(PERCENT);
  origin.setValue(SbVec2f(0,0));
  size.setValue(SbVec2f(1,1));
  horizontalBorder.setValue(1);
  verticalBorder.setValue(1);
  background.setValue(TRUE);
  backgroundColor.setValue(SbColor(1,1,1));
  borderColor.setValue(SbColor(0,0,0));
  highlighted.setValue(FALSE);
  highlightColor.setValue(SbColor(1,0,0));
  horizontalHighlightBorder.setValue(2);
  verticalHighlightBorder.setValue(2);
  horizontalShadow.setValue(0);
  verticalShadow.setValue(0);
  sensitiveBorder.setValue(TRUE);  
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

void SoViewportRegion::doAction(SoAction* aAction){
  SO_ALTERNATEREP_DO_ACTION(aAction)

  //printf("debug : SoViewportRegion::doAction : %ld\n",this);

  SbViewportRegion vpr = SoViewportRegionElement::get(aAction->getState());

  const SbVec2s& win = vpr.getWindowSize();

  short w = 0;
  short h = 0;
  const SbVec2f& siz = size.getValue();
  if(sizeUnit.getValue()==PERCENT) {
    w = (short)SbFROUND(siz[0]*win[0]);
    h = (short)SbFROUND(siz[1]*win[1]);
    // Update output fields :
    if( (w!=f_renderSizePixels[0]) || 
        (h!=f_renderSizePixels[1]) ) {
      f_renderSizePixels.setValue(w,h);
    }
    if( (siz[0]!=f_renderSizePercent[0]) || 
        (siz[1]!=f_renderSizePercent[1]) ) {
      f_renderSizePercent.setValue(siz[0],siz[1]);
    }
  } else { //PIXELS
    w = (short)SbFROUND(siz[0]);
    h = (short)SbFROUND(siz[1]);
    // Update output fields :
    if( (w!=f_renderSizePixels[0]) || 
        (h!=f_renderSizePixels[1]) ) {
      f_renderSizePixels.setValue(w,h);
    }
    float pw = win[0] ? ((float)w)/((float)win[0]) : 0;
    float ph = win[1] ? ((float)h)/((float)win[1]) : 0;
    if( (pw!=f_renderSizePercent[0]) || 
        (ph!=f_renderSizePercent[1]) ) {
      f_renderSizePercent.setValue(pw,ph);
    }
  }


  short x = 0;
  short y = 0;
  const SbVec2f& ori = origin.getValue();
  if(originUnit.getValue()==PERCENT) {
    x = (short)SbFROUND(ori[0]*win[0]);
    y = (short)SbFROUND(ori[1]*win[1]);
    // Update output fields :
    if( (x!=f_renderOriginPixels[0]) || 
        (y!=f_renderOriginPixels[1]) ) {
      f_renderOriginPixels.setValue(x,y);
    }
    if( (ori[0]!=f_renderOriginPercent[0]) || 
        (ori[1]!=f_renderOriginPercent[1]) ) {
      f_renderOriginPercent.setValue(ori[0],ori[1]);
    }
  } else { //PIXELS
    x = (short)SbFROUND(ori[0]);
    y = (short)SbFROUND(ori[1]);
    if( (x!=f_renderOriginPixels[0]) || 
        (y!=f_renderOriginPixels[1]) ) {
      f_renderOriginPixels.setValue(x,y);
    }
    float px = win[0] ? ((float)x)/((float)win[0]) : 0;
    float py = win[1] ? ((float)y)/((float)win[1]) : 0;
    if( (px!=f_renderOriginPercent[0]) || 
        (py!=f_renderOriginPercent[1]) ) {
      f_renderOriginPercent.setValue(px,py);
    }
  }

  // Take into account border :
  SbVec2s b = getBorderSize();
  unsigned short bx = b[0];
  unsigned short by = b[1];

  x += horizontalOriginOffset.getValue();
  y += verticalOriginOffset.getValue();

  x = x + bx;
  y = y + by;
  w = w - 2 * bx;
  h = h - 2 * by;

  if(w<=0) w = 0;
  if(h<=0) h = 0;

  vpr.setViewportPixels(x,y,w,h);

  SoViewportRegionElement::set(aAction->getState(),vpr);
}

void SoViewportRegion::GLRender(SoGLRenderAction* aAction){
  SoViewportRegion::doAction(aAction);

  const SbViewportRegion& vpr = SoViewportRegionElement::get(aAction->getState());

  short w,h;
  vpr.getViewportSizePixels().getValue(w,h);

  if( (w<=0) || (h<=0) ) return;

  short x,y;
  vpr.getViewportOriginPixels().getValue(x,y);

  const SbVec2s& win = vpr.getWindowSize();

  SbBool vis = visible.getValue();

  SbVec2s b = getBorderSize();
  unsigned short bx = b[0];
  unsigned short by = b[1];

  float bred,bgreen,bblue;
  if((highlighted.getValue()==TRUE)&&(showHighlight.getValue()==TRUE)) {
    highlightColor.getValue().getValue(bred,bgreen,bblue);
  } else {
    borderColor.getValue().getValue(bred,bgreen,bblue);
  }
  float red,green,blue;
  backgroundColor.getValue().getValue(red,green,blue);

  int shx = horizontalShadow.getValue();
  int shy = verticalShadow.getValue();

  if( aAction->isOfType(SoGL2PSAction::getClassTypeId())  
      && (((SoGL2PSAction*)aAction)->fileWritingEnabled()==TRUE) ) {

    SoGL2PSAction* gl2psAction = (SoGL2PSAction*)aAction;
    if(vis==FALSE) { 
      // Can't bypass everything, since the gl2psPrintPostScriptEndViewport
      // flushes primitive with a gl2psPrintPrimitives.
      int vx = x;
      int vy = y;
      int vw = w;
      int vh = h;
      gl2psAction->endViewport();
      glViewport(vx,vy,vw,vh);
      gl2psAction->beginViewport(FALSE);
      return;
    }

    if(background.getValue()==TRUE) {

      if( (shx!=0) || (shy!=0) ) {
        int vx = x + shx;
        int vy = y + shy;
        int vw = w;
        int vh = h;
        gl2psAction->endViewport();
        glViewport(vx,vy,vw,vh);
        if(withinWindow(win[0],win[1],vx,vy,vw,vh)==TRUE) {
          glScissor(vx,vy,vw,vh);
          glClearColor(0,0,0,0);
        } 
        gl2psAction->beginViewport();
      }      
      if( (bx>0) || (by>0) ) {
        int vx = x - bx;
        int vy = y - by;
        int vw = w + 2 * bx;
        int vh = h + 2 * by;
        gl2psAction->endViewport();
        glViewport(vx,vy,vw,vh);
        if(withinWindow(win[0],win[1],vx,vy,vw,vh)==TRUE) {
          glScissor(vx,vy,vw,vh);
          glClearColor(bred,bgreen,bblue,0);
        }
        gl2psAction->beginViewport();

      }

      int vx = x;
      int vy = y;
      int vw = w;
      int vh = h;
      gl2psAction->endViewport();
      glViewport(vx,vy,vw,vh);
      if(withinWindow(win[0],win[1],vx,vy,vw,vh)==TRUE) {
        glScissor(vx,vy,vw,vh);
        glClearColor(red,green,blue,0);
      }
      gl2psAction->beginViewport();
    
    } else {

      if( (bx>0) || (by>0) ) {
        int vx = x - bx;
        int vy = y - by;
        int vw = w + 2 * bx;
        int vh = h + 2 * by;
        if(withinWindow(win[0],win[1],vx,vy,vw,vh)==TRUE) {
          gl2psAction->endViewport();
          glViewport(vx,vy,vw,by);
          glScissor(vx,vy,vw,by);
          glClearColor(bred,bgreen,bblue,0);
          gl2psAction->beginViewport();

          gl2psAction->endViewport();
          glViewport(vx,vy+h,vw,by);
          glScissor(vx,vy+h,vw,by);
          glClearColor(bred,bgreen,bblue,0);
          gl2psAction->beginViewport();

          gl2psAction->endViewport();
          glViewport(vx,vy,bx,vh);
          glScissor(vx,vy,bx,vh);
          glClearColor(bred,bgreen,bblue,0);
          gl2psAction->beginViewport();

          gl2psAction->endViewport();
          glViewport(vx+w,vy,bx,vh);
          glScissor(vx+w,vy,bx,vh);
          glClearColor(bred,bgreen,bblue,0);
          gl2psAction->beginViewport();
        } 
      }

      int vx = x;
      int vy = y;
      int vw = w;
      int vh = h;
      gl2psAction->endViewport();
      glViewport(vx,vy,vw,vh);
      if(withinWindow(win[0],win[1],vx,vy,vw,vh)==TRUE) {
        glScissor(vx,vy,vw,vh);
      } 
      gl2psAction->beginViewport(FALSE);

    }

  } else {

    if(vis==FALSE) return; //Bypass all.

    if(background.getValue()==TRUE) {
 
      if( (shx!=0) || (shy!=0) ) {
        int vx = x + shx;
        int vy = y + shy;
        int vw = w;
        int vh = h;
        glViewport(vx,vy,vw,vh);
        if(withinWindow(win[0],win[1],vx,vy,vw,vh)==TRUE) {
          GLboolean oldScissorTest = glIsEnabled(GL_SCISSOR_TEST);
          glEnable(GL_SCISSOR_TEST);
          glScissor(vx,vy,vw,vh);
          glClearColor(0,0,0,0);
          glClear(GL_COLOR_BUFFER_BIT);
          glClear(GL_DEPTH_BUFFER_BIT); //Needed with TGS.
          if(oldScissorTest==GL_FALSE) glDisable(GL_SCISSOR_TEST);
        } 
      }      
      if( (bx>0) || (by>0) ) {
        int vx = x - bx;
        int vy = y - by;
        int vw = w + 2 * bx;
        int vh = h + 2 * by;
        glViewport(vx,vy,vw,vh);
        if(withinWindow(win[0],win[1],vx,vy,vw,vh)==TRUE) {
          GLboolean oldScissorTest = glIsEnabled(GL_SCISSOR_TEST);
          glEnable(GL_SCISSOR_TEST);
          glScissor(vx,vy,vw,vh);
          glClearColor(bred,bgreen,bblue,0);
          glClear(GL_COLOR_BUFFER_BIT);
          glClear(GL_DEPTH_BUFFER_BIT); //Needed with TGS.
          if(oldScissorTest==GL_FALSE) glDisable(GL_SCISSOR_TEST);
        } 
      }      
      int vx = x;
      int vy = y;
      int vw = w;
      int vh = h;
      glViewport(vx,vy,vw,vh);
      if(withinWindow(win[0],win[1],vx,vy,vw,vh)==TRUE) {
        GLboolean oldScissorTest = glIsEnabled(GL_SCISSOR_TEST);
        glEnable(GL_SCISSOR_TEST);
        glScissor(vx,vy,vw,vh);
        glClearColor(red,green,blue,0);
        glClear(GL_COLOR_BUFFER_BIT);
        glClear(GL_DEPTH_BUFFER_BIT); //Needed with TGS.
        if(oldScissorTest==GL_FALSE) glDisable(GL_SCISSOR_TEST);
      } 

    } else {

      if( (bx>0) || (by>0) ) {
        int vx = x - bx;
        int vy = y - by;
        int vw = w + 2 * bx;
        int vh = h + 2 * by;
        glViewport(vx,vy,vw,vh);
        if(withinWindow(win[0],win[1],vx,vy,vw,vh)==TRUE) {
          GLboolean oldScissorTest = glIsEnabled(GL_SCISSOR_TEST);
          glEnable(GL_SCISSOR_TEST);
          glScissor(vx,vy,vw,by);
          glClearColor(bred,bgreen,bblue,0);
          glClear(GL_COLOR_BUFFER_BIT);
          glClear(GL_DEPTH_BUFFER_BIT); //Needed with TGS.

          glScissor(vx,vy+h,vw,by);
          glClearColor(bred,bgreen,bblue,0);
          glClear(GL_COLOR_BUFFER_BIT);
          glClear(GL_DEPTH_BUFFER_BIT); //Needed with TGS.

          glScissor(vx,vy,bx,vh);
          glClearColor(bred,bgreen,bblue,0);
          glClear(GL_COLOR_BUFFER_BIT);
          glClear(GL_DEPTH_BUFFER_BIT); //Needed with TGS.

          glScissor(vx+w,vy,bx,vh);
          glClearColor(bred,bgreen,bblue,0);
          glClear(GL_COLOR_BUFFER_BIT);
          glClear(GL_DEPTH_BUFFER_BIT); //Needed with TGS.

          if(oldScissorTest==GL_FALSE) glDisable(GL_SCISSOR_TEST);
        } 
      }
      int vx = x;
      int vy = y;
      int vw = w;
      int vh = h;
      glViewport(vx,vy,vw,vh);
      if(withinWindow(win[0],win[1],vx,vy,vw,vh)==TRUE) {
        GLboolean oldScissorTest = glIsEnabled(GL_SCISSOR_TEST);
        glEnable(GL_SCISSOR_TEST);
        glScissor(vx,vy,vw,vh);
        if(oldScissorTest==GL_FALSE) glDisable(GL_SCISSOR_TEST);
      } 

    }

  }
}

#define ACTION(a__method,a__action) \
void SoViewportRegion::a__method(a__action* aAction){\
  SoViewportRegion::doAction(aAction);\
  SbViewportRegion vpr = SoViewportRegionElement::get(aAction->getState());\
  short x,y,w,h;\
  vpr.getViewportOriginPixels().getValue(x,y);\
  vpr.getViewportSizePixels().getValue(w,h);\
  /* null size induces problem.*/\
  if(w<=0) w = 1;\
  if(h<=0) h = 1;\
  vpr.setViewportPixels((short)x,(short)y,(short)w,(short)h);\
  SoViewportRegionElement::set(aAction->getState(),vpr);\
}

ACTION(rayPick,SoRayPickAction)
ACTION(pick,SoPickAction)

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

void SoViewportRegion::setPositionPixels(int aX,int aY){
  originUnit.setValue(PIXELS);
  origin.setValue(SbVec2f((float)aX,(float)aY));
}

void SoViewportRegion::setSizePixels(unsigned int aWidth,unsigned int aHeight){
  sizeUnit.setValue(PIXELS);
  size.setValue(SbVec2f((float)aWidth,(float)aHeight));
}

void SoViewportRegion::setPositionPercent(float aX,float aY){
  originUnit.setValue(PERCENT);
  origin.setValue(SbVec2f(aX,aY));
}

void SoViewportRegion::setSizePercent(float aWidth,float aHeight){
  sizeUnit.setValue(PERCENT);
  size.setValue(SbVec2f(aWidth,aHeight));
}

void SoViewportRegion::getPositionPercent(float& aX,float& aY) const {
  if(originUnit.getValue()==PIXELS) {
    //WARNING : may be null if never rendered.
    aX = f_renderOriginPercent[0];
    aY = f_renderOriginPercent[1];
  } else { //PERCENT
    aX = origin.getValue()[0];
    aY = origin.getValue()[1];
  }
}

void SoViewportRegion::getSizePercent(float& aWidth,float& aHeight) const {
  if(sizeUnit.getValue()==PIXELS) {
    //WARNING : may be null if never rendered.
    aWidth = f_renderSizePercent[0];
    aHeight = f_renderSizePercent[1];
  } else { //PERCENT
    aWidth = size.getValue()[0];
    aHeight = size.getValue()[1];
  }
}

void SoViewportRegion::getPositionPixels(int& aX,int& aY) const {
  if(originUnit.getValue()==PIXELS) {
    aX = (int)SbFROUND(origin.getValue()[0]);
    aY = (int)SbFROUND(origin.getValue()[1]);
  } else { //PERCENT
    //WARNING : may be null if never rendered.
    aX = f_renderOriginPixels[0];
    aY = f_renderOriginPixels[1];
  }
}

void SoViewportRegion::getSizePixels(unsigned int& aWidth,
                                     unsigned int& aHeight) const {
  if(sizeUnit.getValue()==PIXELS) {
    aWidth = (unsigned int)SbFROUND(size.getValue()[0]);
    aHeight = (unsigned int)SbFROUND(size.getValue()[1]);
  } else { //PERCENT
    //WARNING : may be null if never rendered.
    aWidth = f_renderSizePixels[0];
    aHeight = f_renderSizePixels[1];
  }
}

void SoViewportRegion::setOriginUnit(Unit aUnit) {
  if(aUnit==originUnit.getValue()) return;
  if(aUnit==PIXELS) {
    setPositionPixels(f_renderOriginPixels[0],
                      f_renderOriginPixels[1]);
  } else {
    setPositionPercent(f_renderOriginPercent[0],
                       f_renderOriginPercent[1]);
  }
}

void SoViewportRegion::setSizeUnit(Unit aUnit) {
  if(aUnit==sizeUnit.getValue()) return;
  if(aUnit==PIXELS) {
    setSizePixels(f_renderSizePixels[0],
                  f_renderSizePixels[1]);
  } else {
    setSizePercent(f_renderSizePercent[0],
                   f_renderSizePercent[1]);
  }
}

SbVec2s SoViewportRegion::getBorderSize() const {
  if((highlighted.getValue()==TRUE)&&(showHighlight.getValue()==TRUE)) {
    return SbVec2s(horizontalHighlightBorder.getValue(),
                   verticalHighlightBorder.getValue());
  } else {
    return SbVec2s(horizontalBorder.getValue(),verticalBorder.getValue());
  }
}

SbBool SoViewportRegion::isInside(int aX,int aY) const {
  int x,y;
  unsigned int w,h;
  getPositionPixels(x,y);
  getSizePixels(w,h);  
  if( (aX>=x) && (aX<int(x+w)) && (aY>=y) && (aY<int(y+h)) ) return TRUE;
  return FALSE;
}

SbBool SoViewportRegion::isResizing() const {
  if(fIsDragging==FALSE) return FALSE;
  return fDragCorner?TRUE:FALSE;
}

SbBool SoViewportRegion::isMoving() const {
  if(fIsDragging==FALSE) return FALSE;
  return fDragCorner==0?TRUE:FALSE;
}

SbBool SoViewportRegion::dispatchEvent(const SoEvent* aEvent) {
  //if(isInside(pos[0],pos[1])==FALSE) return FALSE;

  if(aEvent->isOfType(SoMouseButtonEvent::getClassTypeId())) {
    if(SO_MOUSE_PRESS_EVENT((SoMouseButtonEvent*)aEvent,BUTTON1)) {
      fIsDragging = FALSE;
      const SbVec2s& pos = aEvent->getPosition();
      // Find hit region. The region is hit if a pick
      // hapened along its border.
      int pickSize = 5; //pixels
      int px,py;
      getPositionPixels(px,py);
      unsigned int pw,ph;
      getSizePixels(pw,ph);
      int corner = inCorner(pos[0],pos[1],px,py,pw,ph,pickSize,pickSize);
      if(corner!=0) {
        fIsDragging = TRUE;
        fDragPos = pos;
        fDragCorner = corner;
        fDragOriginUnit = (Unit)originUnit.getValue();
        fDragSizeUnit = (Unit)sizeUnit.getValue();
        fDragOldOrigin = origin.getValue();
        fDragOldSize = size.getValue();
        return TRUE;
      } else if(inBorder(pos[0],pos[1],px,py,pw,ph,pickSize,pickSize)==TRUE) {
        fIsDragging = TRUE;
        fDragPos = pos;
        fDragCorner = 0;
        fDragOriginUnit = (Unit)originUnit.getValue();
        fDragSizeUnit = (Unit)sizeUnit.getValue();
        return TRUE;
      }

    } else if(SO_MOUSE_RELEASE_EVENT((SoMouseButtonEvent*)aEvent,BUTTON1)) {
      if(fIsDragging) {
        fIsDragging = FALSE;
        setOriginUnit(fDragOriginUnit);
        if(fDragCorner!=0) { //It was a resize.
          // Check if zero size area :
          unsigned int pw,ph;
          getSizePixels(pw,ph);
          if((pw==0)||(ph==0)) {
            origin.setValue(fDragOldOrigin);
            originUnit.setValue(fDragOriginUnit);
            size.setValue(fDragOldSize);
            sizeUnit.setValue(fDragSizeUnit);
            return TRUE;
          }
        }
        setSizeUnit(fDragSizeUnit);
        return TRUE;
      }
    }

  } else if(fIsDragging) {

    const SbVec2s& pos = aEvent->getPosition();
    short dx = pos[0] - fDragPos[0];
    short dy = pos[1] - fDragPos[1];

    int px,py;
    getPositionPixels(px,py);

    if(fDragCorner==0) { //Move the region :

      px += dx;
      py += dy;
      setPositionPixels(px,py);

    } else { //Resize the region :

      unsigned int rpw,rph;
      getSizePixels(rpw,rph);
      int pw = rpw;
      int ph = rph;

      if(fDragCorner==1) {        // bottom left
        px += dx;
        py += dy;
        setPositionPixels(px,py);
        pw -= dx;
        ph -= dy;
        if((pw>=0)&&(ph>=0)) setSizePixels(pw,ph);
      } else if(fDragCorner==2) { // top left
        px += dx;
        setPositionPixels(px,py);
        pw -= dx;
        ph += dy;
        if((pw>=0)&&(ph>=0)) setSizePixels(pw,ph);
      } else if(fDragCorner==3) { // bottom right
        py += dy;
        setPositionPixels(px,py);
        pw += dx;
        ph -= dy;
        if((pw>=0)&&(ph>=0)) setSizePixels(pw,ph);
      } else if(fDragCorner==4) { // top right
        pw += dx;
        ph += dy;
        if((pw>=0)&&(ph>=0)) setSizePixels(pw,ph);
      }

    }

    fDragPos = pos;

    return TRUE;

  } else if(sensitiveBorder.getValue()==TRUE) {

    // Showing logic :
    short bx = horizontalBorder.getValue();
    short by = verticalBorder.getValue();
    //If a border is here, no need to show.
    if((visible.getValue()==TRUE) && (fShow==FALSE) && bx && by) return FALSE;

    const SbVec2s& pos = aEvent->getPosition();
    int pickSize = 5; //pixels
    int px,py;
    getPositionPixels(px,py);
    unsigned int pw,ph;
    getSizePixels(pw,ph);
    int corner = inCorner(pos[0],pos[1],px,py,pw,ph,pickSize,pickSize);
    if(corner!=0) {
      if(fShow==TRUE) return TRUE; //Already showing.

      fShow = TRUE;            

      fShowOldVisible = visible.getValue();
      visible.setValue(TRUE);

      fShowOldHighlightBorder = SbVec2s(horizontalHighlightBorder.getValue(),
                                        verticalHighlightBorder.getValue());
      horizontalHighlightBorder.setValue(2);
      verticalHighlightBorder.setValue(2);

      fShowOldBorder = SbVec2s(horizontalBorder.getValue(),
                               verticalBorder.getValue());
      horizontalBorder.setValue(2);
      verticalBorder.setValue(2);

      return TRUE;      
    } else if(inBorder(pos[0],pos[1],px,py,pw,ph,pickSize,pickSize)==TRUE) {
      if(fShow==TRUE) return TRUE; //Already showing.

      fShow = TRUE;

      fShowOldVisible = visible.getValue();
      visible.setValue(TRUE);

      fShowOldHighlightBorder = SbVec2s(horizontalHighlightBorder.getValue(),
                                        verticalHighlightBorder.getValue());
      horizontalHighlightBorder.setValue(2);
      verticalHighlightBorder.setValue(2);

      fShowOldBorder = SbVec2s(horizontalBorder.getValue(),
                               verticalBorder.getValue());
      horizontalBorder.setValue(2);
      verticalBorder.setValue(2);

      return TRUE;
    } else if(fShow==TRUE) {

      fShow = FALSE;

      visible.setValue(fShowOldVisible);

      horizontalHighlightBorder.setValue(fShowOldHighlightBorder[0]);
      verticalHighlightBorder.setValue(fShowOldHighlightBorder[1]);

      horizontalBorder.setValue(fShowOldBorder[0]);
      verticalBorder.setValue(fShowOldBorder[1]);

      return TRUE;
    }
  }

  return FALSE;
}

//void SoViewportRegion::enforceWinSize(const SbVec2s& aSize) {
//  fEnforcedWinSize = aSize;
//}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

void SoViewportRegion::generateAlternateRep(SoAction*) {
  //FIXME : have an alternate rep.
  alternateRep.setValue(new SoSeparator());
}

void SoViewportRegion::clearAlternateRep() {
  alternateRep.setValue(NULL);
}

void SoViewportRegion::write(SoWriteAction* aAction) {
  SoNode* altRep = alternateRep.getValue();
  if(altRep) { //NOTE : To avoid writing HEPVis SoSFEnum.
    altRep->write(aAction);
  } else {
    SoNode::write(aAction);
  }
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

static SbBool withinWindow(int aWinW,int aWinH,
                           int& aX,int& aY,
                           int& aW,int& aH){  
  //    |    |
  //     ---**
  //     0123
  //     --****
  //     012
  if(aWinW<=0) return FALSE;
  if(aWinH<=0) return FALSE;
  if(aX<0) {aW = aW + aX;aX = 0;}
  if(aY<0) {aH = aH + aY;aY = 0;}
  if(aX>=aWinW) return FALSE;
  if(aY>=aWinH) return FALSE;
  if((aX+aW)>aWinW) aW = aWinW - aX;
  if((aY+aH)>aWinH) aH = aWinH - aY;
  if(aW<=0) return FALSE;
  if(aH<=0) return FALSE;
  return TRUE;
}

static SbBool inBorder(int aX,int aY,
                       int aRX,int aRY,
                       int aRW,int aRH,
                       int aSizeX,int aSizeY) {
  if( ((aX>=aRX)&&(aX<=(aRX+aSizeX))) || 
      ((aX>=(aRX+aRW-aSizeX))&&(aX<=(aRX+aRW))) ) {
    if((aY>=aRY)&&(aY<(aRY+aRH))) return TRUE;
  } else if( ((aY>=aRY)&&(aY<=(aRY+aSizeY))) || 
             ((aY>=(aRY+aRH-aSizeY))&&(aY<=(aRY+aRH))) ) {
    if((aX>=aRX)&&(aX<(aRX+aRW))) return TRUE;
  }
  return FALSE;
}

static int inCorner(int aX,int aY,
                    int aRX,int aRY,
                    int aRW,int aRH,
                    int aSizeX,int aSizeY) {
  // 0 : not a corner
  // 1 : bottom left corner
  // 2 : top left corner
  // 3 : bottom right corner
  // 4 : top right corner

  if((aX>=aRX)&&(aX<=(aRX+aSizeX))) {
    if((aY>=aRY)&&(aY<=(aRY+aSizeY))) return 1;
    if((aY>=(aRY+aRH-aSizeY))&&(aY<=(aRY+aRH))) return 2;
  }
  if((aX>=(aRX+aRW-aSizeX))&&(aX<=(aRX+aRW))) {
    if((aY>=aRY)&&(aY<=(aRY+aSizeY))) return 3;
    if((aY>=(aRY+aRH-aSizeY))&&(aY<=(aRY+aRH))) return 4;
  }
  return 0;
}
