// this :
#include <HEPVis/nodekits/SoForm.h>

#include <Inventor/nodes/SoGroup.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoTranslation.h>
#include <Inventor/sensors/SoFieldSensor.h>

#include <math.h>

SO_KIT_SOURCE(SoForm) 
//////////////////////////////////////////////////////////////////////////////
void SoForm::initClass(
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  SO_KIT_INIT_CLASS(SoForm,SoPrimitive,"SoPrimitive");
}
//////////////////////////////////////////////////////////////////////////////
SoForm::SoForm(
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  SO_KIT_CONSTRUCTOR(SoForm);
  SO_KIT_INIT_INSTANCE();
  //
  initialize();
}
//////////////////////////////////////////////////////////////////////////////
SoForm::~SoForm(
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  SoGroup* group = (SoGroup*)frontGroup.getValue();
  if(group!=NULL) 
    group->removeAuditor(&requireUpdateSceneGraph,SoNotRec::FIELD);
  //
  int number = fSensors.getLength();
  for(int count=0;count<number;count++) {
    SoFieldSensor* sensor = (SoFieldSensor*)fSensors[count];
    sensor->detach();
    delete sensor;
  }
  fSensors.truncate(0);
}
//////////////////////////////////////////////////////////////////////////////
void SoForm::initialize(
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  SoGroup* group = (SoGroup*)frontGroup.getValue();
  if(group!=NULL) group->addAuditor(&requireUpdateSceneGraph,SoNotRec::FIELD);
  //
  SoForm::updateSceneGraph();
}
//////////////////////////////////////////////////////////////////////////////
void SoForm::updateSceneGraph(
) 
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  SoPrimitive::updateSceneGraph();
}
//////////////////////////////////////////////////////////////////////////////
void SoForm::addChild(
 SoPrimitive* aNode
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  SoGroup* group = (SoGroup*)frontGroup.getValue();
  if(group==NULL) return;
  SoSeparator* separator = new SoSeparator;
  if(separator==NULL) return;
  SoTranslation* translation = new SoTranslation;
  if(translation==NULL) return;
  separator->addChild(translation);
  separator->addChild(aNode);
  group->addChild(separator);
  SoFieldSensor* sensor;
  sensor = new SoFieldSensor(SoForm::sensorCB,this);
  if(sensor==NULL) return;
  sensor->attach(&(aNode->width));
  fSensors.append(sensor);
  sensor = new SoFieldSensor(SoForm::sensorCB,this);
  if(sensor==NULL) return;
  sensor->attach(&(aNode->height));
  fSensors.append(sensor);
}
//////////////////////////////////////////////////////////////////////////////
void SoForm::sensorCB (
 void* aData
,SoSensor* //aSensor
) 
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  // One of the child has changed its size :
  SoForm* This = (SoForm*)aData;
  SoGroup* group = (SoGroup*)(This->frontGroup.getValue());
  if(group==NULL) return;
  int number = group->getNumChildren();
  for(int count=0;count<number;count++) {
    SoSeparator* separator = (SoSeparator*)group->getChild(count);
    SoTranslation* translation = (SoTranslation*)separator->getChild(0);
    SoPrimitive* child = (SoPrimitive*)separator->getChild(1);
    translation->translation.setValue(getTranslation(child));
  }
  /*
  Attachment attachment;
  for(int count=0;count<number;count++) {
    SoSeparator* separator = (SoSeparator*)group->getChild(count);
    SoPrimitive* child = (SoPrimitive*)separator->getChild(1);
    attachment = (Attachment)child->rightAttachment.getValue();
    if(attachment==ATTACH_FORM) {
      //child->width.getValue();
      if(isNodeRightAttached(child)==FALSE) {
        child->compellSize();
      }
    }
  }
  */
}
//////////////////////////////////////////////////////////////////////////////
SbVec3f SoForm::getTranslation(
 SoPrimitive* aNode
) 
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  SbVec3f total(0.,0.,0.);
  SoPrimitive* node;
  SoPrimitive* current;
  Attachment attachment;
  // x value, by left :
  current = aNode;
  while(1) {
    attachment = (Attachment)current->leftAttachment.getValue();
    if(attachment==ATTACH_NODE) {
      node = (SoPrimitive*)(current->leftNode.getValue());
      if(node==NULL) break;
      total[0] += (node->width.getValue() + current->width.getValue())/2;
      current = node;
    } else if(attachment==ATTACH_OPPOSITE_NODE) {
      node = (SoPrimitive*)(current->leftNode.getValue());
      if(node==NULL) break;
      total[0] += (-node->width.getValue() + current->width.getValue())/2;
      current = node;
    } else {
      break;
    }
  }
  // y value, by top :
  current = aNode;
  while(1) {
    attachment = (Attachment)current->topAttachment.getValue();
    if(attachment==ATTACH_NODE) {
      node = (SoPrimitive*)(current->topNode.getValue());
      if(node==NULL) break;
      total[1] -= (node->height.getValue() + current->height.getValue())/2;
      current = node;
    } else if(attachment==ATTACH_OPPOSITE_NODE) {
      node = (SoPrimitive*)(current->topNode.getValue());
      if(node==NULL) break;
      total[1] += (-node->height.getValue() + current->height.getValue())/2;
      current = node;
    } else { //ATTACH_NODE
      break;
    }
  }
  // y value, by bottom :
  current = aNode;
  while(1) {
    attachment = (Attachment)current->bottomAttachment.getValue();
    if(attachment==ATTACH_NODE) {
      node = (SoPrimitive*)(current->bottomNode.getValue());
      if(node==NULL) break;
      total[1] += (node->height.getValue() + current->height.getValue())/2;
      current = node;
    } else if(attachment==ATTACH_OPPOSITE_NODE) {
      node = (SoPrimitive*)(current->bottomNode.getValue());
      if(node==NULL) break;
      total[1] -= (-node->height.getValue() + current->height.getValue())/2;
      current = node;
    } else { //ATTACH_NODE
      break;
    }
  }
  return total;
}
//////////////////////////////////////////////////////////////////////////////
SbBool SoForm::isNodeRightAttached(
 SoPrimitive* aNode
) 
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  SoGroup* group = (SoGroup*)frontGroup.getValue();
  if(group==NULL) return FALSE;
  SoPrimitive* node;
  Attachment attachment;
  int number = group->getNumChildren();
  for(int count=0;count<number;count++) {
    SoSeparator* separator = (SoSeparator*)group->getChild(count);
    SoPrimitive* child = (SoPrimitive*)separator->getChild(1);
    if(child==aNode) continue;
    attachment = (Attachment)child->leftAttachment.getValue();
    if(attachment==ATTACH_NODE) {
      node = (SoPrimitive*)(child->leftNode.getValue());
      if(node==aNode) return TRUE;
    }
  }
  return FALSE;
}





