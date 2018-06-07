// this :
#include <HEPVis/nodes/SoPolyhedron.h>

#include <Inventor/SbBox.h>
#include <Inventor/SoPrimitiveVertex.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/details/SoPointDetail.h>
#include <Inventor/elements/SoTextureCoordinateElement.h>
#include <Inventor/elements/SoGLCacheContextElement.h>
#include <Inventor/actions/SoAction.h>
#include <Inventor/actions/SoWriteAction.h>

#include <HEPVis/SbMath.h>
#include <HEPVis/SbPolyhedron.h>
#include <HEPVis/SbProjector.h>
#include <HEPVis/actions/SoAlternateRepAction.h>

#include "vdata"

SO_NODE_SOURCE(SoPolyhedron) 
//////////////////////////////////////////////////////////////////////////////
void SoPolyhedron::initClass(
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  SO_NODE_INIT_CLASS(SoPolyhedron,SoShape,"Shape");
}
//////////////////////////////////////////////////////////////////////////////
SoPolyhedron::SoPolyhedron(
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  SO_NODE_CONSTRUCTOR(SoPolyhedron);
  SO_NODE_ADD_FIELD(solid,(TRUE));
  SO_NODE_ADD_FIELD(reducedWireFrame,(TRUE));
  SO_NODE_ADD_FIELD(projection,(SbProjectionNone));
  SO_NODE_ADD_FIELD(polyhedron,(SbPolyhedron()));
  SO_NODE_ADD_FIELD(alternateRep,(NULL));
}
//////////////////////////////////////////////////////////////////////////////
SoPolyhedron::SoPolyhedron(
 const SbPolyhedron& aPolyhedron
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  SO_NODE_CONSTRUCTOR(SoPolyhedron);
  SO_NODE_ADD_FIELD(solid,(TRUE));
  SO_NODE_ADD_FIELD(reducedWireFrame,(TRUE));
  SO_NODE_ADD_FIELD(projection,(SbProjectionNone));
  SO_NODE_ADD_FIELD(polyhedron,(aPolyhedron));
  SO_NODE_ADD_FIELD(alternateRep,(NULL));
}
//////////////////////////////////////////////////////////////////////////////
SoPolyhedron::SoPolyhedron(
 const SbPolyhedron& aPolyhedron
,SbProjectionType aProjection
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  SO_NODE_CONSTRUCTOR(SoPolyhedron);
  SO_NODE_ADD_FIELD(solid,(TRUE));
  SO_NODE_ADD_FIELD(reducedWireFrame,(TRUE));
  SO_NODE_ADD_FIELD(projection,(aProjection));
  SO_NODE_ADD_FIELD(polyhedron,(aPolyhedron));
  SO_NODE_ADD_FIELD(alternateRep,(NULL));
}
//////////////////////////////////////////////////////////////////////////////
SoPolyhedron::SoPolyhedron(
 const SbPolyhedron* aPolyhedron
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  SO_NODE_CONSTRUCTOR(SoPolyhedron);
  SO_NODE_ADD_FIELD(solid,(TRUE));
  SO_NODE_ADD_FIELD(reducedWireFrame,(TRUE));
  SO_NODE_ADD_FIELD(projection,(SbProjectionNone));
  SO_NODE_ADD_FIELD(polyhedron,(*aPolyhedron));
  SO_NODE_ADD_FIELD(alternateRep,(NULL));
}
//////////////////////////////////////////////////////////////////////////////
SoPolyhedron::~SoPolyhedron(
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
}
//////////////////////////////////////////////////////////////////////////////
void SoPolyhedron::generatePrimitives(
 SoAction* aAction
) 
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  const SbPolyhedron& sbPolyhedron = polyhedron.getValue();
  if(sbPolyhedron.GetNoFacets()<=0) return; // Abnormal polyhedron.

  SoState* state = aAction->getState();

  if(state && state->isElementEnabled
                (SoGLCacheContextElement::getClassStackIndex())) {
    SoGLCacheContextElement::shouldAutoCache(state,
      SoGLCacheContextElement::DO_AUTO_CACHE);
    //#if((COIN_MAJOR_VERSION>=3)||((COIN_MAJOR_VERSION==2)&&(COIN_MINOR_VERSION>=5)))
    //SoGLCacheContextElement::incNumShapes(state);
    //#endif
  }

  SbBool useTexFunction =
    (SoTextureCoordinateElement::getType(state) == 
     SoTextureCoordinateElement::FUNCTION);
  const SoTextureCoordinateElement *tce = NULL;
  SbVec4f texCoord;
  if (useTexFunction) {
    tce = SoTextureCoordinateElement::getInstance(state);
  } else {
    texCoord[2] = 0.0;
    texCoord[3] = 1.0;
  }

  if(solid.getValue()==TRUE) {
    SoPrimitiveVertex pv;

    HVNormal3D unitNormal;
    HVPoint3D* pV = sbPolyhedron.GetPV();

    // Assume all facets are convex quadrilaterals :
    bool notLastFace;
    do {
      notLastFace = sbPolyhedron.GetNextUnitNormal(unitNormal);

      beginShape(aAction,POLYGON);
      bool notLastEdge;
      int edgeFlag = 1;
      do {
        // optimize to avoid copying SbVec3f.
        //notLastEdge = sbPolyhedron.GetNextVertex(vertex,edgeFlag);
        int index;
        notLastEdge = sbPolyhedron.GetNextVertexIndex(index,edgeFlag);
        HVPoint3D* vertex = pV+index; //WARNING : must not be modified !

        if(useTexFunction) {
          texCoord=tce->get(SbVec3f((*vertex)[0],(*vertex)[1],(*vertex)[2]),
                           SbVec3f(unitNormal[0],unitNormal[1],unitNormal[2]));
        } else {
          texCoord[0]=0;
          texCoord[1]=0;
        }
        pv.setPoint((*vertex)[0],(*vertex)[1],(*vertex)[2]);
        pv.setNormal(unitNormal[0],unitNormal[1],unitNormal[2]);
        pv.setTextureCoords(texCoord);
        shapeVertex(&pv);

      } while (notLastEdge);
      endShape();
    } while (notLastFace);
  } else {
    SoPrimitiveVertex pvb,pve;
    pve.setTextureCoords(texCoord);
    pvb.setTextureCoords(texCoord);

    SoPointDetail pointDetail;
    pve.setDetail(&pointDetail);
    pvb.setDetail(&pointDetail);

#ifdef __COIN__
    beginShape(aAction,SoShape::LINES);
#endif

    SbProjectionType prj = (SbProjectionType)projection.getValue();
    HEPVis::SbProjector_d projector(prj);

    HVNormal3D unitNormal;
    HVPoint3D* pV = sbPolyhedron.GetPV();

    bool notLastFace;
    do {
      notLastFace = sbPolyhedron.GetNextUnitNormal(unitNormal);

      if( (prj==SbProjectionRZ) || (prj==SbProjectionZR) ) {
        unitNormal.setValue(0,0,1);
      }

      // Treat edges :
      int edgeFlag = 1;
      int prevEdgeFlag = edgeFlag;
      bool notLastEdge;
      SbBool firstEdge = TRUE;
      do {
        //notLastEdge = sbPolyhedron.GetNextVertex(vertex,edgeFlag);
        int index;
        notLastEdge = sbPolyhedron.GetNextVertexIndex(index,edgeFlag);
        HVPoint3D* vertex = pV+index; //WARNING : must not be modified !

        if(reducedWireFrame.getValue()==FALSE) edgeFlag = 1;        
        if(firstEdge) {
          if(edgeFlag) {
            pvb.setNormal(unitNormal[0],unitNormal[1],unitNormal[2]);
            if(prj==SbProjectionNone) {
              pvb.setPoint((*vertex)[0],(*vertex)[1],(*vertex)[2]);
            } else {
              SbVec3d point(*vertex); //have a local copy.
              projector.project(1,&point);
              pvb.setPoint(point[0],point[1],point[2]);
            }
          } else {
          }
          firstEdge = FALSE;
          prevEdgeFlag = edgeFlag;
        } else {
          if(edgeFlag!=prevEdgeFlag) { 
            if(edgeFlag) { // Pass to a visible edge :
              pvb.setNormal(unitNormal[0],unitNormal[1],unitNormal[2]);
              if(prj==SbProjectionNone) {
                pvb.setPoint((*vertex)[0],(*vertex)[1],(*vertex)[2]);
              } else {
                SbVec3d point(*vertex); //have a local copy.
                projector.project(1,&point);
                pvb.setPoint(point[0],point[1],point[2]);
              }
            } else { // Pass to an invisible edge :
              pve.setNormal(unitNormal[0],unitNormal[1],unitNormal[2]);
              if(prj==SbProjectionNone) {
                pve.setPoint((*vertex)[0],(*vertex)[1],(*vertex)[2]);
              } else {
                SbVec3d point(*vertex); //have a local copy.
                projector.project(1,&point);
                pve.setPoint(point[0],point[1],point[2]);
              }
#ifdef __COIN__
              shapeVertex(&pvb);
              shapeVertex(&pve);
#else
              invokeLineSegmentCallbacks(aAction,&pvb,&pve);
#endif
            }
            prevEdgeFlag = edgeFlag;
          } else {
            if(edgeFlag) {
              pve.setNormal(unitNormal[0],unitNormal[1],unitNormal[2]);
              if(prj==SbProjectionNone) {
                pve.setPoint((*vertex)[0],(*vertex)[1],(*vertex)[2]);
              } else {
                SbVec3d point(*vertex); //have a local copy.
                projector.project(1,&point);
                pve.setPoint(point[0],point[1],point[2]);
              }
#ifdef __COIN__
              shapeVertex(&pvb);
              shapeVertex(&pve);
#else
              invokeLineSegmentCallbacks(aAction,&pvb,&pve);
#endif
              pvb = pve;
            } else {
            }
          }
        }
      } while (notLastEdge);
    } while (notLastFace);

#ifdef __COIN__
    endShape();
#endif
  }
}
//////////////////////////////////////////////////////////////////////////////
void SoPolyhedron::computeBBox(
 SoAction*
,SbBox3f& aBox
,SbVec3f& aCenter
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  const SbPolyhedron& sbPolyhedron = polyhedron.getValue();
  if(sbPolyhedron.GetNoFacets()<=0) {
    SbVec3f vmin(-1,-1,-1);
    SbVec3f vmax( 1, 1, 1);
    aBox.setBounds(vmin,vmax);
    aCenter.setValue(0,0,0);
  } else {
    HVNormal3D unitNormal;
    HVPoint3D* pV = sbPolyhedron.GetPV();

    SbBool wireFrame = (solid.getValue()==TRUE ? FALSE : TRUE);
    SbProjectionType prj = (SbProjectionType)projection.getValue();
    HEPVis::SbProjector_d projector(wireFrame?prj:SbProjectionNone);
    SbBool first = TRUE;
    float xmn = 0,ymn = 0,zmn = 0;
    float xmx = 0,ymx = 0,zmx = 0;
    float xct = 0,yct = 0,zct = 0;
    int count = 0;
    // Assume all facets are convex quadrilaterals :
    bool notLastFace;
    do {
      notLastFace = sbPolyhedron.GetNextUnitNormal(unitNormal);
      bool notLastEdge;
      do {
        int edgeFlag = 1;
        //notLastEdge = sbPolyhedron.GetNextVertex(vertex,edgeFlag);
        int index;
        notLastEdge = sbPolyhedron.GetNextVertexIndex(index,edgeFlag);
        HVPoint3D* vertex = 0;
        if(prj==SbProjectionNone) {
          vertex = pV+index; //WARNING : must not be modified !
        } else {
          vertex = new HVPoint3D(*(pV+index));
          projector.project(1,vertex);
        }
        if(first==TRUE) {
          xct = xmx = xmn = (*vertex)[0];
          yct = ymx = ymn = (*vertex)[1];
          zct = zmx = zmn = (*vertex)[2];
          count++;
          first = FALSE;
        } else {
          xmn = SbMinimum(xmn,(*vertex)[0]);
          ymn = SbMinimum(ymn,(*vertex)[1]);
          zmn = SbMinimum(zmn,(*vertex)[2]);
          //
          xmx = SbMaximum(xmx,(*vertex)[0]);
          ymx = SbMaximum(ymx,(*vertex)[1]);
          zmx = SbMaximum(zmx,(*vertex)[2]);
          //
          xct += (*vertex)[0];
          yct += (*vertex)[1];
          zct += (*vertex)[2];
          count++;
        }
        if(prj!=SbProjectionNone) delete vertex;
        //
      } while (notLastEdge);
    } while (notLastFace);
    SbVec3f vmin(xmn,ymn,zmn);
    SbVec3f vmax(xmx,ymx,zmx);
    aBox.setBounds(vmin,vmax);
    if(count==0)
      aCenter.setValue(0,0,0);
    else
      aCenter.setValue(xct/count,yct/count,zct/count);
  }

}

