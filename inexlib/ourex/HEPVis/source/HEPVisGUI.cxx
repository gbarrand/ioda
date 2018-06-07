
#include <HEPVis/GUI.h>

extern "C" {
  void HEPVisGUI_initClasses();
}

//////////////////////////////////////////////////////////////////////////////
void HEPVisGUI_initClasses(
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  if(SoBackPrimitive::getClassTypeId()!=SoType::badType()) return; //done.

  SoBackPrimitive::initClass();
  SoPrimitive::initClass();
  SoForm::initClass();
  SoPushButton::initClass();
  SoArrowButton::initClass();
  SoScrolledList::initClass();
  SoSelectionBox::initClass();
  SoList::initClass();
  SoText::initClass();
  SoCommand::initClass();
  SoFileSelectionBox::initClass();
  SoShell::initClass();
}

