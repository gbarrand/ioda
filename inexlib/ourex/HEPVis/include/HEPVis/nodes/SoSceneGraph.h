/*-----------------------------HEPVis----------------------------------------*/
/*                                                                           */
/* Node:             SoSceneGraph                                            */
/* Description:      SoSeparator to handle OnX-like scene graph              */
/* Author:           Guy Barrand Dec 2008                                    */
/*                                                                           */
/*---------------------------------------------------------------------------*/
#ifndef HEPVis_SoSceneGraph_h
#define HEPVis_SoSceneGraph_h

// Inheritance :
#include <Inventor/nodes/SoSeparator.h>

class SoSceneGraph : public SoSeparator {
  SO_NODE_HEADER(SoSceneGraph); 
public:
  static void initClass();
public:
  SoSceneGraph();
public:
  void setString(const char*);
  void setString(const SbString&);
  const char* getString() const;

  void setInfos(const char*);
  const char* getInfos() const;
protected:
  virtual ~SoSceneGraph();
private: 
  SbString fString; //And not SbName.
  char* fInfos;
};

#endif