#include <Inventor/nodes/SoNormalBinding.h>
#include <Inventor/nodes/SoNormal.h>
#include <Inventor/nodes/SoCoordinate3.h>
#include <Inventor/nodes/SoIndexedFaceSet.h>
#include <Inventor/nodes/SoIndexedLineSet.h>

void SoPolyhedron::generateAlternateRep(SoAction*) {
  //NOTE : not yet tested with the std::vector usage version.

  const SbPolyhedron& sbPolyhedron = polyhedron.getValue();
  if(sbPolyhedron.GetNoFacets()<=0) { // Abnormal polyhedron.
    //have anyway something so that the below write() logic works.
    alternateRep.setValue(new SoSeparator);
    return;
  }

  if(solid.getValue()==TRUE) {

    SoSeparator* separator = new SoSeparator;

    SoNormalBinding* normalBinding = new SoNormalBinding;
    normalBinding->value = SoNormalBinding::PER_FACE;
    separator->addChild(normalBinding);

    SoCoordinate3* coordinate3 = new SoCoordinate3;
    separator->addChild(coordinate3);
    SoNormal* normal = new SoNormal;
    separator->addChild(normal);
    SoIndexedFaceSet* indexedFaceSet = new SoIndexedFaceSet;
    separator->addChild(indexedFaceSet);

    std::vector<SbVec3f> normals;

    std::vector<SbVec3f> points;
    std::vector<int32_t> coordIndex;

    int icoord = 0;
    int iindex = 0;

    HVNormal3D unitNormal;
    HVPoint3D vertex;

    // Assume all facets are convex quadrilaterals :
    bool notLastFace;
    do {
      notLastFace = sbPolyhedron.GetNextUnitNormal(unitNormal);

      // begin face POLYGON
      points.clear();
      int ipoint = 0;

      bool notLastEdge;
      int edgeFlag = 1;
      do {
        notLastEdge = sbPolyhedron.GetNextVertex(vertex,edgeFlag);
        points.push_back(SbVec3f(vertex[0],vertex[1],vertex[2]));
        coordIndex.push_back(icoord + ipoint);ipoint++;

      } while (notLastEdge);
      coordIndex.push_back(SO_END_FACE_INDEX);
      // end face.

      coordinate3->point.setValues(icoord,points.size(),
                                   inlib::vec_data(points));
      icoord += points.size();

      normals.push_back(SbVec3f(unitNormal[0],unitNormal[1],unitNormal[2]));

      indexedFaceSet->coordIndex.setValues
        (iindex,(ipoint+1),inlib::vec_data(coordIndex));
      iindex += ipoint+1;

    } while (notLastFace);

    normal->vector.setValues(0,normals.size(),inlib::vec_data(normals));

    alternateRep.setValue(separator);

  } else {

    SoSeparator* separator = new SoSeparator;

    std::vector<SbVec3f> points;
    std::vector<int32_t> coords;
    int ipoint = 0;

    SbVec3d pvb,pve;

    SbProjectionType prj = (SbProjectionType)projection.getValue();
    HEPVis::SbProjector_d projector(prj);

    HVNormal3D unitNormal;
    HVPoint3D vertex;

    bool notLastFace;
    do {
      notLastFace = sbPolyhedron.GetNextUnitNormal(unitNormal);

      //if( (prj==SbProjectionRZ) || (prj==SbProjectionZR) ) {
        //unitNormal.setValue(0,0,1);
      //}

      // Treat edges :
      int edgeFlag = 1;
      int prevEdgeFlag = edgeFlag;
      bool notLastEdge;
      SbBool firstEdge = TRUE;
      do {
        notLastEdge = sbPolyhedron.GetNextVertex(vertex,edgeFlag);
        if(reducedWireFrame.getValue()==FALSE) edgeFlag = 1;        
        if(firstEdge) {
          if(edgeFlag) {
            pvb.setValue(vertex[0],vertex[1],vertex[2]);
            projector.project(1,&pvb);
          } else {
          }
          firstEdge = FALSE;
          prevEdgeFlag = edgeFlag;
        } else {
          if(edgeFlag!=prevEdgeFlag) { 
            if(edgeFlag) { // Pass to a visible edge :
              pvb.setValue(vertex[0],vertex[1],vertex[2]);
              projector.project(1,&pvb);
            } else { // Pass to an invisible edge :
              pve.setValue(vertex[0],vertex[1],vertex[2]);
              projector.project(1,&pve);

              points.push_back(SbVec3f(pvb[0],pvb[1],pvb[2]));
              points.push_back(SbVec3f(pve[0],pve[1],pve[2]));

              coords.push_back(ipoint);ipoint++;
              coords.push_back(ipoint);ipoint++;
              coords.push_back(SO_END_LINE_INDEX);

            }
            prevEdgeFlag = edgeFlag;
          } else {
            if(edgeFlag) {
              pve.setValue(vertex[0],vertex[1],vertex[2]);
              projector.project(1,&pve);

              points.push_back(SbVec3f(pvb[0],pvb[1],pvb[2]));
              points.push_back(SbVec3f(pve[0],pve[1],pve[2]));

              coords.push_back(ipoint);ipoint++;
              coords.push_back(ipoint);ipoint++;
              coords.push_back(SO_END_LINE_INDEX);

              pvb = pve;
            } else {
            }
          }
        }
      } while (notLastEdge);
    } while (notLastFace);

    SoCoordinate3* coordinate3 = new SoCoordinate3;
    coordinate3->point.setValues(0,points.size(),inlib::vec_data(points));
    separator->addChild(coordinate3);

    SoIndexedLineSet* indexedLineSet = new SoIndexedLineSet;
    indexedLineSet->coordIndex.setValues(0,coords.size(),
                                         inlib::vec_data(coords));
    separator->addChild(indexedLineSet);

    alternateRep.setValue(separator);
  }
}
//////////////////////////////////////////////////////////////////////////////
void SoPolyhedron::clearAlternateRep(
) 
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  alternateRep.setValue(NULL);
}
//////////////////////////////////////////////////////////////////////////////
void SoPolyhedron::doAction(
 SoAction* aAction
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  SO_ALTERNATEREP_DO_ACTION(aAction)
  SoShape::doAction(aAction);
}
//////////////////////////////////////////////////////////////////////////////
void SoPolyhedron::write(
 SoWriteAction* aAction
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  SoNode* altRep = alternateRep.getValue();
  if(altRep) {
    //NOTE : the below logic loose the fact
    //       that we had a SoPolyhedron. 
    //       We may also introduce a coworking SoAltPolyhedron
    //       that has no polyhedron field, and arrange
    //       that the SoAlternateRepAction swaps 
    //       from SoPolyhedron to SoAltPolyhedron.
    altRep->write(aAction);
  } else {
    SbBool old = polyhedron.isDefault();
    polyhedron.setDefault(FALSE); //enforce writing.
    SoShape::write(aAction);
    polyhedron.setDefault(old);
  }
}
//////////////////////////////////////////////////////////////////////////////
void SoPolyhedron::callback(
 SoCallbackAction* aAction
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  SoNode* altRep = alternateRep.getValue();
  if(altRep) {
    //NOTE :
    // To handle SoToVRML2Action that internally uses a SoCallbackAction.
    // With Coin-2.5.0, the SoToVRML2Action does not handle the 
    // generatePrimitives beginShape(LINES). Then we can't use
    // the default SoShape::callback behaviour to produce VRML.
    // (See the SoToVRML2Action cstor that set only triangle callback
    //  on the internal SoCallbackAction)
    //FIXME : we should find a way to check that the action 
    //  is a SoToVRML2Action.
    altRep->callback(aAction);
  } else {
    SoShape::callback(aAction);
  }
}
