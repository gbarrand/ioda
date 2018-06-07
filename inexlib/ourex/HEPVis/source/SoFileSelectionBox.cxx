#include <math.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoTransform.h>
#include <Inventor/sensors/SoFieldSensor.h>

#include <HEPVis/SbFile.h>
#include <HEPVis/nodekits/SoScrolledList.h>

#include <HEPVis/nodekits/SoFileSelectionBox.h>

SO_KIT_SOURCE(SoFileSelectionBox) 
//////////////////////////////////////////////////////////////////////////////
void SoFileSelectionBox::initClass(
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  SO_KIT_INIT_CLASS(SoFileSelectionBox,SoForm,"SoForm");
}
//////////////////////////////////////////////////////////////////////////////
SoFileSelectionBox::SoFileSelectionBox(
)
:fScrolledList(NULL)
,fSensor(NULL)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  SO_KIT_CONSTRUCTOR(SoFileSelectionBox);
  SO_KIT_INIT_INSTANCE();
  //
  SO_NODE_ADD_FIELD(directory,("."));
  //
  initialize();
}
//////////////////////////////////////////////////////////////////////////////
SoFileSelectionBox::~SoFileSelectionBox(
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  fOkCallbackList.clearCallbacks();
  if(fSensor!=NULL) {
    fSensor->detach();
    delete fSensor;
  }
}
//////////////////////////////////////////////////////////////////////////////
void SoFileSelectionBox::initialize(
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  fScrolledList = new SoScrolledList;
  if(fScrolledList!=NULL) {
    fScrolledList->
      addSingleSelectionCallback(SoFileSelectionBox::singleSelectionCB,this);
    addChild(fScrolledList);
  }
  //
  fSensor = new SoFieldSensor(SoFileSelectionBox::sensorCB,this);
  fSensor->attach(&directory);
  //
  setDirectory();
  //
  SoFileSelectionBox::updateSceneGraph();
}
//////////////////////////////////////////////////////////////////////////////
void SoFileSelectionBox::addOkCallback(
 SoCB* aFunction
,void* aUserData
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  fOkCallbackList.
    addCallback((SoCallbackListCB*)aFunction,aUserData);
}
//////////////////////////////////////////////////////////////////////////////
void SoFileSelectionBox::removeOkCallback(
 SoCB* aFunction
,void* aUserData
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  fOkCallbackList.
    removeCallback((SoCallbackListCB*)aFunction,aUserData);
}
//////////////////////////////////////////////////////////////////////////////
void SoFileSelectionBox::invokeOkCallbacks(
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  fOkCallbackList.invokeCallbacks(this);
}
//////////////////////////////////////////////////////////////////////////////
const SbString& SoFileSelectionBox::getSelectedFile(
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  return fSelectedFile;
}
//////////////////////////////////////////////////////////////////////////////
void SoFileSelectionBox::updateSceneGraph(
) 
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  SoForm::updateSceneGraph();
}
//////////////////////////////////////////////////////////////////////////////
void SoFileSelectionBox::singleSelectionCB (
 void* aUserData
 ,SoNode* /*aNode*/
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  SoFileSelectionBox* This = (SoFileSelectionBox*)aUserData;
  if(This==NULL) return;
  if(This->fScrolledList==NULL) return;
  This->fSelectedFile = This->fScrolledList->getSelectedItem();
  This->invokeOkCallbacks();
  This->fSelectedFile = "";
}
//////////////////////////////////////////////////////////////////////////////
void SoFileSelectionBox::sensorCB(
 void* aData
,SoSensor*
) 
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  SoFileSelectionBox* This = (SoFileSelectionBox*)aData;
  if(This==NULL) return;
  This->setDirectory();
}
//////////////////////////////////////////////////////////////////////////////
void SoFileSelectionBox::setDirectory(
) 
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  if(fScrolledList==NULL) return;
  //const char* path = SbFele::getWorkingDirectory();
  const char* path = directory.getValue().getString();
  //SbFile::dumpDirectory(path);
  SbStringList* files = SbFile::getDirectoryFiles(path);
  if(files!=NULL) {
    int number = files->getLength();
    if(number==0) {
      fScrolledList->items.setValue("");
    } else {
      SbString* ss = new SbString[number];
      for(int count=0;count<number;count++) {
        SbString* entry = (*files)[count]; 
        ss[count] = *entry; 
        delete entry;
      }
      fScrolledList->items.setValues(0,number,ss);
      delete [] ss;
    }
    delete files;
  }
}
