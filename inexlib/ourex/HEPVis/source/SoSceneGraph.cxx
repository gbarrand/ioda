/*-----------------------------HEPVis----------------------------------------*/
/*                                                                           */
/* Node:             SoSceneGraph                                            */
/* Description:      SoSeparator to handle OnX-like scene graph              */
/* Author:           Guy Barrand Dec 2008                                    */
/*                                                                           */
/*---------------------------------------------------------------------------*/

// this :
#include <HEPVis/nodes/SoSceneGraph.h>

SO_NODE_SOURCE(SoSceneGraph)

void SoSceneGraph::initClass(){
  SO_NODE_INIT_CLASS(SoSceneGraph,SoSeparator,"Separator");
}

SoSceneGraph::SoSceneGraph():fInfos(0) {
  SO_NODE_CONSTRUCTOR(SoSceneGraph);

  //NOTE : dangerous to have a SoField.
  //       As it is used to deposit encoded
  //       data pointer, it is dangerous if saved
  //       in a file and read back whilst the data are
  //       no more here.

  //SO_NODE_ADD_FIELD(string,(""));
}

#include <cstring>
#include <cstdlib>

inline char* Sb_STRDUP(const char* aString){
  char* p = (char*)::malloc((unsigned)::strlen(aString)+1);
  if(p==NULL) return 0;
  return ::strcpy(p,aString);
}

inline void Sb_STRDEL(char*& aString){
  if(aString) ::free(aString);
  aString = NULL;
}

SoSceneGraph::~SoSceneGraph() {
  Sb_STRDEL(fInfos);
}

void SoSceneGraph::setString(const char* aString) {
  fString = aString;
}
void SoSceneGraph::setString(const SbString& aString) {
  fString = aString;
}
const char* SoSceneGraph::getString() const {return fString.getString();}

void SoSceneGraph::setInfos(const char* aString) {
  fInfos = Sb_STRDUP(aString);
}
const char* SoSceneGraph::getInfos() const {return fInfos;}
